#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

bool isObject(FloridaType inType){
	switch(inType){
		case Bool:
			return true;
		case fixed1:
			return true;
		case fixed2:
			return true;
		case fixed4:
			return true;
		case fixed8:
			return true;
		default:
			return false;
	}
}

void Parser::parse(){
    //If it's not the nullptr, then it's successful.
    result = p0();
};

std::vector<Instruction> Parser::FLVMCodeGen(){
    std::vector<Instruction> instructions = std::vector<Instruction>();
    result->FLVMCodeGen(instructions);
    return instructions;
}

//Mathy stuff
Node* Parser::p0(){
    Node* left = p1();
    std::string current = given[iter].getName();

    //Check for + or - operators between p1() expressions.
    while(current == "+" || current == "-"){
        iter++;
        Node* right = p1();
        //Check for any errors in the subexpression.
        if(right == nullptr){
            return nullptr;
        }
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
    //Check for negations
    std::string current = given[iter].getName();
    if(current == "-"){
        Node* expression;
        //Increment for the minus sign;
        iter++;
        expression = stack->alloc<Negative>(p1());

        return expression;
    }
    //Check for expression within parentheses.
    if(current == "("){
        Node* expression;
        //Increment for the left parenthesis;
        iter++;
        expression = stack->alloc<Parentheses>(p0());
        //Increment for the right parenthesis;
        iter++;

        return expression;
    }
    //Check for numbers.
    if(given[iter].type == FloridaType::fixed8){ 
        Fixed8* number = stack->alloc<Fixed8>(given[iter].getName());
        iter++;

        return number;
    }
    //Check for booleans.
    if(given[iter].type == FloridaType::Bool){
        if(given[iter].name == "true"){
            iter++;
            return stack->alloc<Boolean>(true);
        } else {
            iter++;
            return stack->alloc<Boolean>(false);
        }
    }
    //Undetectable object found.
    std::cout << given[iter].name + " is not an identifiable object.\nType number: " + std::to_string(given[iter].type) + "\n";
    error = true;

    return nullptr;
}



