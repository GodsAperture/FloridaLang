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
    virtual std::string ToPostfix() = 0;
    virtual void GetVariables(std::vector<Association>& inAssociation) = 0;
    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) = 0;

    virtual ~Node(){};
};



class Program : public Node {
public:
    Node* head;
    Program* next;

    Program(Node* inHead);
    void Append(Program* inProgram);
    std::string ToString() override;
    std::string ToPostfix() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
};

class Add : public Node{
public:
    Node* left;
    Node* right;

    Add(Node* LHE, Node* RHE);
    std::string ToString() override;
    std::string ToPostfix() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
};

class Subtract : public Node{
public:
    Node* left;
    Node* right;

    Subtract(Node* LHE, Node* RHE);
    std::string ToString() override;
    std::string ToPostfix() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
};



class Multiply : public Node {
public: 
    Node* left;
    Node* right;

    Multiply(Node* LHE, Node* RHE);
    std::string ToString() override;
    std::string ToPostfix() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
};

class Divide : public Node{
public: 
    Node* left;
    Node* right;

    Divide(Node* LHE, Node* RHE);
    std::string ToString() override;
    std::string ToPostfix() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
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

class Parentheses : public Node{
public:
    Node* subexpression;

    Parentheses(Node* input){
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

    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override {
        subexpression->FLVMCodeGen(inInstructions, inVariables);
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

    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::negate));
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

    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override {
        int position = -1;
        //Search for the variable in the symbols table.
        //Get its position and assign it to that variable.
        for(size_t i = 0; i < inVariables.size(); i++){
            if(inVariables[i].name == this->name){
                position = inVariables[i].position;
                break;
            }
        }
        inInstructions.push_back(Instruction(Operation::fetch, position));
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

class Assignment : public Node{
public:
    Node* left; 
    Node* right;

    Assignment(Node* inLeft, Node* inRight);
    std::string ToString() override;
    std::string ToPostfix() override;
    void GetVariables(std::vector<Association>& inVector) override;
    void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override;
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

    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override {
        inInstructions.push_back(Instruction(Operation::initialize));
    }

    void GetVariables(std::vector<Association>& inVector) override {
        for(size_t i = 0; i < inVector.size(); i++){
            if(inVector[i].name == name){
                //If the same variable name appears, throw an error.
                //Note: Figure out how to set up errors.
                //For now, just return to avoid adding again.
                return;
            }
        }
        //Ignore the right side, because I'm not doing chained initializations for now.
        inVector.push_back(Association(adjective, name, inVector.size()));
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

    virtual void FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables) override {
        inInstructions.push_back(Instruction(value));
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