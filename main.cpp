#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Utilities.h"
#include "word_management/TreeDictionary.h"
#include "InputReader.h"

bool single_query(const WordQuery &query) {
    return query.dictionary->has_word(query);
}

bool check_query(std::vector<WordQuery> query, size_t min_matches) {
    size_t matched_words = 0;
    std::sort(query.begin(), query.end(), [](const WordQuery &q1, const WordQuery &q2) {
        return std::count(q1.query_string.begin(), q1.query_string.end(), '?') < std::count(q2.query_string.begin(), q2.query_string.end(), '?');
    });
    for (size_t i = 0; i < query.size(); ++i) {
        if (min_matches - matched_words > query.size() - i) {
            return false;
        }
        auto word_query = query[i];

        if (word_query.dictionary->has_word(word_query)) {
            matched_words += 1;
        }
    }
    return matched_words >= min_matches;
}

int main() {
    std::string solutions_file = "simple_test/words.txt";
    std::string puzzle_file = "simple_test/puzzle_config.txt";
    std::cout << "Reading solutions file" << std::endl;
    auto solutions = InputReader::read_solutions_list(solutions_file);
    std::cout << "Reading letter mappings" << std::endl;
    auto puzzle = InputReader::read_puzzle(puzzle_file);
    std::cout << "Constructing word manager" << std::endl;
    QueryManager manager(solutions, puzzle.total_words);
    std::cout << "Now testing (" << puzzle.total_words << " choose " << solutions.size() << ") * " << manager.collapsed_multi_words.size() << " = " << manager.total_possibilities  << " possible arrangements." << std::endl;
    std::vector<QueryStruct> successful_queries;
    std::vector<std::vector<bool>> non_blanks;
    size_t check_interval = manager.total_possibilities / 100 / manager.collapsed_multi_words.size() + 1;
    while (!manager.finished) {
//        auto percentage = manager.past_orderings * 100 * manager.collapsed_multi_words.size() / manager.total_possibilities;
//        if (manager.past_orderings % check_interval == 0) {
//            std::cout << (percentage % 10 == 0 ? '|' : '.') << std::flush;
//        }
        if (manager.complies(puzzle)) {
            auto all_queries = manager.generate_queries(puzzle);
            for (auto &query_struct : all_queries) {
                print_container<WordQuery, std::string>(query_struct.query, [](const WordQuery &query) -> std::string {return query.query_string;});
                auto &query = query_struct.query;
//                std::cout << manager.past_orderings << "(" << percentage << "%): ";
                if (check_query(query, puzzle.min_matches)) {
                    successful_queries.push_back(query_struct);
                    non_blanks.push_back(manager.non_blanks);
                }
            }
        }
        manager.next_ordering();
    }
    std::cout << successful_queries.size() << " orderings matching at least " << puzzle.min_matches << " words were found!\n";
    for (const auto &query_struct : successful_queries) {
        print_container<WordQuery, std::string>(query_struct.query, [](const WordQuery &query) -> std::string {return query.query_string;});
        std::vector<std::vector<std::string>> matches;
        size_t max_matches = 0;
        for (size_t query_index = 0; query_index < query_struct.query.size(); ++query_index) {
            const auto query = query_struct.query[query_index];
            const auto word = puzzle.words[query_index];
            if (!puzzle.show_matches) {
                bool matched = single_query(query);
                std::cout << std::string(query.query_string.size(), matched ? '+' : '-') << " ";
            } else {
                matches.push_back(word.dictionary->get_words(query));
                max_matches = std::max(max_matches, matches.back().size());
            }
        }
        if (puzzle.show_matches) {
            for (size_t row = 0; row < max_matches; ++row) {
                for (size_t word = 0; word < query_struct.query.size(); ++word) {
                    if (row < matches[word].size()) {
                        std::cout << matches[word][row];
                    } else {
                        std::cout << std::string(query_struct.query[word].query_string.size(), ' ');
                    }
                    std::cout << " ";
                }
                std::cout << std::endl;
            }

        }
    }
}
