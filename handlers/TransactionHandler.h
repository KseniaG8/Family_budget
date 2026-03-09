#pragma once
#include "../services/TransactionService.h"
#include <nlohmann/json.hpp>

class TransactionHandler {
public:
    TransactionHandler(TransactionService& service);

    nlohmann::json getTransactions(int user_id);
    nlohmann::json addTransaction(int user_id, std::string type, double amount);

private:
    TransactionService& service;
};