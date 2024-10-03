#ifndef StackAllocator_h__
#define StackAllocator_h__
#include <stdlib.h>
#include <utility>
#include <new>
#include <iostream>

class StackAllocator{
private:
    void* head = nullptr;
    void* current = nullptr;
    void* end = nullptr;
public:

    StackAllocator(long input){
        head = std::malloc(input);
        current = head;
        end = (void*) ((size_t) input + (size_t) head);
    }

    // Allocates one object at the top of the stack, and allows you to pass parameters to that object's constructor through it's parameters
    template<typename T, typename... Args>
    T* alloc(Args&&... args) {
        std::size_t bytes_needed = sizeof(T);
        if ((std::size_t) current + bytes_needed >= (std::size_t) end) {
            throw std::bad_alloc(); // Not enough memory
        }
        T* result = new (current) T(std::forward<Args>(args)...); // Placement new with arguments
        current = (void*)((std::size_t) current + bytes_needed);
        return result;
    }

    // Deallocate memory, you need to make sure what you are freeing is the last thing allocated
    // Deallocates one object that is assumed to be at the top of the stack
    template<typename T>
    void dealloc(T* ptr) {
        if (ptr != head) {
            current = (void*) ((long long) current - sizeof(ptr));
            ptr->~T(); // Call the destructor explicitly
        }
    }

    template<typename T>
    void mdealloc(T* ptr){
        current = (void*) ((long long) current - sizeof(ptr));
    }

    template<typename T>
    T* peek(){
        return (T*) current;
    }
};
#endif