#include "UserService.h"

UserService::UserService(Database& db) : database(db) {}

std::string UserService::registerUser(const std::string& login, const std::string& password) {
    if (login.empty())
        return "empty_login";

    if (password.empty())
        return "empty_password";

    if (login.find_first_not_of(' ') == std::string::npos)
        return "empty_login";

    if (password.find_first_not_of(' ') == std::string::npos)
        return "empty_password";

    if (login.length() < 2)
        return "short_login";

    if (password.length() < 6)
        return "short_password";
    
    User existing = database.getUserByLogin(login);
    if (existing.id != -1) return "user_exists";

    if (!database.addUser(login, password))
        return "db_error";

    return "success";
}

bool UserService::loginUser(const std::string& login, const std::string& password) {
    User user = database.getUserByLogin(login);
    return user.password == password;
}
