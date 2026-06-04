#include "TransactionHandler.h"

TransactionHandler::TransactionHandler(TransactionService &service)
    : service(service) {
}

nlohmann::json TransactionHandler::getTransactions(int user_id) {
    auto transactions = service.getUserTransactions(user_id);

    nlohmann::json response = nlohmann::json::array();

    for (auto &t : transactions) {
        response.push_back(
            {{"id", t.id},
             {"type", t.type},
             {"amount", t.amount},
             {"category", t.category}}
        );
    }

    return response;
}

nlohmann::json TransactionHandler::getTransactionsByCategory(
    int user_id,
    const std::string &category
) {
    auto transactions = service.getTransactionsByCategory(user_id, category);

    nlohmann::json response = nlohmann::json::array();

    for (auto &t : transactions) {
        response.push_back(
            {{"id", t.id},
             {"type", t.type},
             {"amount", t.amount},
             {"category", t.category}}
        );
    }

    return response;
}

nlohmann::json TransactionHandler::addTransaction(
    int user_id,
    std::string type,
    double amount,
    std::string category
) {
    service.addTransaction(user_id, type, amount, category);

    return {{"status", "success"}};
}

nlohmann::json TransactionHandler::getBalance(int user_id) {
    double balance = service.getBalance(user_id);

    return {{"status", "success"}, {"balance", balance}};
}

nlohmann::json TransactionHandler::updateTransaction(
    int transaction_id,
    const std::string &type,
    double amount,
    const std::string &category
) {
    bool success =
        service.updateTransaction(transaction_id, type, amount, category);

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

    return {
        {"status", "success"},
        {"transaction",
         {{"id", t.id},
          {"user_id", t.user_id},
          {"type", t.type},
          {"amount", t.amount},
          {"category", t.category}}}};
}

nlohmann::json TransactionHandler::setLimit(
    int user_id,
    const std::string &category,
    double limit_amount,
    const std::string &period
) {
    bool success = service.setLimit(user_id, category, limit_amount, period);

    if (success) {
        return {{"status", "success"}};
    }

    return {{"status", "error"}};
}

nlohmann::json TransactionHandler::checkLimit(
    int user_id,
    const std::string &category,
    const std::string &period
) {
    double limit = service.getLimit(user_id, category, period);

    if (limit < 0) {
        return {{"status", "error"}, {"message", "Limit not found"}};
    }

    double spent = service.getSpentByCategory(user_id, category, period);

    return {{"status", "success"},      {"category", category},
            {"period", period},         {"limit", limit},
            {"spent", spent},           {"remaining", limit - spent},
            {"exceeded", spent > limit}};
}

nlohmann::json TransactionHandler::addGoal(
    int user_id,
    const std::string &name,
    double target_amount,
    const std::string &deadline
) {
    bool success = service.addGoal(user_id, name, target_amount, deadline);

    return {{"status", success ? "success" : "error"}};
}

nlohmann::json TransactionHandler::deleteGoal(int goal_id) {
    bool success = service.deleteGoal(goal_id);

    return {{"status", success ? "success" : "error"}};
}

nlohmann::json TransactionHandler::getGoals(int user_id) {
    auto goals = service.getGoals(user_id);

    nlohmann::json response = nlohmann::json::array();

    for (auto &g : goals) {
        response.push_back(
            {{"id", g.id},
             {"user_id", g.user_id},
             {"name", g.name},
             {"target_amount", g.target_amount},
             {"current_amount", g.current_amount},
             {"progress_percent",
              g.target_amount > 0 ? (g.current_amount / g.target_amount) * 100
                                  : 0},
             {"deadline", g.deadline},
             {"remaining", g.target_amount - g.current_amount}}
        );
    }

    return response;
}

nlohmann::json
TransactionHandler::updateGoalProgress(int goal_id, double current_amount) {
    bool success = service.updateGoalProgress(goal_id, current_amount);

    return {{"status", success ? "success" : "error"}};
}

nlohmann::json TransactionHandler::updateGoal(
    int goal_id,
    const std::string &name,
    double target_amount,
    const std::string &deadline
) {
    bool success = service.updateGoal(goal_id, name, target_amount, deadline);

    return {{"status", success ? "success" : "error"}};
}

