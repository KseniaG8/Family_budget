#include "UserHandler.h"

UserHandler::UserHandler(UserService& service) : service(service) {}

nlohmann::json UserHandler::registerUser(const std::string& login, const std::string& password) {
    std::string result = service.registerUser(login, password);
    return { {"status", result} };
}

nlohmann::json UserHandler::loginUser(const std::string& login, const std::string& password) {
<<<<<<< HEAD
    if (service.loginUser(login, password))
        return { {"status", "success"} };
    else
        return { {"status", "invalid_credentials"} };
}
=======
    return service.loginUser(login, password);
}

nlohmann::json UserHandler::setup2FA(int user_id, const std::string& login) {
    return service.setup2FA(user_id, login);
}

nlohmann::json UserHandler::verifyLogin2FA(const std::string& login, const std::string& code) {
    return service.verifyLogin2FA(login, code);
}
>>>>>>> 22e5bb2d01344dce828f490fe50a23d68012dcbf
