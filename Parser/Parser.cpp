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
    Program* start = stack->alloc<Program>(nullptr);
    Program* current = start;
    bool passed = false;

    //Check for an assignment;
    if(!passed){
        thisNode = assignment();
        if(thisNode != nullptr){
            passed = true;
            thisNode = current->head = stack->alloc<Program>(thisNode);
            while(given[iter].getName() == ";"){
                iter++;
            }
        }
    }
    //Check for an expression;
    if(!passed){
        thisNode = p0();
        if(thisNode != nullptr){
            passed = true;
            thisNode = current->head = stack->alloc<Program>(thisNode);
            while(given[iter].getName() == ";"){
                iter++;
            }
        }
    }

    while(thisNode != nullptr){
        thisNode = nullptr;
        if(given.size() == iter){
            break;
        }
        //Check for an assignment;
        thisNode = assignment();
        if(thisNode != nullptr){
            current->Append(stack->alloc<Program>(thisNode));
            current = current->next;
            while(given[iter].getName() == ";"){
                iter++;
            }
            continue;
        }
        //Check for an expression;
        thisNode = p0();
        if(thisNode != nullptr){
            current->Append(stack->alloc<Program>(thisNode));
            current = current->next;
            while(given[iter].getName() == ";"){
                iter++;
            }
            continue;
        }
    }

    return start;

}

Node* Parser::assignment(){
    Node* left = initialize();        
    Node* right = nullptr;
    Node* initial = stack->peek<Node>();

    //if an assignment is found, then generate a 
    if(given[iter].name == "="){
        iter++;
        //Increment the iterator, and then get p0().
        right = p0();
        if(right == nullptr){
            std::cout << "Error: No assignable expression was found.";
        } else {
            return stack->alloc<Assignment>(left, right);
        }
    } else {
        //If an assignment is not found, then return the initialization.
        return left;
    }

    stack->dealloc(initial);
    return nullptr;

}

Node* Parser::initialize(){
    //If there's not an identifier, then return a nullptr.
    if(given[iter].type != FloridaType::Identifier){
        return nullptr;
    }
    //Otherwise, try to find an initialization or an identifier.

    std::string adjective = given[iter].name;
    iter++;

    //Check for a second identifier, otherwise it's an error.
    if(given[iter].type == FloridaType::Identifier){
        std::string name = given[iter].name;
        iter++;

        return stack->alloc<Initialize>(adjective, name);
    } else {
        //This will be an error.
        return nullptr;
    }

}

//Mathy stuff
Node* Parser::p0(){
    Node* left = p1();
    std::string current = given[iter].getName();

    while(current == "+" || current == "-"){
        iter++;
        Node* right = p1();
        if(current == "+"){
            left = stack->alloc<Add>(left, right);
            current = given[iter].getName();
            continue;
        }
        if(current == "-"){
            left = stack->alloc<Subtract>(left, right);
            current = given[iter].getName();
            continue;
        }
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
            current = given[iter].getName();
            continue;
        }
        if(current == "/"){
            left = stack->alloc<Divide>(left, right);
            current = given[iter].getName();
            continue;
        }
    }

    return left;

}

Node* Parser::p2(){
    std::string current = given[iter].getName();
    if(current == "-"){
        Node* expression;
        //Increment for the minus sign;
        iter++;
        expression = stack->alloc<Negative>(p1());

        return expression;
    }
    if(current == "("){
        Node* expression;
        //Increment for the left parenthesis;
        iter++;
        expression = stack->alloc<Parentheses>(p0());
        //Increment for the right parenthesis;
        iter++;

        return expression;
    }
    if(given[iter].getType() == FloridaType::Identifier){
        Variable* thisVariable = stack->alloc<Variable>("Null", given[iter].getName());
        iter++;     

        return thisVariable;
    }
    if(given[iter].type == FloridaType::fixed64){
        Fixed64* number = stack->alloc<Fixed64>(given[iter].getName());
        iter++;

        return number;
    }

    return nullptr;
}