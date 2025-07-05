#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

void Parser::parse(){
    //The entire program is essentially a scope.
    result = scope();
    //Just a useful debugger to make sure I'm creating the AST properly.
    if(result == nullptr){
        std::cout << "Failed to parse.\n";
        return;
    } else {
        std::cout << result->ToString("", "") << "\n";
    }
};

bool Parser::hasTokens(){
    return iter < given.size();
}

bool Parser::hasTokens(int64_t input){
    return iter + input < given.size();
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



//Mathy stuff
Node* Parser::AddSub(){
    if(!hasTokens(1)){
        return nullptr;
    }
    
    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    //Check for a subexpression.
    left = MulDiv();
    if(left == nullptr){
        return nullptr;
    }

    std::string current = given[iter].getName();
    while(check("+") or check("-")){
        //Check for a product/division subexpression.
        right = MulDiv();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            reset(start);
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
    if(!hasTokens(1)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = primitive();
    if(left == nullptr){
        return left;
    }

    std::string current = given[iter].getName();
    while(check("*") or check("/")){
        //Check for a primitive.
        right = primitive();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            reset(start);
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
            errorStack.push_back("Missing ')' on line " + std::to_string(given[iter].row));
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
        if(given[iter].getName() == "true"){
            iter++;
            return stack->alloc<Boolean>(true);
        } else {
            iter++;
            return stack->alloc<Boolean>(false);
        }
    }
    Node* thisVariable = variable();
    if(thisVariable != nullptr){
        return thisVariable;
    }

    //No primitives were found.
    return nullptr;
}



//Comparisons
Node* Parser::equal(){
    if(!hasTokens(1)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        reset(currInfo());
        return nullptr;
    }

    if(!check("==")){
        reset(start);
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
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        reset(start);
        return nullptr;
    }

    if(given[iter].getName() == "!="){
        iter++;
    } else {
        reset(start);
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
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        reset(start);
        return nullptr;
    }

    if(!check(">")){
        reset(start);
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
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        reset(start);
        return nullptr;
    }

    if(!check(">=")){
        reset(start);
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
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        reset(start);
        return nullptr;
    }

    if(!check("<")){
        reset(start);
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
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AddSub();
    if(left == nullptr){
        reset(start);
        return nullptr;
    }

    if(!check("<=")){
        reset(start);
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

    thing = AddSub();
    if(thing != nullptr){
        return thing;
    }

    return nullptr;
}



Node* Parser::OR(){
    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = AND();
    if(left == nullptr){
        return nullptr;
    }

    while(check("OR")){
        right = AND();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after the OR operator.");
            reset(start);
            return nullptr;
        }

        left = stack->alloc<Or>(left, right);

    }

    return left;

}

Node* Parser::AND(){
    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;

    left = compare();
    if(left == nullptr){
        return nullptr;
    }

    while(check("AND")){
        right = compare();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after the AND operator.");
            reset(start);
            return nullptr;
        }

        left = stack->alloc<And>(left, right);

    }

    return left;
}



//Scopes
Scope* Parser::scope(){
    //Create a new scope for the object.
    Scope* newScope = stack->alloc<Scope>(nullptr, nullptr, currScope);
    //Assign the newest scope to be the current scope.
    currScope = newScope;

    //Get the body of code and attach it to the scope.
    newScope->body = body();
    //Return to the outerscope.
    currScope = currScope->parent;

    return newScope;

}

Body* Parser::body(){
    Node* current = nullptr;
    Body* currentBody = nullptr;
    Body* bodyStart = nullptr;

    //Look out for anything familiar such as assignments or if statements.
    current = commonExpressions();
    if(current == nullptr){
        return nullptr;
    }
    bodyStart = stack->alloc<Body>(current);

    //Treat the Body* as a "linked list."
    currentBody = bodyStart;

    //So long as the program has tokens as the most recent check for
    //a expression hasn't turned up nothing, keep checking.
    while(hasTokens() & (currentBody != nullptr)){
        current = commonExpressions();
        if(current == nullptr){
            return bodyStart;
        }
        //Append code to the "linked list" of expressions.
        currentBody->next = stack->alloc<Body>(current);
        //Move down the "linked list."
        currentBody = currentBody->next;
    }

    return bodyStart;
}

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

    result = initializeAssign();
    if(result != nullptr){
        if(!check(";")){
            error = true;
            std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]\n";
        }
        return result;
    }

    result = assignment();
    if(result != nullptr){
        if(!check(";")){
            error = true;
            std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]\n";
        }
        return result;
    }

    result = initialize();
    if(result != nullptr){
        if(!check(";")){
            error = true;
            std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]\n";
        }
        return result;
    }

    result = IF();
    if(result != nullptr){
        return result;
    }

    result = OR();
    if(result != nullptr){
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
    size_t variableCount = currScope->varCount();
    size_t ifVariables = 0;
    size_t elseVariables = 0;
    IfClass* result = nullptr;

    //Check to see if the syntax matches properly.
    if(check("if") & check("(")){
        Body* ifBody = nullptr;

        condition = OR();
        //Check for the end of the condition and the start of the body.
        if((condition != nullptr) & check(")") & check("{")){
            ifBody = body();
            //Get the proper number of variables in this pseudoscope.
            ifVariables = currScope->varCount() - variableCount;
            //Remove the variables in the pseudoscope.
            for(size_t i = 0; i < ifVariables; i++){
                currScope->varPop();
            }
        } else {
            error = true;
            //Error here
            return nullptr;
        }

        if(!check("}")){
            std::cout << "Missing } expected on line: " << given[iter].row << "\n";
            error = true;
        }

        //Check for an else statement.
        Body* elseBody = nullptr; 
        if(check("else") & check("{")){
            elseBody = body();
            //the count() method will include the number of variables in the if body.
            elseVariables = currScope->varCount() - variableCount;
            for(size_t i = 0; i < elseVariables; i++){
                currScope->varPop();
            }
        } else {
            result = stack->alloc<IfClass>(condition, ifBody, elseBody);
            //Get the number of variables to be popped.
            result->ifVarCount = ifVariables;
            result->elseVarCount = elseVariables;
            return result;
        }

        if(!check("}")){
            std::cout << "Missing } expected on line: " << given[iter].row << "\n";
            error = true;
        }

        //Get the number of variables to be popped.
        result = stack->alloc<IfClass>(condition, ifBody, elseBody);
        result->ifVarCount = ifVariables;
        result->elseVarCount = elseVariables;
        return result;

    }

    return nullptr;

}

