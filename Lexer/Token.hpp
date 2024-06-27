#ifndef Token_h__
#define Token_h__

#include "Types.hpp"
#include <string>

class Token {
private:
	FloridaType type;
	unsigned long row;
	unsigned long column;

public:
	std::string name;

	Token();
	Token(FloridaType inType, char inChar, unsigned long inRow, unsigned long inColumn);

	void operator=(Token right){
		type = right.type;
		row = right.row;
		column = right.column;
	};
	
	FloridaType getType();
	std::string getName();
	void append(char in);
	void changeType(FloridaType in);
	void print();
};

#endif