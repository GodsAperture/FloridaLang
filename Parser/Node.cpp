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

    std::string Program::ToString(std::string inString){
        if(head == nullptr && next != nullptr){
            return next->ToString(inString);
        }
        if(head != nullptr && next == nullptr){
            return head->ToString(inString);
        } else {
            return head->ToString(inString) + "\n" + next->ToString(inString);
        }
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

    int64_t Program::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Assignment
    Assignment::Assignment(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Assignment::ToString(std::string inString){
        return inString + left->ToString(inString) + " = " + right->ToString(inString) + ";";
    }

    void Assignment::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Assignment::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::assign, left->GetPosition(inVariables)));
    }

    int64_t Assignment::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Add
    Add::Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Add::ToString(std::string inString){
        return left->ToString(inString) + " + " + right->ToString(inString);
    }

    void Add::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Add::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::add, -1));
    }

    int64_t Add::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Subtract
    Subtract::Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Subtract::ToString(std::string inString){
        return left->ToString(inString) + " - " + right->ToString(inString);
    }

    void Subtract::GetVariables(std::vector<Association>& inVector){    
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Subtract::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::subtract, -1));
    }

    int64_t Subtract::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Multiply
    Multiply::Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Multiply::ToString(std::string inString){
        return left->ToString(inString) + " * " + right->ToString(inString);
    }

    void Multiply::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Multiply::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::multiply, -1));
    }

    int64_t Multiply::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Divide
    Divide::Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Divide::ToString(std::string inString){
        return left->ToString(inString) + " / " + right->ToString(inString);
    }

    void Divide::GetVariables(std::vector<Association>& inVector){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Divide::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::divide, -1));
    }

    int64_t Divide::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Parentheses
    Parentheses::Parentheses(Node* input){
        subexpression = input;
    }

    std::string Parentheses::ToString(std::string inString){
        return "(" + subexpression->ToString(inString) + ")";
    }

    void Parentheses::GetVariables(std::vector<Association>& inVector){
        subexpression->GetVariables(inVector);
    }

    void Parentheses::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        subexpression->FLVMCodeGen(inInstructions, inVariables);
    }

    int64_t Parentheses::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Negative
    Negative::Negative(Node* input){
        right = input;
    }

    std::string Negative::ToString(std::string inString){
        return "-" + right->ToString(inString);
    }

    void Negative::GetVariables(std::vector<Association>& inVector){
        right->GetVariables(inVector);
    }

    void Negative::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::negate, -1));
    }

    int64_t Negative::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Variable
    Variable::Variable(){
        name = "";
    }

    Variable::Variable(std::string inString){
        name = inString;
    }

    std::string Variable::ToString(std::string inString){
        return name;
    }

    void Variable::GetVariables(std::vector<Association>& inVector){
        //Do nothing.
    }

    void Variable::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
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

    int64_t Variable::GetPosition(std::vector<Association>& inVariables){
        //Search for the variable in the list.
        for(size_t i = 0; i < inVariables.size(); i++){
            if(inVariables[i].name == name){
                return inVariables[i].position;
            }
        }
        return -1;
    }



