#pragma once
#include <nlohmann/json.hpp>

#include "../services/TransactionService.h"

class TransactionHandler {
public:
    TransactionHandler(TransactionService &service);

    nlohmann::json getTransactions(int user_id);
    nlohmann::json addTransaction(
        int user_id,
        std::string type,
        double amount,
        std::string category
    );
    nlohmann::json getBalance(int user_id);
    nlohmann::json
    getTransactionsByCategory(int user_id, const std::string &category);

    nlohmann::json updateTransaction(
        int transaction_id,
        const std::string &type,
        double amount,
        const std::string &category
    );

    nlohmann::json deleteTransaction(int transaction_id);

    nlohmann::json getTransactionById(int transaction_id);

    nlohmann::json setLimit(
        int user_id,
        const std::string &category,
        double limit_amount,
        const std::string &period
    );
    nlohmann::json checkLimit(
        int user_id,
        const std::string &category,
        const std::string &period
    );

    nlohmann::json addGoal(
        int user_id,
        const std::string &name,
        double target_amount,
        const std::string &deadline
    );
    nlohmann::json getGoals(int user_id);
    nlohmann::json updateGoalProgress(int goal_id, double current_amount);
    nlohmann::json updateGoal(
        int goal_id,
        const std::string &name,
        double target_amount,
        const std::string &deadline
    );
    nlohmann::json deleteGoal(int goal_id);

    nlohmann::json createGroup(const std::string &name, int owner_id);
    nlohmann::json deleteGroup(int group_id, int requester_id);

    nlohmann::json addUserToGroup(int group_id, const std::string& login); 
    nlohmann::json getCategoryAnalytics(int user_id);
    nlohmann::json
    removeUserFromGroup(int group_id, int user_id, int requester_id);
    nlohmann::json getGroupMembers(int group_id, int requester_id);
    nlohmann::json getUserGroups(int user_id);

    nlohmann::json addGroupTransaction(
        int group_id,
        int user_id,
        const std::string &type,
        double amount,
        const std::string &category
    );
    nlohmann::json getGroupTransactions(int group_id, int requester_id);
    nlohmann::json getGroupBalance(int group_id, int requester_id);

private:
    TransactionService &service;
};
