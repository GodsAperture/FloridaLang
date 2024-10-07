#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

bool Parser::parse(){
    //If it's not the nullptr, then it's successful.
    Node* tree = this->p0();
    bool isSuccessful = (tree != nullptr);
    //Print the statement back out to see if it even works.
    //I sure hope it does.
    std::string result1 = tree->ToString() + "\n";
    std::string result2 = tree->ToPostfix() + "\n";
    std::cout << result1 << result2;
    return isSuccessful;
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