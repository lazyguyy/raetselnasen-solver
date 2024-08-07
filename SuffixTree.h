
#ifndef RAETSEL_SOLVER_SUFFIXTREE_H
#define RAETSEL_SOLVER_SUFFIXTREE_H
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


enum SuffixContained {No, Maybe, Yes};

struct Node {
    std::unordered_map<char, Node> children;
    bool is_endpoint;
    size_t depth;
};

class SuffixTree {
public:
    bool allow_combinations;
    size_t min_length;
    std::unordered_map<std::string, bool> previous_requests;
    bool add_word(const std::string &word);
    bool has_word(const std::string &query);
    std::unordered_set<std::string> get_words(const std::string &query);
    SuffixTree(bool allow_combinations) : allow_combinations(allow_combinations) {
        min_length = 4;
        root.depth = 0;
    };

    bool dynamic_has_word(const std::string &query);

private:
    Node root;
    bool add_word(Node &current_node, const std::string &word, size_t index);
    bool has_word(Node &current_node, const std::string &query, size_t index);
    void get_words(Node &current_node, const std::string &query, size_t index, std::string &current_path, std::unordered_set<std::string> &);

    bool dynamic_has_word(Node &current_node, const std::string &query, size_t index, std::vector<SuffixContained> &);
};


#endif //RAETSEL_SOLVER_SUFFIXTREE_H
