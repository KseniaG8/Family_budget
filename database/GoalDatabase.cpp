#include "Database.h"

#include <iostream>

bool Database::addGoal(
    int user_id,
    const std::string &name,
    double target_amount,
    const std::string &deadline
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "INSERT INTO goals (user_id, name, target_amount, current_amount, "
        "deadline) "
        "VALUES (?, ?, ?, 0, ?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, target_amount);
    sqlite3_bind_text(stmt, 4, deadline.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

std::vector<Goal> Database::getGoalsByUser(int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::vector<Goal> result;

    std::string sql =
        "SELECT id, user_id, name, target_amount, current_amount, deadline "
        "FROM goals WHERE user_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return result;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Goal g;

        g.id = sqlite3_column_int(stmt, 0);
        g.user_id = sqlite3_column_int(stmt, 1);

        const unsigned char *nameText = sqlite3_column_text(stmt, 2);
        g.name = nameText ? reinterpret_cast<const char *>(nameText) : "";

        g.target_amount = sqlite3_column_double(stmt, 3);
        g.current_amount = sqlite3_column_double(stmt, 4);

        const unsigned char *deadlineText = sqlite3_column_text(stmt, 5);
        g.deadline =
            deadlineText ? reinterpret_cast<const char *>(deadlineText) : "";

        result.push_back(g);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::updateGoalProgress(int goal_id, double current_amount) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql = "UPDATE goals SET current_amount = ? WHERE id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_double(stmt, 1, current_amount);
    sqlite3_bind_int(stmt, 2, goal_id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

bool Database::updateGoal(
    int goal_id,
    const std::string &name,
    double target_amount,
    const std::string &deadline
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "UPDATE goals "
        "SET name = ?, target_amount = ?, deadline = ? "
        "WHERE id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, target_amount);
    sqlite3_bind_text(stmt, 3, deadline.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, goal_id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

bool Database::deleteGoal(int goal_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql = "DELETE FROM goals WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, goal_id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}
