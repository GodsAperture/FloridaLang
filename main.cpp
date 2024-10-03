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
	
	//Load the sample file into a string.
	std::string thing = readFile("sample.fl");
	Lexer FloridaLexer = Lexer(thing);

	//Load the first token and begin the while loop.
	Token thisToken = FloridaLexer.next();
	std::vector<Token> theList;

	// Generate tokens up until a semicolon or EOF is reached.
	while(!FloridaLexer.isEOF()) {
		theList.push_back(thisToken);
		//std::cout << "Lexed token: " << thisToken.name << " type: " << thisToken.getType() << "\n";
		thisToken = FloridaLexer.next();
	}

	//Parse over the list and make sure that the 
	Parser FloridaParser = Parser(theList, 100000);

	bool result = FloridaParser.parse();

	std::cout << result;

	printf("\n");

	return 0;
}