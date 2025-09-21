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

std::string typeString(FloridaType input);

FloridaType typeReturn(std::string inString);

bool typeCheck(FloridaType inType);



//If you need to be told what a node is, God help you.
class Node{
public:
    Node(){
        //Exists for the sake of the default constructor.    
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(const Node&&) = delete;
    Node& operator=(const Node&&) = delete;

    virtual std::string ToString(std::string inLeft, std::string inRight) = 0;
    virtual std::string printAll() = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions) = 0;
    virtual void copy(StackAllocator& input) = 0;

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
    void copy(StackAllocator& input) override;
};

class Variable : public Node{
public:
    Token thisToken;//There exists a default Token().
    bool isLocal = false;
    int64_t distance = -1;
    Variable* next = nullptr;
    FloridaType type = FloridaType::BadToken;

    Variable();
    Variable(Token thisToken, int64_t inDistance, bool inIsLocal);
    Variable(Variable* inVariable);
    void operator=(Variable* input){
        this->thisToken = input->thisToken;
        this->isLocal = input->isLocal;
        this->distance = input->distance;
        this->next = input->next;
    };
    void append(Variable* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;

};

class Scope : public Node{
    public:
        //This points to the outerscope.
        Scope* parent = nullptr;
        //This is the body of code within the scope.
        Body* body = nullptr;
        //This will be the variables of the current scope.
        Variable* variables = nullptr;
        //A "linked list" of functions for the current scope.
        Function* functions = nullptr;

        //Find where in some given Scope a particular variable lies.
        //If it does not exist in the scope, it returns -1.
        int64_t varWhere(std::string input);
        //Find where in some given Scope a particular variable lies.
        //If it does not exist in the scope, it returns -1.
        Function* funGet(std::string input);
        //Push a new variable into the scope's variable stack.
        void push(Variable* input);
        void push(Function* input);
        void varPop();
        //funPop isn't needed because they don't disappear in pseudoscopes.
        size_t varCount();
        size_t funCount();
        Scope();

        //Standard methods.
        Scope();
        Scope(Body* inBody, Variable* inScope, Scope* inParent);
        std::string ToString(std::string inLeft, std::string inRight) override;
        std::string printAll() override;
        void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
        void copy(StackAllocator& input) override;
    
    };

class Initialize : public Node{
public:
    Variable* thisVariable = nullptr;

    Initialize(Variable* inVariable);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class InitializeAssign : public Node{
public:
    Variable* thisVariable = nullptr;
    Node* code = nullptr;
    FloridaType type;

    InitializeAssign(Variable* inVariable, Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class Assignment : public Node{
public:
    Variable* thisVariable = nullptr;
    Node* code = nullptr;

    Assignment(Variable* inVariable, Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};



class Add : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Add(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class Subtract : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Subtract(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};



class Multiply : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Multiply(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class Divide : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Divide(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};



class Parentheses : public Node{
public:
    Node* subexpression = nullptr;

    Parentheses(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class Negative : public Node{
public:
    Node* right = nullptr;

    Negative(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};




class Fixed8 : public Node{
public:
    int64_t value;

    Fixed8(std::string input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class Boolean : public Node{
public:
    bool value;

    Boolean(bool inBool);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};



//Object comparisons
class Equal : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Equal(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class NotEqual : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    NotEqual(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class GreaterThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class GreaterThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class LessThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class LessThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};



//Boolean algebra
class Or : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Or(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class And : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    And(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class Not : public Node{
public:
    Node* right = nullptr;

    Not(Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};



//if statements
class IfClass : public Node{
public:
    Node* condition = nullptr;
    Body* ifBody = nullptr;
    Body* elseBody = nullptr;
    size_t ifVarCount = 0;
    size_t elseVarCount = 0;

    IfClass(Node* inCondition, Body* inIfScope, Body* inElseScope);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class ForLoop : public Node{
public:
    Node* assign = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;
    Body* body = nullptr;

    ForLoop(Node* inAssign, Node* inCondition, Node* inIncrementer, Body* inBody);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class WhileLoop : public Node{
public:
    Node* condition = nullptr;
    Body* body = nullptr;

    WhileLoop(Node* inCondition, Body* inBody);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class Function : public Node{
public:
    std::string_view name;
    bool returnable = false;
    FloridaType type;
    //This is where the instruction set begings.
    int64_t position = -1;
    //This is the full function
    Scope* code = nullptr;
    //For use in the parser and VM.
    //This points to the next function in the scope, if any.
    Function* next = nullptr;
    //This connects all functions across scopes.
    Function* allFunctions = nullptr;

    Function(bool inReturnable, std::string_view inName, Scope* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void append(Variable* input);
    void copy(StackAllocator& input) override;
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
    void copy(StackAllocator& input) override;
};

class Call : public Node{
public:
    Function* function = nullptr;
    Arguments* arguments = nullptr;

    Call(Function* inFunction);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class ReturnClass : public Node{
public:
    Node* statement = nullptr;

    ReturnClass(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void copy(StackAllocator& input) override;
};

class CallStack{
public:
    Scope* thisScope;
    uint64_t reference;
    uint64_t instNumber;

    CallStack();
    CallStack(Scope* inTheScope, uint64_t inReference, uint64_t inInstNumber);
    int64_t where(Variable* inVariable);
};
#endif