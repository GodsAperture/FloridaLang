#ifndef Operation_h__
#define Operation_h__
#include <stdint.h>

enum Operation{
    push,
    multiply,
    divide,
    add,
    subtract
};

class Instruction{
public:
    Operation instruction;
    int64_t literal;

    Instruction(int64_t input){
        instruction = push;
        literal = input;
    }

    Instruction(Operation input){
        instruction = input;
    }

};
#endif