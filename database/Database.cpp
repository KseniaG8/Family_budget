#include "Database.h"
#include <spdlog/spdlog.h>
#include <iostream>

Database::Database(const std::string &db_name) {
  if (sqlite3_open(db_name.c_str(), &db)) {
    spdlog::critical("Can't open database: {}", sqlite3_errmsg(db));
  }
}

void Database::init() {
  const char *transactions_sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            type TEXT,
            amount REAL,
            category TEXT,
            currency TEXT DEFAULT 'RUB',
            description TEXT
        );
    )";

  const char *users_sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            login TEXT UNIQUE,
            password TEXT
        );
    )";

  const char *rates_sql = R"(
        CREATE TABLE IF NOT EXISTS exchange_rates (
            currency_code TEXT PRIMARY KEY,
            rate_to_base REAL
        );
    )";

  char *errMsg = nullptr;

  if (sqlite3_exec(db, transactions_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
    spdlog::error("Failed to create transactions table: {}", errMsg ? errMsg : "unknown error");
    if (errMsg) sqlite3_free(errMsg);
    errMsg = nullptr;
  }

  if (sqlite3_exec(db, users_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
    spdlog::error("Failed to create users table: {}", errMsg ? errMsg : "unknown error");
    if (errMsg) sqlite3_free(errMsg);
    errMsg = nullptr;
  }

  if (sqlite3_exec(db, rates_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
    spdlog::error("Failed to create exchange_rates table: {}", errMsg ? errMsg : "unknown error");
    if (errMsg) sqlite3_free(errMsg);
    errMsg = nullptr;
  }

  const char *insert_default_rates = R"(
        INSERT OR IGNORE INTO exchange_rates (currency_code, rate_to_base) VALUES 
        ('RUB', 1.0), ('USD', 92.50), ('EUR', 100.20);
  )";
  sqlite3_exec(db, insert_default_rates, nullptr, nullptr, nullptr);

  spdlog::info("Database tables initialized successfully!");
}

Database::~Database() { sqlite3_close(db); }

void Database::addTransaction(const Transaction &t) {
  std::string sql = "INSERT INTO transactions (user_id, type, amount, category, currency, description) VALUES (?, ?, ?, ?, ?, ?);";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    spdlog::error("Prepare failed in addTransaction: {}", sqlite3_errmsg(db));
    if (stmt) sqlite3_finalize(stmt);
    return;
  }

  sqlite3_bind_int(stmt, 1, t.user_id);
  sqlite3_bind_text(stmt, 2, t.type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt, 3, t.amount);
  sqlite3_bind_text(stmt, 4, t.category.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, t.currency.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, t.description.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    spdlog::error("Insert transaction failed: {}", sqlite3_errmsg(db));
  }

  if (stmt) sqlite3_finalize(stmt);
}

std::vector<Transaction> Database::getTransactionsByUser(int user_id) {
  std::vector<Transaction> transactions;
  
  std::string sql = "SELECT id, user_id, type, amount, category, currency, description FROM transactions WHERE user_id = ?;";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    spdlog::error("Prepare failed in getTransactionsByUser: {}", sqlite3_errmsg(db));
    return transactions;
  }

  sqlite3_bind_int(stmt, 1, user_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Transaction t;
    t.id = sqlite3_column_int(stmt, 0);
    t.user_id = sqlite3_column_int(stmt, 1);

    const unsigned char *typeText = sqlite3_column_text(stmt, 2);
    if (typeText) t.type = reinterpret_cast<const char *>(typeText);

    t.amount = sqlite3_column_double(stmt, 3);

    const unsigned char *categoryText = sqlite3_column_text(stmt, 4);
    if (categoryText) t.category = reinterpret_cast<const char *>(categoryText);

    const unsigned char *currencyText = sqlite3_column_text(stmt, 5);
    if (currencyText) t.currency = reinterpret_cast<const char *>(currencyText);

    const unsigned char *descText = sqlite3_column_text(stmt, 6);
    if (descText) t.description = reinterpret_cast<const char *>(descText);

    transactions.push_back(t);
  }

  sqlite3_finalize(stmt);
  return transactions;
}

