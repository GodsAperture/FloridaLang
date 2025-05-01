#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

void Parser::parse(){
    //If it's not the nullptr, then it's successful.
    result = AddSub();
};

void Parser::FLVMCodeGen(){
    result->FLVMCodeGen(programInstructions);
}

//Mathy stuff
Node* Parser::AddSub(){
    uint64_t startIter = iter;
    void* startPointer = stack->current;
    Node* left = nullptr;
    Node* right = nullptr;

    //Check for a subexpression.
    left = MulDiv();
    if(left == nullptr){
        return nullptr;
    }

    std::string current = given[iter].name;
    while(current == "+" or current == "-"){
        //Increment for either + or -
        iter++;

        //Check for a product/division subexpression.
        right = MulDiv();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            iter = startIter;
            stack->current = startPointer;
            return nullptr;
        }

        //Check for any sort of sum or difference.
        if(current == "+"){
            left = stack->alloc<Add>(left, right);
            current = given[iter].name;
            continue;
        }
        if(current == "-"){
            left = stack->alloc<Subtract>(left, right);
            current = given[iter].name;
            continue;
        }
    }

    return left;

}

Node* Parser::MulDiv(){
    uint64_t startIter = iter;
    void* startPointer = stack->current;
    Node* left = nullptr;
    Node* right = nullptr;

    left = primitive();
    if(left == nullptr){
        return left;
    }

    std::string current = given[iter].name;
    while(current == "*" or current == "/"){
        //Increment for either * or /
        iter++;

        //Check for a primitive.
        right = primitive();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            iter = startIter;
            stack->current = startPointer;
            return nullptr;
        }

        //Start checking for products or divisions.
        if(current == "*"){
            left = stack->alloc<Multiply>(left, right);
            current = given[iter].name;
            continue;
        }
        if(current == "/"){
            left = stack->alloc<Divide>(left, right);
            current = given[iter].name;
            continue;
        }
    }

    return left;

}

Node* Parser::primitive(){
    //Check for negations
    std::string current = given[iter].getName();
    if(current == "-"){
        Node* expression;
        //Increment for the minus sign;
        iter++;
        expression = stack->alloc<Negative>(MulDiv());

        return expression;
    }
    //Check for expression within parentheses.
    if(current == "("){
        Node* expression;
        //Increment for the left parenthesis;
        iter++;
        expression = stack->alloc<Parentheses>(AddSub());
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

    left = AddSub();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "=="){
        iter++;
    } else {
        return nullptr;
    }

    right = AddSub();
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

    left = AddSub();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "!="){
        iter++;
    } else {
        return nullptr;
    }

    right = AddSub();
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

    left = AddSub();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == ">"){
        iter++;
    } else {
        return nullptr;
    }

    right = AddSub();
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

    left = AddSub();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == ">="){
        iter++;
    } else {
        return nullptr;
    }

    right = AddSub();
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

    left = AddSub();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "<"){
        iter++;
    } else {
        return nullptr;
    }

    right = AddSub();
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

    left = AddSub();
    if(left == nullptr){
        return nullptr;
    }

    if(given[iter].name == "<="){
        iter++;
    } else {
        return nullptr;
    }

    right = AddSub();
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

Body* Parser::body(){
    Body* result = nullptr;

    //TODO
    return result;
}


//x++ returns x + 1;
//++x returns x;

bool Parser::next(){
    //Check to see if all of instructions have been executed.
    if(instructionNumber == programInstructions.size()){
        return false;
    }

    switch (programInstructions[instructionNumber].oper){
        case Operation::cjump:
            //If it's true, then don't skip
            if(!computationVector[computationPointer - 1].boolean){
                //This is to adjust the position of the instruction number.
                instructionNumber = programInstructions[instructionNumber].literal.fixed64;
                //Move the position of the stack pointer back to "delete" the-
                //boolean being used in the prior slot.
                computationPointer--;
            }
            return true;
        case Operation::jump:
            instructionNumber = programInstructions[instructionNumber].literal.fixed64;
            return true;
        case Operation::fetch:
            //Push the value into the stack.
            computationVector[computationPointer++] = computationVector[programInstructions[instructionNumber].literal.fixed64];
            instructionNumber++;
            return true;
        case Operation::assign:
            if(programInstructions[instructionNumber].literal.fixed64 == -1){
                printf("Error: bad assignment position\n");
                return;
                //If there isn't another assignment instruction next, then-
                //the current value is no longer needed.
                if(programInstructions[instructionNumber + 1].oper != Operation::assign){
                    computationPointer--;
                }
            }
            computationVector[programInstructions[instructionNumber].literal.fixed64] = computationVector[computationPointer - 1];
            instructionNumber++;
            return true;
        case Operation::push:
            computationVector[computationPointer++] = programInstructions[instructionNumber].literal;
            instructionNumber++;
            return true;
        case Operation::add:
            //Get the right operand;
            right = computationVector[--computationPointer];
            //Get the left operand;
            left = computationVector[--computationPointer];
            //Operate and push;
            computationVector[computationPointer++].fixed64 = left.fixed64 + right.fixed64;
            instructionNumber++;
            return true;
        case Operation::subtract:
            //Get the right operand;
            right = computationVector[--computationPointer];
            //Get the left operand;
            left = computationVector[--computationPointer];
            //Operate and push;
            computationVector[computationPointer++].fixed64 = left.fixed64 - right.fixed64;
            instructionNumber++;
            return true;
        case Operation::negate:
            //Negate the value;
            computationVector[computationPointer].fixed64 = -computationVector[computationPointer].fixed64;
            instructionNumber++;
            return true;
        case Operation::multiply:
            //Get the right operand;
            right = computationVector[--computationPointer];
            //Get the left operand;
            left = computationVector[--computationPointer];
            //Operate and push;
            computationVector[computationPointer++].fixed64 = left.fixed64 * right.fixed64;
            instructionNumber++;
            return true;
        case Operation::divide:
            //Get the right operand;
            right = computationVector[--computationPointer];
            //Get the left operand;
            left = computationVector[--computationPointer];
            //Operate and push;	
            computationVector[computationPointer++].fixed64 = left.fixed64 / right.fixed64;
            instructionNumber++;
            return true;
        case equals:
            //Get the right operand;
            right = computationVector[--computationPointer];
            //Get the left operand;
            left = computationVector[--computationPointer];
            //Operate and push;
            computationVector[computationPointer++].boolean = left.fixed64 == right.fixed64;
            instructionNumber++;
            return true;
        case nequals:
            //Get the right operand;
            right = computationVector[--computationPointer];
            //Get the left operand;
            left = computationVector[--computationPointer];
            //Operate and push;
            computationVector[computationPointer++].boolean = left.fixed64 != right.fixed64;
            instructionNumber++;
            return true;
        default:
            std::cout << "Error: Unknown instruction given.\nThe instruction number is " + programInstructions[instructionNumber].oper;
            return true;
    }

}