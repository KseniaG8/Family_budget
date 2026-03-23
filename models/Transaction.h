#pragma once
#include <string>

struct Transaction {
  int id;
  int user_id;
  std::string type;
  double amount;
  std::string category;
};
