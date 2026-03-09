#include "Server.h"
#include <iostream>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context, short port,
               UserHandler& userHandler,
               TransactionHandler& transactionHandler)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      userHandler_(userHandler),
      transactionHandler_(transactionHandler)
{
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "Client connected!\n";

                boost::asio::streambuf buf;
                boost::asio::read_until(socket, buf, "\n");

                std::istream is(&buf);
                std::string line;
                std::getline(is, line);

                try {
                    auto j = json::parse(line);
                    json response;

                    if (j.contains("action")) {
                        if (j["action"] == "register") {
                            response = userHandler_.registerUser(j["login"], j["password"]);
                        } else if (j["action"] == "login") {
                            response = userHandler_.loginUser(j["login"], j["password"]);
                        } else if (j["action"] == "add_transaction") {
                            response = transactionHandler_.addTransaction(
                                j["user_id"], j["type"], j["amount"]
                            );
                        } else if (j["action"] == "get_transactions") {
                            response = transactionHandler_.getTransactions(j["user_id"]);
                        }
                    }

                    std::string respStr = response.dump() + "\n";
                    boost::asio::write(socket, boost::asio::buffer(respStr));
                } catch (std::exception& e) {
                    std::cerr << "Error parsing JSON: " << e.what() << "\n";
                }

                try {
                    auto j = json::parse(line);
                    json response;

                    if (!j.contains("action")) {
                        response = { {"status", "error"}, {"message", "Missing action"} };
                    } else { }

                    std::string respStr = response.dump() + "\n";
                    boost::asio::write(socket, boost::asio::buffer(respStr));
                } catch (json::parse_error& e) {
                    json errorResp = { {"status", "error"}, {"message", "Invalid JSON"} };
                    boost::asio::write(socket, boost::asio::buffer(errorResp.dump() + "\n"));
                }
            }

            do_accept();
        });
}