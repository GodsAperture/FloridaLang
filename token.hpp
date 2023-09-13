#pragma once
#include <types.hpp>

class Token{
    Token();
    Token(Type inType, char inChar, unsigned long inRow, unsigned long inColumn);
    void operator=(Token right);
    Type getType();
    void append(char in);
    void changeType(Type in);
};