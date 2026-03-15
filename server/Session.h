#pragma once
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "../handlers/UserHandler.h"
#include "../handlers/TransactionHandler.h"

using boost::asio::ip::tcp;
using json = nlohmann::json;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket,
            UserHandler& userHandler,
            TransactionHandler& transactionHandler);

    void start();

private:
    void read();
    void process(const std::string& line);
    void write(const std::string& response);

    tcp::socket socket_;
    boost::asio::streambuf buffer_;

    UserHandler& userHandler_;
    TransactionHandler& transactionHandler_;
};