//for loop
Node* Parser::FOR(){
    //These first three can remain nullptrs.
    Node* assign = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;
    //If the body is a nullptr, then the user provided no code.
    Body* thisBody = nullptr;

    if(check("for") & check("(")){
        //Get an assignment, if any.
        if(!check(";")){
            assign = initializeAssign();
            //Edge case. A new variable could be made.
            if(assign == nullptr){
                assign = initialize();
            }
            if(!check(";")){
                error = true;
                std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]\n";
            }
        }

        //Check for a condition, if any.
        if(!check(";")){
            condition = OR();
            if(!check(";")){
                error = true;
                std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]\n";
            }
        }
        //Grab the incrementer, if any.
        if(!check(")")){
            incrementer = assignment();
            if(!check(")")){
                error = true;
                return nullptr;
            }
        }

        //Build the body of code for the loop.
        if(check("{")){
            thisBody = body();
        } else {
            error = true;
            std::cout << "Missing { expected on line: " << given[iter].row << "\n";
            return nullptr;
        }

        //Check for the last brace to end the for loop.
        if(!check("}")){
            std::cout << "Missing } expected on line: " << given[iter].row << "\n";
            error = true;
        }

        ForLoop* result = stack->alloc<ForLoop>(assign, condition, incrementer, thisBody);
        //If there was an initialization, then a variable was generated.
        if(assign != nullptr){
            currScope->varPop();
        }
        return result;

    }

    return nullptr;
    
}

