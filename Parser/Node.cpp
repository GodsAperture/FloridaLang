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

/** @brief This allows me to find where in memory the pointer is supposed to exist given some `head`.
 *  @param left T
 *  @param right U
 *  @return T
 */
template<typename T, typename U>
inline T* addOffset(T* left, U* right){
    return (T*) ((int64_t) left + (char*) right);
}

/** @brief This allows me to create an offset for a pointer given some `head`.
 *  @param left T
 *  @param right U
 *  @return T
 */
template<typename T, typename U>
inline T* difference(T* left, U* right){
    return (T*) ((char*) left - (char*) right);
}


//TypecastClass
    TypecastClass::TypecastClass(){
        //This is just a placeholder. It will be changed later.
        type = FloridaType::Typecast;
    }

    void TypecastClass::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(body, head)->ToString(inLeft, inRight, head);
    }

    void TypecastClass::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(body, head)->FLVMCodeGen(inInstructions, head);   
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
    void Scope::push(Initialize* input, void* head){
        //Adjust `input` for simplicity.
        int64_t byteSize = allocationSize(addOffset(addOffset(input, head)->thisVariable, head)->type);
        int64_t theirSize = 0;
        //If there are no initializations, then just slap the variable onto the list.
        if(allInitializations == nullptr){
            allInitializations = input;
            sortedInitializations = input;
            return;
        }
        Initialize* currentInitialize = addOffset(allInitializations, head);
        Initialize* previousInitialize = nullptr;
        //Reach the tail end of the "linked list" of initializations.
        while(currentInitialize->next != nullptr){
            currentInitialize = addOffset(currentInitialize->next, head);
        }

        //Append the initialization to the tail end of the "linked list."
        currentInitialize->next = input;

        //Reach the end of the sorted list.
        currentInitialize = addOffset(sortedInitializations, head);
        while(currentInitialize->memoryOrder != nullptr){
            currentInitialize = addOffset(currentInitialize->memoryOrder, head);
        }
        //If the input is some object, then place it
        //with the other variables whose memory sizes
        //are multiples of the architecture's size (x32, x64, x128, etc.).
        if(addOffset(addOffset(input, head)->thisVariable, head)->objectType != nullptr){
            while(currentInitialize->thisVariable->objectType != nullptr){
                currentInitialize = addOffset(currentInitialize->memoryOrder, head);
            }

            input->memoryOrder = currentInitialize->memoryOrder;
            currentInitialize->memoryOrder = input;
            return;
        }

        currentInitialize = sortedInitializations;
        //This will place input in a sorted place based on size in the linked list.
        while(currentInitialize != nullptr){
            theirSize = allocationSize(addOffset(addOffset(currentInitialize, head)->thisVariable, head)->type);
            if(byteSize <= theirSize){
                previousInitialize = currentInitialize;
                currentInitialize = addOffset(currentInitialize, head)->memoryOrder;
                continue;
            } else {
                break;
            }
        }

        if(previousInitialize == nullptr){
            addOffset(input, head)->memoryOrder = sortedInitializations;
            sortedInitializations = input;
        } else {
            addOffset(previousInitialize, head)->memoryOrder = input;
            addOffset(input, head)->memoryOrder = currentInitialize;
        }
    }

    void Scope::push(Function* input, void* head){
        //If there are no functions, then just slap the function onto the list.
        if(functions == nullptr){
            functions = addOffset(input, head);
            return;
        }
        Function* currFun = functions;
        //Reach the tail end of the "linked list" of functions.
        while(addOffset(currFun, head)->next != nullptr){
            currFun = addOffset(currFun, head)->next;
        }

        //Append the function to the tail end of the "linked list."
        addOffset(currFun, head)->next = input;

    }

    void Scope::push(ObjectClass* input, void* head){
        //If there are no objects, then this is the first defined class in the scope.
        if(allObjects == nullptr){
            allObjects = input;
            return;
        }
        ObjectClass* currObj = allObjects;
        //Find the last element of the linked list.
        while(addOffset(currObj, head)->next != nullptr){
            currObj = addOffset(currObj, head)->next;
        }

        addOffset(currObj, head)->next = input;
    }

    void Scope::byteAssign(void* head){
        if(allInitializations == nullptr){
            return;
        }
        Initialize* currentInitialization = sortedInitializations;
        int64_t byteSize;
        const int64_t bitmask = 7;
        while(currentInitialization != nullptr){
            //Determine the size of the variable's allocation.
            if(addOffset(addOffset(currentInitialization, head)->thisVariable, head)->objectType == nullptr){
                byteSize = allocationSize(addOffset(addOffset(currentInitialization, head)->thisVariable, head)->type);
            } else {
                byteSize = addOffset(addOffset(addOffset(currentInitialization, head)->thisVariable, head)->objectType, head)->memorySize;
            }
            //Assign the current size to the variable's position in memory.
            addOffset(addOffset(currentInitialization, head)->thisVariable, head)->stackBytePosition = variableSlotSize;
            //Increase the size of the stack for the next variable's placement.
            variableSlotSize += byteSize;
            currentInitialization = addOffset(currentInitialization, head)->memoryOrder;
        }
        //In case the memory used isn't an exact multiple of 8, pad it.
        variableSlotSize += bitmask & (8 - (bitmask & variableSlotSize));
    }


    
    void Scope::ToString(std::string inLeft, std::string inRight, void* head){
        if(body != nullptr){
            addOffset(body, head)->ToString(inLeft, ";", head);
        }
    }

    void Scope::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;

        //The new scope will use the last value to pick the proper scope and adjust its value.
        result.operation[0] = Operation::newScope;
        inInstructions->push(result);
        //Be sure to addOffset this otherwise I can never adjust the unique scopes.
        result.fixed8 = whichScope;
        inInstructions->push(result);
        //This is so I can preemptively allocate slots for the scope's variables.
        result.fixed8 = variableSlotSize;
        inInstructions->push(result);
        if(body != nullptr){
            addOffset(body, head)->FLVMCodeGen(inInstructions, head);
        }
        //Push the delete scope operation onto the stack.
        result.operation[0] = Operation::deleteScope;
        inInstructions->push(result);
        //Add which here to be able to adjust the unique Scope here too.
        result.fixed8 = whichScope;
        inInstructions->push(result);
    }



    bool Scope::hasVariable(std::string_view input, void* head){
        //Go through all scopes as needed.
        Scope* currentScope = difference(this, head);
        Initialize* currentInitialize = nullptr;
        while(currentScope != nullptr){
            currentInitialize = addOffset(currentScope, head)->allInitializations;
            //Check all of the initializations in the scope.
            while(currentInitialize != nullptr){
                if(addOffset(addOffset(currentInitialize, head)->thisVariable, head)->thisToken.name == input){
                    return true;
                } else {
                    currentInitialize = addOffset(currentInitialize, head)->next;
                }
            }
            currentScope = addOffset(currentScope, head)->parent;
        }

        return false;
    }

    bool Scope::hasObject(std::string_view input, void* head){
        //Go through all scopes as needed.
        Scope* currentScope = difference(this, head);
        ObjectClass* currentObject = nullptr;
        while(currentScope != nullptr){
            currentObject = addOffset(currentScope, head)->allObjects;
            //Check all of the objects in the scope.
            while(currentObject != nullptr){
                if(addOffset(currentObject, head)->name == input){
                    return true;
                } else {
                    currentObject = addOffset(currentObject, head)->next;
                }
            }
            currentScope = addOffset(currentScope, head)->parent;
        }

        return false;
    }

    bool Scope::hasFunction(std::string_view input, void* head){
        //Go through all scopes as needed.
        Scope* currentScope = difference(this, head);
        Function* currentFunction = nullptr;
        while(currentScope != nullptr){
            currentFunction = addOffset(currentScope, head)->functions;
            //Check all of the initializations in the scope.
            while(currentFunction != nullptr){
                if(addOffset(currentFunction, head)->name == input){
                    return true;
                } else {
                    currentFunction = addOffset(currentFunction, head)->next;
                }
            }
            currentScope = addOffset(currentScope, head)->parent;
        }

        return false;
    }

    Variable* Scope::getVariable(std::string_view input, void* head){
        //Go through all scopes as needed.
        Scope* currentScope = difference(this, head);
        Initialize* currentInitialize = nullptr;
        while(currentScope != nullptr){
            currentInitialize = addOffset(currentScope, head)->allInitializations;
            //Check all of the initializations in the scope.
            while(currentInitialize != nullptr){
                if(addOffset(addOffset(currentInitialize, head)->thisVariable, head)->thisToken.name == input){
                    return addOffset(currentInitialize, head)->thisVariable;
                } else {
                    currentInitialize = addOffset(currentInitialize, head)->next;
                }
            }
            currentScope = addOffset(currentScope, head)->parent;
        }

        return nullptr;
    }

    ObjectClass* Scope::getObject(std::string_view input, void* head){
        //Go through all scopes as needed.
        Scope* currentScope = difference(this, head);
        ObjectClass* currentObject = nullptr;
        while(currentScope != head){
            currentObject = addOffset(currentScope, head)->allObjects;
            //Check all of the objects in the scope.
            while(currentObject != nullptr){
                if(addOffset(currentObject, head)->name == input){
                    return currentObject;
                } else {
                    currentObject = addOffset(currentObject, head)->next;
                }
            }
            currentScope = addOffset(currentScope, head)->parent;
        }

        return nullptr;
    }

    Function* Scope::getFunction(std::string_view input, void* head){
        //Go through all scopes as needed.
        Scope* currentScope = this;
        Function* currentFunction = nullptr;
        while(currentScope != nullptr){
            currentFunction = addOffset(currentScope, head)->functions;
            //Check all of the initializations in the scope.
            while(currentFunction != nullptr){
                if(addOffset(currentFunction, head)->name == input){
                    return currentFunction;
                } else {
                    currentFunction = addOffset(currentFunction, head)->next;
                }
            }
            currentScope = addOffset(currentScope, head)->parent;
        }

        return nullptr;
    }



