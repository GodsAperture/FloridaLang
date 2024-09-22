#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"

bool Parser::parse(){
    //If it's not the nullptr, then it's successful.
    Node* tree = this->p0();
    bool isSuccessful = (tree != nullptr);
    //Print the statement back out to see if it even works.
    //I sure hope it does.
    std::cout << tree->ToString();
    return isSuccessful && iter == this->given.size();
};

// || is a short circuiting boolean or, in other words
// given a || b if expression a is met then b is never evaluated
Node* Parser::p0(){

    Node* thisNode;

    thisNode = add();
    if(thisNode != nullptr){
        return thisNode;
    }

    thisNode = subtract();
    if(thisNode != nullptr){
        return thisNode;
    }

    thisNode = p1();
    if(thisNode != nullptr){
        return thisNode;
    }

    return nullptr;
}

Node* Parser::p1(){
    Node* thisNode = nullptr;

    thisNode = multiply();
    if(thisNode != nullptr){
        return thisNode;
    }

    thisNode = divide();
    if(thisNode != nullptr){
        return thisNode;
    }

    if(thisNode != nullptr){
        return thisNode;
    }

    return nullptr;
}

Node* Parser::p2(){
    Node* thisNode = nullptr; 

    thisNode = exponent();
    if(thisNode != nullptr){
        return thisNode;
    }
    
    thisNode = p3();
    if(thisNode != nullptr){
        return thisNode;
    }

    return nullptr;
}

Node* Parser::p3(){
    Node* thisNode = nullptr;

    thisNode = factorial();
    if(thisNode != nullptr){
        return thisNode;
    }

    thisNode = p4();
    if(thisNode != nullptr){
        return thisNode;
    }

    return nullptr;
}

Node* Parser::p4(){
    Node* thisNode = nullptr;

    thisNode = parentheses();
    if(thisNode != nullptr){
        return thisNode;
    }

    // thisNode = brackets();
    // if(thisNode != nullptr){
    //     return thisNode;
    // }

    // thisNode = curly();
    // if(thisNode != nullptr){
    //     return thisNode;
    // }

    thisNode = negate();
    if(thisNode != nullptr){
        return thisNode;
    }

    thisNode = p5();
    if(thisNode != nullptr){
        return thisNode;
    }

    return nullptr;
}

Node* Parser::p5(){
    if(given[iter].getType() != FloridaType::fix8){
        return nullptr;
    }   

    iter++;
    return stack->alloc<fixed64>(given[iter].getName());
}


//Priority 0
Node* Parser::add(){
    const uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2){
        goto fail;
    }

    left = p1();
    //Check if left is a valid subexpression.
    if(left == nullptr){
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

    right = p0();
    //Check if right is a valid subexpression.
    if(right == nullptr){
        goto fail;
    }

    return stack->alloc<Add>(left, right);

fail:
    iter = initial;
    stack->dealloc<Node>(thisNode);
    return nullptr;
}

Node* Parser::subtract(){
    const uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2){
        goto fail;
    }

    left = p1();
    //Check if left is a valid subexpression.
    if(left == nullptr){
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

    return stack->alloc<Subtract>(left, right);
fail:
    iter = initial;
    stack->dealloc<Node>(thisNode);
    return nullptr;
}


//Priority 1
Node* Parser::multiply(){
    const uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2){
        goto fail;
    }

    left = p2();
    //Check if left is a valid subexpression.
    if(left == nullptr){
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

    right = p1();
    //Check if right is a valid subexpression.
    if(right == nullptr){
        goto fail;
    }

    return stack->alloc<Multiply>(left, right);
fail:
    iter = initial;
    stack->dealloc(thisNode);
    return nullptr;
}

Node* Parser::divide(){
    const uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* left = nullptr;
    Node* right = nullptr;

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter + 1].getName() != "/"){
        goto fail;
    }
    iter++;

    left = p2();
    //Check if left is a valid subexpression.
    if(left == nullptr){
        goto fail;
    }

    right = p1();
    //Check if right is a valid subexpression.
    if(right == nullptr){
        goto fail;
    }

    return stack->alloc<Multiply>(left, right);
fail:
    iter = initial;
    stack->dealloc(thisNode);
    return nullptr;
}


//Priority 2
Node* Parser::exponent(){
    const uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* base = nullptr;
    Node* exponent = nullptr;

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

    base = p5();
    //Check if left is a valid subexpression.
    if(base == nullptr){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "^"){
        goto fail;
    }    
    iter++;

    exponent = p3();
    //Check if right is a valid subexpression.
    if(exponent == nullptr){
        goto fail;
    }

    return stack->alloc<Exponent>(base, exponent);
fail:
    iter = initial;
    stack->dealloc(thisNode);
    return nullptr;
}


//Priority 3
Node* Parser::factorial(){
    const uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* base = nullptr;

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 1){
        goto fail;
    }

    base = p5();
    //Check if left is a valid subexpression.
    if(base == nullptr){
        goto fail;
    }

    //Check for the "!" operator.
    if(given[iter].getName() != "!"){
        goto fail;
    }
    iter++;

    return stack->alloc<Factorial>(base);
fail:
    iter = initial;
    stack->dealloc(thisNode);
    return nullptr;
}


//Priority 4
Node* Parser::parentheses(){
    const uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* subexpression = nullptr;

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "("){
        goto fail;
    }
    iter++;

    subexpression = p0();
    //Check if left is a valid subexpression.
    if(!p0()){
        goto fail;
    }

    //Check if right is a valid subexpression.
    if(given[iter].getName() != ")"){
        goto fail;
    }
    iter++;

    return stack->alloc<Parnetheses>(subexpression);
fail:
    iter = initial;
    stack->dealloc<Node>(thisNode);
    return nullptr;
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

Node* Parser::negate(){
    uint64_t initial = iter;
    Node* thisNode = stack->peek<Node>();
    Node* subexpression = nullptr;

    if((int64_t) (given.size() - iter) <= 1){
        goto fail;  
    }

    if(given[iter].getName() != "-"){
        goto fail;
    }
    iter++;

    subexpression = p1();
    if(!p1()){
        goto fail;
    }

    return stack->alloc<Negative>(subexpression);

fail:
    iter = initial;
    stack->dealloc<Node>(thisNode);
    return nullptr;
}