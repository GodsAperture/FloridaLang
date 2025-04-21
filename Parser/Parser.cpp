#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

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

Node* Parser::multiply(){
    int64_t start = iter;
    Node* left = nullptr;

    left = p2();
    if(left == nullptr){
        return nullptr;
    }

    std::string current = given[iter].getName();
    iter++;

    if(current != "*"){
        stack->dealloc(left);
        iter = start;
        return nullptr;
    }

    Node* right = p1();
    if(right == nullptr){
        stack->dealloc(right);
        iter = start;
        return nullptr;
    }

    return stack->alloc<Multiply>(left, right);

}

Node* Parser::divide(){
    int64_t start = iter;
    Node* left = p2();

    //Check to see if an instance of p2 exists.
    if(left == nullptr){
        return nullptr;
    }

    //Get the token to see if it's + or not.
    std::string current = given[iter].getName();
    iter++;

    //If it's not + then backtrack.
    if(current != "/"){
        stack->dealloc(left);
        iter = start;
        return nullptr;
    }

    Node* right = p1();

    if(right == nullptr){
        stack->dealloc(right);
        iter = start;
        return nullptr;
    }

    return stack->alloc<Divide>(left, right);

}

Node* Parser::p1(){
    Node* result = nullptr;

    result = multiply();
    if(result != nullptr){
        return result;
    }

    result = divide();
    if(result != nullptr){
        return result;
    }

    //If nothing, then it's not one of these.
    return nullptr;

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
    std::cout << given[iter].name + " is not an identifiable object.\n\t-Type number: " + std::to_string(given[iter].type) + "\n";
    error = true;

    return nullptr;
}



//Comparisons
Node* Parser::equal(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = p0();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "=="){
        iter++;
    } else {
        return nullptr;
    }

    right = p0();
    if(right == nullptr){
        //This means that the expression is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<Equal>(left, right);

}

Node* Parser::notEqual(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = p0();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "!="){
        iter++;
    } else {
        return nullptr;
    }

    right = p0();
    if(right == nullptr){
        //This means that the expression is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<NotEqual>(left, right);

}

Node* Parser::greaterThan(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = p0();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == ">"){
        iter++;
    } else {
        return nullptr;
    }

    right = p0();
    if(right == nullptr){
        //This means that the expression is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<GreaterThan>(left, right);

}

Node* Parser::greaterThanOr(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = p0();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == ">="){
        iter++;
    } else {
        return nullptr;
    }

    right = p0();
    if(right == nullptr){
        //This means that the expression is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<GreaterThanOr>(left, right);

}

Node* Parser::lessThan(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = p0();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "<"){
        iter++;
    } else {
        return nullptr;
    }

    right = p0();
    if(right == nullptr){
        //This means that the expression is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<LessThan>(left, right);

}

Node* Parser::lessThanOr(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = p0();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "<="){
        iter++;
    } else {
        return nullptr;
    }

    right = p0();
    if(right == nullptr){
        //This means that the expression is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<LessThanOr>(left, right);

}

//Helper function for comparisons.
Node* Parser::compare(){
    Node* thing = nullptr;
    
    thing = equal();
    if(thing != nullptr){
        return thing;
    }

    thing = notEqual();
    if(thing != nullptr){
        return thing;
    }

    thing = greaterThan();
    if(thing != nullptr){
        return thing;
    }

    thing = greaterThanOr();
    if(thing != nullptr){
        return thing;
    }

    thing = lessThan();
    if(thing != nullptr){
        return thing;
    }

    thing = lessThanOr();
    if(thing != nullptr){
        return thing;
    }

    return nullptr;
}



Node* Parser::OR(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "OR"){
        iter++;
    } else {
        return nullptr;
    }

    right = compare();
    if(right == nullptr){
        //If right returns a nullptr.
        //This means the statement is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<Or>(left, right);
}

Node* Parser::NOR(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "NOR"){
        iter++;
    } else {
        return nullptr;
    }

    right = compare();
    if(right == nullptr){
        //If right returns a nullptr.
        //This means the statement is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<Nor>(left, right);
}

Node* Parser::XOR(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "XOR"){
        iter++;
    } else {
        return nullptr;
    }

    right = compare();
    if(right == nullptr){
        //If right returns a nullptr.
        //This means the statement is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<Xor>(left, right);
}

Node* Parser::XNOR(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "XNOR"){
        iter++;
    } else {
        return nullptr;
    }

    right = compare();
    if(right == nullptr){
        //If right returns a nullptr.
        //This means the statement is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<Xnor>(left, right);
}

//Priority 1 booleans
Node* Parser::AND(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "AND"){
        iter++;
    } else {
        return nullptr;
    }

    right = compare();
    if(right == nullptr){
        //If right returns a nullptr.
        //This means the statement is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<And>(left, right);
}

Node* Parser::NAND(){
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "NAND"){
        iter++;
    } else {
        return nullptr;
    }

    right = compare();
    if(right == nullptr){
        //If right returns a nullptr.
        //This means the statement is malformed.
        error = true;
        return nullptr;
    }

    return stack->alloc<Nand>(left, right);
}

//Scopes
Scope* Parser::scope(){
    //Create a new scope for the object.
    Scope* newScope = nullptr;
    Body* newBody = nullptr;

    //Check for some body of code enclosed by {}.
    if(given[iter].name == "{"){
        iter++;
        newBody = body();
        
        //Check to see if there was some sort of body of code.
        if(newBody != nullptr){
            newScope = stack->alloc<Scope>(newBody, currScope);
            currScope = newScope;
        } else {
            //Something other than a scope was found.
            iter--;
            return nullptr;
        }
    } else {
        //This isn't an error, just that a scope wasn't found.
        //Scopes will always be enclosed by {}.
        //Except for the global scope.
        return nullptr;
    }

    //Reset the counter before leaving scope.
    variableCount = 0;
    //Return to the outerscope.
    currScope = currScope->parent;
    //Increment for the "}", otherwise error.
    if(given[iter].name == "}"){
        iter++;
    } else {
        std::cout << "Expected '}' at line " << given[iter].row << ".\n";
        error = true;
        return nullptr;
    }

    return newScope;
}
