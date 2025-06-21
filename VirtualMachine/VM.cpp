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
            //Start a new relative stack scope.
            left.fixed64 = reference;
            computationVector.push_back(left);
            //We start at the value *after* the last out of scope variable.
            reference = computationVector.size();
            return true;
        case Operation::ireturn:
            computationVector[reference - 1] = top();
            //To do, when functions arrive and return statements exist.
            return true;
        case Operation::cjump:
            //If it's true, then don't skip.
            if(top().boolean){
                //Pop the boolean from the stack.
                instructionNumber++;
                pop();
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
            return true;
        case Operation::gfetch:
            //Push the global value onto the stack.
            computationVector.push_back(computationVector[programInstructions[instructionNumber].literal.fixed64]);
            instructionNumber++;
            return true;
        case Operation::lfetch:
            //Push the local value onto the stack.
            computationVector.push_back(computationVector[reference + programInstructions[instructionNumber].literal.fixed64]);
            instructionNumber++;
            return true;
        case Operation::initialize:
            left.fixed64 = 0;
            computationVector.push_back(left);
            instructionNumber++;
            return true;
        case Operation::gassign:
            computationVector[programInstructions[instructionNumber].literal.fixed64] = top();
            pop();
            instructionNumber++;
            return true;
        case Operation::lassign:
            computationVector[reference + programInstructions[instructionNumber].literal.fixed64] = top();
            instructionNumber++;
            pop();
            return true;
        case Operation::push:
            push(programInstructions[instructionNumber].literal);
            instructionNumber++;
            return true;
        case Operation::pop:
            computationVector.pop_back();
            instructionNumber++;
            return true;
        case Operation::add:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 + right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case Operation::subtract:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 - right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case Operation::negate:
            //Negate the value;
            computationVector[computationVector.size() - 1].fixed64 = -computationVector[computationVector.size() - 1].fixed64;
            instructionNumber++;
            return true;
        case Operation::multiply:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 * right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case Operation::divide:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 / right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case equals:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 == right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case nequals:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 != right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case greater:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 > right.fixed64;
            push(result);
            instructionNumber++;
            return true;
            return true;
        case greateror:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 >= right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case lesser:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 < right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case lesseror:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.fixed64 <= right.fixed64;
            push(result);
            instructionNumber++;
            return true;
        case ior:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.boolean or right.boolean;
            push(result);
            instructionNumber++;
            return true;
        case iand:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.boolean = left.boolean and right.boolean;
            push(result);
            instructionNumber++;
            return true;
        case inot:
            //Get the right operand;
            right = pop();
            //Operate and push;
            result.boolean = !right.boolean;
            push(result);
            instructionNumber++;
            return true;
        default:
            std::cout << "Error: Unknown instruction given.\nThe instruction number is " + std::to_string(programInstructions[instructionNumber].oper);
            return false;
    }

}
