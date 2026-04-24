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
        //Generate the code
        body->FLVMCodeGen(inInstructions);
        //Push the instruction.
        result.operation[0] = Operation::ITypecast;
        inInstructions->push(result);
        //Push the first type.
        result.type[0] = type;
        inInstructions->push(result);
        //Push the second type.
        result.type[0] = secondType;
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
        //Adjust `input` for simplicity.
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
        //are multiples of the architecture's size (x32, x64, x128, etc.).
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
        result.operation[0] = Operation::NewScope;
        inInstructions->push(result);
        //Be sure to addOffset this otherwise I can never adjust the unique scopes.
        result.fixed8 = whichScope;
        inInstructions->push(result);
        //This is so I can preemptively allocate slots for the scope's variables.
        result.fixed8 = variableSlotSize;
        inInstructions->push(result);
        if(body != nullptr){
            body->FLVMCodeGen(inInstructions);
        }
        //Push the delete scope operation onto the stack.
        result.operation[0] = Operation::DeleteScope;
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
        result.operation[0] = Operation::Push;
        inInstructions->push(result);
        //Add the type for debugging purposes.
        result.type[0] = type;
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
        } else {
            if(next != nullptr){
                next->append(input);
            } else {
                next = input;
            }
        }

        return this;

    }

    void Body::ToString(std::string inLeft, std::string inRight){
        current->ToString(inLeft, inRight);
        if(next != nullptr){
            std::cout << '\n';
            next->ToString(inLeft, inRight);
        }
    }

    void Body::FLVMCodeGen(Instructions* inInstructions){
        //Add the current chunk of code if it is not a function.
        std::cout << "Here\n";
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

    void Variable::append(Variable* input, void* head){
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
                std::cout <<  inLeft << std::string(thisVariable->objectType->name);
                std::cout << " " << thisVariable->thisToken.getName() << " = ";
                code->ToString(inLeft, inRight);
                std::cout << inRight;
                return;
            } else {
                std::cout << inLeft << std::string(thisVariable->objectType->name) << " " << thisVariable->thisToken.getName() << inRight;
            }
        } else {
            if(code != nullptr){
                std::cout << inLeft << typeString(thisVariable->thisToken.type) << " " << thisVariable->thisToken.getName() << " = ";
                code->ToString(inLeft, inRight);
                std::cout << inRight;
            } else {
                std::cout << inLeft << typeString(thisVariable->thisToken.type) << " " << thisVariable->thisToken.getName() << inRight;
            }
        }
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
    void Initialize::append(Initialize* input, void* head){
        Initialize* current = this;
        while(current->next != nullptr){
            current = current->next;
        }
        current->next = input;
    }

    //Append `input` to the end of the `memoryOrder` linked list.
    void Initialize::memoryAppend(Initialize* input, void* head){
        Initialize* current = this;
        while(current->memoryOrder != nullptr){
            current = current->memoryOrder;
        }
        current->memoryOrder = input;
    }



//Assignment =
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

        Variable* theVariable = dynamic_cast<Variable*>(left);
        if(theVariable != nullptr){
            //TO DO
            return;
        }

        MemberAccess* theAccess = dynamic_cast<MemberAccess*>(left);
        if(theAccess != nullptr){
            //addOffset(theAccess, head)->AssignCodeGen(inInstructions, head);
            return;
        }

        Dereference* theDereference = dynamic_cast<Dereference*>(left);
        if(theDereference != nullptr){
            //addOffset(theDereference, head)->AssignCodeGen(inInstructions, head);
            return;
        }
    }


    
//Add +
    Addition::Addition(){
        //Do nothing
    }

    void Addition::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " + ";
        right->ToString(inLeft, inRight);
    }

    void Addition::FLVMCodeGen(Instructions* inInstructions){
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push the operation
        result.operation[0] = Operation::IAdd;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }



//Subtraction -
    Subtraction::Subtraction(){
        //Do nothing
    }

    void Subtraction::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " - ";
        right->ToString(inLeft, inRight);
    }

    void Subtraction::FLVMCodeGen(Instructions* inInstructions){
        types result;

        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push the operation
        result.operation[0] = Operation::ISubtract;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }



