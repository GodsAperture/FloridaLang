#include "Node.hpp"
#include <math.h>
#include <stack>
#include <iostream>

inline std::string padding(std::string input){
    for(size_t i = input.size(); i < 12; i++){
        input += " ";
    }

    return input;
}

inline std::string padding2(std::string input){
    for(size_t i = input.size(); i < 16; i++){
        input += " ";
    }

    return input;
}

std::string typeString(FloridaType input){
    switch(input){
    //Boolean
        case FloridaType::Bool:
            return "boolean";
    //Fixed point numbers
        case FloridaType::fixed1:
            return "fixed1";
        case FloridaType::fixed2:
            return "fixed2";
        case FloridaType::fixed4:
            return "fixed4";
        case FloridaType::fixed8:
            return "fixed8";
        case FloridaType::fixedn:
            return "fixedn";
    //Floating point numbers
        case FloridaType::float4:
            return "float4";
        case FloridaType::float8:
            return "float8";
        case FloridaType::floatn:
            return "floatn";
        default:
            return "NULLERROR";
    }

    //This is unreachable, but appeases the compiler.
    return "NULLERROR";
}

FloridaType typeReturn(std::string inString){
    //Void
    if(inString == "void"){
        return FloridaType::tvoid;
    }
    //Boolean
    if(inString == "boolean"){
        return FloridaType::Bool;
    }
    //Fixed point numbers.
    if(inString == "fixed1"){
        return FloridaType::fixed1;
    }
    if(inString == "fixed2"){
        return FloridaType::fixed2; 
    }
    if(inString == "fixed4"){
        return FloridaType::fixed4;
    }
    if(inString == "fixed8"){
        return FloridaType::fixed8;
    }
    if(inString == "fixedn"){
        return FloridaType::fixedn;
    }
    //Floating point numbers.
    if(inString == "float4"){
        return FloridaType::float4;
    }
    if(inString == "float8"){
        return FloridaType::float8;
    }
    if(inString == "floatn"){
        return FloridaType::floatn;
    }
    return FloridaType::Null;
}

bool typeCheck(FloridaType inType){
    switch(inType){
        case FloridaType::fixed1:
        case FloridaType::fixed2:
        case FloridaType::fixed4:
        case FloridaType::fixed8:
        case FloridaType::Bool:
            return true;
        default:
            return false;

    }
}


//Scope
    Scope::Scope(){
        //Do nothing lol
    }

    //Determine where a particular variable exists in a given scope.
    //If it is not found, then return -1.
    int64_t Scope::varWhere(std::string input){
        Variable* currVar = variables;
        int64_t count = 0;
        int64_t result = -1;
        //If it exists, then we'll find the position.
        while(currVar != nullptr){
            if(currVar->thisToken.getName() == input){
                result = count;
                currVar = currVar->next;
            } else {
                count++;
                currVar = currVar->next;
            }
        }
        //If there is no such member, return -1.
        return result;
    }

    //Determine if a particular function exists in a given scope.
    //If it is found, then return its pointer.
    //Otherwise, return the nullptr.
    Function* Scope::funGet(std::string input){
        Function* currFun = functions;
        //If it exists, then we'll find the function.
        while(currFun != nullptr){
            if(currFun->name == input){
                return currFun;
            }
            currFun = currFun->next;
        }
        //If there is no such member, return the nullptr.
        return nullptr;
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

    void Scope::push(Function* input){
        //If there are no functions, then just slap the function onto the list.
        if(functions == nullptr){
            functions = input;
            return;
        }
        Function* currFun = functions;
        //Reach the tail end of the "linked list" of functions.
        while(currFun->next != nullptr){
            currFun = currFun->next;
        }

        //Append the function to the tail end of the "linked list."
        currFun->next = input;

    }

    //pop will remove variables that are going out of scope.
    void Scope::varPop(){
        Variable* currVar = variables;
        Variable* lastVar = nullptr;
        if(currVar != nullptr){
            while(currVar->next != nullptr){
                lastVar = currVar;
                currVar = currVar->next;
            }
            lastVar->next = nullptr;
        }
    }

    size_t Scope::varCount(){
        size_t count = 0;
        Variable* currVar = variables;
        while(currVar != nullptr){
            count++;
            currVar = currVar->next;
        }

        return count;
    }

    size_t Scope::funCount(){
        size_t count = 0;
        Function* currFun = functions;
        while(currFun != nullptr){
            count++;
            currFun = currFun->next;
        }

        return count;
    }

    std::string Scope::ToString(std::string inLeft, std::string inRight){
        if((functions == nullptr) && (body == nullptr)){
            return "";
        }
        if(functions != nullptr){
            return functions->ToString(inLeft, inRight) + body->ToString(inLeft, ";");
        }
        return body->ToString(inLeft, ";");
    }

    std::string Scope::printAll(){
        return body->printAll();
    }

    void Scope::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        if(body != nullptr){
            body->FLVMCodeGen(inInstructions);
        }
    }

    Node* Scope::copy(StackAllocator& input){
        Scope* thisptr = input.alloc<Scope>();
        //If this is the first scope, then it's global and current.
        if(input.currScope == nullptr){
            input.globalScope = thisptr;
            input.currScope = thisptr;
        } else {
        //Otherwise, the currScope is now the parent
        //and change thisptr to be the currScope.
            thisptr->parent = input.currScope;
            input.currScope = thisptr;
        }
        //Don't directly copy the variables, functions, or classes.
        //Add them as you find them to avoid future issues.
        if(body != nullptr){
            thisptr->body = (Body*) body->copy(input);
        }

        return thisptr;
    }

    Scope* Scope::pcopy(StackAllocator& input){
        Scope* thisptr = input.alloc<Scope>();
        //If this is the first scope, then it's global and current.
        if(input.currScope == nullptr){
            input.globalScope = thisptr;
            input.currScope = thisptr;
        } else {
        //Otherwise, the currScope is now the parent
        //and change thisptr to be the currScope.
            thisptr->parent = input.currScope;
            input.currScope = thisptr;
        }
        //Don't directly copy the variables, functions, or classes.
        //Add them as you find them to avoid future issues.
        if(body != nullptr){
            thisptr->body = (Body*) body->copy(input);
        }

        return thisptr;
    }




