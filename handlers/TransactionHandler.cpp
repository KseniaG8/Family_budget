#include "TransactionHandler.h"

TransactionHandler::TransactionHandler(TransactionService &service) : service(service) {}

nlohmann::json TransactionHandler::getTransactions(int user_id) {
    auto transactions = service.getUserTransactions(user_id);

    nlohmann::json response = nlohmann::json::array();

    for (auto &t : transactions) {
        response.push_back({{"id", t.id}, {"type", t.type}, {"amount", t.amount}, {"category", t.category}});
    }

    return response;
}

nlohmann::json TransactionHandler::getTransactionsByCategory(int user_id, const std::string &category) {
    auto transactions = service.getTransactionsByCategory(user_id, category);

    nlohmann::json response = nlohmann::json::array();

    for (auto &t : transactions) {
        response.push_back({{"id", t.id}, {"type", t.type}, {"amount", t.amount}, {"category", t.category}});
    }

    return response;
}

nlohmann::json TransactionHandler::addTransaction(int user_id, std::string type,
                                                  double amount, std::string category, 
                                                  std::string currency, 
                                                  std::string description) {
  service.addTransaction(user_id, type, amount, category, currency, description);
  return {{"status", "success"}};
}

nlohmann::json TransactionHandler::getBalance(int user_id) {
    double balance = service.getBalance(user_id);

    return {{"status", "success"}, {"balance", balance}};
}

nlohmann::json TransactionHandler::updateTransaction(int transaction_id,
                                                     const std::string &type,
                                                     double amount,
                                                     const std::string &category) {
    bool success = service.updateTransaction(transaction_id, type, amount, category);

    if (success) {
        return {{"status", "success"}};
    }

    return {{"status", "error"}};
}

nlohmann::json TransactionHandler::deleteTransaction(int transaction_id) {
    bool success = service.deleteTransaction(transaction_id);

    if (success) {
        return {{"status", "success"}};
    }

    return {{"status", "error"}};
}

nlohmann::json TransactionHandler::getTransactionById(int transaction_id) {
    Transaction t = service.getTransactionById(transaction_id);

    if (t.id == -1) {
        return {{"status", "error"}, {"message", "Transaction not found"}};
    }

    return {{"status", "success"},
            {"transaction",
             {{"id", t.id}, {"user_id", t.user_id}, {"type", t.type}, {"amount", t.amount}, {"category", t.category}}}};
}

nlohmann::json TransactionHandler::setLimit(int user_id, const std::string &category, double limit_amount) {
    bool success = service.setLimit(user_id, category, limit_amount);

    if (success) {
        return {{"status", "success"}};
    }

    return {{"status", "error"}};
}

nlohmann::json TransactionHandler::checkLimit(int user_id, const std::string &category) {
    return service.checkLimit(user_id, category);
}