//Quiet multiplication does not have a character
    QuietMultiplication::QuietMultiplication(){
        //Do nothing
    }

    void QuietMultiplication::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " ";
        right->ToString(inLeft, inRight);
    }

    void QuietMultiplication::FLVMCodeGen(Instructions* inInstructions){
        types result;

        //Generate the code for the left hand object.
        left->FLVMCodeGen(inInstructions);
        //Generate the code for the right hand object.
        right->FLVMCodeGen(inInstructions);
        //Push the operation
        result.operation[0] = Operation::IMultiply;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }


//Multiply *
    Multiplication::Multiplication(){
        //Do nothing
    }

    void Multiplication::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " * ";
        right->ToString(inLeft, inRight);
    }

    void Multiplication::FLVMCodeGen(Instructions* inInstructions){
        types result;
        
        //Generate the code for the left hand object.
        left->FLVMCodeGen(inInstructions);
        //Generate the code for the right hand object.
        right->FLVMCodeGen(inInstructions);
        //Push the operation
        result.operation[0] = Operation::IMultiply;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }



//Division /
    Division::Division(){
        //Do nothing
    };

    void Division::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " / ";
        right->ToString(inLeft, inRight);
    }

    void Division::FLVMCodeGen(Instructions* inInstructions){
        types result;

        //Generate the code for the left and right operands.
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push the operation
        result.operation[0] = Operation::IDivide;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
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
        //Generate the code for the subexpression.
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
        types result;

        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for -
        result.operation[0] = Operation::INegate;
        inInstructions->push(result);
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
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for ==
        result.operation[0] = Operation::equals;
        inInstructions->push(result);
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
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for !=
        result.operation[0] = Operation::nequals;
        inInstructions->push(result);
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
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >
        result.operation[0] = Operation::greater;
        inInstructions->push(result);
    }



//Greater than or equal to >=
    GreaterThanOr::GreaterThanOr(){
        //Do nothing
    }

    void GreaterThanOr::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " >= ";
        right->ToString(inLeft, inRight);
    }

    void GreaterThanOr::FLVMCodeGen(Instructions* inInstructions){
        types result;   
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >=
        result.operation[0] = Operation::greateror;
        inInstructions->push(result);
    }



//Less than <
    LessThan::LessThan(){
        //Do nothing
    }

    void LessThan::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " < ";
        right->ToString(inLeft, inRight);
    }

    void LessThan::FLVMCodeGen(Instructions* inInstructions){
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <
        result.operation[0] = Operation::lesser;
        inInstructions->push(result);
    }



//Less than or equal to <=
    LessThanOr::LessThanOr(){
        //Do nothing
    }

    void LessThanOr::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " <= ";
        right->ToString(inLeft, inRight);
    }

    void LessThanOr::FLVMCodeGen(Instructions* inInstructions){
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <=
        result.operation[0] = Operation::lesseror;
        inInstructions->push(result);
    }



//Or OR
    Or::Or(){
        //Do nothing
    };

    void Or::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " OR ";
        right->ToString(inLeft, inRight);
    }

    void Or::FLVMCodeGen(Instructions* inInstructions){
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for OR
        result.operation[0] = Operation::ior;
        inInstructions->push(result);
    }



//And AND
    And::And(){
        //Do nothing
    }

    void And::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << " AND ";
        right->ToString(inLeft, inRight);
    }

    void And::FLVMCodeGen(Instructions* inInstructions){
        types result;
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for AND
        result.operation[0] = Operation::iand;
        inInstructions->push(result);
    }



//Not !
    Not::Not(){
        //Do nothing
    }

    void Not::ToString(std::string inLeft, std::string inRight){
        std::cout << "!";
        right->ToString(inLeft, inRight);
    }

    void Not::FLVMCodeGen(Instructions* inInstructions){
        types result;
        right->FLVMCodeGen(inInstructions);
        //Push back the operation for !
        result.operation[0] = Operation::inot;
        inInstructions->push(result);
    }



