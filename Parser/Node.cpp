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

    Scope::Scope(Body* inBody, Variable* inVariables, Scope* inParent){
        body = inBody;
        variables = inVariables;
        parent = inParent;
    }

    void Scope::push(Variable* input){
        //If there are no variables, then just slap the variable onto the list.
        if(variables == nullptr){
            variables = input;
            return;
        }
        Variable* currVar = variables;
        //Reach the tail end of the "linked list" of Variables.
        while(currVar->next != nullptr){
            currVar = currVar->next;
        }

        //Append the Variable to the tail end of the "linked list."
        currVar->next = input;

    }

    std::string Scope::ToString(std::string inLeft, std::string inRight){
        return body->ToString(inLeft, inRight);
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
            return current->ToString(inLeft, inRight) + "\n" + next->ToString(inLeft, inRight);
        }

        return current->ToString(inLeft, inRight) + "\n";
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
        next = nullptr;
    }

    Variable::Variable(Variable* inVariable){
        thisToken = inVariable->thisToken;
        distance = inVariable->distance;
        isLocal = inVariable->isLocal;
        next = inVariable->next;
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
        return inLeft + thisVariable->thisToken.getName() + " = " + code->ToString(inLeft, inRight) + ";";
    }

    void Assignment::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Generate code for the assignment.
        code->FLVMCodeGen(inInstructions);
        //Push back the instruction for assignment.
        if(thisVariable->isLocal){
            inInstructions.push_back(Instruction(lassign, thisVariable->distance));
        } else {
            inInstructions.push_back(Instruction(gassign, thisVariable->distance));
        }
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
    IfClass::IfClass(Node* inCondition, Scope* inIfScope, Scope* inElseScope){
        condition = inCondition;
        ifScope = inIfScope;
        elseScope = inElseScope;
    }

    std::string IfClass::ToString(std::string inLeft, std::string inRight){
        if(elseScope == nullptr){
            return inLeft + "if(" + condition->ToString(inLeft, inRight) + "){\n" + 
            ifScope->ToString("  ", inRight) +
            inLeft + "}";
        } else {
            return inLeft + "if(" + condition->ToString(inLeft, inRight) + "){\n" + 
            ifScope->ToString("  ", inRight) +
            inLeft + "} else {\n" + elseScope->ToString("  ", inRight) + "}";
        }
    }

    void IfClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //If there is not an else statement, generate code for just an if statement.
        //otherwise, generate both bodies and cjumps as needed.
        if(elseScope == nullptr){
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Mark where the conditional jump instruction will be.
            int64_t cjumpPosition = inInstructions.size();

            //Push a cjump instruction. This will be edited later.
            inInstructions.push_back(Instruction(cjump));
            ifScope->FLVMCodeGen(inInstructions);

            //Adjust the conditional jump destination.
            inInstructions[cjumpPosition].literal.fixed64 = inInstructions.size();
        } else {
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Get the size of the instruction stack before the body.
            //This is also where the cjump instruction will be.
            int64_t cjumpPosition = inInstructions.size();

            //Push a cjump instruction. This will be edited later.
            inInstructions.push_back(Instruction(cjump));
            ifScope->FLVMCodeGen(inInstructions);

            //Push an unconditional jump to skip over the else statement.
            //This will only be reached if the first branch is taken.
            inInstructions.push_back(Instruction(jump));

            //Get the size of the instructions set after the if body is made.
            int64_t endIfSize = inInstructions.size();

            //Adjust the conditional jump destination.
            inInstructions[cjumpPosition].literal.fixed64 = endIfSize;
            
            //Generate the bytecode for the else statement.
            elseScope->FLVMCodeGen(inInstructions);

            //Adjust the unconditional jump destination.
            inInstructions[endIfSize - 1].literal.fixed64 = inInstructions.size();
        }

    }



//for
    ForLoop::ForLoop(Node* inAssign, Node* inCondition, Node* inIncrementer, Scope* inBody){
        assign = inAssign;
        condition = inCondition;
        incrementer = inIncrementer;
        body = inBody;
    }

    std::string ForLoop::ToString(std::string inLeft, std::string inRight){
        std::string finalString = "for(";
        if(assign != nullptr){
            finalString += assign->ToString(inLeft, inRight);
        } else {
            finalString += ";";
        }

        if(condition != nullptr){
            finalString += condition->ToString(inLeft, inRight);
        }
        finalString += ";";

        if(incrementer != nullptr){
            finalString += incrementer->ToString(inLeft, inRight);
        }
        finalString += "){\n";

        return finalString + body->ToString("  ", inRight) + "}";
    }

    void ForLoop::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //The initial value of a for loop.
        if(assign != nullptr){
            assign->FLVMCodeGen(inInstructions);
        }

        //The condition for a for loop to terminate.
        //If none is provided, then it will perpetually loop.
        if(condition != nullptr){
            condition->FLVMCodeGen(inInstructions);
        } else {
            inInstructions.push_back(Instruction(Operation::push, false));
        }

        //Pushing the conditional loop that can break the condition.
        int64_t cjumpPosition = inInstructions.size();
        inInstructions.push_back(Instruction(Operation::cjump));

        //Add the body of instructions.
        body->FLVMCodeGen(inInstructions);

        //Add another line of code for changes at the end of the loop.
        if(incrementer != nullptr){
            incrementer->FLVMCodeGen(inInstructions);
        }

        inInstructions[cjumpPosition].literal.fixed64 = inInstructions.size();
    }
