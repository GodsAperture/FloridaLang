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
	
	//Load the sample files into strings.
	std::string thing1 = readFile("tester1.fl");
    std::string thing2 = readFile("tester2.fl");
	Lexer testLexer1 = Lexer(thing1);
    Lexer testLexer2 = Lexer(thing2);

	//Load the first token and begin the while loop.
	Token thisToken1 = testLexer1.next();
    Token thisToken2 = testLexer2.next();
	std::vector<Token> testList1;
    std::vector<Token> testList2;

////Test the parser.
	while(thisToken1.getType() != FloridaType::eof) {
		testList1.push_back(thisToken1);
		//std::cout << "Lexed token: " << thisToken.name << " type: " << thisToken.getType() << "\n";
		thisToken1 = testLexer1.next();
	}

////Test the lexer.
    while(thisToken2.getType() != FloridaType::eof) {
		testList2.push_back(thisToken2);
		//std::cout << "Lexed token: " << thisToken.name << " type: " << thisToken.getType() << "\n";
		thisToken2 = testLexer2.next();
	}

	//Parse over the list and make sure that the expression is acceptable.
	Parser parserTest1 = Parser(testList1, 1000);

    /*Expected result to be displayed:
    * 1 + 2 - 3 * 5 / 3 + 28 - 18
    * 1 2 + 3 5 * 3 / - 28 + 18 - 
    * 8
    */
	Node* result = parserTest1.parse();
    std::cout << result->ToRPN() << "\n";

    // The parser has not been defined for anything other than fixed64, yet.
    // for(size_t i = 0; i < testList2.size(); i++){
    //     std::cout << testList2.at(i).getName() << " " << testList2.at(i).getType();
    // }

	return 0;
}