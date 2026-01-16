/*If you're wanting to find a particular class, just Ctrl + F and then type // followed by the class name.
an example would be searching for //TypecastClass
*/

#include "Node.hpp"
#include <math.h>
#include <stack>
#include <iostream>

inline std::string padding(std::string input){
    for(size_t i = input.size(); i < 12; i++){
        input += " ";
    }

    return input;
}

inline std::string padding2(std::string input){
    for(size_t i = input.size(); i < 16; i++){
        input += " ";
    }

    return input;
}

std::string typeString(FloridaType input){
    switch(input){
    //Boolean
        case FloridaType::Bool:
            return "boolean";
    //Fixed point numbers
        case FloridaType::fixed1:
            return "fixed1";
        case FloridaType::fixed2:
            return "fixed2";
        case FloridaType::fixed4:
            return "fixed4";
        case FloridaType::fixed8:
            return "fixed8";
        case FloridaType::fixedn:
            return "fixedn";
    //Floating point numbers
        case FloridaType::float4:
            return "float4";
        case FloridaType::float8:
            return "float8";
        case FloridaType::floatn:
            return "floatn";
        default:
            return "NULLERROR";
    }

    //This is unreachable, but appeases the compiler.
    return "NULLERROR";
}

FloridaType typeReturn(std::string inString){
    //Void
    if(inString == "void"){
        return FloridaType::tvoid;
    }
    //Boolean
    if(inString == "boolean"){
        return FloridaType::Bool;
    }
    //Ufixed pointer number
    if(inString == "ufixed1"){
        return FloridaType::ufixed1;
    }
    if(inString == "ufixed2"){
        return FloridaType::ufixed2;
    }
    if(inString == "ufixed4"){
        return FloridaType::ufixed4;
    }
    if(inString == "ufixed8"){
        return FloridaType::ufixed8;
    }
    //Fixed point numbers.
    if(inString == "fixed1"){
        return FloridaType::fixed1;
    }
    if(inString == "fixed2"){
        return FloridaType::fixed2; 
    }
    if(inString == "fixed4"){
        return FloridaType::fixed4;
    }
    if(inString == "fixed8"){
        return FloridaType::fixed8;
    }
    if(inString == "fixedn"){
        return FloridaType::fixedn;
    }
    //Floating point numbers.
    if(inString == "float4"){
        return FloridaType::float4;
    }
    if(inString == "float8"){
        return FloridaType::float8;
    }
    if(inString == "floatn"){
        return FloridaType::floatn;
    }
    return FloridaType::Null;
}

bool typeCheck(FloridaType inType){
    switch(inType){
        case FloridaType::ufixed1:
        case FloridaType::ufixed2:
        case FloridaType::ufixed4:
        case FloridaType::ufixed8:
        case FloridaType::fixed1:
        case FloridaType::fixed2:
        case FloridaType::fixed4:
        case FloridaType::fixed8:
        case FloridaType::float4:
        case FloridaType::float8:
        case FloridaType::Bool:
            return true;
        default:
            return false;

    }
}

int64_t allocationSize(FloridaType input){
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



//TypecastClass
    TypecastClass::TypecastClass(){
        //This is just a placeholder. It will be changed later.
        type = FloridaType::Typecast;
    }

    std::string TypecastClass::ToString(std::string inLeft, std::string inRight){
        return body->ToString(inLeft, inRight);
    }

    std::string TypecastClass::printAll(){
        std::string result = "";

        switch(type){
            case FloridaType::ufixed1:
                result = "ufixed1cast\n";
                break;
            case FloridaType::ufixed2:
                result = "ufixed2cast\n";
                break;
            case FloridaType::ufixed4:
                result = "ufixed4cast\n";
                break;
            case FloridaType::ufixed8:
                result = "ufixed8cast\n";
                break;
            case FloridaType::fixed1:
                result = "fixed1cast\n";
                break;
            case FloridaType::fixed2:
                result = "fixed2cast\n";
                break;
            case FloridaType::fixed4:
                result = "fixed4cast\n";
                break;
            case FloridaType::fixed8:
                result = "fixed8cast\n";
                break;
            case FloridaType::float4:
                result = "float4cast\n";
                break;
            case FloridaType::float8:
                result = "float8cast\n";
                break;
            default:
                break;
                //Do nothing, unreachable.
        }

        return body->printAll() + result;
    }

    void TypecastClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        body->FLVMCodeGen(inInstructions);
        //Once again, I abuse the integer nature to get the correct operation.
        //This will pick the correct one among 100 total options.
        //10 of those are redundant and only necessary to let the math work.
        result.oper = (Operation) ((body->type - FloridaType::ufixed1) * 10 + (type - FloridaType::ufixed1) + ufixed1TOufixed1);

        inInstructions.push_back(result);
    }

    Node* TypecastClass::copy(StackAllocator& input){
        TypecastClass* result = input.alloc<TypecastClass>();
        result->body = body->copy(input);

        return result;
    }

    TypecastClass* TypecastClass::pcopy(StackAllocator& input){
        TypecastClass* result = input.alloc<TypecastClass>();
        result->body = body->copy(input);

        return result;
    }


