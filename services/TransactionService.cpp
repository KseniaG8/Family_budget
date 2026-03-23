#include "TransactionService.h"

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
                                        double amount, std::string category) {
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