//if
    IfClass::IfClass(){
        //Do nothing
    };

    void IfClass::ToString(std::string inLeft, std::string inRight){
        if(elseBody == nullptr){
            std::cout << inLeft << "\x1b[36mif\x1b[0m(";
            condition->ToString(inLeft, inRight);
            std::cout << "){\n";
                ifBody->ToString("  " + inLeft, ";");
            std::cout << "\n" << inLeft << "}\n";
        } else {
            std::cout << inLeft + "\x1b[36mif\x1b[0m(";
            condition->ToString(inLeft, ";");
            std::cout << "){\n";
                ifBody->ToString("  " + inLeft, inRight);
            std::cout << "\n" << inLeft << "} else {\n";
                elseBody->ToString("  " + inLeft, ";");
            std::cout << "\n" << inLeft << "}\n";
        }
    }

    void IfClass::FLVMCodeGen(Instructions* inInstructions){
        types result;
        //If there is not an else statement, generate code for just an if statement.
        //otherwise, generate both bodies and cjumps as needed.
        if(elseBody == nullptr){
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Mark where the conditional jump instruction will be.
            int64_t cjumpPosition = inInstructions->instructionCount;

            //Push a cjump instruction. This will be edited later.
            result.fixed8 = cjumpPosition;
            inInstructions->push(result);
            ifBody->FLVMCodeGen(inInstructions);

            //Adjust the conditional jump destination.
            inInstructions->instructionSet[cjumpPosition].fixed8 = inInstructions->instructionCount - cjumpPosition;
        } else {
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Mark where the conditional jump instruction will be.
            int64_t cjumpPosition = inInstructions->instructionCount;

            //Push a cjump instruction. This will be edited later.
            result.fixed8 = cjumpPosition;
            inInstructions->push(result);
            ifBody->FLVMCodeGen(inInstructions);

            //Push an unconditional jump to skip over the else statement.
            //This will only be reached if the first branch is taken.
            result.operation[0] = Operation::Jump;
            inInstructions->push(result);

            //Get the size of the instructions set after the if body is made.
            int64_t endIfSize = inInstructions->instructionCount;

            //Adjust the conditional jump destination.
            inInstructions->instructionSet[cjumpPosition].fixed8 = endIfSize - cjumpPosition;
            
            //Generate the bytecode for the else statement.
            elseBody->FLVMCodeGen(inInstructions);

            //Adjust the unconditional jump destination.
            inInstructions->instructionSet[endIfSize].fixed8 = inInstructions->instructionCount;
        }

    }



//for
    ForLoop::ForLoop(){
        //Do nothing
    };

    void ForLoop::ToString(std::string inLeft, std::string inRight){
        std::cout << inLeft << "\x1b[34mfor\x1b[0m(";
        if(assign != nullptr){
            assign->ToString("", "");
            std::cout << "; ";
        } else {
            std::cout << ";";
        }

        if(condition != nullptr){
            condition->ToString("", "");
            std::cout << "; ";
        } else {
            std::cout << ";";
        }

        if(incrementer != nullptr){
            incrementer->ToString("", "");
        }
        std::cout << "){\n";
        body->ToString("  " + inLeft, inRight);
        std::cout << "\n" << inLeft << "}\n";
    }

    void ForLoop::FLVMCodeGen(Instructions* inInstructions){
        types result;
        //The initial value of a for loop.
        if(assign != nullptr){
            assign->FLVMCodeGen(inInstructions);
        }

        //Start the scope, because we don't want to continually assign/initialize.
        result.operation[0] = Operation::NewScope;
        inInstructions->push(result);
        //Determine which scope this is for.
        result.fixed8 = body->whichScope;
        inInstructions->push(result);
        //Get the variable slot size.
        result.fixed8 = body->variableSlotSize;
        inInstructions->push(result);

        //This will be where the end of the loop will unconditionally jump to.
        int64_t jumpTo = inInstructions->instructionCount;
        result.fixed8 = jumpTo;
        inInstructions->push(result);
        
        //The condition for a for loop to terminate.
        //If none is provided, then it will perpetually loop.
        if(condition != nullptr){
            condition->FLVMCodeGen(inInstructions);
        } else {
            result.boolean[0] = false;
            inInstructions->push(result);
        }

        //Pushing the conditional jump that can break the loop.
        int64_t cjumpPosition = inInstructions->instructionCount;
        result.fixed8 = cjumpPosition;
        inInstructions->push(result);

        //Add the body of instructions.
        if(body != nullptr){
            body->body->FLVMCodeGen(inInstructions);
        }

        //Add another line of code for changes at the end of the loop.
        if(incrementer != nullptr){
            incrementer->FLVMCodeGen(inInstructions);
        }

        //Place the unconditional jump instruction at the end.
        result.fixed8 = inInstructions->instructionCount;
        inInstructions->push(result);

        //Adjust the position of the conditional jump to be outside of the loop.
        inInstructions->instructionSet[cjumpPosition].fixed8 = inInstructions->instructionCount - cjumpPosition;

        //End the existing scope.
        result.operation[0] = Operation::DeleteScope;
        inInstructions->push(result);
        //Determine which scope this is for.
        result.fixed8 = body->whichScope;
        inInstructions->push(result);
    }



