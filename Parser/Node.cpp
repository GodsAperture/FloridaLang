#include "Node.hpp"
#include <math.h>
#include <stack>
//Program
    Program::Program(Node* inHead){
        head = inHead;
        next = nullptr;
    }

    void Program::Append(Program* inProgram){
        this->next = inProgram;
    }

    std::string Program::ToString(){
        if(head == nullptr && next != nullptr){
            return next->ToString() + ";\n";
        }
        if(head != nullptr && next == nullptr){
            return head->ToString();
        } else {
            return head->ToString() + ";\n" + next->ToString();
        }
    }

    std::string Program::ToPostfix(){
        return head->ToPostfix() + "\n" + next->ToPostfix();
    }

    void Program::GetVariables(std::vector<Association>& inVector){
        head->GetVariables(inVector);
        if(next != nullptr){
            next->GetVariables(inVector);
        }
    }

    void Program::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        head->FLVMCodeGen(inInstructions, inVariables);
        if(next != nullptr){
            next->FLVMCodeGen(inInstructions, inVariables);
        }
    }



//Assignment
    Assignment::Assignment(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Assignment::ToString(){
        return left->ToString() + " = " + right->ToString();
    }

    std::string Assignment::ToPostfix(){
        return left->ToPostfix() + right->ToPostfix() + "= ";
    }

    void Assignment::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Assignment::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        right->FLVMCodeGen(inInstructions, inVariables);
        //For future reference, loosely typecasting is a no no.
        //This is not safe.
        std::string name = left->name;

        //Leave as -1 for future debugging purposes.
        int64_t position = -1;
        size_t i;
        for(i = 0; i < inVariables.size(); i++){
            if(name == inVariables[i].name){
                position = inVariables[i].position;
            }
        }

    inInstructions.push_back(Instruction(Operation::assign, position));
    }



//Add
    Add::Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Add::ToString(){
        return left->ToString() + " + " + right->ToString();
    }

    std::string Add::ToPostfix(){
        return left->ToPostfix() + right->ToPostfix() + "+ ";
    }

    void Add::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Add::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::add));
    }



//Subtract
    Subtract::Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Subtract::ToString(){
        return left->ToString() + " - " + right->ToString();
    }

    std::string Subtract::ToPostfix(){
        return left->ToPostfix() + right->ToPostfix() + "- ";
    }

    void Subtract::GetVariables(std::vector<Association>& inVector){    
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Subtract::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::subtract));
    }



//Multiply
    Multiply::Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Multiply::ToString(){
        return left->ToString() + " * " + right->ToString();
    }

    std::string Multiply::ToPostfix(){
        return left->ToPostfix() + right->ToPostfix() + "* ";
    } 

    void Multiply::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Multiply::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::multiply));
    }



//Divide
    Divide::Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Divide::ToString(){
        return left->ToString() + " / " + right->ToString();
    }

    std::string Divide::ToPostfix(){
        return left->ToPostfix() + right->ToPostfix() + "/ ";
    }

    void Divide::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Divide::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::divide));
    }



//Parentheses
    Parentheses::Parentheses(Node* input){
        subexpression = input;
    }

    std::string Parentheses::ToString(){
        return "(" + subexpression->ToString() + ")";
    }