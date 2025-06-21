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
    //The abstract syntax tree of the program.
    Node* AST = nullptr;
    
    //Exceutes the current instruction in the virtual machine.
    //Returns true if an instruction was successfully executed.
    bool next();
    //Prints the current object in the stack.
    void print();
    //Prints the stack from the bottom to the end of the vector.
    void printStack();
    //Push the given value to the computation vector.
    void push(types input);
    //Pop the current value from the computation vector.
    types pop();
    //Get the current value from the computation vector.
    types top();

    FloridaVM(Parser input){
        //Generate the program instructions.
        input.result->FLVMCodeGen(programInstructions);
        //Obtain the abstract syntax tree.
        AST = input.result;
    }

};

#endif