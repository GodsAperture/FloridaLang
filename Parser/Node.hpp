#ifndef Node_h__
#define Node_h__

#include "Instructions.cpp"
#include "../Lexer/Types.hpp"
#include "StackAllocator.hpp"
#include <string>
#include <vector>
//Forward declarations, idfk *why* though.
//My C/C++ understanding is still hindering me.
class ObjectClass;

std::string typeString(FloridaType input);

FloridaType typeReturn(std::string inString);

bool typeCheck(FloridaType inType);

class StackAllocator;

class Node;
class Body;
class Variable;
class Pointer;
class ThisKeyword;
class Scope;

class Initialize;
class Assignment;

class Addition;
class Subtraction;
class Multiplication;
class Division;

class Parentheses;
class Negative;

class Primitive;

class TypecastClass;

class Equal;
class NotEqual;
class GreaterThan;
class GreaterThanOr;
class LessThan;
class LessThanOr;

class Or;
class And;
class Not;

class IfClass;
class WhileLoop;
class ForLoop;

class Function;
class Arguments;
class FunctionCall;
class ReturnClass;

class ObjectClass;
class MemberAccess;
class Dereference;
class Error;

//Universal union.
//Can be any node.
union UUnion{
    Node* node;
    Body* body;
    Variable* variable;
    Pointer* pointer;
    ThisKeyword* thisKeyword;

    Scope* scope;
    Initialize* initialize;
    Assignment* assignment;

    Addition* addition;
    Subtraction* subtraction;
    Multiplication* multiplication;
    Division* division;

    Parentheses* parentheses;
    Negative* negative;

    Primitive* primitive;

    TypecastClass* typecastClass;

    Equal* equal;
    NotEqual* nEqual;
    GreaterThan* greaterThan;
    GreaterThanOr* greaterThanOr;
    LessThan* lessThan;
    LessThanOr* lessThanOr;

    Or* orClass;
    And* andClass;
    Not* notClass;

    IfClass* ifClass;
    WhileLoop* whileLoop;
    ForLoop* forLoop;

    Function* function;
    Arguments* arguments;
    FunctionCall* functionCall;
    ReturnClass* returnClass;

    ObjectClass* object;
    MemberAccess* memberAccess;
    Dereference* dereference;
    Error* error;

    UUnion(){
        node = nullptr;
    }
};

enum NodeType : int8_t {
    NTNull,
    NTBody,
    NTVariable,
    NTPointer,
    NTThisKeyword,
    NTScope,

    NTInitialize,
    NTAssignment,

    NTAddition,
    NTSubtraction,
    NTMultiplication,
    NTDivision,

    NTParentheses,
    NTNegative,

    NTPrimitive,

    NTTypecastClass,

    NTEqual,
    NTNotEqual,
    NTGreaterThan,
    NTGreaterThanOr,
    NTLessThan,
    NTLessThanOr,

    NTOr,
    NTAnd,
    NTNot,

    NTIfClass,
    NTWhileLoop,
    NTForLoop,

    NTFunction,
    NTArguments,
    NTFunctionCall,
    NTReturnClass,

    NTObjectClass,
    NTMemberAccess,
    NTDereference,
    NTError,
};

//If you need to be told what a node is, God help you.
class Node{
public:
    NodeType nodeType = NodeType::NTNull;
    FloridaType type = FloridaType::BadToken;
    Node(){
        //Exists for the sake of the default constructor.
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(const Node&&) = delete;
    Node& operator=(const Node&&) = delete;

    //Prints the program back to the user in a proper format.
    virtual void CodePrint(std::string inLeft, std::string inRight) = 0;
    virtual std::string ToString(std::string inLeft, std::string inRight) = 0;
    virtual void FLVMCodeGen(Instructions* inInstructions) = 0;
    //virtual Node* differentiate(int64_t input) = 0;
    virtual ~Node(){};
};

class Body : public Node{
public:
    UNode current;
    Body* next = nullptr;

