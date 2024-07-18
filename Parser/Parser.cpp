#include "Parser.hpp"

bool Parser::parse() {
    return this->p0() && iter == this->given.size();
};

// || is a short circuiting boolean or, in other words
// given a || b if expression a is met then b is never evaluated
bool Parser::p0() {
    return add() || subtract() || p1();
}

bool Parser::p1() {
    return multiply() || divide() || p2();
}

bool Parser::p2() {
    return exponent() || p3();
}

bool Parser::p3() {
    return factorial() || p4();
}

bool Parser::p4() {
    return parentheses() || /*brackets() || curly() ||*/ negate() || p5();
}

bool Parser::p5(){
    if(given[iter].getType() != FloridaType::fix8)
        return false;

    iter++;
    return true;
}


//Priority 0
bool Parser::add(){
    const uint64_t initial = iter;
    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

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
    const uint64_t initial = iter;
    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

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
    const uint64_t initial = iter;
    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

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
    const uint64_t initial = iter;
    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

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
    const uint64_t initial = iter;
    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

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
    const uint64_t initial = iter;
    //Check if this program has reached the end of the token stream.
    if(given.size() - 1 >= iter){
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
    const uint64_t initial = iter;
    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

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
//     const uint64_t initial = iter;
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
//     const uint64_t initial = iter;
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
    uint64_t initial = iter;
    if(given.size() - 1 >= iter){
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