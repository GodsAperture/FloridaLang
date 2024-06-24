#include <iostream>
#include "Lexer/lexer.hpp"
#include <vector>
#include "Parser/node.cpp"
#include "Parser/BaseOperator.hpp"

int main(int argc, char** argv){
	//I forgot, C/C++ won't let you do any real work related to your program outside of main.
	//So to whoever hates this, blame Ritchie and Bjarne, okay?? I don't like this either.

////Basic operators
	//The name stands for Operator List.
	std::vector<BaseOperator> OpList;

	//The operators + and - are of equal precedence.
	OpList.push_back(BaseOperator(std::string("+"), (uint) 1, Type::fix8, Type::fix8, Type::fix8));
	OpList.push_back(BaseOperator(std::string("-"), (uint) 1, Type::fix8, Type::fix8, Type::fix8));

	//The operators * and / are of equal precedence.
	OpList.push_back(BaseOperator(std::string("*"), (uint) 2, Type::fix8, Type::fix8, Type::fix8));
	OpList.push_back(BaseOperator(std::string("/"), (uint) 2, Type::fix8, Type::fix8, Type::fix8));

	//^ is for exponents. 
	OpList.push_back(BaseOperator(std::string("^"), (uint) 3, Type::fix8, Type::fix8, Type::fix8, false));

	//! will be used for factorial
	OpList.push_back(BaseOperator(std::string("!"), (uint) 4, Type::fix8, Type::null, Type::fix8));

	if (true) {
		//Lexer FloridaLexer = Lexer(argv[0]);
		Lexer FloridaLexer = Lexer("sample.fl");

		Token thisToken = FloridaLexer.next();
		std::vector<Node*> theList;
		theList.resize(32);

		//Grab the first token and so long as it isn't EOF it will continue.
		while (!FloridaLexer.file.eof()) {
			//Generate tokens up until a semicolon is reached.
			while(thisToken.getType() != Semicolon){
				
				//This switch statement will generate the appropriate nodes.
				switch(thisToken.getType()){
				//Integers
					case Type::fix2:
						theList.push_back(new Terminal(thisToken));
						continue;
					case Type::fix4:
						theList.push_back(new Terminal(thisToken));
						continue;
					case Type::fix8:
						theList.push_back(new Terminal(thisToken));
						continue;
					case Type::fixn:
						theList.push_back(new Terminal(thisToken));
						continue;
						
				//Decimal numbers		
					case Type::float4:
						theList.push_back(new Terminal(thisToken));
						continue;
					case Type::float8:
						theList.push_back(new Terminal(thisToken));
						continue;
					case Type::floatn:
						theList.push_back(new Terminal(thisToken));
						continue;
				}

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