#ifndef Parser_h__
#define Parser_h__

#include "StackAllocator.hpp"
#include "Node.hpp"
#include <vector>
#include "../Lexer/Token.hpp"
#include <cinttypes>
#include <iostream>
#include <math.h>

class Parser{
public:
//Iterator points to current token

    //The iterator points to the current token.
    uint64_t iter = 0;
    //Error flag for errors.
    bool error = false;
    //All the errors logged.
    std::vector<std::string> errorStack;
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
    //Addition and subtraction.
    Node* AddSub();

    //1 priority
    //Multiplication and division.
    Node* MulDiv();

    //2 priority
    Node* primitive();             //numbers, booleans.
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
    Node* BooleanAlgebra();
    //I'm actually unsure if these are of the same priority...
    Node* OR();             //left: Bool1(), right: Bool1()
    Node* NOR();            //left: Bool1(), right: Bool1()
    Node* XOR();            //left: Bool1(), right: Bool1()
    Node* XNOR();           //left: Bool1(), right: Bool1()

    //1 priority
    Node* Bool1();          
    Node* AND();            //left: compare(), right: compare()
    Node* NAND();           //left: compare(), right: compare()

    //This is so I can "pretty print" the number of errors found.
    //Example, if I have 99 errors I can print out:
    //[ 9] ...
    //[10] ...
    std::string sizer(size_t input){
        std::string result = "[";
        //Get how many decimal places the number is.
        size_t size = ceil(log10((double) errorStack.size()));
        std::string currentNumber = std::to_string(input);
        
        for(size_t i = 1; i < size; i++){
            result += " ";
        }

        return result + currentNumber + "]: ";

    }

    //Error handler
    void errorPrint(){
        for(size_t i = 0; i < errorStack.size(); i++){
            std::cout << sizer(i) << errorStack[i] << "\n";
        }
    }

    Parser(std::vector<Token> inTokens, long size){
        //This is the Global scope.
        currScope = new Scope();
        given = inTokens;
        stack = new StackAllocator(size);
        errorStack = std::vector<std::string>();
    }

    ~Parser(){
        delete stack;
        delete result;
        delete currScope;
    }

};
#endif