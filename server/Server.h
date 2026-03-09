#pragma once
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "../handlers/UserHandler.h"
#include "../handlers/TransactionHandler.h"

using json = nlohmann::json;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port,
           UserHandler& userHandler,
           TransactionHandler& transactionHandler);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    UserHandler& userHandler_;
    TransactionHandler& transactionHandler_;
};