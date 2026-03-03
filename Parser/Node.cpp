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

//Abuse more mathematical mapping methods to simplify this.
std::string fetchPad(FloridaType input, char where){
    std::string result = "fetch";
    const int64_t bitmask = 3;
    //Determine the byte size of the type in question.
    input = (FloridaType) (bitmask & (input - FloridaType::ufixed1));

    //Assign the appropriate locality.
    switch(where){
        case 0:
            result = "l" + result;
            break;
        case 1:
            result = "m" + result;
            break;
        case 2:
            result = "g" + result;
            break;
        case 3:
            result = "h" + result;
            break;
    }
    
    //Assign the appropriate byte size.
    switch(input){
        case 0:
            result += "1";
            break;
        case 1:
            result += "2";
            break;
        case 2:
            result += "4";
            break;
        case 3:
            result += "8";
            break;
        default:
            //Do nothing, just make the compiler stop complaining.
            break;
    }

    return result;
}

//Abuse more mathematical mapping methods to simplify this.
std::string assignPad(FloridaType input, char where){
    std::string result = "assign";
    const int64_t bitmask = 3;
    //Determine the byte size of the type in question.
    input = (FloridaType) (bitmask & (input - FloridaType::ufixed1));

    //Assign the appropriate locality.
    switch(where){
        case 0:
            result = "l" + result;
            break;
        case 1:
            result = "m" + result;
            break;
        case 2:
            result = "g" + result;
            break;
        case 3:
            result = "h" + result;
            break;
    }
    
    //Assign the appropriate byte size.
    switch(input){
        case 0:
            result += "1";
            break;
        case 1:
            result += "2";
            break;
        case 2:
            result += "4";
            break;
        case 3:
            result += "8";
            break;
        default:
            //Do nothing, just make the compiler stop complaining.
            break;
    }

    return result;
}



