#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

Node* Parser::parse(){
    //If it's not the nullptr, then it's successful.
    return this->program();
};

Node* Parser::program(){
    Node* thisNode = nullptr;
    Node* start = stack->peek<Node>();

    do{
        thisNode = nullptr;
        //Check for an assignment;
        thisNode = assignment();
        if(thisNode != nullptr){
            continue;
        }
        //Check for an expression;
        thisNode = p0();
        if(thisNode != nullptr){
            continue;
        }

        if(thisNode == nullptr){
            break;
        }

    }while(thisNode != nullptr);

    return start;

}

Node* Parser::assignment(){
    Node* left;
    Node* right = nullptr;
    std::string current = given[iter].getName();

    if(given[iter].getType() == FloridaType::Identifier){
        //Put the variable on the stack
        left = stack->alloc<Variable>(given[iter].getName());
        //Put the expression on the stack
        right = p0();
        if(right != nullptr){
            left = stack->alloc<Assignment>(left, right);
        } else {
            return nullptr;
        }
    }

    return nullptr;

}

//Mathy stuff
Node* Parser::p0(){
    Node* initial = stack->peek<Node>();
    int start = iter;

    Node* left = p1();
    std::string current = given[iter].getName();

    while(current == "+" || current == "-"){
        iter++;
        Node* right = p1();
        //Check for nullptrs;
        if(right == nullptr){
            stack->dealloc(initial);
            iter = start;

            return nullptr;
        }
        if(current == "+"){
            left = stack->alloc<Add>(left, right);
        }
        if(current == "-"){
            left = stack->alloc<Subtract>(left, right);
        }
        current = given[iter].getName();
    }

    return left;    
}

Node* Parser::p1(){
    Node* initial = stack->peek<Node>();
    int start = iter;

    Node* left = p2();
    std::string current = given[iter].getName();

    if(left == nullptr){
        return nullptr;
    }

    while(current == "*" || current == "/"){
        iter++;
        Node* right = p2();
        //Check for nullptrs;
        if(right == nullptr){
            stack->dealloc(initial);
            iter = start;

            return nullptr;
        }
        if(current == "*"){
            left = stack->alloc<Multiply>(left, right);
        }
        if(current == "/"){
            left = stack->alloc<Divide>(left, right);
        }

        current = given[iter].getName();
    }

    return nullptr;

}

Node* Parser::p2(){
    std::string current = given[iter].getName();
    if(current == "-"){
        Node* expression;
        //Increment for the left parenthesis;
        iter++;
        expression = stack->alloc<Negative>(p1());

        return expression;
    }
    if(current == "("){
        Node* expression;
        //Increment for the left parenthesis;
        iter++;
        expression = stack->alloc<Parnetheses>(p0());
        //Increment for the right parenthesis;
        iter++;

        return expression;
    }
    if(given[iter].getType() == FloridaType::fix64){
        fixed64* number = stack->alloc<fixed64>(given[iter].getName());
        iter++;

        return number;
    }

    return nullptr;
}