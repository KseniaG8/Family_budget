#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

enum class Type {
    Income,
    Expense
};

struct Transaction {
    Type type;
    double amount;
    std::string category;
    std::string date;
};

#endif
