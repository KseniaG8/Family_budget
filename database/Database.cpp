#include "Database.h"

#include <iostream>

void Database::init() {
    const char *transactions_sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            group_id INTEGER,
            type TEXT,
            amount REAL,
            category TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,

            FOREIGN KEY(user_id)
                REFERENCES users(id)
                ON DELETE CASCADE,

            FOREIGN KEY(group_id)
                REFERENCES groups(id)
                ON DELETE CASCADE
        );
    )";

    const char *users_sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            login TEXT UNIQUE,
            password TEXT,
            totp_secret TEXT DEFAULT '',
            is_2fa_enabled INTEGER DEFAULT 0
        );
    )";

    const char *limits_sql = R"(
        CREATE TABLE IF NOT EXISTS limits (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            category TEXT,
            limit_amount REAL,
            period TEXT,
            UNIQUE(user_id, category, period)
        );
    )";

    const char *goals_sql = R"(
        CREATE TABLE IF NOT EXISTS goals (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            name TEXT,
            deadline TEXT,
            target_amount REAL,
            current_amount REAL DEFAULT 0
        );
    )";

    const char *groups_sql = R"(
        CREATE TABLE IF NOT EXISTS groups (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            owner_id INTEGER
        );
    )";

    const char *group_members_sql = R"(
        CREATE TABLE IF NOT EXISTS group_members (
            group_id INTEGER,
            user_id INTEGER,
            role TEXT,
            UNIQUE(group_id, user_id),

            FOREIGN KEY(group_id)
                REFERENCES groups(id)
                ON DELETE CASCADE,

            FOREIGN KEY(user_id)
                REFERENCES users(id)
                ON DELETE CASCADE
        );
    )";

    char *errMsg = nullptr;

    if (sqlite3_exec(db, transactions_sql, nullptr, nullptr, &errMsg) !=
        SQLITE_OK) {
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

    if (sqlite3_exec(db, limits_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create limits table: " << errMsg << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }

    if (sqlite3_exec(db, goals_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create goals table: " << errMsg << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }

    if (sqlite3_exec(db, groups_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create groups table: " << errMsg << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }

    if (sqlite3_exec(db, group_members_sql, nullptr, nullptr, &errMsg) !=
        SQLITE_OK) {
        std::cerr << "Failed to create group_members table: " << errMsg
                  << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
            errMsg = nullptr;
        }
    }
}

Database::Database(const std::string &db_name) {
    if (sqlite3_open(db_name.c_str(), &db)) {
        std::cerr << "Can't open database\n";
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
}

Database::~Database() {
    sqlite3_close(db);
}

bool Database::enable2FA(int user_id, const std::string &secret) {
    std::string query = "UPDATE users SET totp_secret = '" + secret +
                        "' WHERE id = " + std::to_string(user_id) + ";";
    char *errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void Database::updateExchangeRate(
    const std::string &currencyCode,
    double rate
) {
    std::string query =
        "INSERT INTO exchange_rates (currency, rate) VALUES ('" + currencyCode +
        "', " + std::to_string(rate) +
        ") ON CONFLICT(currency) DO UPDATE SET rate = " + std::to_string(rate) +
        ";";
    char *errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
    }
}