//Body
    Body::Body(){
        current = nullptr;
        next = nullptr;
    }

    Body::Body(Node* input){
        current = input;
        next = nullptr;
    }

    //Append the body to the end of the linked list.
    Body* Body::append(Body* input){
        if(current == nullptr){
            current = input;
            return this;
        }

        Body* currBody = this;
        while(currBody->next != nullptr){
            currBody = currBody->next;
        }

        currBody->next = input;
        return this;

    }

    std::string Body::ToString(std::string inLeft, std::string inRight){
        if(next != nullptr){
            return current->ToString(inLeft, inRight) + "\n" + next->ToString(inLeft, inRight);
        }

        return current->ToString(inLeft, inRight);
    }

    std::string Body::printAll(){
        if(next != nullptr){
            return current->printAll() + next->printAll();
        } else {
            return current->printAll();
        }
    }

    void Body::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Add the current chunk of code.
        current->FLVMCodeGen(inInstructions);
        //If next isn't a nullptr, then generate code for it too.
        if(next != nullptr){
            next->FLVMCodeGen(inInstructions);
        }
    }

    Node* Body::copy(StackAllocator& input){
        Body* thisptr = input.alloc<Body>();
        if(next == nullptr){
            thisptr->current = current->copy(input);
        } else {
            thisptr->current = current->copy(input);
            thisptr->next = (Body*) next->copy(input);
        }

        return thisptr;
    }

    Body* Body::pcopy(StackAllocator& input){
                Body* thisptr = input.alloc<Body>();
        if(next == nullptr){
            thisptr->current = current->copy(input);
        } else {
            thisptr->current = current->copy(input);
            thisptr->next = (Body*) next->copy(input);
        }

        return thisptr;
    }


//Variable
    Variable::Variable(){
        //Do nothing. It's not a problem.
    }

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

    void Variable::append(Variable* input){
        Variable* currVar = this;
        if(currVar->next != nullptr){
            while(currVar->next != nullptr){
                currVar = currVar->next;
            }
        }

        currVar->next = input;

    }

    std::string Variable::ToString(std::string inLeft, std::string inRight){
        return thisToken.getName();
    }

    std::string Variable::printAll(){
        if(isLocal){
            return padding2(padding("lfetch") + std::to_string(distance)) + "(*Variable " + thisToken.getName() + "*)\n";
        } else {
            return padding2(padding("gfetch") + std::to_string(distance)) + "(*Variable " + thisToken.getName() + "*)\n";
        }
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

    Node* Variable::copy(StackAllocator& input){
        Variable* thisptr = input.alloc<Variable>();

        thisptr->thisToken = thisToken;
        thisptr->isLocal = isLocal;
        thisptr->distance = distance;
        thisptr->value = value;
        thisptr->next = next->pcopy(input);
        thisptr->type = type;

        return thisptr;
    }

    Variable* Variable::pcopy(StackAllocator& input){
        Variable* thisptr = input.alloc<Variable>();

        thisptr->thisToken = thisToken;
        thisptr->isLocal = isLocal;
        thisptr->distance = distance;
        thisptr->value = value;
        thisptr->next = next->pcopy(input);
        thisptr->type = type;

        return thisptr;
    }



//Initialize
    Initialize::Initialize(){
        //Do nothing.
    };

    Initialize::Initialize(Variable* inVariable){
        thisVariable = inVariable;
    }

    std::string Initialize::ToString(std::string inLeft, std::string inRight){
        return inLeft + typeString(thisVariable->thisToken.type) + " " + thisVariable->thisToken.getName() + inRight;
    }

    std::string Initialize::printAll(){
        return padding2("initialize") + "(*Variable " + thisVariable->thisToken.getName() + "*)\n";
    }

    void Initialize::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Push back a placeholder.
        inInstructions.push_back(Instruction(Operation::initialize, 0));
    }

    Node* Initialize::copy(StackAllocator& input){
        Initialize* thisptr = input.alloc<Initialize>();

        thisptr->thisVariable = thisVariable->pcopy(input);

        return thisptr;
    }

    Initialize* Initialize::pcopy(StackAllocator& input){
        Initialize* thisptr = input.alloc<Initialize>();

        thisptr->thisVariable = thisVariable->pcopy(input);

        return thisptr;
    }



