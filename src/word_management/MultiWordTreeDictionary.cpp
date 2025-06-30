
#include "MultiWordTreeDictionary.h"

bool MultiWordTreeDictionary::has_word(const WordQuery &query) {
    if (previous_requests.count(query.query_string))
        return previous_requests[query.query_string];
    std::vector<SuffixContained> suffix_contained(query.query_string.size(), Maybe);
    auto result = has_word(dictionary.root, query.query_string, 0, suffix_contained);
    previous_requests[query.query_string] = result;
    return result;
}

bool MultiWordTreeDictionary::has_word(Node &current_node, const std::string &query, size_t index, std::vector<SuffixContained> &contained_suffixes) {
    if (index == query.size()) {
        return current_node.is_endpoint;
    }
    if (current_node.is_endpoint) {
        if (contained_suffixes[index] == Maybe) {
            bool contained = has_word(dictionary.root, query, index, contained_suffixes);
            contained_suffixes[index] = contained ? Yes : No;
        }
        return contained_suffixes[index] == Yes;
    }
    auto letter = std::tolower(query[index], std::locale());
    if (letter == '?') {
        for (auto &child_pair: current_node.children) {
            if (has_word(child_pair.second, query, index + 1, contained_suffixes))
                return true;
        }
    } else if (current_node.children.count(letter) > 0){
        return has_word(current_node.children[letter], query, index + 1, contained_suffixes);
    }
    return false;
}


std::vector<std::string> MultiWordTreeDictionary::get_words(const WordQuery &query) {
    std::unordered_map<size_t, std::vector<std::string> > matches_by_suffix;
    std::vector<SuffixContained> suffix_contained(query.query_string.size(), Maybe);
    get_words(dictionary.root, query.query_string, 0, "", matches_by_suffix, suffix_contained);
    return transform_suffix_matches(query.query_string.size(), matches_by_suffix);
}

bool MultiWordTreeDictionary::get_words(Node &current_node, const std::string &query, size_t index, std::string &&current_path, std::unordered_map<size_t, std::vector<std::string> > &matches_by_suffix, std::vector<SuffixContained> &suffix_contained) {
    if (index == query.size()) {
        if (current_node.is_endpoint){
            matches_by_suffix[index - current_path.size()].push_back(current_path);
        }
        return current_node.is_endpoint;
    }
    // jump back to root
    if (current_node.is_endpoint) {
        if (suffix_contained[index] == Maybe) {
            bool contained = get_words(dictionary.root, query, index, "", matches_by_suffix, suffix_contained);
            suffix_contained[index] = contained ? Yes : No;
        }
        if (suffix_contained[index] == Yes) {
            matches_by_suffix[index - current_path.size()].push_back(current_path);
        }
    }
    auto letter = std::tolower(query[index], std::locale());
    bool has_word = false;
    if (letter == '?') {
        for (auto &child_pair: current_node.children) {
            has_word |= get_words(child_pair.second, query, index + 1, current_path + child_pair.first, matches_by_suffix, suffix_contained);
        }
    } else if (current_node.children.count(letter) > 0){
        has_word |= get_words(current_node.children[letter], query, index + 1, current_path + letter, matches_by_suffix, suffix_contained);
    }
    return has_word;
}

std::vector<std::string> MultiWordTreeDictionary::transform_suffix_matches(size_t length, std::unordered_map<size_t, std::vector<std::string>> &matches_by_suffix) {
    std::vector<std::string> unpacked;
    for (size_t i = 0; i < length; ++i) {
        for (const auto &match : matches_by_suffix[i]) {
            unpacked.push_back(std::string(i, ' ') + match + std::string(length - i - match.size(), ' '));
        }
    }
    return unpacked;
}
