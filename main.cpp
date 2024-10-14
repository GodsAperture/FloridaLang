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
	while(thisToken.getType() != FloridaType::eof) {
		theList.push_back(thisToken);
		//std::cout << "Lexed token: " << thisToken.name << " type: " << thisToken.getType() << "\n";
		thisToken = FloridaLexer.next();
	}

	//Parse over the list and make sure that the expression is acceptable.
	Parser FloridaParser = Parser(theList, 1000);

	Node* result = FloridaParser.parse();

	printf("\n");

	std::cout << result->ToPostfix() << "\n\n";
	std::vector<Instruction> thisVector;
	std::stack<int64_t> thisStack;
	result->FLVMCodeGen(thisVector);
	int64_t left;
	int64_t right;

	for(size_t i = 0; i < thisVector.size(); i++){
		switch (thisVector.at(i).instruction){
			case Operation::push:
				thisStack.push(thisVector.at(i).literal);
				continue;
			case Operation::add:
				right = thisStack.top();
				thisStack.pop();
				//Get the left operand
				left = thisStack.top();
				thisStack.pop();
				//Operate and push
				thisStack.push(left + right);
				continue;
			case Operation::subtract:
				//Get the right operand
				right = thisStack.top();
				thisStack.pop();
				//Get the left operand
				left = thisStack.top();
				thisStack.pop();
				//Operate and push
				thisStack.push(left - right);
				continue;
			case Operation::multiply:
				//Get the right operand
				right = thisStack.top();
				thisStack.pop();
				//Get the left operand
				left = thisStack.top();
				thisStack.pop();
				//Operate and push
				thisStack.push(left * right);
				continue;
			case Operation::divide:
				//Get the right operand
				right = thisStack.top();
				thisStack.pop();
				//Get the left operand
				left = thisStack.top();
				thisStack.pop();
				//Operate and push
				thisStack.push(left / right);
				continue;
			default:
				break;
		}
	}

	std::cout << "Result: " << thisStack.top() << "\n";

	return 0;
}