#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <cstring>
#include <iostream>

Function* Parser::findClosestFunction(std::vector<Function*> functions, Arguments* input){
    Function* result = nullptr;
    std::vector<Function*> similarFunctions = std::vector<Function*>();

    //Edge case: `input` is a nullptr because there are no arguments.
    if(input == nullptr){
        for(size_t i = 0; i < functions.size(); i++){
            if(functions[i]->allArguments == nullptr){
                return functions[i];
            }
        }

        //No matching function was found, throw an error.
        return nullptr;
    }

    //Filter all the functions for the ones with equal lengths.
    for(size_t i = 0; i < functions.size(); i++){
        if(functions[i]->allArguments->length() == input->length()){
            similarFunctions.push_back(functions[i]);
        }
    }

    //Edge case error, no matching function was found.
    if(similarFunctions.size() == 0){
        return nullptr;
    }
    
    int64_t count = input->matchCount(functions[0]->allArguments);
    result = functions[0];

    //Check for anything that perfectly matches the function.
    //If one isn't found, this will also check for the closest matching one.
    for(size_t i = 0; i < similarFunctions.size(); i++){
        //If it's a perfect match, return it.
        if(similarFunctions[i]->allArguments->equals(input)){
            return similarFunctions[i];
        //Otherwise, just check to see if it's a better match or not.
        } else {
            //If it's a closer match, then use it instead.
            if(input->matchCount(functions[i]->allArguments) > count){
                count = input->matchCount(functions[i]->allArguments);
                result = functions[i];
            }
        }
    }

    //This is the closest matching function that is not a perfect match.
    return result;
}

uint64_t Parser::allocationSize(FloridaType input){
    switch(input){
        case FloridaType::Bool:
            return 1;
        case FloridaType::ufixed8:
            return 8;
        case FloridaType::ufixed4:
            return 4;
        case FloridaType::ufixed2:
            return 2;
        case FloridaType::ufixed1:
            return 1;
        case FloridaType::fixed8:
            return 8;
        case FloridaType::fixed4:
            return 4;
        case FloridaType::fixed2:
            return 2;
        case FloridaType::fixed1:
            return 1;
        case FloridaType::float8:
            return 8;
        case FloridaType::float4:
            return 4;
        default:
            return 0;
    }
}

void Parser::parse(){
    Scope* result = scope();
    //The entire program is essentially a scope.
    stack->AST = result;
};

std::string Parser::ToString(){
    return stack->AST->ToString("", "");
}

void Parser::CodePrint(){
    stack->AST->CodePrint("", "");
}

bool Parser::hasTokens(){
    return iter < given.size();
}

//Check if `given` has at least `input` tokens available.
bool Parser::hasTokens(int64_t input){
    return iter + input <= given.size();
}

Variable* Parser::findVariable(Scope* input){
    if(iter >= given.size()){
        return nullptr;
    }
    std::string_view name = given[iter].name;
    Scope* theScope = input;
    Initialize* theInitialization = nullptr;

    //Search each parent scope.
    while(theScope != nullptr){
        theInitialization = theScope->allInitializations;
        //Check all variables in a given scope.
        while(theInitialization != nullptr){
            if(theInitialization->thisVariable->thisToken.name == name){
                return theInitialization->thisVariable;
            }
            theInitialization = theInitialization->next;
        }
        theScope = theScope->parent;
    }

    return nullptr;
}

ThisKeyword* Parser::checkForThis(Scope* input){
    if(check("this")){
        ThisKeyword* result = stack->alloc<ThisKeyword>();
        result->objectType = objectStack[objectStack.size() - 1];
        return result;
    }

    return nullptr;
}

bool Parser::check(std::string inString){
    if(given.size() == iter){
        return false;
    }
    
    if(given[iter].getName() == inString){
        iter++;
        return true;
    }
    return false;
}

bool Parser::checkType(FloridaType input){
    if(iter >= given.size()){
        return false;
    }
    if(given[iter].type == input){
        iter++;
        return true;
    }

    return false;
}

