
#ifndef RAETSEL_SOLVER_ANAGRAMDICTIONARY_H
#define RAETSEL_SOLVER_ANAGRAMDICTIONARY_H

#include <fstream>
#include "Dictionary.h"
#include "TreeDictionary.h"

class AnagramDictionary : public Dictionary {
public:
    TreeDictionary anagrams;
    std::unordered_map<std::string, std::vector<std::string>> anagram_to_words;

    AnagramDictionary construct(const std::string &filename, std::vector<Filter> &filters);
    explicit AnagramDictionary(bool print_words): anagrams(print_words) {};
    ~AnagramDictionary() override = default;
    bool add_word(const std::string &word) override;
    bool has_word(const WordQuery &query) override;
    std::vector<std::string> get_words(const WordQuery &query) override;
private:

    bool has_word(const Node &, const std::string &, size_t index, size_t blanks);
};


#endif //RAETSEL_SOLVER_ANAGRAMDICTIONARY_H
