#pragma once
#include "../database/Database.h"
#include <string>

class UserService {
public:
    UserService(Database& db);

    std::string registerUser(const std::string& login, const std::string& password);
    std::string loginUser(const std::string& login, const std::string& password);
    std::string generateTOTPSecret();
    bool verifyTOTPCode(const std::string& secret, const std::string& code);

private:
    Database& database;
};