    Body();
    Body* append(Body* input);
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
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
    void append(Variable* input, void* head);
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class Pointer : public Variable{
public:
    Pointer();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class ThisKeyword : public Variable {
public:
    ThisKeyword();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class Scope : public Node{
    public:
        //This points to the outerscope.
        Scope* parent = nullptr;
        //This is for convenience during debugging.
        Scope* allScopes = nullptr;
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

        //If these scopes are associated with a function or class definition,
        //then these will not be a nullptr.
        Function* associatedFunction = nullptr;
        ObjectClass* associatedClass = nullptr;

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
        void push(Initialize* input);
        //Push a new function into the scope's function stack.
        void push(Function* input);
        //Push a new class into the scope's class stack.
        void push(ObjectClass* input);

        //Assigns each variable a byte value that will be used to
        //determine where in the stack or heap a packed variable is.
        void byteAssign();
        //Checks if a variable is accessible in the current scope.
        bool hasVariable(std::string_view input);
        //Checks if an object is accessible in the current scope.
        bool hasObject(std::string_view input);
        //Checks if a function is accessible in the current scope.
        bool hasFunction(std::string_view input);
        
        //Gets the pointer to the variable in question.
        Variable* getVariable(std::string_view input);
        //Grabs the pointer to the variable in question.
        //The int64_t is the offset that the variable exists at.
        Variable* getVariable(int64_t input);
        //Gets the pointer to the object in question.
        ObjectClass* getObject(std::string_view input);
        //Gets the pointer to the function in question.
        Function* getFunction(std::string_view input);

        //Standard methods.
        Scope();
        void CodePrint(std::string inLeft, std::string inRight) override;
        std::string ToString(std::string inLeft, std::string inRight) override;
        void FLVMCodeGen(Instructions* inInstructions) override;
        
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
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
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
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



//Mathematical operations
class Addition : public Node{
public:
    UNode left;
    UNode right;

    Addition();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class Subtraction : public Node{
public:
    UNode left;
    UNode right;

    Subtraction();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class Multiplication : public Node{
public: 
    UNode left;
    UNode right;

    Multiplication();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class Division : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Division();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



class Parentheses : public Node{
public:
    Error* MissingLP = nullptr;
    Arguments* subexpression = nullptr;
    Error* MissingRP = nullptr;

    Parentheses();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class Negative : public Node{
public:
    UNode right;

    Negative();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



//Primitive type
class Primitive : public Node{
public:
    types value;

    Primitive();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



//Type casts
class TypecastClass : public Node{
public:
    FloridaType secondType = BadToken;
    Node* body = nullptr;

    TypecastClass();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



//Object comparisons
class Equal : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Equal();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class NotEqual : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    NotEqual();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class GreaterThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThan();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class GreaterThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThanOr();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class LessThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThan();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class LessThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThanOr();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



//Boolean algebra
class Or : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Or();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class And : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    And();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class Not : public Node{
public:
    Node* right = nullptr;

    Not();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



class IfClass : public Node{
public:
    Error* MissingLP = nullptr;
    Node* condition = nullptr;
    Error* MissingRP = nullptr;
    Error* MissingIfLCB = nullptr;
    Scope* ifBody = nullptr;
    Error* MissingIfRCB = nullptr;
    Error* MissingElseLCB = nullptr;
    Scope* elseBody = nullptr;
    Error* MissingElseRCB = nullptr;

    IfClass();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class ForLoop : public Node{
public:
    Node* assign = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;
    Error* MissingLCB = nullptr;
    Scope* body = nullptr;
    Error* MissingRCB = nullptr;

    ForLoop();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class WhileLoop : public Node{
public:
    Node* condition = nullptr;
    Error* MissingLCB = nullptr;
    Scope* body = nullptr;
    Error* MissingRCB = nullptr;

    WhileLoop();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};



class Function : public Node{
public:
    //The `this` keyword relative to this function.
    //This will never be a nullptr.
    ObjectClass* thisPointer = nullptr;
    //The function's name, hopefully that's obvious.
    std::string_view name;
    bool returnable = false;
    //This is where the instruction set begins.
    int64_t position = -1;
    //This is where all of the function arguments (if any) are.
    Arguments* allArguments = nullptr;
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
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
    void append(Initialize* input);
    bool contains(Node* input);
};

class Arguments : public Node{
public:
    Node* current = nullptr;
    Arguments* next = nullptr;

    Arguments();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
    //void append(Arguments* input, Node* head);
    bool equals(Arguments* input);
    int64_t matchCount(Arguments* input);
    int64_t length();
    bool hasErrors();
};

class FunctionCall : public Node{
public:
    Function* function = nullptr;
    Arguments* arguments = nullptr;
    int64_t argumentByteSize = 0;

    FunctionCall();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

class ReturnClass : public Node{
public:
    //There will always be a minimum of 1 scope to exit.
    int64_t returnCount = 1;
    Node* statement = nullptr;

    ReturnClass();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
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
//     void CodePrint(std::string inLeft, std::string inRight) override;
//     std::string ToString(std::string inLeft, std::string inRight) override;
//     void FLVMCodeGen(Instructions* inInstructions) override;
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
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
    //Given some variable name, get the offset for the object from this class' stack.
    int64_t whereVariable(std::string input);
    //Given some variable name, get the offset in memory for it.
    //This will be useful for objects in the heap.
    char* variableOffset(std::string input);
};

class MemberAccess : public Node{
public:
    //`left` will only be a `Variable`.
    UNode left;
    //`right` will either be a `MemberAccess` or a `Variable`.
    UNode right;
    
    MemberAccess();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
    int64_t offset();
};

class Dereference : public Node{
public:
    //`left` will either be a `Variable` or a `MemberAccess`.
    UNode left;
    //`right` will be a `Dereference`, a `MemberAccess`, or a `Variable`.
    UNode right;
    ObjectClass* objectType = nullptr;
    
    Dereference();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
    int64_t offset();
};



enum PError{
    PMissingSubexpression,
    PMissingLParentheses,
    PMissingRParentheses,
    PMissingLCurlyBrace,
    PMissingRCurlyBrace,
    PMissingLBracket,
    PMissingRBracket,
    PMissingSemicolon
};

class Error : public Node{
public:
    PError errorType = PMissingSubexpression;
    int64_t row = -1;
    int64_t column = -1;
    Node* body = nullptr;
    std::string placeHolder = "";

    Error();
    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string ErrorPrint();
    void FLVMCodeGen(Instructions* inInstructions) override;
};



class UNode : public Node{
public:
    UUnion joker;

    UNode();
    UNode(const UNode& input);

    UNode operator=(UNode input){
        type = input.type;
        joker = input.joker;
        nodeType = input.nodeType;

        return *this;
    }

    void operator=(Node* right){
        joker.node = right;
        nodeType = right->nodeType;
    }

    void CodePrint(std::string inLeft, std::string inRight) override;
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(Instructions* inInstructions) override;
};

#endif