#include "Node.hpp"

inline std::string padding(std::string input){
    for(size_t i = input.size(); i < 12; i++){
        input += " ";
    }

    return input;

}

//Fixed64
    Fixed8::Fixed8(std::string input){
        value = std::stoll(input);
    }

    std::string Fixed8::ToString(std::string inLeft, std::string inRight){
        return "\x1b[37m" + std::to_string(value) + "\x1b[0m";
    }

    std::string Fixed8::printAll(){
        return padding("push") + std::to_string(value) + "\n";
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
            return "\x1b[32mtrue\x1b[0m";
        } else {
            return "\x1b[31mfalse\x1b[0m";
        }
    }

    std::string Boolean::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Boolean::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        types thing;
        thing.boolean = value;

        inInstructions.push_back(Instruction(FloridaType::Bool, Operation::push, thing));
    }