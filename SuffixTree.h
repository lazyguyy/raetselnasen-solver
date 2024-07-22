
#ifndef RAETSEL_SOLVER_SUFFIXTREE_H
#define RAETSEL_SOLVER_SUFFIXTREE_H
#include <string>
#include <vector>
#include <map>
struct Node {
    std::map<char, Node> children;
    bool is_endpoint;
};

class SuffixTree {
public:
    bool add_word(const std::string &word);
    bool has_word(const std::string &query);
    std::vector<std::string> get_words(const std::string &query);
private:
    Node root;
    bool add_word(Node &current_node, const std::string &word, size_t index);
    bool has_word(Node &current_node, const std::string &query, size_t index);
    std::vector<std::string> get_words(Node &current_node, const std::string &query, size_t index, std::string &current_path);
};


#endif //RAETSEL_SOLVER_SUFFIXTREE_H
