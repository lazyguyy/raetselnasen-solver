
#ifndef RAETSEL_SOLVER_DICTIONARY_H
#define RAETSEL_SOLVER_DICTIONARY_H

#include <string>
#include <vector>
#include "../Utilities.h"

enum dict_type {SingleWordDictionaryType, MultiWordDictionaryType, AnagramDictionaryType};

class Dictionary;

struct WordQuery {
    std::string query_string;
    std::shared_ptr<Dictionary> dictionary;
    size_t unknown_letters;
};

class Dictionary {
public:
    bool print_words;
    Dictionary() = default;
    explicit Dictionary(bool print_words) : print_words(print_words) {}
    virtual ~Dictionary() = default;
    virtual bool add_word(const std::string &word) = 0;
    virtual bool has_word(const WordQuery &query) = 0;
    virtual std::vector<std::string> get_words(const WordQuery &query) = 0;
};


#endif //RAETSEL_SOLVER_DICTIONARY_H
