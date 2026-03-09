#include "TransactionService.h"

TransactionService::TransactionService(Database& db)
    : database(db) {}

std::vector<Transaction> TransactionService::getUserTransactions(int user_id) {
    return database.getTransactionsByUser(user_id);
}

void TransactionService::addTransaction(int user_id, std::string type, double amount) {
    Transaction t;
    t.user_id = user_id;
    t.type = type;
    t.amount = amount;

    database.addTransaction(t);
}

