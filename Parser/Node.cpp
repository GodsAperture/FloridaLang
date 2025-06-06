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
        Variable* currVar = variables;
        int64_t count = 0;
        //If it exists, then we'll find the position.
        while(currVar != nullptr){
            if(currVar->thisToken.getName() == input){
                return count;
            } else {
                count++;
                currVar = currVar->next;
            }
        }
        //If there is no such member, return -1.
        return -1;

    }

    Scope::Scope(Body* inBody, Variable* inVariables){
        body = inBody;
        variables = inVariables;
    }

    void Scope::push(Variable* input){
        Variable* currVar = variables;
        //Reach the tail end of the "linked list" of Variables.
        while(currVar->next != nullptr){
            currVar = currVar->next;
        }

        //Append the Variable to the tail end of the "linked list."
        currVar->next = input;

    }

    std::string Scope::ToString(std::string inLeft, std::string inRight){
            return body->ToString(inLeft, inRight) + "\n";
    }

    void Scope::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        body->FLVMCodeGen(inInstructions);
    }



//Global scope
    Global::Global(Node* inCode){
        code = inCode;
        globalScope = nullptr;
    }

    std::string Global::ToString(std::string inLeft, std::string inRight){
        return code->ToString(inLeft, inRight);
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

    std::string Body::ToString(std::string inLeft, std::string inRight){
        if(next != nullptr){
                return inLeft + current->ToString(inLeft, inRight) + "\n" + next->ToString(inLeft, inRight);
        }

        return inLeft + current->ToString(inLeft, inRight) + "\n";
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
    Variable::Variable(Token inToken, int64_t inDistance, bool inIsLocal){
        thisToken = inToken;
        distance = inDistance;
        isLocal = inIsLocal;
    }

    Variable::Variable(Variable* inVariable){
        thisToken = inVariable->thisToken;
        distance = inVariable->distance;
        isLocal = inVariable->isLocal;
    }

    std::string Variable::ToString(std::string inLeft, std::string inRight){
        return thisToken.getName();
    }

    void Variable::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        if(isLocal){
            //Push back the instruction for local variable.
            inInstructions.push_back(Instruction(lfetch, distance));
        } else {
            //Push back the instruction for the global variable.
            inInstructions.push_back(Instruction(gfetch, distance));
        }
    }



//Initialize
    Initialize::Initialize(Variable* inVariable){
        thisVariable = inVariable;
    }

    std::string Initialize::ToString(std::string inLeft, std::string inRight){
        return thisVariable->thisToken.getName();
    }

    void Initialize::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Do nothing
    }



//Assignment
    Assignment::Assignment(Variable* inVariable, Node* inCode){
        thisVariable = inVariable;
        code = inCode;
    }

    std::string Assignment::ToString(std::string inLeft, std::string inRight){
        return inLeft + thisVariable->thisToken.getName() + " = " + code->ToString(inLeft, inRight);
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

    std::string Add::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " + " + right->ToString(inLeft, inRight);
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

    std::string Subtract::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " - " + right->ToString(inLeft, inRight);
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

    std::string Multiply::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " * " + right->ToString(inLeft, inRight);
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

    std::string Divide::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " / " + right->ToString(inLeft, inRight);
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

    std::string Parentheses::ToString(std::string inLeft, std::string inRight){
        return "(" + subexpression->ToString(inLeft, inRight) + ")";
    }

    void Parentheses::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        subexpression->FLVMCodeGen(inInstructions);
    }



//Negative -
    Negative::Negative(Node* input){
        right = input;
    }

    std::string Negative::ToString(std::string inLeft, std::string inRight){
        return "-" + right->ToString(inLeft, inRight);
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

    std::string Equal::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " == " + right->ToString(inLeft, inRight);
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

    std::string NotEqual::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " != " + right->ToString(inLeft, inRight);
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

    std::string GreaterThan::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " > " + right->ToString(inLeft, inRight);
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

    std::string GreaterThanOr::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " >= " + right->ToString(inLeft, inRight);
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

    std::string LessThan::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " < " + right->ToString(inLeft, inRight);
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

    std::string LessThanOr::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " <= " + right->ToString(inLeft, inRight);
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

    std::string Or::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " OR " + right->ToString(inLeft, inRight);
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

    std::string And::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " AND " + right->ToString(inLeft, inRight);
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

    std::string Not::ToString(std::string inLeft, std::string inRight){
        return "!" + right->ToString(inLeft, inRight);
    }

    void Not::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(inot));
    }



//if
    IfClass::IfClass(Node* inCondition, Scope* inScope){
        condition = inCondition;
        scope = inScope;
    }

    std::string IfClass::ToString(std::string inLeft, std::string inRight){
        return inLeft + "if(" + condition->ToString(inLeft, inRight) + ")\n{" + 
        scope->ToString("\t" + inLeft, inRight) +
        inLeft + "}";
    }

    void IfClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        condition->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(cjump));
        scope->FLVMCodeGen(inInstructions);
    }