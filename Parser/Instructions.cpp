#ifndef Operation_h__
#define Operation_h__

#include <stdint.h>
#include "../Lexer/Types.hpp"
#include "../Lexer/Token.hpp"

class Scope;

//2 byte representation for all operations.
enum Operation : int16_t {
    //`NewScope`, `whichScope`, `variableSlotSize`
    NewScope,
    //`DeleteScope`, `fixed8`
    DeleteScope,
    //`ICall`, `fixed8`
    ICall,
    //`IReturn`, `fixed8`
    IReturn,
    //`CJump`, `fixed8`
    CJump,
    //`Jump`, `fixed8`
    Jump,

    //`pointer`, `fetch1`, `whichScope`, `stackOffset`
    //`pointer` will be in the stack.
    fetch1,
    //`fetch2`, `whichScope`, `stackOffset`
    fetch2,
    //`fetch4`, `whichScope`, `stackOffset`
    fetch4,
    //`fetch8`, `whichScope`, `stackOffset`
    fetch8,

    //`assign1`, `whichScope`, `stackOffset`
    assign1,
    //`assign2`, `whichScope`, `stackOffset`
    assign2,
    //`assign4`, `whichSCope`, `stackOffset`
    assign4,
    //`assign8`, `whichScope`, `stackOffset`
    assign8,

    //`Push`, `type`, `primitive`
    Push,
    //`Pop`, `fixed8`.
    //The fixed8 determines how many elements are popped.
    Pop,

    //`ITypecast`, starting type, ending type
    ITypecast,

    //All math instructions are also organized as such so I
    //can use a math trick to get the correct instruction.
    
    //`IMultiply`, type
    IMultiply,
    //`IDivide`, type
    IDivide,
    //`IAdd`, type
    IAdd,
    //`ISubtract`, type
    ISubtract,
    //`INegate`
    INegate,
    //`IExponent`
    IExponent,

    //Object comparisons
    equals,
    nequals,
    greater,
    greateror,
    lesser,
    lesseror,

    //Boolean algebra
    //i is short for instruction
    //i is also used because these are actual C++ operators.
    ior,
    iand,
    inot
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