//Primitive
    Primitive::Primitive(){
        value.object = nullptr;
        //The type will be determined after a Primitive is created.
    }

    void Primitive::ToString(std::string inLeft, std::string inRight, void* head){
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

    void Primitive::FLVMCodeGen(Instructions* inInstructions, void* head){
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
    Body* Body::append(Body* input, void* head){
        if(current == nullptr){
            current = input;
        } else {
            if(next != nullptr){
                addOffset(next, head)->append(input, head);
            } else {
                next = input;
            }
        }

        return this;

    }

    void Body::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(current, head)->ToString(inLeft, inRight, head);
        if(next != nullptr){
            std::cout << '\n';
            addOffset(next, head)->ToString(inLeft, inRight, head);
        }
    }

    void Body::FLVMCodeGen(Instructions* inInstructions, void* head){
        //Add the current chunk of code if it is not a function.
        if(dynamic_cast<Function*>(current) == nullptr){
            addOffset(current, head)->FLVMCodeGen(inInstructions, head);
            //After the line has finished execution, pop it from the stack.
            //inInstructions.push_back(Instruction(Operation::pop));
        }
        //If next isn't a nullptr, then generate code for it too.
        if(next != nullptr){
            addOffset(next, head)->FLVMCodeGen(inInstructions, head);
        }
    }



//Variable
    Variable::Variable(){
        //Do nothing. It's not a problem.
    }

    void Variable::append(Variable* input, void* head){
        Variable* currVar = this;
        if(addOffset(currVar, head)->next != nullptr){
            while(addOffset(currVar, head)->next != nullptr){
                currVar = currVar->next;
            }
        }

        currVar->next = input;

    }

    void Variable::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << thisToken.getName();
    }

    void Variable::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        //Check if the variable is an object.
        //If it is, fetch everything.
        if(objectType != nullptr){
            //This will fetch the full object.
            for(uint64_t i = 0; i < addOffset(objectType, head)->memorySize / 8; i++){
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

    void Initialize::ToString(std::string inLeft, std::string inRight, void* head){
        if(addOffset(thisVariable, head)->objectType != nullptr){
            if(code != nullptr){
                std::cout <<  inLeft << std::string(addOffset(addOffset(thisVariable, head)->objectType, head)->name);
                std::cout << " " << addOffset(thisVariable, head)->thisToken.getName() << " = ";
                addOffset(code, head)->ToString(inLeft, inRight, head);
                std::cout << inRight;
                return;
            } else {
                std::cout << inLeft << std::string(addOffset(addOffset(thisVariable, head)->objectType, head)->name) << " " << addOffset(thisVariable, head)->thisToken.getName() << inRight;
            }
        } else {
            if(code != nullptr){
                std::cout << inLeft << typeString(addOffset(thisVariable, head)->thisToken.type) << " " << addOffset(thisVariable, head)->thisToken.getName() << " = ";
                addOffset(code, head)->ToString(inLeft, inRight, head);
                std::cout << inRight;
            } else {
                std::cout << inLeft << typeString(addOffset(thisVariable, head)->thisToken.type) << " " << addOffset(thisVariable, head)->thisToken.getName() << inRight;
            }
        }
    }

    void Initialize::FLVMCodeGen(Instructions* inInstructions, void* head){
        //If there's code, then generate it.
        if(code != nullptr){
            //Generate the code for the right hand side.
            addOffset(code, head)->FLVMCodeGen(inInstructions, head);

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
        while(addOffset(current, head)->next != nullptr){
            current = addOffset(current, head)->next;
        }
        addOffset(current, head)->next = input;
    }

    //Append `input` to the end of the `memoryOrder` linked list.
    void Initialize::memoryAppend(Initialize* input, void* head){
        Initialize* current = this;
        while(addOffset(current, head)->memoryOrder != nullptr){
            current = addOffset(current, head)->memoryOrder;
        }
        addOffset(current, head)->memoryOrder = input;
    }



//Assignment =
    Assignment::Assignment(){
        //Do nothing
    };

    void Assignment::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << inLeft;
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " = ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
        std::cout << inRight;
    }

    void Assignment::FLVMCodeGen(Instructions* inInstructions, void* head){
        //Evaluate the right hand side before assigning.
        if(right != nullptr){
            addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        }
        //Generate the fetch instructions for this object.
        //These will be changed to assignment instructions.
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);

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
    Add::Add(){
        //Do nothing
    }

    void Add::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " + ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Add::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push the operation
        result.operation[0] = Operation::add;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }



//Subtract -
    Subtract::Subtract(){
        //Do nothing
    }

    void Subtract::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " - ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Subtract::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;

        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push the operation
        result.operation[0] = Operation::subtract;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }



