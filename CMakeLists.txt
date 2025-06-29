cmake_minimum_required(VERSION 3.28)
project(raetsel_solver)

set(CMAKE_CXX_STANDARD 17)

add_executable(raetsel_solver main.cpp
        Utilities.h
        Utilities.cpp
        word_management/TreeDictionary.h
        word_management/TreeDictionary.cpp
        QueryManager.cpp
        QueryManager.h
        io/InputReader.cpp
        io/InputReader.h
        Puzzle.cpp
        Puzzle.h
        word_management/AnagramDictionary.cpp
        word_management/AnagramDictionary.h
        word_management/Dictionary.h
        word_management/MultiWordTreeDictionary.cpp
        word_management/MultiWordTreeDictionary.h
        io/OptionParser.cpp
        io/OptionParser.h)
