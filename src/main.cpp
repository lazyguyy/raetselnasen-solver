#include <iostream>
#include <string>
#include "Utilities.h"
#include "word_management/TreeDictionary.h"
#include "io/InputReader.h"
#include "QueryManager.h"
#include "io/OptionParser.h"

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

void test_dictionary(std::shared_ptr<Dictionary> &dictionary) {
    std::string input;
    while (std::getline(std::cin, input)) {
        size_t unknowns = static_cast<size_t>(std::count(input.begin(), input.end(), '?'));
        WordQuery query {input, dictionary, unknowns};
        std::cout << dictionary->has_word(query) << std::endl;
        auto found = dictionary->get_words(query);
        if (!found.empty()) {
            print_container<std::string, std::string>(found, [](const std::string &input) -> std::string{return input;});
        }
    }
}
void show_matched(std::vector<WordQuery> &queries) {
    for (const auto &query : queries) {
        bool matched = single_query(query);
        std::cout << std::string(query.query_string.size(), matched ? '+' : '-') << " ";
    }
    std::cout << std::endl;
}

void show_matches(std::vector<WordQuery> &queries) {
    std::vector<std::vector<std::string>> matches;
    size_t max_matches = 0;
    for (const auto &query : queries) {
        matches.push_back(query.dictionary->get_words(query));
        max_matches = std::max(max_matches, matches.back().size());
    }
    for (size_t row = 0; row < max_matches; ++row) {
        for (size_t word = 0; word < queries.size(); ++word) {
            if (row < matches[word].size()) {
                std::cout << matches[word][row];
            } else {
                std::cout << std::string(queries[word].query_string.size(), ' ');
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}

void query_test_mode(Puzzle &puzzle) {
    std::string input;
    while (std::getline(std::cin, input)) {
        auto queries = split_string(input, ' ');
        if (queries.size() != puzzle.words.size()) {
            std::cout << "A query needs to consist of " << puzzle.words.size() << " words (" << queries.size() << " words were given)." << std::endl;
            continue;
        }
        std::vector<WordQuery> word_queries;
        for (size_t i = 0; i < queries.size(); ++i) {
            WordQuery word_query;
            word_query.query_string = queries[i];
            word_query.dictionary = puzzle.words[i].dictionary;
            word_query.unknown_letters = std::count(queries[i].begin(), queries[i].end(), '?');
            word_queries.push_back(word_query);
        }
        if (puzzle.show_matches) {
            show_matches(word_queries);
        } else {
            show_matched(word_queries);
        }
    }
}

int main(int argc, char **argv) {
    OptionParser parser(argc, argv);
    std::string puzzle_file = "puzzle_config.txt";
    if (parser.has_option_value("-f")) {
        puzzle_file = parser.get_option_value("-f");
    }
    std::cout << "Reading puzzle file" << std::endl;
    auto puzzle = InputReader::read_puzzle(puzzle_file);
    if (parser.has_option("-i")) {
        std::cout << "Entering interactive mode. Simply enter a query below." << std::endl;
        query_test_mode(puzzle);
    }
    if (parser.has_option_value("-t")) {
        std::string dict = parser.get_option_value("-t");
        if (puzzle.dictionaries.count(dict) == 0) {
            std::cout << "No Dictionary of name " << dict << " was defined." << std::endl;
            return 0;
        }
        std::cout << "Entering Dictionary Test Mode for " << dict << std::endl;
        test_dictionary(puzzle.dictionaries[dict]);
    }
    std::cout << "Constructing word manager" << std::endl;
    QueryManager manager(puzzle.input_words, puzzle.total_words);
    std::cout << "There are (" << puzzle.total_words << " choose " << puzzle.input_words.size() << ") * " << manager.collapsed_multi_words.size() << " = " << manager.total_possibilities  << " possible configurations." << std::endl;
    std::vector<QueryStruct> successful_queries;
    std::vector<std::vector<bool>> non_blanks;
    size_t last_percentage = 0;
    while (!manager.finished) {
        auto percentage = (manager.past_orderings + 1) * 100 * manager.collapsed_multi_words.size() / manager.total_possibilities;
        while (last_percentage <= percentage) {
            std::cout << (last_percentage % 10 ? '#' : '|');
            last_percentage++;
        }
        if (manager.complies(puzzle)) {
            auto all_queries = manager.generate_queries(puzzle);
            for (auto &query_struct : all_queries) {
                auto &query = query_struct.query;
                if (check_query(query, puzzle.min_matches)) {
                    successful_queries.push_back(query_struct);
                    non_blanks.push_back(manager.non_blanks);
                }
            }
        }
        manager.next_ordering();
    }
    std::cout << "\n" << successful_queries.size() << " orderings matching at least " << puzzle.min_matches << " words were found!\n";
    for (size_t i = 0; i < successful_queries.size(); ++i) {
        auto &query_struct = successful_queries[i];
        print_container<WordQuery, std::string>(query_struct.query, [](const WordQuery &query) -> std::string {return query.query_string;});
        if (puzzle.show_matches) {
            show_matches(query_struct.query);
        } else {
            show_matched(query_struct.query);
        }
        if (puzzle.show_configuration) {
            print_container<bool, bool>(non_blanks[i], [](const bool &b) -> bool{return b;});
        }
    }
}
