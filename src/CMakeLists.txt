cmake_minimum_required(VERSION 3.1)
project(raetsel_solver)

set(CMAKE_CXX_STANDARD 11)

add_executable(raetsel_solver main.cpp
        Utilities.h
        Utilities.cpp
        word_management/TreeDictionary.h
        word_management/TreeDictionary.cpp
        QueryManager.cpp
        QueryManager.h
        io/InputReader.cpp
        io/InputReader.h
        word_management/AnagramDictionary.cpp
        word_management/AnagramDictionary.h
        word_management/Dictionary.h
        word_management/MultiWordTreeDictionary.cpp
        word_management/MultiWordTreeDictionary.h
        io/OptionParser.cpp
        io/OptionParser.h)
