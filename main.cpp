#include <iostream>
#include <fstream>
#include <string>
#include "Utilities.h"
#include "SuffixTree.h"
#include "InputReader.h"

void check_dictionary(SuffixTree &dictionary) {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::cout << line << " has word value " << get_word_value(line) << std::endl;
        if (dictionary.has_word(line)) {
            auto solutions = dictionary.get_words(line);
            std::cout << solutions.size() << " matches found. Solutions include: " << std::endl;
            for (auto &solution : solutions) {
                std::cout << solution << std::endl;
            }
        } else {
            std::cout << "No match found!" << std::endl;
        }
    }
}

int main() {
    std::string dictionary_file = "definitive_word_list_german.txt";
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
    check_dictionary(dictionary);
    WordManager manager(solutions, puzzle.total_words);
    std::cout << "Now testing " << manager.total_possibilities << " possible arrangements..." << std::endl;
    size_t check_interval = manager.total_possibilities / 100 + 1;
    while (!manager.finished) {
        if (manager.past_orderings % check_interval == 0) {
            std::cout << manager.past_orderings * 100 / manager.total_possibilities << "% done." << std::endl;
        }
        if (manager.complies(puzzle)) {
            auto all_queries = manager.generate_queries(puzzle);
            for (auto &query : all_queries) {
                size_t matched_words = 0;
                for (auto &word : query) {
                    if (dictionary.has_word(word)) {
                        matched_words += 1;
                        break;
                    }
                }
                if (matched_words >= min_matches) {
                    std::cout << matched_words << " match(es) for query ";
                    print_container(query);
                    for (auto &single_query : query) {
                        if (dictionary.has_word(single_query)) {
                            std::cout << single_query << " matches ";
                            print_container(dictionary.get_words(single_query));
                        }
                    }
                }
            }
        }
        manager.next_ordering();
    }
}