void Database::clearTransactions() {
  const char *sql = "DELETE FROM transactions;";
  char *errMsg = nullptr;

  if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
    std::cerr << "Failed to clear table: "
              << (errMsg ? errMsg : "unknown error") << std::endl;

    if (errMsg)
      sqlite3_free(errMsg);
  }
}

bool Database::addUser(const std::string &login, const std::string &password) {
  std::string sql = "INSERT INTO users (login, password) VALUES (?, ?);";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
    if (stmt)
      sqlite3_finalize(stmt);
    return false;
  }

  sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    std::cerr << "Insert user failed: " << sqlite3_errmsg(db) << "\n";
    if (stmt)
      sqlite3_finalize(stmt);
    return false;
  }

  if (stmt)
    sqlite3_finalize(stmt);
  return true;
}

User Database::getUserByLogin(const std::string &login) {
  User user{-1, "", ""};

  std::string sql = "SELECT id, login, password FROM users WHERE login = ?;";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
    if (stmt)
      sqlite3_finalize(stmt);
    return user;
  }

  sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    user.id = sqlite3_column_int(stmt, 0);
    const unsigned char *loginText = sqlite3_column_text(stmt, 1);
    const unsigned char *passText = sqlite3_column_text(stmt, 2);

    if (loginText)
      user.login = reinterpret_cast<const char *>(loginText);

    if (passText)
      user.password = reinterpret_cast<const char *>(passText);
  }

  if (stmt)
    sqlite3_finalize(stmt);
  return user;
}

double Database::getBalanceByUser(int user_id) {
  double balance = 0.0;
  std::string sql = R"(
      SELECT t.type, t.amount, IFNULL(er.rate_to_base, 1.0)
      FROM transactions t
      LEFT JOIN exchange_rates er ON t.currency = er.currency_code
      WHERE t.user_id = ?;
  )";

  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    spdlog::error("Prepare failed in getBalanceByUser: {}", sqlite3_errmsg(db));
    if (stmt) sqlite3_finalize(stmt);
    return balance;
  }

  sqlite3_bind_int(stmt, 1, user_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *typeText = sqlite3_column_text(stmt, 0);
    double amount = sqlite3_column_double(stmt, 1);
    double rate = sqlite3_column_double(stmt, 2);

    std::string type;
    if (typeText) type = reinterpret_cast<const char *>(typeText);

    double converted_amount = amount * rate;

    if (type == "income" || type == "Income")
      balance += converted_amount;
    else if (type == "expense" || type == "Expense")
      balance -= converted_amount;
  }

  if (stmt) sqlite3_finalize(stmt);
  return balance;
}

std::vector<Transaction>
Database::getTransactionsByCategory(int user_id, const std::string &category) {
  std::vector<Transaction> result;

  std::string sql = "SELECT id, user_id, type, amount, category FROM "
                    "transactions WHERE user_id = ? AND category = ?;";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
    return result;
  }

  sqlite3_bind_int(stmt, 1, user_id);
  sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Transaction t;

    t.id = sqlite3_column_int(stmt, 0);
    t.user_id = sqlite3_column_int(stmt, 1);
    t.type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    t.amount = sqlite3_column_double(stmt, 3);

    const unsigned char *catText = sqlite3_column_text(stmt, 4);
    t.category = catText ? reinterpret_cast<const char *>(catText) : "";

    result.push_back(t);
  }

  if (stmt)
    sqlite3_finalize(stmt);
  return result;
}
void Database::updateExchangeRate(const std::string& currency_code, double rate_to_base) {
    std::string sql = "INSERT OR REPLACE INTO exchange_rates (currency_code, rate_to_base) VALUES (?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, currency_code.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 2, rate_to_base);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to update exchange rate for " << currency_code << "\n";
        }
    }
    if (stmt) sqlite3_finalize(stmt);
}
