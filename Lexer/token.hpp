#pragma once
#include "types.cpp"
#include <string>

class Token{
private:
    Type type;
    std::string string;
    unsigned long row;
    unsigned long column;
public:
    Token();
    Token(Type inType, char inChar, unsigned long inRow, unsigned long inColumn);
    void operator=(Token right);
    Type getType();
    void append(char in);
    void changeType(Type in);
};