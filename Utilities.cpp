
#include <functional>
#include "Utilities.h"

std::vector<std::vector<size_t>> get_all_combinations(std::vector<size_t> &limits) {
    std::vector<std::vector<size_t>> index_combinations;
    index_combinations.emplace_back(limits.size());
    print_vector<size_t>(index_combinations[0]);
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
        value += int(letter) - int('a') + 1;
    }
    return value;
}



