#include <iostream>
#include "token.hpp"

//Default Token constructor.
Token::Token(){
    row = 0;
    column = 0;
    name = "";
};



//Secondary Token constructor.
Token::Token(Type inType, char inChar, unsigned long inRow, unsigned long inColumn){

    type = inType;
    name.push_back(inChar);
    row = inRow;
    column = inColumn;

};



//Make my life easier, have an overload for '='.
void Token::operator=(Token right){
    type = right.type;
    name = right.name;
    row = right.row;
    column = right.column;
}



//Return the Token's 'type'.
Type Token::getType(){
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
void Token::changeType(Type in){
    type = in;
}



//Let me print tokens to see if there are any bugs.
void Token::print(){
    std::cout << name;
}