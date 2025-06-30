
#include <functional>
#include <sstream>
#include "Utilities.h"

std::vector<std::vector<size_t>> get_all_combinations(std::vector<size_t> &limits) {
    std::vector<std::vector<size_t>> index_combinations;
    index_combinations.emplace_back(limits.size());
    for (size_t i = 0; i < limits.size(); ++i) {
        if (limits[i] != 1) {
            size_t current_size = index_combinations.size();
            for (size_t comb_index = 0; comb_index < current_size; ++comb_index) {
                for (size_t copy_index = 0; copy_index < limits[i] - 1; ++copy_index) {
                    std::vector<size_t> copy(index_combinations[comb_index]);
                    copy[i] = 1 + copy_index;
                    index_combinations.push_back(std::move(copy));
                }
            }
        }
    }
    return index_combinations;
}

size_t get_word_value(const std::string &word) {
    size_t value = 0;
    for (auto &letter : word) {
        if (letter == '?') {
            value += 1;
            continue;
        }
        value += int(std::tolower(letter, std::locale())) - int('a') + 1;
    }
    return value;
}

std::vector<WordList> collapse_multi_words(std::vector<MultiWord> &multiwords) {
    std::vector<size_t> index_ranges;
    index_ranges.reserve(multiwords.size());
    for (const auto & known_word : multiwords) {
        index_ranges.push_back(known_word.size());
    }
    auto all_combinations = get_all_combinations(index_ranges);
    std::vector<WordList> collapsed_multi_words;
    for (auto &combination : all_combinations) {
        WordList current_combination;
        for (auto i = 0; i < combination.size(); ++i) {
            current_combination.push_back(multiwords[i][combination[i]]);
        }
        collapsed_multi_words.push_back(current_combination);
    }
    return collapsed_multi_words;
}

//FilterFunction create_length_filter(Puzzle &p, bool interpret_as_max_length) {
//    size_t last_break = 0;
//    std::unordered_set<size_t> occurring_lengths;
//    std::vector<size_t> ordered_word_breaks(p.word_breaks.begin(), p.word_breaks.end());
//    std::sort(ordered_word_breaks.begin(), ordered_word_breaks.end());
//    for (auto b : ordered_word_breaks){
//        occurring_lengths.insert(b + 1 - last_break);
//        last_break = b + 1;
//    }
//    std::function<bool(std::string &)> lambda;
//    if (interpret_as_max_length) {
//        auto max_element = std::max_element(occurring_lengths.begin(), occurring_lengths.end());
//        auto max_length = *max_element;
//        lambda = [max_length](std::string &s) {return s.size() <= max_length;};
//    } else {
//        lambda = [occurring_lengths](std::string &s) {return occurring_lengths.count(s.size()) > 0;};
//    }
//    return lambda;
//}

size_t n_choose_k(size_t n, size_t k) {
    if (k == 0)
        return 1;
    return n * n_choose_k(n - 1, k - 1) / k;

}

std::vector<std::string> split_string(std::string &string, char splitAt) {
    std::stringstream stream(string);
    std::string segment;
    std::vector<std::string> segments;
    while (std::getline(stream, segment, splitAt)) {
        segments.push_back(segment);
    }
    return std::move(segments);
}



