#include <iostream>
#include "Token.hpp"

//Default Token constructor.
Token::Token(){
    row = 0;
    column = 0;
    name = "";
};



//Secondary Token constructor.
Token::Token(FloridaType inType, char inChar, unsigned long inRow, unsigned long inColumn){

    type = inType;
    name.push_back(inChar);
    row = inRow;
    column = inColumn;

};

//Tertiary Token constructor.
Token::Token(FloridaType inType, std::string inName, unsigned long inRow, unsigned long inColumn){
    type = inType;
    name = inName;
    row = inRow;
    column = inColumn;
}



//Return the Token's 'type'.
FloridaType Token::getType(){
    return type;
}


//Return the Token's 'name'.
std::string Token::getName(){
    return name;
}

//Append the given character to the end Token and adds 1 to its length.
void Token::append(char in){

    name.push_back(in);

}



//Allow for the changing of the type via method.
void Token::changeType(FloridaType in){
    type = in;
}



//Let me print tokens to see if there are any bugs.
void Token::print(){
    std::cout << name;
}