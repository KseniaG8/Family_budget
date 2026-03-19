#include "UserService.h"
#include <botan/bcrypt.h>
#include <botan/system_rng.h>

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

bool UserService::loginUser(const std::string& login, const std::string& password) {
    User user = database.getUserByLogin(login);
    if (user.id == -1) return false;

    bool isPasswordValid = Botan::check_bcrypt(password, user.password);
    
    return isPasswordValid;
}
