#pragma once
#include "../services/TransactionService.h"
#include <nlohmann/json.hpp>

class TransactionHandler {
public:
    TransactionHandler(TransactionService &service);

    nlohmann::json getTransactions(int user_id);
    nlohmann::json getBalance(int user_id);
    nlohmann::json getTransactionsByCategory(int user_id, const std::string &category);
    
    nlohmann::json addTransaction(int user_id, std::string type, double amount,
                                  std::string category, std::string currency, 
                                  std::string description);

    nlohmann::json updateTransaction(int transaction_id, const std::string &type, double amount, const std::string &category);
    nlohmann::json deleteTransaction(int transaction_id);
    nlohmann::json getTransactionById(int transaction_id);
    nlohmann::json setLimit(int user_id, const std::string &category, double limit_amount);
    nlohmann::json checkLimit(int user_id, const std::string &category);

private:
    TransactionService &service;
};