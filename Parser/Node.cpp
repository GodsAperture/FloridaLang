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
            variableCount++;
            return;
        }
        Variable* currVar = variables;
        //Reach the tail end of the "linked list" of Variables.
        while(currVar->next != nullptr){
            currVar = currVar->next;
        }

        //Append the Variable to the tail end of the "linked list."
        currVar->next = input;
        //Adjust the variable count appropriately.
        variableCount++;

    }

    size_t Scope::count(){
        size_t count = 0;
        Variable* currVar = variables;
        while(currVar != nullptr){
            count++;
            currVar = currVar->next;
        }

        return count;
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
        code = nullptr;
    }

    Initialize::Initialize(Variable* inVariable, Node* inCode){
        thisVariable = inVariable;
        code = inCode;
    }

    std::string Initialize::ToString(std::string inLeft, std::string inRight){
        if(code == nullptr){
            return inLeft + thisVariable->thisToken.getName();
        } else {
            return inLeft + thisVariable->thisToken.getName() + " = " + code->ToString(inLeft, inRight);
        }
    }

    void Initialize::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Push back a placeholder.
        inInstructions.push_back(Instruction(Operation::initialize, 0));
        //Generate the code for the assignment, if any.
        if(code != nullptr){
            code->FLVMCodeGen(inInstructions);
            if(thisVariable->isLocal){
                inInstructions.push_back(Instruction(Operation::lassign, thisVariable->distance));
            } else {
                inInstructions.push_back(Instruction(Operation::gassign, thisVariable->distance));
            }
        }

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
    IfClass::IfClass(Node* inCondition, Body* inIfBody, Body* inElseBody){
        condition = inCondition;
        ifBody = inIfBody;
        elseBody = inElseBody;
    }

    std::string IfClass::ToString(std::string inLeft, std::string inRight){
        if(elseBody == nullptr){
            return inLeft + "if(" + condition->ToString(inLeft, inRight) + "){\n" + 
            ifBody->ToString("  " + inLeft, inRight) +
            inLeft + "}";
        } else {
            return inLeft + "if(" + condition->ToString(inLeft, inRight) + "){\n" + 
            ifBody->ToString("  ", inRight) +
            inLeft + "} else {\n" + elseBody->ToString("  " + inLeft, inRight) + inLeft + "}";
        }
    }

    void IfClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //If there is not an else statement, generate code for just an if statement.
        //otherwise, generate both bodies and cjumps as needed.
        if(elseBody == nullptr){
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Mark where the conditional jump instruction will be.
            int64_t cjumpPosition = inInstructions.size();

            //Push a cjump instruction. This will be edited later.
            inInstructions.push_back(Instruction(Operation::cjump));
            ifBody->FLVMCodeGen(inInstructions);

            //Place a pop instruction for each local variable.
            for(size_t i = 0; i < ifVarCount; i++){
                inInstructions.push_back(Instruction(Operation::pop));
            }

            //Adjust the conditional jump destination.
            inInstructions[cjumpPosition].literal.fixed64 = inInstructions.size();
        } else {
            //Generate the instructions for the if condition.
            condition->FLVMCodeGen(inInstructions);

            //Get the size of the instruction stack before the body.
            //This is also where the cjump instruction will be.
            int64_t cjumpPosition = inInstructions.size();

            //Push a cjump instruction. This will be edited later.
            inInstructions.push_back(Instruction(Operation::cjump));
            ifBody->FLVMCodeGen(inInstructions);

            //Place a pop instruction for each local variable.
            for(size_t i = 0; i < ifVarCount; i++){
                inInstructions.push_back(Instruction(Operation::pop));
            }

            //Push an unconditional jump to skip over the else statement.
            //This will only be reached if the first branch is taken.
            inInstructions.push_back(Instruction(Operation::jump));

            //Get the size of the instructions set after the if body is made.
            int64_t endIfSize = inInstructions.size();

            //Adjust the conditional jump destination.
            inInstructions[cjumpPosition].literal.fixed64 = endIfSize;
            
            //Generate the bytecode for the else statement.
            elseBody->FLVMCodeGen(inInstructions);

            //Place a pop instruction for each variable in the elseBody.
            for(size_t i = 0; i < elseVarCount; i++){
                inInstructions.push_back(Instruction(Operation::pop));
            }

            //Adjust the unconditional jump destination.
            inInstructions[endIfSize - 1].literal.fixed64 = inInstructions.size();
        }

    }



//for
    ForLoop::ForLoop(Node* inAssign, Node* inCondition, Node* inIncrementer, Body* inBody){
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

        //This will be where the end of the loop will unconditionally jump to.
        int64_t jumpTo = inInstructions.size();

        //The condition for a for loop to terminate.
        //If none is provided, then it will perpetually loop.
        if(condition != nullptr){
            condition->FLVMCodeGen(inInstructions);
        } else {
            inInstructions.push_back(Instruction(Operation::push, false));
        }

        //Pushing the conditional jump that can break the loop.
        int64_t cjumpPosition = inInstructions.size();
        inInstructions.push_back(Instruction(Operation::cjump));

        //Add the body of instructions.
        body->FLVMCodeGen(inInstructions);

        //Add another line of code for changes at the end of the loop.
        if(incrementer != nullptr){
            incrementer->FLVMCodeGen(inInstructions);
        }

        //Place the unconditional jump instruction at the end.
        inInstructions.push_back(Instruction(jump, jumpTo));
        inInstructions[cjumpPosition].literal.fixed64 = inInstructions.size();

        //Place a pop instruction if there was an initialization.
        if(assign != nullptr){
            inInstructions.push_back(Instruction(Operation::pop));
        }
    }



//while
    WhileLoop::WhileLoop(Node* inCondition, Body* inBody){
        condition = inCondition;
        body = inBody;
    }

    std::string WhileLoop::ToString(std::string inLeft, std::string inRight){
        return inLeft + "while(" + condition->ToString(inLeft, inRight) + "){\n" + body->ToString("  " + inLeft, inRight) + inLeft + "}";
    }

    void WhileLoop::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Where the unconditional jump will always land.
        size_t start = inInstructions.size();
        //Generate the code for the condition.
        condition->FLVMCodeGen(inInstructions);
        //The location of the conditional jump statement.
        size_t here = inInstructions.size();
        //Lay down the conditional jump.
        inInstructions.push_back(Instruction(Operation::cjump));
        //Generate the code for the body.
        body->FLVMCodeGen(inInstructions);
        //Place an unconditional jump to restart the loop.
        inInstructions.push_back(Instruction(Operation::jump, start));
        //Adjust where the unconditional jump will land.
        inInstructions[here].literal.fixed64 = inInstructions.size();
    }