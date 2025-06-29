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
    std::cout << "Entering Dictionary Test Mode" << std::endl;
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

int main(int argc, char **argv) {
    OptionParser parser(argc, argv);
    std::string puzzle_file = "puzzle_config.txt";
    if (parser.has_option_value("-f")) {
        puzzle_file = parser.get_option_value("-f");
    }
    std::cout << "Reading puzzle file" << std::endl;
    auto puzzle = InputReader::read_puzzle(puzzle_file);
    if (parser.has_option("-i")) {
        // TODO start interactive mode
    }
    if (parser.has_option_value("-t")) {
        std::string dict = parser.get_option_value("-t");
        test_dictionary(puzzle.dictionaries[dict]);
    }
    std::cout << "Constructing word manager" << std::endl;
    QueryManager manager(puzzle.input_words, puzzle.total_words);
    std::cout << "Now testing (" << puzzle.total_words << " choose " << puzzle.input_words.size() << ") * " << manager.collapsed_multi_words.size() << " = " << manager.total_possibilities  << " possible arrangements." << std::endl;
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
