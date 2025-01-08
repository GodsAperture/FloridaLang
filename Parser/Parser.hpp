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
    uint64_t iter = 0;
    bool error = false;
    std::vector<Token> given;
    StackAllocator* stack = nullptr;
//Comments on the rightmost parts of the functions
//are solely so I can build the parser properly.

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
    Node* For();            //statements: Node*, body: Node*.
    Node* While();          //condition: Node*, body: Node*.

//Boolean statements
    Node* If();             //condition: Node*, body: Node*.

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
        stack = new StackAllocator(size);
    }

};
#endif