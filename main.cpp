#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Utilities.h"
#include "SuffixTree.h"
#include "InputReader.h"


bool check_query(SuffixTree &dictionary, Query &query, size_t min_matches) {
    size_t matched_words = 0;
    std::sort(query.begin(), query.end(), [](const std::string &s1, const std::string &s2) {
        return std::count(s1.begin(), s1.end(), '?') < std::count(s2.begin(), s2.end(), '?');
    });
    for (size_t i = 0; i < query.size(); ++i) {
        if (min_matches - matched_words > query.size() - i) {
            std::cout << std::endl;
            return false;
        }
        auto word = query[i];
        std::cout << word;
        if (dictionary.dynamic_has_word(word)) {
            std::cout << "+ ";
            matched_words += 1;
            if (matched_words >= min_matches) {
                std::cout << std::endl;
                return true;
            }
        } else {
            std::cout << "- ";
        }
    }
    std::cout << std::endl;
    return false;
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
        check_query(dictionary, query, 0);
    }
}


int main() {
    std::string dictionary_file = "german_words_no_umlaut_singles.txt";
    std::string solutions_file = "solutions.txt";
    std::string puzzle_file = "letter_mappings.txt";
    bool allow_word_combinations = true;
    size_t min_matches = 5;
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
    std::vector<Query> successful_queries;
    std::vector<std::vector<bool>> non_blanks;
    while (!manager.finished) {
        auto percentage = manager.past_orderings * 100 / manager.total_possibilities;
        if (manager.complies(puzzle)) {
            auto all_queries = manager.generate_queries(puzzle);
            for (auto &query : all_queries) {
                std::cout << manager.past_orderings << "(" << percentage << "%): ";
                if (check_query(dictionary, query, min_matches)) {
                    successful_queries.push_back(query);
                    non_blanks.push_back(manager.non_blanks);
                }
            }
        }
        manager.next_ordering();
    }
    std::cout << successful_queries.size() << " orderings matching at least " << min_matches << " words were found!";
    for (size_t i = 0; i < successful_queries.size(); ++i) {
        print_container(successful_queries[i]);
        std::cout << "Original words were placed as follows: ";
        print_container(non_blanks[i]);
//                    for (auto &single_query : query) {
//                        if (dictionary.has_word(single_query)) {
//                            std::cout << single_query << " matches ";
//                            print_container(dictionary.get_words(single_query));
//                        }
//                    }

    }
}