//Quiet multiply does not have a character
    QuietMultiply::QuietMultiply(){
        //Do nothing
    }

    void QuietMultiply::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void QuietMultiply::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;

        //Generate the code for the left hand object.
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        //Generate the code for the right hand object.
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push the operation
        result.operation[0] = Operation::multiply;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }


//Multiply *
    Multiply::Multiply(){
        //Do nothing
    }

    void Multiply::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " * ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Multiply::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        
        //Generate the code for the left hand object.
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        //Generate the code for the right hand object.
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push the operation
        result.operation[0] = Operation::multiply;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }



//Divide /
    Divide::Divide(){
        //Do nothing
    };

    void Divide::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " / ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Divide::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;

        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push the operation
        result.operation[0] = Operation::subtract;
        inInstructions->push(result);
        //Push the data type
        result.type[0] = left->type;
        inInstructions->push(result);
    }



//Parentheses ()
    Parentheses::Parentheses(){
        //Do nothing
    }

    void Parentheses::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << "(";
        addOffset(subexpression, head)->ToString(inLeft, inRight, head);
        std::cout << ")";
    }

    void Parentheses::FLVMCodeGen(Instructions* inInstructions, void* head){
        addOffset(subexpression, head)->FLVMCodeGen(inInstructions, head);
    }



