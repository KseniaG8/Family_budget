#include "Session.h"
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

Session::Session(tcp::socket socket, UserHandler &userHandler, TransactionHandler &transactionHandler)
    : socket_(std::move(socket)), userHandler_(userHandler), transactionHandler_(transactionHandler) {}

void Session::start() { read_request(); }

void Session::read_request() {
    auto self = shared_from_this();

    http::async_read(socket_, buffer_, request_, [self](beast::error_code ec, std::size_t) {
        if (!ec) {
            self->handle_request();
        }
    });
}

void Session::handle_request() {
    try {
        std::string target = std::string(request_.target());

        if (request_.method() == http::verb::post && target == "/register") {
            json body = json::parse(request_.body());

            auto result = userHandler_.registerUser(body["login"], body["password"]);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::post && target == "/login") {
            json body = json::parse(request_.body());

            auto result = userHandler_.loginUser(body["login"], body["password"]);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::post && target == "/transactions") {
            json body = json::parse(request_.body());

            std::string category = body.contains("category") ? body["category"].get<std::string>() : "";

            auto result = transactionHandler_.addTransaction(body["user_id"], body["type"], body["amount"], category);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::get && target.find("/transactions?") == 0) {
            int user_id = std::stoi(get_query_param(target, "user_id"));

            auto result = transactionHandler_.getTransactions(user_id);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::get && target.find("/balance?") == 0) {
            int user_id = std::stoi(get_query_param(target, "user_id"));

            auto result = transactionHandler_.getBalance(user_id);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::get && target.find("/transactions/category?") == 0) {
            int user_id = std::stoi(get_query_param(target, "user_id"));
            std::string category = get_query_param(target, "category");

            auto result = transactionHandler_.getTransactionsByCategory(user_id, category);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::put && target == "/transactions") {
            json body = json::parse(request_.body());

            auto result = transactionHandler_.updateTransaction(
                body["transaction_id"], body["type"], body["amount"], body["category"]);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::delete_ && target.find("/transactions?") == 0) {
            int transaction_id = std::stoi(get_query_param(target, "id"));

            auto result = transactionHandler_.deleteTransaction(transaction_id);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::get && target.find("/transaction?") == 0) {
            int transaction_id = std::stoi(get_query_param(target, "id"));

            auto result = transactionHandler_.getTransactionById(transaction_id);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::post && target == "/limits") {
            json body = json::parse(request_.body());

            auto result = transactionHandler_.setLimit(body["user_id"], body["category"], body["limit"]);

            send_response(result);
            return;
        }

        if (request_.method() == http::verb::get && target.find("/limits/check?") == 0) {
            int user_id = std::stoi(get_query_param(target, "user_id"));
            std::string category = get_query_param(target, "category");

            auto result = transactionHandler_.checkLimit(user_id, category);

            send_response(result);
            return;
        }

        send_response({{"status", "error"}, {"message", "Unknown endpoint"}}, http::status::not_found);

    } catch (const std::exception &e) {
        send_response({{"status", "error"}, {"message", e.what()}}, http::status::bad_request);
    }
}

void Session::send_response(const json &body, http::status status) {
    auto self = shared_from_this();

    response_.version(request_.version());
    response_.result(status);
    response_.set(http::field::server, "FamilyBudgetServer");
    response_.set(http::field::content_type, "application/json");
    response_.body() = body.dump();
    response_.prepare_payload();

    http::async_write(socket_, response_, [self](beast::error_code ec, std::size_t) {
        self->socket_.shutdown(tcp::socket::shutdown_send, ec);
    });
}

std::string Session::get_query_param(const std::string &target, const std::string &key) {
    auto question_pos = target.find('?');
    if (question_pos == std::string::npos) {
        return "";
    }

    std::string query = target.substr(question_pos + 1);
    std::string search = key + "=";

    auto key_pos = query.find(search);
    if (key_pos == std::string::npos) {
        return "";
    }

    auto value_start = key_pos + search.length();
    auto value_end = query.find('&', value_start);

    if (value_end == std::string::npos) {
        return query.substr(value_start);
    }

    return query.substr(value_start, value_end - value_start);
}
