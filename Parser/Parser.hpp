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
//Comments on the rightmost parts of the functions
//are solely so I can build the parser properly.

//0 priority
    Node* p0();
    Node* add();            //left: p1(), right: p0();
    Node* subtract();       //left: p1(), right: p0();

//1 priority
    Node* p1();
    Node* multiply();       //left: p2(), right: p1();
    Node* divide();         //left: p2(), right: p1();

//2 priority
    Node* p2();
    Node* parentheses();    //subexpression: p0();
    // Node* brackets();
    // Node* curly();

//3 priority
    Node* p3();             //numbers...for now.

public:
    Parser(std::vector<Token> input, long size){
        given = input;
        stack = new StackAllocator(size);
    }

bool parse();

};
#endif