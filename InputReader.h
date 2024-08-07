
#ifndef RAETSEL_SOLVER_INPUTREADER_H
#define RAETSEL_SOLVER_INPUTREADER_H

#include "Utilities.h"
#include "SuffixTree.h"
#include "WordManager.h"


class InputReader {
public:
    static Puzzle read_puzzle_file(std::string &filename);
    static SuffixTree read_word_list(std::string &filename, WordFilter &filter, bool);
    static std::vector<MultiWord> read_solutions_list(std::string &filename);

    static QueryFilter read_filter_file(std::string &filename);
};


#endif //RAETSEL_SOLVER_INPUTREADER_H
