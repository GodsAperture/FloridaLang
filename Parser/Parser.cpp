#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

Node* Parser::parse(){
    //If it's not the nullptr, then it's successful.
    return this->p0();
};

Node* Parser::p0(){
    Node* left = p1();
    std::string current = given[iter].getName();

    while(current == "+" || current == "-"){
        iter++;
        Node* right = p1();
        if(current == "+"){
            left = stack->alloc<Add>(left, right);
        } else {
            left = stack->alloc<Subtract>(left, right);
        }
        current = given[iter].getName();
    }

    return left;    
}

Node* Parser::p1(){
    Node* left = p2();
    std::string current = given[iter].getName();

    while(current == "*" || current == "/"){
        iter++;
        Node* right = p2();
        if(current == "*"){
            left = stack->alloc<Multiply>(left, right);
        } else {
            left = stack->alloc<Divide>(left, right);
        }   
        current = given[iter].getName();
    }

    return left;
}

Node* Parser::p2(){
    iter++;
    return stack->alloc<fixed64>(given[iter - 1].getName());
}