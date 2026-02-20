#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "User.h"
#include <vector>
#include <fstream>
#include <sstream>

class UserManager {
private:
    std::vector<User> users;
    User* currentUser = nullptr;

public:
    void loadUsers() {
        std::ifstream file("users.txt");
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string username, password;

            std::getline(ss, username, ',');
            std::getline(ss, password, ',');

            User user;
            user.username = username;
            user.password = password;

            users.push_back(user);
        }
    }

    void saveUsers() {
        std::ofstream file("users.txt");
        for (const auto& user : users) {
            file << user.username << ","
                 << user.password << "\n";
        }
    }

    bool registerUser(const std::string& username, const std::string& password) {
        for (const auto& u : users)
            if (u.username == username)
                return false;

        User newUser;
        newUser.username = username;
        newUser.password = password;
        users.push_back(newUser);

        saveUsers();
        return true;
    }

    bool login(const std::string& username, const std::string& password) {
        for (auto& u : users) {
            if (u.username == username && u.password == password) {
                currentUser = &u;
                currentUser->budget.loadFromFile("data_" + username + ".txt");
                return true;
            }
        }
        return false;
    }

    User* getCurrentUser() {
        return currentUser;
    }
};

#endif
