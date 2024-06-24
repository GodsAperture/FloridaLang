#include "../Lexer/token.hpp"
#include <vector>

//Nodes exist to build the Abstract Syntax Tree.
//Judge me all you like, I haven't taken a course in making languages, so the joke's
//on whoever thinks they can do it better, but still uses Florida.
class Node{
protected:
    Node* head;
    Token token;
public:
    Node& operator=(Node& in){
        token = in.token;
        head = in.head;
    }

    Node(){
        head = nullptr;
    }

    std::string getName();

};  

//LeftOperator is an operator that operates only on the Node to the left.
class LeftOperator : Node{
private:
    Node* left;
public:
    LeftOperator(Token& inToken){
        token = inToken;
    }

    void makeLeft(Node* in);


};

//RightOperator is an operator that operates only on the Node to the right.
class RightOperator : Node{
private:
    Node* right;
public:
    RightOperator(Token& inToken){
        token = inToken;
    }

    void makeRight(Node* in);

};

//BinaryOperator is an operator that operates on the left and right Nodes.
class BinaryOperator : Node{
private:
    Node* left;
    Node* right;
public:
    void makeLeft(Node* in);

    void makeRight(Node* in);

    BinaryOperator(Token& inToken){
        token = inToken;
    }
};

//PairedOperators will always come in pairs, such as (), [], {}, and their nested variations such as [[]].
class PairedOperator : Node{
private:
    Node* body;
public:
    PairedOperator(Token& inToken){
        token = inToken;
    }

    void makeBody(Node* in);
};

//Function will be the Node to designate a function call.
class Function : Node{
private:
    std::vector<Node*> arguments;
public:
    Function(Token& in){
        token = in;
    }

    void makeArguments(std::vector<Node*> in);
};

//Methods are class methods, they're just functions with a period in between them and the object.
class Method : Node{
private:
    std::vector<Node*> arguments;
public:
    Method(Token& in){
        token = in;
    }

    void makeArguments(std::vector<Node*> in);

};

//Terminals are objects in the language. These are numbers, matrices, strings, etc.
class Terminal : Node{
private:
public:
    Terminal(Token inToken){
        token = inToken;
    }
};