bool Parser::check(std::initializer_list<std::string> input){
    if(given.size() == iter){
        return false;
    }
    std::vector args = input;
    for(size_t i = 0; i < args.size(); i++){
        if(given[iter].name == args[i]){
            iter++;
            return true;
        }
    }

    return false;
}

inline FloridaType adjustedType(FloridaType input){
    switch(input){
        case FloridaType::ufixed1:
            return FloridaType::ufixed4;
        case FloridaType::ufixed2:
            return FloridaType::ufixed4;
        case FloridaType::fixed1:
            return FloridaType::fixed4;
        case FloridaType::fixed2:
            return FloridaType::fixed4;
        default:
            return input;
    }
}

//This function will determine the return type of math operations.
inline FloridaType returnType(FloridaType left, FloridaType right){
    left = adjustedType(left);
    right = adjustedType(right);

    if(left > right){
        return left;
    } else {
        return right;
    }
}

std::vector<Function*> Parser::findFunctions(){
    std::vector<Function*> result = std::vector<Function*>();
    Scope* currentScope = stack->currentScope;
    Function* currentFunction = nullptr;
    //Check to see if there are any tokens to consume.
    if(iter >= given.size()){
        return result;
    }

    //Find all functions whose name match the token.
    std::string name = std::string(given[iter].name);
    iter++;
    //This is a while loop.
    while(currentScope != nullptr){
        currentFunction = currentScope->functions;
        //If the name matches, it goes into the vector.
        while(currentFunction != nullptr){
            if(currentFunction->name == name){
                result.push_back(currentFunction);
            }
        }
        currentScope = currentScope->parent;
    }

    return result;

}



//Error related methods.
Error* Parser::MissingSubexpression(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingSubexpression;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "???";

    errorStack.push_back(result);
    return result;
}

Error* Parser::MissingLParentheses(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingLParentheses;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"(\"";

    errorStack.push_back(result);
    return result;
}

Error* Parser::MissingRParentheses(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingRParentheses;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\")\"";

    errorStack.push_back(result);
    return result;
}

Error* Parser::MissingLCurlyBrace(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingLCurlyBrace;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"{\"";

    errorStack.push_back(result);
    return result;
}

Error* Parser::MissingRCurlyBrace(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingRCurlyBrace;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\")\"";

    errorStack.push_back(result);
    return result;
}

Error* Parser::MissingLBracket(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingLBracket;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"[\"";

    errorStack.push_back(result);
    return result;
}

Error* Parser::MissingRBracket(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingLBracket;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"]\"";

    errorStack.push_back(result);
    return result;
}

Error* Parser::MissingSemicolon(){
    Error* result = stack->alloc<Error>();

    result->type = FloridaType::TError;
    result->errorType = PError::PMissingSemicolon;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\";\"";

    errorStack.push_back(result);
    return result;
}



//Mathy stuff
UNode Parser::add(){
    UNode left;
    UNode right;
    UNode result;

    left = subtract();
    if(left.joker.node == nullptr){
        return result;
    }

    //Failing here is not an error.
    if(!check("+")){
        return left;
    }

    result = stack->alloc<Addition>();
    result.joker.addition->left = left;

    right = add();
    if(right.joker.node == nullptr){
        result.joker.addition->right = MissingSubexpression();
        return result;
    }

    result.joker.addition->right = right;
    result.joker.addition->type = returnType(left.joker.addition->type, right.joker.addition->type);

    return result;
}

UNode Parser::subtract(){
    UNode left;
    UNode right;
    UNode result;

    left = multiply();
    if(left.joker.node == nullptr){
        return result;
    }

    //Failing here is not an error.
    if(!check("-")){
        return left;
    }

    result = stack->alloc<Subtraction>();
    result.joker.subtraction->left = left;

    right = subtract();
    if(right.joker.node == nullptr){
        result.joker.subtraction->right = MissingSubexpression();
        return result;
    }

    result.joker.subtraction->right = right;
    result.joker.subtraction->type = returnType(left.joker.subtraction->type, right.joker.subtraction->type);

    return result;
}

