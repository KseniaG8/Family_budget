#include "TransactionHandler.h"

TransactionHandler::TransactionHandler(TransactionService& service)
    : service(service) {}

    
nlohmann::json TransactionHandler::getTransactions(int user_id) {
    auto transactions = service.getUserTransactions(user_id);

    nlohmann::json response = nlohmann::json::array();

    for (auto& t : transactions) {
        response.push_back({
            {"id", t.id},
            {"type", t.type},
            {"amount", t.amount}
        });
    }

    return response;
}


nlohmann::json TransactionHandler::addTransaction(int user_id, std::string type, double amount) {
    service.addTransaction(user_id, type, amount);

    return {
        {"status", "success"}
    };
}