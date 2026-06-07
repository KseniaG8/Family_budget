#pragma once
#include "../database/Database.h"
#include <string>
#include <nlohmann/json.hpp>

class UserService {
public:
    UserService(Database& db);

    std::string registerUser(const std::string& login, const std::string& password);
    nlohmann::json loginUser(const std::string& login, const std::string& password);
    nlohmann::json setup2FA(int user_id, const std::string& login);
    nlohmann::json verifyLogin2FA(const std::string& login, const std::string& code);

private:
    Database& database;
};
