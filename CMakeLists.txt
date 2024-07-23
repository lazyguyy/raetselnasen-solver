cmake_minimum_required(VERSION 3.28)
project(raetsel_solver)

set(CMAKE_CXX_STANDARD 17)

add_executable(raetsel_solver main.cpp
        Utilities.h
        Utilities.cpp
        SuffixTree.h
        SuffixTree.cpp
        WordManager.cpp
        WordManager.h
        InputReader.cpp
        InputReader.h)
