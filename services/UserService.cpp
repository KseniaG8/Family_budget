#include "UserService.h"
#include <botan/bcrypt.h>
#include <botan/system_rng.h>
#include <botan/totp.h>
#include <botan/base32.h>

UserService::UserService(Database& db) : database(db) {}

std::string UserService::registerUser(const std::string& login, const std::string& password) {
    if (login.empty()) return "empty_login";
    if (password.empty()) return "empty_password";
    if (login.find_first_not_of(' ') == std::string::npos) return "empty_login";
    if (password.find_first_not_of(' ') == std::string::npos) return "empty_password";
    if (login.length() < 2) return "short_login";
    if (password.length() < 6) return "short_password";
    
    User existing = database.getUserByLogin(login);
    if (existing.id != -1) return "user_exists";

    Botan::System_RNG rng;
    std::string hashedPassword = Botan::generate_bcrypt(password, rng, 12);

    if (!database.addUser(login, hashedPassword))
        return "db_error";

    return "success";
}

std::string UserService::loginUser(const std::string& login, const std::string& password) {
    User user = database.getUserByLogin(login);
    if (user.id == -1) return "invalid_credentials";

    bool isPasswordValid = Botan::check_bcrypt(password, user.password);
    
    if (!isPasswordValid) {
        return "invalid_credentials";
    }

    if (user.is_2fa_enabled == 1) {
        return "require_2fa"; 
    }

    return "success";
}

nlohmann::json UserService::setup2FA(int user_id, const std::string& login) {
    Botan::System_RNG rng;
    std::vector<uint8_t> secret_bytes(16);
    rng.randomize(secret_bytes.data(), secret_bytes.size());
    
    std::string secret = Botan::base32_encode(secret_bytes);
    
    if (database.enable2FA(user_id, secret)) {
        std::string uri = "otpauth://totp/FamilyBudget:" + login + 
                          "?secret=" + secret + "&issuer=FamilyBudget";
        return {{"status", "success"}, {"secret", secret}, {"uri", uri}};
    }
    
    return {{"status", "error"}, {"message", "Database error"}};
}

bool UserService::verifyLogin2FA(const std::string& login, const std::string& code) {
    User user = database.getUserByLogin(login);
    if (user.id == -1 || user.totp_secret.empty() || user.is_2fa_enabled == 0) return false;

    try {
        std::vector<uint8_t> secret_bytes = Botan::base32_decode(user.totp_secret);
        Botan::SymmetricKey key(secret_bytes);
        Botan::TOTP totp(key, "SHA-1", 6, 30);
        
        return totp.verify(std::stoul(code), std::time(nullptr), 1);
    } catch (...) {
        return false;
    }
}
