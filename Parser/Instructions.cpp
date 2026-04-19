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

    //`fetch1`, `whichScope`, `stackOffset`
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

    //All type casts
    //I'm writing them in this order take advantage of a math property.
    //There are dead instructions here, but it will make it easier for me
    //to determine which one to use.
    //I can use the FloridaTypes' numbers as a linear combination.
    //Unfortunately, this also means I have to define ALL of them in VM.
    //C'est la vie.

    //`ITypecast`, starting type, ending type
    ITypecast,

    //All ufixed1TO casts.
    ufixed1TOufixed1,
    ufixed1TOufixed2,
    ufixed1TOufixed4,
    ufixed1TOufixed8,
    ufixed1TOfixed1,
    ufixed1TOfixed2,
    ufixed1TOfixed4,
    ufixed1TOfixed8,
    ufixed1TOfloat4,
    ufixed1TOfloat8,

    //All ufixed2TO casts.
    ufixed2TOufixed1,
    ufixed2TOufixed2,
    ufixed2TOufixed4,
    ufixed2TOufixed8,
    ufixed2TOfixed1,
    ufixed2TOfixed2,
    ufixed2TOfixed4,
    ufixed2TOfixed8,
    ufixed2TOfloat4,
    ufixed2TOfloat8,

    //All ufixed4TO casts.
    ufixed4TOufixed1,
    ufixed4TOufixed2,
    ufixed4TOufixed4,
    ufixed4TOufixed8,
    ufixed4TOfixed1,
    ufixed4TOfixed2,
    ufixed4TOfixed4,
    ufixed4TOfixed8,
    ufixed4TOfloat4,
    ufixed4TOfloat8,

    //All ufixed8TO casts.
    ufixed8TOufixed1,
    ufixed8TOufixed2,
    ufixed8TOufixed4,
    ufixed8TOufixed8,
    ufixed8TOfixed1,
    ufixed8TOfixed2,
    ufixed8TOfixed4,
    ufixed8TOfixed8,
    ufixed8TOfloat4,
    ufixed8TOfloat8,

    //All fixed1TO casts.
    fixed1TOufixed1,
    fixed1TOufixed2,
    fixed1TOufixed4,
    fixed1TOufixed8,
    fixed1TOfixed1,
    fixed1TOfixed2,
    fixed1TOfixed4,
    fixed1TOfixed8,
    fixed1TOfloat4,
    fixed1TOfloat8,

    //All fixed2TO casts.
    fixed2TOufixed1,
    fixed2TOufixed2,
    fixed2TOufixed4,
    fixed2TOufixed8,
    fixed2TOfixed1,
    fixed2TOfixed2,
    fixed2TOfixed4,
    fixed2TOfixed8,
    fixed2TOfloat4,
    fixed2TOfloat8,

    //All fixed4TO casts.
    fixed4TOufixed1,
    fixed4TOufixed2,
    fixed4TOufixed4,
    fixed4TOufixed8,
    fixed4TOfixed1,
    fixed4TOfixed2,
    fixed4TOfixed4,
    fixed4TOfixed8,
    fixed4TOfloat4,
    fixed4TOfloat8,

    //All fixed8TO casts.
    fixed8TOufixed1,
    fixed8TOufixed2,
    fixed8TOufixed4,
    fixed8TOufixed8,
    fixed8TOfixed1,
    fixed8TOfixed2,
    fixed8TOfixed4,
    fixed8TOfixed8,
    fixed8TOfloat4,
    fixed8TOfloat8,

    //All float4TO casts.
    float4TOufixed1,
    float4TOufixed2,
    float4TOufixed4,
    float4TOufixed8,
    float4TOfixed1,
    float4TOfixed2,
    float4TOfixed4,
    float4TOfixed8,
    float4TOfloat4,
    float4TOfloat8,

    //All float8TO casts.
    float8TOufixed1,
    float8TOufixed2,
    float8TOufixed4,
    float8TOufixed8,
    float8TOfixed1,
    float8TOfixed2,
    float8TOfixed4,
    float8TOfixed8,
    float8TOfloat4,
    float8TOfloat8,

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