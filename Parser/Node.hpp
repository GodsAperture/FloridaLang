#ifndef Node_h__
#define Node_h__

#include "Instruction.cpp"
#include "../Lexer/Types.hpp"
#include <string>
#include <vector>

//These just exist.
class Association{
public:
    std::string adjective;
    std::string name;
    int64_t position;

    Association(std::string inAdjective, std::string inName, int64_t inPosition){
        adjective = inAdjective;
        name = inName;
        position = inPosition;
    }

    Association operator=(Association right){
        adjective = right.adjective;
        name = right.name;
        position = right.position;

        return *this;
    }
};

//A Scope will be the literal global scope and function scope.
class Scope{
public:
    std::string name;
    std::vector<Association> objects;

    Scope(std::string inName, std::vector<Association> inObjects){
        name = inName;
        objects = inObjects;
    }

    Scope operator=(Scope& right){
        name = right.name;
        objects = right.objects;

        return *this;
    }

    int64_t size(){
        return objects.size();
    }
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
    virtual void GetVariables(Scope& inScope) = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions) = 0;
    //This is exclusively for use on Variable in Assignment.
    virtual int64_t GetPosition(std::vector<Scope>& inScopes) = 0;

    virtual ~Node(){};
};



class Program : public Node{
public:
    Node* head;
    Program* next;

    Program(Node* inHead);
    void Append(Program* inProgram);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class Add : public Node{
public:
    Node* left;
    Node* right;

    Add(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class Subtract : public Node{
public:
    Node* left;
    Node* right;

    Subtract(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};



class Multiply : public Node{
public: 
    Node* left;
    Node* right;

    Multiply(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class Divide : public Node{
public: 
    Node* left;
    Node* right;

    Divide(Node* LHE, Node* RHE);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};



class Exponent : public Node{
public:
    Node* left;
    Node* right;

    Exponent(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString(std::string inString) override {
        return left->ToString(inString) + "^" + right->ToString(inString);
    }

};



class Factorial : public Node{
public:
    Node* left;

    Factorial(Node* input){
        left = input;
    }

    std::string ToString(std::string inString) override {
        return left->ToString(inString) + "!";
    }

};

class Parentheses : public Node{
public:
    Node* subexpression;

    Parentheses(Node* input);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class Negative : public Node{
public:
    Node* right;

    Negative(Node* input);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};



class Variable : public Node{
public:
    std::string name;

    Variable();
    Variable(std::string inString);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;

    Variable& operator=(Variable& right){
        name = right.name;

        return *this;
    }
};

class Assignment : public Node{
public:
    Node* left; 
    Node* right;

    Assignment(Node* inLeft, Node* inRight);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class Initialize : public Node{
public:
    std::string adjective;
    std::string name;

    Initialize();
    Initialize(std::string inType, std::string inString);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;

    Initialize& operator=(Initialize& right){
        adjective = right.adjective;
        name = right.name;

        return *this;
    }

};

class Fixed8 : public Node{
public:
    int64_t value;

    Fixed8(std::string input);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;

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
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};



class IfObject : public Node{
public:
    Node* condition;
    Node* body;

    IfObject(Node* inCondition, Node* inBody);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class ForLoop : public Node{
public:
    Node* initialization;
    Node* condition;
    Node* increment;
    Node* body;

    ForLoop(Node* inInitialization, Node* inCondition, Node* inIncrement, Node* inBody);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;


};

class Function : public Node{
public:
    std::string type;
    std::string name;
    std::string arguments;
    Node* body;

    Function(std::string type, std::string inName, std::string inArguments, Node* inBody);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;

};

class Goto : public Node{
public:
    std::string name;

    Goto(std::string inName);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class Cgoto : public Node{
    Node* condition;
    Node* body;

    Cgoto(Node* inCondition, Node* inBody);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};

class Landing : public Node{
public:
    std::string name;

    Landing(std::string inName);
    std::string ToString(std::string inString) override;
    void GetVariables(Scope& inScope) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions) override;
    int64_t GetPosition(std::vector<Scope>& inScopes) override;
};
#endif