#include "TransactionService.h"
#include "MLCategorizer.h"
#include <spdlog/spdlog.h>

TransactionService::TransactionService(Database &db) : database(db) {}

std::vector<Transaction> TransactionService::getUserTransactions(int user_id) {
  return database.getTransactionsByUser(user_id);
}

std::vector<Transaction>
TransactionService::getTransactionsByCategory(int user_id,
                                              const std::string &category) {
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

double TransactionService::getBalance(int user_id) {
  return database.getBalanceByUser(user_id);
}
