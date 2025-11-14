#include "VM.hpp"

void FloridaVM::push(types input){
    computationVector.push_back(input);
}

types FloridaVM::pop(){
    types result = computationVector.back();
    computationVector.pop_back();

    return result;
}

types FloridaVM::top(){
    return computationVector.back();
}

inline void padRight(std::string input){
    for(size_t i = 0; i < 12 - input.size(); i++){
        input += " ";
    }

    std::cout << input + "\n";

}

inline void padRight(std::string input, std::string number){
    size_t size = input.size();
    for(size_t i = 0; i < 12 - size; i++){
        input += " ";
    }

    std::cout << input + number + "\n";

}



ExistingScope* FloridaVM::callNew(){
    if(allCalls->head < allCalls->end){
        allCalls->head = new(allCalls->current) ExistingScope();
        allCalls->top = allCalls->current;
        allCalls->current = (ExistingScope*) (sizeof(ExistingScope) + (size_t) allCalls->current);
        return allCalls->current;
    } else {
        std::cout << "The callAllocator does not have enough memory.\nThe limit of " + std::to_string(((size_t)(allCalls->end) - (size_t)(allCalls->head)) / sizeof(ExistingScope)) + " was exceeded.";
        std::bad_alloc();
    }

    //Unreachable
    return nullptr;
}

void FloridaVM::callPop(){
    if(allCalls->current != allCalls->head){
        allCalls->current = allCalls->head;
        allCalls->head = (ExistingScope*) ((size_t) (allCalls->head) - sizeof(ExistingScope));
    } else {
        std::cout << "The virtual machine has attempted to release a scope it does not have.";
    }
}

ExistingScope* FloridaVM::callTop(){
    return allCalls->top;
}



void FloridaVM::printAll(){
    std::cout << "\n    ====Instruction set debugger====\n\n";
    std::string result = "";
    if(allFunctions != nullptr){
        result += allFunctions->printAll();
    }

    result += "(*MAIN SCRIPT*)\n";

    std::cout << result + AST->printAll();
}

bool FloridaVM::next(){
    //left and right hand members for operations.
    types left;
    types right;
    types result;
    ExistingScope* newScope = nullptr;

    //Check to see if all of instructions have been executed.
    if(instructionNumber >= programInstructions.size()){
        return false;
    }

    switch (programInstructions[instructionNumber].oper){
        case Operation::call:
            //Move the instruction to the start of its instruction set.
            instructionNumber = programInstructions[instructionNumber].literal.fixed64;
            //Adjust the instruction number of the Existing Scope.
            callTop()->instructionNumber = instructionNumber;
            return true;
        case Operation::newScope:
            //Create a new scope in the call stack.
            callNew();
            //Define the base pointer for the new ExistingScope.
            callTop()->reference = reference;
            //Adjust the VM's current reference to be the new size of the vector.
            reference = computationVector.size();
            break;
        case Operation::deleteScope:
            //Readjust the reference to be the prior reference.
            reference = callTop()->reference;
            //Pop the Existing Scope from the scope stack.
            callPop();
            break;
        case Operation::ireturn:
            //Adjust these to pre-scope values.
            reference = callTop()->reference;
            //Adjust the instruction number to be whatever it was before the call.
            instructionNumber = callTop()->instructionNumber;
            //Remove the scope from the call stack.
            callPop();
            //Move the result to the top of the old stack.
            computationVector[reference] = top();
            //Shed all elements above the original top of the stack.
            for(size_t i = computationVector.size(); i > reference; i--){
                computationVector.pop_back();
            }
            break;
        case Operation::cjump:
            //If it's true, then don't skip.
            if(top().boolean){
                //Pop the boolean from the stack.
                pop();
                instructionNumber++;
                return true;
            } else {
                //This is to adjust the position of the instruction number.
                instructionNumber = programInstructions[instructionNumber].literal.fixed64;
                //Pop the boolean from the stack.
                pop();
                return true;
            }
        case Operation::jump:
            instructionNumber = programInstructions[instructionNumber].literal.fixed64;
            break;
        case Operation::gfetch:
            //Push the global value onto the stack.
            computationVector.push_back(computationVector[programInstructions[instructionNumber].literal.fixed64]);
            break;
/*This*/case Operation::mfetch:
            break;
        case Operation::lfetch:
            //Push the local value onto the stack.
            computationVector.push_back(computationVector[reference + programInstructions[instructionNumber].literal.fixed64]);
            break;
        case Operation::initialize:
            left.fixed64 = 0;
            computationVector.push_back(left);
            break;
        case Operation::gassign:
            computationVector[programInstructions[instructionNumber].literal.fixed64] = top();
            pop();
            break;
/*This*/case Operation::massign:
            pop();
            break;
        case Operation::lassign:
            computationVector[reference + programInstructions[instructionNumber].literal.fixed64] = top();
            pop();            
            break;
        case Operation::push:
            push(programInstructions[instructionNumber].literal);
            break;
        case Operation::pop:
            computationVector.pop_back();
            break;
        case Operation::add:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 + right.fixed64;
            push(result);
            break;
        case Operation::subtract:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 - right.fixed64;
            push(result);
            break;
        case Operation::negate:
            //Negate the value;
            computationVector[computationVector.size() - 1].fixed64 = -computationVector[computationVector.size() - 1].fixed64;
            break;
        case Operation::multiply:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 * right.fixed64;
            push(result);
            break;
        case Operation::divide:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 / right.fixed64;
            push(result);
            break;
        case equals:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 == right.fixed64;
            push(result);
            break;
        case nequals:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 != right.fixed64;
            push(result);
            break;
        case greater:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 > right.fixed64;
            push(result);
            break;
        case greateror:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 >= right.fixed64;
            push(result);
            break;
        case lesser:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 < right.fixed64;
            push(result);
            break;
        case lesseror:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 <= right.fixed64;
            push(result);
            break;
        case ior:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.boolean or right.boolean;
            push(result);
            break;
        case iand:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.boolean and right.boolean;
            push(result);
            break;
        case inot:
            //Get the right operand;
            right = pop();
            //Operate and push;
            result.boolean = !right.boolean;
            push(result);
            break;
        default:
            std::cout << "Error: Unknown instruction given.\nThe instruction number is " + std::to_string(programInstructions[instructionNumber].oper);
            return false;
    }
    instructionNumber++;
    return true;
}

bool FloridaVM::debuggerNext(){
    return true;
}
