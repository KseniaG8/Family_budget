#pragma once
#include <string>
#include <vector>
#include <map>
#include "../models/Transaction.h"

class MLCategorizer {
public:
    void train(const std::vector<Transaction>& history);
    std::string predictCategory(const std::string& description);

private:
    std::vector<std::string> tokenize(const std::string& text);

    std::map<std::string, int> category_counts; 
    std::map<std::string, std::map<std::string, int>> word_category_counts; 
    std::map<std::string, int> total_words_in_category;
    int total_transactions = 0;
};
