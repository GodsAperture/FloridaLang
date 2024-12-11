#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

Node* Parser::parse(){
    //If it's not the nullptr, then it's successful.
    return this->program();
};

//List of all statements that can exist in a program.
Program* Parser::programList(){
    Node* thisNode = nullptr;
    Program* thisProgram = nullptr;

    //Check for an assignment.
    thisNode = assignment();
    if(thisNode != nullptr){
        thisProgram = stack->alloc<Program>(thisNode);
        while(given[iter].getName() == ";"){
            iter++;
        }
        return thisProgram;
    }

    //Check for an expression.
    thisNode = p0();
    if(thisNode != nullptr){
        thisNode = stack->alloc<Program>(thisNode);
        while(given[iter].getName() == ";"){
            iter++;
        }
        return thisProgram;
    }

    //If nothing works, then there's an error. Return a nullptr.
    return thisProgram;

}

Node* Parser::program(){
    Node* thisNode = nullptr;
    Program* thisProgram = nullptr;
    Program* start = stack->alloc<Program>(nullptr);
    Program* current = start;
    bool passed = false;

    //Check for an assignment;
    if(!passed){
        thisNode = assignment();
        if(thisNode != nullptr){
            current->head = thisProgram = stack->alloc<Program>(thisNode);
            while(given[iter].getName() == ";"){
                iter++;
            }
            passed = true;
        }
    }

    //Check for an expression;
    if(!passed){
        thisNode = p0();
        if(!passed && thisNode != nullptr){
            current->head = thisProgram = stack->alloc<Program>(thisNode);
            while(given[iter].getName() == ";"){
                iter++;
            }
            passed = true;
        }
    }
    
    while(thisProgram != nullptr){
        //If all tokens have been consumed, then terminate parsing.
        if(given.size() == iter){
            break;
        }
        thisProgram = nullptr;

        //Check for another statement.
        //A nullptr means there has been an error.
        thisProgram = programList();
        if(thisProgram != nullptr){
            current->Append(thisProgram);
            current = current->next;
        } else {
            printf("There has been an error in the parser.\nCheck Parser::program()");
            //An error has occured in the parser.
            return nullptr;
        }
    }

    return start;

}

Node* Parser::assignment(){
    Node* left = initialize();        
    Node* right = nullptr;
    Node* initial = stack->peek<Node>();

    //Check for a basic reassignment to an existing variable.
    if(left == nullptr && given[iter].type == FloridaType::Identifier){
        left = stack->alloc<Variable>("Null", given[iter].name);
        iter++;
    }

    //if an assignment is found
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

    //Check for a second identifier, otherwise it's a reassignment.
    if(given[iter].type == FloridaType::Identifier){
        std::string name = given[iter].name;
        iter++;

        return stack->alloc<Initialize>(adjective, name);
    } else {
        iter--;
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