UNode Parser::multiply(){
    UNode left;
    UNode right;
    UNode result;

    left = divide();
    if(left.joker.node == nullptr){
        return result;
    }

    //Failing here is not an error.
    if(!check("*")){
        return left;
    }

    result = stack->alloc<Multiplication>();
    result.joker.multiplication->left = left;

    right = multiply();
    if(right.joker.node == nullptr){
        result.joker.multiplication->right = MissingSubexpression();
        return result;
    }

    result.joker.multiplication->right = right;
    result.joker.multiplication->type = returnType(left.joker.multiplication->type, right.joker.multiplication->type);

    return result;
}

UNode Parser::divide(){
    Node* left = nullptr;
    Node* right = nullptr;
    Division* result = nullptr;

    left = primitive();
    if(left == nullptr){
        return nullptr;
    }

    //Failing here is not an error.
    if(!check("/")){
        return left;
    }

    result = stack->alloc<Division>();
    result->left = left;

    right = divide();
    if(right == nullptr){
        return MissingSubexpression();
    }

    result->right = right;
    result->type = returnType(left->type, right->type);

    return result;
}

UNode Parser::primitive(){
    UNode pointer;

    if(check("-")){
        UNode expression;
        expression = stack->alloc<Negative>();
        expression.joker.negative->right = multiply();

        return expression;
    }
    pointer = parentheses();
    if(pointer.joker.node != nullptr){
        return pointer;
    }



    //Check for numbers.
    if(checkType(FloridaType::ufixed8)){
        UNode number;
        number = stack->alloc<Primitive>();
        number->value.ufixed8 = std::stoull(given[iter - 1].getName());
        number->type = FloridaType::ufixed8;

        return number;
    }
    if(checkType(FloridaType::ufixed4)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.ufixed4[0] = std::stoul(given[iter - 1].getName());
        number->type = FloridaType::ufixed4;

        return number;
    }
    if(checkType(FloridaType::ufixed2)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.ufixed2[0] = std::stoul(given[iter - 1].getName());
        number->type = FloridaType::ufixed2;

        return number;
    }
    if(checkType(FloridaType::ufixed1)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.ufixed1[0] = std::stoul(given[iter - 1].getName());
        number->type = FloridaType::ufixed1;

        return number;
    }
    if(checkType(FloridaType::fixed8)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed8 = std::stoll(given[iter - 1].getName());
        number->type = FloridaType::fixed8;

        return number;
    }
    if(checkType(FloridaType::fixed4)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed4[0] = std::stol(given[iter - 1].getName());
        number->type = FloridaType::fixed4;

       return number;
    }
    if(checkType(FloridaType::fixed2)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed2[0] = std::stol(given[iter - 1].getName());
        number->type = FloridaType::fixed2;

        return number;
    }
    if(checkType(FloridaType::fixed1)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed1[0] = std::stol(given[iter - 1].getName());
        number->type = FloridaType::fixed1;

        return number;
    }
    if(checkType(FloridaType::float8)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.float8 = std::stod(given[iter - 1].getName());
        number->type = FloridaType::float8;

        return number;
    }
    if(checkType(FloridaType::float4)){
        Primitive* number = stack->alloc<Primitive>();
        number->value.float4[0] = std::stof(given[iter - 1].getName());
        number->type = FloridaType::float4;

        return number;
    }


    
    //Check for booleans.
    if(checkType(FloridaType::Bool)){
        Primitive* result = stack->alloc<Primitive>();
        if(given[iter - 1].getName() == "true"){
            result->value.boolean[0] = true;
            result->type = FloridaType::Bool;

            return result;
        } else {
            result->value.boolean[0] = false;
            result->type = FloridaType::Bool;
            
            return result;
        }
    }

    //Hail Mary attempt.
    pointer = memberAccess(stack->currentScope);

    //If a MemberAccess is not found, then fuck it, nothing was.
    return pointer;
}

Parentheses* Parser::mandatoryParentheses(){
    Parentheses* result = stack->alloc<Parentheses>();
    if(check("(")){
        result->subexpression = arguments();

        if(check(")")){
            return result;
        } else {
            result->MissingRP = MissingRParentheses();
            return result;
        }
    } else {
        result->MissingLP = MissingLParentheses();
        result->subexpression = arguments();

        if(!check(")")){
            result->MissingRP = MissingRParentheses();
        }

        return result;
    }
}

