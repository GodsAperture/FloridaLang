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
};

class Node{
public:
    Node(){
        //Exists for the sake of the default constructor.    
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(const Node&&) = delete;
    Node& operator=(const Node&&) = delete;

    virtual std::string ToString() = 0;
    //This is exclusively used for Variable.
    virtual void GetVariables(std::vector<Association>& inAssociation) = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) = 0;
    //This is exclusively for use on Variable in Assignment.
    virtual int64_t GetPosition(std::vector<Association>& inVariables) = 0;

    virtual ~Node(){};
};



class Program : public Node{
public:
    Node* head;
    Program* next;

    Program(Node* inHead);
    void Append(Program* inProgram);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Add : public Node{
public:
    Node* left;
    Node* right;

    Add(Node* LHE, Node* RHE);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Subtract : public Node{
public:
    Node* left;
    Node* right;

    Subtract(Node* LHE, Node* RHE);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};



class Multiply : public Node{
public: 
    Node* left;
    Node* right;

    Multiply(Node* LHE, Node* RHE);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Divide : public Node{
public: 
    Node* left;
    Node* right;

    Divide(Node* LHE, Node* RHE);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};



class Exponent : public Node{
public:
    Node* left;
    Node* right;

    Exponent(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + "^" + right->ToString();
    }

    void GetVariables(std::vector<Association>& inVector) override {
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }


};



class Factorial : public Node{
public:
    Node* left;

    Factorial(Node* input){
        left = input;
    }

    std::string ToString() override {
        return left->ToString() + "!";
    }

};

class Parentheses : public Node{
public:
    Node* subexpression;

    Parentheses(Node* input);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Negative : public Node{
public:
    Node* right;

    Negative(Node* input);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};



class Variable : public Node{
public:
    std::string name;

    Variable();
    Variable(std::string inString);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;

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
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Initialize : public Node{
public:
    std::string adjective;
    std::string name;

    Initialize();
    Initialize(std::string inType, std::string inString);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;

    Initialize& operator=(Initialize& right){
        adjective = right.adjective;
        name = right.name;

        return *this;
    }

};

class Fixed64 : public Node{
public:
    int64_t value;

    Fixed64(std::string input);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;

};

class float64 : public Node{
public:
    double value;

    float64(std::string input){
        value = std::stod(input);
    }

    std::string ToString() override {
        return std::to_string(value);
    }

};

class Boolean : public Node{
public:
    bool value;

    Boolean(bool inBool);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};



class IfObject : public Node{
public:
    Node* condition;
    Node* body;

    IfObject(Node* inCondition, Node* inBody);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Goto : public Node{
public:
    std::string name;

    Goto(std::string inName);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Cgoto : public Node{
    Node* condition;
    Node* body;

    Cgoto(Node* inCondition, Node* inBody);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};

class Landing : public Node{
public:
    std::string name;

    Landing(std::string inName);
    std::string ToString() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
    int64_t GetPosition(std::vector<Association>& inVariables) override;
};
#endif