#include "StackAllocator.hpp"
#include "Node.hpp"
    
    StackAllocator::StackAllocator(){
        //Do nothing
    }

    StackAllocator::StackAllocator(long input){
        head = std::malloc(input + 1);
        current = (void*) ((char*) head + 8);
        currentScope = nullptr;
        AST = (Node*) current;
        end = (void*) ((size_t) input + (size_t) head);
    }
