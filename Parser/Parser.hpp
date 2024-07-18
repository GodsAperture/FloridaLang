#ifndef Parser_h__
#define Parser_h__

#include <vector>
#include "../Lexer/Token.hpp"
#include <cinttypes>
#include <iostream>

class Parser{
private:
//Iterator points to current token
    uint64_t iter = 0;
    std::vector<Token> given;

//0 priority
    bool p0();

//1 priority
    bool add();
    bool subtract();
    bool p1();

//2 priority
    bool multiply();
    bool divide();
    bool p2();

//3 priority
    bool exponent();
    bool p3();

//4 priority
    bool factorial();
    bool p4();

//5 priority
    bool parentheses();
    // bool brackets();
    // bool curly();
    bool negate();

//6 priority
    bool p5();

public:
    Parser(std::vector<Token> input){
        given = input;
    }

bool parse();

};

#endif