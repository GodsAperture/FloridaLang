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

    void Program::GetVariables(Scope& inScope){
        head->GetVariables(inScope);
        if(next != nullptr){
            next->GetVariables(inScope);
        }
    }

    void Program::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        head->FLVMCodeGen(inInstructions);
        if(next != nullptr){
            next->FLVMCodeGen(inInstructions);
        }
    }

    int64_t Program::GetPosition(std::vector<Scope>& inScopes){
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

    void Assignment::GetVariables(Scope& inScope){
        left->GetVariables(inScope);
        right->GetVariables(inScope);
    }

    void Assignment::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::assign, left->GetPosition()));
    }

    int64_t Assignment::GetPosition(std::vector<Scope>& inScopes){
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

    void Add::GetVariables(Scope& inScope){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Add::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::add, -1));
    }

    int64_t Add::GetPosition(std::vector<Scope>& inScopes){
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

    void Subtract::GetVariables(Scope& inScope){    
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Subtract::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::subtract, -1));
    }

    int64_t Subtract::GetPosition(std::vector<Scope>& inScopes){
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

    void Multiply::GetVariables(Scope& inScope){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Multiply::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::multiply, -1));
    }

    int64_t Multiply::GetPosition(std::vector<Scope>& inScopes){
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

    void Divide::GetVariables(Scope& inScope){
        left->GetVariables(inVector);
        right->GetVariables(inVector);
    }

    void Divide::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions, inVariables);
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::divide, -1));
    }

    int64_t Divide::GetPosition(std::vector<Scope>& inScopes){
        return -1;
    }



//Parentheses
    Parentheses::Parentheses(Node* input){
        subexpression = input;
    }

    std::string Parentheses::ToString(std::string inString){
        return "(" + subexpression->ToString(inString) + ")";
    }

    void Parentheses::GetVariables(Scope& inScope){
        subexpression->GetVariables(inVector);
    }

    void Parentheses::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        subexpression->FLVMCodeGen(inInstructions, inVariables);
    }

    int64_t Parentheses::GetPosition(std::vector<Scope>& inScopes){
        return -1;
    }



