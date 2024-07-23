
#ifndef RAETSEL_SOLVER_UTILITIES_H
#define RAETSEL_SOLVER_UTILITIES_H
#include <string>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <bits/shared_ptr.h>
#include <functional>

using SharedWord = std::shared_ptr<std::string>;
using MultiWord = std::vector<SharedWord>;
using WordList = std::vector<std::shared_ptr<std::string>>;


// should return true if the word should be kept
using FilterFunction = std::function<bool(std::string&)>;
using WordFilter = std::vector<FilterFunction>;

enum LetterMapType {front, back, middle};
struct LetterMap {
    LetterMapType type;
    size_t letter_index;
    size_t ordered_words_index;
    char get_letter(std::string &word) const {
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
    size_t total_words;
};


std::vector<std::vector<size_t>> get_all_combinations(std::vector<size_t> &limits);
std::vector<WordList> collapse_multi_words(std::vector<MultiWord> &multiwords);
size_t get_word_value(const std::string &word);
FilterFunction create_length_filter(Puzzle &, bool);
size_t n_choose_k(size_t n, size_t k);
template <typename value_t>
void print_container(const std::vector<value_t> &vec) {
    for (const auto &v : vec) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}
template <typename value_t>
void print_container(const std::unordered_set<value_t> &set) {
    for (const auto &s : set) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
}
#endif //RAETSEL_SOLVER_UTILITIES_H
