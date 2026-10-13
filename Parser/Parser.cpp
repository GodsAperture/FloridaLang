#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <cstring>
#include <iostream>

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
    if(iter == given.size()){
        return result;
    }

    std::string name = std::string(given[iter].name);
    iter++;
    while(currentScope != nullptr){
        currentFunction = currentScope->functions;
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

    result->errorType = PError::PMissingSubexpression;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "???";

    errors.push_back(result);
    return result;
}

Error* Parser::MissingLParentheses(){
    Error* result = stack->alloc<Error>();

    result->errorType = PError::PMissingLParentheses;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"(\"";

    errors.push_back(result);
    return result;
}

Error* Parser::MissingRParentheses(){
    Error* result = stack->alloc<Error>();

    result->errorType = PError::PMissingRParentheses;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\")\"";

    errors.push_back(result);
    return result;
}

Error* Parser::MissingLCurlyBrace(){
    Error* result = stack->alloc<Error>();

    result->errorType = PError::PMissingLCurlyBrace;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"{\"";

    errors.push_back(result);
    return result;
}

Error* Parser::MissingRCurlyBrace(){
    Error* result = stack->alloc<Error>();

    result->errorType = PError::PMissingRCurlyBrace;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\")\"";

    errors.push_back(result);
    return result;
}

Error* Parser::MissingLBracket(){
    Error* result = stack->alloc<Error>();

    result->errorType = PError::PMissingLBracket;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"[\"";

    errors.push_back(result);
    return result;
}

Error* Parser::MissingRBracket(){
    Error* result = stack->alloc<Error>();

    result->errorType = PError::PMissingLBracket;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\"]\"";

    errors.push_back(result);
    return result;
}

Error* Parser::MissingSemicolon(){
    Error* result = stack->alloc<Error>();

    result->errorType = PError::PMissingSemicolon;
    result->body = bodyStack[bodyStack.size() - 1];
    result->row = given[iter - 1].row;
    result->column = given[iter - 1].column;
    result->placeHolder = "\";\"";

    errors.push_back(result);
    return result;
}



//Mathy stuff
Node* Parser::add(){
    Node* left = nullptr;
    Node* right = nullptr;
    Addition* result = nullptr;

    left = subtract();
    if(left == nullptr){
        return nullptr;
    }

    //Failing here is not an error.
    if(!check("+")){
        return left;
    }

    result = stack->alloc<Addition>();
    result->left = left;

    right = add();
    if(right == nullptr){
        result->right = MissingSubexpression();
        return result;
    }

    result->right = right;
    result->type = returnType(left->type, right->type);

    return result;
}

Node* Parser::subtract(){
    Node* left = nullptr;
    Node* right = nullptr;
    Subtraction* result = nullptr;

    left = multiply();
    if(left == nullptr){
        return nullptr;
    }

    //Failing here is not an error.
    if(!check("-")){
        return left;
    }

    result = stack->alloc<Subtraction>();
    result->left = left;

    right = subtract();
    if(right == nullptr){
        return MissingSubexpression();
    }

    result->right = right;
    result->type = returnType(left->type, right->type);

    return result;
}

Node* Parser::multiply(){
    Node* left = nullptr;
    Node* right = nullptr;
    Multiplication* result = nullptr;

    left = divide();
    if(left == nullptr){
        return nullptr;
    }

    //Failing here is not an error.
    if(!check("*")){
        return left;
    }

    result = stack->alloc<Multiplication>();
    result->left = left;

    right = multiply();
    if(right == nullptr){
        result->right = MissingSubexpression();
        return result;
    }

    
    result->right = right;
    result->type = returnType(left->type, right->type);

    return result;
}

