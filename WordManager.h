
#ifndef RAETSEL_SOLVER_WORDMANAGER_H
#define RAETSEL_SOLVER_WORDMANAGER_H
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Utilities.h"


using MultiWord = std::vector<std::string>;
using LetterMatrix = std::unordered_map<size_t, std::vector<char>>;
using Query = std::vector<std::string>;

class WordManager {
public:
    std::vector<bool> non_blanks;
    std::vector<MultiWord> known_words;

    bool finished = false;
    bool next_ordering();
    bool complies(Puzzle &puzzle);
    std::unordered_set<Query> generate_queries(Puzzle &p);
    WordManager(std::vector<MultiWord> &known_words, size_t total_words) :
        known_words(known_words) {
        non_blanks.reserve(total_words);
        for (size_t i = 0; i < known_words.size(); ++i) {
            non_blanks[i] = true;
        }
        index_ranges.reserve(known_words.size());
        for (const auto & known_word : known_words) {
                index_ranges.push_back(known_word.size());
            }
        all_index_combinations = get_all_combinations(index_ranges);
    }
private:
    std::vector<size_t> index_ranges;
    std::vector<std::vector<size_t>> all_index_combinations;
    std::unordered_map<size_t, size_t> words_to_known_words;
    static Query build_query(LetterMatrix &letters, std::unordered_map<size_t, size_t> index_mapping,
                            std::vector<size_t> word_indices, std::unordered_set<size_t> word_breaks);
};

#endif //RAETSEL_SOLVER_WORDMANAGER_H