//InitializeAssign
    InitializeAssign::InitializeAssign(){
        //Do nothing
    }

    InitializeAssign::InitializeAssign(Variable* inVariable, Node* inBody){
        thisVariable = inVariable;
        code = inBody;
    }

    std::string InitializeAssign::ToString(std::string inLeft, std::string inRight){
        return inLeft + "\x1b[36m" + typeString(thisVariable->thisToken.type) + "\x1b[0m " + thisVariable->ToString(inLeft, inRight) + " = " + code->ToString(inLeft, inRight) + inRight;
    }

    std::string InitializeAssign::printAll(){
        std::string thing = "gassign";
        if(thisVariable->isLocal){
            thing = "lassign";
        }
        return padding2("initialize") + "(*Variable " + thisVariable->thisToken.getName() + "*)\n" + code->printAll() + padding2(padding(thing) + std::to_string(thisVariable->distance)) + "(*Variable " + thisVariable->thisToken.getName() + "*)\n";
    }

    void InitializeAssign::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Push back a placeholder.
        inInstructions.push_back(Instruction(Operation::initialize, 0));

        //Generate the code for the right hand side.
        code->FLVMCodeGen(inInstructions);

        //Generate the assignment using the variable.
        if(thisVariable->isLocal){
            inInstructions.push_back(Instruction(lassign, thisVariable->distance));
        } else {
            inInstructions.push_back(Instruction(gassign, thisVariable->distance));
        }
    }

    Node* InitializeAssign::copy(StackAllocator& input){
        InitializeAssign* thisptr = input.alloc<InitializeAssign>();

        thisptr->thisVariable = thisVariable->pcopy(input);
        thisptr->code = code->copy(input);
        thisptr->type = type;

        return thisptr;
    }

    InitializeAssign* InitializeAssign::pcopy(StackAllocator& input){
        InitializeAssign* thisptr = input.alloc<InitializeAssign>();

        thisptr->thisVariable = thisVariable->pcopy(input);
        thisptr->code = code->copy(input);
        thisptr->type = type;

        return thisptr;
    }



