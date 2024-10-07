#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

bool Parser::parse(){
    //If it's not the nullptr, then it's successful.
    Node* tree = this->p0();
    bool isSuccessful = (tree != nullptr);
    //Print the statement back out to see if it even works.
    //I sure hope it does.
    std::string result1 = tree->ToString() + "\n";
    std::string result2 = tree->ToPostfix() + "\n";
    std::cout << result1 << result2;
    return isSuccessful;
};

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

    thisNode = p2();
    if(thisNode != nullptr){
        return thisNode;
    }

    return nullptr;
}

Node* Parser::p2(){
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

    thisNode = p3();
    if(thisNode != nullptr){
        return thisNode;
    }

    return nullptr;
}

Node* Parser::p3(){
    if(given[iter].getType() != FloridaType::fix8){
        return nullptr;
    }

    iter++;
    return stack->alloc<fixed64>(given[iter - 1].getName());
}


//Priority 0
Node* Parser::add(){
    const uint64_t initial = iter;
    Add* thisNode = stack->alloc<Add>(nullptr, nullptr);

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2){
        goto fail;
    }

    thisNode->left = p1();
    //Check if left is a valid subexpression.
    if(thisNode->left == nullptr){
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

    thisNode->right = p0();
    //Check if right is a valid subexpression.
    if(thisNode->right == nullptr){
        goto fail;
    }

    return thisNode;

fail:
    iter = initial;
    stack->dealloc<Add>(thisNode);
    return nullptr;
}

Node* Parser::subtract(){
    const uint64_t initial = iter;
    Subtract* thisNode = stack->alloc<Subtract>(nullptr, nullptr);

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2){
        goto fail;
    }

    thisNode->left = p1();
    //Check if left is a valid subexpression.
    if(thisNode->left == nullptr){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "-"){
        goto fail;
    }
    iter++;

    thisNode->right = p0();
    //Check if right is a valid subexpression.
    if(thisNode->right == nullptr){
        goto fail;
    }

    return thisNode;
fail:
    iter = initial;
    stack->dealloc<Subtract>(thisNode);
    return nullptr;
}


//Priority 1
Node* Parser::multiply(){
    const uint64_t initial = iter;
    Multiply* thisNode = stack->alloc<Multiply>(nullptr, nullptr);

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2){
        goto fail;
    }

    thisNode->left = p2();
    //Check if left is a valid subexpression.
    if(thisNode->left == nullptr){
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

    thisNode->right = p1();
    //Check if right is a valid subexpression.
    if(thisNode->right == nullptr){
        goto fail;
    }


    return thisNode;
fail:
    iter = initial;
    stack->dealloc<Multiply>(thisNode);
    return nullptr;
}

Node* Parser::divide(){
    const uint64_t initial = iter;
    Divide* thisNode = stack->alloc<Divide>(nullptr, nullptr);

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

    thisNode->left = p2();
    //Check if left is a valid subexpression.
    if(thisNode->left == nullptr){
        goto fail;
    }

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "/"){
        goto fail;
    }
    iter++;

    thisNode->right = p1();
    //Check if right is a valid subexpression.
    if(thisNode->right == nullptr){
        goto fail;
    }

    return thisNode;
fail:
    iter = initial;
    stack->dealloc<Divide>(thisNode);
    return nullptr;
}

//Priority 2
Node* Parser::parentheses(){
    const uint64_t initial = iter;
    Parnetheses* thisNode = stack->alloc<Parnetheses>(nullptr);

    //Check if this program has reached the end of the token stream.
    if((int64_t) (given.size() - iter) <= 2)
        goto fail;

    //Check if the center of this subexpression is the "+" operator.
    if(given[iter].getName() != "("){
        goto fail;
    }
    iter++;

    thisNode->subexpression = p0();
    //Check if left is a valid subexpression.
    if(!p0()){
        goto fail;
    }

    //Check if right is a valid subexpression.
    if(given[iter].getName() != ")"){
        goto fail;
    }
    iter++;

    return thisNode;
fail:
    iter = initial;
    stack->dealloc<Parnetheses>(thisNode);
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