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



void FloridaVM::callNew(){
    if(allCalls->head < allCalls->end){
        allCalls->head = new(allCalls->current) CallStack();
        allCalls->current = (CallStack*) (sizeof(CallStack) + (size_t) allCalls->current);
    } else {
        std::cout << "The callAllocator does not have enough memory.\nThe limit of " + std::to_string(((size_t)(allCalls->end) - (size_t)(allCalls->head)) / sizeof(CallStack)) + " was exceeded.";
        std::bad_alloc();
    }
}

void FloridaVM::callPop(){
    if(allCalls->current != allCalls->head){
        allCalls->current = allCalls->head;
        allCalls->head = (CallStack*) ((size_t) (allCalls->head) - sizeof(CallStack));
    } else {
        std::cout << "The virtual machine has attempted to release a scope it does not have.";
    }
}

CallStack* FloridaVM::callTop(){
    return allCalls->top;
}



void FloridaVM::printAll(){
    std::string result = "";
    if(allFunctions != nullptr){
        result += allFunctions->printAll();
    }

    result += "(*MAIN SCRIPT*)\n";

    std::cout << result + AST->printAll();
}

void FloridaVM::printInstructions(){
    Operation oper;
    int64_t literal;
    for(size_t i = 0; i < programInstructions.size(); i++){
        oper  = programInstructions[i].oper;
        literal = programInstructions[i].literal.fixed64;
        switch(oper){
            case Operation::scope:
                padRight("scope");
                continue;
            case Operation::call:
                padRight("call", std::to_string(literal));
                continue;
            case Operation::ireturn:
                padRight("ireturn");
                continue;
            case Operation::gfetch:
                padRight("gfetch", std::to_string(literal));
                continue;
            case Operation::lfetch:
                padRight("lfetch", std::to_string(literal));
                continue;
            case Operation::cjump:
                padRight("cjump", std::to_string(literal));
                continue;
            case Operation::jump:
                padRight("jump", std::to_string(literal));
                continue;
            case Operation::initialize:
                padRight("initialize");
                continue;
            case Operation::gassign:
                padRight("gassign", std::to_string(literal));
                continue;
            case Operation::lassign:
                padRight("lassign", std::to_string(literal));
                continue;
            case Operation::push:
                padRight("push", std::to_string(literal));
                continue;
            case Operation::pop:
                padRight("pop");
                continue;
            case Operation::multiply:
                padRight("multiply");
                continue;
            case Operation::divide:
                padRight("divide");
                continue;
            case Operation::add:
                padRight("add");
                continue;
            case Operation::negate:
                padRight("negate");
                continue;
            case Operation::equals:
                padRight("equals");
                continue;
            case Operation::nequals:
                padRight("nequals");
                continue;
            case Operation::greater:
                padRight("greater");
                continue;
            case Operation::greateror:
                padRight("greaterOr");
                continue;
            case Operation::lesser:
                padRight("lesser");
                continue;
            case Operation::lesseror:
                padRight("lesseror");
                continue;
            case Operation::ior:
                padRight("ior");
                continue;
            case Operation::iand:
                padRight("iand");
                continue;
            case Operation::inot:
                padRight("inot");
                continue;
            default:
                padRight("UNKOWN");
                return;
        }
    }
}

bool FloridaVM::next(){
    //left and right hand members for operations
    types left;
    types right;
    types result;

    //Check to see if all of instructions have been executed.
    if(instructionNumber >= programInstructions.size()){
        return false;
    }

    switch (programInstructions[instructionNumber].oper){
        case Operation::call:
            //Move the instruction to the start of its instruction set.
            instructionNumber = programInstructions[instructionNumber].literal.fixed64;
            return true;
        case Operation::scope:
            //The new reference frame is the size of the vector.
            callNew();
            callTop()->reference = reference;
            reference = computationVector.size();
            break;
        case Operation::ireturn:
            //Adjust these to pre-scope values.

            //Move the result to the top of the old stack.
            computationVector[reference] = top();
            //Shed all elements above the original top of the stack.
            for(size_t i = computationVector.size(); i >= reference; i--){
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
