#include "StackAllocator.hpp"
#include "Node.hpp"
    
    StackAllocator::StackAllocator(){
        //Do nothing
    }

    StackAllocator::StackAllocator(long input){
        head = std::malloc(input);
        current = head;
        ZEROPTR = (Node*) give();
        ONEPTR = (Node*) give();
        AST = (Node*) current;
        end = (void*) ((size_t) input + (size_t) head);
    }

    //This only exists to create the ZEROPTR, ONEPTR, and FUNPTR.
    void* StackAllocator::give(){
        void* result = current;
        current = (void*) ((std::size_t) current + 8);
        return result;
    }

    //For the statement `StackAllocator.copy(input)`, `input` is made a copy of `StackAllocator`.
    void StackAllocator::copy(StackAllocator& input){
        //Find out how large the AST is within the original allocation.
        std::ptrdiff_t diff = (char*) current - (char*) head;
        //Create an allocation of that size.
        input.head = malloc(diff);
        //Adjust current to be at `input.head`.
        input.current = input.head;
        //Set `input.end`
        input.end = (void*) ((long long) input.head + diff);
        input.ZEROPTR = (Node*) input.give();
        input.ONEPTR = (Node*) input.give();
        //Copy the AST from the original to the new StackAllocator. 
        input.AST = AST->copy(input);
    }