//TypecastClass
    TypecastClass::TypecastClass(){
        //This is just a placeholder. It will be changed later.
        type = FloridaType::Typecast;
    }

    void TypecastClass::ToString(std::string inLeft, std::string inRight){
        body->ToString(inLeft, inRight);
    }

    void TypecastClass::FLVMCodeGen(Instructions* inInstructions){
        types result;
        body->FLVMCodeGen(inInstructions);
        //Once again, I abuse the integer nature to get the correct operation.
        //This will pick the correct one among 100 total options.
        //10 of those are redundant and only necessary to let the math work.
        result.fixed8 = ((body->type - FloridaType::ufixed1) * 10 + (type - FloridaType::ufixed1) + ufixed1TOufixed1);

        inInstructions->push(result);
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

        //Reach the end of the sorted list.
        currentInitialize = sortedInitializations;
        while(currentInitialize->memoryOrder != nullptr){
            currentInitialize = currentInitialize->memoryOrder;
        }
        //If the input is some object, then place it
        //with the other variables whose memory sizes
        //are multiples of 8.
        if(input->thisVariable->objectType != nullptr){
            while(currentInitialize->thisVariable->objectType != nullptr){
                currentInitialize = currentInitialize->memoryOrder;
            }

            input->memoryOrder = currentInitialize->memoryOrder;
            currentInitialize->memoryOrder = input;
            return;
        }

        currentInitialize = sortedInitializations;
        //This will place input in a sorted place based on size in the linked list.
        while(currentInitialize != nullptr){
            theirSize = allocationSize(currentInitialize->thisVariable->type);
            if(byteSize <= theirSize){
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


    
    void Scope::ToString(std::string inLeft, std::string inRight){
        if(body != nullptr){
            body->ToString(inLeft, ";");
        }
    }

    void Scope::FLVMCodeGen(Instructions* inInstructions){
        types result;

        //The new scope will use the last value to pick the proper scope and adjust its value.
        result.operation[0] = Operation::newScope;
        inInstructions->push(result);
        //Be sure to add this otherwise I can never adjust the unique scopes.
        result.fixed8 = whichScope;
        inInstructions->push(result);
        //This is so I can preemptively allocate slots for the scope's variables.
        result.fixed8 = variableSlotSize;
        inInstructions->push(result);
        if(body != nullptr){
            body->FLVMCodeGen(inInstructions);
        }
        //Push the delete scope operation onto the stack.
        result.operation[0] = Operation::deleteScope;
        inInstructions->push(result);
        //Add which here to be able to adjust the unique Scope here too.
        result.fixed8 = whichScope;
        inInstructions->push(result);
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

    void Primitive::ToString(std::string inLeft, std::string inRight){
        switch(type){
            case ufixed1:
                std::cout << std::to_string(value.ufixed1[0]);
                break;
            case ufixed2:
                std::cout << std::to_string(value.ufixed2[0]);
                break;
            case ufixed4:
                std::cout << std::to_string(value.ufixed4[0]);
                break;
            case ufixed8:
                std::cout << std::to_string(value.ufixed8);
                break;
            case fixed1:
                std::cout << std::to_string(value.fixed1[0]);
                break;
            case fixed2:
                std::cout << std::to_string(value.fixed2[0]);
                break;
            case fixed4:
                std::cout << std::to_string(value.fixed4[0]);
                break;
            case fixed8:
                std::cout << std::to_string(value.fixed8);
                break;
            case float4:
                std::cout << std::to_string(value.float4[0]);
                break;
            case float8:
                std::cout << std::to_string(value.float8);
                break;
            case Bool:
                if(value.boolean[0]){
                    std::cout << "\x1b[32mtrue\x1b[0m";
                } else {
                    std::cout << "\x1b[31mfalse\x1b[0m";
                }
                break;
            default:
                break;
            //Error
        }
    }

    void Primitive::FLVMCodeGen(Instructions* inInstructions){
        types result;
        //Generate a push instruction.
        result.operation[0] = Operation::push;
        inInstructions->push(result);
        //The primitive to be pushed.
        result = value;
        inInstructions->push(result);
    }



//Body
    Body::Body(){
        current = nullptr;
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

    void Body::ToString(std::string inLeft, std::string inRight){
        current->ToString(inLeft, inRight);
        std::cout << "\n";
        if(next != nullptr){
            next->ToString(inLeft, inRight);
        }
    }

    void Body::FLVMCodeGen(Instructions* inInstructions){
        //Add the current chunk of code if it is not a function.
        if(dynamic_cast<Function*>(current) == nullptr){
            current->FLVMCodeGen(inInstructions);
            //After the line has finished execution, pop it from the stack.
            //inInstructions.push_back(Instruction(Operation::pop));
        }
        //If next isn't a nullptr, then generate code for it too.
        if(next != nullptr){
            next->FLVMCodeGen(inInstructions);
        }
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

    void Variable::ToString(std::string inLeft, std::string inRight){
        std::cout << thisToken.getName();
    }

    void Variable::FLVMCodeGen(Instructions* inInstructions){
        types result;
        //Check if the variable is an object.
        //If it is, fetch everything.
        if(objectType != nullptr){
            //This will fetch the full object.
            for(uint64_t i = 0; i < objectType->memorySize / 8; i++){
                result.operation[0] = Operation::fetch8;
                inInstructions->push(result);
                result.fixed8 = stackBytePosition + i * 8;
                inInstructions->push(result);
            }

            return;
        }

        //More math-magic. I can map the `type` to the correct `operation`.
        int64_t bitmask = 7L;
        //Determine which fetch operation is needed.
        result.operation[0] = (Operation) ((bitmask & (type - FloridaType::ufixed1)) + Operation::assign1);
        inInstructions->push(result);
        //Get which scope the variable is in.
        result.fixed8 = owner->whichScope;
        inInstructions->push(result);
        //This is where in the stack the variable starts.
        result.fixed8 = stackBytePosition;
        inInstructions->push(result);
    }



//Initialize
    Initialize::Initialize(){
        //Do nothing.
    };

    void Initialize::ToString(std::string inLeft, std::string inRight){ 
        if(thisVariable->objectType != nullptr){
            if(code != nullptr){
                std::cout <<  inLeft << std::string(thisVariable->objectType->name) << " " << thisVariable->thisToken.getName() << " = ";
                code->ToString(inLeft, inRight);
                std::cout << inRight;
            }
            std::cout << inLeft << std::string(thisVariable->objectType->name) << " " << thisVariable->thisToken.getName() << inRight;
        }
        if(code != nullptr){
            std::cout << inLeft << typeString(thisVariable->thisToken.type) << " " << thisVariable->thisToken.getName() << " = ";
            code->ToString(inLeft, inRight);
            std::cout << inRight;
        }
        std::cout << inLeft << typeString(thisVariable->thisToken.type) << " " << thisVariable->thisToken.getName() << inRight;
    }

    void Initialize::FLVMCodeGen(Instructions* inInstructions){
        //If there's code, then generate it.
        if(code != nullptr){
            //Generate the code for the right hand side.
            code->FLVMCodeGen(inInstructions);

            //Get the correct byte size.
            types result;
            switch(allocationSize(type)){
                case 1:
                    result.operation[0] = Operation::assign1;
                    break;
                case 2:
                    result.operation[0] = Operation::assign2;
                    break;
                case 4:
                    result.operation[0] = Operation::assign4;
                    break;
                case 8:
                    result.operation[0] = Operation::assign8;
                    break;
            }
            inInstructions->push(result);
            //Get which scope this belongs to.
            result.fixed8 = thisVariable->owner->whichScope;
            inInstructions->push(result);
            //Get the stack offset.
            result.fixed8 = thisVariable->stackBytePosition;
            inInstructions->push(result);
        }
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

    void Assignment::ToString(std::string inLeft, std::string inRight){
        std::cout << inLeft;
        left->ToString(inLeft, inRight);
        std::cout << " = ";
        right->ToString(inLeft, inRight);
        std::cout << inRight;
    }

    void Assignment::FLVMCodeGen(Instructions* inInstructions){
        //Evaluate the right hand side before assigning.
        if(right != nullptr){
            right->FLVMCodeGen(inInstructions);
        }
        //Generate the fetch instructions for this object.
        //These will be changed to assignment instructions.
        left->FLVMCodeGen(inInstructions);
        Variable* theVariable = nullptr;
        MemberAccess* theAccess = nullptr;
        Dereference* theDereference = nullptr;

        theVariable = dynamic_cast<Variable*>(left);
        if(theVariable != nullptr){
            theVariable->AssignCodeGen(inInstructions);
            return;
        }

        theAccess = dynamic_cast<MemberAccess*>(left);
        if(theAccess != nullptr){
            theAccess->AssignCodeGen(inInstructions);
            return;
        }

        theDereference = dynamic_cast<Dereference*>(left);
        if(theDereference != nullptr){
            theDereference->AssignCodeGen(inInstructions);
            return;
        }
    }


    
//Add +
    Add::Add(){
        //Do nothing
    }

    void Add::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " + ";
        right->ToString(inLeft, inRight);
    }

    void Add::FLVMCodeGen(Instructions* inInstructions){
        FloridaType theType = left->type;
        types result;

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



//Subtract -
    Subtract::Subtract(){
        //Do nothing
    }

    void Subtract::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " - ";
        right->ToString(inLeft, inRight);
    }

    void Subtract::FLVMCodeGen(Instructions* inInstructions){
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



//Multiply *
    Multiply::Multiply(){
        //Do nothing
    }

    void Multiply::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " * ";
        right->ToString(inLeft, inRight);
    }

    void Multiply::FLVMCodeGen(Instructions* inInstructions){
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



//Divide /
    Divide::Divide(){
        //Do nothing
    };

    void Divide::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " / ";
        right->ToString(inLeft, inRight);
    }

    void Divide::FLVMCodeGen(Instructions* inInstructions){
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



//Parentheses ()
    Parentheses::Parentheses(){
        //Do nothing
    }

    void Parentheses::ToString(std::string inLeft, std::string inRight){
        std::cout << "(";
        subexpression->ToString(inLeft, inRight);
        std::cout << ")";
    }

    void Parentheses::FLVMCodeGen(Instructions* inInstructions){
        subexpression->FLVMCodeGen(inInstructions);
    }



//Negative -
    Negative::Negative(){
        //Do nothing;
    }

    void Negative::ToString(std::string inLeft, std::string inRight){
        std::cout << "-";
        right->ToString(inLeft, inRight);
    }

    void Negative::FLVMCodeGen(Instructions* inInstructions){
        Instruction result = Instruction();
        //Determine which operation needs to be generated.
        //Because of the organization in Instruction.cpp,
        //A linear combination can find the operation type.
        result.oper = (Operation) ((right->type - FloridaType::ufixed1) * 5 + Operation::ufixed1negate);

        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(result);
    }



//Equal ==
    Equal::Equal(){
        //Do nothing
    }

    void Equal::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " == ";
        right->ToString(inLeft, inRight);
    }

    void Equal::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for ==
        inInstructions.push_back(Instruction(equals));
    }



//Not equal !=
    NotEqual::NotEqual(){
        //Do nothing
    }

    void NotEqual::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " != ";
        right->ToString(inLeft, inRight);
    }

    void NotEqual::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for !=
        inInstructions.push_back(Instruction(nequals));
    }



//Greater than >
    GreaterThan::GreaterThan(){
        //Do nothing
    }

    void GreaterThan::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " > ";
        right->ToString(inLeft, inRight);
    }

    void GreaterThan::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >
        inInstructions.push_back(Instruction(greater));
    }



//Greater than or equal to >=
    GreaterThanOr::GreaterThanOr(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " >= " + right->ToString(inLeft, inRight);
    }

    void GreaterThanOr::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >=
        inInstructions.push_back(Instruction(greateror));
    }



