#ifndef token

#include "types.cpp"
#include <string>

class Token {
private:
	Type type;
	unsigned long row;
	unsigned long column;

public:
	std::string name;

	Token();
	Token(Type inType, char inChar, unsigned long inRow, unsigned long inColumn);

	void operator=(Token right){
		type = right.type;
		row = right.row;
		column = right.column;
	};
	
	Type getType();
	void append(char in);
	void changeType(Type in);
	void print();
};

#endif 