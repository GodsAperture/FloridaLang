#ifndef Operation_h__
#define Operation_h__

#include <stdint.h>
#include "../Lexer/Types.hpp"
#include "../Lexer/Token.hpp"

class Scope;

//2 byte representation for all operations.
//All operations will start with the letter I, shortened from Instruction.
enum Operation : int16_t {
    //`NewScope`, `whichScope`, `variableSlotSize`
    //Generates a new scope associated with `whichScope` and preallocates memory the size of `variableSlotSize`.
    INewScope,
    //`DeleteScope`
    IDeleteScope,
    //`ICall`, `fixed8`
    //Calls the function associated with the number `fixed8`.
    ICall,
    //`IReturn`, `fixed8`
    //Exits `fixed8` scopes.
    IReturn,
    //`CJump`, `fixed8`
    //Conditionally jumps `fixed8` positions forward in the instruction set.
    ICJump,
    //`Jump`, `fixed8`
    //Jumps to the `fixed8` position in the instruction set.
    IJump,

    //`IFetch1`, `whichScope`, `stackOffset`
    IFetch1,
    //`IFetch2`, `whichScope`, `stackOffset`
    IFetch2,
    //`IFetch4`, `whichScope`, `stackOffset`
    IFetch4,
    //`IFetch8`, `whichScope`, `stackOffset`
    IFetch8,
    //IFetchN will essentially "memcpy" `size` elements from within the stack to the top of the stack.
    //`IFetchN`, `whichScope`, `stackOffset`, `size`
    IFetchN,

    //`IAssign1`, `whichScope`, `stackOffset`
    IAssign1,
    //`IAssign2`, `whichScope`, `stackOffset`
    IAssign2,
    //`IAssign4`, `whichSCope`, `stackOffset`
    //stack: `pointer`  
    IAssign4,
    //`IAssign8`, `whichScope`, `stackOffset`
    IAssign8,
    //IAssignN will essentially "memcpy" `size` elements from the top of the stack to within the stack.
    //`IAssingN`, `whichScope`, `stackOffset`, `size`
    IAssignN,

    //`IHeapFetch1`, `index`
    //stack: `pointer`
    IHeapFetch1,
    //`IHeapFetch2`, `index`
    //stack: `pointer`
    IHeapFetch2,
    //`IHeapFetch4`, `index`
    //stack: `pointer`
    IHeapFetch4,
    //`IHeapFetch8`
    //stack: `pointer`
    IHeapFetch8,
    //IHeapFetchN will essentially "memcpy" `size` elements from the `pointer` to the top of the stack.
    //`IHeapFetchN`, `size`
    //stack: `pointer`
    IHeapFetchN,

    //`IHeapAssign1`, `index`
    //stack: `pointer`
    IHeapAssign1,
    //`IHeapAssign2`, `index`
    //stack: `pointer`
    IHeapAssign2,
    //`IHeapAssign4`, `index`
    //stack: `pointer`
    IHeapAssign4,
    //`IHeapAssign8`
    //stack: `pointer`
    IHeapAssign8,
    //IHeapAssignN will essentially "memcpy" `size` elements from the top of the stack to the `pointer`.
    //`IHeapFetchN`, `size`
    //stack: `pointer`
    IHeapAssignN,

    //`Push`, `type`, `primitive`
    //Pushes the `primitive` to the stack. `type` is only for debugging.
    IPush,
    //`Pop`, `fixed8`.
    //The fixed8 determines how many elements are popped.
    IPop,

    //`ITypecast`, `FloridaType`, `FloridaType`
    //The `types` object is cast from the first type to the second type.
    ITypecast,
    
    //`IMultiply`, `FloridaType`, `FloridaType`
    IMultiply,
    //`IDivide`, `FloridaType`, `FloridaType`
    IDivide,
    //`IAdd`, `FloridaType`, `FloridaType`
    IAdd,
    //`ISubtract`, `FloridaType`, `FloridaType`
    ISubtract,
    //`INegate`
    INegate,
    //`IExponent`
    IExponent,

    //Object comparisons
    IEquals,
    INEquals,
    IGreater,
    IGreaterOr,
    ILesser,
    ILesserOr,

    //Boolean algebra
    //i is short for instruction
    //i is also used because these are actual C++ operators.
    IOr,
    IAnd,
    INot
};

//A convenient 8 byte section of memory that can represent everything I need.
//Arrays of `types` will be used to represent objects in the stack and heap.
union types{
    //Booleans.
    bool boolean[8];

    //Floating point values;
    double float8;
    float float4[2];

    //Fixed point values;
    int64_t fixed8;
    int32_t fixed4[2];
    int16_t fixed2[4];
    int8_t fixed1[8];

    //Unsigned fixed point values;
    uint64_t ufixed8;
    uint32_t ufixed4[2];
    uint16_t ufixed2[4];
    uint8_t ufixed1[8];

    //Information
    FloridaType type[4];
    Operation operation[4];

    //Heap thrown objects.
    types* object;

    types(){
        object = nullptr;
    }
};

//Instruction debugger exists to write clean IR code to the terminal.
class InstructionsDebugger{
public:
    std::string_view name;
    types* body = nullptr;
    std::string comment = "";
    std::string_view end;
    //This is measured in multiples of 8.
    uint64_t byteCount = 0;
    uint64_t maxByteCount = 0;
    InstructionsDebugger* next = nullptr;

    InstructionsDebugger(){
        //Do nothing.
    };

    void push(types input){
        if(byteCount < maxByteCount){
            body[byteCount] = input;
        } else {
            body = (types*) realloc(body, 2 * maxByteCount);
            maxByteCount *= 2;
            body[byteCount] = input;
            byteCount++;
        }
    }
};

//The class that will contain all of the instructions for the virtual machine.
class Instructions{
public:
    types* instructionSet = nullptr;    
    InstructionsDebugger* debuggedIR = nullptr;
    uint64_t instructionCount = 0;
    uint64_t maxInstructionCount = 0;
    //Normally this would be a 
    uint64_t currentInstruction = 0;

    //`input` is for the number of instructions.
    //This will change dynamically and then after finishing
    //excess memory will be freed up.
    Instructions(uint64_t input){
        //This guarantees that maxByteCount is a multiple of 8.
        maxInstructionCount = (7L & (8 - (7L & input))) + input;
        //Generate a region for the bytecode.
        instructionSet = (types*) malloc(maxInstructionCount);
    }

    //Adds the `types` to the instruction set.
    void push(types input){
        if(instructionCount < maxInstructionCount){
            instructionSet[instructionCount] = input;
        } else {
            instructionSet = (types*) realloc(instructionSet, 2 * maxInstructionCount);
            maxInstructionCount *= 2;
            instructionSet[instructionCount] = input;
        }
        instructionCount++;
    }

    //Returns the current instruction in the instruction set and increments.
    inline types next(){
        return instructionSet[currentInstruction++];
    }

    //Only grab `types` of sizes 1, 2, 4, and 8.
    inline types grab(uint64_t size){
        types result;

        for(uint64_t i = 0; i < size; i++){
            result.fixed1[i] = instructionSet[instructionCount >> 3].fixed1[7L & instructionCount];
            instructionCount++;
        }

        return result;
    }

};
#endif