Parentheses* Parser::parentheses(){
    Parentheses* result = nullptr;
    if(check("(")){
        result = stack->alloc<Parentheses>();

        result->subexpression = arguments();

        if(check(")")){
            return result;
        } else {
            result->MissingRP = MissingRParentheses();
            return result;
        }
    }

    return nullptr;
}



//Helper function for comparisons.
Node* Parser::compare(){
    Node* left = nullptr;
    Node* right = nullptr;
    std::string name = "";

    left = add();
    if(left == nullptr){
        return nullptr;
    }
    
    int64_t namePlacement = iter;
    if(!check({"==", "!=", ">", ">=", "<", "<="})){
        return left;
    }

    right = add();
    if(right == nullptr){
        right = MissingSubexpression();
    }

    if(std::string(given[namePlacement].name) == "=="){
        Equal* result = stack->alloc<Equal>();
        result->left = left;
        result->right = right;

        return result;
    }

    if(std::string(given[namePlacement].name) == "!="){
        NotEqual* result = stack->alloc<NotEqual>();
        result->left = left;
        result->right = right;

        return result;
    }

    if(std::string(given[namePlacement].name) == ">"){
        GreaterThan* result = stack->alloc<GreaterThan>();
        result->left = left;
        result->right = right;

        return result;
    }

    if(std::string(given[namePlacement].name) == ">="){
        GreaterThanOr* result = stack->alloc<GreaterThanOr>();
        result->left = left;
        result->right = right;

        return result;
    }

    if(std::string(given[namePlacement].name) == "<"){
        LessThan* result = stack->alloc<LessThan>();
        result->left = left;
        result->right = right;

        return result;
    }

    if(std::string(given[namePlacement].name) == "<="){
        LessThanOr* result = stack->alloc<LessThanOr>();
        result->left = left;
        result->right = right;

        return result;
    }

    return nullptr;
}



Node* Parser::OR(){
    Node* left = nullptr;
    Or* result = nullptr;

    left = AND();
    if(left == nullptr){
        return nullptr;
    }

    if(!check("OR")){
        return left;
    }
    result = stack->alloc<Or>();

    result->left = left;
    result->right = OR();
    if(result->right == nullptr){
        return MissingSubexpression();
    }

    return result;

}

Node* Parser::AND(){
    Node* left = nullptr;
    And* result = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    if(!check("AND")){
        return left;
    }
    result = stack->alloc<And>();

    result->right = AND();
    if(result->right == nullptr){
        return MissingSubexpression();
    }

    return result;
}

void Parser::addScope(Scope* input){
    Scope* currentScope = stack->allScopes;
    if(stack->allScopes == nullptr){
        stack->allScopes = input;
        return;
    }

    while(currentScope->allScopes != nullptr){
        currentScope = currentScope->allScopes;
    }

    currentScope->allScopes = input;
}



//Scopes
Scope* Parser::scope(){
    //Create a new scope for the object.
    Scope* newScope = stack->alloc<Scope>();

    //Set the scope value for use later in the VM.
    newScope->whichScope = scopeCount;
    //Increment the scope counter in the parser.
    scopeCount++;

    //Assign the current scope as the parent of the new scope.
    newScope->parent = stack->currentScope;
    //Assign the newest scope to be the current scope.
    stack->currentScope = newScope;
    //If there is no other scope, then make it the global scope.
    if(stack->globalScope == nullptr){
        stack->globalScope = stack->currentScope;
    }

    //Get the body of code and attach it to the scope.
    newScope->body = body();
    //Return to the outerscope.
    stack->currentScope = stack->currentScope->parent;
    //Make sure to assign each variable an appropriate size.
    newScope->byteAssign();
    //Add it to all scopes.
    addScope(newScope);

    return newScope;
}

