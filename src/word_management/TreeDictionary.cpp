
#include "TreeDictionary.h"
#include <locale>
#include <iostream>

TreeDictionary TreeDictionary::construct(const std::string &filename, std::vector<Filter> &filter) {
    std::ifstream word_file(filename);
    std::string line;
    size_t total_words = 0;
    while (std::getline(word_file, line)) {
        if (std::all_of(filter.begin(), filter.end(), [&line](Filter &f) {return f(line);})) {
            total_words += add_word(line);
        }
    }
    std::cout << "A tree dictionary containing " << total_words << " words has been constructed." << std::endl;
    return *this;
}


bool TreeDictionary::add_word(const std::string &word) {
    if (word.empty())
        return false;
    if (print_words)
        std::cout << word << std::endl;
    return add_word(root, word, 0);
}
bool TreeDictionary::add_word(Node &node, const std::string &word, size_t index) {
    if (index == word.size()) {
        bool is_new_word = !node.is_endpoint;
        node.is_endpoint = true;
        return is_new_word;
    }
    char letter = std::tolower(word[index], std::locale());

    if (node.children.count(letter) == 0){
        node.children[letter] = std::make_shared<Node>(node.depth + 1);
    }
    return add_word(*node.children[letter], word, index + 1);
}

bool TreeDictionary::has_word(const WordQuery &query) {
    return has_word(root, query.query_string, 0);
}

bool TreeDictionary::has_word(Node &current_node, const std::string &query, size_t index) {
    if (index == query.size()) {
        return current_node.is_endpoint;
    }
    auto letter = std::tolower(query[index], std::locale());
    if (letter == '?') {
        for (auto &child_pair: current_node.children) {
            if (has_word(*child_pair.second, query, index + 1))
                return true;
        }
    } else if (current_node.children.count(letter) > 0){
        return has_word(*current_node.children[letter], query, index + 1);
    }
    return false;
}

std::vector<std::string> TreeDictionary::get_words(const WordQuery &query) {
    std::vector<std::string> matches;
    get_words(root, query.query_string, "", matches);
    return matches;
}

void TreeDictionary::get_words(Node &current_node, const std::string &query, std::string &&current_path, std::vector<std::string> &matches) {
    size_t index = current_path.size();
    if (index == query.size()) {
        if (current_node.is_endpoint) {
            matches.push_back(current_path);
        }
    }

    auto letter = std::tolower(query[index], std::locale());
    if (letter == '?') {
        for (auto &child_pair: current_node.children) {
            get_words(*child_pair.second, query, current_path + child_pair.first, matches);
        }
    } else if (current_node.children.count(letter) > 0){
        get_words(*current_node.children[letter], query, current_path + letter, matches);
    }
}