//Scope
    Scope::Scope(){
        //Do nothing lol
    }

    //Determine where a particular variable exists in a given scope.
    //If it is not found, then return -1.
    int64_t Scope::whereVariable(std::string input){
        Initialize* currInit = allInitializations;
        int64_t count = 0;
        int64_t result = -1;
        //If it exists, then we'll find the position.
        while(currInit != nullptr){
            if(currInit->thisVariable->thisToken.getName() == input){
                result = count;
                currInit = currInit->next;
            } else {
                count++;
                currInit = currInit->next;
            }
        }
        //If there is no such member, return -1.
        return result;
    }

    //Given a variable name, find its initialization.
    Initialize* Scope::getInit(std::string input){
        Initialize* currInit = allInitializations;
        //If it exists, then we'll find the position.
        while(currInit != nullptr){
            if(currInit->thisVariable->thisToken.getName() == input){
                return currInit;
            } else {
                currInit = currInit->next;
            }
        }
        //If there is no such member, return -1.
        return nullptr;
    }

    //Determine if a particular function exists in a given scope.
    //If it is found, then return its pointer.
    //Otherwise, return the nullptr.
    Function* Scope::funGet(std::string input){
        Function* currFun = functions;
        //If it exists, then we'll find the function.
        while(currFun != nullptr){
            if(currFun->name == input){
                return currFun;
            }
            currFun = currFun->next;
        }
        //If there is no such member, return the nullptr.
        return nullptr;
    }

    //Determine if a particular object exists in a given scope.
    //If it is found, then return its pointer.
    //Otherwise, return the nullptr.
    ObjectClass* Scope::objGet(std::string input){
        ObjectClass* currObj = allObjects;
        if(currObj != nullptr){
            while(currObj != nullptr){
                if(std::string(currObj->name) == input){
                    return currObj;
                } else {
                    currObj = currObj->next;
                }
            }
        }
        return nullptr;
    }

    //Add the Initialization to the two linked lists that sort
    //them by order found and size in bytes.
    void Scope::push(Initialize* input){
        int64_t byteSize = allocationSize(input->thisVariable->type);
        int64_t theirSize = 0;
        //If there are no initializations, then just slap the variable onto the list.
        if(allInitializations == nullptr){
            allInitializations = input;
            sortedInitializations = input;
            return;
        }
        Initialize* currentInitialize = allInitializations;
        Initialize* previousInitialize = nullptr;
        //Reach the tail end of the "linked list" of initializations.
        while(currentInitialize->next != nullptr){
            currentInitialize = currentInitialize->next;
        }

        //Append the initialization to the tail end of the "linked list."
        currentInitialize->next = input;

        //If the input is some object, prepend it.
        //These are always whole multiples of 8, so
        //order doesn't particularly matter for these.
        if(input->thisVariable->objectType != nullptr){
            input->memoryOrder = sortedInitializations;
            sortedInitializations = input;
        }

        currentInitialize = sortedInitializations;
        //This will place input in a sorted place based on size in the linked list.
        while(currentInitialize != nullptr){
            theirSize = allocationSize(currentInitialize->thisVariable->type);
            if(byteSize < theirSize){
                previousInitialize = currentInitialize;
                currentInitialize = currentInitialize->memoryOrder;
                continue;
            } else {
                break;
            }
        }

        if(previousInitialize == nullptr){
            input->memoryOrder = sortedInitializations;
            sortedInitializations = input;
        } else {
            previousInitialize->memoryOrder = input;
            input->memoryOrder = currentInitialize;
        }
    }

    void Scope::push(Function* input){
        //If there are no functions, then just slap the function onto the list.
        if(functions == nullptr){
            functions = input;
            return;
        }
        Function* currFun = functions;
        //Reach the tail end of the "linked list" of functions.
        while(currFun->next != nullptr){
            currFun = currFun->next;
        }

        //Append the function to the tail end of the "linked list."
        currFun->next = input;

    }

    void Scope::push(ObjectClass* input){
        //If there are no objects, then this is the first defined class in the scope.
        if(allObjects == nullptr){
            allObjects = input;
            return;
        }
        ObjectClass* currObj = allObjects;
        //Find the last element of the linked list.
        while(currObj->next != nullptr){
            currObj = currObj->next;
        }

        currObj->next = input;
    }

    size_t Scope::varCount(){
        size_t count = 0;
        Initialize* currInit = allInitializations;
        while(currInit != nullptr){
            count++;
            currInit = currInit->next;
        }

        return count;
    }

    size_t Scope::funCount(){
        size_t count = 0;
        Function* currFun = functions;
        while(currFun != nullptr){
            count++;
            currFun = currFun->next;
        }

        return count;
    }

    void Scope::byteAssign(){
        if(allInitializations == nullptr){
            return;
        }
        Initialize* currentInitialization = sortedInitializations;
        int64_t byteSize;
        const int64_t bitmask = 7;
        while(currentInitialization != nullptr){
            //Determine the size of the variable's allocation.
            if(currentInitialization->thisVariable->objectType == nullptr){
                byteSize = allocationSize(currentInitialization->thisVariable->type);
            } else {
                byteSize = currentInitialization->thisVariable->objectType->memorySize;
            }
            //Assign the current size to the variable's position in memory.
            currentInitialization->thisVariable->stackBytePosition = variableSlotSize;
            //Increase the size of the stack for the next variable's placement.
            variableSlotSize += byteSize;
            currentInitialization = currentInitialization->memoryOrder;
        }
        //In case the memory used isn't an exact multiple of 8, pad it.
        variableSlotSize += bitmask & (8 - (bitmask & variableSlotSize));
    }

    std::string Scope::ToString(std::string inLeft, std::string inRight){
        if(body != nullptr){
            return body->ToString(inLeft, ";");
        } 
        return "";
    }

    std::string Scope::printAll(){

        return "newScope\n" + body->printAll() + "deleteScope\n";
    }

    void Scope::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.oper = Operation::push;
        result.literal.fixed8 = whichScope;
        //Be sure to add this otherwise I can never adjust the unique scopes.
        inInstructions.push_back(result);
        //The new scope will use the last value to pick the proper scope and adjust its value.
        inInstructions.push_back(Instruction(newScope));
        //This is so I can preemptively allocate slots for the scope's variables.
        Instruction initializeCount = Instruction();
        initializeCount.literal.fixed8 = variableSlotSize;
        inInstructions.push_back(initializeCount);
        if(body != nullptr){
            body->FLVMCodeGen(inInstructions);
        }
        //Add it here to be able to adjust the unique Scope here too.
        inInstructions.push_back(result);
        inInstructions.push_back(Instruction(deleteScope));
    }

    Node* Scope::copy(StackAllocator& input){
        Scope* thisptr = input.alloc<Scope>();
        //If this is the first scope, then it's global and current.
        if(input.currentScope == nullptr){
            input.globalScope = thisptr;
            input.currentScope = thisptr;
        } else {
        //Otherwise, the currentScope is now the parent
        //and change thisptr to be the currentScope.
            thisptr->parent = input.currentScope;
            input.currentScope = thisptr;
        }
        //Don't directly copy the variables, functions, or classes.
        //Add them as you find them to avoid future issues.
        if(body != nullptr){
            thisptr->body = (Body*) body->copy(input);
        }

        return thisptr;
    }

    Scope* Scope::pcopy(StackAllocator& input){
        Scope* thisptr = input.alloc<Scope>();
        //If this is the first scope, then it's global and current.
        if(input.currentScope == nullptr){
            input.globalScope = thisptr;
            input.currentScope = thisptr;
        } else {
        //Otherwise, the currentScope is now the parent
        //and change thisptr to be the currentScope.
            thisptr->parent = input.currentScope;
            input.currentScope = thisptr;
        }
        //Don't directly copy the variables, functions, or classes.
        //Add them as you find them to avoid future issues.
        if(body != nullptr){
            thisptr->body = (Body*) body->copy(input);
        }

        return thisptr;
    }

    bool Scope::hasVariable(std::string_view input){
        //Go through all scopes as needed.
        Scope* currentScope = this;
        Initialize* currentInitialize = nullptr;
        while(currentScope != nullptr){
            currentInitialize = currentScope->allInitializations;
            //Check all of the initializations in the scope.
            while(currentInitialize != nullptr){
                if(currentInitialize->thisVariable->thisToken.name == input){
                    return true;
                } else {
                    currentInitialize = currentInitialize->next;
                }
            }
            currentScope = currentScope->parent;
        }

        return false;
    }

    bool Scope::hasObject(std::string_view input){
        //Go through all scopes as needed.
        Scope* currentScope = this;
        ObjectClass* currentObject = nullptr;
        while(currentScope != nullptr){
            currentObject = currentScope->allObjects;
            //Check all of the objects in the scope.
            while(currentObject != nullptr){
                if(currentObject->name == input){
                    return true;
                } else {
                    currentObject = currentObject->next;
                }
            }
            currentScope = currentScope->parent;
        }

        return false;
    }

    bool Scope::hasFunction(std::string_view input){
        //Go through all scopes as needed.
        Scope* currentScope = this;
        Function* currentFunction = nullptr;
        while(currentScope != nullptr){
            currentFunction = currentScope->functions;
            //Check all of the initializations in the scope.
            while(currentFunction != nullptr){
                if(currentFunction->name == input){
                    return true;
                } else {
                    currentFunction = currentFunction->next;
                }
            }
            currentScope = currentScope->parent;
        }

        return false;
    }

    Variable* Scope::getVariable(std::string_view input){
        //Go through all scopes as needed.
        Scope* currentScope = this;
        Initialize* currentInitialize = nullptr;
        while(currentScope != nullptr){
            currentInitialize = currentScope->allInitializations;
            //Check all of the initializations in the scope.
            while(currentInitialize != nullptr){
                if(currentInitialize->thisVariable->thisToken.name == input){
                    return currentInitialize->thisVariable;
                } else {
                    currentInitialize = currentInitialize->next;
                }
            }
            currentScope = currentScope->parent;
        }

        return nullptr;
    }

    ObjectClass* Scope::getObject(std::string_view input){
        //Go through all scopes as needed.
        Scope* currentScope = this;
        ObjectClass* currentObject = nullptr;
        while(currentScope != nullptr){
            currentObject = currentScope->allObjects;
            //Check all of the objects in the scope.
            while(currentObject != nullptr){
                if(currentObject->name == input){
                    return currentObject;
                } else {
                    currentObject = currentObject->next;
                }
            }
            currentScope = currentScope->parent;
        }

        return nullptr;
    }

    Function* Scope::getFunction(std::string_view input){
        //Go through all scopes as needed.
        Scope* currentScope = this;
        Function* currentFunction = nullptr;
        while(currentScope != nullptr){
            currentFunction = currentScope->functions;
            //Check all of the initializations in the scope.
            while(currentFunction != nullptr){
                if(currentFunction->name == input){
                    return currentFunction;
                } else {
                    currentFunction = currentFunction->next;
                }
            }
            currentScope = currentScope->parent;
        }

        return nullptr;
    }

    char Scope::whereVariable(std::string_view input){
        Scope* currentScope = this;
        Variable* currentVariable = nullptr;
        bool found = false;
        while(currentScope != nullptr){
            currentVariable = currentScope->allInitializations->thisVariable;
            while(currentVariable != nullptr){
                if(currentVariable->thisToken.name == input){
                    found = true;
                    break;
                } else {
                    currentVariable = currentVariable->next;
                }
            }
            if(found){
                break;
            }
            currentScope = currentScope->parent;
        }

        if(currentScope == nullptr){
            std::cout << "Error: Where could not be determined.\n";
            return -1;
        }

        //Local scope
        if(currentScope->parent == nullptr){
            return 0;
        }

        //Middle scope
        if((currentScope->parent != nullptr) and (currentScope != this)){
            return 1;
        }

        //Global scope
        if(currentScope == this){
            return 2;
        }

        //Not sure how to determine heap yet.

        return 4;
    }


