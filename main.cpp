#include <iostream>
#include <vector>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.cpp"

int main(){

	Lexer FloridaLexer = Lexer("sample.fl");

	Token thisToken = FloridaLexer.next();
	std::vector<Token> theList = std::vector<Token>(32);

	//Generate tokens up until a semicolon or EOF is reached.
	while(thisToken.getType() != FloridaType::Semicolon && thisToken.getType() != FloridaType::eof){
		theList.push_back(thisToken);
		thisToken = FloridaLexer.next();
	}

	Parser FloridaParser;

	bool result = FloridaParser.parse(theList);

	std::cout << result;

	//Close the file, because if I don't there's a big problem.
	FloridaLexer.file.close();
	printf("\n");

	return 0;
}