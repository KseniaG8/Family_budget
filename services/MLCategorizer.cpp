#include "MLCategorizer.h"
#include <sstream>
#include <cmath>
#include <spdlog/spdlog.h>

std::vector<std::string> MLCategorizer::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream stream(text);
    std::string word;
    while (stream >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

void MLCategorizer::train(const std::vector<Transaction>& history) {
    category_counts.clear();
    word_category_counts.clear();
    total_words_in_category.clear();
    total_transactions = 0;

    for (const auto& t : history) {
        if (t.category.empty() || t.description.empty() || t.category == "Разное") continue;
        
        category_counts[t.category]++;
        total_transactions++;
        
        auto words = tokenize(t.description);
        for (const auto& word : words) {
            word_category_counts[t.category][word]++;
            total_words_in_category[t.category]++;
        }
    }
    spdlog::info("[ML] Trained on {} labeled transactions", total_transactions);
}

std::string MLCategorizer::predictCategory(const std::string& description) {
    if (category_counts.empty() || total_transactions == 0) return "Разное"; 

    auto words = tokenize(description);
    if (words.empty()) return "Разное";

    std::string best_category = "Разное";
    double max_score = -1e9; 

    for (const auto& [category, count] : category_counts) {

        double score = std::log((double)count / total_transactions);

        for (const auto& word : words) {
            int word_count = word_category_counts[category][word];
            int vocab_size = 10000; 

            double p_word = (double)(word_count + 1) / (total_words_in_category[category] + vocab_size);
            score += std::log(p_word);
        }

        if (score > max_score) {
            max_score = score;
            best_category = category;
        }
    }
    
    return best_category;
}
