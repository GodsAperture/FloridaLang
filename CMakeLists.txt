cmake_minimum_required(VERSION 3.22)
project(main)

file(GLOB_RECURSE FLEXER_SOURCE Lexer/*.cpp Parser/*.cpp)
add_executable(main main.cpp ${FLEXER_SOURCE})
target_include_directories(main PUBLIC Lexer/ PUBLIC Parser/)