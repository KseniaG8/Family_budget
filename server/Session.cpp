#include "Session.h"
#include <iostream>

Session::Session(tcp::socket socket, UserHandler &userHandler,
                 TransactionHandler &transactionHandler)
    : socket_(std::move(socket)), userHandler_(userHandler),
      transactionHandler_(transactionHandler) {}

void Session::start() { read(); }

void Session::read() {
  auto self = shared_from_this();

  boost::asio::async_read_until(
      socket_, buffer_, "\n",
      [this, self](boost::system::error_code ec, std::size_t) {
        if (!ec) {
          std::istream is(&buffer_);
          std::string line;
          std::getline(is, line);

          process(line);
        }
      });
}

void Session::process(const std::string &line) {
  json response;

  try {
    auto j = json::parse(line);

    if (!j.contains("action")) {
      response = {{"status", "error"}, {"message", "Missing action"}};
    } else if (j["action"] == "register") {
      response = userHandler_.registerUser(j["login"], j["password"]);
    } else if (j["action"] == "login") {
      response = userHandler_.loginUser(j["login"], j["password"]);
    } else if (j["action"] == "add_transaction") {
      std::string category = j.contains("category") ? j["category"] : "";
      response = transactionHandler_.addTransaction(j["user_id"], j["type"],
                                                    j["amount"], category);
    } else if (j["action"] == "get_transactions") {
      response = transactionHandler_.getTransactions(j["user_id"]);
    } else if (j["action"] == "get_balance") {
      response = transactionHandler_.getBalance(j["user_id"]);
    } else if (j["action"] == "get_transactions_by_category") {
      response = transactionHandler_.getTransactionsByCategory(j["user_id"],
                                                               j["category"]);
    } else {
      response = {{"status", "error"}, {"message", "Unknown action"}};
    }

  } catch (json::parse_error &) {
    response = {{"status", "error"}, {"message", "Invalid JSON"}};
  }

  write(response.dump() + "\n");
}

void Session::write(const std::string &response) {
  auto self = shared_from_this();

  boost::asio::async_write(socket_, boost::asio::buffer(response),
                           [this, self](boost::system::error_code ec,
                                        std::size_t) { socket_.close(); });
}
