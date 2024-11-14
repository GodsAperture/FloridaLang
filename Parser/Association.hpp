module;

export module Association;
#include "Node.hpp"

export class Association{
public:
    Variable token;
    int64_t position;

    Association(Variable inVariable, int64_t inPosition){
        token = inVariable;
        position = inPosition;
    }
};