#include "Database.h"
#include <iostream>

void Database::init() {
    const char* transactions_sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            type TEXT,
            amount REAL
        );
    )";

    const char* users_sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            login TEXT UNIQUE,
            password TEXT
        );
    )";

    char* errMsg = nullptr;

    if (sqlite3_exec(db, transactions_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create transactions table: "
                  << (errMsg ? errMsg : "unknown error") << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }

    if (sqlite3_exec(db, users_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create users table: "
                  << (errMsg ? errMsg : "unknown error") << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }
}


Database::Database(const std::string& db_name) {
    if (sqlite3_open(db_name.c_str(), &db)) {
        std::cerr << "Can't open database\n";
    }
}

Database::~Database() {
    sqlite3_close(db);
}


void Database::addTransaction(const Transaction& t) {
    std::string sql = "INSERT INTO transactions (user_id, type, amount) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt) sqlite3_finalize(stmt);
        return;
    }

    sqlite3_bind_int(stmt, 1, t.user_id);
    sqlite3_bind_text(stmt, 2, t.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, t.amount);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert transaction failed: " << sqlite3_errmsg(db) << "\n";
    }

    if (stmt) sqlite3_finalize(stmt);
}


std::vector<Transaction> Database::getTransactionsByUser(int user_id) {
    std::vector<Transaction> result;

    std::string sql = "SELECT id, user_id, type, amount FROM transactions WHERE user_id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt) sqlite3_finalize(stmt);
        return result;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction t;

        t.id = sqlite3_column_int(stmt, 0);
        t.user_id = sqlite3_column_int(stmt, 1);

        const unsigned char* typeText = sqlite3_column_text(stmt, 2);
        if (typeText)
            t.type = reinterpret_cast<const char*>(typeText);

        t.amount = sqlite3_column_double(stmt, 3);

        result.push_back(t);
    }

    if (stmt) sqlite3_finalize(stmt);
    return result;
}

void Database::clearTransactions() {
    const char* sql = "DELETE FROM transactions;";
    char* errMsg = nullptr;

    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to clear table: "
                  << (errMsg ? errMsg : "unknown error") << std::endl;

        if (errMsg)
            sqlite3_free(errMsg);
    }
}


bool Database::addUser(const std::string& login, const std::string& password) {
    std::string sql = "INSERT INTO users (login, password) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt) sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert user failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt) sqlite3_finalize(stmt);
        return false;
    }

    if (stmt) sqlite3_finalize(stmt);
    return true;
}

User Database::getUserByLogin(const std::string& login) {
    User user{-1, "", ""};

    std::string sql = "SELECT id, login, password FROM users WHERE login = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt) sqlite3_finalize(stmt);
        return user;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user.id = sqlite3_column_int(stmt, 0);
        const unsigned char* loginText = sqlite3_column_text(stmt, 1);
        const unsigned char* passText = sqlite3_column_text(stmt, 2);

        if (loginText)
            user.login = reinterpret_cast<const char*>(loginText);

        if (passText)
            user.password = reinterpret_cast<const char*>(passText);
    }

    if (stmt) sqlite3_finalize(stmt);
    return user;
}