//Primitive
    Primitive::Primitive(){
        value.object = nullptr;
        //The type will be determined after a Primitive is created.
    }

    std::string Primitive::ToString(std::string inLeft, std::string inRight){
        std::string result = "";
        switch(type){
            case ufixed1:
                result = std::to_string(value.ufixed1[0]);
                break;
            case ufixed2:
                result = std::to_string(value.ufixed2[0]);
                break;
            case ufixed4:
                result = std::to_string(value.ufixed4[0]);
                break;
            case ufixed8:
                result = std::to_string(value.ufixed8);
                break;
            case fixed1:
                result = std::to_string(value.fixed1[0]);
                break;
            case fixed2:
                result = std::to_string(value.fixed2[0]);
                break;
            case fixed4:
                result = std::to_string(value.fixed4[0]);
                break;
            case fixed8:
                result = std::to_string(value.fixed8);
                break;
            case float4:
                result = std::to_string(value.float4[0]);
                break;
            case float8:
                result = std::to_string(value.float8);
                break;
            case Bool:
                if(value.boolean[0]){
                    result = "\x1b[32mtrue\x1b[0m";
                } else {
                    result = "\x1b[31mfalse\x1b[0m";
                }
                break;
            default:
                break;
            //Error
        }

        return result;
    }

    std::string Primitive::printAll(){
        std::string result = "";
        switch(type){
            case ufixed1:
                result = std::to_string(value.ufixed1[0]);
                break;
            case ufixed2:
                result = std::to_string(value.ufixed2[0]);
                break;
            case ufixed4:
                result = std::to_string(value.ufixed4[0]);
                break;
            case ufixed8:
                result = std::to_string(value.ufixed8);
                break;
            case fixed1:
                result = std::to_string(value.fixed1[0]);
                break;
            case fixed2:
                result = std::to_string(value.fixed2[0]);
                break;
            case fixed4:
                result = std::to_string(value.fixed4[0]);
                break;
            case fixed8:
                result = std::to_string(value.fixed8);
                break;
            case float4:
                result = std::to_string(value.float4[0]);
                break;
            case float8:
                result = std::to_string(value.float8);
                break;
            case Bool:
                if(value.boolean[0]){
                    result = "\x1b[32mtrue\x1b[0m";
                } else {
                    result = "\x1b[31mfalse\x1b[0m";
                }
                break;
            default:
                break;
            //Error
        }

        return padding("push") + result + "\n";

    }

    void Primitive::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal = value;
        result.type = type;
        result.oper = Operation::push;

        inInstructions.push_back(result);
    }

    Node* Primitive::copy(StackAllocator& input){
        Primitive* result = input.alloc<Primitive>();
        result->type = type;
        result->value = value;

        return result;
    }

    Primitive* Primitive::pcopy(StackAllocator& input){
        Primitive* result = input.alloc<Primitive>();
        result->type = type;
        result->value = value;

        return result;        
    }



//Body
    Body::Body(){
        current = nullptr;
        next = nullptr;
    }

    Body::Body(Node* input){
        current = input;
        next = nullptr;
    }

    //Append the body to the end of the linked list.
    Body* Body::append(Body* input){
        if(current == nullptr){
            current = input;
            return this;
        }

        Body* currBody = this;
        while(currBody->next != nullptr){
            currBody = currBody->next;
        }

        currBody->next = input;
        return this;

    }

    std::string Body::ToString(std::string inLeft, std::string inRight){
        if(next != nullptr){
            return current->ToString(inLeft, inRight) + "\n" + next->ToString(inLeft, inRight);
        }

        return current->ToString(inLeft, inRight);
    }

    std::string Body::printAll(){
        if(next != nullptr){
            if(dynamic_cast<Function*>(current) == nullptr){
                return current->printAll() + next->printAll();
            } else {
                return next->printAll();
            }
        }
        if(dynamic_cast<Function*>(current) == nullptr){
            return current->printAll();
        }

        return "";

    }

    void Body::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Add the current chunk of code if it is not a function.
        if(dynamic_cast<Function*>(current) == nullptr){
            current->FLVMCodeGen(inInstructions);
        }
        //If next isn't a nullptr, then generate code for it too.
        if(next != nullptr){
            next->FLVMCodeGen(inInstructions);
        }
    }

    Node* Body::copy(StackAllocator& input){
        Body* thisptr = input.alloc<Body>();
        if(next == nullptr){
            thisptr->current = current->copy(input);
        } else {
            thisptr->current = current->copy(input);
            thisptr->next = (Body*) next->copy(input);
        }

        return thisptr;
    }

    Body* Body::pcopy(StackAllocator& input){
                Body* thisptr = input.alloc<Body>();
        if(next == nullptr){
            thisptr->current = current->copy(input);
        } else {
            thisptr->current = current->copy(input);
            thisptr->next = (Body*) next->copy(input);
        }

        return thisptr;
    }


//Variable
    Variable::Variable(){
        //Do nothing. It's not a problem.
    }

    void Variable::append(Variable* input){
        Variable* currVar = this;
        if(currVar->next != nullptr){
            while(currVar->next != nullptr){
                currVar = currVar->next;
            }
        }

        currVar->next = input;

    }

    std::string Variable::ToString(std::string inLeft, std::string inRight){
        return thisToken.getName();
    }

    std::string Variable::printAll(){
        std::string stackOffset = std::to_string(stackBytePosition / 8);
        std::string byteOffset = std::to_string(stackBytePosition % 8);
        std::string byteSize = std::to_string(allocationSize(type));
        std::string contextComment = "(*Variable " + thisToken.getName() + " || stackOffset: " + stackOffset + " byteOffset: " + byteOffset + " *)\n";
        if(where == 0){
            return padding2(padding("lfetch" + byteSize) + stackOffset) + contextComment;
        }
        if(where == 1){
            return padding2(padding("push") + std::to_string(owner->whichScope)) + "(*scope: " + std::string(owner->name) + "*)\n" +
            padding2(padding("mfetch" + byteSize) + stackOffset) + contextComment;
        }
        if(where == 2){
            return padding2(padding("push" + byteOffset)) + "\n" +
            padding2(padding("gfetch" + byteSize) + stackOffset) + contextComment;
        }
        if(where == 3){
            return padding2(padding("push") + "HEAP_ADDRESS") + "(*Determined at run time*)" + "\n" +
            padding2(padding("hfetch" + byteSize) + std::to_string(stackBytePosition)) + contextComment;
        }
        return "";
    }

    void Variable::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction instruction;
        if(where == 0){
            switch(allocationSize(type)){
                case 1:
                    instruction.oper = gfetch1;
                    break;
                case 2:
                    instruction.oper = gfetch2;
                    break;
                case 4:
                    instruction.oper = gfetch4;
                    break;
                case 8:
                    instruction.oper = gfetch8;
            }
            instruction.literal.fixed8 = stackBytePosition;
            //Push back the instruction for local variable.
            inInstructions.push_back(instruction);
            return;
        }
        if(where == 1){
            instruction.oper = Operation::push;
            instruction.literal.fixed8 = owner->whichScope;
            //Push the push instruction onto the stack.
            inInstructions.push_back(instruction);
            switch(allocationSize(type)){
                case 1:
                    instruction.oper = mfetch1;
                    break;
                case 2:
                    instruction.oper = mfetch2;
                    break;
                case 4:
                    instruction.oper = mfetch4;
                    break;
                case 8:
                    instruction.oper = mfetch8;
            }            
            instruction.literal.fixed8 = stackBytePosition;
            //Push the mfetch instruction onto the stack.
            inInstructions.push_back(instruction);
            return;
        }
        if(where == 2){
            switch(allocationSize(type)){
                case 1:
                    instruction.oper = lfetch1;
                    break;
                case 2:
                    instruction.oper = lfetch2;
                    break;
                case 4:
                    instruction.oper = lfetch4;
                    break;
                case 8:
                    instruction.oper = lfetch8;
            }
            instruction.literal.fixed8 = stackBytePosition;
            //Push back the instruction for the global variable.
            inInstructions.push_back(instruction);
            return;
        }
        if(where == 3){
            switch(allocationSize(type)){
                case 1:
                    instruction.oper = hfetch1;
                    break;
                case 2:
                    instruction.oper = hfetch2;
                    break;
                case 4:
                    instruction.oper = hfetch4;
                    break;
                case 8:
                    instruction.oper = hfetch8;
            }
            instruction.literal.fixed8 = stackBytePosition;
            //Push back the instruction for the global variable.
            inInstructions.push_back(instruction);
            return;
        }
    }

    Node* Variable::copy(StackAllocator& input){
        Variable* thisptr = input.alloc<Variable>();

        thisptr->thisToken = thisToken;
        thisptr->where = where;
        thisptr->stackBytePosition = stackBytePosition;
        thisptr->value = value;
        thisptr->type = type;

        return thisptr;
    }

    Variable* Variable::pcopy(StackAllocator& input){
        Variable* thisptr = input.alloc<Variable>();

        thisptr->thisToken = thisToken;
        thisptr->where = where;
        thisptr->stackBytePosition = stackBytePosition;
        thisptr->value = value;
        thisptr->type = type;

        return thisptr;
    }



