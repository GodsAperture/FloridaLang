cmake_minimum_required(VERSION 3.22)
project(Florida)

file(GLOB_RECURSE FLEXER_SOURCE Lexer/*.cpp)
add_executable(Florida main.cpp ${FLEXER_SOURCE})
target_include_directories(Florida PUBLIC Lexer/)