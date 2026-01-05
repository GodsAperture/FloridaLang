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

//This is not a standard sort method.
//This is for allocating objects into whole 8 byte sections of memory.
//Then the rest will be sorted by size from largest to smallest.
Initialize* Parser::InitializeSort(Initialize* input){
    if(input == nullptr){
        return nullptr;
    }

    Initialize* result = nullptr;
    Initialize* sortList = nullptr;
    Initialize* sortedList = nullptr;
    Initialize* current = input;
    Initialize* next = input->next;

    class DLL{
    public:
        DLL* previous = nullptr;
        Initialize* current = nullptr;
        DLL* next = nullptr;

        DLL(Initialize* input){
            current = input;
            if(input->next != nullptr){
                next = new DLL(input->next);
                next->previous = this;
            }
            input->next = nullptr;
        }

        //Returns the next part of the chain if there is one.
        DLL* remove(){
            DLL* result;
            if((previous == nullptr) && (next != nullptr)){
                next->previous = nullptr;
                result = next;
            }
            if((previous != nullptr) && (next == nullptr)){
                previous->next = nullptr;
                result = previous;
            }
            if((previous != nullptr) && (next != nullptr)){
                previous->next = next;
                next->previous = previous;
                result = next;
            }
            if((previous == nullptr) && (next == nullptr)){
                result = nullptr;
            }
            delete this;
            return result;
        }

        DLL* getNext(){
            if(next != nullptr){
                return next;
            } else {
                return this;
            }
        }

        DLL* rewind(){
            DLL* result = this;
            while(result->previous != nullptr){
                result = result->previous;
            }

            return result;
        }

    };

    //First loop puts all objects that are multiples of 8 to result.
    while(true){
        //Check to see if the variable uses an allocation whose size is a multiple of 8 bytes.
        //Otherwise, push it to the sort list.
        if(allocationSize(current->thisVariable->type) % 8 == 0){
            //Append the allocation to result.
            if(result != nullptr){
                result->memoryAppend(current);
            } else {
                result = current;
            }
        } else {
            if(sortList != nullptr){
                sortList->memoryAppend(current);
            } else {
                sortList = current;
            }
        }
        if(next == nullptr){
            break;
        } else {
            //Move to the next part of this linked list.
            current = next;
            next = next->next;
        }
        
    }

    //If there are any members to be sorted, then sort them manually.
    if(sortList != nullptr){
        //This loop sorts the SortList from largest to smallest.
        //This is a more hard coded solution that lets me think a little less.
        //Sort out all 4 byte primitives.
        DLL* currentDLL = new DLL(sortList);
        DLL* previous = nullptr;
        while(true){
            previous = currentDLL;
            if(allocationSize(currentDLL->current->thisVariable->type) == 4){
                if(sortedList != nullptr){
                    sortedList->memoryAppend(currentDLL->current);
                } else {
                    sortedList = currentDLL->current;
                }
                currentDLL = currentDLL->remove();
            } else {
                currentDLL = currentDLL->getNext();
            }
            if(previous == currentDLL->getNext()){
                break;
            }
        }


        if(currentDLL != nullptr){
            currentDLL = currentDLL->rewind();
            previous = nullptr;

            //Get all 2 byte allocations and append them.
            while(true){
                previous = currentDLL;
                if(allocationSize(currentDLL->current->thisVariable->type) == 2){
                    if(sortedList != nullptr){
                        sortedList->memoryAppend(currentDLL->current);
                    } else {
                        sortedList = currentDLL->current;
                    }
                    currentDLL = currentDLL->remove();
                } else {
                    currentDLL = currentDLL->getNext();
                }
                if(previous == currentDLL->getNext()){
                    break;
                }
            }
        }

        if(currentDLL != nullptr){
            currentDLL = currentDLL->rewind();
            //Get all 1 byte allocations and append them.
            //All that can remain are 1 byte allocations.
            while(currentDLL != nullptr){
                sortedList->memoryAppend(currentDLL->current);
                currentDLL = currentDLL->remove();
            }
        }

        result->memoryAppend(sortedList);
    }

    return result;

}

