#include "node.hpp"

    std::string Node::getName(){
        return Node::token.name;
    }

    void LeftOperator::makeLeft(Node* in){
        this->left = in;
    }

    void RightOperator::makeRight(Node* in){
        this->right = in;
    }

    void BinaryOperator::makeLeft(Node* in){
        this->left = in;
    }

    void BinaryOperator::makeRight(Node* in){
        this->right = in;
    }

    void PairedOperator::makeBody(Node* in){
        this->body = in;
    }

    void Function::makeArguments(std::vector<Node*> in){
        this->arguments = in;
    }

    void Method::makeArguments(std::vector<Node*> in){
        this->arguments = in;
    }