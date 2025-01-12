#ifndef Operation_h__
#define Operation_h__

#include <stdint.h>
#include "../Lexer/Types.hpp"

enum Operation{
    fetch,
    cjump,
    jump,
    initialize,
    assign,
    push,
    multiply,
    divide,
    add,
    subtract,
    negate
};

union types{
    //Booleans.
    bool boolean;

    //Floating point values;
    double float64;
    float float32;

    //Fixed point values;
    int64_t fixed64;
    int32_t fixed32;
    int16_t fixed16;
    int8_t fixed8;

    //Unsigned fixed point values;
    uint64_t ufixed64;
    uint32_t ufixed32;
    uint16_t ufixed16;
    uint8_t ufixed8;
};

class Instruction{
public:
    FloridaType type;
    Operation oper;
    types literal;

    Instruction(Operation inOper, int64_t inPosition){
        oper = inOper;
        literal.fixed64 = inPosition;
    }

    Instruction(FloridaType inType, types inLiteral){
        type = inType;
        oper = Operation::fetch;
        literal = inLiteral;
    }

    Instruction(FloridaType inType, Operation inOperation, types inLiteral){
        type = inType;
        oper = inOperation;
        literal = inLiteral;
    }

};
#endif