//while loop
Node* Parser::WHILE(){
    if(check("while") & check("(")){
        Node* condition = nullptr;
        Body* thisBody = nullptr;
        WhileLoop* result = nullptr;

        //If this is a nullptr, then it will run perpetually.
        condition = OR();
        if(!check(")")){
            error = true;
            return nullptr;
        }

        if(!check("{")){
            error = true;
            return nullptr;
        }

        thisBody = body();
        if(thisBody == nullptr){
            error = true;
            return nullptr;
        }

        if(!check("}")){
            error = true;
            return nullptr;
        }

        result = stack->alloc<WhileLoop>(condition, thisBody);
        return result;

    }

    return nullptr;
}

//Variable stuff
Variable* Parser::variable(){
    if(!hasTokens(2)){
        return nullptr;
    }
    //Check to see if the variable is a valid token.
    if((given[iter].getType() == FloridaType::Identifier) & (given[iter + 1].getName() != "(")){
        bool isLocal = false;
        Scope* thisScope = currScope;
        //Find the variable in any of the prior connected scopes.
        while(thisScope->parent != nullptr){
            if(thisScope->varWhere(given[iter].getName()) != -1){
                isLocal = true;
                break;
            } else {
                thisScope = thisScope->parent;
            }
        }

        //The given variable does not exist if where(given[iter].getName()) returns -1.
        if(thisScope->varWhere(given[iter].getName()) == -1){
            std::cout << "Unknown variable: " << given[iter].getName() << "\n";
            return nullptr;
        }

        //Assign the location of the variable in the scope.
        Variable* newVariable = nullptr;
        if(!isLocal){
            newVariable = stack->alloc<Variable>(given[iter], thisScope->varWhere(given[iter].getName()), false);
        } else {
            //The count - 1 is because count keeps track of the stack size.
            //If I have a stack of size 1 and an element at 0, then I need to move back none.
            newVariable = stack->alloc<Variable>(given[iter], thisScope->varWhere(given[iter].getName()), true);
        }
        iter++;
        return newVariable;
    }

    //This isn't an error, just that a variable wasn't found.
    return nullptr;
}

Initialize* Parser::initialize(){
    if(!hasTokens(2)){
        return nullptr;
    }
    //Check to see if the variable is a valid token.
    bool bool1 = typeCheck(given[iter].getType());
    Token theToken = given[iter + 1];
    theToken.type = typeReturn(given[iter].getName());
    bool bool2 = given[iter + 1].getType() == FloridaType::Identifier;

    if(bool1 & bool2){
        iter++;
        iter++;
        //This will be stack allocated in scope().
        Variable* newVariable = stack->alloc<Variable>(theToken, currScope->varCount(), currScope->parent != nullptr);
        newVariable->type = theToken.type;
        //The expected stack size will be larger because of the new variable.
        Initialize* newInitialize = stack->alloc<Initialize>(newVariable);
        //Add the variable to the current scope.
        currScope->push(newVariable);

        return newInitialize;
    }

    //This isn't an error, just that a variable wasn't found.
    return nullptr;
}

InitializeAssign* Parser::initializeAssign(){
    if(!hasTokens(3)){
        return nullptr;
    }
    //Check to see if the variable is a valid token.
    bool bool1 = typeCheck(given[iter].getType());
    FloridaType type = typeReturn(given[iter].getName());
    bool bool2 = given[iter + 1].getType() == FloridaType::Identifier;
    Token theToken = given[iter + 1];
    theToken.type = type;
    bool bool3 = given[iter + 2].getName() == "=";

    if(bool1 & bool2 & bool3){
        iter++;
        iter++;
        iter++;
        Node* code = nullptr;
        InitializeAssign* result = stack->alloc<InitializeAssign>(nullptr, nullptr);
        result->type = type;

        //This will be stack allocated in the scope `currScope`.
        Variable* newVariable = stack->alloc<Variable>(theToken, currScope->varCount(), currScope->parent != nullptr);
        result->thisVariable = newVariable;
        //Add the variable to the current scope.
        currScope->push(newVariable);

        code = commonExpressions();
        result->code = code;

        return result;

    }

    return nullptr;

}

