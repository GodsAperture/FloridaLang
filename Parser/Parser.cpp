#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

void Parser::parse(){
    //If it's not the nullptr, then it's successful.

    result = body();
};

bool Parser::check(std::string inString){
    if(given[iter].name == inString){
        iter++;
        return true;
    }
    return false;
}

void Parser::FLVMCodeGen(){
    result->FLVMCodeGen(programInstructions);
}

//Mathy stuff
Node* Parser::AddSub(){
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    //Check for a subexpression.
    left = MulDiv();
    if(left == nullptr){
        return nullptr;
    }

    std::string current = given[iter].name;
    while(check("+") or check("-")){
        //Check for a product/division subexpression.
        right = MulDiv();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            iter = startIter;
            stack->dealloc(startPointer);
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
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = primitive();
    if(left == nullptr){
        return left;
    }

    std::string current = given[iter].name;
    while(check("*") or check("/")){
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
    if(check("-")){
        Node* expression;
        //Increment for the minus sign;
        expression = stack->alloc<Negative>(MulDiv());

        return expression;
    }
    //Check for expression within parentheses.
    if(check("(")){
        Node* expression;
        //Increment for the left parenthesis;
        expression = stack->alloc<Parentheses>(AddSub());
        //Increment for the right parenthesis;
        if(!check(")")){
            errorStack.push_back("Missing ')' on line " + given[iter].row);
        }

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
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        stack->dealloc(startPointer);
        iter = startIter;
        return nullptr;
    }

    if(!check("==")){
        stack->dealloc(startPointer);
        iter = startIter;
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
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        stack->dealloc(startPointer);
        iter = startIter;
        return nullptr;
    }

    if(given[iter].name == "!="){
        iter++;
    } else {
        stack->dealloc(startPointer);
        iter = startIter;
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
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        stack->dealloc(startPointer);
        iter = startIter;
        return nullptr;
    }

    if(!check(">")){
        stack->dealloc(startPointer);
        iter = startIter;
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
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        stack->dealloc(startPointer);
        iter = startIter;
        return nullptr;
    }

    if(check(">=")){
        stack->dealloc(startPointer);
        iter = startIter;
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
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        stack->dealloc(startPointer);
        iter = startIter;
        return nullptr;
    }

    if(!check("<")){
        iter = startIter;
        stack->dealloc(startPointer);
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
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        stack->dealloc(startPointer);
        iter = startIter;
        return nullptr;
    }

    if(check("<=")){
        stack->dealloc(startPointer);
        iter = startIter;
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
    uint64_t startIter = iter;
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AND();
    if(left == nullptr){
        return nullptr;
    }

    std::string current = given[iter].name;
    while(check("OR")){
        right = AND();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            iter = startIter;
            stack->dealloc(startPointer);
            return nullptr;
        }

        left = stack->alloc<Or>(left, right);
        current = given[iter].name;

    }

    return left;

}

Node* Parser::AND(){
    uint64_t startIter = iter;  
    Node* startPointer = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    std::string current = given[iter].name;
    while(check("AND")){
        right = compare();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            iter = startIter;
            stack->dealloc(startPointer);
            return nullptr;
        }

        left = stack->alloc<And>(left, right);
        current = given[iter].name;

    }

    return left;
}

//Scopes
Scope* Parser::scope(){
    //Create a new scope for the object.
    Scope* newScope = nullptr;
    Body* newBody = nullptr;

    //Check for some body of code enclosed by {}.
    if(check("{")){
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
    if(!check("}")){
        std::cout << "Expected '}' at line " << given[iter].row << ".\n";
        error = true;
        return nullptr;
    }

    return newScope;
}

Body* Parser::body(){
    Node* current = nullptr;
    Body* currentBody = nullptr;
    Body* bodyStart = nullptr;

    current = commonExpressions();
    bodyStart = stack->alloc<Body>(current);
    currentBody = bodyStart;

    while(currentBody != nullptr){
        current = commonExpressions();
        currentBody->next = stack->alloc<Body>(current);
        currentBody = currentBody->next;
        if(!check(";")){
            error = true;
            std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]";
        }
    }

    return bodyStart;
}

Node* Parser::commonExpressions(){
    Node* result;

    result = AddSub();
    if(result != nullptr){
        return result;
    }

    result = OR();
    if(result != nullptr){
        return result;
    }

    return nullptr;
}


//x++ returns x + 1;
//++x returns x;

void Parser::push(types input){
    computationVector.push_back(input);
}

types Parser::pop(){
    types result = computationVector.back();
    computationVector.pop_back();

    return result;
}

types Parser::top(){
    return computationVector.back();
}

bool Parser::next(){
    //left and right hand members for operations
    types left;
    types right;
    types result;

    //Check to see if all of instructions have been executed.
    if(instructionNumber == programInstructions.size()){
        return false;
    }

    switch (programInstructions[instructionNumber].oper){
        case Operation::cjump:
            //If it's true, then don't skip
            if(!top().boolean){
                //This is to adjust the position of the instruction number.
                instructionNumber = programInstructions[instructionNumber].literal.fixed64;
                //Move the position of the stack pointer back to "delete" the-
                //boolean being used in the prior slot.
            }
            return true;
        case Operation::jump:
            instructionNumber = programInstructions[instructionNumber].literal.fixed64;
            return true;
        case Operation::fetch:
            //Push the value into the stack.
            computationVector.push_back(computationVector[programInstructions[instructionNumber].literal.fixed64]);
            instructionNumber++;
            return true;
        case Operation::assign:
            computationVector[programInstructions[instructionNumber].literal.fixed64] = top();
            instructionNumber++;
            return true;
        case Operation::push:
            push(programInstructions[instructionNumber].literal);
            instructionNumber++;
            return true;
        case Operation::add:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 + right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case Operation::subtract:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 - right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case Operation::negate:
            //Negate the value;
            result.fixed64 = -pop().fixed64;
            push(result);
            instructionNumber++;
            return true;
        case Operation::multiply:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 * right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case Operation::divide:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 / right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case equals:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 == right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case nequals:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 != right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case greater:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 > right.fixed64;
            push(result);
            instructionNumber++;
            return true;
            return true;
        case greateror:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 >= right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case lesser:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 < right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case lesseror:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 <= right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case ior:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.boolean or right.boolean;
            push(result);
            instructionNumber++;
            return true;
        case iand:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.boolean and right.boolean;
            push(result);
            instructionNumber++;
            return true;
        case inot:
            //Get the right operand;
            right = pop();
            //Operate and push;
            result.boolean = !right.boolean;
            push(result);
            instructionNumber++;
            return true;
        default:
            std::cout << "Error: Unknown instruction given.\nThe instruction number is " + programInstructions[instructionNumber].oper;
            return false;
    }

}
