
#ifndef RAETSEL_SOLVER_UTILITIES_H
#define RAETSEL_SOLVER_UTILITIES_H
#include <string>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <functional>
#include <regex>


using WordList = std::vector<std::shared_ptr<std::string>>;
using SharedWord = std::shared_ptr<std::string>;
using MultiWord = std::vector<SharedWord>;


using FilterFunction = std::function<bool(const std::string&)>;

struct Filter {
    bool satisfy;
    FilterFunction function;
    [[nodiscard]] bool operator() (const std::string &word) const {
        return function(word) == satisfy;
    }
    [[nodiscard]] bool is_valid(const std::string &word) const {
        return function(word) == satisfy;
    }
};

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


std::vector<std::vector<size_t>> get_all_combinations(std::vector<size_t> &limits);
std::vector<WordList> collapse_multi_words(std::vector<MultiWord> &multiwords);
size_t get_word_value(const std::string &word);
size_t n_choose_k(size_t n, size_t k);
std::vector<std::string> split_string(std::string &, char splitAt);
std::string find_base_path(const std::string &file_name);

template <typename value_t, typename value_s = value_t>
void print_container(const std::vector<value_t> &vec, std::function<value_s(const value_t &)> transform = [](value_t &inp) -> value_s{return inp;}) {
    for (const auto &v : vec) {
        std::cout << transform(v) << " ";
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
