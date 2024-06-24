#include <string>
#include "./Lexer/types.cpp"

//This will be how operators are defined in Florida.
class BaseOperator{
private:
public:
    std::string name;   //The literal characters used to define the operator.
    uint precedence;    //The precedence of the operator for a given type. A higher precedence means it will occur first.
    Type typeLeft;      //The type of the left argument (if applicable, otherwise null).
    Type typeRight;     //The type of the right argument (if applicable, otherwise null).

    BaseOperator(std::string inName, uint inPrecedence, Type inTypeLeft, Type inTypeRight){
        name = inName;
        precedence = inPrecedence;
        typeLeft = inTypeLeft;
        typeLeft = inTypeRight;
    }
};