//Negative
    Negative::Negative(Node* input){
        right = input;
    }

    std::string Negative::ToString(std::string inString){
        return "-" + right->ToString(inString);
    }

    void Negative::GetVariables(Scope& inScope){
        right->GetVariables(inVector);
    }

    void Negative::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::negate, -1));
    }

    int64_t Negative::GetPosition(std::vector<Scope>& inScopes){
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

    void Variable::GetVariables(Scope& inScope){
        //Do nothing.
    }

    void Variable::FLVMCodeGen(std::vector<Instruction>& inInstructions){
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

    int64_t Variable::GetPosition(std::vector<Scope>& inScopes){
        //Search for the variable in the list.
        //Work from the current scope outward.
        for(size_t i = inScopes.size() - 1; i >= 0; i--){
            for(size_t j = 0; j < inScopes[i].size(); j++){
                //Find the name of the particular variable in the scope
                if(inScopes[i].objects[j].name == name){
                    return inScopes[i].objects[j].position;
                }
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

    void Initialize::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Null
    }

    void Initialize::GetVariables(Scope& inScope){
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

    int64_t Variable::GetPosition(std::vector<Scope>& inScopes){
        //Search for the variable in the list.
        //Work from the current scope outward.
        for(size_t i = inScopes.size() - 1; i >= 0; i--){
            for(size_t j = 0; j < inScopes[i].size(); j++){
                //Find the name of the particular variable in the scope
                if(inScopes[i].objects[j].name == name){
                    return inScopes[i].objects[j].position;
                }
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

    void IfObject::GetVariables(Scope& inScope){
        //Do nothing.
    }

    void IfObject::FLVMCodeGen(std::vector<Instruction>& inInstructions){
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

    int64_t IfObject::GetPosition(std::vector<Scope>& inScopes){
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

    void ForLoop::GetVariables(Scope& inScope){
        initialization->GetVariables(inVector);
        body->GetVariables(inVector);
    }

    void ForLoop::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        size_t start = 0;
        size_t cjumpPoint = 0;
        
        //Initialize the variable, if there is one.
        if(initialization != nullptr){
            initialization->FLVMCodeGen(inInstructions, inVariables);
        }

        //This is the beginning of the for loop.
        if(inInstructions.size() == 0){
            start = inInstructions.size();
        } else {
            start = inInstructions.size() - 1;
        }

        //Check for a condition.
        if(condition != nullptr){
            condition->FLVMCodeGen(inInstructions, inVariables);
            //Push the instruction for cjump
            inInstructions.push_back(Instruction(Operation::cjump, 0));
            //Get the position of the cjump.
            cjumpPoint = inInstructions.size();
        }

        //The increment always occurs after the body executes.
        if(increment != nullptr){
            increment->FLVMCodeGen(inInstructions, inVariables);
        }

        //Generate the code for the body of the for loop.
        body->FLVMCodeGen(inInstructions, inVariables);

        //This is the end of the for loop.
        size_t end = inInstructions.size();

        //Adjust the location where the cjump goes to.
        inInstructions[cjumpPoint - 1].literal.fixed64 = end;
        inInstructions.push_back(Instruction(Operation::jump, start));

    }

    int64_t ForLoop::GetPosition(std::vector<Scope>& inScopes){
        return -1;
    }



//Function
    Function::Function(std::string inType, std::string inName, std::string inArguments, Node* inBody){
        type = inType;
        name = inName;
        arguments = inArguments;
        body = inBody;
    }

    std::string Function::ToString(std::string inString){

        std::string argumentList = "(";

        //arguments.size() is cut in half because half of it
        //is the variable name, the other half is variable types.
        //The minus one is so I don't add an unnecessary comma.
        for(size_t i = 0; i < arguments.size() / 2 - 1; i++){
            argumentList += arguments[2 * i + 1] + ", ";
        }

        //If there is a nonzero amount of arguments then
        //I will make sure to have one less comma than 
        //the number of arguments.
        if(arguments.size() != 0){
            //Add the last variable in the list to the function
            //without an extra commma.
            argumentList += arguments[arguments.size() - 1];
        }

        argumentList += ")";

        return inString + name + argumentList;
    }

    void Function::GetVariables(Scope& inScope){
        body->GetVariables(inVector);
    }

    void Function::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        
    }

    int64_t Function::GetPosition(std::vector<Scope>& inScopes){
        return -1;
    }


//Goto
    Goto::Goto(std::string inName){
        name = inName;
    }

    std::string Goto::ToString(std::string inString){
        return inString + name + ": ";
    }

    void Goto::GetVariables(Scope& inScope){
        //Do nothing
    }

    void Goto::FLVMCodeGen(std::vector<Instruction>& inInstructions){
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

    int64_t Goto::GetPosition(std::vector<Scope>& inScopes){
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

    void Cgoto::GetVariables(Scope& inScope){
        //Do nothing.
    }

    void Cgoto::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        condition->FLVMCodeGen(inInstructions, inVariables);
        inInstructions.push_back(Instruction(Operation::cjump, -1));
        body->FLVMCodeGen(inInstructions, inVariables);
    }

    int64_t Cgoto::GetPosition(std::vector<Scope>& inScopes){
        return -1;
    }



//Landing
    Landing::Landing(std::string inName){
        name = inName;
    }

    std::string Landing::ToString(std::string inString){
        return inString + name + ":";
    }

    void Landing::GetVariables(Scope& inScope){
        inVector.push_back(Association("Null", name, -1));
    }

    void Landing::FLVMCodeGen(std::vector<Instruction>& inInstructions){
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

    int64_t Landing::GetPosition(std::vector<Scope>& inScopes){
        return -1;
    }