//Negative -
    Negative::Negative(){
        //Do nothing;
    }

    void Negative::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << "-";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Negative::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;

        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for -
        result.operation[0] = Operation::negate;
        inInstructions->push(result);
    }



//Equal ==
    Equal::Equal(){
        //Do nothing
    }

    void Equal::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " == ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Equal::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for ==
        result.operation[0] = Operation::equals;
        inInstructions->push(result);
    }



//Not equal !=
    NotEqual::NotEqual(){
        //Do nothing
    }

    void NotEqual::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " != ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void NotEqual::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        left->FLVMCodeGen(inInstructions, head);
        right->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for !=
        result.operation[0] = Operation::nequals;
        inInstructions->push(result);
    }



//Greater than >
    GreaterThan::GreaterThan(){
        //Do nothing
    }

    void GreaterThan::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " > ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void GreaterThan::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for >
        result.operation[0] = Operation::greater;
        inInstructions->push(result);
    }



//Greater than or equal to >=
    GreaterThanOr::GreaterThanOr(){
        //Do nothing
    }

    void GreaterThanOr::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " >= ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void GreaterThanOr::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;   
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for >=
        result.operation[0] = Operation::greateror;
        inInstructions->push(result);
    }



//Less than <
    LessThan::LessThan(){
        //Do nothing
    }

    void LessThan::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " < ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void LessThan::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for <
        result.operation[0] = Operation::lesser;
        inInstructions->push(result);
    }