//while
    WhileLoop::WhileLoop(){
        //Do nothing
    };

    void WhileLoop::ToString(std::string inLeft, std::string inRight){
        std::cout << inLeft << "while(";
        if(condition != nullptr){
            condition->ToString(inLeft, inRight);
        }
        std::cout << "){\n";
        if(body != nullptr){
            body->ToString("  " + inLeft, inRight);
        }
        std::cout << "\n" << inLeft << "}\n";
    }

    void WhileLoop::FLVMCodeGen(Instructions* inInstructions){
        types result;
        //Generate the push instruction.
        result.operation[0] = Operation::Push;
        inInstructions->push(result);
        //Put the primitive next to it.
        result.fixed8 = body->whichScope;
        inInstructions->push(result);

        //Start of the scope, granted while loops don't really need scopes.
        //This is just for consistency and ease of use.
        //It just makes work easier for me.
        result.operation[0] = Operation::NewScope;
        inInstructions->push(result);
        //Don't forget which scope.
        result.fixed8 = body->whichScope;
        inInstructions->push(result);
        //Add the variable slot size.
        result.fixed8 = body->variableSlotSize;
        inInstructions->push(result);

        //Where the unconditional jump will always land.
        size_t start = inInstructions->instructionCount;
        //Generate the code for the condition, if any.
        if(condition != nullptr){
            condition->FLVMCodeGen(inInstructions);
        }
        //The location of the conditional jump statement.
        size_t here = inInstructions->instructionCount;    
        //Lay down the conditional jump.
        result.operation[0] = Operation::CJump;
        inInstructions->push(result);
        //Generate the code for the body.
        body->body->FLVMCodeGen(inInstructions);
        //Place an unconditional jump to restart the loop.
        result.operation[0] = Operation::Jump;
        inInstructions->push(result);
        //The location to jump to.
        result.fixed8 = start;
        inInstructions->push(result);
        //Adjust where the unconditional jump will land.
        inInstructions->instructionSet[here].fixed8 = inInstructions->instructionCount - here;

        //End the scope.
        result.operation[0] = Operation::DeleteScope;
        inInstructions->push(result);
        //Which scope is being deleted.
        result.fixed8 = body->whichScope;
        inInstructions->push(result);
    }



//Function
    Function::Function(){
        //Do nothing
    };

    void Function::ToString(std::string inLeft, std::string inRight){
        Initialize* currInit = code->allInitializations;

        std::cout << inLeft + "\x1b[36m" + typeString(type) + "\x1b[35m " + std::string(name) + "\x1b[0m(";
        
        //Combine all the variables, if any.
        if(code->allInitializations != nullptr){
            //Stop right before the last variable to not append an extra comma.
            for(int i = 1; i < argumentCount; i++){
                std::cout << "\x1b[36m" + typeString(currInit->thisVariable->thisToken.type) << "\x1b[0m " << currInit->thisVariable->thisToken.getName() << ", ";
                currInit = currInit->next;
            }
            //Append the last variable without an extra comma.
            std::cout << "\x1b[36m" << typeString(currInit->thisVariable->thisToken.type) << "\x1b[0m " << currInit->thisVariable->thisToken.getName();
        }
        //Return the function printed in the only correct format.
        std::cout << inLeft << "){\n";
        code->ToString("  " + inLeft, ";");
        std::cout << "\n" << inLeft + "}\n";
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
            position = inInstructions->currentInstruction;
            //If there's any code, then genereate the instructions for it.
            if(code != nullptr){
                code->body->FLVMCodeGen(inInstructions);
            }
            //If there are more functions, then define them too.
            if(allFunctions != nullptr){
                allFunctions->FLVMCodeGen(inInstructions);
            }
        }
    }



