#pragma once
#include <vector>
#include "../models/Transaction.h"
#include "../models/User.h"
#include <sqlite3.h>

class Database {
public:
    Database(const std::string& db_name);
    ~Database();

    void init();

    void addTransaction(const Transaction& t);
    std::vector<Transaction> getTransactionsByUser(int user_id);

    void clearTransactions();

    bool addUser(const std::string& login, const std::string& password);
    User getUserByLogin(const std::string& login);

private:
    sqlite3* db = nullptr;
};
