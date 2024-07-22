#include <iostream>
#include <fstream>
#include <string>
#include "SuffixTree.h"
#include "Utilities.h"
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
    std::string dictionary_file = "german_words_no_umlaut.txt";
    std::string solutions_file = "solutions.txt";
    std::string puzzle_file = "letter_mappings.txt";
    WordFilter filter {};
    std::cout << "Building dictionary" << std::endl;
    auto dictionary = InputReader::read_word_list(dictionary_file, filter);
    std::cout << "Reading solutions file" << std::endl;
    auto solutions = InputReader::read_solutions_list(solutions_file);
    std::sort(solutions.begin(), solutions.end(),
              [](const std::string &first, const std::string &second) {return get_word_value(first) <= get_word_value(second);});
    std::cout << "Reading letter mappings" << std::endl;
    auto puzzle = InputReader::read_puzzle_file(puzzle_file);
    WordManager manager(solutions, puzzle.letter_queries.size());
    while (!manager.finished) {
        if (manager.complies(puzzle)) {
            auto all_queries = manager.generate_queries(puzzle);
            for (auto &query : all_queries) {
                bool is_valid = true;
                for (auto &word : query) {
                    if (!dictionary.has_word(word)) {
                        is_valid = false;
                        break;
                    }
                }
                if (is_valid) {
                    print_vector<std::string>(query);
                }
            }
        }
        manager.next_ordering();
    }
}
