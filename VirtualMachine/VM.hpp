#ifndef VM_h__
#define VM_h__

#include "../Parser/StackAllocator.hpp"
#include "../Parser/Node.hpp"
#include <vector>
#include "../Lexer/Token.hpp"
#include "../Parser/Parser.hpp"
#include <cinttypes>
#include <iostream>
#include <math.h>

class FloridaVM{
public:
    //This will handle relative scopes.
    size_t reference = 0;
    //The current instruction.
    size_t instructionNumber = 0;
    //All of the program instructions.
    std::vector<Instruction> programInstructions = std::vector<Instruction>();
    //The pseudo-stack of the language.
    std::vector<types> computationVector = std::vector<types>();
    //The call stack
    CallStack* callStack;
    //The abstract syntax tree of the program.
    Node* AST = nullptr;
    Function* allFunctions;
    
    //Exceutes the current instruction in the virtual machine.
    //Returns true if an instruction was successfully executed.
    bool next();
    //Prints all of the instructions and any relevant values.
    void printInstructions();
    //Prints the stack from the bottom to the end of the vector.
    void printStack();
    //Push the given value to the computation vector.
    void push(types input);
    //Pop the current value from the computation vector.
    types pop();
    //Get the current value from the computation vector.
    types top();

    FloridaVM(Parser input){
        //Generate the function bytecode.
        input.allFunctions->FLVMCodeGen(programInstructions);
        //Adjust the start of the program.
        instructionNumber = programInstructions.size();
        //Generate the rest of the instructions.
        input.result->FLVMCodeGen(programInstructions);
        //Generate a call stack.
        //This defaults to 100 calls.
        callStack = (CallStack*) malloc(sizeof(CallStack) * 100);
        //Obtain the abstract syntax tree.
        AST = input.result;
        //Obtain all function definitions
        allFunctions = input.allFunctions;
    }

    ~FloridaVM(){
        delete callStack;
    }

};

#endif