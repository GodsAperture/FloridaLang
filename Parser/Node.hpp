#ifndef Node_h__
#define Node_h__

#include "Instructions.cpp"
#include "../Lexer/Types.hpp"
#include "StackAllocator.hpp"
#include <string>
#include <vector>
//Forward declarations, idfk *why* though.
//My C/C++ understanding is still hindering me.
class Function;
class ObjectClass;
class Initialize;

std::string typeString(FloridaType input);

FloridaType typeReturn(std::string inString);

bool typeCheck(FloridaType inType);

class StackAllocator;
class ExistingScope;
class Scope;

//If you need to be told what a node is, God help you.
class Node{
public:
    FloridaType type = FloridaType::BadToken;
    Node(){
        //Exists for the sake of the default constructor.    
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(const Node&&) = delete;
    Node& operator=(const Node&&) = delete;

    //Prints the program back to the user in a proper format.
    virtual void ToString(std::string inLeft, std::string inRight, void* head) = 0;
    virtual void FLVMCodeGen(Instructions* inInstructions, void* head) = 0;
    //virtual Node* differentiate(int64_t input) = 0;
    virtual ~Node(){};
};

class Body : public Node{
public:
    Node* current = nullptr;
    Body* next = nullptr;

    Body();
    Body* append(Body* input, void* head);
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class Variable : public Node{
public:
    Token thisToken;//There exists a default Token() constructor.
    Scope* owner = nullptr;
    //`stackOffset` = stackBytePosition >> 3.
    //`byteOffset` = (stackBytePostion % 8) / allocationSize(variable->type).
    //Example: computationStack[`stackIndex`].fixed1[`byteOffset`]
    int64_t stackBytePosition = -1;
    types value;
    Variable* next = nullptr;
    //Applicable only if the variable is not a primitive type.
    ObjectClass* objectType = nullptr;
    
    Variable();
    //I get this comes off as really janky, but whatever, it's convenient.
    void operator=(Variable* input){
        this->thisToken = input->thisToken;
        this->owner = input->owner;
        this->stackBytePosition = input->stackBytePosition;
        this->value = input->value;
        this->next = input->next;
        this->objectType = input->objectType;
    };
    void append(Variable* input, void* head);
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
    //Generates the appropriate assignment instruction. 
    void AssignCodeGen(Instructions* inInstructions, void* head);
};

class Scope : public Node{
    public:
        //This points to the outerscope.
        Scope* parent = nullptr;
        //The corresponding "name" of the scope.
        std::string_view name;
        //This is the body of code within the scope.
        Body* body = nullptr;
        //This will be the initializations of the current scope.
        Initialize* allInitializations = nullptr;
        //This will be the initializations sorted by memory size.
        Initialize* sortedInitializations = nullptr;
        //This will contain all of the class definitions in the scope.
        ObjectClass* allObjects = nullptr;
        //A "linked list" of functions for the current scope.
        Function* functions = nullptr;
        //The counter will determine which index to use in the `uniqueScopes` in the VM.
        int64_t whichScope = 0;
        //This will help with how many slots there are in the stack.
        //This is measured in bytes.
        int64_t variableSlotSize = 0;

        //Find where in some given Scope a particular variable lies.
        //If it does not exist in the scope, it returns -1.
        int64_t whereVariable(std::string input);
        Initialize* getInit(std::string input);
        //Find where in some given Scope a particular variable lies.
        //If it does not exist in the scope, it returns -1.
        Function* funGet(std::string input);
        ObjectClass* objGet(std::string input);
        //Add the Initialization to the two linked lists that sort
        //them by order found and size in bytes.
        void push(Initialize* input, void* head);
        //Push a new function into the scope's function stack.
        void push(Function* input, void* head);
        //Push a new class into the scope's class stack.
        void push(ObjectClass* input, void* head);
        //Assigns each variable a byte value that will be used to
        //determine where in the stack or heap a packed variable is.
        void byteAssign(void* head);
        //Checks if a variable is accessible in the current scope.
        bool hasVariable(std::string_view input, void* head);
        //Checks if an object is accessible in the current scope.
        bool hasObject(std::string_view input, void* head);
        //Checks if a function is accessible in the current scope.
        bool hasFunction(std::string_view input, void* head);
        //Gets the pointer to the variable in question.
        Variable* getVariable(std::string_view input, void* head);
        //Gets the pointer to the object in question.
        ObjectClass* getObject(std::string_view input, void* head);
        //Gets the pointer to the function in question.
        Function* getFunction(std::string_view input, void* head);
        //Determine if the variable in question is global, middle, local, or in the heap.
        char whereVariable(std::string_view input, void* head);
        //Standard methods.
        Scope();
        void ToString(std::string inLeft, std::string inRight, void* head) override;
        void FLVMCodeGen(Instructions* inInstructions, void* head) override;
        
    };

class Initialize : public Node{
public:
    Variable* thisVariable = nullptr;
    //Only useful for function, method, class, constructor, etc. defintions.
    Initialize* next = nullptr;
    //This will be the order the variable is placed in memory to pack it tightly.
    Initialize* memoryOrder = nullptr;
    //If there is an assignment, this will hold it.
    Node* code = nullptr;

