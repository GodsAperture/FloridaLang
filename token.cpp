#pragma once

#include <iostream>
#include <types.hpp>

class Token{
    private:
        Type type;
        std::string string;
        unsigned long row;
        unsigned long column;

    public:
        //Default Token constructor.
        Token(){
            row = 0;
            column = 0;
        };



        //Secondary Token constructor.
        Token(Type inType, char inChar, unsigned long inRow, unsigned long inColumn){

            type = inType;
            string.push_back(inChar);
            row = inRow;
            column = inColumn;

        };



        //Make my life easier, have an overload for '='.
        void operator=(Token right){
            type = right.type;
            string = right.string;
            row = right.row;
            column = right.column;
        }



        //Return the Token's 'type'.
        Type getType(){
            return type;
        }



        //Append the given character to the end Token and adds 1 to its length.
        void append(char in){

            string.push_back(in);

        }



        //Allow for the changing of the type via method.
        void changeType(Type in){
            type = in;
        }

};