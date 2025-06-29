
#include <algorithm>
#include "../InputReader.h"
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
    return true;
}

bool AnagramDictionary::has_word(const Node &current, const std::string &query, size_t index, size_t blanks) {
    if (index == query.size()) {
        if (current.is_endpoint)
            return blanks == 0;
        for (const auto &pair : current.children) {

        }
    }
    return false;
}

std::vector<std::string> AnagramDictionary::get_words(const WordQuery &query) {
    return std::vector<std::string>();
}