    Initialize();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
    //Append the `input` to the end of the linked list `next` of Initializations.
    void append(Initialize* input, void* head);
    //Append `input` to the end of the `memoryOrder` linked list.
    void memoryAppend(Initialize* input, void* head);
};

class Assignment : public Node{
public:
    //This can either be a `MemberAccess*` or a `Variable*`.
    Node* left = nullptr;
    Node* right = nullptr;

    Assignment();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



//Mathematical operations
class Add : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Add();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class Subtract : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Subtract();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



class Multiply : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Multiply();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class Divide : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Divide();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



class Parentheses : public Node{
public:
    Node* subexpression = nullptr;

    Parentheses();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class Negative : public Node{
public:
    Node* right = nullptr;

    Negative();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



//Primitive type
class Primitive : public Node{
public:
    types value;

    Primitive();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



//Type casts
class TypecastClass : public Node{
public:
    Node* body = nullptr;

    TypecastClass();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



//Object comparisons
class Equal : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Equal();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class NotEqual : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    NotEqual();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class GreaterThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThan();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class GreaterThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThanOr();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class LessThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThan();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class LessThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThanOr();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



//Boolean algebra
class Or : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Or();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class And : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    And();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class Not : public Node{
public:
    Node* right = nullptr;

    Not();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



class IfClass : public Node{
public:
    Node* condition = nullptr;
    Scope* ifBody = nullptr;
    Scope* elseBody = nullptr;
    size_t ifVarCount = 0;
    size_t elseVarCount = 0;

    IfClass();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class ForLoop : public Node{
public:
    Node* assign = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;
    Scope* body = nullptr;

    ForLoop();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class WhileLoop : public Node{
public:
    Node* condition = nullptr;
    Scope* body = nullptr;

    WhileLoop();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class Function : public Node{
public:
    std::string_view name;
    bool returnable = false;
    //This is where the instruction set begins.
    int64_t position = -1;
    //This is how many arguments the function has.
    int64_t argumentCount = 0;
    //This is the full function
    Scope* code = nullptr;
    //For use in the parser and VM.
    //This points to the next function in the scope, if any.
    Function* next = nullptr;
    //Previous functions are needed to determine what is the previous scope.
    Function* previous = nullptr;
    //This connects all functions across scopes.
    Function* allFunctions = nullptr;
    //This makes sure code isn't generated twice.
    bool alreadyGenerated = false;

    Function();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
    void append(Initialize* input, void* head);
};

class Arguments : public Node{
public:
    Node* current = nullptr;
    Arguments* next = nullptr;

    Arguments();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
    void append(Arguments* input, Node* head);
};

class Call : public Node{
public:
    Function* function = nullptr;
    Arguments* arguments = nullptr;
    int64_t argumentByteSize = 0;

    Call();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};

class ReturnClass : public Node{
public:
    //There will always be a minimum of 1 scope to exit.
    int64_t returnCount = 1;
    Node* statement = nullptr;

    ReturnClass();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
};



// class Method : public Node{
// public:
//     std::string_view name;
//     //This is where the instruction set begings.
//     int64_t position = -1;
//     //This is how many arguments the method has.
//     //There will always be at least 1 argument.
//     int64_t argumentCount = 1;
//     //This is the full method definition.
//     Scope* code = nullptr;
//     //For use in the parser and VM.
//     //This points to the next method in the scope, if any.
//     Method* next = nullptr;
//     //Previous methods are needed to determine what is the previous scope.
//     Method* previous = nullptr;
//     //This connects all methods across scopes.
//     Method* allMethods = nullptr;
//     //This makes sure code isn't generated twice.
//     bool alreadyGenerated = false;
//
//     Method();
//     void ToString(std::string inLeft, std::string inRight, void* head) override;
//     
//     void FLVMCodeGen(Instructions* inInstructions, void* head) override;
//     void append(Variable* input);
//     
//     Method* pcopy(StackAllocator& input);
// };

class ObjectClass : public Node{
public:
    std::string_view name;
    Scope* code = nullptr;
    ObjectClass* next = nullptr;
    //Memory size is also useful for determining the compressed stack size.
    uint64_t memorySize = 0;
    int64_t variableCount = 0;
    ////TO DO
    void* defaultConstruction = nullptr;
    //Method* allMethods = nullptr;
    ObjectClass* heapPrevious = nullptr;
    ObjectClass* heapNext = nullptr;

    ObjectClass();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
    //Given some variable name, get the offset for the object from this class' stack.
    int64_t whereVariable(std::string input);
    //Given some variable name, get the offset in memory for it.
    //This will be useful for objects in the heap.
    char* variableOffset(std::string input);
};

class MemberAccess : public Node{
public:
    //`left` will only be a `Variable`, `MemberAccess`, or a `Dereference`.
    Node* left = nullptr;
    Variable* right = nullptr;
    ObjectClass* thisObject = nullptr;
    
    MemberAccess();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
    void AssignCodeGen(Instructions* inInstructions, Node* head);
    Variable* getBaseVariable();
};

class Dereference : public Node{
public:
    //`left` will only be a `Variable`, `MemberAccess`, or a `Dereference`.
    Node* left = nullptr;
    Variable* right = nullptr;
    ObjectClass* thisObject = nullptr;
    
    Dereference();
    void ToString(std::string inLeft, std::string inRight, void* head) override;
    void FLVMCodeGen(Instructions* inInstructions, void* head) override;
    void AssignCodeGen(Instructions* inInstructions, Node* head);
    Variable* getBaseVariable();
};

#endif