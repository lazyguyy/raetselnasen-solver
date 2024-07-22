
#include "WordManager.h"
#include <algorithm>
#include <unordered_map>

namespace std {
    template <> struct hash<Query>
    {
        size_t operator()(const Query & query) const
        {
            /* your code here, e.g. "return hash<int>()(x.value);" */
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
    if (!finished)
        finished = std::next_permutation(non_blanks.begin(), non_blanks.end());
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
    for (auto &letter_query : p.letter_queries) {
        if (!words_to_known_words.count(letter_query.ordered_words_index))
            continue;
        auto known_word_index = words_to_known_words[letter_query.ordered_words_index];
        auto word = known_words[known_word_index];
        if (letter_query.type == middle && word.size() % 2 != 1)
            return false;
        if (letter_query.letter_index >= word.size())
            return false;
    }
    return true;
}

std::unordered_set<Query> WordManager::generate_queries(Puzzle &p) {
    std::unordered_set<Query> queries;
    // evaluate all rules for the current choice of blanks
    // multiple letters are possible if the MultiWord is an actual MultiWord
    LetterMatrix letters(p.letter_queries.size());
    for (size_t query_index = 0; query_index < p.letter_queries.size(); ++query_index) {
        auto &letter_query = p.letter_queries[query_index];
        auto queried_word = letter_query.ordered_words_index;
        if (words_to_known_words.count(queried_word) != 0) {
            auto &multi_word = known_words[words_to_known_words[queried_word]];
            std::vector<char> results;
            for (auto &word : multi_word) {
                results.push_back(letter_query.get_letter(word));
            }
            letters[query_index] = results;
        }
    }
    for (auto &index_comb : all_index_combinations) {
        queries.insert(build_query(letters, words_to_known_words, index_comb, p.word_breaks));
    }
    return queries;
}

Query WordManager::build_query(LetterMatrix &letters, std::unordered_map<size_t, size_t> index_mapping,
                                     std::vector<size_t> word_indices, std::unordered_set<size_t> word_breaks) {
    std::vector<std::string> queries;
    std::string query;
    for (size_t i = 0; i < letters.size(); ++i) {
        if (index_mapping.count(i) == 0) {
            query += '?';
        } else {
            auto letter_index = word_indices[index_mapping[i]];
            query += letters[i][letter_index];
        }
        if (word_breaks.count(i)) {
            queries.push_back(query);
            query = std::string();
        }
    }
    return queries;

}
