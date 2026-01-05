#ifndef StackAllocator_h__
#define StackAllocator_h__

#include <stdlib.h>
#include <utility>
#include <new>
#include <iostream>
#include "Node.hpp"

class Node;
class Scope;
class Function;

class StackAllocator{
public:
    void* head = nullptr;
    void* current = nullptr;
    void* end = nullptr;

    //Other pointers
    Node* ZEROPTR = nullptr;
    Node* ONEPTR = nullptr;
    //The Abstract Syntax Tree
    Node* AST = nullptr;

    //The global scope...wasn't that obvious?
    Scope* globalScope = nullptr;
    //The currently active scope.
    Scope* currentScope = nullptr;
    //The function/method being currently defined.
    Function* currentFunction = nullptr;
    //A linked list of all the functions in the program.
    Function* allFunctions = nullptr;
    //The class being currently defined.
    //Object* currentObject = nullptr;
    //A linked list of all the classes in the program.
    //Ojbect* allObjects = nullptr;

    StackAllocator();

    StackAllocator(long input);

    ~StackAllocator(){
        free(head);
        head = nullptr;
        ZEROPTR = nullptr;
        ONEPTR = nullptr;
    }

    // Allocates one object at the top of the stack, and allows you to pass parameters to that object's constructor through it's parameters
    template<typename T, typename... Args>
    T* alloc(Args&&... args){
    
        std::size_t bytes_needed = sizeof(T);
        bytes_needed += 8 - (bytes_needed % 8);
        if ((char*) current + bytes_needed > (char*) end) {
            throw std::bad_alloc(); // Not enough memory
        }
        T* result = new (current) T(std::forward<Args>(args)...); // Placement new with arguments
        current = (void*)((char*) current + bytes_needed);
        return result;
    }

    // Deallocate memory, you need to make sure what you are freeing is the last thing allocated
    // Deallocates one object that is assumed to be at the top of the stack
    template<typename T>
    void dealloc(T* ptr){
        if(ptr != head){
            std::size_t bytes_needed = sizeof(T);
            bytes_needed += (8 - (bytes_needed % 8)) % 8;
            current = (void*) ((std::size_t) current - bytes_needed);
            ptr->~T(); // Call the destructor explicitly
        }
    }

    //This only exists to create the ZEROPTR, ONEPTR, and FUNPTR.
    void* give();

    template<typename T>
    void mdealloc(T* ptr){
        current = (void*) ((long long) current - sizeof(T));
    }

    template<typename T>
    T* peek(){
        return (T*) current;
    }

    //For the statement `StackAllocator.copy(input)`, `input` is made a copy of `StackAllocator`.
    void copy(StackAllocator& input);
};
#endif