//Less than or equal to <=
    LessThanOr::LessThanOr(){
        //Do nothing
    }

    void LessThanOr::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " <= ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void LessThanOr::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        left->FLVMCodeGen(inInstructions, head);
        right->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for <=
        result.operation[0] = Operation::lesseror;
        inInstructions->push(result);
    }



//Or OR
    Or::Or(){
        //Do nothing
    };

    void Or::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " OR ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Or::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for OR
        result.operation[0] = Operation::ior;
        inInstructions->push(result);
    }



//And AND
    And::And(){
        //Do nothing
    }

    void And::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << " AND ";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void And::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(left, head)->FLVMCodeGen(inInstructions, head);
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the instruction for AND
        result.operation[0] = Operation::iand;
        inInstructions->push(result);
    }



//Not !
    Not::Not(){
        //Do nothing
    }

    void Not::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << "!";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void Not::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        addOffset(right, head)->FLVMCodeGen(inInstructions, head);
        //Push back the operation for !
        result.operation[0] = Operation::inot;
        inInstructions->push(result);
    }



//if
    IfClass::IfClass(){
        //Do nothing
    };

    void IfClass::ToString(std::string inLeft, std::string inRight, void* head){
        if(elseBody == nullptr){
            std::cout << inLeft << "\x1b[36mif\x1b[0m(";
            addOffset(condition, head)->ToString(inLeft, inRight, head);
            std::cout << "){\n";
                addOffset(ifBody, head)->ToString("  " + inLeft, ";", head);
            std::cout << "\n" << inLeft << "}\n";
        } else {
            std::cout << inLeft + "\x1b[36mif\x1b[0m(";
            addOffset(condition, head)->ToString(inLeft, ";", head);
            std::cout << "){\n";
                addOffset(ifBody, head)->ToString("  " + inLeft, inRight, head);
            std::cout << "\n" << inLeft << "} else {\n";
                addOffset(elseBody, head)->ToString("  " + inLeft, ";", head);
            std::cout << "\n" << inLeft << "}\n";
        }
    }

    void IfClass::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        //If there is not an else statement, generate code for just an if statement.
        //otherwise, generate both bodies and cjumps as needed.
        if(elseBody == nullptr){
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions, head);

            //Mark where the conditional jump instruction will be.
            int64_t cjumpPosition = inInstructions->instructionCount;

            //Push a cjump instruction. This will be edited later.
            result.fixed8 = cjumpPosition;
            inInstructions->push(result);
            addOffset(ifBody, head)->FLVMCodeGen(inInstructions, head);

            //Adjust the conditional jump destination.
            inInstructions->instructionSet[cjumpPosition].fixed8 = inInstructions->instructionCount - cjumpPosition;
        } else {
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions, head);

            //Mark where the conditional jump instruction will be.
            int64_t cjumpPosition = inInstructions->instructionCount;

            //Push a cjump instruction. This will be edited later.
            result.fixed8 = cjumpPosition;
            inInstructions->push(result);
            addOffset(ifBody, head)->FLVMCodeGen(inInstructions, head);

            //Push an unconditional jump to skip over the else statement.
            //This will only be reached if the first branch is taken.
            result.operation[0] = Operation::jump;
            inInstructions->push(result);

            //Get the size of the instructions set after the if body is made.
            int64_t endIfSize = inInstructions->instructionCount;

            //Adjust the conditional jump destination.
            inInstructions->instructionSet[cjumpPosition].fixed8 = endIfSize - cjumpPosition;
            
            //Generate the bytecode for the else statement.
            elseBody->FLVMCodeGen(inInstructions, head);

            //Adjust the unconditional jump destination.
            inInstructions->instructionSet[endIfSize].fixed8 = inInstructions->instructionCount;
        }

    }



