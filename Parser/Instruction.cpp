#ifndef Operation_h__
#define Operation_h__
#include <stdint.h>

enum Operation{
    initialize,
    assign,
    push,
    multiply,
    divide,
    add,
    subtract,
    negate
};

class Instruction{
public:
    Operation oper;
    int64_t literal;

    Instruction(Operation input){
        oper = input;
    }

    Instruction(int64_t input){
        literal = input;
    }
};
#endif