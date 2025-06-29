
#include <algorithm>
#include "../io/InputReader.h"
#include "AnagramDictionary.h"

AnagramDictionary AnagramDictionary::construct(const std::string &filename, std::vector<Filter> &filter)  {
    std::ifstream input_stream(filename);
    std::string line;
    size_t total_words = 0;
    while (std::getline(input_stream, line)) {
        if (std::all_of(filter.begin(), filter.end(), [&line](Filter &f) {return f(line);})) {
            total_words += add_word(line);
        }
    }
    std::cout << "A dictionary containing " << total_words << " words has been constructed." << std::endl;
    return *this;
}

bool AnagramDictionary::add_word(const std::string &word) {
    auto sorted_word = std::string(word);
    std::sort(sorted_word.begin(), sorted_word.end());
    if (anagram_to_words.count(sorted_word) == 0) {
        anagram_to_words[sorted_word] = std::vector<std::string>();
    }
    anagram_to_words[sorted_word].push_back(word);
    return anagrams.add_word(sorted_word);
}

bool AnagramDictionary::has_word(const WordQuery &query) {
    std::string query_string(query.query_string);
    std::sort(query_string.begin(), query_string.end());
    return has_word(anagrams.root, query_string, query.unknown_letters, query.unknown_letters);
}

bool AnagramDictionary::has_word(Node &current, const std::string &query, size_t index, size_t blanks) {
    if (index == query.size()) {
        if (blanks == 0) {
            return current.is_endpoint;
        }
        for (auto &pair : current.children) {
            if (has_word(pair.second, query, index, blanks - 1)) {
                return true;
            }
        }
        return false;
    }
    auto letter = std::tolower(query[index], std::locale());
    bool contained = false;
    if (current.children.count(letter) > 0) {
        contained |= has_word(current.children[letter], query, index + 1, blanks);
    }
    if (!contained && blanks > 0) {
        for (auto &pair: current.children) {
            if (pair.first == letter) {
                continue;
            }
            contained |= has_word(pair.second, query, index, blanks - 1);
        }
    }
    return contained;
}

std::vector<std::string> AnagramDictionary::get_words(const WordQuery &query) {
    std::unordered_set<std::string> found_anagrams;
    std::string query_string(query.query_string);
    std::sort(query_string.begin(), query_string.end());
    get_words(anagrams.root, query_string, query.unknown_letters, "", query.unknown_letters, found_anagrams);
    std::vector<std::string> recovered_anagrams;
    for (const auto &anagram : found_anagrams) {
        for (const auto &recovered : anagram_to_words[anagram]) {
            recovered_anagrams.push_back(recovered);
        }
    }
    return recovered_anagrams;
}


void AnagramDictionary::get_words(Node &current, const std::string &query, size_t index, std::string &&current_path, size_t blanks, std::unordered_set<std::string> &found_anagrams) {
    if (index == query.size()) {
        if (blanks == 0) {
            if (current.is_endpoint)
                found_anagrams.insert(current_path);
        }
        for (auto &pair : current.children) {
            get_words(pair.second, query, index, current_path + pair.first, blanks - 1, found_anagrams);
        }
    }
    auto letter = std::tolower(query[index], std::locale());
    if (current.children.count(letter) > 0) {
        get_words(current.children[letter], query, index + 1, current_path + letter, blanks, found_anagrams);
    }
    if (blanks > 0) {
        for (auto &pair: current.children) {
            if (pair.first == letter) {
                continue;
            }
            get_words(pair.second, query, index, current_path + pair.first, blanks - 1, found_anagrams);
        }
    }
}
