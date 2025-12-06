#include "Node.hpp"

inline std::string padding(std::string input){
    for(size_t i = input.size(); i < 12; i++){
        input += " ";
    }

    return input;

}



//Float8
    Float8::Float8(){
        //Do nothing
    }

    std::string Float8::ToString(std::string inLeft, std::string inRight){
        return "\x1b[37m" + std::to_string(value) + "\x1b[0m";
    }

    std::string Float8::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Float8::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal.float64 = value;
        result.oper = Operation::push;
        result.type = FloridaType::fixed8;

        inInstructions.push_back(result);
    }

    Node* Float8::copy(StackAllocator& input){
        Float8* thisptr = input.alloc<Float8>();

        thisptr->value = value;

        return thisptr;
    }

    Float8* Float8::pcopy(StackAllocator& input){
        Float8* thisptr = input.alloc<Float8>();

        thisptr->value = value;

        return thisptr;  
    }



//Fixed8
    Fixed8::Fixed8(){
        //Do nothing
    };

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
        Instruction result = Instruction();
        result.literal.fixed64 = value;
        result.oper = Operation::push;
        result.type = FloridaType::fixed8;

        inInstructions.push_back(result);
    }

    Node* Fixed8::copy(StackAllocator& input){
        Fixed8* thisptr = input.alloc<Fixed8>();

        thisptr->value = value;

        return thisptr;
    }

    Fixed8* Fixed8::pcopy(StackAllocator& input){
        Fixed8* thisptr = input.alloc<Fixed8>();

        thisptr->value = value;

        return thisptr;        
    }



//Boolean
    Boolean::Boolean(){
        //Do nothing
    };

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

    Node* Boolean::copy(StackAllocator& input){
        Boolean* thisptr = input.alloc<Boolean>();

        thisptr->value = value;

        return thisptr;
    }

    Boolean* Boolean::pcopy(StackAllocator& input){
        Boolean* thisptr = input.alloc<Boolean>();

        thisptr->value = value;

        return thisptr;        
    }