Body* Parser::body(){
    Node* temp = nullptr;
    Body* result = stack->alloc<Body>();
    bodyStack.push_back(result);
    
    //Look out for anything familiar such as assignments or if statements.
    temp = commonExpressions();
    if(temp == nullptr){
        bodyStack.pop_back();
        return nullptr;
    }
    
    result->current = temp;
    //Recursively call to check for another body of code.
    result->next = body();
    bodyStack.pop_back();

    return result;
}

//Convenient method
UNode Parser::commonExpressions(){
    UNode result;

    result = call(stack->currentScope);
    if(result.joker.node != nullptr){
        return result;
    }

    result = Return();
    if(result.joker.node != nullptr){
        return result;
    }

    result = function();
    if(result.joker.node != nullptr){
        return result;
    }

    result = object();
    if(result.joker.node != nullptr){
        return result;
    }

    result = assignment();
    if(result.joker.node != nullptr){
        if(!check(";")){
            MissingSemicolon();
        }
        return result;
    }

    result = initialize();
    if(result.joker.node != nullptr){
        if(!check(";")){
            MissingSemicolon();
        }
        return result;
    }

    result = IF();
    if(result.joker.node != nullptr){
        return result;
    }

    result = OR();
    if(result.joker.node != nullptr){
        if(!check(";")){
            MissingSemicolon();
        }
        return result;
    }

    result = FOR();
    if(result.joker.node != nullptr){
        return result;
    }
    
    result = WHILE();
    if(result.joker.node != nullptr){
        return result;
    }

    //Reaching this is not problematic.
    return result;
}

//Convenient method
UNode Parser::commonStatements(){
    UNode result;

    //Check for comparison statements.
    result = OR();
    if(result.joker.node != nullptr){
        return result;
    }

    //Check for assignments.
    result = assignment();
    if(result.joker.node != nullptr){
        return result;
    }

    //Reaching this is problematic.
    return result;

}

//if statement
IfClass* Parser::IF(){
    Node* condition = nullptr;
    IfClass* result = nullptr;

    //Check to see if the syntax matches properly.
    if(check("if")){
        result = stack->alloc<IfClass>();
        result->ifBody->name = given[iter - 1].name;
        uint64_t ifPosition = iter - 2;

        //Check for a left parentheses.
        if(!check("(")){
            result->MissingLP = MissingLParentheses();
        }

        condition = OR();
        //Get the condition of the if statement.
        if(condition == nullptr){
            result->condition = MissingSubexpression();
        }
        //Check for a right parentheses.
        if(!check(")")){
            result->MissingRP = MissingRParentheses();
        }
        //Check for a left curly brace.
        if(!check("{")){
            result->MissingIfLCB = MissingLCurlyBrace();
        }

        //Check for the if body.
        result->ifBody = scope();
        result->ifBody->name = given[ifPosition].name;
        //Check for a right curly brace.
        if(!check("}")){
            result->MissingIfRCB = MissingRCurlyBrace();
        }

        //Check for an else body.
        if(!check("else")){
            return result;
        }
        uint64_t elsePosition = iter - 1;

        if(!check("{")){
            result->MissingElseLCB = MissingLCurlyBrace();
        }

        result->elseBody = scope();
        result->elseBody->name = given[elsePosition].name;

        if(!check("}")){
           result->MissingElseRCB = MissingRCurlyBrace();
        }

        return result;

    }

    return nullptr;

}

//for loop
ForLoop* Parser::FOR(){
    if(check("for")){
        //If the body is a nullptr, then the user provided no code.
        Scope* thisScope = stack->alloc<Scope>();
        addScope(thisScope);
        ForLoop* result = stack->alloc<ForLoop>();
        result->body = thisScope;
        thisScope->name = given[iter - 2].name;

        //Assign the scopes its unique scope value.
        thisScope->whichScope = scopeCount;
        //Increment the scope counter.
        scopeCount++;

        //Check for a parentheses.
        if(!check("(")){
            MissingLParentheses();
        }

        //Make the current scope the new one.
        thisScope->parent = stack->currentScope;
        stack->currentScope = thisScope;

        //Get an assignment, if any.
        result->assign = initialize();
        if(!check(";")){
            MissingSemicolon();
        }

        //Check for a condition, if any.
        result->condition = compare();
        if(!check(";")){
            MissingSemicolon();
        }

        //Grab the incrementer, if any.
        result->incrementer = assignment();
        if(!check(")")){
            MissingRParentheses();
        }

        //Build the body of code for the loop.
        if(!check("{")){
            MissingLCurlyBrace();
        }
        result->body->body = body();

        //Check for the last brace to end the for loop.
        if(!check("}")){
            MissingRCurlyBrace();
        }

        //Return to the outer scope.
        stack->currentScope = stack->currentScope->parent;

        //Assign each variable a placement in the pack.
        result->body->byteAssign();

        return result;

    }

    return nullptr;
    
}

