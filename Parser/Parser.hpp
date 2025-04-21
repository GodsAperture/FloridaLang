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

    //The iterator points to the current token.
    uint64_t iter = 0;
    //Error flag for errors.
    bool error = false;
    //The given vector of tokens to be parsed into a program.
    std::vector<Token> given;
    //The StackAllocator keeps my program slightly tidier.
    StackAllocator* stack = nullptr;
    //This will be the resulting AST.
    Node* result = nullptr;
    //The currently active scope.
    Scope* currScope = nullptr;
    //Keeps track of how many variables are in a scope.
    //This value will be reset after a scope has been exited.
    int64_t variableCount = 0;
//Comments on the rightmost parts of the functions
//are solely so I can build the parser properly.

//Program related functions.
    //Parse the tokens to generate a program.
    void parse();
    //Generate the code.
    std::vector<Instruction> FLVMCodeGen();
    //This will handles scopes and scoping.
    Scope* scope();
    //Generate full bodies of some scope.
    Body* body();

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
    Node* parentheses();    //subexpression: p0();Node

    //Comparisons
    Node* equal();
    Node* notEqual();
    Node* greaterThan();
    Node* lessThan();
    Node* greaterThanOr();
    Node* lessThanOr();

    //Helper function for comparisons
    Node* compare();

    //0 priority
    Node* Bool0();
    //I'm actually unsure if these are of the same priority...
    Node* OR();             //left: Bool1(), right: Bool1()
    Node* NOR();            //left: Bool1(), right: Bool1()
    Node* XOR();            //left: Bool1(), right: Bool1()
    Node* XNOR();           //left: Bool1(), right: Bool1()

    //1 priority
    Node* Bool1();          
    Node* AND();            //left: compare(), right: compare()
    Node* NAND();           //left: compare(), right: compare()

    Parser(std::vector<Token> inTokens, long size){
        //This is the Global scope.
        currScope = new Scope();
        given = inTokens;
        stack = new StackAllocator(size);
    }

    ~Parser(){
        delete stack;
        delete result;
        delete currScope;
    }

};
#endif