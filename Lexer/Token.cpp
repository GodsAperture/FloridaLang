#include <iostream>
#include "Token.hpp"

//Default Token constructor.
Token::Token(){
    type = FloridaType::Strings;
    name = "";
    row = 0;
    column = 0;
}

Token::Token(FloridaType inType, std::string_view inString, unsigned long inRow, unsigned long inColumn){
    type = inType;
    name = inString;
    row = inRow;
    column = inColumn;
};

Token::Token(FloridaType inType, char inChar, unsigned long inRow, unsigned long inColumn){
    type = inType;
    name = std::string(1, inChar);
    row = inRow;
    column = inColumn;
};

//Return the Token's 'type'.
FloridaType Token::getType(){
    return type;
}

//Return the Token's 'name'.
std::string Token::getName(){
    return std::string(name);
}