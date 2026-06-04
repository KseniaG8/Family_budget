#pragma once
#include <sqlite3.h>
#include <mutex>
#include <vector>

#include "../models/Goal.h"
#include "../models/Group.h"
#include "../models/GroupMember.h"
#include "../models/Transaction.h"
#include "../models/User.h"

class Database {
public:
    Database(const std::string &db_name);
    ~Database();

    void init();

    void addTransaction(const Transaction &t);
    std::vector<Transaction> getTransactionsByUser(int user_id);

    void clearTransactions();

    bool addUser(const std::string &login, const std::string &password);
    User getUserByLogin(const std::string &login);

    bool updateTransaction(
        int transaction_id,
        const std::string &type,
        double amount,
        const std::string &category
    );

    double getBalanceByUser(int user_id);

    std::vector<Transaction>
    getTransactionsByCategory(int user_id, const std::string &category);

    bool deleteTransaction(int transaction_id);

    Transaction getTransactionById(int transaction_id);

    bool setLimit(
        int user_id,
        const std::string &category,
        double limit_amount,
        const std::string &period
    );
    double getLimit(
        int user_id,
        const std::string &category,
        const std::string &period
    );
    double getSpentByCategory(
        int user_id,
        const std::string &category,
        const std::string &period
    );

    bool addGoal(
        int user_id,
        const std::string &name,
        double target_amount,
        const std::string &deadline
    );

    std::vector<Goal> getGoalsByUser(int user_id);

    bool updateGoalProgress(int goal_id, double current_amount);

    bool updateGoal(
        int goal_id,
        const std::string &name,
        double target_amount,
        const std::string &deadline
    );

    bool deleteGoal(int goal_id);

    int createGroup(const std::string &name, int owner_id);

    bool deleteGroup(int group_id);

    bool groupExists(int group_id);

    bool addUserToGroup(int group_id, int user_id, const std::string &role);

    bool removeUserFromGroup(int group_id, int user_id);

    bool userExists(int user_id);

    std::string getUserRoleInGroup(int group_id, int user_id);

    std::vector<GroupMember> getGroupMembers(int group_id);

    std::vector<Group> getUserGroups(int user_id);

    bool isUserInGroup(int group_id, int user_id);

    bool addGroupTransaction(
        int group_id,
        int user_id,
        const std::string &type,
        double amount,
        const std::string &category
    );

    std::vector<Transaction> getGroupTransactions(int group_id);

    double getGroupBalance(int group_id);

    bool enable2FA(int user_id, const std::string& secret);
    void updateExchangeRate(const std::string& currencyCode, double rate);
    
private:
    sqlite3 *db = nullptr;
    std::mutex db_mutex;
};
