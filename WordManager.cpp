
#include "WordManager.h"
#include <algorithm>
#include <unordered_map>

namespace std {
    template <> struct hash<Query>
    {
        size_t operator()(const Query & query) const
        {
            size_t value = 0;
            size_t prime = 31;
            for (const auto &word : query) {
                value = hash<std::string>()(word) + prime * value;
            }
            return value;
        }
    };
}

bool WordManager::next_ordering() {
    if (!finished) {
        past_orderings += 1;
        finished = !std::next_permutation(non_blanks.begin(), non_blanks.end());
    }
    return finished;
}

bool WordManager::complies(Puzzle &p) {
    words_to_known_words = std::unordered_map<size_t, size_t>();
    size_t current_word = 0;
    for (size_t i = 0; i < non_blanks.size(); ++i) {
        if (non_blanks[i]) {
            words_to_known_words[i] = current_word;
            current_word++;
        }
    }
    complying_word_lists = std::unordered_set<size_t>();
    for (size_t i = 0; i < collapsed_multi_words.size(); ++i) {
        if (isValidWordList(p, collapsed_multi_words[i]))
            complying_word_lists.insert(i);
    }
    return !complying_word_lists.empty();
}

std::vector<QueryStruct> WordManager::generate_queries(Puzzle &p) {
    std::vector<QueryStruct> queries;
    queries.reserve(complying_word_lists.size());
    for (auto valid : complying_word_lists) {
        queries.push_back({std::move(build_query(collapsed_multi_words[valid], p)), valid});
    }
    return queries;
}

Query WordManager::build_query(WordList &word_list, Puzzle &p) {
    Query queries;
    std::string query;
    for (size_t i = 0; i < p.letter_queries.size(); ++i) {
        auto letter_map = p.letter_queries[i];
        if (words_to_known_words.count(letter_map.ordered_words_index)) {
            query += letter_map.get_letter(*word_list[words_to_known_words[letter_map.ordered_words_index]]);
        } else {
            query += '?';
        }
        if (p.word_breaks.count(i)) {
            queries.push_back(std::move(query));
            query = std::string();
        }
    }
    if (!query.empty())
        queries.push_back(std::move(query));
    return queries;

}

bool WordManager::isValidWordList(Puzzle &p, WordList &word_list) {
    for (auto &letter_query : p.letter_queries) {
        if (!words_to_known_words.count(letter_query.ordered_words_index))
            continue;
        auto known_word_index = words_to_known_words[letter_query.ordered_words_index];
        auto word = word_list[known_word_index];
        if (letter_query.type == middle && (*word).size() % 2 != 1)
            return false;
        if (letter_query.letter_index >= (*word).size())
            return false;
    }
    return true;
}

WordManager::WordManager(std::vector<MultiWord> &known_words, size_t total_words) {
    past_orderings = 0;
    non_blanks = std::vector<bool>(total_words);
    for (size_t i = 0; i < known_words.size(); ++i) {
        non_blanks[i + total_words - known_words.size()] = true;
    }
    collapsed_multi_words = collapse_multi_words(known_words);
    total_possibilities = n_choose_k(total_words, total_words - known_words.size()) * collapsed_multi_words.size();
    for (auto &word_list : collapsed_multi_words) {
        std::sort(word_list.begin(), word_list.end(), [](const SharedWord &w1, const SharedWord &w2) {
            return (get_word_value(*w1) < get_word_value(*w2)) || (get_word_value(*w1) == get_word_value(*w2) && *w1 <= *w2);
        });
        for (auto &word : word_list) {
            std::cout << *word << " (" << get_word_value(*word) << ")" << std::endl;
        }
    }
}
