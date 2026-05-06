#pragma once
#include "../services/UserService.h"
#include <nlohmann/json.hpp>

class UserHandler {
public:
    UserHandler(UserService& service);

    nlohmann::json registerUser(const std::string& login, const std::string& password);
    nlohmann::json loginUser(const std::string& login, const std::string& password);
    nlohmann::json setup2FA(int user_id, const std::string& login);
    nlohmann::json verifyLogin2FA(const std::string& login, const std::string& code);

private:
    UserService& service;
};
