#ifndef Operation_h__
#define Operation_h__

#include <stdint.h>
#include "../Lexer/Types.hpp"
#include "../Lexer/Token.hpp"

class Scope;

enum Operation{
    newScope,
    deleteScope,
    call,
    ireturn,
    cjump,
    jump,

    //gfetch = global variable fetch
    //mfetch = middle of the stack variable fetch
    //lfetch = local scope variable fetch
    //hfetch = heap variable fetch

    //1 byte fetch requests
    gfetch1,
    mfetch1,
    lfetch1,
    hfetch1,

    //2 byte fetch requests
    gfetch2,
    mfetch2,
    lfetch2,
    hfetch2,

    //4 byte fetch requests
    gfetch4,
    mfetch4,
    lfetch4,
    hfetch4,

    //8 byte fetch requests
    gfetch8,
    mfetch8,
    lfetch8,
    hfetch8,

    //gassign = global variable assign
    //massign = middle of the stack variable assign
    //lassign = local scope variable assign
    //hassign = heap variable assign

    //1 byte assign requests
    gassign1,
    massign1,
    lassign1,
    hassign1,

    //2 byte assign requests
    gassign2,
    massign2,
    lassign2,
    hassign2,

    //4 byte assign requests
    gassign4,
    massign4,
    lassign4,
    hassign4,

    //8 byte assign requests
    gassign8,
    massign8,
    lassign8,
    hassign8,

    //`Initialize` will have a value to determine how many slots to open.
    initialize,
    //`Push` will contain the literal to be pushed.
    push,
    //`Pop` will have a value to determine how many slots to move back.
    pop,

    //All type casts
    //I'm writing them in this order take advantage of a math property.
    //There are dead instructions here, but it will make it easier for me
    //to determine which one to use.
    //I can use the FloridaTypes' numbers as a linear combination.
    //Unfortunately, this also means I have to define ALL of them in VM.
    //C'est la vie.

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

    //ufixed1 math instructions
    ufixed1multiply,
    ufixed1divide,
    ufixed1add,
    ufixed1subtract,
    ufixed1negate,

    //ufixed2 math instructions
    ufixed2multiply,
    ufixed2divide,
    ufixed2add,
    ufixed2subtract,
    ufixed2negate,

    //ufixed4 math instructions
    ufixed4multiply,
    ufixed4divide,
    ufixed4add,
    ufixed4subtract,
    ufixed4negate,

    //ufixed8 math instructions
    ufixed8multiply,
    ufixed8divide,
    ufixed8add,
    ufixed8subtract,
    ufixed8negate,

    //fixed1 math instructions
    fixed1multiply,
    fixed1divide,
    fixed1add,
    fixed1subtract,
    fixed1negate,

    //fixed2 math instructions
    fixed2multiply,
    fixed2divide,
    fixed2add,
    fixed2subtract,
    fixed2negate,

    //fixed4 math instructions
    fixed4multiply,
    fixed4divide,
    fixed4add,
    fixed4subtract,
    fixed4negate,

    //fixed8 math instructions
    fixed8multiply,
    fixed8divide,
    fixed8add,
    fixed8subtract,
    fixed8negate,

    //float4 math isntuctions
    float4multiply,
    float4divide,
    float4add,
    float4subtract,
    float4negate,

    //float8 math instructions
    float8multiply,
    float8divide,
    float8add,
    float8subtract,
    float8negate,

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

    //Heap thrown objects.
    types* object;

    types(){
        object = nullptr;
    }
};  

class Instruction{
public:
    FloridaType type;
    Operation oper;
    types literal;

    Instruction(){
        //Do nothing, just make the compiler stop complaining.
    }

    Instruction(Operation inOper){
        oper = inOper;
    }

    Instruction(Operation inOper, int64_t inPosition){
        oper = inOper;
        literal.fixed8 = inPosition;
    }

    Instruction(FloridaType inType, Operation inOperation, types inLiteral){
        type = inType;
        oper = inOperation;
        literal = inLiteral;
    }

};
#endif