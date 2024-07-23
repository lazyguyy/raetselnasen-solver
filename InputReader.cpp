
#include "InputReader.h"
#include <fstream>
#include <sstream>
#include <regex>

std::vector<MultiWord> InputReader::read_solutions_list(std::string &filename) {
    std::vector<MultiWord> words;
    std::ifstream input_file(filename);
    std::string line;
    while (std::getline(input_file, line)) {
        if (line.empty())
            continue;
        std::stringstream split_stream(line);
        MultiWord multi_word;
        std::string segment;
        while (std::getline(split_stream, segment, ',')) {
            multi_word.push_back(std::make_shared<std::string>(segment));
        }
        words.push_back(std::move(multi_word));
    }
    return words;
}

// Puzzle format:
// each line corresponds to a letter query and are of the form #1{F|M|L}#2
// where #1 and #2 are both numbers, #2 corresponding to the word index in the
// (by word value) ordered word list and #1 corresponding to the letter index
// needed for F and L queries.
// The central letter distinguishes between three different kinds of queries:
//  - F takes the #1th letter from the front
//  - L takes the #1th letter from the back
//  - M takes the middle letter (only makes sense for words of odd length)
// double empty lines signify that the next word begins
Puzzle InputReader::read_puzzle_file(std::string &filename) {
    std::regex expr(R"((L-?\d*|M|\d+)P(\d+))");
    std::vector<LetterMap> letter_queries;
    std::unordered_set<size_t> word_breaks;
    std::ifstream file_stream(filename);
    std::string line;
    size_t query_index = 0;
    size_t total_words = 0;
    while (std::getline(file_stream, line)) {
        if (line.empty()) {
            word_breaks.insert(query_index - 1);
            continue;
        } else {
            std::smatch match;
            std::regex_match(line, match, expr);
            auto mapping = match[1].str();
            size_t letter_index = 0;
            size_t word_index = std::stoi(match[2].str()) - 1;
            total_words = std::max(total_words, word_index + 1);
            LetterMapType type;
            if (mapping[0] == 'L') {
                type = back;
                if (mapping.size() != 1) {
                    letter_index = std::stoi(mapping.substr(2));
                }
            } else if (mapping[0] == 'M') {
                type = middle;
            } else {
                type = front;
                letter_index = std::stoi(mapping) - 1;
            }
            LetterMap new_query {type, letter_index, word_index};
            letter_queries.push_back(new_query);
            query_index += 1;
        }
    }
    word_breaks.insert(query_index - 1);
    return {letter_queries, word_breaks, total_words};
}

SuffixTree InputReader::read_word_list(std::string &filename, WordFilter &filter, bool allow_combinations) {
    std::ifstream word_file (filename);
    SuffixTree dictionary = SuffixTree(allow_combinations);
    std::string line;
    size_t total_words = 0;
    while (std::getline(word_file, line)) {
        if (std::all_of(filter.begin(), filter.end(), [&line](FilterFunction &f) {return f(line);})) {
            total_words += dictionary.add_word(line);
        }
    }
    std::cout << "Built dictionary suffix tree with " << total_words << " entries." << std::endl;
    return dictionary;
}
