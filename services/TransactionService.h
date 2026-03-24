#pragma once
#include "../database/Database.h"
#include "../models/Transaction.h"
#include <vector>

class TransactionService {
public:
  TransactionService(Database &db);

  std::vector<Transaction> getUserTransactions(int user_id);

  void addTransaction(int user_id, std::string type, double amount,
                      std::string category, std::string currency, 
                      std::string description); 

  double getBalance(int user_id);

  std::vector<Transaction>
  getTransactionsByCategory(int user_id, const std::string &category);

private:
  Database &database;
};