//Less than <
    LessThan::LessThan(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " < " + right->ToString(inLeft, inRight);
    }

    void LessThan::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <
        inInstructions.push_back(Instruction(lesser));
    }



//Less than or equal to <=
    LessThanOr::LessThanOr(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " <= " + right->ToString(inLeft, inRight);
    }

    void LessThanOr::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <=
        inInstructions.push_back(Instruction(lesseror));
    }



//Or OR
    Or::Or(){
        //Do nothing
    };

    void ::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " OR " + right->ToString(inLeft, inRight);
    }

    void Or::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for OR
        inInstructions.push_back(Instruction(ior));
    }



//And AND
    And::And(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " AND " + right->ToString(inLeft, inRight);
    }

    void And::FLVMCodeGen(Instructions* inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for AND
        inInstructions.push_back(Instruction(iand));
    }



//Not !
    Not::Not(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return "!" + right->ToString(inLeft, inRight);
    }

    void Not::FLVMCodeGen(Instructions* inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(inot));
    }



//if
    IfClass::IfClass(){
        //Do nothing
    };

    void ::ToString(std::string inLeft, std::string inRight){
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

    void IfClass::FLVMCodeGen(Instructions* inInstructions){
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



//for
    ForLoop::ForLoop(){
        //Do nothing
    };

    void ::ToString(std::string inLeft, std::string inRight){
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

    void ForLoop::FLVMCodeGen(Instructions* inInstructions){
        //The initial value of a for loop.
        if(assign != nullptr){
            assign->FLVMCodeGen(inInstructions);
        }

        //Start the scope, because we don't want to continually assign/initialize.
        inInstructions.push_back(Instruction(Operation::newScope, body->variableSlotSize >> 3));

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



//while
    WhileLoop::WhileLoop(){
        //Do nothing
    };

    void ::ToString(std::string inLeft, std::string inRight){
        return inLeft + "while(" + condition->ToString(inLeft, inRight) + "){\n" + body->ToString("  " + inLeft, inRight) + inLeft + "}";
    }

    void WhileLoop::FLVMCodeGen(Instructions* inInstructions){
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



//Function
    Function::Function(){
        //Do nothing
    };

    void ::ToString(std::string inLeft, std::string inRight){
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

    void Function::FLVMCodeGen(Instructions* inInstructions){
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



//Function calls
    Call::Call(){
        //Do nothing
    }

    Call::Call(Function* inFunction){
        function = inFunction;
    }

    void ::ToString(std::string inLeft, std::string inRight){
        std::string theName = "\x1b[35m" + std::string(function->name) + "\x1b[0m";
        std::string theArguments = arguments->ToString(inLeft, "");
        return theName + "(" + theArguments + ")";
    }

    void Call::FLVMCodeGen(Instructions* inInstructions){
        //This will allow the user to access classes and variables from
        //the most recently created version of this scope.
        Instruction newScopeOperation;
        Instruction theCall;
        newScopeOperation.literal.fixed8 = function->code->variableSlotSize;
        newScopeOperation.secondary = function->code->whichScope;
        
        //Push the necessary scope reference onto the stack.
        inInstructions.push_back(newScopeOperation);
        //Generate the arguments, if any.
        if(arguments != nullptr){
            arguments->FLVMCodeGen(inInstructions);
        }

        theCall.oper = Operation::call;
        theCall.literal.fixed8 = function->position;

        //Make the function call after the function's arguments are created.
        inInstructions.push_back(theCall);
    }



//Arguments
    Arguments::Arguments(){
        //Do nothing lmfao
        //The default settings are already in place.
    }

    void ::ToString(std::string inLeft, std::string inRight){
        if(next != nullptr){
            return current->ToString(inLeft, "") + ", " + next->ToString(inLeft, "");
        } else {
            return current->ToString(inLeft, "");
        }

        //Unreachable, just like uncountable infinity.
        return "";

    }

    void Arguments::FLVMCodeGen(Instructions* inInstructions){
        Arguments* currArgs = this;
        while(currArgs->next != nullptr){
            currArgs->current->FLVMCodeGen(inInstructions);
            currArgs = currArgs->next;
        }
        currArgs->current->FLVMCodeGen(inInstructions);
    }



//Return statements
    ReturnClass::ReturnClass(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return inLeft + "\x1b[34mreturn\x1b[0m " + statement->ToString(inLeft, inRight) + inRight;
    }

    void ReturnClass::FLVMCodeGen(Instructions* inInstructions){
        //There will be code with the return statement.
        statement->FLVMCodeGen(inInstructions);
        Instruction result = Instruction();
        result.oper = Operation::ireturn;
        //This determines how many scopes to exit.
        result.literal.fixed8 = returnCount;
        inInstructions.push_back(result);
    }



//Objects
    ObjectClass::ObjectClass(){
        type = FloridaType::Object;
    }

    void ::ToString(std::string inLeft, std::string inRight){
        std::string theInitialize = "";

        return "object " + std::string(name) + "{\n" + 
            code->body->ToString("  " + inLeft, inRight) + "\n" +
        inLeft + "}\n";
    }

    void ObjectClass::FLVMCodeGen(Instructions* inInstructions){
        //TO DO
    }



//MemberAccess
    MemberAccess::MemberAccess(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + "." + right->ToString(inLeft, inRight);
    }

    void MemberAccess::FLVMCodeGen(Instructions* inInstructions){
        Variable* theVariable = nullptr;
        MemberAccess* theAccess = nullptr;
        Dereference* theDereference = nullptr;
        Node* currentNode = left;

        //This will create a push instruction or adjust the literal of the instruction.
        theAccess = dynamic_cast<MemberAccess*>(currentNode);
        if(theAccess != nullptr){
            theAccess->left->FLVMCodeGen(inInstructions);
            if(inInstructions.back().oper == Operation::push){
                inInstructions.back().literal.fixed8 += theAccess->right->stackBytePosition;
            } else {
                Instruction thePush;
                thePush.oper = Operation::push;
                inInstructions.push_back(thePush);
            }
        }

        //This will create a dereference instruction for the object.
        theDereference = dynamic_cast<Dereference*>(currentNode);
        if(theDereference != nullptr){
            theDereference->left->FLVMCodeGen(inInstructions);  
            //The instructions for this don't exist yet.
        }

        //Reaching a variable means that the chain has ended.
        theVariable = dynamic_cast<Variable*>(currentNode);
        if(theVariable != nullptr){
            theVariable->FLVMCodeGen(inInstructions);
        }
    }



//Dereference
    Dereference::Dereference(){
        //Do nothing
    }

    void ::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + "->" + right->ToString(inLeft, inRight);
    }

    void Dereference::FLVMCodeGen(Instructions* inInstructions){
        Variable* theVariable = nullptr;
        MemberAccess* theAccess = nullptr;
        Dereference* theDereference = nullptr;
        Node* currentNode = left;

        theAccess = dynamic_cast<MemberAccess*>(currentNode);
        if(theAccess != nullptr){
            theAccess->left->FLVMCodeGen(inInstructions);
            if(inInstructions.back().oper == Operation::push){
                inInstructions.back().literal.fixed8 += theAccess->right->stackBytePosition;
            } else {
                Instruction thePush;
                thePush.oper = Operation::push;
                inInstructions.push_back(thePush);
            }
        }

        theDereference = dynamic_cast<Dereference*>(currentNode);
        if(theDereference != nullptr){
            theDereference->left->FLVMCodeGen(inInstructions);
            //The instructions for this don't exist yet.
        }

        //Reaching a variable means that the chain has ended.
        theVariable = dynamic_cast<Variable*>(currentNode);
        if(theVariable != nullptr){
            theVariable->FLVMCodeGen(inInstructions);
        }
    }
