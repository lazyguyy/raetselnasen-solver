
#ifndef RAETSEL_SOLVER_OPTIONPARSER_H
#define RAETSEL_SOLVER_OPTIONPARSER_H


#include <string>
#include <vector>
#include <algorithm>

class OptionParser {
public:
    std::vector<std::string> tokens;

    OptionParser(int &argc, char **argv) {
        for (size_t i = 0; i < argc; ++i) {
            tokens.emplace_back(argv[i]);
        }
    }

    bool has_option(std::string &&option) {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

    bool has_option_value(std::string &&option) {
        auto pos = std::find(tokens.begin(), tokens.end(), option);
        return pos != tokens.end() && ++pos != tokens.end();
    }

    std::string get_option_value(std::string &&option) {
        auto pos = std::find(tokens.begin(), tokens.end(), option);
        return *++pos;
    }
};


#endif //RAETSEL_SOLVER_OPTIONPARSER_H
