#include "TransactionService.h"

TransactionService::TransactionService(Database &db) : database(db) {
}

std::vector<Transaction> TransactionService::getUserTransactions(int user_id) {
    return database.getTransactionsByUser(user_id);
}

std::vector<Transaction> TransactionService::getTransactionsByCategory(
    int user_id,
    const std::string &category
) {
    return database.getTransactionsByCategory(user_id, category);
}

void TransactionService::addTransaction(
    int user_id,
    std::string type,
    double amount,
    std::string category
) {
    Transaction t;
    t.user_id = user_id;
    t.type = type;
    t.amount = amount;
    t.category = category;

    database.addTransaction(t);
}

double TransactionService::getBalance(int user_id) {
    return database.getBalanceByUser(user_id);
}

bool TransactionService::updateTransaction(
    int transaction_id,
    const std::string &type,
    double amount,
    const std::string &category
) {
    return database.updateTransaction(transaction_id, type, amount, category);
}

bool TransactionService::deleteTransaction(int transaction_id) {
    return database.deleteTransaction(transaction_id);
}

Transaction TransactionService::getTransactionById(int transaction_id) {
    return database.getTransactionById(transaction_id);
}

std::map<std::string, double> TransactionService::getCategoryStatistics(
    int user_id
) {
    return database.getCategoryStatistics(user_id);
}

bool TransactionService::setLimit(
    int user_id,
    const std::string &category,
    double limit_amount,
    const std::string &period
) {
    return database.setLimit(user_id, category, limit_amount, period);
}

double TransactionService::getLimit(
    int user_id,
    const std::string &category,
    const std::string &period
) {
    return database.getLimit(user_id, category, period);
}

double TransactionService::getSpentByCategory(
    int user_id,
    const std::string &category,
    const std::string &period
) {
    return database.getSpentByCategory(user_id, category, period);
}

bool TransactionService::addGoal(
    int user_id,
    const std::string &name,
    double target_amount,
    const std::string &deadline
) {
    return database.addGoal(user_id, name, target_amount, deadline);
}

std::vector<Goal> TransactionService::getGoals(int user_id) {
    return database.getGoalsByUser(user_id);
}

bool TransactionService::updateGoalProgress(
    int goal_id,
    double current_amount
) {
    return database.updateGoalProgress(goal_id, current_amount);
}

bool TransactionService::updateGoal(
    int goal_id,
    const std::string &name,
    double target_amount,
    const std::string &deadline
) {
    return database.updateGoal(goal_id, name, target_amount, deadline);
}

bool TransactionService::deleteGoal(int goal_id) {
    return database.deleteGoal(goal_id);
}

int TransactionService::createGroup(const std::string &name, int owner_id) {
    int group_id = database.createGroup(name, owner_id);

    if (group_id != -1) {
        database.addUserToGroup(group_id, owner_id, "owner");
    }

    return group_id;
}

bool TransactionService::deleteGroup(int group_id) {
    return database.deleteGroup(group_id);
}

bool TransactionService::groupExists(int group_id) {
    return database.groupExists(group_id);
}

bool TransactionService::addUserToGroup(int group_id, int user_id) {
    return database.addUserToGroup(group_id, user_id, "member");
}

nlohmann::json TransactionService::getCategoryAnalytics(int user_id) {
    auto transactions = database.getTransactionsByUser(user_id);
    std::map<std::string, double> category_sums;
    for (const auto &t : transactions) {
        if (t.type == "expense" || t.type == "Expense") {
            category_sums[t.category] += t.amount;
        }
    }
    nlohmann::json result = nlohmann::json::array();
    for (const auto &[cat, amount] : category_sums) {
        result.push_back({{"category", cat}, {"amount", amount}});
    }
    return result;
}

bool TransactionService::removeUserFromGroup(int group_id, int user_id) {
    return database.removeUserFromGroup(group_id, user_id);
}

bool TransactionService::userExists(int user_id) {
    return database.userExists(user_id);
}

std::vector<GroupMember> TransactionService::getGroupMembers(int group_id) {
    return database.getGroupMembers(group_id);
}

std::vector<Group> TransactionService::getUserGroups(int user_id) {
    return database.getUserGroups(user_id);
}

bool TransactionService::isUserInGroup(int group_id, int user_id) {
    return database.isUserInGroup(group_id, user_id);
}

std::string TransactionService::getUserRoleInGroup(int group_id, int user_id) {
    return database.getUserRoleInGroup(group_id, user_id);
}

bool TransactionService::addGroupTransaction(
    int group_id,
    int user_id,
    const std::string &type,
    double amount,
    const std::string &category
) {
    return database.addGroupTransaction(
        group_id, user_id, type, amount, category
    );
}

std::vector<Transaction> TransactionService::getGroupTransactions(int group_id
) {
    return database.getGroupTransactions(group_id);
}

double TransactionService::getGroupBalance(int group_id) {
    return database.getGroupBalance(group_id);
}
