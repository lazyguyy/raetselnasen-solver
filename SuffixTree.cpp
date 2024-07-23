
#include "SuffixTree.h"
#include <locale>

bool SuffixTree::add_word(const std::string &word) {
    return add_word(root, word, 0);
}
bool SuffixTree::add_word(Node &node, const std::string &word, size_t index) {
    if (index == word.size()) {
        bool is_new_word = !node.is_endpoint;
        node.is_endpoint = true;
        return is_new_word;
    }
    char letter = std::tolower(word[index], std::locale());

    if (node.children.count(letter) == 0){
        auto new_node = Node();
        node.children[letter] = new_node;
    }
    return add_word(node.children[letter], word, index + 1);
}

bool SuffixTree::has_word(const std::string &query) {
    return has_word(root, query, 0);
}

bool SuffixTree::has_word(Node &current_node, const std::string &query, size_t index) {
    if (index == query.size()) {
        return current_node.is_endpoint;
    }
    if (allow_combinations && current_node.is_endpoint && has_word(root, query, index)) {
        return true;
    }
    auto letter = std::tolower(query[index], std::locale());
    if (letter == '?') {
        for (auto &child_pair: current_node.children) {
            if (has_word(child_pair.second, query, index + 1))
                return true;
        }
    } else if (current_node.children.count(letter) > 0){
        return has_word(current_node.children[letter], query, index + 1);
    }
    return false;
}

std::unordered_set<std::string> SuffixTree::get_words(const std::string &query) {
    std::string offset;
    std::unordered_set<std::string> solutions;
    get_words(root, query, 0, offset, solutions);
    return solutions;
}

void SuffixTree::get_words(Node &current_node, const std::string &query, size_t index, std::string &current_path, std::unordered_set<std::string> &matches) {
    if (index == query.size()) {
        if (current_node.is_endpoint)
            matches.insert(current_path);
        return;
    }
    if (allow_combinations && current_node.is_endpoint) {
        std::string copied_path(current_path);
        get_words(root, query, index, copied_path, matches);
    }
    auto letter = std::tolower(query[index], std::locale());
    auto new_offset = current_path + letter;
    if (letter == '?') {
        for (auto &child_pair: current_node.children) {
            new_offset[index] = child_pair.first;
            get_words(child_pair.second, query, index + 1, new_offset, matches);
        }
        return;
    } else if (current_node.children.count(letter) > 0){
        get_words(current_node.children[letter], query, index + 1, new_offset, matches);
    }
    return;
}