//while loop
WhileLoop* Parser::WHILE(){
    if(check("while")){
        int64_t namePlacement = iter - 1;
        WhileLoop* result = stack->alloc<WhileLoop>();

        if(!check("(")){
            MissingLParentheses();
        }

        //If this is a nullptr, then it will run perpetually.
        result->condition = OR();
        if(!check(")")){
            MissingLParentheses();
        }

        if(!check("{")){
            MissingLCurlyBrace();
        }

        result->body = scope();
        if(result->body == nullptr){
            //Error, I'm not sure how to handle it.
        }

        if(!check("}")){
            MissingRCurlyBrace();
        }
        
        result->body->name = given[namePlacement].name;

        //Assign each variable a placement in the pack.
        result->body->byteAssign();

        return result;
    }

    return nullptr;
}

//Variable stuff
Variable* Parser::variable(Scope* input){
    Variable* result = nullptr;

    result = findVariable(input);
    if(result != nullptr){
        return result;
    }

    result = checkForThis(input);
    if(result != nullptr){
        return result;
    }

    //This isn't an error, just that a variable wasn't found.
    return nullptr;
}

Node* Parser::initialize(){
    if(!hasTokens(3)){
        return nullptr;
    }

    //int64_t adverbs = descriptor();
    
    //Check to see if this matches a valid initialization.
    bool bool1 = (given[iter].getType() == FloridaType::Adjective) or stack->currentScope->hasObject(given[iter].name);
    std::string_view typeName = given[iter].name;
    FloridaType theType = typeReturn(given[iter].getName());
    bool bool2 = given[iter + 1].getType() == FloridaType::Identifier;
    Token theToken = given[iter + 1];
    theToken.type = theType;
    bool bool3 = given[iter + 2].getName() == "=";

    //Check for a plain initialization.
    if(bool1 & bool2){
        iter++;
        iter++;
        //This will be stack allocated in scope().
        Variable* newVariable = stack->alloc<Variable>();
        Initialize* result = stack->alloc<Initialize>();
        newVariable->thisToken = theToken;
        newVariable->owner = stack->currentScope;
        newVariable->type = theType;
        
        //The expected stack size will be larger because of the new variable.
        result->thisVariable = newVariable;
        //Add the variable to `allInitializations` and `sortedInitalizations`.
        stack->currentScope->push(result);

        //Check if it is a user defined object. If so, assign its object type.
        if(theType == FloridaType::Null){
            result->thisVariable->objectType = stack->currentScope->getObject(typeName);
        }

        //If there's an assignment operator, then there should be a statement that follows.
        if(bool3){
            iter++;
            result->code = commonStatements();
            if(result->code == nullptr){
                result->code = MissingSubexpression();
            }
        }

        return result;
    }

    //This isn't an error, just that an initialization wasn't found.
    return nullptr;
}

Node* Parser::assignment(){
    UNode left = dereference(stack->currentScope);
    if(left.joker.node == nullptr){
        return nullptr;
    }
    Assignment* result = nullptr;

    if(check("=")){
        result = stack->alloc<Assignment>();
        result->left = left.joker.node;
        result->right = commonExpressions();

        return result;
        if(result->right == nullptr){
            result->right = MissingSubexpression();
        }

        return result;
    }

    //This could be an object that has a method applied to it.
    //"Object.method();" as an exmaple.
    return left.joker.node;
}

