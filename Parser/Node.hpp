#ifndef Node_h__
#define Node_h__

#include "Instruction.cpp"
#include "../Lexer/Types.hpp"
#include <string>
#include <vector>



class Association{
public:
    Token token;
    FloridaType type;
    int64_t position;
};



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

    virtual std::string ToString(std::string inString) = 0;
    //This is exclusively used for Variable.
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions) = 0;

    virtual ~Node(){};
};

class Body : public Node{
public:
    Node* current;
    Body* next;

    Body(Node* inCurrent, Body* inNext);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Scope : public Node{
    public:
        //This is where the scope is in the stack.
        //This is only relevant during run time.
        int64_t position = 0;
        //This points to the outerscope.
        Scope* parent = nullptr;
        //This is the body of code within the scope.
        Body* body = nullptr;
        //This will be the variables of the current scope.
        std::vector<Association> variables = std::vector<Association>(4);

        //Find where in some given Scope a particular variable lies.
        int64_t where(std::string input);
        //Push a new variable into the scope's variable stack.
        void push(Association input);
        Scope();
        Scope(Body* inBody, Scope* inScope);
        std::string ToString(std::string inString) override;
        void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    
    };



class Add : public Node{
public:
    Node* left;
    Node* right;

    Add(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Subtract : public Node{
public:
    Node* left;
    Node* right;

    Subtract(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Multiply : public Node{
public: 
    Node* left;
    Node* right;

    Multiply(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Divide : public Node{
public: 
    Node* left;
    Node* right;

    Divide(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



class Parentheses : public Node{
public:
    Node* subexpression;

    Parentheses(Node* input);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Negative : public Node{
public:
    Node* right;

    Negative(Node* input);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};




class Fixed8 : public Node{
public:
    int64_t value;

    Fixed8(std::string input);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class float64 : public Node{
public:
    double value;

    float64(std::string input){
        value = std::stod(input);
    }

    std::string ToString(std::string inString) override {
        return std::to_string(value);
    }

};

class Boolean : public Node{
public:
    bool value;

    Boolean(bool inBool);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



//Object comparisons
class Equal : public Node{
public:
    Node* left;
    Node* right;

    Equal(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class NotEqual : public Node{
public:
    Node* left;
    Node* right;

    NotEqual(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class GreaterThan : public Node{
public:
    Node* left;
    Node* right;

    GreaterThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class GreaterThanOr : public Node{
public:
    Node* left;
    Node* right;

    GreaterThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class LessThan : public Node{
public:
    Node* left;
    Node* right;

    LessThan(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class LessThanOr : public Node{
public:
    Node* left;
    Node* right;

    LessThanOr(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};



//Boolean algebra
class Or : public Node{
public:
    Node* left;
    Node* right;

    Or(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Nor : public Node{
public:
    Node* left;
    Node* right;

    Nor(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Xor : public Node{
public:
    Node* left;
    Node* right;

    Xor(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Xnor : public Node{
public:
    Node* left;
    Node* right;

    Xnor(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class And : public Node{
public:
    Node* left;
    Node* right;

    And(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Nand : public Node{
public:
    Node* left;
    Node* right;

    Nand(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};

class Not : public Node{
public:
    Node* right;

    Not(Node* inRight);
    std::string ToString(std::string inString) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
};
#endif