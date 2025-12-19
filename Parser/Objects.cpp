#include "Node.hpp"

inline std::string padding(std::string input){
    for(size_t i = input.size(); i < 12; i++){
        input += " ";
    }

    return input;

}



//Float8
    Float8::Float8(){
        type = FloridaType::float8;
    }

    std::string Float8::ToString(std::string inLeft, std::string inRight){
        return "\x1b[37m" + std::to_string(value) + "\x1b[0m";
    }

    std::string Float8::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Float8::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal.float8 = value;
        result.oper = Operation::push;
        result.type = FloridaType::float8;

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



//Float4
    Float4::Float4(){
        type = FloridaType::float4;
    }

    std::string Float4::ToString(std::string inLeft, std::string inRight){
        return std::to_string(value);
    }

    std::string Float4::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Float4::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal.float4 = value;
        result.oper = Operation::push;
        result.type = FloridaType::float4;

        inInstructions.push_back(result);
    }

    Node* Float4::copy(StackAllocator& input){
        Float4* result = input.alloc<Float4>();
        result->value = value;

        return result;
    }

    Float4* Float4::pcopy(StackAllocator& input){
        Float4* result = input.alloc<Float4>();
        result->value = value;

        return result;
    }



//Fixed8
    Fixed8::Fixed8(){
        type = FloridaType::fixed8;
    };

    std::string Fixed8::ToString(std::string inLeft, std::string inRight){
        return "\x1b[37m" + std::to_string(value) + "\x1b[0m";
    }

    std::string Fixed8::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Fixed8::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal.fixed8 = value;
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



//Fixed4
    Fixed4::Fixed4(){
        type = FloridaType::fixed4;
    }

    std::string Fixed4::ToString(std::string inLeft, std::string inRight){
        return std::to_string(value);
    }

    std::string Fixed4::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Fixed4::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal.fixed4 = value;
        result.oper = Operation::push;
        result.type = FloridaType::fixed4;

        inInstructions.push_back(result);
    }

    Node* Fixed4::copy(StackAllocator& input){
        Fixed4* result = input.alloc<Fixed4>();
        result->value = value;

        return result;
    }

    Fixed4* Fixed4::pcopy(StackAllocator& input){
        Fixed4* result = input.alloc<Fixed4>();
        result->value = value;

        return result;
    }



//Fixed2
    Fixed2::Fixed2(){
        type = FloridaType::fixed2;
    }

    std::string Fixed2::ToString(std::string inLeft, std::string inRight){
        return std::to_string(value);
    }

    std::string Fixed2::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Fixed2::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal.fixed2 = value;
        result.oper = Operation::push;
        result.type = FloridaType::fixed2;

        inInstructions.push_back(result);
    }

    Node* Fixed2::copy(StackAllocator& input){
        Fixed2* result = input.alloc<Fixed2>();
        result->value = value;

        return result;
    }

    Fixed2* Fixed2::pcopy(StackAllocator& input){
        Fixed2* result = input.alloc<Fixed2>();
        result->value = value;

        return result;
    }



//Fixed1
    Fixed1::Fixed1(){
        type = FloridaType::fixed1;
    }

    std::string Fixed1::ToString(std::string inLeft, std::string inRight){
        return std::to_string(value);
    }

    std::string Fixed1::printAll(){
        return padding("push") + std::to_string(value) + "\n";
    }

    void Fixed1::FLVMCodeGen(std::vector<Instruction>& inInstructions){
        Instruction result = Instruction();
        result.literal.fixed1 = value;
        result.oper = Operation::push;
        result.type = FloridaType::fixed1;

        inInstructions.push_back(result);
    }

    Node* Fixed1::copy(StackAllocator& input){
        Fixed1* result = input.alloc<Fixed1>();
        result->value = value;

        return result;
    }

    Fixed1* Fixed1::pcopy(StackAllocator& input){
        Fixed1* result = input.alloc<Fixed1>();
        result->value = value;

        return result;
    }



//Boolean
    Boolean::Boolean(){
        type = FloridaType::Bool;
    };

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
