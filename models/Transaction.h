#pragma once
#include <string>

struct Transaction {
  int id;
  int user_id;
  int group_id = -1;
  std::string type;
  double amount;
  std::string category;
  std::string currency;    
  std::string description; 
};