//for
    ForLoop::ForLoop(){
        //Do nothing
    };

    void ForLoop::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << inLeft << "\x1b[34mfor\x1b[0m(";
        if(assign != nullptr){
            addOffset(assign, head)->ToString("", "", head);
            std::cout << "; ";
        } else {
            std::cout << ";";
        }

        if(condition != nullptr){
            addOffset(condition, head)->ToString("", "", head);
            std::cout << "; ";
        } else {
            std::cout << ";";
        }

        if(incrementer != nullptr){
            addOffset(incrementer, head)->ToString("", "", head);
        }
        std::cout << "){\n";
        addOffset(body, head)->ToString("  " + inLeft, inRight, head);
        std::cout << "\n" << inLeft << "}\n";
    }

    void ForLoop::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        //The initial value of a for loop.
        if(assign != nullptr){
            addOffset(assign, head)->FLVMCodeGen(inInstructions, head);
        }

        //Start the scope, because we don't want to continually assign/initialize.
        result.operation[0] = Operation::newScope;
        inInstructions->push(result);
        //Determine which scope this is for.
        result.fixed8 = ((Scope*) addOffset(body, head))->whichScope;
        inInstructions->push(result);
        //Get the variable slot size.
        result.fixed8 = ((Scope*) addOffset(body, head))->variableSlotSize;
        inInstructions->push(result);

        //This will be where the end of the loop will unconditionally jump to.
        int64_t jumpTo = inInstructions->instructionCount;
        result.fixed8 = jumpTo;
        inInstructions->push(result);
        
        //The condition for a for loop to terminate.
        //If none is provided, then it will perpetually loop.
        if(condition != nullptr){
            addOffset(condition, head)->FLVMCodeGen(inInstructions, head);
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
            addOffset(((Scope*) addOffset(body, head))->body, head)->FLVMCodeGen(inInstructions, head);
        }

        //Add another line of code for changes at the end of the loop.
        if(incrementer != nullptr){
            addOffset(incrementer, head)->FLVMCodeGen(inInstructions, head);
        }

        //Place the unconditional jump instruction at the end.
        result.fixed8 = inInstructions->instructionCount;
        inInstructions->push(result);

        //Adjust the position of the conditional jump to be outside of the loop.
        inInstructions->instructionSet[cjumpPosition].fixed8 = inInstructions->instructionCount - cjumpPosition;

        //End the existing scope.
        result.operation[0] = Operation::deleteScope;
        inInstructions->push(result);
        //Determine which scope this is for.
        result.fixed8 = ((Scope*) addOffset(body, head))->whichScope;
        inInstructions->push(result);
    }



//while
    WhileLoop::WhileLoop(){
        //Do nothing
    };

    void WhileLoop::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << inLeft << "while(";
        if(condition != nullptr){
            addOffset(condition, head)->ToString(inLeft, inRight, head);
        }
        std::cout << "){\n";
        if(body != nullptr){
            addOffset(body, head)->ToString("  " + inLeft, inRight, head);
        }
        std::cout << "\n" << inLeft << "}\n";
    }

    void WhileLoop::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        //Generate the push instruction.
        result.operation[0] = Operation::push;
        inInstructions->push(result);
        //Put the primitive next to it.
        result.fixed8 = body->whichScope;
        inInstructions->push(result);

        //Start of the scope, granted while loops don't really need scopes.
        //This is just for consistency and ease of use.
        //It just makes work easier for me.
        result.operation[0] = Operation::newScope;
        inInstructions->push(result);
        //Don't forget which scope.
        result.fixed8 = ((Scope*) addOffset(body, head))->whichScope;
        inInstructions->push(result);
        //Add the variable slot size.
        result.fixed8 = body->variableSlotSize;
        inInstructions->push(result);

        //Where the unconditional jump will always land.
        size_t start = inInstructions->instructionCount;
        //Generate the code for the condition, if any.
        if(condition != nullptr){
            addOffset(condition, head)->FLVMCodeGen(inInstructions, head);
        }
        //The location of the conditional jump statement.
        size_t here = inInstructions->instructionCount;    
        //Lay down the conditional jump.
        result.operation[0] = Operation::cjump;
        inInstructions->push(result);
        //Generate the code for the body.
        addOffset(((Scope*) addOffset(body, head))->body, head)->FLVMCodeGen(inInstructions, head);
        //Place an unconditional jump to restart the loop.
        result.operation[0] = Operation::jump;
        inInstructions->push(result);
        //The location to jump to.
        result.fixed8 = start;
        inInstructions->push(result);
        //Adjust where the unconditional jump will land.
        inInstructions->instructionSet[here].fixed8 = inInstructions->instructionCount - here;

        //End the scope.
        result.operation[0] = Operation::deleteScope;
        inInstructions->push(result);
        //Which scope is being deleted.
        result.fixed8 = ((Scope*) addOffset(body, head))->whichScope;
        inInstructions->push(result);
    }



