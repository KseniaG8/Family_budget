#ifndef USER_H
#define USER_H

#include <string>
#include "BudgetManager.h"

struct User {
    std::string username;
    std::string password;
    BudgetManager budget;
};

#endif
