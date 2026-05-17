#pragma once
#include <string>

struct Goal {
    int id;
    int family_id; 
    std::string name;
    double target_amount;
    double current_amount;
};
