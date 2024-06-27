#ifndef Lexer

#include "Token.cpp"
#include <fstream>

class Lexer {
private:
	const std::string fileName;
	unsigned long row;
	unsigned long column;

public:
	std::fstream file;
	Lexer(const std::string& inFile);
	bool getEOF();
	//Grabs the next token.
	Token next();
};

#endif