//Initialize
    Initialize::Initialize(){
        adjective = "";
        name = "";
    }

    Initialize::Initialize(std::string inType, std::string inString){
        adjective = inType;
        name = inString;
    }

    std::string Initialize::ToString(std::string inString){
        return adjective + " " + name;
    }

    void Initialize::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        //Null
    }

    void Initialize::GetVariables(std::vector<Association>& inVector){
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

    int64_t Initialize::GetPosition(std::vector<Association>& inVariables){
        //Search for the variable in the list.
        for(size_t i = 0; i < inVariables.size(); i++){
            if(inVariables[i].name == name){
                return inVariables[i].position;
            }
        }
        return -1;
    }



//If
    IfObject::IfObject(Node* inCondition, Node* inBody){
        condition = inCondition;
        body = inBody;
    }

    std::string IfObject::ToString(std::string inString){
        return "if(" + condition->ToString(inString) + "){\n" + body->ToString("\t" + inString) + "\n}";
    }

    void IfObject::GetVariables(std::vector<Association>& inVector){
        //Do nothing.
    }

    void IfObject::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        size_t start;
        size_t end;
        
        condition->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(cjump, -1));
        //Find the start of the if statement in the instruction set.
        start = inInstructions.size();
        
        body->FLVMCodeGen(inInstructions, inVariables);
        //Find the end of the if statement in the instruction set.
        end = inInstructions.size();

        //Adjust the size of the body, so that I can jump to
        //the correct place in the instruction set.
        //Subtract one because the VM will increment.
        inInstructions[start - 1] = Instruction(cjump, end);
    }

    int64_t IfObject::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//For
    ForLoop::ForLoop(Node* inInitialization, Node* inCondition, Node* inIncrement, Node* inBody){
        initialization = inInitialization;
        condition = inCondition;
        increment = inIncrement;
        body = inBody;
    }

    std::string ForLoop::ToString(std::string inString){
        std::string part1 = "";
        std::string part2 = "";
        std::string part3 = "";

        if(initialization != nullptr){
            part1 = initialization->ToString(inString);
        }

        if(condition != nullptr){
            part2 = " " + condition->ToString(inString);
        }

        if(increment != nullptr){
            part3 = " " + increment->ToString(inString);
        }

        return inString + "for(" + part1 + ";" + part2 + ";" + part3 + "){\n" + inString + body->ToString("\t" + inString) + "\n" + inString + "}";
    }

    void ForLoop::GetVariables(std::vector<Association>& inVector){
        initialization->GetVariables(inVector);
        body->GetVariables(inVector);
    }

    void ForLoop::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        size_t start = 0;
        
        if(initialization != nullptr){
            initialization->FLVMCodeGen(inInstructions, inVariables);
        }

        //This is the beginning of the for loop.
        start = inInstructions.size();

        if(increment != nullptr){
            increment->FLVMCodeGen(inInstructions, inVariables);
        }

        //If there's not a condition, then append true.
        if(condition != nullptr){
            condition->FLVMCodeGen(inInstructions, inVariables);
            inInstructions.push_back(Instruction(Operation::cjump, 0));
        }

        //Generate the code for the body of the for loop.
        body->FLVMCodeGen(inInstructions, inVariables);

        //This is the end of the for loop.
        size_t end = inInstructions.size();

        inInstructions.push_back(Instruction(Operation::jump, start));

    }

    int64_t ForLoop::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Goto
    Goto::Goto(std::string inName){
        name = inName;
    }

    std::string Goto::ToString(std::string inString){
        return inString + name + ": ";
    }

    void Goto::GetVariables(std::vector<Association>& inVector){
        //Do nothing
    }

    void Goto::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        int64_t position = -1;
        //Find the variable in the symbols table and adjust its position.
        for(size_t i = 0; i < inVariables.size(); i++){
            if(inVariables[i].name == name){
                position = inVariables[i].position;
                break;
            }
        }

        //Create an instruction that goes to a specific place in the symbols table.
        inInstructions.push_back(Instruction(Operation::jump, position));
    }

    int64_t Goto::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Cgoto
    Cgoto::Cgoto(Node* inCondition, Node* inBody){
        condition = inCondition;
        body = inBody;
    }

    std::string Cgoto::ToString(std::string inString){
        return "";
    }

    void Cgoto::GetVariables(std::vector<Association>& inVector){
        //Do nothing.
    }

    void Cgoto::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        condition->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::cjump, -1));
        body->FLVMCodeGen(inInstructions, inVariables);
    }

    int64_t Cgoto::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Landing
    Landing::Landing(std::string inName){
        name = inName;
    }

    std::string Landing::ToString(std::string inString){
        return inString + name + ":";
    }

    void Landing::GetVariables(std::vector<Association>& inVector){
        inVector.push_back(Association("Null", name, -1));
    }

    void Landing::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        int64_t position = -1;
        //
        for(size_t i = 0; i < inVariables.size(); i++){
            if(name == inVariables[i].name){
                position = i;
                break;
            }
        }
        //If the position is not found, an error has occured.
        if(position == -1){
            printf("Error: A goto landing point wasn't registered.\n");
            inInstructions.clear();
            return;
        }

        //Adjust the landing point in the symbols table.
        inVariables[position].position = inInstructions.size(); 

    }

    int64_t Landing::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }