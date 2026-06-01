#include "Database.h"

#include <iostream>

bool Database::addUser(const std::string &login, const std::string &password) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql = "INSERT INTO users (login, password) VALUES (?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert user failed: " << sqlite3_errmsg(db) << "\n";
        if (stmt) {
            sqlite3_finalize(stmt);
        }
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

User Database::getUserByLogin(const std::string &login) {
    std::lock_guard<std::mutex> lock(db_mutex);

    User user{-1, "", ""};

    std::string sql = "SELECT id, login, password FROM users WHERE login = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return user;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user.id = sqlite3_column_int(stmt, 0);
        const unsigned char *loginText = sqlite3_column_text(stmt, 1);
        const unsigned char *passText = sqlite3_column_text(stmt, 2);

        if (loginText) {
            user.login = reinterpret_cast<const char *>(loginText);
        }

        if (passText) {
            user.password = reinterpret_cast<const char *>(passText);
        }
    }

    sqlite3_finalize(stmt);
    return user;
}

bool Database::userExists(int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "SELECT 1 FROM users "
        "WHERE id = ? "
        "LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    bool exists = sqlite3_step(stmt) == SQLITE_ROW;

    sqlite3_finalize(stmt);
    return exists;
}
