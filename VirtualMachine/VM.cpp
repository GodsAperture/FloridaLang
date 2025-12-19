#include "VM.hpp"

inline void FloridaVM::push(types input){
    computationVector.push_back(input);
}

inline void FloridaVM::pop(){
    computationVector.pop_back();
}

inline types FloridaVM::top(){
    return computationVector.back();
}

inline void FloridaVM::edit(types input){
    computationVector[computationVector.size() - 1] = input;
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
    switch (programInstructions[instructionNumber].oper){
        case Operation::call:
            //Push the current Instruction number to the INStack.
            INPush(instructionNumber);            
            //Move the instruction to the start of its instruction set.
            instructionNumber = programInstructions[instructionNumber].literal.fixed8;
            return true;
        case Operation::newScope:
            //Use left to get the correct UniqueScope.
            left = top();
            //Move the CurrentInfo value to the BPStack.
            BPPush(left.fixed8, CurrentBP[left.fixed8]);
            //Adjust CurrentBP.
            CurrentBP[left.fixed8] = reference;
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
            for(int i = 0; i < current.literal.fixed8; i++){
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
                instructionNumber = current.literal.fixed8;
                //Pop the boolean from the stack.
                pop();
                return true;
            }
        case Operation::jump:
            instructionNumber = current.literal.fixed8;
            break;



////Variable related instructions.
        case Operation::gfetch:
            //Push the global value onto the stack.
            push(computationVector[programInstructions[instructionNumber].literal.fixed8]);
            break;
        case Operation::mfetch:
            //Grab the prior value to index allScopes.
            //This is WhichScope.   
            left = computationVector[computationVector.size() - 1];
            //Get the proper scope in question and fetch its reference.
            left.fixed8 = BPScope(left.fixed8);
            //Using the distance provided at the top of the stack, push the resulting value onto the stack.
            result = computationVector[left.fixed8 + current.literal.fixed8];
            //Overwrite the trash with the result.
            edit(result);
            break;
        case Operation::lfetch:
            //Push the local value onto the stack.
            push(computationVector[reference + programInstructions[instructionNumber].literal.fixed8]);
            break;
        case Operation::initialize:
            left.fixed8 = 0;
            push(left);
            break;
        case Operation::gassign:
            computationVector[programInstructions[instructionNumber].literal.fixed8] = top();
            pop();
            break;
        case Operation::massign:
            //This is the result for the variable.
            result = computationVector[computationVector.size() - 2];
            //Grab WhichScope.
            left = computationVector[computationVector.size() - 1];
            //Move the result to the place in the computation vector.
            computationVector[BPReference(left.fixed8)] = result;
            //Pop the result and the ExistingScope position.
            pop();
            pop();
            break;
        case Operation::lassign:
            computationVector[reference + programInstructions[instructionNumber].literal.fixed8] = top();
            pop();            
            break;



////VM related instructions.
        case Operation::push:
            push(programInstructions[instructionNumber].literal);
            break;
        case Operation::pop:
            computationVector.pop_back();
            break;



////Typecasting, in a different order.
        //TOufixed1 casts.
        case Operation::ufixed1TOufixed1:
            //Redundant. Intentionally undefined.
        case Operation::ufixed2TOufixed1:
            result.ufixed1 = (uint8_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOufixed1:
            result.ufixed1 = (uint8_t) top().ufixed4;
            edit(result);
            break;
        case Operation::ufixed8TOufixed1:
            result.ufixed1 = (uint8_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed1:
            result.ufixed1 = (uint8_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOufixed1:
            result.ufixed1 = (uint8_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOufixed1:
            result.ufixed1 = (uint8_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOufixed1:
            result.ufixed1 = (uint8_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed1:
            result.ufixed1 = (uint8_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOufixed1:
            result.ufixed1 = (uint8_t) top().float8;
            edit(result);
            break;

        //TOufixed2 casts.
        case Operation::ufixed1TOufixed2:
            result.ufixed2 = (uint16_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOufixed2:
            //Redundant. Intentionally undefined.
        case Operation::ufixed4TOufixed2:
            result.ufixed2 = (uint16_t) top().ufixed4;
            edit(result);
            break;
        case Operation::ufixed8TOufixed2:
            result.ufixed2 = (uint16_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed2:
            result.ufixed2 = (uint16_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOufixed2:
            result.ufixed2 = (uint16_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOufixed2:
            result.ufixed2 = (uint16_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOufixed2:
            result.ufixed2 = (uint16_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed2:
            result.ufixed2 = (uint16_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOufixed2:
            result.ufixed2 = (uint16_t) top().float8;
            edit(result);
            break;

        //TOufixed4 casts.
        case Operation::ufixed1TOufixed4:
            result.ufixed4 = (uint32_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOufixed4:
            result.ufixed4 = (uint32_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOufixed4:
            //Redundant. Intentionally undefined.
            break;
        case Operation::ufixed8TOufixed4:
            result.ufixed4 = (uint32_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed4:
            result.ufixed4 = (uint32_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOufixed4:
            result.ufixed4 = (uint32_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOufixed4:
            result.ufixed4 = (uint32_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOufixed4:
            result.ufixed4 = (uint32_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed4:
            result.ufixed4 = (uint32_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOufixed4:
            result.ufixed4 = (uint32_t) top().float8;
            edit(result);
            break;

        //TOufixed8 casts.
        case Operation::ufixed1TOufixed8:
            result.ufixed8 = (uint64_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOufixed8:
            result.ufixed8 = (uint64_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed4;
            edit(result);
            break;
        case Operation::ufixed8TOufixed8:
            result.ufixed8 = (uint64_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed8:
            result.ufixed8 = (uint64_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOufixed8:
            result.ufixed8 = (uint64_t) top().float8;
            edit(result);
            break;

        //TOfixed1 casts.
        case Operation::ufixed1TOfixed1:
            result.fixed1 = (int8_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOfixed1:
            result.fixed1 = (int8_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOfixed1:
            result.fixed1 = (int8_t) top().fixed4;
            edit(result);
            break;
        case Operation::ufixed8TOfixed1:
            result.fixed1 = (int8_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfixed1:
            //Redundant. Intentionally undefined.
            break;
        case Operation::fixed2TOfixed1:
            result.fixed1 = (int8_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOfixed1:
            result.fixed1 = (int8_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOfixed1:
            result.fixed1 = (int8_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfixed1:
            result.fixed1 = (int8_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOfixed1:
            result.fixed1 = (int8_t) top().float8;
            edit(result);
            break;

        //TOfixed2 casts.
        case Operation::ufixed1TOfixed2:
            result.fixed2 = (int16_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOfixed2:
            result.fixed2 = (int16_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOfixed2:
            result.fixed2 = (int16_t) top().fixed4;
            edit(result);
            break;
        case Operation::ufixed8TOfixed2:
            result.ufixed8 = (int16_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed1TOfixed2:
            result.fixed2 = (int16_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOfixed2:
            //Redundant. Intentionally undefined.
            break;
        case Operation::fixed4TOfixed2:
            result.fixed2 = (int16_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOfixed2:
            result.fixed2 = (int16_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfixed2:
            result.fixed2 = (int16_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOfixed2:
            result.fixed2 = (int16_t) top().float8;
            edit(result);
            break;

        //TOfixed4 casts.
        case Operation::ufixed1TOfixed4:
            result.fixed4 = (int32_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOfixed4:
            result.fixed4 = (int32_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOfixed4:
            result.fixed4 = (int32_t) top().fixed4;
            edit(result);
            break;
        case Operation::ufixed8TOfixed4:
            result.fixed4 = (int32_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfixed4:
            result.fixed4 = (int32_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOfixed4:
            result.fixed4 = (int32_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOfixed4:
            //Redundant. Intentionally undefined.
            break;
        case Operation::fixed8TOfixed4:
            result.fixed4 = (int32_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfixed4:
            result.fixed4 = (int32_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOfixed4:
            result.fixed4 = (int32_t) top().float8;
            edit(result);
            break;

        //TOfixed8 casts.
        case Operation::ufixed1TOfixed8:
            result.fixed8 = (int64_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOfixed8:
            result.fixed8 = (int64_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOfixed8:
            result.fixed8 = (int64_t) top().fixed4;
            edit(result);
            break;
        case Operation::ufixed8TOfixed8:
            result.fixed8 = (int64_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfixed8:
            result.fixed8 = (int64_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOfixed8:
            result.fixed8 = (int64_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOfixed8:
            result.fixed8 = (int64_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOfixed8:
            //Redundant. Intentionally undefined.
            break;
        case Operation::float4TOfixed8:
            result.fixed8 = (int64_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOfixed8:
            result.fixed8 = (int64_t) top().float8;
            edit(result);
            break;

        //TOfloat4 casts.
        case Operation::ufixed1TOfloat4:
            result.float4 = (_Float32) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOfloat4:
            result.float4 = (_Float32) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOfloat4:
            result.float4 = (_Float32) top().fixed4;
            edit(result);
            break;
        case Operation::ufixed8TOfloat4:
            result.float4 = (_Float32) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfloat4:
            result.float4 = (_Float32) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOfloat4:
            result.float4 = (_Float32) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOfloat4:
            result.float4 = (_Float32) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOfloat4:
            result.float4 = (_Float32) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfloat4:
            //Redundant. Intentionally undefined.
            break;
        case Operation::float8TOfloat4:
            result.float4 = (_Float32) top().float8;
            edit(result);
            break;

        //TOfloat8 casts.
        case Operation::ufixed1TOfloat8:
            result.float8 = (uint64_t) top().ufixed1;
            edit(result);
            break;
        case Operation::ufixed2TOfloat8:
            result.float8 = (uint64_t) top().ufixed2;
            edit(result);
            break;
        case Operation::ufixed4TOfloat8:
            result.float8 = (uint64_t) top().fixed4;
            edit(result);
            break;
        case Operation::ufixed8TOfloat8:
            result.float8 = (uint64_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfloat8:
            result.float8 = (uint64_t) top().fixed1;
            edit(result);
            break;
        case Operation::fixed2TOfloat8:
            result.float8 = (uint64_t) top().fixed2;
            edit(result);
            break;
        case Operation::fixed4TOfloat8:
            result.float8 = (uint64_t) top().fixed4;
            edit(result);
            break;
        case Operation::fixed8TOfloat8:
            result.float8 = (uint64_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfloat8:
            result.float8 = (uint64_t) top().float4;
            edit(result);
            break;
        case Operation::float8TOfloat8:
            result.float8 = (uint64_t) top().float8;
            edit(result);
            break;

////fixed8 mathematical instructions.
        case Operation::fixed8add:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            //Operate and edit;
            result.fixed8 = left.fixed8 + right.fixed8;
            edit(result);
            break;
        case Operation::fixed8subtract:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            //Operate and edit;
            result.fixed8 = left.fixed8 - right.fixed8;
            edit(result);
            break;
        case Operation::fixed8negate:
            //Negate the value;
            computationVector[computationVector.size() - 1].fixed8 = -computationVector[computationVector.size() - 1].fixed8;
            break;
        case Operation::fixed8multiply:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            //Operate and push;
            result.fixed8 = left.fixed8 * right.fixed8;
            edit(result);
            break;
        case Operation::fixed8divide:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            //Operate and push;
            result.fixed8 = left.fixed8 / right.fixed8;
            edit(result);
            break;



////float8 mathematical operations
            case Operation::float8add:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.float8 = left.float8 + right.float8;
            push(result);
            break;
        case Operation::float8subtract:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.float8 = left.float8 - right.float8;
            push(result);
            break;
        case Operation::float8negate:
            //Negate the value;
            computationVector[computationVector.size() - 1].float8 = -computationVector[computationVector.size() - 1].float8;
            break;
        case Operation::float8multiply:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.float8 = left.float8 * right.float8;
            push(result);
            break;
        case Operation::float8divide:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.float8 = left.float8 / right.float8;
            push(result);
            break;



////Comparison instructions.
        case equals:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.fixed8 == right.fixed8;
            push(result);
            break;
        case nequals:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.fixed8 != right.fixed8;
            push(result);
            break;
        case greater:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.fixed8 > right.fixed8;
            push(result);
            break;
        case greateror:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.fixed8 >= right.fixed8;
            push(result);
            break;
        case lesser:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.fixed8 < right.fixed8;
            push(result);
            break;
        case lesseror:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.fixed8 <= right.fixed8;
            push(result);
            break;
        case ior:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.boolean or right.boolean;
            push(result);
            break;



//Boolean algebra
        case iand:
            //Get the right operand;
            right = top();
            pop();
            //Get the left operand;
            left = top();
            pop();
            //Operate and push;
            result.boolean = left.boolean and right.boolean;
            push(result);
            break;
        case inot:
            //Get the right operand;
            right = top();
            pop();
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
