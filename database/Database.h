#pragma once
#include "../models/Transaction.h"
#include "../models/User.h"
#include "../models/Goal.h"
#include <sqlite3.h>
#include <vector>

class Database {
public:
    Database(const std::string &db_name);
    ~Database();

    void init();

    void addTransaction(const Transaction &t);
    std::vector<Transaction> getTransactionsByUser(int user_id);
    std::vector<Transaction> getTransactionsByCategory(int user_id, const std::string &category);
    bool updateTransaction(int transaction_id, const std::string &type, double amount, const std::string &category);
    bool deleteTransaction(int transaction_id);
    Transaction getTransactionById(int transaction_id);
    void clearTransactions();

    bool addUser(const std::string &login, const std::string &password);
    User getUserByLogin(const std::string &login);
    bool enable2FA(int user_id, const std::string &secret);

    double getBalanceByUser(int user_id);
    bool setLimit(int user_id, const std::string &category, double limit_amount);
    double getLimit(int user_id, const std::string &category);
    double getSpentByCategory(int user_id, const std::string &category);

    int createFamily(const std::string &family_name);
    bool joinFamily(int user_id, int family_id);
    int getUserFamilyId(int user_id);

    bool addGoal(int family_id, const std::string &name, double target_amount);
    std::vector<Goal> getGoalsByFamily(int family_id);
    bool addMoneyToGoal(int goal_id, double amount);

private:
    sqlite3 *db = nullptr;
};