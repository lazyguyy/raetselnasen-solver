
#ifndef RAETSEL_SOLVER_PUZZLE_H
#define RAETSEL_SOLVER_PUZZLE_H
#include <utility>
#include <vector>
#include "Utilities.h"
#include "word_management/Dictionary.h"


struct Word {
    std::vector<LetterMap> letters;
    std::vector<Filter> word_filters;
    std::shared_ptr<Dictionary> dictionary;

    Word(std::vector<LetterMap> &letters, std::vector<Filter> &word_filters, std::shared_ptr<Dictionary> d):
            letters(letters), word_filters(word_filters), dictionary(std::move(d)) {}

};


class Puzzle {
public:
    std::vector<Word> words;
    size_t min_matches;
    size_t total_words;
    size_t detail_level;
    bool show_matches;
};


#endif //RAETSEL_SOLVER_PUZZLE_H
