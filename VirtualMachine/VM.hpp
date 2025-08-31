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

    class callAllocator{
    public:
        CallStack* head = nullptr;
        CallStack* top = nullptr;
        CallStack* current = nullptr;
        CallStack* end = nullptr;

        callAllocator(uint64_t input){
            //Generate the calls allocator.
            head = (CallStack*) malloc(input * sizeof(CallStack));
            //The current pointer will be the start.
            current = head;
            //The
            end = (CallStack*) (input * sizeof(CallStack) + (size_t) head);
        }

        ~callAllocator(){
            delete head;
            head = nullptr;
            current = nullptr;
            end = nullptr;
        }

    };

    //This will handle relative scopes.
    size_t reference = 0;
    //The current instruction.
    size_t instructionNumber = 0;
    //All of the program instructions.
    std::vector<Instruction> programInstructions = std::vector<Instruction>();
    //The pseudo-stack of the language.
    std::vector<types> computationVector = std::vector<types>();
    //The call stack. The stack that manages the functions.
    callAllocator* allCalls = nullptr;

    //The abstract syntax tree of the program.
    Node* AST = nullptr;
    Function* allFunctions = nullptr;
    
    //Exceutes the current instruction in the virtual machine.
    //Returns true if an instruction was successfully executed.
    bool next();
    //Prints the current instruction, the next instruction, and the instruction number.
    //Returns `true` if an instruction was successfully executed.
    bool debuggerNext();
    //Prints all of the instructions and any relevant values.
    void printInstructions();
    //Prints the stack from the bottom to the end of the vector.
    void printStack();
    //Prints the stack with some prettier debugging.
    void printAll();
    //Push the given value to the computation vector.
    void push(types input);
    //Pop the current value from the computation vector.
    types pop();
    //Get the current value from the computation vector.
    types top();

    //Call stack related methods.
    //Generate a new callStack object in the Stack Allocator.
    void callNew();
    //Pop the top-most callStack from the Stack Allocator.
    void callPop();
    //Get the topmost element from the Stack Allocator.
    CallStack* callTop();

    FloridaVM(Parser input){
        //Generate the function bytecode.
        input.allFunctions->FLVMCodeGen(programInstructions);
        //The function stack. The stack for the functions.
        allCalls = new callAllocator(100 * sizeof(CallStack));
        //Create the first call, which is the main program.
        callNew();
        //Make it so that the first call will end the program upon returning.
        callTop()->instNumber = programInstructions.size();
        callTop()->reference = 0;
        //Adjust the start of the program.
        instructionNumber = programInstructions.size();
        //Generate the rest of the instructions.
        input.result->FLVMCodeGen(programInstructions);
        //Obtain the abstract syntax tree.
        AST = input.result;
        //Obtain all function definitions
        allFunctions = input.allFunctions;
    }

    ~FloridaVM(){
        delete allCalls;
    }

};

#endif