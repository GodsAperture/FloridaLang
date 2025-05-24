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
    std::vector<int64_t> variableCount = std::vector<int64_t>(0);

    //Increments the stack counter.
    void countInc();
    //Decrements the stack counter.
    void countDec();
    //Sets the counter
    void countSet(int64_t input);
    //Returns the current stack size.
    int64_t count();


    //These methods and members are for the virtual machine.

    //The value that keeps track of where we are in the instructions.
    size_t instructionNumber = 0;
    //The instruction set for the program.
    std::vector<Instruction> programInstructions = std::vector<Instruction>();
    //The "stack"
    std::vector<types> computationVector = std::vector<types>();
    //Exceutes the current instruction in the virtual machine.
    //Returns true if an instruction was successfully executed.
    bool next();
    //Prints the current object in the stack.
    void print();
    //Prints the stack from the bottom to the end of the vector.
    void printStack();
//Comments on the rightmost parts of the functions
//are solely so I can build the parser properly.

//ComputationVector related methods

    //Push the given value to the computation vector.
    void push(types input);
    //Pop the current value from the computation vector.
    types pop();
    //Get the current value from the computation vector.
    types top();
    //Check to see if the given string is the next token.
    //If it is, then increment the iterator.
    bool check(std::string inString);

//Program related functions.

    //Parse the tokens to generate a program.
    void parse();
    //Generate the code.
    void FLVMCodeGen();
    //Check for a variable.
    Variable* variable();
    //Check for an initialization.
    Variable* initialize();
    //Check for an assignment.
    Node* assignment();

//Tree related functions.

    Scope* scope();
    //Generate full bodies of some scope.
    Body* body();
    //Helper function for common expresssions.
    //Assignments, functions, objects with methods, etc.
    Node* commonExpressions();
    bool hasTokens();

//Mathematical expressions

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
    Node* OR();             //left: Bool1(), right: Bool1()

    //1 priority
    Node* AND();            //left: compare(), right: compare()

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

    //For the love of God, don't forget this.
    //Delete and free shit you don't need.
    //Stay in good habit of this.
    ~Parser(){
        delete stack;
        delete result;
        delete currScope;
    }

};
#endif