Node* Parser::object(){
    if(!hasTokens(3)){
        return nullptr;
    }

    bool bool1 = given[iter].getName() == "object";
    std::string_view name = given[iter + 1].name;
    bool bool2 = given[iter + 2].getName() == "{";

    if(bool1 & bool2){
        iter++;
        iter++;
        iter++;
        ObjectClass* result = stack->alloc<ObjectClass>();
        result->type = FloridaType::Object;
        result->name = name;
        result->code = scope();
        result->code->associatedClass = result;

        if(!check("}")){
            //TO DO, debugging.
        }
        stack->currentScope->push(result);

        Initialize* currentInitialize = result->code->allInitializations;
        //Use this to determine the size of the object in memory.
        if(currentInitialize != nullptr){
            //The tail end of `memoryOrder` will have the variable
            //with the highest `stackBytePosition`. If I addOffset its
            //stackBytePosition to its size in memory, then I know
            //how large this object will be.
            while(currentInitialize->memoryOrder != nullptr){
                currentInitialize = currentInitialize->memoryOrder;
            }

            //Determine the size of the object allocation.
            if(currentInitialize->thisVariable->objectType == nullptr){
                result->memorySize = currentInitialize->thisVariable->stackBytePosition + allocationSize(currentInitialize->thisVariable->type);
            } else {
                result->memorySize = currentInitialize->thisVariable->stackBytePosition + currentInitialize->thisVariable->objectType->memorySize;
            }
        }
        return result;
    }

    return nullptr;

}

UNode Parser::dereference(Scope* input){
    UNode left;
    UNode right;
    UNode result;

    left = memberAccess(input);
    if(left.joker.node == nullptr){
        return left;
    }

    if(!check("->")){
        return left;
    }

    right = dereference(input);
    if(right.joker.node == nullptr){
        right.joker.node = MissingSubexpression();
    }

    result = stack->alloc<Dereference>();
    result.joker.dereference->left = left;
    result.joker.dereference->right = right;

    return result;
}

UNode Parser::memberAccess(Scope* input){
    UNode left;
    Scope* leftScope = nullptr;
    UNode right;
    UNode result;

    left = call(input);
    //Check to see if it's a call instead of an error.
    if(left.joker.node != nullptr){        
        leftScope = left.joker.functionCall->function->code;
    }

    left = variable(input);
    //Check to see if it's a variable instead of an error.
    if(left.joker.node != nullptr){
        leftScope = left.joker.variable->owner;
    }

    if(!check(".")){
        return left;
    }
    result = stack->alloc<MemberAccess>();
    result.joker.memberAccess->left = left;

    result.joker.memberAccess->right = memberAccess(leftScope).joker.node;

    return result;
}



//int64_t descriptor(){
//}

//Function stuff
//TO FIX
Node* Parser::function(){
    if(!hasTokens(3)){
        return nullptr;
    }
    //Check if the function has a non-void return statement.
    bool returnable = given[iter].getName() != "void";
    FloridaType returnType = typeReturn(given[iter].getName());
    bool bool1 = (given[iter].getType() == FloridaType::Adjective) or stack->currentScope->hasObject(given[iter].name);
    //Grab the function's name as a string_view.
    std::string_view name = given[iter + 1].name;
    bool bool2 = given[iter + 1].getType() == FloridaType::Identifier;

    if(bool1 & bool2 & (given[iter + 2].name == "(")){
        Function* result = stack->alloc<Function>();
        //I forgot to comment what this is.
        //I dunno what it is.
        //int64_t firstHalfStackSize = 0;
        result->previous = stack->currentFunction;  
        stack->currentFunction = result;
        result->returnable = returnable;
        result->name = name;

        //Include the function for use in the original scope.
        result->next = stack->currentScope->functions;
        stack->currentScope->functions = result;

        //Include the function in the allFunctions chain for the VM.
        result->allFunctions = stack->allFunctions;
        stack->allFunctions = result;

        result->type = returnType;
        Scope* newScope = stack->alloc<Scope>();
        addScope(newScope);
        newScope->associatedFunction = result;
        newScope->name = name;
        
        //Assign the scope its own unqiue value.
        newScope->whichScope = scopeCount;
        //Increment the scope counter.
        scopeCount++;

        //Adjust the current scope to be that of the function.
        newScope->parent = stack->currentScope;
        result->code = newScope;
        stack->currentScope = newScope;
        iter++;
        iter++;
        iter++;

        //Get all argument initializations, if any.
        result->allArguments = initializeArguments();

        if(!check(")")){
            MissingRParentheses();
        }

        if(!check("{")){
            MissingLCurlyBrace();
        }

        //It doesn't matter if this is a nullptr or not.
        result->code->body = body();

        if(!check("}")){
            MissingRCurlyBrace();
        }

        //Return the relevant function scope to the previous function.
        stack->currentFunction = stack->currentFunction->previous;
        //Return the scope to the previous scope.
        stack->currentScope = newScope->parent;

        //Assign each variable a placement in the pack.
        result->code->byteAssign();

        return result;

    }

    return nullptr;

}

