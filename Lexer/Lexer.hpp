#ifndef Lexer_h__
#define Lexer_h__

#include "Token.hpp"
#include <fstream>

class Lexer {
private:
	unsigned long row = 1;
	unsigned long column = 0;
	unsigned long count = 0;

public:
	const std::string file;
	Lexer(const std::string& inString);
	bool isEOF();
	//Grabs the next token.
	Token next();
	//Grabs the next character.
	char get();
	void get(char& inChar);
	void unget();
	char peek();
};

#endif