//Function calls
    FunctionCall::FunctionCall(){
        //Do nothing
    }

    void FunctionCall::ToString(std::string inLeft, std::string inRight){
        std::cout << "\x1b[35m" + std::string(function->name) + "\x1b[0m(";
        arguments->ToString(inLeft, "");
        std::cout << ")";
    }

    void FunctionCall::FLVMCodeGen(Instructions* inInstructions){
        types result;
        //Create a new scope
        result.operation[0] = Operation::NewScope;
        inInstructions->push(result);
        //Push which scope this is.
        result.fixed8 = function->code->whichScope;
        inInstructions->push(result);
        //Push how many slots are needed by this scope.
        result.fixed8 = function->code->variableSlotSize;
        inInstructions->push(result);

        //Generate the arguments, if any.
        if(arguments != nullptr){
            arguments->FLVMCodeGen(inInstructions);
        }

        //Push the call operation.
        result.operation[0] = Operation::ICall;
        inInstructions->push(result);
        //This is where in the instruction set the function exists.
        result.fixed8 = function->position;
        inInstructions->push(result);
    }



//Arguments
    Arguments::Arguments(){
        //Do nothing lmfao
        //The default settings are already in place.
    }

    void Arguments::ToString(std::string inLeft, std::string inRight){
        if(next != nullptr){
            current->ToString(inLeft, "");
            std::cout << ", ";
            next->ToString(inLeft, "");
        } else {
            return current->ToString(inLeft, "");
        }
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

    void ReturnClass::ToString(std::string inLeft, std::string inRight){
        std::cout << inLeft << "\x1b[34mreturn\x1b[0m ";
        statement->ToString(inLeft, inRight);
        std::cout << inRight;
    }

    void ReturnClass::FLVMCodeGen(Instructions* inInstructions){
        types result;
        //There will be code with the return statement.
        statement->FLVMCodeGen(inInstructions);
        //Generate the return instruction and how many scopes to exit.
        result.operation[0] = Operation::IReturn;
        //This determines how many scopes to exit.
        result.fixed8 = returnCount;
        inInstructions->push(result);
    }



//Objects
    ObjectClass::ObjectClass(){
        type = FloridaType::Object;
    }

    void ObjectClass::ToString(std::string inLeft, std::string inRight){
        std::cout << "object " + std::string(name) + "{\n";
        if(code != nullptr){
            code->body->ToString("  " + inLeft, inRight);
        }
        std::cout << inLeft << "\n}\n";
    }

    void ObjectClass::FLVMCodeGen(Instructions* inInstructions){
        //TO DO
    }



//MemberAccess
    MemberAccess::MemberAccess(){
        //Do nothing
    }

    void MemberAccess::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << ".";
        right->ToString(inLeft, inRight);
    }

    void MemberAccess::FLVMCodeGen(Instructions* inInstructions){
        types result;
        int64_t offset = 0;
        MemberAccess* current = this;
        Variable* last = nullptr;

        //Determine how far into the stack the access chain goes.
        while(true){
            offset += current->left->stackBytePosition;

            //Move to the next one, if possible.
            current = dynamic_cast<MemberAccess*>(current->right);
            if(current != nullptr){
                continue;
            }

            //If a variable is found, then a break is gauaranteed.
            last = dynamic_cast<Variable*>(right);
            if(last != nullptr){
                offset += last->stackBytePosition;
            }

            break;
        }

        //Push the push operation to the stack.
        result.operation[0] = Operation::Push;
        inInstructions->push(result);
        //Push the offset size to the stack.
        result.fixed8 = offset;
        inInstructions->push(result);
    }



//Dereference
    Dereference::Dereference(){
        //Do nothing
    }

    void Dereference::ToString(std::string inLeft, std::string inRight){
        left->ToString(inLeft, inRight);
        std::cout << "->";
        right->ToString(inLeft, inRight);
    }

    void Dereference::FLVMCodeGen(Instructions* inInstructions){
        //Do nothing for now
    }
