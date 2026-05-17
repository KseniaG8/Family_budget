#pragma once
#include "../handlers/TransactionHandler.h"
#include "../handlers/UserHandler.h"
#include <boost/asio.hpp>

class Server {
public:
    Server(boost::asio::io_context &io_context,
           short port,
           UserHandler &userHandler,
           TransactionHandler &transactionHandler);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    UserHandler &userHandler_;
    TransactionHandler &transactionHandler_;
};
