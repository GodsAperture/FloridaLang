#include <iostream>
#include <vector>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"

	std::string readFile(std::string filePath){
		std::fstream file = std::fstream(filePath);
		std::string val((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return val;
	}

int main(){
	
	std::string thing = readFile("sample.fl");
	Lexer FloridaLexer = Lexer(thing);

	Token thisToken = FloridaLexer.next();
	std::vector<Token> theList;

	//Generate tokens up until a semicolon or EOF is reached.
	while(!FloridaLexer.isEOF()) {
		theList.push_back(thisToken);
		std::cout << "Lexed token: " << thisToken.name << " type: " << thisToken.getType() << "\n";
		thisToken = FloridaLexer.next();
	}
	
	Parser FloridaParser = Parser(theList);

	bool result = FloridaParser.parse();

	std::cout << result;

	printf("\n");

	return 0;
}