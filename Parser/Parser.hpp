#ifndef Parser_h__
#define Parser_h__

#include "StackAllocator.hpp"
#include "Node.hpp"
#include <vector>
#include "../Lexer/Token.hpp"
#include <cinttypes>
#include <iostream>

class Parser{
private:
//Iterator points to current token
    uint64_t iter = 0;
    std::vector<Token> given;
    StackAllocator* stack = nullptr;

//0 priority
    Node* p0();

//1 priority
    Node* add();
    Node* subtract();
    Node* p1();

//2 priority
    Node* multiply();
    Node* divide();
    Node* p2();

//3 priority
    Node* exponent();
    Node* p3();

//4 priority
    Node* factorial();
    Node* p4();

//5 priority
    Node* parentheses();
    // Node* brackets();
    // Node* curly();
    Node* negate();

//6 priority
    Node* p5();

public:
    Parser(std::vector<Token> input, long size){
        given = input;
        stack = new StackAllocator(size);
    }

bool parse();

};
#endif