#ifndef Node_h__
#define Node_h__
#include <math.h>
#include <string>
#include <stack>
#include <vector>
#include "Instruction.cpp"

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
    virtual std::string ToRPN() = 0;//These have parentheses, ToPostfix() does not.
    virtual void ToStack(std::stack<long long>& inStack) = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inVector) = 0;
    virtual long long ToEvaluate() = 0;
    virtual ~Node(){};
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

    std::string ToRPN() override {
        return "(" + left->ToRPN() + ") (" + right->ToRPN() + ") + ";
    }

    long long ToEvaluate() override {
        return left->ToEvaluate() + right->ToEvaluate();
    }

    void ToStack(std::stack<long long>& inStack) override {
        left->ToStack(inStack);
        right->ToStack(inStack);
        long long val1 = inStack.top();
        inStack.pop();
        long long val2 = inStack.top();
        inStack.pop();
        inStack.push(val2 + val1);
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

    std::string ToRPN() override {
        return "(" + left->ToRPN() + ") (" + right->ToRPN() + ") - ";
    }

    long long ToEvaluate() override {
        return left->ToEvaluate() - right->ToEvaluate();
    }

    void ToStack(std::stack<long long>& inStack) override {
        left->ToStack(inStack);
        right->ToStack(inStack);
        long long val1 = inStack.top();
        inStack.pop();
        long long val2 = inStack.top();
        inStack.pop();
        inStack.push(val2 - val1);
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

    std::string ToRPN() override {
        return "(" + left->ToRPN() + ") (" + right->ToRPN() + ") * ";
    }
    
    long long ToEvaluate() override {
        return left->ToEvaluate() * right->ToEvaluate();
    }

    void ToStack(std::stack<long long>& inStack) override {
        left->ToStack(inStack);
        right->ToStack(inStack);
        long long val1 = inStack.top();
        inStack.pop();
        long long val2 = inStack.top();
        inStack.pop();
        inStack.push(val2 * val1);
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

    std::string ToRPN() override {
        return "(" + left->ToRPN() + ") (" + right->ToRPN() + ") / ";
    }

    long long ToEvaluate() override {
        return left->ToEvaluate() / right->ToEvaluate();
    }

    void ToStack(std::stack<long long>& inStack) override {
        left->ToStack(inStack);
        right->ToStack(inStack);
        long long val1 = inStack.top();
        inStack.pop();
        long long val2 = inStack.top();
        inStack.pop();
        inStack.push(val2 / val1);
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

    std::string ToRPN() override {
        return "(" + left->ToRPN() + right->ToRPN() + ") ^ ";
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "^ ";
    }

    void ToStack(std::stack<long long>& inStack) override {
        left->ToStack(inStack);
        right->ToStack(inStack);
        long long val1 = inStack.top();
        inStack.pop();
        long long val2 = inStack.top();
        inStack.pop();
        inStack.push(pow(val2, val1));
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

    std::string ToRPN() override {
        return "(" + left->ToRPN() + ") ! ";
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + "! ";
    }

    void ToStack(std::stack<long long>& inStack) override {
        left->ToStack(inStack);
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

    std::string ToRPN() override {
        return subexpression->ToRPN();
    }

    std::string ToPostfix() override {
        return subexpression->ToPostfix();
    }

    void ToStack(std::stack<long long>& inStack) override {
        subexpression->ToStack(inStack);
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

    std::string ToRPN() override {
        return "(" + right->ToPostfix() + ") - ";
    }

    std::string ToPostfix() override {
        return right->ToPostfix() + "NEG ";
    }

    void ToStack(std::stack<long long>& inStack) override {
        right->ToStack(inStack);
    }
};

class fixed64 : public Node{
public:
    long long value;

    fixed64(std::string input){
        value = std::stoll(input);
    };

    std::string ToString() override {
        return std::to_string(value);
    }

    std::string ToPostfix() override {
        return std::to_string(value) + " ";
    }

    std::string ToRPN() override {
        return std::to_string(value) + " ";
    }

    long long ToEvaluate() override {
        return value;
    }

    void ToStack(std::stack<long long>& inStack) override {
        inStack.push(value);
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

    std::string ToRPN() override {
        return std::to_string(value) + " ";
    }
};
#endif