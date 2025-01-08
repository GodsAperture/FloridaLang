#include "Node.hpp"

//Fixed64
    Fixed64::Fixed64(std::string input){
        value = std::stoll(input);
    }

    std::string Fixed64::ToString(){
        return std::to_string(value);
    }

    void Fixed64::GetVariables(std::vector<Association>& inVector){
        //Null
    }

    void Fixed64::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        inInstructions.push_back(Instruction(Operation::push, value));
    }

    int64_t Fixed64::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }



//Boolean
    Boolean::Boolean(bool inBool){
        value = inBool;
    }

    std::string Boolean::ToString(){
        return std::to_string(value);
    }

    void Boolean::GetVariables(std::vector<Association>& inVector){
        //Do nothing.
    }

    void Boolean::FLVMCodeGen(std::vector<Instruction>& inInstructions, std::vector<Association>& inVariables){
        types thing;
        thing.boolean = value;

        inInstructions.push_back(Instruction(FloridaType::Bool, thing));
    }

    int64_t Boolean::GetPosition(std::vector<Association>& inVariables){
        return -1;
    }
