#pragma once

#include "../handlers/TransactionHandler.h"
#include "../handlers/UserHandler.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, UserHandler &userHandler, TransactionHandler &transactionHandler);

    void start();

private:
    void read_request();
    void handle_request();
    void send_response(const nlohmann::json &body, boost::beast::http::status status = boost::beast::http::status::ok);

    std::string get_query_param(const std::string &target, const std::string &key);

    boost::asio::ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_;

    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;

    UserHandler &userHandler_;
    TransactionHandler &transactionHandler_;
};
