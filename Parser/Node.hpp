#ifndef Node_h__
#define Node_h__
#include <math.h>
#include <string>
#include <stack>
#include <vector>
#include "Instruction.cpp"
#include "../Lexer/Types.hpp"

//This just exists.
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
    virtual std::string ToPostfix() = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inVector) = 0;

    virtual ~Node(){};
};

class Statement : public Node{
public:
    Node* subexpression;

    Statement(Node* input){
        subexpression = input;
    }

    std::string ToString() override {
        return subexpression->ToString() + ";";
    }

    std::string ToPostfix() override {
        return subexpression->ToPostfix() + ";\n";
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector){
        subexpression->FLVMCodeGen(inVector);
    }
};

class Assignment : public Node{
public:
    Node* left;
    Node* right;

    Assignment(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string ToString() override {
        return left->ToString() + " = " + right->ToString() + ";";
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "= ";
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector){
        right->FLVMCodeGen(inVector);
        inVector.push_back(Instruction(Operation::assign));
    }
};

class Add : public Node{
public:
    Node* left;
    Node* right;

    Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " + " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "+ ";
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        left->FLVMCodeGen(inVector);
        right->FLVMCodeGen(inVector);
        inVector.push_back(Instruction(Operation::add));
    }

};

class Subtract : public Node{
public:
    Node* left;
    Node* right;

    Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " - " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "- ";
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        left->FLVMCodeGen(inVector);
        right->FLVMCodeGen(inVector);
        inVector.push_back(Instruction(Operation::subtract));
    }
};



class Multiply : public Node {
public: 
    Node* left;
    Node* right;

    Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " * " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "* ";
    }    

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        left->FLVMCodeGen(inVector);
        right->FLVMCodeGen(inVector);
        inVector.push_back(Instruction(Operation::multiply));
    }
};

class Divide : public Node{
public: 
    Node* left;
    Node* right;

    Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " / " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "/ ";
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        left->FLVMCodeGen(inVector);
        right->FLVMCodeGen(inVector);
        inVector.push_back(Instruction(Operation::divide));
    }
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

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "^ ";
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

    std::string ToPostfix() override {
        return left->ToPostfix() + "! ";
    }
};

class Parnetheses : public Node{
public:
    Node* subexpression;

    Parnetheses(Node* input){
        subexpression = input;
    }

    std::string ToString() override {
        return "(" + subexpression->ToString() + ")";
    }

    std::string ToPostfix() override {
        return subexpression->ToPostfix();
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        subexpression->FLVMCodeGen(inVector);
    }
};

class Negative : public Node{
public:
    Node* right;

    Negative(Node* input){
        right = input;
    }

    std::string ToString() override {
        return "-" + right->ToString();
    }

    std::string ToPostfix() override {
        return right->ToPostfix() + "NEG ";
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector){
        right->FLVMCodeGen(inVector);
        inVector.push_back(Instruction(Operation::negate));
    }
};



class Variable : public Node{
public:
    std::string name;

    Variable(){
        name = "";
    }

    Variable(std::string input){
        name = input;
    }

    std::string ToString() override {
        return name;
    }

    std::string ToPostfix() override {
        return name;
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        //Null lmfaoooooooo
    }

    Variable& operator=(Variable& right){
        name = right.name;

        return *this;
    }
};

class fixed64 : public Node{
public:
    int64_t value;

    fixed64(std::string input){
        value = std::stoll(input);
    };

    std::string ToString() override {
        return std::to_string(value);
    }

    std::string ToPostfix() override {
        return std::to_string(value) + " ";
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        inVector.push_back(Instruction(value));
    }

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

    std::string ToPostfix() override {
        return std::to_string(value) + " ";
    }

};
#endif