#ifndef Node_h__
#define Node_h__
#include <string>

//This just exists.
class Node{
public:
    Node(){
        //Exists for the sake of the default constructor.    
    }
    virtual std::string ToString();
};

class Add : public Node{
private: 
    Node* left;
    Node* right;
public:
    Add(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " + " + right ->ToString();
    }
};

class Subtract : public Node{
private: 
    Node* left;
    Node* right;
public:
    Subtract(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " - " + right ->ToString();
    }
};

class Multiply : public Node {
private: 
    Node* left;
    Node* right;
public:
    Multiply(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " * " + right ->ToString();
    }
};

class Divide : public Node{
private: 
    Node* left;
    Node* right;
public:
    Divide(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + " / " + right ->ToString();
    }
};

class Exponent : public Node{
private:
    Node* left;
    Node* right;
public:
    Exponent(Node* LHE, Node* RHE){
        left = LHE;
        right = RHE;
    }

    std::string ToString() override {
        return left->ToString() + "^" + right ->ToString();
    }
};

class Factorial : public Node{
private:
    Node* base;
public:
    Factorial(Node* input){
        base = input;
    }

    std::string ToString() override {
        return base->ToString() + "!";
    }
};

class Parnetheses : public Node{
private:
    Node* subexpression;
public:
    Parnetheses(Node* input){
        subexpression = input;
    }

    std::string ToString() override {
        return "(" + subexpression->ToString() + ")";
    }
};

class Negative : public Node{
private:
    Node* subexpression;
public:
    Negative(Node* input){
        subexpression = input;
    }

    std::string ToString() override {
        return "-" + subexpression->ToString();
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
};
#endif