#include "Server.h"
#include "Session.h"
#include <iostream>

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context,
               short port,
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
                std::cout << "Client connected\n";

                std::make_shared<Session>(
                    std::move(socket),
                    userHandler_,
                    transactionHandler_
                )->start();
            }

            do_accept();
        });
}