//Assignment
    Assignment::Assignment(){
        //Do nothing
    };

    Assignment::Assignment(Variable* inVariable, Node* inCode){
        thisVariable = inVariable;
        code = inCode;
    }

    std::string Assignment::ToString(std::string inLeft, std::string inRight){
        return inLeft + thisVariable->ToString(inLeft, inRight) + " = " + code->ToString(inLeft, inRight) + inRight;
    }

    std::string Assignment::printAll(){
        if(thisVariable->isLocal){
            return code->printAll() + padding2(padding("lassign") + std::to_string(thisVariable->distance)) + "(*Variable " + thisVariable->thisToken.getName() + "*)\n";
        } else {
            return code->printAll() + padding2(padding("gassign") + std::to_string(thisVariable->distance)) + "(*Variable " + thisVariable->thisToken.getName() + "*)\n";
        }
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

    Node* Assignment::copy(StackAllocator& input){
        Assignment* thisptr = input.alloc<Assignment>();

        thisptr->thisVariable = thisVariable->pcopy(input);
        thisptr->code = code->copy(input);

        return thisptr;
    }

    Assignment* Assignment::pcopy(StackAllocator& input){
        Assignment* thisptr = input.alloc<Assignment>();

        thisptr->thisVariable = thisVariable->pcopy(input);
        thisptr->code = code->copy(input);

        return thisptr;
    }


    
//Add +
    Add::Add(){
        //Do nothing
    }

    Add::Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Add::ToString(std::string inLeft, std::string inRight){
        std::string leftString = left->ToString(inLeft, inRight);
        std::string rightString = right->ToString(inLeft, inRight);
        return leftString + " + " + rightString;
    }

    std::string Add::printAll(){
        return left->printAll() + right->printAll() + "add\n";
    }

    void Add::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::add, -1));
    }

    Node* Add::copy(StackAllocator& input){
        Add* thisptr = input.alloc<Add>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Add* Add::pcopy(StackAllocator& input){
        Add* thisptr = input.alloc<Add>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Subtract -
    Subtract::Subtract(){
        //Do nothing
    }

    Subtract::Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Subtract::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " - " + right->ToString(inLeft, inRight);
    }

    std::string Subtract::printAll(){
        return left->printAll() + right->printAll() + "subtract\n";
    }

    void Subtract::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::subtract, -1));
    }

    Node* Subtract::copy(StackAllocator& input){
        Subtract* thisptr = input.alloc<Subtract>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Subtract* Subtract::pcopy(StackAllocator& input){
        Subtract* thisptr = input.alloc<Subtract>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Multiply *
    Multiply::Multiply(){
        //Do nothing
    }

    Multiply::Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Multiply::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " * " + right->ToString(inLeft, inRight);
    }

    std::string Multiply::printAll(){
        return left->printAll() + right->printAll() + "multiply\n";
    }

    void Multiply::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::multiply, -1));
    }

    Node* Multiply::copy(StackAllocator& input){
        Multiply* thisptr = input.alloc<Multiply>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Multiply* Multiply::pcopy(StackAllocator& input){
        Multiply* thisptr = input.alloc<Multiply>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Divide /
    Divide::Divide(){
        //Do nothing
    };

    Divide::Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string Divide::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " / " + right->ToString(inLeft, inRight);
    }

    std::string Divide::printAll(){
        return left->printAll() + right->printAll() + "divide\n";
    }

    void Divide::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::divide, -1));
    }

    Node* Divide::copy(StackAllocator& input){
        Divide* thisptr = input.alloc<Divide>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Divide* Divide::pcopy(StackAllocator& input){
        Divide* thisptr = input.alloc<Divide>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Parentheses ()
    Parentheses::Parentheses(){
        //Do nothing
    }

    Parentheses::Parentheses(Node* input){
        subexpression = input;
    }

    std::string Parentheses::ToString(std::string inLeft, std::string inRight){
        return "(" + subexpression->ToString(inLeft, inRight) + ")";
    }

    std::string Parentheses::printAll(){
        return subexpression->printAll();
    }

    void Parentheses::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        subexpression->FLVMCodeGen(inInstructions);
    }

    Node* Parentheses::copy(StackAllocator& input){
        Parentheses* thisptr = input.alloc<Parentheses>();

        thisptr->subexpression = subexpression->copy(input);

        return thisptr;
    }

    Parentheses* Parentheses::pcopy(StackAllocator& input){
        Parentheses* thisptr = input.alloc<Parentheses>();

        thisptr->subexpression = subexpression->copy(input);

        return thisptr;        
    }



//Negative -
    Negative::Negative(){
        //Do nothing;
    }

    Negative::Negative(Node* input){
        right = input;
    }

    std::string Negative::ToString(std::string inLeft, std::string inRight){
        return "-" + right->ToString(inLeft, inRight);
    }

    std::string Negative::printAll(){
        return right->printAll() + "negate\n";
    }

    void Negative::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::negate, -1));
    }

    Node* Negative::copy(StackAllocator& input){
        Negative* thisptr = input.alloc<Negative>();

        thisptr->right = right->copy(input);

        return thisptr;
    }

    Negative* Negative::pcopy(StackAllocator& input){
        Negative* thisptr = input.alloc<Negative>();

        thisptr->right = right->copy(input);

        return thisptr;        
    }



