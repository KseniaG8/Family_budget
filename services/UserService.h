#pragma once
<<<<<<< HEAD
#include "../database/Database.h"
=======
#include <nlohmann/json.hpp>
#include <string>
#include "../database/Database.h"
#include "../models/User.h"
>>>>>>> 22e5bb2d01344dce828f490fe50a23d68012dcbf

class UserService {
public:
    UserService(Database &db);

<<<<<<< HEAD
    std::string registerUser(const std::string& login, const std::string& password);
    bool loginUser(const std::string& login, const std::string& password);

private:
    Database& database;
};
=======
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
>>>>>>> 22e5bb2d01344dce828f490fe50a23d68012dcbf
