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
    //All existing scopes that are generated after parsing.
    std::vector<Scope> scopes;
    //Scope stack to help generate scopes during parsing.
    std::vector<Scope> scopeStack;
    //The StackAllocator keeps my program slightly tidier.
    StackAllocator* stack = nullptr;
//Comments on the rightmost parts of the functions
//are solely so I can build the parser properly.

//Scope related methods.
    Scope& global(){
        return scopeStack[0];
    }

    Scope& top(){
        return scopeStack[scopeStack.size() - 1];
    }

//Program
    Program* programList();
    Node* parse();
    Node* program();        //Composed of lots of statements.

//Statements
    Node* initialize();     //left: keyword, right: std::string.
    Node* variable();       //member: ???
    Node* assignment();     //left: declaration(), right: expression().
    Node* expression();     //subexpession: p0().

//Branching statements and jumps
    Node* jump();           //name: std::string.
    Node* landing();        //where: std::string.
    Node* jumpIf();         //where: std::string, condition: boolean.
    Node* If();             //condition: boolean, body: program().

//Loops
    Node* For();            //statements: Node*, body: Node*.
    Node* While();          //condition: Node*, body: Node*.

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
    // Node* brackets();
    // Node* curly();

    Parser(std::vector<Token> inTokens, long size){
        given = inTokens;
        scopes = std::vector<Scope>(8);
        scopeStack = std::vector<Scope>(4);
        stack = new StackAllocator(size);
    }

};
#endif