
#ifndef RAETSEL_SOLVER_UTILITIES_H
#define RAETSEL_SOLVER_UTILITIES_H
#include <string>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <iostream>

enum LetterMapType {front, back, middle};
struct LetterMap {
    LetterMapType type;
    size_t letter_index;
    size_t ordered_words_index;
    char get_letter(std::string &word) {
        switch (type) {
            case front:
                return word[letter_index];
            case back:
                return word[word.size() - letter_index - 1];
            default:
                return word[word.size() / 2];
        }
    }
};
struct Puzzle {
    std::vector<LetterMap> letter_queries;
    std::unordered_set<size_t> word_breaks;
};

std::vector<std::vector<size_t>> get_all_combinations(std::vector<size_t> &limits);
size_t get_word_value(const std::string &word);
template <typename value_t>
void print_vector(const std::vector<value_t> &vec) {
    for (const auto &v : vec) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}
#endif //RAETSEL_SOLVER_UTILITIES_H
