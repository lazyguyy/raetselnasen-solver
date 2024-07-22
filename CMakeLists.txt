cmake_minimum_required(VERSION 3.28)
project(raetsel_solver)

set(CMAKE_CXX_STANDARD 17)

add_executable(raetsel_solver main.cpp
        SuffixTree.cpp
        SuffixTree.h
        Utilities.h
        WordManager.cpp
        WordManager.h
        Utilities.cpp
        InputReader.cpp
        InputReader.h)
