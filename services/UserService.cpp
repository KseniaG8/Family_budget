#include "UserService.h"
#include "CryptoHelper.h"
#include <botan/bcrypt.h>
#include <botan/system_rng.h>

UserService::UserService(Database& db) : database(db) {}


nlohmann::json UserService::registerUser(const std::string &login, const std::string &password) {
    std::string hashedPassword = CryptoHelper::hashPassword(password);
    
    if (database.addUser(login, hashedPassword)) { 
        spdlog::info("User {} registered successfully with hashed password", login);
        return {{"status", "success"}};
    } else {
        spdlog::warn("Registration failed for user: {}", login);
        return {{"status", "error"}, {"message", "User already exists or DB error"}};
    }
}
nlohmann::json UserService::loginUser(const std::string &login, const std::string &password) {
    std::string hashedPassword = CryptoHelper::hashPassword(password);

    int user_id = database.checkUser(login, hashedPassword);
    
    if (user_id != -1) {
        spdlog::info("User {} logged in successfully", login);
        return {{"status", "success"}, {"user_id", user_id}};
    } else {
        spdlog::warn("Failed login attempt for user: {}", login);
        return {{"status", "error"}, {"message", "Invalid login or password"}};
    }
}
