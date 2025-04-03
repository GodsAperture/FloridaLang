#ifndef Parser_h__
#define Parser_h__

#include "StackAllocator.hpp"
#include "Node.hpp"
#include <vector>
#include "../Lexer/Token.hpp"
#include <cinttypes>
#include <iostream>

class Parser{
public:
//Iterator points to current token
    //The iterator is for the parser.
    uint64_t iter = 0;
    //Error flag for errors.
    bool error = false;
    //The given vector of tokens to be parsed into a program.
    std::vector<Token> given;
    //The StackAllocator keeps my program slightly tidier.
    StackAllocator* stack = nullptr;
    //This will be the resulting AST.
    Node* result;
//Comments on the rightmost parts of the functions
//are solely so I can build the parser properly.

//Program
    void parse();
    std::vector<Instruction> FLVMCodeGen();

//Mathematical expressions
    //0 priority
    Node* p0();
    Node* add();            //left: p1(), right: p0();
    Node* subtract();       //left: p1(), right: p0();

    //1 priority
    Node* p1();
    Node* multiply();       //left: p2(), right: p1();
    Node* divide();         //left: p2(), right: p1();

    //2 priority
    Node* p2();             //numbers, booleans.
    Node* parentheses();    //subexpression: p0();

    Parser(std::vector<Token> inTokens, long size){
        given = inTokens;
        stack = new StackAllocator(size);
    }

};
#endif