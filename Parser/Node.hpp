#ifndef Node_h__
#define Node_h__

#include "Instruction.cpp"
#include "../Lexer/Types.hpp"
#include <string>
#include <vector>



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
    Node* current;
    Body* next;

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

    Variable(Token thisToken, int64_t inDistance, bool inIsLocal);
    Variable(Variable* inVariable);
    void operator=(Variable* input){
        this->thisToken = input->thisToken;
        this->isLocal = input->isLocal;
        this->distance = input->distance;
        this->next = input->next;
    };
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

        //Find where in some given Scope a particular variable lies.
        //If it does not exist in the scope, it returns -1.
        int64_t where(std::string input);
        //Push a new variable into the scope's variable stack.
        void push(Variable* input);
        Scope();
        Scope(Body* inBody, Variable* inScope, Scope* inParent);
        std::string ToString(std::string inLeft, std::string inRight) override;
        void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    
    };

class Global : public Node{
public:
    Scope* globalScope;
    Node* code;

    Global(Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Initialize : public Node{
public:
    Variable* thisVariable;

    Initialize(Variable* inVariable);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Assignment : public Node{
public:
    Variable* thisVariable;
    Node* code;

    Assignment(Variable* inVariable, Node* inCode);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Add : public Node{
public:
    Node* left;
    Node* right;

    Add(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Subtract : public Node{
public:
    Node* left;
    Node* right;

    Subtract(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Multiply : public Node{
public: 
    Node* left;
    Node* right;

    Multiply(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Divide : public Node{
public: 
    Node* left;
    Node* right;

    Divide(Node* LHE, Node* RHE);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Parentheses : public Node{
public:
    Node* subexpression;

    Parentheses(Node* input);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Negative : public Node{
public:
    Node* right;

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
    Node* left;
    Node* right;

    Equal(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class NotEqual : public Node{
public:
    Node* left;
    Node* right;

    NotEqual(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class GreaterThan : public Node{
public:
    Node* left;
    Node* right;

    GreaterThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class GreaterThanOr : public Node{
public:
    Node* left;
    Node* right;

    GreaterThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class LessThan : public Node{
public:
    Node* left;
    Node* right;

    LessThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class LessThanOr : public Node{
public:
    Node* left;
    Node* right;

    LessThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



//Boolean algebra
class Or : public Node{
public:
    Node* left;
    Node* right;

    Or(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class And : public Node{
public:
    Node* left;
    Node* right;

    And(Node* inLeft, Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Not : public Node{
public:
    Node* right;

    Not(Node* inRight);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



//if statements
class IfClass : public Node{
public:
    Node* condition;
    Scope* ifScope;
    Scope* elseScope;

    IfClass(Node* inCondition, Scope* inIfScope, Scope* inElseScope);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class ForLoop : public Node{
public:
    Node* assign;
    Node* condition;
    Node* incrementer;
    Scope* body;

    ForLoop(Node* inAssign, Node* inCondition, Node* incrementer, Scope* body);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class WhileLoop : public Node{
public:
    Node* condition;
    Scope* body;

    WhileLoop(Node* inCondition, Scope* inBody);
    std::string ToString(std::string inLeft, std::string inRight) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};
#endif