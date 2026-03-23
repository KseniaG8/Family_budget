#pragma once
#include "../models/Transaction.h"
#include "../models/User.h"
#include <sqlite3.h>
#include <vector>

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

  double getBalanceByUser(int user_id);

  std::vector<Transaction>
  getTransactionsByCategory(int user_id, const std::string &category);

private:
  sqlite3 *db = nullptr;
};
