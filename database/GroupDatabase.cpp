#include "Database.h"

#include <iostream>

int Database::createGroup(const std::string &name, int owner_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql = "INSERT INTO groups (name, owner_id) VALUES (?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, owner_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);

    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

bool Database::groupExists(int group_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "SELECT 1 FROM groups "
        "WHERE id = ? "
        "LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";

        return false;
    }

    sqlite3_bind_int(stmt, 1, group_id);

    bool exists = sqlite3_step(stmt) == SQLITE_ROW;

    sqlite3_finalize(stmt);

    return exists;
}

bool Database::addUserToGroup(
    int group_id,
    int user_id,
    const std::string &role
) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "INSERT OR IGNORE INTO group_members (group_id, user_id, role) VALUES "
        "(?, ?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, group_id);
    sqlite3_bind_int(stmt, 2, user_id);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

bool Database::isUserInGroup(int group_id, int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "SELECT 1 FROM group_members "
        "WHERE group_id = ? AND user_id = ? "
        "LIMIT 1;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, group_id);
    sqlite3_bind_int(stmt, 2, user_id);

    bool exists = sqlite3_step(stmt) == SQLITE_ROW;

    sqlite3_finalize(stmt);
    return exists;
}

bool Database::removeUserFromGroup(int group_id, int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "DELETE FROM group_members "
        "WHERE group_id = ? AND user_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, group_id);
    sqlite3_bind_int(stmt, 2, user_id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

std::string Database::getUserRoleInGroup(int group_id, int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::string sql =
        "SELECT role FROM group_members "
        "WHERE group_id = ? AND user_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return "";
    }

    sqlite3_bind_int(stmt, 1, group_id);
    sqlite3_bind_int(stmt, 2, user_id);

    std::string role = "";

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *roleText = sqlite3_column_text(stmt, 0);
        role = roleText ? reinterpret_cast<const char *>(roleText) : "";
    }

    sqlite3_finalize(stmt);
    return role;
}

std::vector<Group> Database::getUserGroups(int user_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::vector<Group> result;

    std::string sql =
        "SELECT g.id, g.name, g.owner_id "
        "FROM groups g "
        "JOIN group_members gm ON g.id = gm.group_id "
        "WHERE gm.user_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return result;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Group g;

        g.id = sqlite3_column_int(stmt, 0);

        const unsigned char *nameText = sqlite3_column_text(stmt, 1);
        g.name = nameText ? reinterpret_cast<const char *>(nameText) : "";

        g.owner_id = sqlite3_column_int(stmt, 2);

        result.push_back(g);
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<GroupMember> Database::getGroupMembers(int group_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    std::vector<GroupMember> result;

    std::string sql =
        "SELECT u.id, u.login, gm.role "
        "FROM users u "
        "JOIN group_members gm ON u.id = gm.user_id "
        "WHERE gm.group_id = ?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return result;
    }

    sqlite3_bind_int(stmt, 1, group_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        GroupMember member;

        member.id = sqlite3_column_int(stmt, 0);

        const unsigned char *loginText = sqlite3_column_text(stmt, 1);
        member.login =
            loginText ? reinterpret_cast<const char *>(loginText) : "";

        const unsigned char *roleText = sqlite3_column_text(stmt, 2);
        member.role = roleText ? reinterpret_cast<const char *>(roleText) : "";

        result.push_back(member);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::deleteGroup(int group_id) {
    std::lock_guard<std::mutex> lock(db_mutex);

    char *errMsg = nullptr;

    std::string deleteMembers = "DELETE FROM group_members WHERE group_id = " +
                                std::to_string(group_id) + ";";

    std::string deleteTransactions =
        "DELETE FROM transactions WHERE group_id = " +
        std::to_string(group_id) + ";";

    std::string deleteGroup =
        "DELETE FROM groups WHERE id = " + std::to_string(group_id) + ";";

    if (sqlite3_exec(db, deleteMembers.c_str(), nullptr, nullptr, &errMsg) !=
        SQLITE_OK) {
        std::cerr << "Failed to delete group members: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(
            db, deleteTransactions.c_str(), nullptr, nullptr, &errMsg
        ) != SQLITE_OK) {
        std::cerr << "Failed to delete group transactions: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, deleteGroup.c_str(), nullptr, nullptr, &errMsg) !=
        SQLITE_OK) {
        std::cerr << "Failed to delete group: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}
