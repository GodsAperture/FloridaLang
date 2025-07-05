#ifndef Node_h__
#define Node_h__

#include "Instruction.cpp"
#include "../Lexer/Types.hpp"
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
    //This is exclusively used for Variable.
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions) = 0;

    virtual ~Node(){};
};

class Body : public Node{
public:
    Node* current = nullptr;
    Body* next = nullptr;

    Body(Node* inCurrent, Body* inNext);
    Body(Node* inCurrent);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Variable : public Node{
public:
    Token thisToken;
    bool isLocal = false;
    int64_t distance;
    Variable* next = nullptr;
    FloridaType type = FloridaType::BadToken;

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
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;

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
        Scope(Body* inBody, Variable* inScope, Scope* inParent);
        std::string ToString(std::string inLeft, std::string inRight) override;
        void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    
    };

class Initialize : public Node{
public:
    Variable* thisVariable = nullptr;

    Initialize(Variable* inVariable);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class InitializeAssign : public Node{
public:
    Variable* thisVariable = nullptr;
    Node* code = nullptr;
    FloridaType type;

    InitializeAssign(Variable* inVariable, Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Assignment : public Node{
public:
    Variable* thisVariable = nullptr;
    Node* code = nullptr;

    Assignment(Variable* inVariable, Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Add : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Add(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Subtract : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Subtract(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Multiply : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Multiply(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Divide : public Node{
public: 
    Node* left = nullptr;
    Node* right = nullptr;

    Divide(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Parentheses : public Node{
public:
    Node* subexpression = nullptr;

    Parentheses(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Negative : public Node{
public:
    Node* right = nullptr;

    Negative(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};




class Fixed8 : public Node{
public:
    int64_t value;

    Fixed8(std::string input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Boolean : public Node{
public:
    bool value;

    Boolean(bool inBool);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



//Object comparisons
class Equal : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Equal(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class NotEqual : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    NotEqual(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class GreaterThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class GreaterThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    GreaterThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class LessThan : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class LessThanOr : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    LessThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



//Boolean algebra
class Or : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    Or(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class And : public Node{
public:
    Node* left = nullptr;
    Node* right = nullptr;

    And(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Not : public Node{
public:
    Node* right = nullptr;

    Not(Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
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
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class ForLoop : public Node{
public:
    Node* assign = nullptr;
    Node* condition = nullptr;
    Node* incrementer = nullptr;
    Body* body = nullptr;

    ForLoop(Node* inAssign, Node* inCondition, Node* inIncrementer, Body* inBody);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class WhileLoop : public Node{
public:
    Node* condition = nullptr;
    Body* body = nullptr;

    WhileLoop(Node* inCondition, Body* inBody);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Function : public Node{
public:
    std::string_view name;
    bool returnable = false;
    FloridaType type;
    //This is where the instruction set begings.
    int64_t position;
    Scope* variables = nullptr;
    //This is the body of the function.
    Scope* theFunction = nullptr;
    //For use in the parser and VM.
    Function* next = nullptr;

    Function(bool inReturnable, std::string_view inName, Scope* inInitialize, Scope* inTheFunction);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void append(Variable* input);
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;

};

class Call : public Node{
public:
    Function* function = nullptr;
    Body* arguments = nullptr;

    Call(Function* inFunction);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class ReturnClass : public Node{
public:
    Node* statement = nullptr;

    ReturnClass(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};
#endif