//Function
    Function::Function(){
        //Do nothing
    };

    void Function::ToString(std::string inLeft, std::string inRight, void* head){
        Initialize* currInit = addOffset(code, head)->allInitializations;

        std::cout << inLeft + "\x1b[36m" + typeString(type) + "\x1b[35m " + std::string(name) + "\x1b[0m(";
        
        //Combine all the variables, if any.
        if(addOffset(code, head)->allInitializations != nullptr){
            //Stop right before the last variable to not append an extra comma.
            for(int i = 1; i < argumentCount; i++){
                std::cout << "\x1b[36m" + typeString(addOffset(addOffset(currInit, head)->thisVariable, head)->thisToken.type) << "\x1b[0m " << addOffset(addOffset(currInit, head)->thisVariable, head)->thisToken.getName() << ", ";
                currInit = currInit->next;
            }
            //Append the last variable without an extra comma.
            std::cout << "\x1b[36m" << typeString(addOffset(addOffset(currInit, head)->thisVariable, head)->thisToken.type) << "\x1b[0m " << addOffset(addOffset(currInit, head)->thisVariable, head)->thisToken.getName();
        }
        //Return the function printed in the only correct format.
        std::cout << inLeft << "){\n";
        addOffset(code, head)->ToString("  " + inLeft, ";", head);
        std::cout << "\n" << inLeft + "}\n";
    }

    void Function::append(Initialize* input, void* head){
        Initialize* currInit = addOffset(code, head)->allInitializations;
        if(currInit != nullptr){
            while(addOffset(currInit, head)->next != nullptr){
                currInit = addOffset(currInit, head)->next;
            }
            addOffset(currInit, head)->next = input;
        } else {
            addOffset(code, head)->allInitializations = input;
        }
    }

    void Function::FLVMCodeGen(Instructions* inInstructions, void* head){
        //Change where this function starts in the program.
        if(!alreadyGenerated){
            position = inInstructions->currentInstruction;
            if(code != nullptr){
                addOffset(((Scope*) addOffset(code, head))->body, head)->FLVMCodeGen(inInstructions, head);
            }
            if(allFunctions != nullptr){
                addOffset(allFunctions, head)->FLVMCodeGen(inInstructions, head);
            }
        }
    }



//Function calls
    Call::Call(){
        //Do nothing
    }

    void Call::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << "\x1b[35m" + std::string(function->name) + "\x1b[0m(";
        addOffset(arguments, head)->ToString(inLeft, "", head);
        std::cout << ")";
    }

    void Call::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        //Create a new scope
        result.operation[0] = Operation::newScope;
        inInstructions->push(result);
        //Push which scope this is.
        result.fixed8 = ((Scope*) addOffset(((Function*) addOffset(function, head))->code, head))->whichScope;
        //Push how many slots are needed by this scope.
        result.fixed8 = ((Scope*) addOffset(((Function*) addOffset(function, head))->code, head))->variableSlotSize;
        inInstructions->push(result);

        //Generate the arguments, if any.
        if(arguments != nullptr){
            arguments->FLVMCodeGen(inInstructions, head);
        }

        //Push the call operation.
        result.operation[0] = Operation::call;
        inInstructions->push(result);
        //This is where in the instruction set the function exists.
        result.fixed8 = ((Function*) addOffset(function, head))->position;
        inInstructions->push(result);
    }



//Arguments
    Arguments::Arguments(){
        //Do nothing lmfao
        //The default settings are already in place.
    }

    void Arguments::ToString(std::string inLeft, std::string inRight, void* head){
        if(next != nullptr){
            addOffset(current, head)->ToString(inLeft, "", head);
            std::cout << ", ";
            addOffset(next, head)->ToString(inLeft, "", head);
        } else {
            return addOffset(current, head)->ToString(inLeft, "", head);
        }
    }

    void Arguments::FLVMCodeGen(Instructions* inInstructions, void* head){
        Arguments* currArgs = this;
        while(((Arguments*) addOffset(currArgs, head))->next != nullptr){
            addOffset(((Arguments*) addOffset(currArgs, head))->current, head)->FLVMCodeGen(inInstructions, head);
            currArgs = ((Arguments*) addOffset(currArgs, head))->next;
        }
        addOffset(((Arguments*) addOffset(currArgs, head))->current, head)->FLVMCodeGen(inInstructions, head);
    }



