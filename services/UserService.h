#pragma once
#include "../database/Database.h"

class UserService {
public:
    UserService(Database& db);

    std::string registerUser(const std::string& login, const std::string& password);
    bool loginUser(const std::string& login, const std::string& password);

private:
    Database& database;
};