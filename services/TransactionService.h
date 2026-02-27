#pragma once
#include "../database/Database.h"
#include "../models/Transaction.h"
#include <vector>

class TransactionService {
public:
    TransactionService(Database& db);

    std::vector<Transaction> getUserTransactions(int user_id);
    void addTransaction(int user_id, std::string type, double amount);

private:
    Database& database;
};