//Initialize
    Initialize::Initialize(){
        //Do nothing.
    };

    std::string Initialize::ToString(std::string inLeft, std::string inRight){
        if(thisVariable->objectType != nullptr){
            if(code != nullptr){
                return inLeft + std::string(thisVariable->objectType->name) + " " + thisVariable->thisToken.getName() + " = " + code->ToString(inLeft, inRight) + inRight;
            }
            return inLeft + std::string(thisVariable->objectType->name) + " " + thisVariable->thisToken.getName() + inRight;
        }
        if(code != nullptr){
            return inLeft + typeString(thisVariable->thisToken.type) + " " + thisVariable->thisToken.getName() + " = " +  code->ToString(inLeft, inRight) + inRight;
        }
        return inLeft + typeString(thisVariable->thisToken.type) + " " + thisVariable->thisToken.getName() + inRight;
    }

    std::string Initialize::printAll(){
        return padding2("initialize") + "(*Variable " + thisVariable->thisToken.getName() + "*)\n";
    }

    void Initialize::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //If there's code, then generate it.
        if(code != nullptr){
            //Generate the code for the right hand side.
            code->FLVMCodeGen(inInstructions);

            //After executing those instructions, we want to assign the result.
            Instruction push = Instruction();
            push.oper = Operation::push;
            //Get the byte index and push it.
            push.literal.fixed8 = thisVariable->stackBytePosition % 8;
            inInstructions.push_back(push);

            //Get the correct byte size.
            Instruction assign = Instruction();
            switch(allocationSize(type)){
                case 1:
                    assign.oper = Operation::lassign1;
                    break;
                case 2:
                    assign.oper = Operation::lassign2;
                    break;
                case 4:
                    assign.oper = Operation::lassign4;
                    break;
                case 8:
                    assign.oper = Operation::lassign8;
                    break;
            }
            //Get the stack index.
            assign.literal.fixed8 = thisVariable->stackBytePosition / 8;
            //Push an assignment instruction.
            inInstructions.push_back(assign);
        }
    }

    Node* Initialize::copy(StackAllocator& input){
        Initialize* thisptr = input.alloc<Initialize>();

        thisptr->thisVariable = thisVariable->pcopy(input);
        input.currentScope->push(thisptr);
        thisptr->code = code->copy(input);

        return thisptr;
    }

    Initialize* Initialize::pcopy(StackAllocator& input){
        Initialize* thisptr = input.alloc<Initialize>();

        thisptr->thisVariable = thisVariable->pcopy(input);
        input.currentScope->push(thisptr);
        thisptr->code = code->copy(input);

        return thisptr;
    }

    //Append the `input` to the end of the linked list of Initializations.
    void Initialize::append(Initialize* input){
        Initialize* current = this;
        while(current->next != nullptr){
            current = current->next;
        }
        current->next = input;
    }

    //Append `input` to the end of the `memoryOrder` linked list.
    void Initialize::memoryAppend(Initialize* input){
        Initialize* current = this;
        while(current->memoryOrder != nullptr){
            current = current->memoryOrder;
        }
        current->memoryOrder = input;
    }



