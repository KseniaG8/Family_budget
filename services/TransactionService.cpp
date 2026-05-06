#include "TransactionService.h"

TransactionService::TransactionService(Database &db) : database(db) {}

std::vector<Transaction> TransactionService::getUserTransactions(int user_id) {
    return database.getTransactionsByUser(user_id);
}

std::vector<Transaction> TransactionService::getTransactionsByCategory(int user_id, const std::string &category) {
    return database.getTransactionsByCategory(user_id, category);
}

void TransactionService::addTransaction(int user_id, std::string type, double amount, std::string category) {
    Transaction t;
    t.user_id = user_id;
    t.type = type;
    t.amount = amount;
    t.category = category;

    database.addTransaction(t);
}

double TransactionService::getBalance(int user_id) { return database.getBalanceByUser(user_id); }

bool TransactionService::updateTransaction(int transaction_id,
                                           const std::string &type,
                                           double amount,
                                           const std::string &category) {
    return database.updateTransaction(transaction_id, type, amount, category);
}

bool TransactionService::deleteTransaction(int transaction_id) { return database.deleteTransaction(transaction_id); }

Transaction TransactionService::getTransactionById(int transaction_id) {
    return database.getTransactionById(transaction_id);
}

bool TransactionService::setLimit(int user_id, const std::string &category, double limit_amount) {
    return database.setLimit(user_id, category, limit_amount);
}

nlohmann::json TransactionService::checkLimit(int user_id, const std::string &category) {
    double limit = database.getLimit(user_id, category);

    if (limit < 0) {
        return {{"status", "error"}, {"message", "Limit not found"}};
    }

    double spent = database.getSpentByCategory(user_id, category);

    return {{"status", "success"},
            {"category", category},
            {"limit", limit},
            {"spent", spent},
            {"remaining", limit - spent},
            {"exceeded", spent > limit}};
}
