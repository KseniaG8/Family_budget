#include "UserService.h"
#include <botan/base32.h>
#include <botan/bcrypt.h>
#include <botan/otp.h>
#include <botan/system_rng.h>
<<<<<<< HEAD
=======
#include <botan/otp.h>
#include <botan/base32.h>
>>>>>>> 22e5bb2d01344dce828f490fe50a23d68012dcbf

UserService::UserService(Database &db) : database(db) {
}

std::string UserService::registerUser(
    const std::string &login,
    const std::string &password
) {
    if (login.empty()) {
        return "empty_login";
    }
    if (password.empty()) {
        return "empty_password";
    }
    if (login.find_first_not_of(' ') == std::string::npos) {
        return "empty_login";
    }
    if (password.find_first_not_of(' ') == std::string::npos) {
        return "empty_password";
    }
    if (login.length() < 2) {
        return "short_login";
    }
    if (password.length() < 6) {
        return "short_password";
    }

    User existing = database.getUserByLogin(login);
    if (existing.id != -1) {
        return "user_exists";
    }

    Botan::System_RNG rng;
    std::string hashedPassword = Botan::generate_bcrypt(password, rng, 12);

    if (!database.addUser(login, hashedPassword)) {
        return "db_error";
    }

    return "success";
}

<<<<<<< HEAD
bool UserService::loginUser(const std::string& login, const std::string& password) {
    User user = database.getUserByLogin(login);
    if (user.id == -1) return false;

    bool isPasswordValid = Botan::check_bcrypt(password, user.password);
    
    return isPasswordValid;
=======
nlohmann::json
UserService::loginUser(const std::string &login, const std::string &password) {
    User user = database.getUserByLogin(login);
    if (user.id == -1) {
        return {{"status", "error"}, {"message", "Неверные данные"}};
    }
    if (!Botan::check_bcrypt(password, user.password)) {
        return {{"status", "error"}, {"message", "Неверные данные"}};
    }

    return {
        {"status", "success"},
        {"is_2fa_enabled", user.is_2fa_enabled == 1},
        {"user_id", user.id},
        {"login", user.login}};
}

User UserService::getUserByLogin(const std::string &login) {
    return database.getUserByLogin(login);
}

nlohmann::json UserService::setup2FA(int user_id, const std::string &login) {
    Botan::System_RNG rng;
    std::vector<uint8_t> secret_bytes(16);
    rng.randomize(secret_bytes.data(), secret_bytes.size());

    std::string db_secret = Botan::base32_encode(secret_bytes);

    std::string display_secret = db_secret;
    display_secret.erase(
        std::remove(display_secret.begin(), display_secret.end(), '='),
        display_secret.end()
    );

    if (database.enable2FA(user_id, db_secret)) {
        std::string uri = "otpauth://totp/FamilyBudget:" + login +
                          "?secret=" + display_secret + "&issuer=FamilyBudget";
        return {
            {"status", "success"}, {"secret", display_secret}, {"uri", uri}};
    }

    return {{"status", "error"}, {"message", "Database error"}};
}

nlohmann::json
UserService::verifyLogin2FA(const std::string &login, const std::string &code) {
    User user = database.getUserByLogin(login);
    if (user.id == -1 || user.totp_secret.empty() || user.is_2fa_enabled == 0) {
        return {{"status", "error"}, {"message", "2FA не настроена"}};
    }

    try {
        Botan::secure_vector<uint8_t> secret_bytes =
            Botan::base32_decode(user.totp_secret);
        Botan::TOTP totp(secret_bytes.data(), secret_bytes.size());

        if (totp.verify_totp(std::stoul(code), std::time(nullptr), 1)) {
            return {
                {"status", "success"},
                {"user_id", user.id},
                {"login", user.login}};
        }
    } catch (...) {
    }
    return {{"status", "error"}, {"message", "Неверный код"}};
>>>>>>> 22e5bb2d01344dce828f490fe50a23d68012dcbf
}
