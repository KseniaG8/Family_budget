#pragma once
#include "../database/Database.h"
#include "../models/Transaction.h"
#include <nlohmann/json.hpp>
#include <vector>

class TransactionService {
public:
    TransactionService(Database &db);

    std::vector<Transaction> getUserTransactions(int user_id);

    void addTransaction(int user_id, std::string type, double amount,
                        std::string category, std::string currency, 
                        std::string description); 

    double getBalance(int user_id);

    std::vector<Transaction> getTransactionsByCategory(int user_id, const std::string &category);

    bool updateTransaction(int transaction_id, const std::string &type, double amount, const std::string &category);

    bool deleteTransaction(int transaction_id);

    Transaction getTransactionById(int transaction_id);

    bool setLimit(int user_id, const std::string &category, double limit_amount);
    nlohmann::json checkLimit(int user_id, const std::string &category);

private:
    Database &database;
};