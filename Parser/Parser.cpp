#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

bool typeCheck(FloridaType inType){
    switch(inType){
        case FloridaType::fixed1:
        case FloridaType::fixed2:
        case FloridaType::fixed4:
        case FloridaType::fixed8:
        case FloridaType::Bool:
            return true;
        default:
            return false;

    }
}

void Parser::parse(){
    //If it's not the nullptr, then it's successful.
    result = scope();
    std::cout << result->ToString("", "") << "\n";
};

void Parser::countInc(){
    stackCount[stackCount.size() - 1] = stackCount[stackCount.size() - 1] + 1;
}

void Parser::countDec(){
    stackCount[stackCount.size() - 1] = stackCount[stackCount.size() - 1] - 1;
}

void Parser::countSet(int64_t input){
    stackCount[stackCount.size() - 1] = input;
}

int64_t Parser::count(){
    return stackCount[stackCount.size() - 1];
}



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

void Parser::FLVMCodeGen(){
    result->FLVMCodeGen(programInstructions);
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
            countDec();
            left = stack->alloc<Add>(left, right);
            current = given[iter].name;
            continue;
        }
        if(current == "-"){
            countDec();
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
            countDec();
            left = stack->alloc<Multiply>(left, right);
            current = given[iter].name;
            continue;
        }
        if(current == "/"){
            countDec();
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

        countInc();
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

        countInc();
        return expression;
    }
    //Check for numbers.
    if(given[iter].type == FloridaType::fixed8){ 
        Fixed8* number = stack->alloc<Fixed8>(given[iter].getName());
        iter++; 

        countInc();
        return number;
    }
    //Check for booleans.
    if(given[iter].type == FloridaType::Bool){
        if(given[iter].getName() == "true"){
            iter++;
            countInc();
            return stack->alloc<Boolean>(true);
        } else {
            iter++;
            countInc();
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

    countDec();
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

    countDec();
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

    countDec();
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

    countDec();
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

    countDec();
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

    countDec();
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
    if(!hasTokens(3)){
        return nullptr;
    }

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

        countDec();
        left = stack->alloc<Or>(left, right);

    }

    return left;

}

Node* Parser::AND(){
    if(!hasTokens(3)){
        return nullptr;
    }

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

        countDec();
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

    //Start a new relative stack counter.
    stackCount.push_back(0);
    //Get the body of code and attach it to the scope.
    newScope->body = body();
    //Pop this scope from the stack counter.
    stackCount.pop_back();
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

    return nullptr;
}



//if statement
Node* Parser::IF(){
    if(!hasTokens(5)){
        return nullptr;
    }

    Node* condition = nullptr;

    //Check to see if the syntax matches properly.
    if(check("if") & check("(")){
        Scope* ifScope = nullptr;

        condition = OR();
        //Check for the end of the condition and the start of the body.
        if((condition != nullptr) & check(")") & check("{")){
            ifScope = scope();
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
        Scope* elseScope = nullptr;
        if(check("else") & check("{")){
            elseScope = scope();
        } else {
            return stack->alloc<IfClass>(condition, ifScope, elseScope);
        }

        if(!check("}")){
            std::cout << "Missing } expected on line: " << given[iter].row << "\n";
            error = true;
        }

        return stack->alloc<IfClass>(condition, ifScope, elseScope);

    }

    return nullptr;

}

//for loop
Node* Parser::FOR(){
    Node* assign = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;
    Scope* thisBody = nullptr;

    if(check("for") & check("(")){
        //Make a new scope and assign it as the current scope.
        thisBody = stack->alloc<Scope>(nullptr, nullptr, currScope);
        currScope = thisBody;
        stackCount.push_back(0);

        //Get an assignment, if any.
        if(!check(";")){
            assign = assignment();
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
                currScope = currScope->parent;
                stackCount.pop_back();
                return nullptr;
            }
        }

        //Build the body of code for the loop.
        if(check("{")){
            thisBody->body = body();
        } else {
            error = true;
            std::cout << "Missing { expected on line: " << given[iter].row << "\n";
            currScope = currScope->parent;
            stackCount.pop_back();
            return nullptr;
        }

        //Check for the last brace to end the for loop.
        if(!check("}")){
            std::cout << "Missing } expected on line: " << given[iter].row << "\n";
            error = true;
        }

        //Return to the prior scope.
        currScope = currScope->parent;
        stackCount.pop_back();
        return stack->alloc<ForLoop>(assign, condition, incrementer, thisBody);

    }

    return nullptr;
    
}


