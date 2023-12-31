#include <iostream>
#include "Lexer/lexer.hpp"
#include <vector>

int main(int argc, char** argv){

	if (true) {
		// Lexer FloridaLexer = Lexer(argv[0]);
		Lexer FloridaLexer = Lexer("sample.fl");
		printf("Goodbye bugs!\n");

		Token thisToken = FloridaLexer.next();
		std::vector<Token> theList;

		// Grab the first token and so long as it isn't EOF it will continue.
		while (!FloridaLexer.file.eof()) {
			theList.push_back(thisToken);
			std::cout << thisToken.string << std::endl;

			thisToken = FloridaLexer.next();
		}

		// Close the file, because if I don't there's a big problem.
		FloridaLexer.file.close();
		printf("\n");
	} else {

		printf("No arguments were passed, try again with arguments.\n");
	}
	return 0;
}