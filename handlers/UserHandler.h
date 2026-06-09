#pragma once
#include "../services/UserService.h"
#include <nlohmann/json.hpp>

class UserHandler {
public:
    UserHandler(UserService& service);

    nlohmann::json registerUser(const std::string& login, const std::string& password);
    nlohmann::json loginUser(const std::string& login, const std::string& password);

private:
    UserService& service;
};