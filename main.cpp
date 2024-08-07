#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Utilities.h"
#include "SuffixTree.h"
#include "InputReader.h"


bool check_query(SuffixTree &dictionary, Query query, size_t min_matches) {
    size_t matched_words = 0;
    std::sort(query.begin(), query.end(), [](const std::string &s1, const std::string &s2) {
        return std::count(s1.begin(), s1.end(), '?') < std::count(s2.begin(), s2.end(), '?');
    });
    for (size_t i = 0; i < query.size(); ++i) {
        if (min_matches - matched_words > query.size() - i) {
            return false;
        }
        auto word = query[i];
        dictionary.min_length = std::min(4ULL, word.size()/2);
        if (dictionary.dynamic_has_word(word)) {
            matched_words += 1;
        }
    }
    return matched_words >= min_matches;
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
    std::string dictionary_file = "super_clean.txt";
    std::string solutions_file = "solutions24.txt";
    std::string puzzle_file = "letter_mappings24.txt";
    std::string filter_file = "query_filters24.txt";
    bool allow_word_combinations = false;
    size_t min_matches = 6;
    WordFilter filter;
    std::cout << "Reading query filters file" << std::endl;
    auto query_filters = InputReader::read_filter_file(filter_file);
    std::cout << "Reading solutions file" << std::endl;
    auto solutions = InputReader::read_solutions_list(solutions_file);
    std::cout << "Reading letter mappings" << std::endl;
    auto puzzle = InputReader::read_puzzle_file(puzzle_file);
    filter.emplace_back([](std::string &word) {return word[0] != '#';});
    filter.push_back(std::move(create_length_filter(puzzle, allow_word_combinations)));
    std::cout << "Building dictionary" << std::endl;
    auto dictionary = InputReader::read_word_list(dictionary_file, filter, allow_word_combinations);
//    check_dictionary(dictionary);
    WordManager manager(solutions, puzzle.total_words);
    std::cout << "Now testing (" << puzzle.total_words << " choose " << solutions.size() << ") * " << manager.collapsed_multi_words.size() << " = " << manager.total_possibilities  << " possible arrangements." << std::endl;
    std::vector<QueryStruct> successful_queries;
    std::vector<std::vector<bool>> non_blanks;
    size_t check_interval = manager.total_possibilities / 100 / manager.collapsed_multi_words.size() + 1;
    while (!manager.finished) {
        auto percentage = manager.past_orderings * 100 * manager.collapsed_multi_words.size() / manager.total_possibilities;
        if (manager.past_orderings % check_interval == 0) {
            std::cout << (percentage % 10 == 0 ? '|' : '.') << std::flush;
        }
        if (manager.complies(puzzle)) {
            auto all_queries = manager.generate_queries(puzzle);
            for (auto &query_struct : all_queries) {
                auto &query = query_struct.query;
                if (!query_filters.is_valid_query(query)) {
                    continue;
                }
//                std::cout << manager.past_orderings << "(" << percentage << "%): ";
                if (check_query(dictionary, query, min_matches)) {
                    successful_queries.push_back(query_struct);
                    non_blanks.push_back(manager.non_blanks);
                }
            }
        }
        manager.next_ordering();
    }
    std::cout << "|" << std::endl;
    std::cout << successful_queries.size() << " orderings matching at least " << min_matches << " words were found!\n";
    for (size_t i = 0; i < successful_queries.size(); ++i) {
        auto &query_struct = successful_queries[i];
        print_container(query_struct.query);
        for (auto &word : query_struct.query) {
            std::vector<std::string> single_query{word};
            bool matched = check_query(dictionary, single_query, 1);
            std::cout << std::string(word.size(), matched ? '+' : '-') << " ";
        }
        std::cout << std::endl;
        for (const auto &word : manager.collapsed_multi_words[query_struct.origin_index]) {
            std::cout << *word << " ";
        }
        std::cout << std::endl;
//        std::cout << "Original words were placed as follows: ";
        print_container(non_blanks[i]);
        std::cout << std::endl;
//                    for (auto &single_query : query) {
//                        if (dictionary.has_word(single_query)) {
//                            std::cout << single_query << " matches ";
//                            print_container(dictionary.get_words(single_query));
//                        }
//                    }

    }
}