//Return statements
    ReturnClass::ReturnClass(){
        //Do nothing
    }

    void ReturnClass::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << inLeft << "\x1b[34mreturn\x1b[0m ";
        addOffset(statement, head)->ToString(inLeft, inRight, head);
        std::cout << inRight;
    }

    void ReturnClass::FLVMCodeGen(Instructions* inInstructions, void* head){
        types result;
        //There will be code with the return statement.
        addOffset(statement, head)->FLVMCodeGen(inInstructions, head);
        //Generate the return instruction and how many scopes to exit.
        result.operation[0] = Operation::ireturn;
        //This determines how many scopes to exit.
        result.fixed8 = returnCount;
        inInstructions->push(result);
    }



//Objects
    ObjectClass::ObjectClass(){
        type = FloridaType::Object;
    }

    void ObjectClass::ToString(std::string inLeft, std::string inRight, void* head){
        std::cout << "object " + std::string(name) + "{\n";
        if(code != nullptr){
            addOffset(addOffset(code, head)->body, head)->ToString("  " + inLeft, inRight, head);
        }
        std::cout << inLeft << "\n}\n";
    }

    void ObjectClass::FLVMCodeGen(Instructions* inInstructions, void* head){
        //TO DO
    }



//MemberAccess
    MemberAccess::MemberAccess(){
        //Do nothing
    }

    void MemberAccess::ToString(std::string inLeft, std::string inRight, void* head){
        addOffset(left, head)->ToString(inLeft, inRight, head);
        std::cout << ".";
        addOffset(right, head)->ToString(inLeft, inRight, head);
    }

    void MemberAccess::FLVMCodeGen(Instructions* inInstructions, void* head){
        // types result;
        // Node* currentNode = left;

        // //This will create a push instruction or adjust the literal of the instruction.
        // MemberAccess* theAccess = dynamic_cast<MemberAccess*>(currentNode);
        // if(theAccess != nullptr){
        //     theAccess->left->FLVMCodeGen(inInstructions, head);
        //     if(inInstructions.back().oper == Operation::push){
        //         inInstructions.back().literal.fixed8 += theAccess->right->stackBytePosition;
        //     } else {
        //         Instruction thePush;
        //         thePush.oper = Operation::push;
        //         inInstructions.push_back(thePush);
        //     }
        // }

        // //This will create a dereference instruction for the object.
        // Dereference* theDereference = dynamic_cast<Dereference*>(currentNode);
        // if(theDereference != nullptr){
        //     theDereference->left->FLVMCodeGen(inInstructions, head);  
        //     //The instructions for this don't exist yet.
        // }

        // //Reaching a variable means that the chain has ended.
        // Variable* theVariable = dynamic_cast<Variable*>(currentNode);
        // if(theVariable != nullptr){
        //     theVariable->FLVMCodeGen(inInstructions, head);
        // }
    }



//Dereference
    Dereference::Dereference(){
        //Do nothing
    }

    void Dereference::ToString(std::string inLeft, std::string inRight, void* head){
    //     addOffset(left, head)->ToString(inLeft, inRight, head);
    //     std::cout << "->";
    //     addOffset(right, head)->ToString(inLeft, inRight, head);
    // }

    // void Dereference::FLVMCodeGen(Instructions* inInstructions, void* head){
    //     types result;
    //     Node* currentNode = left;

    //     //This will create a push instruction or adjust the literal of the instruction.
    //     MemberAccess* theAccess = dynamic_cast<MemberAccess*>(currentNode);
    //     if(theAccess != nullptr){
    //         theAccess->left->FLVMCodeGen(inInstructions, head);
    //         if(inInstructions.back().oper == Operation::push){
    //             inInstructions.back().literal.fixed8 += theAccess->right->stackBytePosition;
    //         } else {
    //             Instruction thePush;
    //             thePush.oper = Operation::push;
    //             inInstructions.push_back(thePush);
    //         }
    //     }

    //     //This will create a dereference instruction for the object.
    //     Dereference* theDereference = dynamic_cast<Dereference*>(currentNode);
    //     if(theDereference != nullptr){
    //         theDereference->left->FLVMCodeGen(inInstructions, head);  
    //         //The instructions for this don't exist yet.
    //     }

    //     //Reaching a variable means that the chain has ended.
    //     Variable* theVariable = dynamic_cast<Variable*>(currentNode);
    //     if(theVariable != nullptr){
    //         theVariable->FLVMCodeGen(inInstructions, head);
    //     }
    }

    void Dereference::FLVMCodeGen(Instructions* inInstructions, void* head){
        //Do nothing for now
    }
