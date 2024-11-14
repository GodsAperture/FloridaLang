#ifndef Node_h__
#define Node_h__

#include <math.h>
#include <string>
#include <stack>
#include <vector>
#include "Association.hpp"
#include "Instruction.cpp"
#include "../Lexer/Types.hpp"
import Association;

//These just exist.
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
    virtual void GetVariables(std::vector<Association>& inVector) = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inVector) = 0;

    virtual ~Node(){};
};



class Program : public Node {
public:
    Node* head;
    Program* next;

    Program(Node* inHead){
        head = inHead;
        next = nullptr;
    }

    void Append(Program* inProgram){
        this->next = inProgram;
    }

    std::string ToString() override {
        if(head == nullptr && next != nullptr){
            return next->ToString() + ";\n";
        }
        if(head != nullptr && next == nullptr){
            return head->ToString();
        } else {
            return head->ToString() + ";\n" + next->ToString();
        }
    }

    std::string ToPostfix() override {
        return head->ToPostfix() + "\n" + next->ToPostfix();
    }

    void GetVariables(std::vector<Association>& inVector) override {
        head->GetVariables(inVector);
        if(next != nullptr){
            next->GetVariables(inVector);
        }
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector){
        head->FLVMCodeGen(inVector);
        if(next != nullptr){
            inVector.push_back(Operation::pop);
            next->FLVMCodeGen(inVector);
        }
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
        return left->ToString() + " = " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "= ";
    }

    void GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
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

    void GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
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

    void GetVariables(std::vector<Association>& inVector) override {
        left->GetVariables(inVector);
        right->GetVariables(inVector);
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

    void GetVariables(std::vector<Association>& inVector) override {
        left->GetVariables(inVector);
        right->GetVariables(inVector);
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

    void GetVariables(std::vector<Association>& inVector) override {
        left->GetVariables(inVector);
        right->GetVariables(inVector);
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

    void GetVariables(std::vector<Association>& inVector){
        subexpression->GetVariables(inVector);
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

    void GetVariables(std::vector<Association>& inVector){
        right->GetVariables(inVector);
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector){
        right->FLVMCodeGen(inVector);
        inVector.push_back(Instruction(Operation::negate));
    }
};


class Variable : public Node{
public:
    std::string adjective;
    std::string name;

    Variable(){
        adjective = "";
        name = "";
    }

    Variable(std::string inAdjective, std::string inString){
        adjective = inAdjective;
        name = inString;
    }

    std::string ToString() override {
        return name;
    }

    std::string ToPostfix() override {
        return name;
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        inVector.push_back(Instruction(Operation::fetch));
    }

    void GetVariables(std::vector<Association>& inVector) override {
        //Do nothing.
    }

    Variable& operator=(Variable& right){
        adjective = right.adjective;
        name = right.name;

        return *this;
    }
};

class Initialize : public Node{
public:
    std::string adjective;
    std::string name;

    Initialize(){
        adjective = "";
        name = "";
    }

    Initialize(std::string inType, std::string inString){
        adjective = inType;
        name = inString;
    }

    std::string ToString() override {
        return name;
    }

    std::string ToPostfix() override {
        return name;
    }

    void FLVMCodeGen(std::vector<Instruction>& inVector) override {
        inVector.push_back(Instruction(Operation::varPush));
        inVector.push_back(Instruction(Operation::initialize));
    }

    void GetVariables(std::vector<Association>& inVector) override {
        types value;
        value.fixed64 = -1;
        size_t i;
        for(i = 0; i < inVector.size(); i++){
            if(inVector[i].token.name == name){
                //If the same variable name appears, throw an error.
                //Note: Figure out how to set up errors.
                //For now, just return to avoid adding again.
                return;
            }
        }
        //Ignore the right side, because I'm not doing chained initializations for now.
        inVector.push_back(Association(Variable(adjective, name), -1));
    }

    Initialize& operator=(Initialize& right){
        adjective = right.adjective;
        name = right.name;

        return *this;
    }
};

class Fixed64 : public Node{
public:
    int64_t value;

    Fixed64(std::string input){
        value = std::stoll(input);
    };

    std::string ToString() override {
        return std::to_string(value);
    }

    std::string ToPostfix() override {
        return std::to_string(value) + " ";
    }

    void GetVariables(std::vector<Association>& inVector) override {
        //Null
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