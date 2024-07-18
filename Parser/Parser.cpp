#include "Parser.hpp"

bool Parser::parse(){
    return this->p0();
};


bool Parser::p0(){

    bool b1 = add();
    bool b2 = subtract();
    bool b3 = p1();

    return b1 | b2 | b3;

}

bool Parser::p1(){

    bool b1 = multiply();
    bool b2 = divide();
    bool b3 = p2();

    return b1 | b2 | b3;

}

bool Parser::p2(){

    bool b1 = exponent();
    bool b2 = p3();

    return b1 | b2;

}

bool Parser::p3(){

    bool b1 = factorial();
    bool b2 = p4();

    return b1 | b2;

}

bool Parser::p4(){

    bool b1 = parentheses();
    // bool b2 = brackets();
    // bool b3 = curly();
    bool b4 = negate();
    bool b5 = p5();

    return b1 | /*b2 | b3 |*/ b4 | b5;

}

bool Parser::p5(){
    if(given[iter].getType() != FloridaType::fix8){
        return false;
    } else {
        iter++;
        return true;
    }

}


//Priority 0
bool Parser::add(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 2 <= iter){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p1()){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "+"){
        goto fail;
    }
    iter++;

    if(given.size() <= iter){
        goto fail;
    }

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
    if(given.size() - 2 <= iter){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p1()){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "-"){
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


//Priority 1
bool Parser::multiply(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 2 <= iter){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p2()){
        goto fail;
    }

    //Check if the center of this subexpression is the "*" operator.
    if(given[iter].getName() != "*"){
        goto fail;
    }
    iter++;

    if(given.size() <= iter){
        goto fail;
    }

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
    if(given.size() - 2 <= iter){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "/"){
        goto fail;
    }
    iter++;

    //Check if left is a valid subexpression.
    if(!p2()){
        goto fail;
    }

    //Check if right is a valid subexpression.
    if(!p1()){
        goto fail;
    }

    return true;
fail:
    iter = initial;
    return false;
}


//Priority 2
bool Parser::exponent(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 2 <= iter){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p5()){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "^"){
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


//Priority 3
bool Parser::factorial(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 <= iter){
        goto fail;
    }

    //Check if left is a valid subexpression.
    if(!p5()){
        goto fail;
    }

    //Check for the "!" operator.
    if(given[iter].getName() != "!"){
        goto fail;
    }
    iter++;

    return true;
fail:
    iter = initial;
    return false;
}


//Priority 4
bool Parser::parentheses(){
    const ulong initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 2 <= iter){
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
    iter++;

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
    if(given.size() - 1 <= iter){
        goto fail;
    }

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