#include "Node.hpp"

//Fixed64
    Fixed8::Fixed8(std::string input){
        value = std::stoll(input);
    }

    std::string Fixed8::ToString(std::string inLeft, std::string inRight){
        return std::to_string(value);
    }

    void Fixed8::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        inInstructions.push_back(Instruction(Operation::push, value));
    }



//Boolean
    Boolean::Boolean(bool inBool){
        value = inBool;
    }

    std::string Boolean::ToString(std::string inLeft, std::string inRight){
        if(value){
            return "true";
        } else {
            return "false";
        }
    }

    void Boolean::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        types thing;
        thing.boolean = value;

        inInstructions.push_back(Instruction(FloridaType::Bool, Operation::push, thing));
    }