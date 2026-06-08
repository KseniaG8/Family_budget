#include "Database.h"

#include <iostream>

bool Database::setLimit(
    int user_id,
    const std::string &category,
    double limit_amount,
    const std::string &period
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "INSERT INTO limits (user_id, category, limit_amount, period) "
        "VALUES (?, ?, ?, ?) "
        "ON CONFLICT(user_id, category, period) "
        "DO UPDATE SET limit_amount = excluded.limit_amount;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, limit_amount);
    sqlite3_bind_text(stmt, 4, period.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

double Database::getLimit(
    int user_id,
    const std::string &category,
    const std::string &period
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    double limit = -1.0;

    std::string sql =
        "SELECT limit_amount FROM limits "
        "WHERE user_id = ? AND category = ? AND period = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return limit;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, period.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        limit = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return limit;
}

double Database::getSpentByCategory(
    int user_id,
    const std::string &category,
    const std::string &period
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    double spent = 0.0;

    std::string dateCondition;

    if (period == "daily") {
        dateCondition = "date(created_at) = date('now')";
    } else if (period == "weekly") {
        dateCondition = "date(created_at) >= date('now', '-6 days')";
    } else if (period == "monthly") {
        dateCondition = "date(created_at) >= date('now', 'start of month')";
    } else {
        return spent;
    }

    std::string sql =
        "SELECT amount FROM transactions "
        "WHERE user_id = ? "
        "AND category = ? "
        "AND type = 'expense' "
        "AND group_id IS NULL "
        "AND " +
        dateCondition + ";";

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
