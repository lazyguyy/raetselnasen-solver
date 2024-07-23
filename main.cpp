#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Utilities.h"
#include "SuffixTree.h"
#include "InputReader.h"


size_t check_query(SuffixTree &dictionary, Query &query) {
    size_t matched_words = 0;
    for (auto &word : query) {
        if (dictionary.has_word(word)) {
            matched_words += 1;
        }
    }
    return matched_words;
}

size_t check_query_detailed(SuffixTree &dictionary, Query &query) {
    size_t matched_words = 0;
    for (auto &word : query) {
        auto results = dictionary.get_words(word);
        if (results.empty()) {
            std::cout << word << " doesn't match." << std::endl;
        } else {
            std::cout << word << " matches ";
            print_container(results);
            matched_words += 1;
        }
    }
    return matched_words;
}

void check_dictionary(SuffixTree &dictionary) {
    std::cout << "Enter queries (dictionary is in combination mode: " << dictionary.allow_combinations << "):" << std::endl;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty())
            continue;
        std::stringstream split_stream(line);
        Query query;
        std::string word;
        while (std::getline(split_stream, word, ' ')) {
            std::cout << word << " has word value at least " << get_word_value(word) << std::endl;
            query.push_back(std::move(word));
        }
        check_query_detailed(dictionary, query);
    }
}


int main() {
    std::string dictionary_file = "small_word_list_german.txt";
    std::string solutions_file = "solutions.txt";
    std::string puzzle_file = "letter_mappings.txt";
    bool allow_word_combinations = true;
    size_t min_matches = 2;
    WordFilter filter;
    std::cout << "Reading solutions file" << std::endl;
    auto solutions = InputReader::read_solutions_list(solutions_file);
    std::cout << "Reading letter mappings" << std::endl;
    auto puzzle = InputReader::read_puzzle_file(puzzle_file);
    filter.push_back(std::move(create_length_filter(puzzle, allow_word_combinations)));
    std::cout << "Building dictionary" << std::endl;
    auto dictionary = InputReader::read_word_list(dictionary_file, filter, allow_word_combinations);
//    check_dictionary(dictionary);
    WordManager manager(solutions, puzzle.total_words);
    std::cout << "Now testing " << manager.total_possibilities << " possible arrangements..." << std::endl;
    size_t check_interval = manager.total_possibilities / 1000 + 1;
    while (!manager.finished) {
        auto percentage = manager.past_orderings * 1000 / manager.total_possibilities / 10.0;
        if (manager.past_orderings % check_interval == 0) {
            std::cout << percentage << "% done." << std::endl;
        }
        if (manager.complies(puzzle)) {
            auto all_queries = manager.generate_queries(puzzle);
            for (auto &query : all_queries) {
//                if (percentage >= 56) {
//                    print_container(query);
//                }
                size_t matched_words = check_query(dictionary, query);
                if (matched_words >= min_matches) {
                    std::cout << matched_words << " match(es) for ";
                    print_container(query);
                    std::cout << "Original words were placed as follows: ";
                    print_container(manager.non_blanks);
//                    for (auto &single_query : query) {
//                        if (dictionary.has_word(single_query)) {
//                            std::cout << single_query << " matches ";
//                            print_container(dictionary.get_words(single_query));
//                        }
//                    }
                }
            }
        }
        manager.next_ordering();
    }
}
