#include "TransactionHandler.h"

TransactionHandler::TransactionHandler(TransactionService &service)
    : service(service) {}

nlohmann::json TransactionHandler::getTransactions(int user_id) {
  auto transactions = service.getUserTransactions(user_id);

  nlohmann::json response = nlohmann::json::array();

  for (auto &t : transactions) {
    response.push_back({{"id", t.id},
                        {"type", t.type},
                        {"amount", t.amount},
                        {"category", t.category}});
  }

  return response;
}

nlohmann::json
TransactionHandler::getTransactionsByCategory(int user_id,
                                              const std::string &category) {
  auto transactions = service.getTransactionsByCategory(user_id, category);

  nlohmann::json response = nlohmann::json::array();

  for (auto &t : transactions) {
    response.push_back({{"id", t.id},
                        {"type", t.type},
                        {"amount", t.amount},
                        {"category", t.category}});
  }

  return response;
}

nlohmann::json TransactionHandler::addTransaction(int user_id, std::string type,
                                                  double amount, std::string category, 
                                                  std::string currency, 
                                                  std::string description) {
  service.addTransaction(user_id, type, amount, category, currency, description);
  return {{"status", "success"}};
}

nlohmann::json TransactionHandler::getBalance(int user_id) {
  double balance = service.getBalance(user_id);

  return {{"status", "success"}, {"balance", balance}};
}
