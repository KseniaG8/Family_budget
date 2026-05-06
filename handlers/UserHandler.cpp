#include "UserHandler.h"

UserHandler::UserHandler(UserService& service) : service(service) {}

nlohmann::json UserHandler::registerUser(const std::string& login, const std::string& password) {
    std::string result = service.registerUser(login, password);
    return { {"status", result} };
}

nlohmann::json UserHandler::loginUser(const std::string& login, const std::string& password) {
    if (service.loginUser(login, password))
        return { {"status", "success"} };
    else
        return { {"status", "invalid_credentials"} };
}