//Variable stuff
Variable* Parser::variable(){
    if(!hasTokens(1)){
        return nullptr;
    }

    //Check to see if the variable is a valid token.
    if(given[iter].getType() == FloridaType::Identifier){
        bool isLocal = false;
        Scope* thisScope = currScope;
        //Find the variable in any of the prior connected scopes.
        while(thisScope->parent != nullptr){
            if(thisScope->where(given[iter].getName()) != -1){
                isLocal = true;
                break;
            } else {
                thisScope = thisScope->parent;
            }
        }

        //The given variable does not exist if where(given[iter].getName()) returns -1.
        if(thisScope->where(given[iter].getName()) == -1){
            std::cout << "Unknown variable: " << given[iter].getName() << "\n";
            return nullptr;
        }

        //Assign the location of the variable in the scope.
        Variable* newVariable = stack->alloc<Variable>(given[iter], thisScope->where(given[iter].getName()), isLocal);
        iter++;
        countInc();
        return newVariable;
    }

    //This isn't an error, just that a variable wasn't found.
    return nullptr;
}

Variable* Parser::initialize(){
    if(!hasTokens(2)){
        return nullptr;
    }
    //Check to see if the variable is a valid token.
    bool bool1 = typeCheck(given[iter].getType());
    bool bool2 = given[iter + 1].getType() == FloridaType::Identifier;
    if(bool1 & bool2){
        //Create a new variable object in the heap.
        //This will be stack allocated in scope().
        Variable* newVariable = stack->alloc<Variable>(given[iter + 1], count(), currScope->parent != nullptr);
        //Add the variable to the current scope.
        currScope->push(newVariable);
        iter++;
        iter++;

        return newVariable;
    }

    //This isn't an error, just that a variable wasn't found.
    return nullptr;
}

Node* Parser::assignment(){
    if(!hasTokens(3)){
        return nullptr;
    }
    Start start = currInfo();

    Variable* thisInitialization = initialize();
    if(thisInitialization != nullptr){
        if(!check("=")){
            return stack->alloc<Initialize>(thisInitialization);
        }

        Node* thisStatement = commonExpressions();
        if(thisStatement != nullptr){
            return stack->alloc<Assignment>(thisInitialization, thisStatement);
        }

        return nullptr;
    }

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
    if(instructionNumber >= programInstructions.size()){
        return false;
    }

    switch (programInstructions[instructionNumber].oper){
        case Operation::cjump:
            //If it's true, then don't skip.
            if(!top().boolean){
                //This is to adjust the position of the instruction number.
                instructionNumber = programInstructions[instructionNumber].literal.fixed64;
                //Pop the boolean from the stack.
                pop();
                return true;
            }

            //Pop the boolean from the stack.
            pop();
            instructionNumber++;
            return true;
        case Operation::jump:
            instructionNumber = programInstructions[instructionNumber].literal.fixed64;
            return true;
        case Operation::gfetch:
            //Push the global value onto the stack.
            computationVector.push_back(computationVector[programInstructions[instructionNumber].literal.fixed64]);
            instructionNumber++;
            return true;
        case Operation::lfetch:
            //Push the local value onto the stack.
            computationVector.push_back(computationVector[computationVector.size() - programInstructions[instructionNumber].literal.fixed64 - 1]);
            instructionNumber++;
            return true;
        case Operation::gassign:
            computationVector[programInstructions[instructionNumber].literal.fixed64] = top();
            instructionNumber++;
            return true;
        case Operation::lassign:
            computationVector[computationVector.size() - 1 - programInstructions[instructionNumber].literal.fixed64] = top();
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
            std::cout << "Error: Unknown instruction given.\nThe instruction number is " + std::to_string(programInstructions[instructionNumber].oper);
            return false;
    }

}
