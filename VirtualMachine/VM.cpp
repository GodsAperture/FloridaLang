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

void FloridaVM::infoPrint(){
    std::cout << "--Instruction number: " << instructionNumber << "\n";
    std::cout << "--Stack size: " << computationVector.size() << "\n";
    std::cout << "--Call Stack count: " << BPStackSize / 2 << "\n";
    std::cout << "\n";
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



void FloridaVM::BPPush(uint64_t whichScope, uint64_t reference){
    if(BPStackSize + 2 < BPMax){
        BPStack[BPStackSize] = whichScope;
        BPStack[BPStackSize + 1] = reference;
        BPStackSize += 2;
    } else {
        throw new std::bad_alloc();
    }
}

uint64_t FloridaVM::BPScope(uint64_t input){
    return BPStack[2 * input];
}

uint64_t FloridaVM::BPReference(uint64_t input){
    return BPStack[2 * input + 1];
}

uint64_t FloridaVM::BPTopScope(){
    return BPStack[BPStackSize - 2];
}

uint64_t FloridaVM::BPTopReference(){
    return BPStack[BPStackSize - 1];
}

void FloridaVM::BPPop(){
    BPStackSize -= 2;
}

void FloridaVM::INPush(uint64_t input){
    if(INStackSize + 1 < INMax){
        INStack[INStackSize] = input;
        INStackSize++;
    } else {
        throw new std::bad_alloc();
    }
}

uint64_t FloridaVM::INPop(){
    INStackSize--;
    return INStack[INStackSize];
}



void FloridaVM::printAll(){
    std::cout << "\n    ====Instruction set debugger====\n\n";
    std::string result = "";
    if(allFunctions != nullptr){
        result += allFunctions->printAll();
    }

    result += "\t(*MAIN SCRIPT*)\n";

    std::cout << result + AST->printAll();
}

bool FloridaVM::next(){
    //left, right, and resulting members for operations and convenience.
    types left;
    types right;
    types result;

    //Check to see if all of instructions have been executed.
    if(instructionNumber >= programInstructions.size()){
        return false;
    }

    //Other variables that exist for convenience.
    Instruction current = programInstructions[instructionNumber];
    //ExistingScope* currScope = allCalls->top;
    switch (programInstructions[instructionNumber].oper){
        case Operation::call:
            //Push the current Instruction number to the INStack.
            INPush(instructionNumber);            
            //Move the instruction to the start of its instruction set.
            instructionNumber = programInstructions[instructionNumber].literal.fixed64;
            return true;
        case Operation::newScope:
            //Use left to get the correct UniqueScope.
            left = top();
            //Move the CurrentInfo value to the BPStack.
            BPPush(left.fixed64, CurrentBP[left.fixed64]);
            //Adjust CurrentBP.
            CurrentBP[left.fixed64] = reference;
            //Adjust the VM's current reference to be the new size of the vector.
            reference = computationVector.size() - 1;
            //Pop the information used to adjust the proper unique scope.
            pop();
            break;
        case Operation::deleteScope:
            //Readjust the reference to be the prior reference.
            reference = CurrentBP[BPTopScope()];
            //Readjust the most recent scope reference.
            CurrentBP[BPTopScope()] = BPTopScope();
            //Pop the last scope from the BPStack.
            BPPop();
            //Pop the trash from the top of the stack.
            pop();
            break;
        case Operation::ireturn:
            //Grab the resulting value that is to be returned from the scope.
            result = top();
            //The return instruction has to exit scope a particular number of times.
            for(int i = 0; i < current.literal.fixed64; i++){
                //Readjust the reference to be the prior reference.
                reference = CurrentBP[BPTopScope()];
                //Readjust the most recent scope reference.
                CurrentBP[BPTopScope()] = BPTopReference();
                //Pop the last scope from the BPStack.
                BPPop();
            }
            //Adjust the Instruction number to the last known value.
            instructionNumber = INPop();
            //Move the result to the spot above the old stack.
            computationVector[reference + 1] = result;
            //Shed all elements above the original top of the stack.
            for(size_t i = computationVector.size(); i > reference + 2; i--){
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
                instructionNumber = current.literal.fixed64;
                //Pop the boolean from the stack.
                pop();
                return true;
            }
        case Operation::jump:
            instructionNumber = current.literal.fixed64;
            break;



////Variable related instructions.
        case Operation::gfetch:
            //Push the global value onto the stack.
            computationVector.push_back(computationVector[programInstructions[instructionNumber].literal.fixed64]);
            break;
        case Operation::mfetch:
            //Grab the prior value to index allScopes.
            //This is WhichScope.   
            left = computationVector[computationVector.size() - 1];
            //Get the proper scope in question and fetch its reference.
            left.fixed64 = BPScope(left.fixed64);
            //Using the distance provided at the top of the stack, push the resulting value onto the stack.
            result = computationVector[left.fixed64 + current.literal.fixed64];
            //Overwrite the trash with 
            computationVector[computationVector.size() - 1] = result;
            break;
        case Operation::lfetch:
            //Push the local value onto the stack.
            push(computationVector[reference + programInstructions[instructionNumber].literal.fixed64]);
            break;
        case Operation::initialize:
            left.fixed64 = 0;
            push(left);
            break;
        case Operation::gassign:
            computationVector[programInstructions[instructionNumber].literal.fixed64] = top();
            pop();
            break;
        case Operation::massign:
            //This is the result for the variable.
            result = computationVector[computationVector.size() - 2];
            //Grab WhichScope.
            left = computationVector[computationVector.size() - 1];
            //Move the result to the place in the computation vector.
            computationVector[BPReference(left.fixed64)] = result;
            //Pop the result and the ExistingScope position.
            pop();
            pop();
            break;
        case Operation::lassign:
            computationVector[reference + programInstructions[instructionNumber].literal.fixed64] = top();
            pop();            
            break;



////VM related instructions.
        case Operation::push:
            push(programInstructions[instructionNumber].literal);
            break;
        case Operation::pop:
            computationVector.pop_back();
            break;



////horizontal casting
        case Operation::float8TOfixed8:
            computationVector[computationVector.size() - 1].fixed64 = (int64_t) computationVector[computationVector.size() - 1].float64;
            break;
        case Operation::float8TOufixed8:
            computationVector[computationVector.size() - 1].ufixed64 = (uint64_t) computationVector[computationVector.size() - 1].float64;
            break;
        case Operation::fixed8TOufixed8:
            computationVector[computationVector.size() - 1].ufixed64 = (uint64_t) computationVector[computationVector.size() - 1].fixed64;
            break;
        case Operation::fixed8TOfloat8:
            computationVector[computationVector.size() - 1].float64 = (_Float64) computationVector[computationVector.size() - 1].fixed64;
            break;
        case Operation::ufixed8TOfloat8:
            computationVector[computationVector.size() - 1].float64 = (_Float64) computationVector[computationVector.size() - 1].ufixed64;
            break;
        case Operation::ufixed8TOfixed8:
            computationVector[computationVector.size() - 1].fixed64 = (uint64_t) computationVector[computationVector.size() - 1].ufixed64;
            break;



////float8 downcasting
        case Operation::float8TOfloat4:
            computationVector[computationVector.size() - 1].float32 = (_Float32) computationVector[computationVector.size() - 1].float64;
            break;

////float8 upcasting
        case Operation::float4TOfloat8:
            computationVector[computationVector.size() - 1].float64 = (_Float64) computationVector[computationVector.size() - 1].float32;
            break;

////ufixed8 downcasting
        case Operation::ufixed8TOufixed4:
            computationVector[computationVector.size() - 1].ufixed32 = (uint32_t) computationVector[computationVector.size() - 1].ufixed64;
            break;
        case Operation::ufixed8TOufixed2:
            computationVector[computationVector.size() - 1].ufixed16 = (uint16_t) computationVector[computationVector.size() - 1].ufixed64;
            break;
        case Operation::ufixed8TOufixed1:
            computationVector[computationVector.size() - 1].ufixed8 = (uint8_t) computationVector[computationVector.size() - 1].ufixed64;
            break;

////fixed8 mathematical instructions.
        case Operation::fixed8add:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 + right.fixed64;
            push(result);
            break;
        case Operation::fixed8subtract:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 - right.fixed64;
            push(result);
            break;
        case Operation::fixed8negate:
            //Negate the value;
            computationVector[computationVector.size() - 1].fixed64 = -computationVector[computationVector.size() - 1].fixed64;
            break;
        case Operation::fixed8multiply:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 * right.fixed64;
            push(result);
            break;
        case Operation::fixed8divide:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.fixed64 = left.fixed64 / right.fixed64;
            push(result);
            break;



////float8 mathematical operations
            case Operation::float8add:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.float64 = left.float64 + right.float64;
            push(result);
            break;
        case Operation::float8subtract:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.float64 = left.float64 - right.float64;
            push(result);
            break;
        case Operation::float8negate:
            //Negate the value;
            computationVector[computationVector.size() - 1].float64 = -computationVector[computationVector.size() - 1].float64;
            break;
        case Operation::float8multiply:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.float64 = left.float64 * right.float64;
            push(result);
            break;
        case Operation::float8divide:
            //Get the right operand;
            right = pop();
            //Get the left operand;
            left = pop();
            //Operate and push;
            result.float64 = left.float64 / right.float64;
            push(result);
            break;



////Boolean algebra instructions.
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