Node* Parser::divide(){
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

Node* Parser::primitive(){
    Node* pointer = nullptr;

    if(check("-")){
        Negative* expression = stack->alloc<Negative>();
        expression->right = multiply();

        return expression;
    }
    pointer = parentheses();
    if(pointer != nullptr){
        return pointer;
    }



    //Check for numbers.
    if(checkType(FloridaType::ufixed8)){
        Primitive* number = stack->alloc<Primitive>();
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

    //Check for function calls.
    pointer = call();
    if(pointer != nullptr){
        return pointer;
    }
    //Check for variables.
    pointer = variable();
    if(pointer != nullptr){
        return pointer;
    }
    //No primitives were found.
    return nullptr;
}

Node* Parser::parentheses(){
    Parentheses* result = nullptr;
    if(check("(")){
        result = stack->alloc<Parentheses>();

        result->subexpression = arguments();
        if(result->subexpression == nullptr){
            return MissingSubexpression();
        }

        if(check(")")){
            return result;
        } else {
            Error* thing = MissingRParentheses();
            thing->body = result->subexpression;
            return thing;
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
    Start start = currInfo();
    Node* temp = nullptr;
    Body* result = stack->alloc<Body>();
    bodyStack.push_back(result);
    
    //Look out for anything familiar such as assignments or if statements.
    temp = commonExpressions();
    if(temp == nullptr){
        reset(start);
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
Node* Parser::commonExpressions(){
    Node* result;

    result = call();
    if(result != nullptr){
        return result;
    }

    result = Return();
    if(result != nullptr){
        return result;
    }

    result = function();
    if(result != nullptr){
        return result;
    }

    result = object();
    if(result != nullptr){
        return result;
    }

    result = assignment();
    if(result != nullptr){
        if(!check(";")){
            MissingSemicolon();
        }
        return result;
    }

    result = initialize();
    if(result != nullptr){
        if(!check(";")){
            MissingSemicolon();
        }
        return result;
    }

    result = IF();
    if(result != nullptr){
        return result;
    }

    result = OR();
    if(result != nullptr){
        if(!check(";")){
            MissingSemicolon();
        }
        return result;
    }

    result = FOR();
    if(result != nullptr){
        return result;
    }
    
    result = WHILE();
    if(result != nullptr){
        return result;
    }

    //Reaching this is not problematic.
    return nullptr;
}

//Convenient method
Node* Parser::commonStatements(){
    Node* result = nullptr;

    //Check for comparison statements.
    result = OR();
    if(result != nullptr){
        return result;
    }

    //Check for assignments.
    result = assignment();
    if(result != nullptr){
        return result;
    }

    //Reaching this is problematic.
    return nullptr;

}

//if statement
Node* Parser::IF(){ 
    Node* condition = nullptr;
    IfClass* result = nullptr;

    //Check to see if the syntax matches properly.
    if(check("if")){
        result = stack->alloc<IfClass>();
        result->ifBody->name = given[iter - 1].name;
        uint64_t ifPosition = iter - 2; 

        condition = OR();
        //Get the condition of the if statement.
        if(condition == nullptr){
            result->condition = MissingSubexpression();
        }
        //Check to see if the parentheses is there.
        if(!check(")")){
            MissingLParentheses();
        }
        //Check for the end of the condition and the start of the body.
        if(!check("{")){
            MissingLCurlyBrace();
        }

        //Check for the if body.
        result->ifBody = scope();
        result->ifBody->name = given[ifPosition].name;
        if(!check("}")){
            MissingRCurlyBrace();
        }

        //Check for an else body.
        if(!check("else")){
            return result;
        }

        if(!check("{")){
            MissingLCurlyBrace();
        }

        uint64_t elsePosition = iter - 2;
        result->elseBody = scope();
        result->elseBody->name = given[elsePosition].name;

        if(!check("}")){
            MissingRCurlyBrace();
        }

        return result;

    }

    return nullptr;

}

//for loop
Node* Parser::FOR(){
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
Node* Parser::WHILE(){
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
Node* Parser::variable(){
    if(!hasTokens(1)){
        return nullptr;
    }

    //Check to see if the variable is accessible from the current scope.
    if(stack->currentScope->hasVariable(given[iter].name)){
        iter++;
        return stack->currentScope->getVariable(given[iter - 1].name);
    }

    //This isn't an error, just that a variable wasn't found.
    return nullptr;
}

Node* Parser::initialize(){
    if(!hasTokens(3)){
        return nullptr;
    }
    
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
    Node* left = dereference(stack->currentScope);
    if(left == nullptr){
        return nullptr;
    }
    Assignment* result = nullptr;

    if(check("=")){
        result = stack->alloc<Assignment>();
        result->left = left;
        result->right = commonExpressions();

        return result;
        if(result->right == nullptr){
            result->right = MissingSubexpression();
        }

        return result;
    }

    //This could be an object that has a method applied to it.
    //"Object.method();" as an exmaple.
    return left;
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

Node* Parser::dereference(Scope* input){
    if(!hasTokens(1)){
        return nullptr;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    Dereference* result = nullptr;

    left = memberAccess(input);
    if(left == nullptr){
        return nullptr;
    }

    if(!check("->")){
        return left;
    }

    right = dereference(input);
    if(right == nullptr){
        //Error
    }

    result = stack->alloc<Dereference>();
    result->left = left;
    result->right = right;

    return result;

}

Node* Parser::memberAccess(Scope* input){
    if(!hasTokens(1)){
        return nullptr;
    }
    Variable* left = nullptr;
    Node* right = nullptr;
    MemberAccess* result = nullptr;

    if(input->hasVariable(given[iter].name)){
        left = input->getVariable(given[iter].name);
        if(left == nullptr){
            return nullptr;
        }
        iter++;

        if(!check(".")){
            return right;
        }

        right = memberAccess(left->objectType->code);
        if(right == nullptr){
            //Error
        }

        result = stack->alloc<MemberAccess>();
        result->left = left;
        result->right = right;

        return result;
    }

    return result;

}



//Function stuff
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

Node* Parser::initializeArguments(){
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

Node* Parser::arguments(){
    Arguments* result = nullptr;
    Node* argument = nullptr;

    argument = returnableExpressions();
    //Check to see if there was an expression.
    if(argument != nullptr){
        if(check(",")){
            result = stack->alloc<Arguments>();
            result->current = argument;
            result->next = arguments();
            //If nothing was returned when there is an expected expression,
            //then it is an error.
            if((result->next = nullptr)){
                result->next = MissingSubexpression();
            }
        } else {
            return argument;
        }

        return result;
    }
    return nullptr;
}

Node* Parser::returnableExpressions(){
    Node* result;

    result = call();
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
Node* Parser::call(){
    //If there are any functions of this name in the
    //current scope or higher scope, then they will be found.
    std::vector<Function*> allNamedFunctions = findFunctions();
    FunctionCall* result = nullptr;

    if(allNamedFunctions.size() == 0){
        return nullptr;
    }
    if(check("(")){
        Node* args = arguments();
    } else {
        //Something about returning a function pointer.
        return nullptr;
    }
    if(check(")")){

    } else {
        Error* error = MissingRParentheses();
        error->body = result;
        return error;
    }
    // if(false){
    //     //Find out which function it is.
    //     Scope* tempScope = stack->currentScope;
    //     Scope* oldScope = tempScope;
    //     while(tempScope->funGet(given[iter].getName()) == nullptr){
    //         tempScope = tempScope->parent;
    //         if(tempScope == nullptr){
    //             std::cout << "Function '" << name << "' was not found in any reachable scope.\n";
    //             return nullptr;
    //         }
    //     }
    //     iter++;
    //     iter++;
    //     //Create the call object, and adjust the scope to a new scope.
    //     //If I don't, variables/functions might not be grabbed from the proper scope.
    //     FunctionCall* result = stack->alloc<FunctionCall>();
    //     result->function = tempScope->funGet(name);
    //     result->arguments = args;
    //     //Adjust the scope here.
    //     stack->currentScope = result->function->code;
    //     result->arguments = arguments();
        
    //     if(!check(")")){
    //         MissingRParentheses();
    //     }

    //     //Readjust the scope to the previous scope.
    //     stack->currentScope = oldScope;

    //     return result;

    // }

    return nullptr;

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
