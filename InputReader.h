
#ifndef RAETSEL_SOLVER_INPUTREADER_H
#define RAETSEL_SOLVER_INPUTREADER_H

#include <functional>
#include "Utilities.h"
#include "SuffixTree.h"
#include "WordManager.h"

// should return true if the word should be kept
using FilterFunction = std::function<bool(std::string&)>;
using WordFilter = std::vector<FilterFunction>;

class InputReader {
public:
    static Puzzle read_puzzle_file(std::string &filename);
    static SuffixTree read_word_list(std::string &filename, WordFilter &filter);
    static std::vector<MultiWord> read_solutions_list(std::string &filename);

};


#endif //RAETSEL_SOLVER_INPUTREADER_H
