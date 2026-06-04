#include "Database.h"

#include <iostream>

void Database::addTransaction(const Transaction &t) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "INSERT INTO transactions (user_id, type, amount, "
        "category, created_at) VALUES (?, ?, ?, ?, datetime('now'));";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_bind_int(stmt, 1, t.user_id);
    sqlite3_bind_text(stmt, 2, t.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, t.amount);
    sqlite3_bind_text(stmt, 4, t.category.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert transaction failed: " << sqlite3_errmsg(db)
                  << "\n";
    }

    sqlite3_finalize(stmt);
}

std::vector<Transaction> Database::getTransactionsByUser(int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::vector<Transaction> result;

    std::string sql =
        "SELECT id, user_id, group_id, type, amount, category "
        "FROM transactions "
        "WHERE user_id = ? AND group_id IS NULL;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return result;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction t;

        t.id = sqlite3_column_int(stmt, 0);
        t.user_id = sqlite3_column_int(stmt, 1);
        t.group_id = sqlite3_column_int(stmt, 2);

        const unsigned char *typeText = sqlite3_column_text(stmt, 3);
        if (typeText) {
            t.type = reinterpret_cast<const char *>(typeText);
        }

        t.amount = sqlite3_column_double(stmt, 4);

        const unsigned char *categoryText = sqlite3_column_text(stmt, 5);
        if (categoryText) {
            t.category = reinterpret_cast<const char *>(categoryText);
        }

        result.push_back(t);
    }

    sqlite3_finalize(stmt);
    return result;
}

void Database::clearTransactions() {
    std::lock_guard<std::mutex> lock(db_mutex);

    const char *sql = "DELETE FROM transactions;";
    char *errMsg = nullptr;

    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to clear table: "
                  << (errMsg ? errMsg : "unknown error") << std::endl;

        if (errMsg) {
            sqlite3_free(errMsg);
        }
    }
}

double Database::getBalanceByUser(int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    double balance = 0.0;

    std::string sql =
        "SELECT type, amount FROM transactions "
        "WHERE user_id = ? AND group_id IS NULL;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return balance;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *typeText = sqlite3_column_text(stmt, 0);
        double amount = sqlite3_column_double(stmt, 1);

        std::string type;
        if (typeText) {
            type = reinterpret_cast<const char *>(typeText);
        }

        if (type == "income" || type == "Income") {
            balance += amount;
        } else if (type == "expense" || type == "Expense") {
            balance -= amount;
        }
    }

    sqlite3_finalize(stmt);
    return balance;
}

std::vector<Transaction>
Database::getTransactionsByCategory(int user_id, const std::string &category) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::vector<Transaction> result;

    std::string sql =
        "SELECT id, user_id, group_id, type, amount, category "
        "FROM transactions "
        "WHERE user_id = ? AND category = ? AND group_id IS NULL;";
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
        t.group_id = sqlite3_column_int(stmt, 2);
        t.type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        t.amount = sqlite3_column_double(stmt, 4);

        const unsigned char *catText = sqlite3_column_text(stmt, 5);
        t.category = catText ? reinterpret_cast<const char *>(catText) : "";

        result.push_back(t);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::updateTransaction(
    int transaction_id,
    const std::string &type,
    double amount,
    const std::string &category
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "UPDATE transactions "
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
    std::lock_guard<std::mutex> lock(db_mutex);

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
    std::lock_guard<std::mutex> lock(db_mutex);

    Transaction t;
    t.id = -1;

    std::string sql =
        "SELECT id, user_id, type, amount, category "
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
        t.category =
            categoryText ? reinterpret_cast<const char *>(categoryText) : "";
    }

    sqlite3_finalize(stmt);
    return t;
}

bool Database::addGroupTransaction(
    int group_id,
    int user_id,
    const std::string &type,
    double amount,
    const std::string &category
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "INSERT INTO transactions (user_id, group_id, type, amount, category, "
        "created_at) "
        "VALUES (?, ?, ?, ?, ?, datetime('now'));";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, group_id);
    sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, amount);
    sqlite3_bind_text(stmt, 5, category.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

std::vector<Transaction> Database::getGroupTransactions(int group_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::vector<Transaction> result;

    std::string sql =
        "SELECT id, user_id, group_id, type, amount, category "
        "FROM transactions WHERE group_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return result;
    }

    sqlite3_bind_int(stmt, 1, group_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transaction t;

        t.id = sqlite3_column_int(stmt, 0);
        t.user_id = sqlite3_column_int(stmt, 1);
        t.group_id = sqlite3_column_int(stmt, 2);

        const unsigned char *typeText = sqlite3_column_text(stmt, 3);
        t.type = typeText ? reinterpret_cast<const char *>(typeText) : "";

        t.amount = sqlite3_column_double(stmt, 4);

        const unsigned char *categoryText = sqlite3_column_text(stmt, 5);
        t.category =
            categoryText ? reinterpret_cast<const char *>(categoryText) : "";

        result.push_back(t);
    }

    sqlite3_finalize(stmt);
    return result;
}

double Database::getGroupBalance(int group_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    double balance = 0.0;

    std::string sql =
        "SELECT type, amount FROM transactions WHERE group_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return balance;
    }

    sqlite3_bind_int(stmt, 1, group_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *typeText = sqlite3_column_text(stmt, 0);
        std::string type =
            typeText ? reinterpret_cast<const char *>(typeText) : "";

        double amount = sqlite3_column_double(stmt, 1);

        if (type == "income" || type == "Income") {
            balance += amount;
        } else if (type == "expense" || type == "Expense") {
            balance -= amount;
        }
    }

    sqlite3_finalize(stmt);
    return balance;
}
