
#include "InputReader.h"
#include "word_management/AnagramDictionary.h"
#include "word_management/TreeDictionary.h"
#include "word_management/MultiWordTreeDictionary.h"
#include <fstream>
#include <sstream>
#include <regex>

std::vector<MultiWord> InputReader::read_solutions_list(const std::string &filename) {
    std::vector<MultiWord> words;
    std::ifstream input_file(filename);
    std::string line;
    while (std::getline(input_file, line)) {
        if (line.empty())
            continue;
        if (line[0] == '#')
            continue;
        std::stringstream split_stream(line);
        MultiWord multi_word;
        std::string segment;
        while (std::getline(split_stream, segment, ',')) {
            multi_word.push_back(std::make_shared<std::string>(segment));
        }
        words.push_back(std::move(multi_word));
    }
    return words;
}

std::string trim_string(const std::string &string, const std::string &whitespace = " \t\n") {
    const auto start = string.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return "";
    const auto end = string.find_last_not_of(whitespace);
    return string.substr(start, end - start + 1);
};

std::string read_value(std::ifstream &input_stream, char delim, std::string &&name = "word") {
    std::string value;
    std::getline(input_stream, value, delim);
    auto trimmed = trim_string(value);
//    std::cout << "read " << name << " '" << trimmed << "' from input stream (read until delimiter '" << delim << "')." << std::endl;
    return trim_string(value);
}

int peek_to_next_non_ws_char(std::ifstream &input_stream) {
    while (input_stream && (std::isspace(input_stream.peek()) || input_stream.peek() == '\n'))
        input_stream.get();
//    std::cout << "Next important character: '" << (char) input_stream.peek() << "'." << std::endl;
    return input_stream.peek();
}

int eat_next_char(std::ifstream &input_stream) {
    peek_to_next_non_ws_char(input_stream);
    return input_stream.get();
}

filter_settings InputReader::parse_filter(std::ifstream &input_stream) {
    filter_settings settings;
    while (input_stream && peek_to_next_non_ws_char(input_stream) != '}') {
        auto variable = read_value(input_stream, '=');
        auto value = read_value(input_stream, ';');
        bool valid = true;
        if (variable == "satisfy") {
            settings.satisfy = value == "true";
        } else if (variable == "regex") {
            settings.regex = value;
        } else {
            std::cout << "Unknown filter option '" << variable << "' with value '" << value << "'" << std::endl;
            valid = false;
        }
        if (valid) {
            std::cout << "Set Filter " << variable << " to " << value << "." << std::endl;
        }
    }
    // advance over '}'
    eat_next_char(input_stream);
    // advance over optional ';'
    if (peek_to_next_non_ws_char(input_stream) == ';')
        eat_next_char(input_stream);
    return settings;
}


std::vector<LetterMap> InputReader::parse_letters(std::string &letter_maps) {
    std::regex expr(R"(([FBM]\d*)P(\d+))");
    std::vector<LetterMap> queries;
    for (auto &query : split_string(letter_maps, ';')) {
        query = trim_string(query);
        std::smatch match;
        std::regex_match(query, match, expr);
        // match[0] is the entire match, captured groups start at index 1
        auto mapping = match[1].str();
        size_t letter_index = 0;
        size_t word_index = std::stoi(match[2].str()) - 1;
        LetterMapType type;
        if (mapping[0] == 'M') {
            type = middle;
        } else {
            letter_index = std::stoi(mapping.substr(1)) - 1;
            type = mapping[0] == 'B' ? back : front;
        }
        queries.push_back({type, letter_index, word_index});
    }
    return queries;
}

word_settings InputReader::parse_word(std::ifstream &input_stream) {
    word_settings settings;
    while (input_stream && peek_to_next_non_ws_char(input_stream) != '}') {
        auto variable = read_value(input_stream, '=');
        if (variable == "letters") {
            // parse '['
            eat_next_char(input_stream);
            std::string letters_string;
            std::getline(input_stream, letters_string, ']');
            settings.letters = parse_letters(letters_string);
            // advance over optional ';'
            if (peek_to_next_non_ws_char(input_stream) == ';')
                eat_next_char(input_stream);
        } else if (variable == "filters") {
            std::vector<filter_settings> filters;
            eat_next_char(input_stream);
            while (input_stream && input_stream.get() != ']') {
                filters.push_back(parse_filter(input_stream));
            }
            settings.filters = filters;
            // advance over optional ';'
            if (peek_to_next_non_ws_char(input_stream) == ';')
                eat_next_char(input_stream);
        } else {
            auto value = read_value(input_stream, ';');
            if (variable == "dictionary") {
                settings.dictionary_name = value;
            } else {
                std::cout << "Unknown word option '" << variable << "' with value '" << value << "'" << std::endl;
            }
        }
    }
    // advance over '}'
    eat_next_char(input_stream);
    // advance over optional ';'
    if (peek_to_next_non_ws_char(input_stream) == ';')
        eat_next_char(input_stream);
    return settings;
}

Word construct_word(word_settings &word_settings, std::unordered_map<std::string, std::shared_ptr<Dictionary>> &dicts) {
    std::vector<Filter> filters;
    for (const auto &f : word_settings.filters) {
        std::regex match_string(f.regex);
        FilterFunction ff = [match_string](const std::string &to_judge) -> bool {
            std::smatch match;
            return std::regex_match(to_judge, match, match_string);
        };
        filters.push_back({f.satisfy, ff});
    }
    if (dicts.count(word_settings.dictionary_name) == 0) {
        std::cout << "Missing dictionary of name '" << word_settings.dictionary_name << "'." << std::endl;
        exit(-1);
    }
    return {word_settings.letters, filters, dicts[word_settings.dictionary_name]};
}