void Parser::parse(){
    //The entire program is essentially a scope.
    stack->AST = scope();
    //Just a "useful" debugger to make sure I'm creating the AST properly.
    if(error){
        std::cout << "The parser failed to parse the full file.\n";
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
        std::cout << "Sum or difference found!\n";
        //Check for a product/division subexpression.
        right = MulDiv();
        if(right == nullptr){
            error = true;
            errorStack.push_back("Expression expected after " + current);
            reset(start);
            return nullptr;
        }

        FloridaType resultType = returnType(left->type, right->type);
        //Determine if either the left or the right need to be typecasted.
        if(left->type != resultType){
            TypecastClass* that = stack->alloc<TypecastClass>();
            that->body = left;
            that->type = resultType;

            left = that;
        }
        if(right->type != resultType){
            TypecastClass* that = stack->alloc<TypecastClass>();
            that->body = right;
            that->type = resultType;

            right = that;
        }

        //Check for any sort of sum or difference.
        if(current == "+"){
            Add* result = stack->alloc<Add>();
            result->left = left;
            result->right = right;
            result->type = resultType;

            left = result;
            current = given[iter].name;
            continue;
        }
        if(current == "-"){
            Subtract* result = stack->alloc<Subtract>();
            result->left = left;
            result->right = right;
            result->type = resultType;

            left = result;
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

        FloridaType resultType = returnType(left->type, right->type);
        //Determine if either the left or the right need to be typecasted.
        if(left->type != resultType){
            TypecastClass* that = stack->alloc<TypecastClass>();
            that->body = left;
            that->type = resultType;

            left = that;     
        }
        if(right->type != resultType){
            TypecastClass* that = stack->alloc<TypecastClass>();
            that->body = right;
            that->type = resultType;

            right = that;
        }

        //Start checking for products or divisions.
        if(current == "*"){
            Multiply* result = stack->alloc<Multiply>();
            result->left = left;
            result->right = right;
            result->type = resultType;

            current = given[iter].name;
            left = result;
            continue;
        }
        if(current == "/"){
            Divide* result = stack->alloc<Divide>();
            result->left = left;
            result->right = right;
            result->type = resultType;

            current = given[iter].name;
            left = result;
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
    if((given[iter].type == FloridaType::ufixed8) and (given[iter].getName() != "ufixed8")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.ufixed8[0] = std::stod(given[iter].getName());
        number->type = FloridaType::ufixed8;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::ufixed4) and (given[iter].getName() != "ufixed4")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.ufixed4[0] = std::stol(given[iter].getName());
        number->type = FloridaType::ufixed4;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::ufixed2) and (given[iter].getName() != "ufixed2")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.ufixed2[0] = std::stol(given[iter].getName());
        number->type = FloridaType::ufixed2;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::ufixed1) and (given[iter].getName() != "ufixed1")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.ufixed1[0] = std::stol(given[iter].getName());
        number->type = FloridaType::ufixed1;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::fixed8) and (given[iter].getName() != "fixed8")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed8[0] = std::stod(given[iter].getName());
        number->type = FloridaType::fixed8;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::fixed4) and (given[iter].getName() != "fixed4")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed4[0] = std::stod(given[iter].getName());
        number->type = FloridaType::fixed4;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::fixed2) and (given[iter].getName() != "fixed2")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed2[0] = std::stod(given[iter].getName());
        number->type = FloridaType::fixed2;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::fixed1) and (given[iter].getName() != "fixed1")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.fixed1[0] = std::stod(given[iter].getName());
        number->type = FloridaType::fixed1;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::float8) and (given[iter].getName() != "float8")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.float8[0] = std::stod(given[iter].getName());
        number->type = FloridaType::float8;
        iter++;

        return number;
    }
    if((given[iter].type == FloridaType::float4) and (given[iter].getName() != "float4")){
        Primitive* number = stack->alloc<Primitive>();
        number->value.float4[0] = std::stod(given[iter].getName());
        number->type = FloridaType::float4;
        iter++;

        return number;
    }


    
    //Check for booleans.
    if((given[iter].type == FloridaType::Bool) and (given[iter].getName() != "boolean")){
        Primitive* result = stack->alloc<Primitive>();
        if(given[iter].getName() == "true"){
            result->value.boolean[0] = true;
            result->type = FloridaType::Bool;
            iter++;

            return result;
        } else {
            result->value.boolean[0] = false;
            result->type = FloridaType::Bool;
            iter++;
            
            return result;
        }
    }
    Call* thisCall = call();
    if(thisCall != nullptr){
        return thisCall;
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
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;
    Equal* result = nullptr;

    left = AddSub();
    if(left == nullptr){
        reset(start);
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

    FloridaType resultType = returnType(left->type, right->type);
    //Determine if either the left or the right need to be typecasted.
    if(left->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = left;
        that->type = resultType;

        left = that;     
    }
    if(right->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = right;
        that->type = resultType;

        right = that;
    }

    result = stack->alloc<Equal>();
    result->left = left;
    result->right = right;
    result->type = FloridaType::Bool;

    return result;

}

Node* Parser::notEqual(){
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;
    NotEqual* result = nullptr;

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

    FloridaType resultType = returnType(left->type, right->type);
    //Determine if either the left or the right need to be typecasted.
    if(left->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = left;
        that->type = resultType;

        left = that;     
    }
    if(right->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = right;
        that->type = resultType;

        right = that;
    }

    result = stack->alloc<NotEqual>();
    result->left = left;
    result->right = right;
    result->type = FloridaType::Bool;

    return result;

}

Node* Parser::greaterThan(){
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;
    GreaterThan* result = nullptr;

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

    FloridaType resultType = returnType(left->type, right->type);
    //Determine if either the left or the right need to be typecasted.
    if(left->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = left;
        that->type = resultType;

        left = that;     
    }
    if(right->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = right;
        that->type = resultType;

        right = that;
    }

    result = stack->alloc<GreaterThan>();
    result->left = left;
    result->right = right;
    result->type = FloridaType::Bool;

    return result;

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
    LessThan* result = nullptr;

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

    FloridaType resultType = returnType(left->type, right->type);
    //Determine if either the left or the right need to be typecasted.
    if(left->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = left;
        that->type = resultType;

        left = that;     
    }
    if(right->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = right;
        that->type = resultType;

        right = that;
    }

    result = stack->alloc<LessThan>();
    result->left = left;
    result->right = right;
    result->type = FloridaType::Bool;

    return result;

}

Node* Parser::lessThanOr(){
    if(!hasTokens(3)){
        return nullptr;
    }

    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;
    LessThanOr* result = nullptr;

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

    FloridaType resultType = returnType(left->type, right->type);
    //Determine if either the left or the right need to be typecasted.
    if(left->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = left;
        that->type = resultType;

        left = that;     
    }
    if(right->type != resultType){
        TypecastClass* that = stack->alloc<TypecastClass>();
        that->body = right;
        that->type = resultType;

        right = that;
    }

    result = stack->alloc<LessThanOr>();
    result->left = left;
    result->right = right;
    result->type = FloridaType::Bool;

    return result;

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
    Or* result = nullptr;

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

        result = stack->alloc<Or>();
        result->left = left;
        result->right = right;
        result->type = FloridaType::Bool;

        left = result;

    }

    return left;

}

