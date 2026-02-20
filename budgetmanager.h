#ifndef BUDGET_MANAGER_H
#define BUDGET_MANAGER_H

#include "Transaction.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class BudgetManager {
private:
    std::vector<Transaction> transactions;

public:
    void addTransaction(const Transaction& t) {
        transactions.push_back(t);
    }

    void showTransactions() const {
        for (const auto& t : transactions) {
            std::cout << (t.type == Type::Income ? "[Доход] " : "[Расход] ")
                      << t.amount << " | "
                      << t.category << " | "
                      << t.date << "\n";
        }
    }

    double calculateBalance() const {
        double balance = 0.0;
        for (const auto& t : transactions) {
            if (t.type == Type::Income)
                balance += t.amount;
            else
                balance -= t.amount;
        }
        return balance;
    }

    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        for (const auto& t : transactions) {
            file << (t.type == Type::Income ? "Income" : "Expense") << ","
                 << t.amount << ","
                 << t.category << ","
                 << t.date << "\n";
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;

        transactions.clear();
        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string typeStr, amountStr, category, date;

            std::getline(ss, typeStr, ',');
            std::getline(ss, amountStr, ',');
            std::getline(ss, category, ',');
            std::getline(ss, date, ',');

            Transaction t;
            t.type = (typeStr == "Income") ? Type::Income : Type::Expense;
            t.amount = std::stod(amountStr);
            t.category = category;
            t.date = date;

            transactions.push_back(t);
        }
    }
};

#endif