dict_settings InputReader::parse_dictionary(std::ifstream &input_stream) {
    dict_settings settings;
    settings.print_words = false;
    while (input_stream && peek_to_next_non_ws_char(input_stream) != '}') {
        auto variable = read_value(input_stream, '=', "variable");
        bool valid = true;
        if (variable == "filters") {
            std::vector<filter_settings> filters;
            eat_next_char(input_stream);
            while (input_stream && input_stream.get() != ']') {
                filters.push_back(parse_filter(input_stream));
            }
            settings.filters = filters;
            // advance over optional ';'
            if (peek_to_next_non_ws_char(input_stream) == ';')
                eat_next_char(input_stream);
            continue;
        }
        auto value = read_value(input_stream, ';', "value");
        if (variable == "name") {
            settings.name = value;
        } else if (variable == "print_words") {
            settings.print_words = value == "true";
        } else if (variable == "filename") {
            settings.filename = value;
        } else if (variable == "type") {
            if (value == "single_word") {
                settings.type = SingleWordDictionaryType;
            } else if (value == "multi_word") {
                settings.type = MultiWordDictionaryType;
            } else if (value == "anagram") {
                settings.type = AnagramDictionaryType;
            } else {
                std::cout << "Unknown dictionary type '" << value << "'" << std::endl;
            }
        } else {
            std::cout << "Unknown dictionary option '" << variable << "' with value '" << value << "'" << std::endl;
            valid = false;
        }
        if (valid) {
            std::cout << "Set Dictionary " << variable << " to " << value << "." << std::endl;
        }
    }
    // advance over '}'
    eat_next_char(input_stream);
    // advance over optional ';'
    if (peek_to_next_non_ws_char(input_stream) == ';')
        eat_next_char(input_stream);
    return settings;
}


general_settings InputReader::parse_settings(std::ifstream &input_stream) {
    general_settings settings{};
    while (input_stream && peek_to_next_non_ws_char(input_stream) != '}') {
        auto variable = read_value(input_stream, '=', "variable");
        auto value = read_value(input_stream, ';', "value");
        bool valid = true;
        if (variable == "detail") {
            settings.detail_level = std::stoi(value);
        } else if (variable == "min_matches") {
            settings.min_matches = std::stoi(value);
        } else if (variable == "show_matches") {
            settings.show_matches = value == "true";
        } else if (variable == "total_words") {
            settings.total_words = std::stoi(value);
        } else {
            std::cout << "Unknown general setting: '" << variable << "' with value '" << value << "'." << std::endl;
            valid = false;
        }
        if (valid)
            std::cout << "Set " << variable << " to " << value << "." << std::endl;
    }
    // advance over '}'
    eat_next_char(input_stream);
    // advance over optional ';'
    if (peek_to_next_non_ws_char(input_stream) == ';')
        eat_next_char(input_stream);
    return settings;
}

Puzzle InputReader::read_puzzle(const std::string &puzzle_config_file) {
    std::ifstream input_stream(puzzle_config_file);
    std::vector<word_settings> unconstructed_words;
    std::vector<Word> words;
    std::unordered_map<std::string, std::shared_ptr<Dictionary>> dicts;
    std::string type;
    general_settings settings{};
    while (input_stream && !(type = read_value(input_stream, '{', "type")).empty()) {
        if (type.empty()) {
            break;
        }
        if (type == "dictionary") {
            auto parsed = parse_dictionary(input_stream);
            std::vector<Filter> filters;
            for (const auto &f : parsed.filters) {
                std::regex match_string(f.regex);
                FilterFunction ff = [match_string](const std::string &judge) -> bool {
                    std::smatch match;
                    return std::regex_match(judge, match, match_string);
                };
                filters.push_back({f.satisfy, ff});
            }
            std::shared_ptr<Dictionary> dict;
            switch (parsed.type) {
                case AnagramDictionaryType:
                    dict = std::make_shared<AnagramDictionary>(AnagramDictionary(parsed.print_words).construct(parsed.filename, filters));
                    break;
                case SingleWordDictionaryType:
                    dict = std::make_shared<TreeDictionary>(TreeDictionary(parsed.print_words).construct(parsed.filename, filters));
                    break;
                case MultiWordDictionaryType:
                    dict = std::make_shared<MultiWordTreeDictionary>(MultiWordTreeDictionary(parsed.print_words).construct(parsed.filename, filters));
                    break;
            }
            dicts[parsed.name] = dict;
        }
        if (type == "word") {
            auto parsed = parse_word(input_stream);
            // delay word construction, so it does not matter if the dictionaries are defined below the words in the
            // configuration file
            unconstructed_words.push_back(parsed);
        }
        if (type == "general") {
            settings = parse_settings(input_stream);
        }
    }
    std::cout << "read puzzle input!" << std::endl;
    words.reserve(unconstructed_words.size());
    for (auto &word_settings : unconstructed_words) {
        words.push_back(construct_word(word_settings, dicts));
    }
    return {words, settings.min_matches, settings.total_words, settings.detail_level, settings.show_matches};
}