Arguments* Parser::initializeArguments(){
    Arguments* result = nullptr;
    Node* subresult = nullptr;

    subresult = initialize();
    if(subresult != nullptr){
        result = stack->alloc<Arguments>();
        result->current = subresult;
        if(check(",")){
            result->next = initializeArguments();
        }

        return result;
    }

    return nullptr;
}

Arguments* Parser::arguments(){
    Node* code = nullptr;
    Arguments* result = nullptr;

    //Check for any sort  of subexpression.
    code = returnableExpressions();
    //If there is no code, then there is no argument.
    if(code == nullptr){
        return nullptr;
    }

    result = stack->alloc<Arguments>();
    result->current = code;

    //If there is a comma, then there is another argument.
    if(check(",")){
        result->next = arguments();
        //If there isn't a subexpression,
        //then place an error and continue checking.
        if(result->next->current == nullptr){
            result->next->current = MissingSubexpression();
        }
    }

    return result;
}

Node* Parser::returnableExpressions(){
    Node* result;

    result = call(stack->currentScope);
    if(result != nullptr){
        return result;
    }

    result = assignment();
    if(result != nullptr){
        if(!check(";")){
            MissingSubexpression();
        }
        return result;
    }

    result = OR();
    if(result != nullptr){
        return result;
    }

    //Reaching this IS a problem.
    return nullptr;
}

//TO FIX
FunctionCall* Parser::call(Scope* input){
    //If there are any functions of this name in the
    //current scope or higher scope, then they will be found.
    std::vector<Function*> allNamedFunctions = findFunctions();
    FunctionCall* result = nullptr;
    Parentheses* par = nullptr;

    //Check to see if there are any matching functions.
    //If not, then return a nullptr to show nothing was found.
    if(allNamedFunctions.size() == 0){
        return nullptr;
    }
    //Check for any arguments.
    par = parentheses();
    if(par == nullptr){
        //Something something return a function pointer.
        //For future me to fix.
        return nullptr;
    }

    result = stack->alloc<FunctionCall>();
    result->function = findClosestFunction(allNamedFunctions, par->subexpression);
    result->arguments = par->subexpression;

    return result;

}

Node* Parser::Return(){
    //Check for the string and then the statement that follows.
    if(check("return")){
        Node* statement = commonStatements();
        ReturnClass* result = stack->alloc<ReturnClass>();
        result->statement = statement;
        int64_t returnCount = 1;

        //currentScope will always be a deeper scope or the same scope as currFunct.
        Scope* currentScope = stack->currentScope;
        Scope* thisScope = stack->currentFunction->code;

        //The current function scope will always be in an outer scope if not the current one.
        while(currentScope != thisScope){
            returnCount++;
            currentScope = currentScope->parent;
        }

        //This is how many scopes to escape upon returning from the function.
        result->returnCount = returnCount;

        if(!check(";")){
            MissingSemicolon();
        }

        //Check if the return type differs from what is on the return line.
        if(statement->type != stack->currentFunction->type){
            TypecastClass* thing = stack->alloc<TypecastClass>();
            thing->type = stack->currentFunction->type;
            thing->body = statement;
            result->statement = thing;
        }

        return result;
    }
    //The ~~cake~~ return is a lie.
    return nullptr;

}
