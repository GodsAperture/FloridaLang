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

    //This will emulate function calls.
    class CallAllocator{
    public:
        ExistingScope* head = nullptr;
        ExistingScope* current = nullptr;
        ExistingScope* top = nullptr; 
        ExistingScope* end = nullptr;

        CallAllocator(uint64_t input){
            //Generate the calls allocator.
            head = (ExistingScope*) malloc(input * sizeof(ExistingScope) + (8 - (sizeof(ExistingScope) * input) % 8) % 8);
            //The current pointer will be the start.
            current = head;
            //The end of the memory region.
            end = (ExistingScope*) (input * sizeof(ExistingScope) + (size_t) head);
        }

        ~CallAllocator(){
            delete head;
            head = nullptr;
            current = nullptr;
            end = nullptr;
        }

        ExistingScope* operator[](int64_t input){
            return (ExistingScope*) ((char*) head + input * sizeof(ExistingScope));
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
    //The call stack. The stack that manages the function calls.
    CallAllocator* allCalls = nullptr;
    //A place for all unique existing Scopes.
    CallAllocator* uniqueScopes = nullptr;

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
    //Push the given value to the computation vector.
    void push(types input);
    //Pop the current value from the computation vector.
    types pop();
    //Get the current value from the computation vector.
    types top();
    //Just prints out relevant information about the VM's state.
    void infoPrint();

    //Call stack related methods.
    //Generate a new callStack object in the Stack Allocator.
    ExistingScope* callNew();
    //Pop the top-most callStack from the Stack Allocator.
    void callPop();
    //Get the topmost element from the Stack Allocator.
    ExistingScope* callTop();

    FloridaVM(Parser input){
        //Generate the function bytecode.
        input.stack->allFunctions->FLVMCodeGen(programInstructions);
        //The function stack. The stack for the functions.
        allCalls = new CallAllocator(100);
        //The placement for all unique ExistingScopes.
        uniqueScopes = new CallAllocator(input.allScopes.size());
        uniqueScopes->top = uniqueScopes->head;
        //Place each unique scope into the uniqueScopes allocation IN ORDER.
        for(size_t i = 0; i < input.allScopes.size(); i++){
            //Allocate an object first and foremost.
            uniqueScopes->current = new(uniqueScopes->current) ExistingScope();
            //Copy over its elements.
            uniqueScopes->current->instructionNumber = input.allScopes[i]->instructionNumber;
            uniqueScopes->current->reference = input.allScopes[i]->reference;
            uniqueScopes->current->whichScope = input.allScopes[i]->whichScope;
            //top now points to it.
            uniqueScopes->top = uniqueScopes->current;
            //Current now points to an empty space.
            uniqueScopes->current = uniqueScopes->current + 1;
        }
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
        delete allCalls;
        delete uniqueScopes;
    }

};

#endif