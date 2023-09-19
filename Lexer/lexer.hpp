#pragma once

#include "token.hpp"
#include <fstream>

class Lexer{
    private:
        char* fileName;
        unsigned long row;
        unsigned long column;
    public:
        std::fstream file;
        Lexer(char* &inFile);
        bool getEOF();
        Token next();
};