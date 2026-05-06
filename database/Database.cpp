#include "Database.h"
#include <iostream>

void Database::init() {
    const char *transactions_sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            type TEXT,
            amount REAL,
            category TEXT
        );
    )";

    const char *users_sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            login TEXT UNIQUE,
            password TEXT
        );
    )";

    const char *limits_sql = R"(
        CREATE TABLE IF NOT EXISTS limits (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            category TEXT,
            limit_amount REAL,
            UNIQUE(user_id, category)
        );
    )";

    char *errMsg = nullptr;

    if (sqlite3_exec(db, transactions_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create transactions table: " << (errMsg ? errMsg : "unknown error") << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }

    if (sqlite3_exec(db, users_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create users table: " << (errMsg ? errMsg : "unknown error") << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }

    if (sqlite3_exec(db, limits_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create limits table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

Database::Database(const std::string &db_name) {
    if (sqlite3_open(db_name.c_str(), &db)) {
        std::cerr << "Can't open database\n";
    }
}

Database::~Database() { sqlite3_close(db); }

void Database::addTransaction(const Transaction &t) {
    std::string sql = "INSERT INTO transactions (user_id, type, amount, "
                      "category) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt)
            sqlite3_finalize(stmt);
        return;
    }

    sqlite3_bind_int(stmt, 1, t.user_id);
    sqlite3_bind_text(stmt, 2, t.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, t.amount);
    sqlite3_bind_text(stmt, 4, t.category.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert transaction failed: " << sqlite3_errmsg(db) << "\n";
    }

    if (stmt)
        sqlite3_finalize(stmt);
}

std::vector<Transaction> Database::getTransactionsByUser(int user_id) {
    std::vector<Transaction> result;

    std::string sql = "SELECT id, user_id, type, amount, category FROM "
                      "transactions WHERE user_id = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt)
            sqlite3_finalize(stmt);
        return result;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction t;

        t.id = sqlite3_column_int(stmt, 0);
        t.user_id = sqlite3_column_int(stmt, 1);

        const unsigned char *typeText = sqlite3_column_text(stmt, 2);
        if (typeText)
            t.type = reinterpret_cast<const char *>(typeText);

        t.amount = sqlite3_column_double(stmt, 3);

        const unsigned char *categoryText = sqlite3_column_text(stmt, 4);
        if (categoryText)
            t.category = reinterpret_cast<const char *>(categoryText);

        result.push_back(t);
    }

    if (stmt)
        sqlite3_finalize(stmt);
    return result;
}

void Database::clearTransactions() {
    const char *sql = "DELETE FROM transactions;";
    char *errMsg = nullptr;

    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to clear table: " << (errMsg ? errMsg : "unknown error") << std::endl;

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

    std::string sql = "SELECT type, amount FROM transactions WHERE user_id = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt)
            sqlite3_finalize(stmt);
        return balance;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *typeText = sqlite3_column_text(stmt, 0);
        double amount = sqlite3_column_double(stmt, 1);

        std::string type;
        if (typeText)
            type = reinterpret_cast<const char *>(typeText);

        if (type == "income" || type == "Income")
            balance += amount;
        else if (type == "expense" || type == "Expense")
            balance -= amount;
    }

    if (stmt)
        sqlite3_finalize(stmt);
    return balance;
}

std::vector<Transaction> Database::getTransactionsByCategory(int user_id, const std::string &category) {
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

bool Database::updateTransaction(int transaction_id,
                                 const std::string &type,
                                 double amount,
                                 const std::string &category) {
    std::string sql = "UPDATE transactions "
                      "SET type = ?, amount = ?, category = ? "
                      "WHERE id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, amount);
    sqlite3_bind_text(stmt, 3, category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, transaction_id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

bool Database::deleteTransaction(int transaction_id) {
    std::string sql = "DELETE FROM transactions WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, transaction_id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

Transaction Database::getTransactionById(int transaction_id) {
    Transaction t;
    t.id = -1;

    std::string sql = "SELECT id, user_id, type, amount, category "
                      "FROM transactions WHERE id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return t;
    }

    sqlite3_bind_int(stmt, 1, transaction_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        t.id = sqlite3_column_int(stmt, 0);
        t.user_id = sqlite3_column_int(stmt, 1);

        const unsigned char *typeText = sqlite3_column_text(stmt, 2);
        t.type = typeText ? reinterpret_cast<const char *>(typeText) : "";

        t.amount = sqlite3_column_double(stmt, 3);

        const unsigned char *categoryText = sqlite3_column_text(stmt, 4);
        t.category = categoryText ? reinterpret_cast<const char *>(categoryText) : "";
    }

    sqlite3_finalize(stmt);
    return t;
}

bool Database::setLimit(int user_id, const std::string &category, double limit_amount) {
    std::string sql = "INSERT INTO limits (user_id, category, limit_amount) "
                      "VALUES (?, ?, ?) "
                      "ON CONFLICT(user_id, category) "
                      "DO UPDATE SET limit_amount = excluded.limit_amount;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, limit_amount);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

double Database::getLimit(int user_id, const std::string &category) {
    double limit = -1.0;

    std::string sql = "SELECT limit_amount FROM limits "
                      "WHERE user_id = ? AND category = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return limit;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        limit = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return limit;
}

double Database::getSpentByCategory(int user_id, const std::string &category) {
    double spent = 0.0;

    std::string sql = "SELECT amount FROM transactions "
                      "WHERE user_id = ? AND category = ? AND type = 'expense';";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return spent;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        spent += sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return spent;
}