//Assignment
    Assignment::Assignment(){
        //Do nothing
    };

    std::string Assignment::ToString(std::string inLeft, std::string inRight){
        return inLeft + left->ToString(inLeft, inRight) + " = " + right->ToString(inLeft, inRight) + inRight;
    }

    std::string Assignment::printAll(){
        std::string returnType = "";
        Variable* theVariable = nullptr;
        MemberAccess* theMemberAccess = nullptr;
        Dereference* theDereference = nullptr;

        theVariable = dynamic_cast<Variable*>(left);
        theMemberAccess = dynamic_cast<MemberAccess*>(left);
        theDereference = dynamic_cast<Dereference*>(left);

        if(theMemberAccess != nullptr){
            theVariable = theMemberAccess->right;
        }

        if(theDereference != nullptr){
            theVariable = theDereference->right;
        }
        
        if(theVariable != nullptr){
            switch(theVariable->where){
                case 0:
                    returnType = "lassign";
                    break;
                case 1:
                    returnType = "massign";
                    break;
                case 2: 
                    returnType = "gassign";
                    break;
                case 3:
                    returnType = "hassign";
                    break;
            }
        }

        return right->printAll() + 
        left->printAll() + "\n" +
        "lassign\n";

    }

    void Assignment::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction assign;
        Variable* someVariable = dynamic_cast<Variable*>(left);
        if(someVariable != nullptr){
            //Generate code for the assignment.
            right->FLVMCodeGen(inInstructions);
            //Push back the instruction for assignment.
            if(someVariable->where == 0){
                switch(allocationSize(left->type)){
                    case 1:
                        assign.oper = Operation::lassign1;
                        break;
                    case 2:
                        assign.oper = Operation::lassign2;
                        break;
                    case 4:
                        assign.oper = Operation::lassign4;
                        break;
                    case 8:
                        assign.oper = Operation::lassign8;
                        break;
                }
                assign.literal.fixed8 = someVariable->stackBytePosition;
                inInstructions.push_back(assign);
                return;
            }
            if(someVariable->where == 1){
                //Add the location of the scope in the instruction set.
                switch(allocationSize(someVariable->type)){
                    case 1:
                        assign.oper = Operation::massign1;
                        break;
                    case 2:
                        assign.oper = Operation::massign2;
                        break;
                    case 4:
                        assign.oper = Operation::massign4;
                        break;
                    case 8:
                        assign.oper = Operation::massign8;
                        break;
                }
                assign.literal.fixed8 = someVariable->stackBytePosition;
                //Push back the assignment instruction.
                inInstructions.push_back(assign);
                return;
            }
            if(someVariable->where == 2){
                switch(allocationSize(someVariable->type)){
                    case 1:
                        assign.oper = Operation::gassign1;
                        break;
                    case 2:
                        assign.oper = Operation::gassign2;
                        break;
                    case 4:
                        assign.oper = Operation::gassign4;
                        break;
                    case 8:
                        assign.oper = Operation::gassign8;
                        break;
                }
                assign.literal.fixed8 = someVariable->stackBytePosition;
                inInstructions.push_back(assign);
                return;
            }
            if(someVariable->where == 3){
                switch(allocationSize(someVariable->type)){
                    case 1:
                        assign.oper = Operation::hassign1;
                        break;
                    case 2:
                        assign.oper = Operation::hassign2;
                        break;
                    case 4:
                        assign.oper = Operation::hassign4;
                        break;
                    case 8:
                        assign.oper = Operation::hassign8;
                        break;
                }
                assign.literal.fixed8 = someVariable->stackBytePosition;
                inInstructions.push_back(assign);
            }
        }
    }

    Node* Assignment::copy(StackAllocator& input){
        Assignment* thisptr = input.alloc<Assignment>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Assignment* Assignment::pcopy(StackAllocator& input){
        Assignment* thisptr = input.alloc<Assignment>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }


    
//Add +
    Add::Add(){
        //Do nothing
    }

    Add::Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Add::ToString(std::string inLeft, std::string inRight){
        std::string leftString = left->ToString(inLeft, inRight);
        std::string rightString = right->ToString(inLeft, inRight);
        return leftString + " + " + rightString;
    }

    std::string Add::printAll(){
        return left->printAll() + right->printAll() + "add\n";
    }

    void Add::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        FloridaType theType = left->type;
        Instruction result = Instruction();

        if(left->type < right->type){
            theType = right->type;
        }

        //Determine which operation needs to be generated.
        //Because of the organization in Instruction.cpp,
        //A linear combination can find the operation type.
        result.oper = (Operation) ((theType - FloridaType::ufixed1) * 5 + Operation::ufixed1add);

        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(result);
    }

    Node* Add::copy(StackAllocator& input){
        Add* thisptr = input.alloc<Add>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Add* Add::pcopy(StackAllocator& input){
        Add* thisptr = input.alloc<Add>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Subtract -
    Subtract::Subtract(){
        //Do nothing
    }

    Subtract::Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Subtract::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " - " + right->ToString(inLeft, inRight);
    }

    std::string Subtract::printAll(){
        return left->printAll() + right->printAll() + "subtract\n";
    }

    void Subtract::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        FloridaType theType = left->type;
        Instruction result = Instruction();

        if(left->type < right->type){
            theType = right->type;
        }

        //Determine which operation needs to be generated.
        //Because of the organization in Instruction.cpp,
        //A linear combination can find the operation type.
        result.oper = (Operation) ((theType - FloridaType::ufixed1) * 5 + Operation::ufixed1subtract);

        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(result);
    }

    Node* Subtract::copy(StackAllocator& input){
        Subtract* thisptr = input.alloc<Subtract>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Subtract* Subtract::pcopy(StackAllocator& input){
        Subtract* thisptr = input.alloc<Subtract>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Multiply *
    Multiply::Multiply(){
        //Do nothing
    }

    Multiply::Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Multiply::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " * " + right->ToString(inLeft, inRight);
    }

    std::string Multiply::printAll(){
        return left->printAll() + right->printAll() + "multiply\n";
    }

    void Multiply::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        FloridaType theType = left->type;
        Instruction result = Instruction();

        if(left->type < right->type){
            theType = right->type;
        }

        //Determine which operation needs to be generated.
        //Because of the organization in Instruction.cpp,
        //A linear combination can find the operation type.
        result.oper = (Operation) ((theType - FloridaType::ufixed1) * 5 + Operation::ufixed1multiply);

        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(result);
    }

    Node* Multiply::copy(StackAllocator& input){
        Multiply* thisptr = input.alloc<Multiply>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Multiply* Multiply::pcopy(StackAllocator& input){
        Multiply* thisptr = input.alloc<Multiply>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Divide /
    Divide::Divide(){
        //Do nothing
    };

    Divide::Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Divide::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " / " + right->ToString(inLeft, inRight);
    }

    std::string Divide::printAll(){
        return left->printAll() + right->printAll() + "divide\n";
    }

    void Divide::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        FloridaType theType = left->type;
        Instruction result = Instruction();

        if(left->type < right->type){
            theType = right->type;
        }

        //Determine which operation needs to be generated.
        //Because of the organization in Instruction.cpp,
        //A linear combination can find the operation type.
        result.oper = (Operation) ((theType - FloridaType::ufixed1) * 5 + Operation::ufixed1divide);

        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(result);
    }

    Node* Divide::copy(StackAllocator& input){
        Divide* thisptr = input.alloc<Divide>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Divide* Divide::pcopy(StackAllocator& input){
        Divide* thisptr = input.alloc<Divide>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Parentheses ()
    Parentheses::Parentheses(){
        //Do nothing
    }

    Parentheses::Parentheses(Node* input){
        subexpression = input;
    }

    std::string Parentheses::ToString(std::string inLeft, std::string inRight){
        return "(" + subexpression->ToString(inLeft, inRight) + ")";
    }

    std::string Parentheses::printAll(){
        return subexpression->printAll();
    }

    void Parentheses::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        subexpression->FLVMCodeGen(inInstructions);
    }

    Node* Parentheses::copy(StackAllocator& input){
        Parentheses* thisptr = input.alloc<Parentheses>();

        thisptr->subexpression = subexpression->copy(input);

        return thisptr;
    }

    Parentheses* Parentheses::pcopy(StackAllocator& input){
        Parentheses* thisptr = input.alloc<Parentheses>();

        thisptr->subexpression = subexpression->copy(input);

        return thisptr;        
    }



//Negative -
    Negative::Negative(){
        //Do nothing;
    }

    Negative::Negative(Node* input){
        right = input;
    }

    std::string Negative::ToString(std::string inLeft, std::string inRight){
        return "-" + right->ToString(inLeft, inRight);
    }

    std::string Negative::printAll(){
        return right->printAll() + "negate\n";
    }

    void Negative::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        //Determine which operation needs to be generated.
        //Because of the organization in Instruction.cpp,
        //A linear combination can find the operation type.
        result.oper = (Operation) ((right->type - FloridaType::ufixed1) * 5 + Operation::ufixed1negate);

        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(result);
    }

    Node* Negative::copy(StackAllocator& input){
        Negative* thisptr = input.alloc<Negative>();

        thisptr->right = right->copy(input);

        return thisptr;
    }

    Negative* Negative::pcopy(StackAllocator& input){
        Negative* thisptr = input.alloc<Negative>();

        thisptr->right = right->copy(input);

        return thisptr;        
    }



