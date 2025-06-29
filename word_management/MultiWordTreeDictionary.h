
#ifndef RAETSEL_SOLVER_MULTIWORDTREEDICTIONARY_H
#define RAETSEL_SOLVER_MULTIWORDTREEDICTIONARY_H


#include "Dictionary.h"
#include "TreeDictionary.h"

enum SuffixContained {No, Maybe, Yes};

class MultiWordTreeDictionary : public Dictionary{
public:
    std::unordered_map<std::string, bool> previous_requests;
    TreeDictionary dictionary;

    bool has_word(const WordQuery &query) override;
    bool add_word(const std::string &word) override {
        return dictionary.add_word(word);
    };
    std::vector<std::string> get_words(const WordQuery &query) override;

    explicit MultiWordTreeDictionary(bool print_words): dictionary(print_words) {};
    ~MultiWordTreeDictionary() override = default;

    MultiWordTreeDictionary construct(std::string &filename, std::vector<Filter> &filter) {
        dictionary.construct(filename, filter);
        return *this;
    }

private:
    bool has_word(Node &current_node, const std::string &query, size_t index, std::vector<SuffixContained> &);
    bool get_words(Node &current_node, const std::string &query, size_t index, std::string &&current_path, std::unordered_map<size_t, std::vector<std::string> > &matches_by_suffix, std::vector<SuffixContained> &);
    static std::vector<std::string> transform_suffix_matches(size_t length, std::unordered_map<size_t, std::vector<std::string>> &matches_by_suffix);
};


#endif //RAETSEL_SOLVER_MULTIWORDTREEDICTIONARY_H
