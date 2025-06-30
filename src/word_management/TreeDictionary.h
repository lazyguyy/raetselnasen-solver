
#ifndef RAETSEL_SOLVER_TREEDICTIONARY_H
#define RAETSEL_SOLVER_TREEDICTIONARY_H
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include "../Utilities.h"
#include "Dictionary.h"


struct Node {
    std::unordered_map<char, Node> children;
    bool is_endpoint;
    size_t depth;
};

class TreeDictionary : public Dictionary {
public:
    Node root;

    explicit TreeDictionary(bool printWords) : Dictionary(printWords) {
        root.depth = 0;
        root.is_endpoint = false;
    };
    ~TreeDictionary() override = default;

    bool add_word(const std::string &word) override;
    bool has_word(const WordQuery &query) override;
    std::vector<std::string> get_words(const WordQuery &query) override;
    TreeDictionary construct(const std::string &filename, std::vector<Filter> &filter);

private:
    bool add_word(Node &current_node, const std::string &word, size_t index);
    bool has_word(Node &current_node, const std::string &query, size_t index);
    void get_words(Node &current_node, const std::string &query, std::string &&current_path, std::vector<std::string> &matches);
};


#endif //RAETSEL_SOLVER_TREEDICTIONARY_H