//Equal ==
    Equal::Equal(){
        //Do nothing
    }

    Equal::Equal(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Equal::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " == " + right->ToString(inLeft, inRight);
    }

    std::string Equal::printAll(){
        return left->printAll() + right->printAll() + "equals\n";
    }

    void Equal::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for ==
        inInstructions.push_back(Instruction(equals));
    }

    Node* Equal::copy(StackAllocator& input){
        Equal* thisptr = input.alloc<Equal>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Equal* Equal::pcopy(StackAllocator& input){
        Equal* thisptr = input.alloc<Equal>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Not equal !=
    NotEqual::NotEqual(){
        //Do nothing
    }

    NotEqual::NotEqual(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string NotEqual::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " != " + right->ToString(inLeft, inRight);
    }

    std::string NotEqual::printAll(){
        return left->printAll() + right->printAll() + "nequals\n";
    }

    void NotEqual::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for !=
        inInstructions.push_back(Instruction(nequals));
    }

    Node* NotEqual::copy(StackAllocator& input){
        NotEqual* thisptr = input.alloc<NotEqual>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    NotEqual* NotEqual::pcopy(StackAllocator& input){
        NotEqual* thisptr = input.alloc<NotEqual>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;        
    }



//Greater than >
    GreaterThan::GreaterThan(){
        //Do nothing
    }

    GreaterThan::GreaterThan(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string GreaterThan::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " > " + right->ToString(inLeft, inRight);
    }

    std::string GreaterThan::printAll(){
        return left->printAll() + right->printAll() + "greater\n";
    }

    void GreaterThan::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >
        inInstructions.push_back(Instruction(greater));
    }

    Node* GreaterThan::copy(StackAllocator& input){
        GreaterThan* thisptr = input.alloc<GreaterThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    GreaterThan* GreaterThan::pcopy(StackAllocator& input){
        GreaterThan* thisptr = input.alloc<GreaterThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Greater than or equal to >=
    GreaterThanOr::GreaterThanOr(){
        //Do nothing
    }

    GreaterThanOr::GreaterThanOr(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string GreaterThanOr::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " >= " + right->ToString(inLeft, inRight);
    }

    std::string GreaterThanOr::printAll(){
        return left->printAll() + right->printAll() + "greateror\n";
    }

    void GreaterThanOr::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >=
        inInstructions.push_back(Instruction(greateror));
    }

    Node* GreaterThanOr::copy(StackAllocator& input){
        GreaterThanOr* thisptr = input.alloc<GreaterThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    GreaterThanOr* GreaterThanOr::pcopy(StackAllocator& input){
        GreaterThanOr* thisptr = input.alloc<GreaterThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Less than <
    LessThan::LessThan(){
        //Do nothing
    }

    LessThan::LessThan(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string LessThan::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " < " + right->ToString(inLeft, inRight);
    }

    std::string LessThan::printAll(){
        return left->printAll() + right->printAll() + "lesser\n";
    }

    void LessThan::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <
        inInstructions.push_back(Instruction(lesser));
    }

    Node* LessThan::copy(StackAllocator& input){
        LessThan* thisptr = input.alloc<LessThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    LessThan* LessThan::pcopy(StackAllocator& input){
        LessThan* thisptr = input.alloc<LessThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }


//Less than or equal to <=
    LessThanOr::LessThanOr(){
        //Do nothing
    }

    LessThanOr::LessThanOr(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string LessThanOr::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " <= " + right->ToString(inLeft, inRight);
    }

    std::string LessThanOr::printAll(){
        return left->printAll() + right->printAll() + "lesseror\n";
    }

    void LessThanOr::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <=
        inInstructions.push_back(Instruction(lesseror));
    }

    Node* LessThanOr::copy(StackAllocator& input){
        LessThanOr* thisptr = input.alloc<LessThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    LessThanOr* LessThanOr::pcopy(StackAllocator& input){
        LessThanOr* thisptr = input.alloc<LessThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;        
    }



//Or OR
    Or::Or(){
        //Do nothing
    };

    Or::Or(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Or::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " OR " + right->ToString(inLeft, inRight);
    }

    std::string Or::printAll(){
        return left->printAll() + right->printAll() + "ior\n";
    }

    void Or::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for OR
        inInstructions.push_back(Instruction(ior));
    }

    Node* Or::copy(StackAllocator& input){
        Or* thisptr = input.alloc<Or>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Or* Or::pcopy(StackAllocator& input){
        Or* thisptr = input.alloc<Or>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//And AND
    And::And(){
        //Do nothing
    }

    And::And(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string And::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " AND " + right->ToString(inLeft, inRight);
    }

    std::string And::printAll(){
        return left->printAll() + right->printAll() + "iand\n";
    }
    
    void And::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for AND
        inInstructions.push_back(Instruction(iand));
    }

    Node* And::copy(StackAllocator& input){
        And* thisptr = input.alloc<And>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    And* And::pcopy(StackAllocator& input){
        And* thisptr = input.alloc<And>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Not !
    Not::Not(){
        //Do nothing
    }

    Not::Not(Node* inRight){
        right = inRight; 
    }

    std::string Not::ToString(std::string inLeft, std::string inRight){
        return "!" + right->ToString(inLeft, inRight);
    }

    std::string Not::printAll(){
        return right->printAll() + "inot\n";
    }

    void Not::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(inot));
    }

    Node* Not::copy(StackAllocator& input){
        Not* thisptr = input.alloc<Not>();

        thisptr->right = right->copy(input);

        return thisptr;
    }

    Not* Not::pcopy(StackAllocator& input){
        Not* thisptr = input.alloc<Not>();

        thisptr->right = right->copy(input);

        return thisptr;        
    }



//if
    IfClass::IfClass(){
        //Do nothing
    };

    std::string IfClass::ToString(std::string inLeft, std::string inRight){
        if(elseBody == nullptr){
            return inLeft + "\x1b[36mif\x1b[0m(" + condition->ToString(inLeft, inRight) + "){\n" + 
            ifBody->ToString("  " + inLeft, ";") + "\n" +
            inLeft + "}";
        } else {
            return inLeft + "\x1b[36mif\x1b[0m(" + condition->ToString(inLeft, ";") + "){\n" + 
            ifBody->ToString("  " + inLeft, inRight) + "\n" +
            inLeft + "} else {\n" + elseBody->ToString("  " + inLeft, ";") + "\n" +
            inLeft + "}";
        }
    }

    std::string IfClass::printAll(){
        std::string result = "\t(*condition*)\n" + condition->printAll();
        if(ifBody != nullptr){
            result += "\t(*if body*)\n" + ifBody->printAll();
        }
        if(elseBody != nullptr){
            result += "\t(*else body*)\n" + elseBody->printAll() + "\t(*end else*)";
        } else {
            result += "\t(*end if*)\n";
        }

        return result;
    }

    void IfClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //If there is not an else statement, generate code for just an if statement.
        //otherwise, generate both bodies and cjumps as needed.
        if(elseBody == nullptr){
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Mark where the conditional jump instruction will be.
            int64_t cjumpPosition = inInstructions.size();

            //Push a cjump instruction. This will be edited later.
            inInstructions.push_back(Instruction(Operation::cjump));
            ifBody->FLVMCodeGen(inInstructions);

            //Adjust the conditional jump destination.
            inInstructions[cjumpPosition].literal.fixed8 = inInstructions.size() - cjumpPosition;
        } else {
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Get the size of the instruction stack before the body.
            //This is also where the cjump instruction will be.
            int64_t cjumpPosition = inInstructions.size();

            //Push a cjump instruction. This will be edited later.
            inInstructions.push_back(Instruction(Operation::cjump));
            ifBody->FLVMCodeGen(inInstructions);

            //Push an unconditional jump to skip over the else statement.
            //This will only be reached if the first branch is taken.
            inInstructions.push_back(Instruction(Operation::jump));

            //Get the size of the instructions set after the if body is made.
            int64_t endIfSize = inInstructions.size();

            //Adjust the conditional jump destination.
            inInstructions[cjumpPosition].literal.fixed8 = endIfSize - cjumpPosition;
            
            //Generate the bytecode for the else statement.
            elseBody->FLVMCodeGen(inInstructions);

            //Adjust the unconditional jump destination.
            inInstructions[endIfSize - 1].literal.fixed8 = inInstructions.size();
        }

    }

    Node* IfClass::copy(StackAllocator& input){
        IfClass* thisptr = input.alloc<IfClass>();

        //Check for a condition.
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        //Check for an ifBody
        if(ifBody != nullptr){
            thisptr->ifBody = ifBody->pcopy(input);
        }
        //Check for an elseBody
        if(elseBody != nullptr){
            thisptr->elseBody = elseBody->pcopy(input);
        }

        return thisptr;
    }

    IfClass* IfClass::pcopy(StackAllocator& input){
        IfClass* thisptr = input.alloc<IfClass>();

        //Check for a condition.
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        //Check for an ifBody
        if(ifBody != nullptr){
            thisptr->ifBody = ifBody->pcopy(input);
        }
        //Check for an elseBody
        if(elseBody != nullptr){
            thisptr->elseBody = elseBody->pcopy(input);
        }

        return thisptr;
    }



//for
    ForLoop::ForLoop(){
        //Do nothing
    };

    std::string ForLoop::ToString(std::string inLeft, std::string inRight){
        std::string finalString = inLeft + "\x1b[34mfor\x1b[0m(";
        if(assign != nullptr){
            finalString += assign->ToString("", ";");
        } else {
            finalString += ";";
        }

        if(condition != nullptr){
            finalString += " " + condition->ToString("", ";");
        }
        finalString += ";";

        if(incrementer != nullptr){
            finalString += " " + incrementer->ToString("", "");
        }
        finalString += "){\n";

        return finalString + inLeft + body->ToString(inLeft, inRight) + inLeft + "\n" + inLeft + "}\n";
    }

    std::string ForLoop::printAll(){
        std::string result = "  (*Start of for loop*)\n";
        if(assign != nullptr){
            result += "\t(*assignment*)\n" + assign->printAll();
        }
        if(condition != nullptr){
            result += "\t(*condition*)\n" + condition->printAll() + 
                padding2("cjump") + "(*cjump out of the loop*)\n";
        }
        if(body != nullptr){
            result += "\t(*body*)\n" + body->printAll();
        }
        if(incrementer != nullptr){
            result += "\t(*incrementer*)\n" + incrementer->printAll();
        }
        result += padding2("cjump") + "(*cjump to the condition start*)\n\t(*End of for loop*)\n";

        return result;
    }

    void ForLoop::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //The initial value of a for loop.
        if(assign != nullptr){
            assign->FLVMCodeGen(inInstructions);
        }

        //Start the scope, because we don't want to continually assign/initialize.
        inInstructions.push_back(Instruction(Operation::newScope));

        //This will be where the end of the loop will unconditionally jump to.
        int64_t jumpTo = inInstructions.size();

        //The condition for a for loop to terminate.
        //If none is provided, then it will perpetually loop.
        if(condition != nullptr){
            condition->FLVMCodeGen(inInstructions);
        } else {
            inInstructions.push_back(Instruction(Operation::push, false));
        }

        //Pushing the conditional jump that can break the loop.
        int64_t cjumpPosition = inInstructions.size();
        inInstructions.push_back(Instruction(Operation::cjump));

        //Add the body of instructions.
        body->body->FLVMCodeGen(inInstructions);

        //Add another line of code for changes at the end of the loop.
        if(incrementer != nullptr){
            incrementer->FLVMCodeGen(inInstructions);
        }

        //Place the unconditional jump instruction at the end.
        inInstructions.push_back(Instruction(jump, jumpTo));

        //Adjust the position of the conditional jump to be outside of the loop
        inInstructions[cjumpPosition].literal.fixed8 = inInstructions.size() - cjumpPosition;

        //End the existing scope.
        inInstructions.push_back(Instruction(Operation::deleteScope));
    }

    Node* ForLoop::copy(StackAllocator& input){
        ForLoop* thisptr = input.alloc<ForLoop>();

        if(assign != nullptr){
            thisptr->assign = assign->copy(input);
        }
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(incrementer != nullptr){
            thisptr->incrementer = incrementer->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }

    ForLoop* ForLoop::pcopy(StackAllocator& input){
        ForLoop* thisptr = input.alloc<ForLoop>();

        if(assign != nullptr){
            thisptr->assign = assign->copy(input);
        }
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(incrementer != nullptr){
            thisptr->incrementer = incrementer->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }



//while
    WhileLoop::WhileLoop(){
        //Do nothing
    };

    std::string WhileLoop::ToString(std::string inLeft, std::string inRight){
        return inLeft + "while(" + condition->ToString(inLeft, inRight) + "){\n" + body->ToString("  " + inLeft, inRight) + inLeft + "}";
    }

    std::string WhileLoop::printAll(){
        std::string result = "";
        if(condition != nullptr){
            result += "\t(*condition*)\n" + condition->printAll() + 
                "negate\n" +
                padding2("cjump") + "\t(*cjump to escape the loop*)\n";
        }
        if(body != nullptr){
            result += "\t(*body*)\n" + body->printAll() + 
                padding2("cjump") + "\t(*cjump to the condition*)\n\t(*End of while loop*)";
        }

        return result;
    }

    void WhileLoop::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.oper = Operation::push;
        result.literal.fixed8 = body->whichScope;
        //Push the corresponding scope value to the instructions.
        inInstructions.push_back(result);
        //Start of the scope, granted while loops don't really need scopes.
        //It just makes work easier for me.
        inInstructions.push_back(Instruction(Operation::newScope));
        //Where the unconditional jump will always land.
        size_t start = inInstructions.size();
        //Generate the code for the condition, if any.
        condition->FLVMCodeGen(inInstructions);
        //The location of the conditional jump statement.
        size_t here = inInstructions.size();
        //Lay down the conditional jump.
        inInstructions.push_back(Instruction(Operation::cjump));
        //Generate the code for the body.
        body->body->FLVMCodeGen(inInstructions);
        //Place an unconditional jump to restart the loop.
        inInstructions.push_back(Instruction(Operation::jump, start));
        //Adjust where the unconditional jump will land.
        inInstructions[here].literal.fixed8 = inInstructions.size() - here;
        //End the scope.
        inInstructions.push_back(Instruction(Operation::deleteScope));
    }

    Node* WhileLoop::copy(StackAllocator& input){
        WhileLoop* thisptr = input.alloc<WhileLoop>();

        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }

    WhileLoop* WhileLoop::pcopy(StackAllocator& input){
        WhileLoop* thisptr = input.alloc<WhileLoop>();

        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }



//Function
    Function::Function(){
        //Do nothing
    };

    std::string Function::ToString(std::string inLeft, std::string inRight){
        std::string varString = "";
        Initialize* currInit = code->allInitializations;

        //Combine all the variables, if any.
        if(code->allInitializations != nullptr){
            //Stop right before the last variable to not append an extra comma.
            for(int i = 1; i < argumentCount; i++){
                varString += "\x1b[36m" + typeString(currInit->thisVariable->thisToken.type) + "\x1b[0m " + currInit->thisVariable->thisToken.getName() + ", ";
                currInit = currInit->next;
            }
            //Append the last variable without an extra comma.
            varString += "\x1b[36m" + typeString(currInit->thisVariable->thisToken.type) + "\x1b[0m " + currInit->thisVariable->thisToken.getName();
        }
        //Return the function printed in the only correct format.
        std::string result = inLeft + "\x1b[36m" + typeString(type) + "\x1b[35m " + std::string(name) + "\x1b[0m(" + varString + "){\n" + 
            code->ToString("  " + inLeft, ";") +
        "\n" + inLeft + "}\n";

        if(next != nullptr){
            return result + next->ToString(inLeft, inRight);
        } else {
            return result;
        }

    }

    std::string Function::printAll(){
        std::string result = "\t(*Function " + std::string(name) + "(";
        Initialize* thisInitialize = code->allInitializations;

        for(int64_t i = 0; i < argumentCount - 1; i++){
            result += std::string(thisInitialize->thisVariable->thisToken.name) + ", ";
            thisInitialize = thisInitialize->next;
        }
        if(thisInitialize != nullptr){
            result += std::string(thisInitialize->thisVariable->thisToken.name);
        }

        result += ")*)\n";

        if(code->body != nullptr){
            result += code->body->printAll() + "\n";
        }
        if(allFunctions != nullptr){
            return result + allFunctions->printAll();
        }
        return result;
    }

    void Function::append(Initialize* input){
        Initialize* currInit = code->allInitializations;
        if(currInit != nullptr){
            while(currInit->next != nullptr){
                currInit = currInit->next;
            }
            currInit->next = input;
        } else {
            code->allInitializations = input;
        }
    }

    void Function::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Change where this function starts in the program.
        if(!alreadyGenerated){
            position = inInstructions.size();
            if(code != nullptr){
                code->body->FLVMCodeGen(inInstructions);
            }
            if(allFunctions != nullptr){
                allFunctions->FLVMCodeGen(inInstructions);
            }
        }
    }

    Node* Function::copy(StackAllocator& input){
        Function* thisptr = input.alloc<Function>();
        Scope* theScope = input.currentScope;
        Function* theFunction = nullptr;

        //Copy the function over.
        thisptr->name = name;
        thisptr->returnable = returnable;
        thisptr->type = type;
        thisptr->position = position;
        thisptr->argumentCount = argumentCount;

        //We have to uniquely copy the scope for the function.
        //This is because function arguments do not have initializations.
        Scope* newScope = input.alloc<Scope>();
        //Assign the newly generated scope to the function.
        thisptr->code = newScope;
        //Adjust the current scope member to the newly made scope.
        newScope->parent = input.currentScope;
        input.currentScope = newScope;
        //Directly copy the arguments over to the new scope.
        //Otherwise, they are never found and added in the correct order.
        Initialize* theirInitialize = code->allInitializations;
        Initialize* newInitialize = nullptr;
        for(int64_t i = 0; i < argumentCount; i++){
            newInitialize = theirInitialize->pcopy(input);
            newScope->push(newInitialize);
            theirInitialize = theirInitialize->next;
        }
        //Copy the body of the function over.
        newScope->body = code->body->pcopy(input);

        //Put the function into the current scope.
        if(theScope->functions == nullptr){
            theScope->functions = thisptr;
        } else {
            theFunction = theScope->functions;
            while(theFunction->next != nullptr){
                theFunction = theFunction->next;
            }
            theFunction->next = thisptr;
        }

        //Add the function to the chain.
        if(input.allFunctions == nullptr){
            input.allFunctions = thisptr;
        } else {
            theFunction = input.allFunctions;
            while(theFunction->allFunctions != nullptr){
                theFunction = theFunction->allFunctions;
            }
            theFunction->allFunctions = thisptr;
        }

        //Move back to the previous scope.
        input.currentScope = input.currentScope->parent;
        return thisptr;
    }

    Function* Function::pcopy(StackAllocator& input){
        Function* thisptr = input.alloc<Function>();

        thisptr->returnable = returnable;
        thisptr->position = position;
        thisptr->code = code->pcopy(input);

        return thisptr;
    }



//Function calls
    Call::Call(){
        //Do nothing
    }

    Call::Call(Function* inFunction){
        function = inFunction;
    }

    std::string Call::ToString(std::string inLeft, std::string inRight){
        std::string theName = "\x1b[35m" + std::string(function->name) + "\x1b[0m";
        std::string theArguments = arguments->ToString(inLeft, "");
        return theName + "(" + theArguments + ")";
    }

    std::string Call::printAll(){
        std::string result = "";

        return "newScope\n" + arguments->printAll() + padding2("call") + "(*Function " + std::string(function->name) + "*)\n";
    }

    void Call::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //This will allow the user to access classes and variables from
        //the most recently created version of this scope.
        Instruction uniqueScopeNumber;
        uniqueScopeNumber.oper = Operation::push;
        uniqueScopeNumber.literal.fixed8 = function->code->whichScope;
        //Push the necessary scope reference onto the stack.
        inInstructions.push_back(uniqueScopeNumber);
        //Start the scope here.
        inInstructions.push_back(Instruction(Operation::newScope));
        //Generate the arguments, if any.
        if(arguments != nullptr){
            arguments->FLVMCodeGen(inInstructions);
        }

        //Make the function call after the function's arguments are created.
        inInstructions.push_back(Instruction(Operation::call, function->position));
    }

    Node* Call::copy(StackAllocator& input){
        Call* thisptr = input.alloc<Call>();
        //Get the list of currently existing functions in the scope
        Scope* theScope = input.currentScope;
        Function* theFunction = input.currentScope->functions;

        //Traverse the linked lists to find the function.
        //It is guaranteed to exist, but may not be in the current scope.
        while(theScope != nullptr){
            theFunction = theScope->functions;
            while(theFunction != nullptr){
                if(theFunction->name == function->name){
                    thisptr->function = theFunction;

                    thisptr->function = theFunction;
                    thisptr->arguments = arguments->pcopy(input);

                    return thisptr;
                }
                theFunction = theFunction->next;
            }
            theScope = theScope->parent;
        }

        //Technically unreachable
        return nullptr; 
    }

    Call* Call::pcopy(StackAllocator& input){
        Call* thisptr = input.alloc<Call>();
        //Get the list of currently existing functions in the scope
        Scope* theScope = input.currentScope;
        Function* theFunction = input.currentScope->functions;

        //Traverse the linked lists to find the function.
        //It is guaranteed to exist, but may not be in the current scope.
        while(theScope != nullptr){
            theFunction = theScope->functions;
            while(theFunction != nullptr){
                if(theFunction->name == function->name){
                    thisptr->function = theFunction;

                    thisptr->function = theFunction;
                    thisptr->arguments = arguments->pcopy(input);

                    return thisptr;
                }
                theFunction = theFunction->next;
            }
            theScope = theScope->parent;
        }

        //Technically unreachable
        return nullptr; 
    }



//Arguments
    Arguments::Arguments(){
        //Do nothing lmfao
        //The default settings are already in place.
    }

    Arguments::Arguments(Node* input){
        current = input;
    }

    std::string Arguments::ToString(std::string inLeft, std::string inRight){
        if(next != nullptr){
            return current->ToString(inLeft, "") + ", " + next->ToString(inLeft, "");
        } else {
            return current->ToString(inLeft, "");
        }

        //Unreachable, just like uncountable infinity.
        return "";

    }

    std::string Arguments::printAll(){
        if(next != nullptr){
            return current->printAll() + next->printAll();
        } else {
            return current->printAll();
        }

        //Unreachable, just like that 1600 SAT score.
        return "";

    }

    void Arguments::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Arguments* currArgs = this;
        while(currArgs->next != nullptr){
            currArgs->current->FLVMCodeGen(inInstructions);
            currArgs = currArgs->next;
        }
        currArgs->current->FLVMCodeGen(inInstructions);
    }

    Node* Arguments::copy(StackAllocator& input){
        Arguments* thisptr = input.alloc<Arguments>();

        if(current != nullptr){
            thisptr->current = current->copy(input);
        }
        if(next != nullptr){
            thisptr->next = next->pcopy(input);
        }

        return thisptr;
    }

    Arguments* Arguments::pcopy(StackAllocator& input){
        Arguments* thisptr = input.alloc<Arguments>();

        if(current != nullptr){
            thisptr->current = current->copy(input);
        }
        if(next != nullptr){
            thisptr->next = next->pcopy(input);
        }

        return thisptr;        
    }



