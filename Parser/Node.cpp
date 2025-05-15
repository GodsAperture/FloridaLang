#include "Node.hpp"
#include <math.h>
#include <stack>

//Scope
    Scope::Scope(){
        //Do nothing lol
    }

    //Determine where a particular variable exists in a given scope.
    //If it is not found, then return -1.
    int64_t Scope::where(std::string input){
        //If it exists, then we'll find the position.
        for(size_t i = 0; i < variables.size(); i++){
            if(input == variables[i]){
                return i;
            }
        }
        //If there is no such member, return -1.
        return -1;

    }

    Scope::Scope(Body* inBody, Scope* inScope){
        body = inBody;
        parent = inScope;
    }

    void Scope::push(Variable* input){
        variables.push_back(input->thisToken.getName());
    }

    std::string Scope::ToString(std::string inString){
        return inString + "{" + body->ToString(inString) + "}";
    }

    void Scope::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        body->FLVMCodeGen(inInstructions);
    }



//Global scope
    Global::Global(Node* inCode){
        code = inCode;
        globalScope = nullptr;
    }

    std::string Global::ToString(std::string inString){
        return code->ToString(inString);
    }

    void Global::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        code->FLVMCodeGen(inInstructions);
    }



//Body
    Body::Body(Node* inCurrent, Body* inNext){
        current = inCurrent;
        next = inNext;
    }

    Body::Body(Node* inCurrent){
        current = inCurrent;
        next = nullptr;
    }

    std::string Body::ToString(std::string inString){
        if(next != nullptr){
            return inString + current->ToString(inString) + ";\n" + next->ToString(inString);
        }

        return inString + current->ToString(inString) + ";\n";
    }

    void Body::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Add the current chunk of code.
        current->FLVMCodeGen(inInstructions);
        //If next isn't a nullptr, then generate code for it too.
        if(next != nullptr){
            next->FLVMCodeGen(inInstructions);
        }
    }



//Variable
    Variable::Variable(Token inToken, int64_t inDistance){
        thisToken = inToken;
        distance = inDistance;
    }

    std::string Variable::ToString(std::string inString){
        return thisToken.getName();
    }

    void Variable::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Do nothing
    }



//Assignment
    Assignment::Assignment(Variable* inVariable, Node* inCode){
        thisVariable = inVariable;
        code = inCode;
    }

    std::string Assignment::ToString(std::string inString){
        return thisVariable->thisToken.getName() + " = " + code->ToString(inString);
    }

    void Assignment::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Generate code for the assignment.
        code->FLVMCodeGen(inInstructions);
        //Push back the instruction for assignment.
        inInstructions.push_back(Instruction(assign, thisVariable->distance));
    }


    
//Add +
    Add::Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Add::ToString(std::string inString){
        return left->ToString(inString) + " + " + right->ToString(inString);
    }

    void Add::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::add, -1));
    }



//Subtract -
    Subtract::Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Subtract::ToString(std::string inString){
        return left->ToString(inString) + " - " + right->ToString(inString);
    }

    void Subtract::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::subtract, -1));
    }



//Multiply *
    Multiply::Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Multiply::ToString(std::string inString){
        return left->ToString(inString) + " * " + right->ToString(inString);
    }

    void Multiply::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::multiply, -1));
    }



//Divide /
    Divide::Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Divide::ToString(std::string inString){
        return left->ToString(inString) + " / " + right->ToString(inString);
    }

    void Divide::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::divide, -1));
    }



//Parentheses ()
    Parentheses::Parentheses(Node* input){
        subexpression = input;
    }

    std::string Parentheses::ToString(std::string inString){
        return "(" + subexpression->ToString(inString) + ")";
    }

    void Parentheses::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        subexpression->FLVMCodeGen(inInstructions);
    }



//Negative -
    Negative::Negative(Node* input){
        right = input;
    }

    std::string Negative::ToString(std::string inString){
        return "-" + right->ToString(inString);
    }

    void Negative::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::negate, -1));
    }



//Equal ==
    Equal::Equal(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Equal::ToString(std::string inString){
        return left->ToString(inString) + " == " + right->ToString(inString);
    }

    void Equal::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for ==
        inInstructions.push_back(Instruction(equals));
    }



//Not equal !=
    NotEqual::NotEqual(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string NotEqual::ToString(std::string inString){
        return left->ToString(inString) + " != " + right->ToString(inString);
    }

    void NotEqual::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for !=
        inInstructions.push_back(Instruction(nequals));
    }



//Greater than >
    GreaterThan::GreaterThan(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string GreaterThan::ToString(std::string inString){
        return left->ToString(inString) + " > " + right->ToString(inString);
    }

    void GreaterThan::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >
        inInstructions.push_back(Instruction(greater));
    }



//Greater than or equal to >=
    GreaterThanOr::GreaterThanOr(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string GreaterThanOr::ToString(std::string inString){
        return left->ToString(inString) + " >= " + right->ToString(inString);
    }

    void GreaterThanOr::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >=
        inInstructions.push_back(Instruction(greateror));
    }



//Less than <
    LessThan::LessThan(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string LessThan::ToString(std::string inString){
        return left->ToString(inString) + " < " + right->ToString(inString);
    }

    void LessThan::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <
        inInstructions.push_back(Instruction(lesser));
    }



//Less than or equal to <=
    LessThanOr::LessThanOr(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string LessThanOr::ToString(std::string inString){
        return left->ToString(inString) + " <= " + right->ToString(inString);
    }

    void LessThanOr::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <=
        inInstructions.push_back(Instruction(lesseror));
    }



//Or OR
    Or::Or(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Or::ToString(std::string inString){
        return left->ToString(inString) + " OR " + right->ToString(inString);
    }

    void Or::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for OR
        inInstructions.push_back(Instruction(ior));
    }



//And AND
    And::And(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string And::ToString(std::string inString){
        return left->ToString(inString) + " AND " + right->ToString(inString);
    }

    void And::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for AND
        inInstructions.push_back(Instruction(iand));
    }



//Not !
    Not::Not(Node* inRight){
        right = inRight; 
    }

    std::string Not::ToString(std::string input){
        return "!" + right->ToString(input);
    }

    void Not::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(inot));
    }
