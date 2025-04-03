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

    virtual std::string ToString(std::string inString) = 0;
    //This is exclusively used for Variable.
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions) = 0;

    virtual ~Node(){};
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
#endif