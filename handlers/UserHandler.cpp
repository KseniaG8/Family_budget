#include "UserHandler.h"

UserHandler::UserHandler(UserService& service) : service(service) {}

nlohmann::json UserHandler::registerUser(const std::string& login, const std::string& password) {
    std::string result = service.registerUser(login, password);
    return { {"status", result} };
}

nlohmann::json UserHandler::loginUser(const std::string& login, const std::string& password) {
    std::string result = service.loginUser(login, password);
    
    return { {"status", result} }; 
}

nlohmann::json UserHandler::setup2FA(int user_id, const std::string& login) {
    return service.setup2FA(user_id, login);
}

nlohmann::json UserHandler::verifyLogin2FA(const std::string& login, const std::string& code) {
    if (service.verifyLogin2FA(login, code)) {
        return {{"status", "success"}};
    }
    return {{"status", "invalid_code"}, {"message", "Неверный код 2FA"}};
}
