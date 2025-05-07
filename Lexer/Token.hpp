#ifndef Token_h__
#define Token_h__

#include "Types.hpp"
#include <string>
#include <cinttypes>

class Token {
public:
	FloridaType type;
	unsigned long row;
	unsigned long column;

	std::string_view name;

	Token();
	Token(FloridaType inType, char inChar, unsigned long inRow, unsigned long inColumn);
	Token(FloridaType inType, std::string inString, unsigned long inRow, unsigned long inColumn);
	
	FloridaType getType();
	std::string getName();
	void append(char in);
	void changeType(FloridaType in);
	void print();
};

#endif