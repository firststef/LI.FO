cmake_minimum_required(VERSION 3.14)
project(LIFOProj)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(LIFOProj
        Automata.h
        GrammarII.cpp
        GrammarII.h
        Regex.cpp
        Regex.h
        Source.cpp
        Utils.h
        SymbolAutomaton.cpp
        SymbolAutomaton.h)
