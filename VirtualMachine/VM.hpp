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
    uint64_t reference = 0;
    //The current instruction.
    size_t instructionNumber = 0;
    //All of the program instructions.
    std::vector<Instruction> programInstructions = std::vector<Instruction>();
    //The pseudo-stack of the language.
    std::vector<types> computationVector = std::vector<types>();
    //Base pointer stack. Takes the form WhichScope, Reference.
    uint64_t* BPStack = nullptr;
    uint64_t BPStackSize = 0;
    uint64_t BPMax = 0;
    //Instruction number stack.
    uint64_t* INStack = nullptr;
    uint64_t INStackSize = 0;
    uint64_t INMax = 0;
    /*The most currently relevant base pointers and where they are in the stack.
    The index represents the respective scope.*/
    uint64_t* CurrentBP = nullptr;
    //No need for CurrentInfoSize, that depends on how many scopes exist in the original code.

    //The abstract syntax tree of the program.
    Node* AST = nullptr;
    Function* allFunctions = nullptr;
    
    //Exceutes the current instruction in the virtual machine.
    //Returns true if an instruction was successfully executed.
    bool next();
    //Prints the current instruction, the next instruction, and the instruction number.
    //Returns `true` if an instruction was successfully executed.
    bool debuggerNext();
    //Prints the stack from the bottom to the end of the vector.
    void printStack();
    //Prints the stack with some prettier debugging.
    void printAll();
    //Just prints out relevant information about the VM's state.
    void infoPrint();

////Computation stack specific tools.

    //Push the given value to the computation vector.
    inline void push(types input);
    //Pop the current value from the computation vector.
    inline void pop();
    //Get the current value from the computation vector.
    inline types top();
    //Make a copy of the topmost element.
    inline void copy();
    //Edit the top of the stack.
    inline void edit(types input);

    //Call stack related methods.
    //Add a new element in the BPStack.
    void BPPush(uint64_t input, uint64_t whichScope);
    //Get the top-most Reference from the BPStack.
    uint64_t BPTopReference();
    //Get the top-most Scope from the BPStack.
    uint64_t BPTopScope();

    //Index the appropriate scope.
    uint64_t BPScope(uint64_t input);
    //Index the appropriate reference.
    uint64_t BPReference(uint64_t input);

    //Pop the top-most element (WhichScope, Reference) from the BPStack.
    void BPPop();
    //Add a new element in the INStack.
    void INPush(uint64_t input);
    //Pop the top-most element from the INStack.
    uint64_t INPop();

    FloridaVM(Parser input){
        //Generate the function bytecode.
        if(input.stack->allFunctions != nullptr){
            input.stack->allFunctions->FLVMCodeGen(programInstructions);
        }
        //The base pointer stack. The stack for the base pointers and their respective scope.
        BPStack = (uint64_t*) malloc(2 * 100 * sizeof(uint64_t));
        INStack = (uint64_t*) malloc(100 * sizeof(uint64_t));
        CurrentBP = (uint64_t*) malloc(sizeof(uint64_t) * input.scopeCount);

        for(int i = 0; i < input.scopeCount; i++){
            CurrentBP[i] = 0;
        }

        BPMax = 2 * 100;
        INMax = 100;
        //Adjust the start of the program.
        instructionNumber = programInstructions.size();
        //Generate the rest of the instructions.
        input.stack->AST->FLVMCodeGen(programInstructions);
        //Obtain the abstract syntax tree.
        AST = input.stack->AST;
        //Obtain all function definitions
        allFunctions = input.stack->allFunctions;
    }

    ~FloridaVM(){
        delete BPStack;
        delete INStack;
    }

};

#endif