nlohmann::json
TransactionHandler::createGroup(const std::string &name, int owner_id) {
    int group_id = service.createGroup(name, owner_id);

    if (group_id == -1) {
        return {{"status", "error"}, {"message", "Failed to create group"}};
    }

    return {{"status", "success"}, {"group_id", group_id}};
}

nlohmann::json TransactionHandler::deleteGroup(int group_id, int requester_id) {
    std::string role = service.getUserRoleInGroup(group_id, requester_id);

    if (role.empty()) {
        return {
            {"status", "error"},
            {"message", "User is not a member of this group"}};
    }

    if (role != "owner") {
        return {
            {"status", "error"},
            {"message", "Only group owner can delete the group"}};
    }

    bool success = service.deleteGroup(group_id);

    return {{"status", success ? "success" : "error"}};
}

nlohmann::json TransactionHandler::addUserToGroup(int group_id, int user_id) {
    if (!service.userExists(user_id)) {
        return {{"status", "error"}, {"message", "User not found"}};
    }

    if (!service.groupExists(group_id)) {
        return {{"status", "error"}, {"message", "Group not found"}};
    }

    bool success = service.addUserToGroup(group_id, user_id);

    if (!success) {
        return {
            {"status", "error"}, {"message", "Failed to add user to group"}};
    }

    return {{"status", "success"}};
}

nlohmann::json TransactionHandler::removeUserFromGroup(
    int group_id,
    int user_id,
    int requester_id
) {
    std::string role = service.getUserRoleInGroup(group_id, requester_id);

    if (role.empty()) {
        return {
            {"status", "error"},
            {"message", "User is not a member of this group"}};
    }

    if (role != "owner") {
        return {
            {"status", "error"},
            {"message", "Only group owner can remove members"}};
    }

    std::string targetRole = service.getUserRoleInGroup(group_id, user_id);

    if (targetRole == "owner") {
        return {{"status", "error"}, {"message", "Owner cannot be removed"}};
    }

    bool success = service.removeUserFromGroup(group_id, user_id);

    return {{"status", success ? "success" : "error"}};
}

nlohmann::json
TransactionHandler::getGroupMembers(int group_id, int requester_id) {
    if (!service.isUserInGroup(group_id, requester_id)) {
        return {{"status", "error"}, {"message", "Access denied"}};
    }

    auto members = service.getGroupMembers(group_id);

    nlohmann::json response = nlohmann::json::array();

    for (const auto &member : members) {
        response.push_back(
            {{"id", member.id}, {"login", member.login}, {"role", member.role}}
        );
    }

    return response;
}

nlohmann::json TransactionHandler::getUserGroups(int user_id) {
    auto groups = service.getUserGroups(user_id);

    nlohmann::json response = nlohmann::json::array();

    for (const auto &g : groups) {
        response.push_back(
            {{"id", g.id}, {"name", g.name}, {"owner_id", g.owner_id}}
        );
    }

    return response;
}

nlohmann::json TransactionHandler::addGroupTransaction(
    int group_id,
    int user_id,
    const std::string &type,
    double amount,
    const std::string &category
) {
    if (!service.isUserInGroup(group_id, user_id)) {
        return {
            {"status", "error"},
            {"message", "User is not a member of this group"}};
    }

    bool success =
        service.addGroupTransaction(group_id, user_id, type, amount, category);

    return {{"status", success ? "success" : "error"}};
}

nlohmann::json
TransactionHandler::getGroupTransactions(int group_id, int requester_id) {
    if (!service.isUserInGroup(group_id, requester_id)) {
        return {{"status", "error"}, {"message", "Access denied"}};
    }

    auto transactions = service.getGroupTransactions(group_id);

    nlohmann::json response = nlohmann::json::array();

    for (const auto &t : transactions) {
        response.push_back(
            {{"id", t.id},
             {"user_id", t.user_id},
             {"group_id", t.group_id},
             {"type", t.type},
             {"amount", t.amount},
             {"category", t.category}}
        );
    }

    return response;
}

nlohmann::json
TransactionHandler::getGroupBalance(int group_id, int requester_id) {
    if (!service.isUserInGroup(group_id, requester_id)) {
        return {{"status", "error"}, {"message", "Access denied"}};
    }

    double balance = service.getGroupBalance(group_id);

    return {
        {"status", "success"}, {"group_id", group_id}, {"balance", balance}};
}
