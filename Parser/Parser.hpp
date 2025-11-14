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

    class Start{
    public:
        void* startNode;
        int64_t startIter;

        Start(){
            startNode = nullptr;
            startIter = 0;
        }

        Start(void* inStart, int64_t inIter){
            startNode = inStart;
            startIter = inIter;
        }

        Start& operator=(Start input){
            startNode = input.startNode;
            startIter = input.startIter;

            return *this;
        }

    };

    //The iterator points to the current token.
    uint64_t iter = 0;
    //This keep track of how many scopes have been found.
    uint64_t scopeCount = 0;
    //Retain a vector for all of the existing scopes.
    std::vector<ExistingScope*> allScopes = std::vector<ExistingScope*>();
    //Error flag for errors.
    bool error = false;
    //All the errors logged.
    std::vector<std::string> errorStack;
    //The given vector of tokens to be parsed into a program.
    std::vector<Token> given;
    //The StackAllocator keeps my program slightly tidier.
    StackAllocator* stack = nullptr;

    bool check(std::string inString);

//Program related functions.

    //Parse the tokens to generate a program.
    void parse();
    //Generate the code.
    void FLVMCodeGen();

//Variables and related operations

    //Check for a variable.
    Variable* variable();
    //Check for an initialization.
    Initialize* initialize();
    //Check for an initialization with an assignment.
    InitializeAssign* initializeAssign();
    //Check for an assignment.
    Assignment* assignment();

//Tree related functions.

    Scope* scope();
    //Generate full bodies of some scope.
    Body* body();
    //Helper function for common expresssions.
    //Assignments, functions, objects with methods, etc.
    Node* commonExpressions();
    //Small subexpressions.
    Node* commonStatements();
    bool hasTokens();
    bool hasTokens(int64_t input);

//Mathematical expressions.
//Technically, all mathematical expressions are subexpressions of boolean expressions.

    //0 priority
    //Addition and subtraction.
    Node* AddSub();

    //1 priority
    //Multiplication and division.
    Node* MulDiv();

    //2 priority
    Node* primitive();      //numbers, booleans.
    Node* parentheses();    //subexpression: p0();Node

//Booleans

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
    Node* OR();             //left: AND(), right: AND()

    //1 priority
    Node* AND();            //left: compare(), right: compare()

    //if statement
    Node* IF();

    //for loop
    Node* FOR();
    Node* WHILE();

    //Functions and methods
    Node* function();
    void functionAppend(Function* input);
    Call* call();
    ReturnClass* Return();

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
        stack = new StackAllocator(size);
        //This is the Global scope.
        stack->currScope = nullptr;
        given = inTokens;
        errorStack = std::vector<std::string>();
    }

    //For the love of God, don't forget this.
    //Delete and free shit you don't need.
    //Stay in good habit of this.
    ~Parser(){
        //Do nothing lmao.
    }

    Start currInfo(){
        return Start(stack->current, iter);
    }

    void reset(Start input){
        stack->current = input.startNode;
        iter = input.startIter;
    }
    
};
#endif