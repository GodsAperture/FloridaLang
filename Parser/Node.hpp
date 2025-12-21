#ifndef Node_h__
#define Node_h__

#include "Instruction.cpp"
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

    virtual std::string ToString(std::string inLeft, std::string inRight) = 0;
    //Prints out the program as its bytecode.
    virtual std::string printAll() = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions) = 0;
    virtual Node* copy(StackAllocator& input) = 0;

    virtual ~Node(){};
};

class Body : public Node{
public:
    Node* current = nullptr;
    Body* next = nullptr;

    Body();
    Body(Node* input);
    Body* append(Body* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Body* pcopy(StackAllocator& input);
};

class Variable : public Node{
public:
    Token thisToken;//There exists a default Token() constructor.
    //0 = local, 1 = middle, 2 = global, 3 = error. 
    char where = 3;
    Scope* owner = nullptr;
    int64_t distance = -1;
    types value;
    Variable* next = nullptr;

    Variable();
    void operator=(Variable* input){
        this->thisToken = input->thisToken;
        this->owner = input->owner;
        this->where = input->where;
        this->distance = input->distance;
        this->next = input->next;
    };
    void append(Variable* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Variable* pcopy(StackAllocator& input);
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
        //This will contain all of the class definitions in the scope.
        ObjectClass* allObjects = nullptr;
        //A "linked list" of functions for the current scope.
        Function* functions = nullptr;
        //The counter will determine which index to use in the `uniqueScopes` in the VM.
        int64_t whichScope = 0;

        //Find where in some given Scope a particular variable lies.
        //If it does not exist in the scope, it returns -1.
        int64_t varWhere(std::string input);
        Initialize* getInit(std::string input);
        //Find where in some given Scope a particular variable lies.
        //If it does not exist in the scope, it returns -1.
        Function* funGet(std::string input);
        ObjectClass* objGet(std::string input);
        //Push a new initialization into the scope's initialize stack.
        void push(Initialize* input);
        //Push a new function into the scope's function stack.
        void push(Function* input);
        //Push a new class into the scope's class stack.
        void push(ObjectClass* input);
        //funPop isn't needed because they don't disappear in pseudoscopes.
        size_t varCount();
        size_t funCount();

        //Standard methods.
        Scope();
        std::string ToString(std::string inLeft, std::string inRight) override;
        std::string printAll() override;
        void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
        Node* copy(StackAllocator& input) override;
        Scope* pcopy(StackAllocator& input);
    };

class Initialize : public Node{
public:
    Variable* thisVariable = nullptr;
    //Only useful for function, method, class, constructor, etc. defintions.
    Initialize* next = nullptr;

    Initialize();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    virtual Initialize* pcopy(StackAllocator& input);
};

class InitializeAssign : public Initialize{
public:
    Node* code = nullptr;

    InitializeAssign();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    InitializeAssign* pcopy(StackAllocator& input) override;
};

class Assignment : public Node{
public:
    Variable* thisVariable = nullptr;
    Node* code = nullptr;

    Assignment();
    Assignment(Variable* inVariable, Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Assignment* pcopy(StackAllocator& input);
};



//Mathematical operations
class Add : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Add();
    Add(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Add* pcopy(StackAllocator& input);
};

class Subtract : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Subtract();
    Subtract(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Subtract* pcopy(StackAllocator& input);
};



class Multiply : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Multiply();
    Multiply(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Multiply* pcopy(StackAllocator& input);
};

class Divide : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Divide();
    Divide(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Divide* pcopy(StackAllocator& input);
};



class Parentheses : public Node{
public:
    Node* subexpression = nullptr;

    Parentheses();
    Parentheses(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Parentheses* pcopy(StackAllocator& input);
};

class Negative : public Node{
public:
    Node* right = nullptr;

    Negative();
    Negative(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Negative* pcopy(StackAllocator& input);
};



//Primitive type
class Primitive : public Node{
public:
    types value;

    Primitive();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Primitive* pcopy(StackAllocator& input);
};



//Type casts
class TypecastClass : public Node{
public:
    Node* body = nullptr;

    TypecastClass();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    TypecastClass* pcopy(StackAllocator& input);
};



//Object comparisons
class Equal : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Equal();
    Equal(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Equal* pcopy(StackAllocator& input);
};

class NotEqual : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    NotEqual();
    NotEqual(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    NotEqual* pcopy(StackAllocator& input);
};

class GreaterThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThan();
    GreaterThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    GreaterThan* pcopy(StackAllocator& input);
};

class GreaterThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThanOr();
    GreaterThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    GreaterThanOr* pcopy(StackAllocator& input);
};

class LessThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThan();
    LessThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    LessThan* pcopy(StackAllocator& input);
};

class LessThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThanOr();
    LessThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    LessThanOr* pcopy(StackAllocator& input);
};



//Boolean algebra
class Or : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Or();
    Or(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Or* pcopy(StackAllocator& input);
};

class And : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    And();
    And(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    And* pcopy(StackAllocator& input);
};

class Not : public Node{
public:
    Node* right = nullptr;

    Not();
    Not(Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Not* pcopy(StackAllocator& input);
};



class IfClass : public Node{
public:
    Node* condition = nullptr;
    Scope* ifBody = nullptr;
    Scope* elseBody = nullptr;
    size_t ifVarCount = 0;
    size_t elseVarCount = 0;

    IfClass();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    IfClass* pcopy(StackAllocator& input);
};

class ForLoop : public Node{
public:
    Node* assign = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;
    Scope* body = nullptr;

    ForLoop();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    ForLoop* pcopy(StackAllocator& input);
};

class WhileLoop : public Node{
public:
    Node* condition = nullptr;
    Scope* body = nullptr;

    WhileLoop();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    WhileLoop* pcopy(StackAllocator& input);
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
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void append(Initialize* input);
    Node* copy(StackAllocator& input) override;
    Function* pcopy(StackAllocator& input);
};

class Arguments : public Node{
public:
    Node* current = nullptr;
    Arguments* next = nullptr;

    Arguments();
    Arguments(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void append(Arguments* input);
    Node* copy(StackAllocator& input) override;
    Arguments* pcopy(StackAllocator& input);
};

class Call : public Node{
public:
    Function* function = nullptr;
    Arguments* arguments = nullptr;

    Call();
    Call(Function* inFunction);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Call* pcopy(StackAllocator& input);
};

class ReturnClass : public Node{
public:
    //There will always be a minimum of 1 scope to exit.
    int64_t returnCount = 1;
    Node* statement = nullptr;

    ReturnClass();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    ReturnClass* pcopy(StackAllocator& input);
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
//     std::string ToString(std::string inLeft, std::string inRight) override;
//     std::string printAll() override;
//     void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
//     void append(Variable* input);
//     Node* copy(StackAllocator& input) override;
//     Method* pcopy(StackAllocator& input);
// };

class ObjectClass : public Node{
public:
    std::string_view name;
    Scope* code = nullptr;
    ObjectClass* next = nullptr;
    uint64_t memorySize = 0;
    ////TO DO
    void* defaultConstruction = nullptr;
    //Method* allMethods = nullptr;

    ObjectClass();
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    ObjectClass* pcopy(StackAllocator& input);
};

#endif