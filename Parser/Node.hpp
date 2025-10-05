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
    bool isLocal = false;
    int64_t distance = -1;
    types value;
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
    Node* copy(StackAllocator& input) override;
    Variable* pcopy(StackAllocator& input);
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
        //All the classes that exist in the scope.
        //

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

        //Standard methods.
        Scope();
        Scope(Body* inBody, Variable* inScope, Scope* inParent);
        std::string ToString(std::string inLeft, std::string inRight) override;
        std::string printAll() override;
        void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
        Node* copy(StackAllocator& input) override;
        Scope* pcopy(StackAllocator& input);
    };

class Initialize : public Node{
public:
    Variable* thisVariable = nullptr;

    Initialize();
    Initialize(Variable* inVariable);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Initialize* pcopy(StackAllocator& input);
};

class InitializeAssign : public Node{
public:
    Variable* thisVariable = nullptr;
    Node* code = nullptr;
    FloridaType type;

    InitializeAssign();
    InitializeAssign(Variable* inVariable, Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    InitializeAssign* pcopy(StackAllocator& input);
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




class Fixed8 : public Node{
public:
    int64_t value;

    Fixed8();
    Fixed8(std::string input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Fixed8* pcopy(StackAllocator& input);
};

class Boolean : public Node{
public:
    bool value;

    Boolean();
    Boolean(bool inBool);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    Boolean* pcopy(StackAllocator& input);
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



//if statements
class IfClass : public Node{
public:
    Node* condition = nullptr;
    Body* ifBody = nullptr;
    Body* elseBody = nullptr;
    size_t ifVarCount = 0;
    size_t elseVarCount = 0;

    IfClass();
    IfClass(Node* inCondition, Body* inIfScope, Body* inElseScope);
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
    Body* body = nullptr;

    ForLoop();
    ForLoop(Node* inAssign, Node* inCondition, Node* inIncrementer, Body* inBody);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    ForLoop* pcopy(StackAllocator& input);
};

class WhileLoop : public Node{
public:
    Node* condition = nullptr;
    Body* body = nullptr;

    WhileLoop();
    WhileLoop(Node* inCondition, Body* inBody);
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
    FloridaType type = FloridaType::BadToken;
    //This is where the instruction set begings.
    int64_t position = -1;
    //This is the full function
    Scope* code = nullptr;
    //For use in the parser and VM.
    //This points to the next function in the scope, if any.
    Function* next = nullptr;
    //This connects all functions across scopes.
    Function* allFunctions = nullptr;
    //This makes sure code isn't generated twice.
    bool alreadyGenerated = false;

    Function();
    Function(bool inReturnable, std::string_view inName, Scope* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    void append(Variable* input);
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
    Node* statement = nullptr;

    ReturnClass();
    ReturnClass(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    std::string printAll() override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    Node* copy(StackAllocator& input) override;
    ReturnClass* pcopy(StackAllocator& input);
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