//Equal ==
    Equal::Equal(){
        //Do nothing
    }

    Equal::Equal(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Equal::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " == " + right->ToString(inLeft, inRight);
    }

    std::string Equal::printAll(){
        return left->printAll() + right->printAll() + "equals\n";
    }

    void Equal::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for ==
        inInstructions.push_back(Instruction(equals));
    }

    Node* Equal::copy(StackAllocator& input){
        Equal* thisptr = input.alloc<Equal>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Equal* Equal::pcopy(StackAllocator& input){
        Equal* thisptr = input.alloc<Equal>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Not equal !=
    NotEqual::NotEqual(){
        //Do nothing
    }

    NotEqual::NotEqual(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string NotEqual::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " != " + right->ToString(inLeft, inRight);
    }

    std::string NotEqual::printAll(){
        return left->printAll() + right->printAll() + "nequals\n";
    }

    void NotEqual::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for !=
        inInstructions.push_back(Instruction(nequals));
    }

    Node* NotEqual::copy(StackAllocator& input){
        NotEqual* thisptr = input.alloc<NotEqual>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    NotEqual* NotEqual::pcopy(StackAllocator& input){
        NotEqual* thisptr = input.alloc<NotEqual>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;        
    }



//Greater than >
    GreaterThan::GreaterThan(){
        //Do nothing
    }

    GreaterThan::GreaterThan(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string GreaterThan::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " > " + right->ToString(inLeft, inRight);
    }

    std::string GreaterThan::printAll(){
        return left->printAll() + right->printAll() + "greater\n";
    }

    void GreaterThan::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >
        inInstructions.push_back(Instruction(greater));
    }

    Node* GreaterThan::copy(StackAllocator& input){
        GreaterThan* thisptr = input.alloc<GreaterThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    GreaterThan* GreaterThan::pcopy(StackAllocator& input){
        GreaterThan* thisptr = input.alloc<GreaterThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Greater than or equal to >=
    GreaterThanOr::GreaterThanOr(){
        //Do nothing
    }

    GreaterThanOr::GreaterThanOr(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string GreaterThanOr::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " >= " + right->ToString(inLeft, inRight);
    }

    std::string GreaterThanOr::printAll(){
        return left->printAll() + right->printAll() + "greateror\n";
    }

    void GreaterThanOr::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for >=
        inInstructions.push_back(Instruction(greateror));
    }

    Node* GreaterThanOr::copy(StackAllocator& input){
        GreaterThanOr* thisptr = input.alloc<GreaterThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    GreaterThanOr* GreaterThanOr::pcopy(StackAllocator& input){
        GreaterThanOr* thisptr = input.alloc<GreaterThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Less than <
    LessThan::LessThan(){
        //Do nothing
    }

    LessThan::LessThan(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string LessThan::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " < " + right->ToString(inLeft, inRight);
    }

    std::string LessThan::printAll(){
        return left->printAll() + right->printAll() + "lesser\n";
    }

    void LessThan::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <
        inInstructions.push_back(Instruction(lesser));
    }

    Node* LessThan::copy(StackAllocator& input){
        LessThan* thisptr = input.alloc<LessThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    LessThan* LessThan::pcopy(StackAllocator& input){
        LessThan* thisptr = input.alloc<LessThan>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }


//Less than or equal to <=
    LessThanOr::LessThanOr(){
        //Do nothing
    }

    LessThanOr::LessThanOr(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string LessThanOr::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " <= " + right->ToString(inLeft, inRight);
    }

    std::string LessThanOr::printAll(){
        return left->printAll() + right->printAll() + "lesseror\n";
    }

    void LessThanOr::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for <=
        inInstructions.push_back(Instruction(lesseror));
    }

    Node* LessThanOr::copy(StackAllocator& input){
        LessThanOr* thisptr = input.alloc<LessThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    LessThanOr* LessThanOr::pcopy(StackAllocator& input){
        LessThanOr* thisptr = input.alloc<LessThanOr>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;        
    }



//Or OR
    Or::Or(){
        //Do nothing
    };

    Or::Or(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string Or::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " OR " + right->ToString(inLeft, inRight);
    }

    std::string Or::printAll(){
        return left->printAll() + right->printAll() + "ior\n";
    }

    void Or::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for OR
        inInstructions.push_back(Instruction(ior));
    }

    Node* Or::copy(StackAllocator& input){
        Or* thisptr = input.alloc<Or>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    Or* Or::pcopy(StackAllocator& input){
        Or* thisptr = input.alloc<Or>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//And AND
    And::And(){
        //Do nothing
    }

    And::And(Node* inLeft, Node* inRight){
        left = inLeft;
        right = inRight;
    }

    std::string And::ToString(std::string inLeft, std::string inRight){
        return left->ToString(inLeft, inRight) + " AND " + right->ToString(inLeft, inRight);
    }

    std::string And::printAll(){
        return left->printAll() + right->printAll() + "iand\n";
    }
    
    void And::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        left->FLVMCodeGen(inInstructions);
        right->FLVMCodeGen(inInstructions);
        //Push back the instruction for AND
        inInstructions.push_back(Instruction(iand));
    }

    Node* And::copy(StackAllocator& input){
        And* thisptr = input.alloc<And>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }

    And* And::pcopy(StackAllocator& input){
        And* thisptr = input.alloc<And>();

        thisptr->left = left->copy(input);
        thisptr->right = right->copy(input);

        return thisptr;
    }



//Not !
    Not::Not(){
        //Do nothing
    }

    Not::Not(Node* inRight){
        right = inRight; 
    }

    std::string Not::ToString(std::string inLeft, std::string inRight){
        return "!" + right->ToString(inLeft, inRight);
    }

    std::string Not::printAll(){
        return right->printAll() + "inot\n";
    }

    void Not::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        right->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(inot));
    }

    Node* Not::copy(StackAllocator& input){
        Not* thisptr = input.alloc<Not>();

        thisptr->right = right->copy(input);

        return thisptr;
    }

    Not* Not::pcopy(StackAllocator& input){
        Not* thisptr = input.alloc<Not>();

        thisptr->right = right->copy(input);

        return thisptr;        
    }



//if
    IfClass::IfClass(){
        //Do nothing
    };

    IfClass::IfClass(Node* inCondition, Body* inIfBody, Body* inElseBody){
        condition = inCondition;
        ifBody = inIfBody;
        elseBody = inElseBody;
    }

    std::string IfClass::ToString(std::string inLeft, std::string inRight){
        if(elseBody == nullptr){
            return inLeft + "if(" + condition->ToString(inLeft, inRight) + "){\n" + 
            ifBody->ToString("  " + inLeft, ";") +
            inLeft + "}";
        } else {
            return inLeft + "if(" + condition->ToString(inLeft, ";") + "){\n" + 
            ifBody->ToString("  ", inRight) +
            inLeft + "} else {\n" + elseBody->ToString("  " + inLeft, ";") + inLeft + "}";
        }
    }

    std::string IfClass::printAll(){
        std::string result = "(*condition*)\n" + condition->printAll();
        if(ifBody != nullptr){
            result += "(*if body*)\n" + ifBody->printAll();
        }
        if(elseBody != nullptr){
            result += "(*else body*)\n" + elseBody->printAll();
        }

        return result;
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

    Node* IfClass::copy(StackAllocator& input){
        IfClass* thisptr = input.alloc<IfClass>();

        //Check for a condition.
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        //Check for an ifBody
        if(ifBody != nullptr){
            thisptr->ifBody = ifBody->pcopy(input);
        }
        //Check for an elseBody
        if(elseBody != nullptr){
            thisptr->elseBody = elseBody->pcopy(input);
        }

        return thisptr;
    }

    IfClass* IfClass::pcopy(StackAllocator& input){
        IfClass* thisptr = input.alloc<IfClass>();

        //Check for a condition.
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        //Check for an ifBody
        if(ifBody != nullptr){
            thisptr->ifBody = ifBody->pcopy(input);
        }
        //Check for an elseBody
        if(elseBody != nullptr){
            thisptr->elseBody = elseBody->pcopy(input);
        }

        return thisptr;
    }



//for
    ForLoop::ForLoop(){
        //Do nothing
    };

    ForLoop::ForLoop(Node* inAssign, Node* inCondition, Node* inIncrementer, Body* inBody){
        assign = inAssign;
        condition = inCondition;
        incrementer = inIncrementer;
        body = inBody;
    }

    std::string ForLoop::ToString(std::string inLeft, std::string inRight){
        std::string finalString = inLeft + "\x1b[34mfor\x1b[0m(";
        if(assign != nullptr){
            finalString += assign->ToString("", ";");
        } else {
            finalString += ";";
        }

        if(condition != nullptr){
            finalString += " " + condition->ToString("", ";");
        }
        finalString += ";";

        if(incrementer != nullptr){
            finalString += " " + incrementer->ToString("", "");
        }
        finalString += "){\n";

        return finalString + inLeft + body->ToString(inLeft, inRight) + inLeft + "\n" + inLeft + "}";
    }

    std::string ForLoop::printAll(){
        std::string result = "  (*Start of for loop*)\n";
        if(assign != nullptr){
            result += "  (*assignment*)\n" + assign->printAll();
        }
        if(condition != nullptr){
            result += "  (*condition*)\n" + condition->printAll() + 
                padding2("cjump") + "(*cjump out of the loop*)\n";
        }
        if(body != nullptr){
            result += "  (*body*)\n" + body->printAll();
        }
        if(incrementer != nullptr){
            result += "  (*incrementer*)\n" + incrementer->printAll();
        }
        result += padding2("cjump") + "(*cjump to the condition start*)\n  (*End of for loop*)\n";

        return result;
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

    Node* ForLoop::copy(StackAllocator& input){
        ForLoop* thisptr = input.alloc<ForLoop>();

        if(assign != nullptr){
            thisptr->assign = assign->copy(input);
        }
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(incrementer != nullptr){
            thisptr->incrementer = incrementer->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }

    ForLoop* ForLoop::pcopy(StackAllocator& input){
        ForLoop* thisptr = input.alloc<ForLoop>();

        if(assign != nullptr){
            thisptr->assign = assign->copy(input);
        }
        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(incrementer != nullptr){
            thisptr->incrementer = incrementer->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }



//while
    WhileLoop::WhileLoop(){
        //Do nothing
    };

    WhileLoop::WhileLoop(Node* inCondition, Body* inBody){
        condition = inCondition;
        body = inBody;
    }

    std::string WhileLoop::ToString(std::string inLeft, std::string inRight){
        return inLeft + "while(" + condition->ToString(inLeft, inRight) + "){\n" + body->ToString("  " + inLeft, inRight) + inLeft + "}";
    }

    std::string WhileLoop::printAll(){
        std::string result = "";
        if(condition != nullptr){
            result += "(*condition*)\n" + condition->printAll() + 
                "negate\n" +
                padding2("cjump") + "(*cjump to escape the loop*)\n";
        }
        if(body != nullptr){
            result += "(*body*)\n" + body->printAll() + 
                padding2("cjump") + "(*cjump to the condition*)";
        }

        return result;
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

    Node* WhileLoop::copy(StackAllocator& input){
        WhileLoop* thisptr = input.alloc<WhileLoop>();

        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }

    WhileLoop* WhileLoop::pcopy(StackAllocator& input){
        WhileLoop* thisptr = input.alloc<WhileLoop>();

        if(condition != nullptr){
            thisptr->condition = condition->copy(input);
        }
        if(body != nullptr){
            thisptr->body = body->pcopy(input);
        }

        return thisptr;
    }



//Function
    Function::Function(){
        //Do nothing
    };

    Function::Function(bool inReturnable, std::string_view inName, Scope* inCode){
        name = inName;
        returnable = inReturnable;
        code = inCode;
    }

    std::string Function::ToString(std::string inLeft, std::string inRight){
        std::string varString = "";
        Variable* currVar = code->variables;

        //Combine all the variables, if any.
        if(code->variables != nullptr){
            //Stop right before the last variable to not append an extra comma.
            while(currVar->next != nullptr){
                varString += "\x1b[36m" + typeString(currVar->thisToken.type) + "\x1b[0m " + currVar->thisToken.getName() + ", ";
                currVar = currVar->next;
            }
            //Append the last variable without an extra comma.
            varString += "\x1b[36m" + typeString(currVar->thisToken.type) + "\x1b[0m " + currVar->thisToken.getName();
        }
        //Return the function printed in the only correct format.
        std::string result = inLeft + "\x1b[36m" + typeString(type) + "\x1b[35m " + std::string(name) + "\x1b[0m(" + varString + "){\n" + 
            code->ToString("  " + inLeft, ";") +
        "\n" + inLeft + "}\n\n";

        if(next != nullptr){
            return result + next->ToString(inLeft, inRight);
        } else {
            return result;
        }

    }

    std::string Function::printAll(){
        std::string result = "(*Function " + std::string(name) + "*)\n";
        if(code->body != nullptr){
            result += code->body->printAll() + "\n";
        }
        if(allFunctions != nullptr){
            return result + allFunctions->printAll();
        }
        return result;
    }

    void Function::append(Variable* input){
        Variable* currVar = code->variables;
        if(currVar != nullptr){
            while(currVar->next != nullptr){
                currVar = currVar->next;
            }
            currVar->next = input;
        } else {
            code->variables = input;
        }
    }

    void Function::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Change where this function starts in the program.
        if(!alreadyGenerated){
            position = inInstructions.size();
            if(code != nullptr){
                code->FLVMCodeGen(inInstructions);
            }
            if(allFunctions != nullptr){
                allFunctions->FLVMCodeGen(inInstructions);
            }
        }
    }

    Node* Function::copy(StackAllocator& input){
        Function* thisptr = input.alloc<Function>();

        thisptr->returnable = returnable;
        thisptr->position = position;
        thisptr->code = code->pcopy(input);

        return thisptr;
    }

    Function* Function::pcopy(StackAllocator& input){
        Function* thisptr = input.alloc<Function>();

        thisptr->returnable = returnable;
        thisptr->position = position;
        thisptr->code = code->pcopy(input);

        return thisptr;
    }



//Function calls
    Call::Call(){
        //Do nothing
    }

    Call::Call(Function* inFunction){
        function = inFunction;
    }

    std::string Call::ToString(std::string inLeft, std::string inRight){
        std::string theName = "\x1b[35m" + std::string(function->name) + "\x1b[0m";
        std::string theArguments = arguments->ToString(inLeft, "");
        return theName + "(" + theArguments + ")";
    }

    std::string Call::printAll(){
        std::string result = "";

        return arguments->printAll() + padding2("call") + "(*Function " + std::string(function->name) + "*)\n";
    }

    void Call::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        //Where the scope instruction sits.
        //size_t where = inInstructions.size();
        //Make the function call after the function's arguments are created.
        inInstructions.push_back(Instruction(Operation::scope, 0));
        if(arguments != nullptr){
            Arguments* currBody = arguments;
            int64_t count = 0;
            while(currBody != nullptr){
                //Initialize for the argument
                inInstructions.push_back(Instruction(Operation::initialize, 0));
                //Generate code for the argument.
                currBody->current->FLVMCodeGen(inInstructions);
                //Move to the next node in the linked list.
                currBody = currBody->next;
                //Assign it to the initialization made before.
                inInstructions.push_back(Instruction(Operation::lassign, count));
                count++;
            }
        }

        //Make the function call after the function's arguments are created.
        inInstructions.push_back(Instruction(Operation::call, function->position));
    }

    Node* Call::copy(StackAllocator& input){
        Call* thisptr = input.alloc<Call>();
        //Get the list of currently existing functions in the scope
        Scope* theScope = input.currScope;
        Function* theFunction = input.currScope->functions;

        //Traverse the linked lists to find the function.
        //It is guaranteed to exist, but may not be in the current scope.
        while(input.currScope != nullptr){
            while(function->name != theFunction->name){
                theFunction = theFunction->next;
                if(theFunction == nullptr){
                    break;
                }
            }
            theScope = theScope->parent;
        }

        thisptr->function = theFunction;
        thisptr->arguments = arguments->pcopy(input);

        return thisptr;
    }

    Call* Call::pcopy(StackAllocator& input){
        Call* thisptr = input.alloc<Call>();
        //Get the list of currently existing functions in the scope
        Scope* theScope = input.currScope;
        Function* theFunction = input.currScope->functions;

        //Traverse the linked lists to find the function.
        //It is guaranteed to exist, but may not be in the current scope.
        while(input.currScope != nullptr){
            while(function->name != theFunction->name){
                theFunction = theFunction->next;
                if(theFunction == nullptr){
                    break;
                }
            }
            theScope = theScope->parent;
        }

        thisptr->function = theFunction;
        thisptr->arguments = arguments->pcopy(input);

        return thisptr;        
    }



//Arguments
    Arguments::Arguments(){
        //Do nothing lmfao
        //The default settings are already in place.
    }

    Arguments::Arguments(Node* input){
        current = input;
    }

    std::string Arguments::ToString(std::string inLeft, std::string inRight){
        if(next != nullptr){
            return current->ToString(inLeft, "") + ", " + next->ToString(inLeft, "");
        } else {
            return current->ToString(inLeft, "");
        }

        //Unreachable, just like uncountable infinity.
        return "";

    }

    std::string Arguments::printAll(){
        if(next != nullptr){
            return current->printAll() + next->printAll();
        } else {
            return current->printAll();
        }

        //Unreachable, just like that 1600 SAT score.
        return "";

    }

    void Arguments::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Arguments* currArgs = this;
        while(currArgs != nullptr){
            currArgs->current->FLVMCodeGen(inInstructions);
            currArgs = currArgs->next;
        }
    }

    Node* Arguments::copy(StackAllocator& input){
        Arguments* thisptr = input.alloc<Arguments>();

        if(current != nullptr){
            thisptr->current = current->copy(input);
        }
        if(next != nullptr){
            thisptr->next = next->pcopy(input);
        }

        return thisptr;
    }

    Arguments* Arguments::pcopy(StackAllocator& input){
        Arguments* thisptr = input.alloc<Arguments>();

        if(current != nullptr){
            thisptr->current = current->copy(input);
        }
        if(next != nullptr){
            thisptr->next = next->pcopy(input);
        }

        return thisptr;        
    }



//Return statements
    ReturnClass::ReturnClass(){
        //Do nothing
    }

    ReturnClass::ReturnClass(Node* input){
        statement = input;
    }

    std::string ReturnClass::ToString(std::string inLeft, std::string inRight){
        return inLeft + "\x1b[34mreturn\x1b[0m " + statement->ToString(inLeft, inRight) + inRight;
    }

    std::string ReturnClass::printAll(){
        return statement->printAll() + "return\n";
    }

    void ReturnClass::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        statement->FLVMCodeGen(inInstructions);
        inInstructions.push_back(Instruction(Operation::ireturn));
    }

    Node* ReturnClass::copy(StackAllocator& input){
        ReturnClass* thisptr = input.alloc<ReturnClass>();

        if(statement != nullptr){
            thisptr->statement = statement->copy(input);
        }

        return thisptr;
    }

    ReturnClass* ReturnClass::pcopy(StackAllocator& input){
        ReturnClass* thisptr = input.alloc<ReturnClass>();

        if(statement != nullptr){
            thisptr->statement = statement->copy(input);
        }

        return thisptr;        
    }



//CallStack
    CallStack::CallStack(){
        thisScope = nullptr;
        reference = 0;
        instNumber = 0;
    }

    CallStack::CallStack(Scope* inTheScope, uint64_t inReference, uint64_t inInstNumber){
        thisScope = inTheScope;
        reference = inReference;
        instNumber = inInstNumber;
    }

    int64_t CallStack::where(Variable* inVariable){
        if(thisScope->variables == nullptr){
            return -1;
        }

        Variable* currVar = thisScope->variables;
        int64_t count = 0;
        while(currVar != nullptr){
            if(currVar->thisToken.getName() == inVariable->thisToken.getName()){
                return count;
            }
            currVar = currVar->next;
            count++;
        }

        return -1;
    }