
#ifndef RAETSEL_SOLVER_WORDMANAGER_H
#define RAETSEL_SOLVER_WORDMANAGER_H
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <bits/shared_ptr.h>
#include "Utilities.h"

using Query = std::vector<std::string>;
struct QueryStruct {
    Query query;
    size_t origin_index;
};


class WordManager {
public:
    std::vector<bool> non_blanks;
    bool finished = false;
    size_t total_possibilities;
    size_t past_orderings;
    std::vector<WordList> collapsed_multi_words;

    bool next_ordering();
    bool complies(Puzzle &puzzle);
    std::vector<QueryStruct> generate_queries(Puzzle &p);
    bool isValidWordList(Puzzle &, WordList &);
    WordManager(std::vector<MultiWord> &known_words, size_t total_words);
private:
    std::unordered_set<size_t> complying_word_lists;
    std::unordered_map<size_t, size_t> words_to_known_words;
    Query build_query(WordList &, Puzzle &);
};

#endif //RAETSEL_SOLVER_WORDMANAGER_H
