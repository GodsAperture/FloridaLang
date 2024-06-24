#include <iostream>
#include "Lexer/lexer.hpp"
#include <vector>
#include "Parser/node.cpp"
#include "Parser/BaseOperator.hpp"

int main(int argc, char** argv){

	//I forgot, C/C++ won't let you do any real work related to your program outside of main.
	//So to whoever hates this, blame Ritchie and Bjarne, okay?? I don't like this either.

	//The name stands for Operator List.
	std::vector<BaseOperator> OpList;

	//The operator '+' is of middle precedence.
	OpList.push_back(BaseOperator(std::string("+"), (uint) 1, integer, integer));


	if (true) {
		//Lexer FloridaLexer = Lexer(argv[0]);
		Lexer FloridaLexer = Lexer("sample.fl");

		Token thisToken = FloridaLexer.next();
		Token lastToken;
		std::vector<Token> theList;
		theList.resize(32);

		//Grab the first token and so long as it isn't EOF it will continue.
		while (!FloridaLexer.file.eof()) {
			//Generate tokens up until a newline is reached.
			while(lastToken.getType() != Semicolon){
				theList.push_back(thisToken);

				lastToken = thisToken;
				thisToken = FloridaLexer.next();
			}



		}

		//Close the file, because if I don't there's a big problem.
		FloridaLexer.file.close();
		printf("\n");
	} else {

		printf("No arguments were passed, try again with arguments.\n");
	}
	return 0;
}