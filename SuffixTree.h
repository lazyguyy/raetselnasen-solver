
#ifndef RAETSEL_SOLVER_SUFFIXTREE_H
#define RAETSEL_SOLVER_SUFFIXTREE_H
#include <string>
#include <vector>
#include <map>
#include <unordered_set>

struct Node {
    std::map<char, Node> children;
    bool is_endpoint;
};

class SuffixTree {
public:
    bool add_word(const std::string &word);
    bool has_word(const std::string &query);
    std::unordered_set<std::string> get_words(const std::string &query);
    bool allow_combinations;
    SuffixTree(bool allow_combinations) : allow_combinations(allow_combinations) {};
private:
    Node root;
    bool add_word(Node &current_node, const std::string &word, size_t index);
    bool has_word(Node &current_node, const std::string &query, size_t index);
    void get_words(Node &current_node, const std::string &query, size_t index, std::string &current_path, std::unordered_set<std::string> &);
};


#endif //RAETSEL_SOLVER_SUFFIXTREE_H
