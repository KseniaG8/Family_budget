#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "../database/Database.h"
#include "../models/User.h"

class UserService {
public:
    UserService(Database &db);

    std::string
    registerUser(const std::string &login, const std::string &password);
    nlohmann::json
    loginUser(const std::string &login, const std::string &password);
    User getUserByLogin(const std::string &login);

    nlohmann::json setup2FA(int user_id, const std::string &login);
    nlohmann::json
    verifyLogin2FA(const std::string &login, const std::string &code);

private:
    Database &database;
};