Node* Parser::AND(){
    Start start = currInfo();
    Node* left = nullptr;
    Node* right = nullptr;
    And* result = nullptr;

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

        result = stack->alloc<And>();
        result->left = left;
        result->right = right;
        result->type = FloridaType::Bool;

        left = result;

    }

    return left;
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

    return newScope;
}

Body* Parser::body(){
    Node* current = nullptr;
    Body* currentBody = nullptr;
    Body* bodyStart = stack->alloc<Body>();
    
    //Look out for anything familiar such as assignments or if statements.
    current = commonExpressions();
    if(current == nullptr){
        return nullptr;
    }
    Body* tempBody = stack->alloc<Body>();
    tempBody->current = current;
    bodyStart->append(tempBody);
    //std::cout << current->ToString(">>", ";") << "\n";
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
        currentBody = stack->alloc<Body>(current);
        //Move down the "linked list."
        bodyStart->append(currentBody);

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

    result = object();
    if(result != nullptr){
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
    IfClass* result = nullptr;

    //Check to see if the syntax matches properly.
    if(check("if") & check("(")){
        uint64_t ifPosition = iter - 2;
        Scope* ifScope = nullptr;

        condition = OR();
        //Check for the end of the condition and the start of the body.
        if((condition != nullptr) & check(")") & check("{")){
            ifScope = scope();
            ifScope->name = given[ifPosition].name;
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
            uint64_t elsePosition = iter - 2;
            elseScope = scope();
            elseScope->name = given[elsePosition].name;
        } else {
            result = stack->alloc<IfClass>();
            result->condition = condition;
            result->ifBody = ifScope;
            result->elseBody = elseScope;
            return result;
        }

        if(!check("}")){
            std::cout << "Missing } expected on line: " << given[iter].row << "\n";
            error = true;
        }

        result = stack->alloc<IfClass>();
        result->condition = condition;
        result->ifBody = ifScope;
        result->elseBody = elseScope;
        return result;

    }

    return nullptr;

}

//for loop
Node* Parser::FOR(){
    //These first three can remain nullptrs.
    Initialize* initializeVar = nullptr;
    Initialize* initializeAssignVar = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;

    if(check("for") & check("(")){
        //If the body is a nullptr, then the user provided no code.
        Scope* thisScope = stack->alloc<Scope>();
        thisScope->name = given[iter - 2].name;

        //Assign the scopes its unique scope value.
        thisScope->whichScope = scopeCount;
        //Increment the scope counter.
        scopeCount++;

        //Make the current scope the new one.
        thisScope->parent = stack->currentScope;
        stack->currentScope = thisScope;

        //Get an assignment, if any.
        if(!check(";")){
            initializeAssignVar = initialize();
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
            //Add the body of code to the scope.
            thisScope->body = body();
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

        ForLoop* result = stack->alloc<ForLoop>();
        if(initializeVar != nullptr){
            result->assign = initializeVar;
        }
        if(initializeAssignVar != nullptr){
            result->assign = initializeAssignVar;
        }
        result->condition = condition;
        result->incrementer = incrementer;
        result->body = thisScope;

        //Return to the outer scope.
        stack->currentScope = stack->currentScope->parent;

        return result;

    }

    return nullptr;
    
}

//while loop
Node* Parser::WHILE(){
    if(check("while") & check("(")){
        std::string_view name = given[iter - 2].name;
        Node* condition = nullptr;
        Scope* theScope = nullptr;
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

        theScope = scope();
        if(theScope == nullptr){
            error = true;
            return nullptr;
        }

        if(!check("}")){
            error = true;
            return nullptr;
        }

        result = stack->alloc<WhileLoop>();
        result->condition = condition;
        result->body = theScope;
        theScope->name = name;

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
        //Where is the way to describe if a variable is local, middle, or global.
        char where = 3;
        Scope* thisScope = stack->currentScope;
        int64_t stackBytePosition = -1;
        //Find what scope the variable resides in.
        while(stackBytePosition == -1){
            //If we have gone through all scopes, the variable doesn't exist.
            if(thisScope == nullptr){
                std::cout << "Unknown variable '" << given[iter].getName() << "' was not found in any reachable scope.\n";
                return nullptr;
            } else {
                //If a value other than -1 is returned, then we break from the loop.
                stackBytePosition = thisScope->whereVariable(given[iter].getName());
                if(stackBytePosition == -1){
                    thisScope = thisScope->parent;
                } else {
                    break;
                }
            }
        }
        
        FloridaType theType = thisScope->getInit(given[iter].getName())->type;
        
        //Determine if lfetch, mfetch, or gfetch is needed.
        //This is for lfetch
        if(thisScope == stack->currentScope){
            where = 0;
        }
        //This is for mfetch
        if((thisScope != stack->currentScope) && (thisScope->parent != nullptr)){
            where = 1;
        }
        //This is for gfetch
        if(thisScope->parent == nullptr){
            where = 2;
        }

        //Assign the location of the variable in the scope.
        Variable* newVariable = stack->alloc<Variable>();
        newVariable->thisToken = given[iter];
        newVariable->stackBytePosition = stackBytePosition;
        newVariable->where = where;
        newVariable->owner = thisScope;
        newVariable->type = theType;

        iter++;
        return newVariable;
    }

    //This isn't an error, just that a variable wasn't found.
    return nullptr;
}

Initialize* Parser::initialize(){
    if(!hasTokens(3)){
        return nullptr;
    }
    //Check to see if the variable is a valid token.
    bool bool1 = typeCheck(given[iter].getType());
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

        if(bool3){
            iter++;
            result->code = commonStatements();
        }

        return result;
    }

    //This isn't an error, just that an initialization wasn't found.
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

ObjectClass* Parser::object(){
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

        if(!check("}")){
            //TO DO, debugging.
        }

        //Sort the initializations to be packed in memory.
        result->memoryLayout = InitializeSort(result->code->allInitializations);

        //Now I'm going to make a default object from the initializations.
        if(result->memoryLayout != nullptr){
            Initialize* currInit = result->memoryLayout;
            void* pointer = stack->current;
            //This way you know what the default object looks like.
            result->defaultConstruction = pointer;
            //Determine the size of the region in memory.
            while(currInit != nullptr){
                //Check to see if there's a simple initialization.
                //If so, then make it part of the default object.
                uint64_t memSize = allocationSize(currInit->thisVariable->type);
                //TO DO: Adjust for when an object is passed here instead of a primitive.
                if(currInit->code != nullptr){
                    //TO DO: support default initializations.
                }
                result->memorySize += memSize;
                currInit = currInit->memoryOrder;
                pointer = (void*) (((char*) pointer) + memSize);
                result->variableCount++;
            }
            uint64_t resultingMemorySize = result->memorySize + (8 - (result->memorySize % 8)) % 8;
            result->memorySize = resultingMemorySize;
            stack->current = (void*) (((char*) stack->current) + resultingMemorySize);
        }

        return result;
        
    }

    return nullptr;

}



//Function stuff
Node* Parser::function(){
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
        Function* result = stack->alloc<Function>();
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
        int64_t variableCount = 0;
        Scope* newScope = stack->alloc<Scope>();
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

        Initialize* hasInitialization = initialize();
        if(hasInitialization != nullptr){
            variableCount++;
            //Check for more initializations.
            while(check(",")){
                hasInitialization = initialize();
                variableCount++;
            }
        }

        if(!check(")") & !check("{")){
            error = true;
            return nullptr;
        }

        //It doesn't matter if this is a nullptr or not.
        result->code->body = body();
        //How many arguments are expected in the function.
        result->argumentCount = variableCount;

        if(!check("}")){
            error = true;
            return nullptr;
        }

        //Return the relevant function scope to the previous function.
        stack->currentFunction = stack->currentFunction->previous;
        //Return the scope to the previous scope.
        stack->currentScope = newScope->parent;

        //std::cout << "Function " << name << " of number: " << result->code->whichScope << "\n";

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
        //Find out which function it is.
        Scope* tempScope = stack->currentScope;
        Scope* oldScope = tempScope;
        while(tempScope->funGet(given[iter].getName()) == nullptr){
            tempScope = tempScope->parent;
            if(tempScope == nullptr){
                std::cout << "Function '" << name << "' was not found in any reachable scope.\n";
                return nullptr;
            }
        }
        iter++;
        iter++;
        //Create the call object, and adjust the scope to a new scope.
        //If I don't, variables/functions might not be grabbed from the proper scope.
        Call* result = stack->alloc<Call>();
        result->function = tempScope->funGet(name);
        //Adjust the scope here.
        stack->currentScope = result->function->code;
        Arguments* arguments = stack->alloc<Arguments>();
        result->arguments = arguments;
        arguments->current = commonStatements();
        
        //Check for all arguments.
        while(check(",")){
            //Move along the "linked list" of arguments.
            arguments->next = stack->alloc<Arguments>(commonStatements());
            arguments = arguments->next;
        }

        if(!check(")")){
            error = true;
            return nullptr;
        }

        //Readjust the scope to the previous scope.
        stack->currentScope = oldScope;

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
            error = true;
            std::cout << "Missing ';' expected at [" + std::to_string(given[iter].row) + ", " + std::to_string(given[iter].column) +"]\n";
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
    //The return is a lie.
    return nullptr;

}
