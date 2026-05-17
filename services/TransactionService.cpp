#include "TransactionService.h"
#include "MLCategorizer.h"
#include <spdlog/spdlog.h>

TransactionService::TransactionService(Database &db) : database(db) {}

std::vector<Transaction> TransactionService::getUserTransactions(int user_id) {
    return database.getTransactionsByUser(user_id);
}

std::vector<Transaction> TransactionService::getTransactionsByCategory(int user_id, const std::string &category) {
    return database.getTransactionsByCategory(user_id, category);
}

void TransactionService::addTransaction(int user_id, std::string type,
                                        double amount, std::string category, 
                                        std::string currency,
                                        std::string description) {

  if (category.empty() && !description.empty()) {
      auto history = database.getTransactionsByUser(user_id);
      MLCategorizer ml;
      ml.train(history);
      category = ml.predictCategory(description); 

      spdlog::info("ML Auto-categorized '{}' as '{}' for user {}", description, category, user_id);
      
  } else if (category.empty()) {
      category = "Разное";
  }

  Transaction t;
  t.user_id = user_id;
  t.type = type;
  t.amount = amount;
  t.category = category; 
  t.currency = currency; 
  t.description = description; 

  database.addTransaction(t);
}

double TransactionService::getBalance(int user_id) { return database.getBalanceByUser(user_id); }

bool TransactionService::updateTransaction(int transaction_id,
                                           const std::string &type,
                                           double amount,
                                           const std::string &category) {
    return database.updateTransaction(transaction_id, type, amount, category);
}

bool TransactionService::deleteTransaction(int transaction_id) { return database.deleteTransaction(transaction_id); }

Transaction TransactionService::getTransactionById(int transaction_id) {
    return database.getTransactionById(transaction_id);
}

bool TransactionService::setLimit(int user_id,
                                  const std::string &category,
                                  double limit_amount,
                                  const std::string &period) {
    return database.setLimit(user_id, category, limit_amount, period);
}

double TransactionService::getLimit(int user_id, const std::string &category, const std::string &period) {
    return database.getLimit(user_id, category, period);
}

double TransactionService::getSpentByCategory(int user_id, const std::string &category, const std::string &period) {
    return database.getSpentByCategory(user_id, category, period);
}

bool TransactionService::addGoal(int user_id, const std::string &name, double target_amount) {
    return database.addGoal(user_id, name, target_amount);
}

std::vector<Goal> TransactionService::getGoals(int user_id) { return database.getGoalsByUser(user_id); }

bool TransactionService::updateGoalProgress(int goal_id, double current_amount) {
    return database.updateGoalProgress(goal_id, current_amount);
}

int TransactionService::createGroup(const std::string &name, int owner_id) {
    int group_id = database.createGroup(name, owner_id);

    if (group_id != -1) {
        database.addUserToGroup(group_id, owner_id);
    }

    return group_id;
}

bool TransactionService::addUserToGroup(int group_id, int user_id) {
    return database.addUserToGroup(group_id, user_id);
}

std::vector<Group> TransactionService::getUserGroups(int user_id) { return database.getUserGroups(user_id); }

bool TransactionService::addGroupTransaction(
    int group_id, int user_id, const std::string &type, double amount, const std::string &category) {
    return database.addGroupTransaction(group_id, user_id, type, amount, category);
}

std::vector<Transaction> TransactionService::getGroupTransactions(int group_id) {
    return database.getGroupTransactions(group_id);
}

double TransactionService::getGroupBalance(int group_id) { return database.getGroupBalance(group_id); }
