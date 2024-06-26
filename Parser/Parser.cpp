#ifndef Parser_h__
#define Parser_h__

#include <vector>
#include "../Lexer/Token.hpp"

class Parser{
private:
//Iterator points to current token
    ulong iter = 0;
    std::vector<Token> given;

//0 priority
    bool p0();

//1 priority
    bool add();
    bool subtract();
    bool p1();

//2 priority
    bool multiply();
    bool divide();
    bool p2();

//3 priority
    bool exponent();
    bool p3();

//4 priority
    bool factorial();
    bool p4();

//5 priority
    bool parentheses();
    // bool brackets();
    // bool curly();
    bool negate();

//6 priority
    bool p5();

public:
bool parse(std::vector<Token> input);

};

bool Parser::p0(){
    if(add()){
        return true;
    }
    
    if(subtract()){
        return true;
    };

    if(p2()){
        return true;
    }

    return false;

}

bool Parser::p1(){
    if(multiply()){
        return true;
    }

    if(divide()){
        return true;
    }

    if(p2()){
        return true;
    }

    return false;

}

bool Parser::p2(){
    if(exponent()){
        return true;
    }

    if(p3()){
        return true;
    }

    return false;

}

bool Parser::p3(){
    if(factorial()){
        return true;
    }

    if(p4()){
        return true;
    }

    return false;

}

bool Parser::p4(){
    if(parentheses()){
        return true;
    }

    // if(brackets()){
    //     return true;
    // }

    // if(curly()){
    //     return true;
    // }

    if(p5()){
        return true;
    }

    return false;

}

bool Parser::p5(){
    if(given[iter].getType() != FloridaType::fix8){
        return false;
    } else {
        iter++;
        return true;
    }

}



bool Parser::add(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "+"){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p1()){
        goto fail;
    }

    iter++;

    //Check if right is a valid subexpression.
    if(!p0()){
        goto fail;
    }

    return true;
fail:
    iter = initial;
    return false;
}

bool Parser::subtract(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "-"){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p1()){
        goto fail;
    }

    iter++;

    //Check if right is a valid subexpression.
    if(!p0()){
        goto fail;
    }

    return true;
fail:
    iter = initial;
    return false;
}



bool Parser::multiply(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "*"){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p2()){
        goto fail;
    }

    iter++;

    //Check if right is a valid subexpression.
    if(!p1()){
        goto fail;
    }

    return true;
fail:
    iter = initial;
    return false;
}

bool Parser::divide(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "/"){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p2()){
        goto fail;
    }

    iter++;

    //Check if right is a valid subexpression.
    if(!p1()){
        goto fail;
    }

    return true;
fail:
    iter = initial;
    return false;
}



bool Parser::exponent(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "^"){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p5()){
        goto fail;
    }

    iter++;

    //Check if right is a valid subexpression.
    if(!p3()){
        goto fail;
    }

    return true;
fail:
    iter = initial;
    return false;
}



bool Parser::factorial(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "!"){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p5()){
        goto fail;
    }

    iter++;

    return true;
fail:
    iter = initial;
    return false;
}



bool Parser::parentheses(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "("){
        goto fail;
    }
    iter++;

    //Check if left is a valid subexpression.
    if(!p0()){
        goto fail;
    }

    //Check if right is a valid subexpression.
    if(given[iter].getName() != ")"){
        goto fail;
    }

    return true;
fail:
    iter = initial;
    return false;
}

// bool Parser::brackets(){
//     const ulong initial = iter;
//     //Check if this program has reached the end of the token stream.
//     if(given.size() - 1 <= iter){
//         goto fail;
//     }

//     //Check if the center of this subexpression is the "+" operator.
//     if(given[iter].getName() != "["){
//         goto fail;
//     }
//     iter++;

//     //Check if left is a valid subexpression.
//     if(!p0()){
//         goto fail;
//     }

//     //Check if right is a valid subexpression.
//     if(given[iter].getName() != "]"){
//         goto fail;
//     }

//     return true;

// fail:
//     iter = initial;
//     return false;
// }

// bool Parser::curly(){
//     const ulong initial = iter;
//     //Check if this program has reached the end of the token stream.
//     if(given.size() - 1 <= iter){
//         goto fail;
//     }

//     //Check if the center of this subexpression is the "+" operator.
//     if(given[iter].getName() != "{"){
//         goto fail;
//     }
//     iter++;

//     //Check if left is a valid subexpression.
//     if(!p0()){
//         goto fail;
//     }

//     //Check if right is a valid subexpression.
//     if(given[iter].getName() != "}"){
//         goto fail;
//     }

//     return true;

// fail:
//     iter = initial;
//     return false;
// }

bool Parser::negate(){
    ulong initial = iter;
    if(given[iter].getName() != "-"){
        return false;
        goto fail;
    }
    iter++;

    if(!p1()){
        return false;
        goto fail;
    }

    return true;

fail:
    iter = initial;
    return false;
}

#endif