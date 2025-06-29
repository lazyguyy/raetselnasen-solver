
#ifndef RAETSEL_SOLVER_INPUTREADER_H
#define RAETSEL_SOLVER_INPUTREADER_H

#include "Utilities.h"
#include "word_management/TreeDictionary.h"
#include "QueryManager.h"


struct filter_settings {
    bool satisfy;
    std::string regex;
};

struct dict_settings {
    std::string name;
    std::string filename;
    std::vector<filter_settings> filters;
    dict_type type;
    bool print_words;
};

struct word_settings {
    std::string dictionary_name;
    std::vector<LetterMap> letters;
    std::vector<filter_settings> filters;
};

struct general_settings {
    size_t detail_level;
    size_t min_matches;
    size_t total_words;
    bool show_matches;
};



class InputReader {
public:
    static Puzzle read_puzzle(const std::string &puzzle_config_file);
    static std::vector<MultiWord> read_solutions_list(const std::string &filename);

//    static QueryFilter read_filter_file(std::string &filename);
private:
    static filter_settings parse_filter(std::ifstream &);
    static std::vector<LetterMap> parse_letters(std::string &);
    static word_settings parse_word(std::ifstream &);
    static dict_settings parse_dictionary(std::ifstream &);
    static general_settings parse_settings(std::ifstream &);
};


#endif //RAETSEL_SOLVER_INPUTREADER_H