Assignment* Parser::assignment(){
    Start start = currInfo();

    Variable* thisVariable = variable();
    if((thisVariable != nullptr) and check("=")){
        Node* thisStatement = commonExpressions();
        if(thisStatement != nullptr){
            return stack->alloc<Assignment>(thisVariable, thisStatement);
        }

        return nullptr;
        
    }

    if(thisVariable != nullptr){
        reset(start);
    }
    //This isn't an error, just that an assignment wasn't found.
    return nullptr;
}



//Function stuff
Function* Parser::function(){
    if(!hasTokens(3)){
        return nullptr;
    }
    //Check if the function has a non-void return statement.
    bool returnable = given[iter].getName() != "void";
    FloridaType returnType = typeReturn(given[iter].getName());
    bool bool1 = typeCheck(given[iter].getType());
    //Grab the function's name as a string_view.
    std::string_view name = given[iter + 1].name;
    bool bool2 = given[iter + 1].getType() == FloridaType::Identifier;
    bool bool3 = given[iter + 2].getName() == "(";

    if(bool1 & bool2 & bool3){
        Function* result = stack->alloc<Function>(returnable, name, nullptr, nullptr);
        result->type = returnType;
        Scope* newScope = stack->alloc<Scope>(nullptr, nullptr, currScope);
        result->variables = newScope;
        currScope = newScope;
        iter++;
        iter++;
        iter++;

        Initialize* hasInitialization = initialize();
        if(hasInitialization != nullptr){
            //Check for more initializations.
            while(check(",")){
                hasInitialization = initialize();
            }
        }

        if(!check(")") & !check("{")){
            error = true;
            return nullptr;
        }

        //It doesn't matter if this is a nullptr or not.
        result->theFunction = scope();

        if(!check("}")){
            error = true;
            return nullptr;
        }

        currScope = newScope->parent;
        //Include the function for use in its respective scope.
        currScope->push(result);
        return result;

    }

    return nullptr;

}

Call* Parser::call(){
    if(!hasTokens(3)){
        return nullptr;
    }

    bool bool1 = given[iter].type == FloridaType::Identifier;
    std::string name = given[iter].getName();
    bool bool2 = given[iter + 1].getName() == "(";

    if(bool1 & bool2){
        if(currScope->funGet(given[iter].getName()) == nullptr){
            error = true;
            std::cout << "Unknown function";
            return nullptr;
        }
        iter++;
        iter++;
        Call* result = stack->alloc<Call>(nullptr);
        result->function = currScope->funGet(name);
        Body* arguments = stack->alloc<Body>(nullptr);
        result->arguments = arguments;
        arguments->current = commonStatements();
        //Check for all arguments.
        while(check(",")){
            //Move along the "linked list" of arguments.
            arguments->next = stack->alloc<Body>(commonStatements());
            arguments = arguments->next;
        }

        if(!check(")")){
            error = true;
            return nullptr;
        }

        return result;

    }

    return nullptr;

}

ReturnClass* Parser::Return(){
    if(!hasTokens(2)){
        return nullptr;
    }
    //Check for the string and then the statement that follows.
    if(check("return")){
        ReturnClass* result = stack->alloc<ReturnClass>(commonStatements());
        if(!check(";")){
            error = true;
            std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]\n";
        }
        return result;
    }
    //The return is a lie.
    return nullptr;

}