//Return statements
    ReturnClass::ReturnClass(){
        //Do nothing
    }

    std::string ReturnClass::ToString(std::string inLeft, std::string inRight){
        return inLeft + "\x1b[34mreturn\x1b[0m " + statement->ToString(inLeft, inRight) + inRight;
    }

    std::string ReturnClass::printAll(){
        return statement->printAll() + padding("return") + std::to_string(returnCount) + "\n";
    }

    void ReturnClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //There will be code with the return statement.
        statement->FLVMCodeGen(inInstructions);
        Instruction result = Instruction();
        result.oper = Operation::ireturn;
        //This determines how many scopes to exit.
        result.literal.fixed8 = returnCount;
        inInstructions.push_back(result);
    }

    Node* ReturnClass::copy(StackAllocator& input){
        ReturnClass* thisptr = input.alloc<ReturnClass>();

        if(statement != nullptr){
            thisptr->statement = statement->copy(input);
        }

        return thisptr;
    }

    ReturnClass* ReturnClass::pcopy(StackAllocator& input){
        ReturnClass* thisptr = input.alloc<ReturnClass>();

        if(statement != nullptr){
            thisptr->statement = statement->copy(input);
        }

        return thisptr;        
    }



//Objects
    ObjectClass::ObjectClass(){
        type = FloridaType::Object;
    }

    std::string ObjectClass::ToString(std::string inLeft, std::string inRight){
        std::string theInitialize = "";

        return "object " + std::string(name) + "{\n" + 
            code->body->ToString("  " + inLeft, inRight) + "\n" +
        inLeft + "}\n";
    }

    std::string ObjectClass::printAll(){
        //There is nothing to print.
        return "";
    }

    void ObjectClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //TO DO
    }

    Node* ObjectClass::copy(StackAllocator& input){
        ObjectClass* result = input.alloc<ObjectClass>();
        Scope* currentScope = input.currentScope;

        result->name = name;
        result->code = code->pcopy(input);
        currentScope->push(result);

        return result;
    }

    ObjectClass* ObjectClass::pcopy(StackAllocator& input){
        ObjectClass* result = input.alloc<ObjectClass>();
        Scope* currentScope = input.currentScope;

        result->name = name;
        result->code = code->pcopy(input);
        currentScope->push(result);

        return result;
    }



