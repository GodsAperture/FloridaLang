#include "VM.hpp"

inline void FloridaVM::push(types& input){
    computationStack[stackSize - 1] = input;
    stackSize++;
}

inline void FloridaVM::pop(int64_t input){
    stackSize -= input;
}

inline void FloridaVM::pop(){
    stackSize--;
}

inline types& FloridaVM::top(){
    return computationStack[stackSize - 1];
}

inline void FloridaVM::copy(){
    push(computationStack[stackSize - 1]);
    stackSize++;
}

inline void FloridaVM::edit(types input){
    computationStack[stackSize - 1] = input;
}



void FloridaVM::infoPrint(){
    std::cout << "--Instruction number: " << instructionNumber << "\n";
    std::cout << "--Stack size: " << stackSize << "\n";
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

char FloridaVM::next(){
    //left, right, and resulting members for operations and convenience.
    types left;
    types right;
    types result;
    //byteOffset and stackOffset are for naming convenience.
    int64_t byteOffset = 0;
    int64_t stackOffset = 0;
    //These constants are important for fast modulo.
    const int64_t bitmask4 = 1;
    const int64_t bitmask2 = 3;
    const int64_t bitmask1 = 7;

    //Check to see if all of instructions have been executed.
    if(instructionNumber >= programInstructions.size()){
        return false;
    }

    //Other variables that exist for convenience.
    Instruction current = programInstructions[instructionNumber];
    switch (programInstructions[instructionNumber].oper){
        ////VM related instructions.
        case Operation::push:
            push(programInstructions[instructionNumber].literal);
            break;
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
            reference = stackSize - 1;
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
            computationStack[reference + 1] = result;
            //pop to the previous top of the stack.
            pop(stackSize - reference + 2);
            break;
        case Operation::cjump:
            //If it's true, then don't skip.
            instructionNumber += !top().boolean[0] * (current.literal.fixed8 - 1) + 1;
            pop();
            return true;
        case Operation::jump:
            instructionNumber = current.literal.fixed8;
            break;
        case Operation::initialize:
            stackSize++;
            break;



////Variable related instructions.
            //You might be asking "Why didn't I just move the whole union instead of part of the union?"
            //The reason why is this creates a security vulnerability called "Byte peeking."
            //If I give the user the full 8 bytes, there may be other bytes that the user
            //will be given that aren't part of what they requested.
            //So, I grab parts of the full union.
            //Is it slower? Yes. Is it safer? Yes.

//Fetch instructions
        //Bitshifting an int64_t by N is equivalent to dividing an int64_t by 2^N.
        //Bitmasking an int64_t using 2^N-1 is the equivalent to doing int64_t % 2^N.
        //These hold true for when N is natural number.
        case Operation::fetch1:
            break;
        case Operation::fetch2:
            break;
        case Operation::fetch4:
            break;
        case Operation::fetch8:
            break;
        


////Typecasting, in a different order.
        //TOufixed1 casts.
        case Operation::ufixed1TOufixed1:
            //Redundant. Intentionally undefined.
        case Operation::ufixed2TOufixed1:
            result.ufixed1[0] = (uint8_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOufixed1:
            result.ufixed1[0] = (uint8_t) top().ufixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOufixed1:
            result.ufixed1[0] = (uint8_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed1:
            result.ufixed1[0] = (uint8_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOufixed1:
            result.ufixed1[0] = (uint8_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOufixed1:
            result.ufixed1[0] = (uint8_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOufixed1:
            result.ufixed1[0] = (uint8_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed1:
            result.ufixed1[0] = (uint8_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOufixed1:
            result.ufixed1[0] = (uint8_t) top().float8;
            edit(result);
            break;

        //TOufixed2 casts.
        case Operation::ufixed1TOufixed2:
            result.ufixed2[0] = (uint16_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOufixed2:
            //Redundant. Intentionally undefined.
        case Operation::ufixed4TOufixed2:
            result.ufixed2[0] = (uint16_t) top().ufixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOufixed2:
            result.ufixed2[0] = (uint16_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed2:
            result.ufixed2[0] = (uint16_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOufixed2:
            result.ufixed2[0] = (uint16_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOufixed2:
            result.ufixed2[0] = (uint16_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOufixed2:
            result.ufixed2[0] = (uint16_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed2:
            result.ufixed2[0] = (uint16_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOufixed2:
            result.ufixed2[0] = (uint16_t) top().float8;
            edit(result);
            break;

        //TOufixed4 casts.
        case Operation::ufixed1TOufixed4:
            result.ufixed4[0] = (uint32_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOufixed4:
            result.ufixed4[0] = (uint32_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOufixed4:
            //Redundant. Intentionally undefined.
            break;
        case Operation::ufixed8TOufixed4:
            result.ufixed4[0] = (uint32_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed4:
            result.ufixed4[0] = (uint32_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOufixed4:
            result.ufixed4[0] = (uint32_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOufixed4:
            result.ufixed4[0] = (uint32_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOufixed4:
            result.ufixed4[0] = (uint32_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed4:
            result.ufixed4[0] = (uint32_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOufixed4:
            result.ufixed4[0] = (uint32_t) top().float8;
            edit(result);
            break;

        //TOufixed8 casts.
        case Operation::ufixed1TOufixed8:
            result.ufixed8 = (uint64_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOufixed8:
            result.ufixed8 = (uint64_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOufixed8:
            result.ufixed8 = (uint64_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOufixed8:
            result.ufixed8 = (uint64_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOufixed8:
            result.ufixed8 = (uint64_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOufixed8:
            result.ufixed8 = (uint64_t) top().float8;
            edit(result);
            break;

        //TOfixed1 casts.
        case Operation::ufixed1TOfixed1:
            result.fixed1[0] = (int8_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOfixed1:
            result.fixed1[0] = (int8_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOfixed1:
            result.fixed1[0] = (int8_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOfixed1:
            result.fixed1[0] = (int8_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfixed1:
            //Redundant. Intentionally undefined.
            break;
        case Operation::fixed2TOfixed1:
            result.fixed1[0] = (int8_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOfixed1:
            result.fixed1[0] = (int8_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOfixed1:
            result.fixed1[0] = (int8_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfixed1:
            result.fixed1[0] = (int8_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOfixed1:
            result.fixed1[0] = (int8_t) top().float8;
            edit(result);
            break;

        //TOfixed2 casts.
        case Operation::ufixed1TOfixed2:
            result.fixed2[0] = (int16_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOfixed2:
            result.fixed2[0] = (int16_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOfixed2:
            result.fixed2[0] = (int16_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOfixed2:
            result.ufixed8 = (int16_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed1TOfixed2:
            result.fixed2[0] = (int16_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOfixed2:
            //Redundant. Intentionally undefined.
            break;
        case Operation::fixed4TOfixed2:
            result.fixed2[0] = (int16_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOfixed2:
            result.fixed2[0] = (int16_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfixed2:
            result.fixed2[0] = (int16_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOfixed2:
            result.fixed2[0] = (int16_t) top().float8;
            edit(result);
            break;

        //TOfixed4 casts.
        case Operation::ufixed1TOfixed4:
            result.fixed4[0] = (int32_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOfixed4:
            result.fixed4[0] = (int32_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOfixed4:
            result.fixed4[0] = (int32_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOfixed4:
            result.fixed4[0] = (int32_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfixed4:
            result.fixed4[0] = (int32_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOfixed4:
            result.fixed4[0] = (int32_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOfixed4:
            //Redundant. Intentionally undefined.
            break;
        case Operation::fixed8TOfixed4:
            result.fixed4[0] = (int32_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfixed4:
            result.fixed4[0] = (int32_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOfixed4:
            result.fixed4[0] = (int32_t) top().float8;
            edit(result);
            break;

        //TOfixed8 casts.
        case Operation::ufixed1TOfixed8:
            result.fixed8 = (int64_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOfixed8:
            result.fixed8 = (int64_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOfixed8:
            result.fixed8 = (int64_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOfixed8:
            result.fixed8 = (int64_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfixed8:
            result.fixed8 = (int64_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOfixed8:
            result.fixed8 = (int64_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOfixed8:
            result.fixed8 = (int64_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOfixed8:
            //Redundant. Intentionally undefined.
            break;
        case Operation::float4TOfixed8:
            result.fixed8 = (int64_t) top().float4[0];
            edit(result);
            break;
        case Operation::float8TOfixed8:
            result.fixed8 = (int64_t) top().float8;
            edit(result);
            break;

        //TOfloat4 casts.
        case Operation::ufixed1TOfloat4:
            result.float4[0] = (_Float32) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOfloat4:
            result.float4[0] = (_Float32) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOfloat4:
            result.float4[0] = (_Float32) top().fixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOfloat4:
            result.float4[0] = (_Float32) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfloat4:
            result.float4[0] = (_Float32) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOfloat4:
            result.float4[0] = (_Float32) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOfloat4:
            result.float4[0] = (_Float32) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOfloat4:
            result.float4[0] = (_Float32) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfloat4:
            //Redundant. Intentionally undefined.
            break;
        case Operation::float8TOfloat4:
            result.float4[0] = (_Float32) top().float8;
            edit(result);
            break;

        //TOfloat8 casts.
        case Operation::ufixed1TOfloat8:
            result.float8 = (uint64_t) top().ufixed1[0];
            edit(result);
            break;
        case Operation::ufixed2TOfloat8:
            result.float8 = (uint64_t) top().ufixed2[0];
            edit(result);
            break;
        case Operation::ufixed4TOfloat8:
            result.float8 = (uint64_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::ufixed8TOfloat8:
            result.float8 = (uint64_t) top().ufixed8;
            edit(result);
            break;
        case Operation::fixed1TOfloat8:
            result.float8 = (uint64_t) top().fixed1[0];
            edit(result);
            break;
        case Operation::fixed2TOfloat8:
            result.float8 = (uint64_t) top().fixed2[0];
            edit(result);
            break;
        case Operation::fixed4TOfloat8:
            result.float8 = (uint64_t) top().fixed4[0];
            edit(result);
            break;
        case Operation::fixed8TOfloat8:
            result.float8 = (uint64_t) top().fixed8;
            edit(result);
            break;
        case Operation::float4TOfloat8:
            result.float8 = (uint64_t) top().float4[0];
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
            computationStack[stackSize - 1].fixed8 = -computationStack[stackSize - 1].fixed8;
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
            computationStack[stackSize - 1].float8 = -computationStack[stackSize - 1].float8;
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
            result.boolean[0] = left.fixed8 == right.fixed8;
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
            result.boolean[0] = left.fixed8 != right.fixed8;
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
            result.boolean[0] = left.fixed8 > right.fixed8;
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
            result.boolean[0] = left.fixed8 >= right.fixed8;
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
            result.boolean[0] = left.fixed8 < right.fixed8;
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
            result.boolean[0] = left.fixed8 <= right.fixed8;
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
            result.boolean[0] = left.boolean[0] or right.boolean[0];
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
            result.boolean[0] = left.boolean[0] and right.boolean[0];
            push(result);
            break;
        case inot:
            //Get the right operand;
            right = top();
            pop();
            //Operate and push;
            result.boolean[0] = !right.boolean[0];
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
