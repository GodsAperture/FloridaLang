#ifndef Node_h__
#define Node_h__
#include <string>

//This just exists.
class Node{
public:
    Node(){
        //Exists for the sake of the default constructor.    
    }
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(const Node&&) = delete;
    Node& operator=(const Node&&) = delete;
    virtual std::string ToString() = 0;
    virtual std::string ToPostfix() = 0;
    virtual ~Node(){}
};

class Add : public Node{
public:
    Node* left;
    Node* right;

    Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " + " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "+ ";
    }

};

class Subtract : public Node{
public:
    Node* left;
    Node* right;

    Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " - " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "- ";
    }
};

class Multiply : public Node {
public: 
    Node* left;
    Node* right;

    Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " * " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "* ";
    }    
};

class Divide : public Node{
public: 
    Node* left;
    Node* right;

    Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " / " + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "/ ";
    }
};

class Exponent : public Node{
public:
    Node* left;
    Node* right;

    Exponent(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + "^" + right->ToString();
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + right->ToPostfix() + "^ ";
    }
};

class Factorial : public Node{
public:
    Node* left;

    Factorial(Node* input){
        left = input;
    }

    std::string ToString() override {
        return left->ToString() + "!";
    }

    std::string ToPostfix() override {
        return left->ToPostfix() + "! ";
    }
};

class Parnetheses : public Node{
public:
    Node* subexpression;

    Parnetheses(Node* input){
        subexpression = input;
    }

    std::string ToString() override {
        return "(" + subexpression->ToString() + ")";
    }

    std::string ToPostfix() override {
        return subexpression->ToPostfix();
    }

};

class Negative : public Node{
public:
    Node* right;

    Negative(Node* input){
        right = input;
    }

    std::string ToString() override {
        return "-" + right->ToString();
    }

    std::string ToPostfix() override {
        return "0 " + right->ToPostfix() + "- ";
    }
};

class fixed64 : public Node{
private:
    std::string value;
public:
    fixed64(std::string input){
        value = input;
    };

    std::string ToString() override {
        return value;
    }

    std::string ToPostfix() override {
        return value + " ";
    }
};
#endif