//MemberAccess
    MemberAccess::MemberAccess(){
        //Do nothing
    }

    std::string MemberAccess::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + "." + right->ToString(inLeft, inRight);
    }

    std::string MemberAccess::printAll(){
        //TO DO
        return "";
    }

    void MemberAccess::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //TO DO
    }

    Node* MemberAccess::copy(StackAllocator& input){
        MemberAccess* result = input.alloc<MemberAccess>();

        result->left = left->copy(input);
        result->right = right->pcopy(input);

        return result;
    }

    MemberAccess* MemberAccess::pcopy(StackAllocator& input){
        MemberAccess* result = input.alloc<MemberAccess>();

        result->left = left->copy(input);
        result->right = right->pcopy(input);

        return result;
    }

//Dereference
    Dereference::Dereference(){
        //Do nothing
    }

    std::string Dereference::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + "->" + right->ToString(inLeft, inRight);
    }

    std::string Dereference::printAll(){
        //TO DO
        return "";
    }

    void Dereference::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //TO DO
    }

    Node* Dereference::copy(StackAllocator& input){
        Dereference* result = input.alloc<Dereference>();

        result->left = left->copy(input);
        result->right = right->pcopy(input);

        return result;
    }

    Dereference* Dereference::pcopy(StackAllocator& input){
                Dereference* result = input.alloc<Dereference>();
        
        result->left = left->copy(input);
        result->right = right->pcopy(input);

        return result;
    }
