#ifndef Lexer_h
#define Lexer_h

#include "Token.hpp"
#include "Helpers.hpp"
#include <fstream>
#include <string>

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