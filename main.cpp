#include <iostream>
#include <vector>
#include <chrono>
#include <typeinfo>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"

	std::string readFile(std::string filePath){
		std::fstream file = std::fstream(filePath);
		std::string val((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return val;
	}

	std::string printValues(std::vector<types>& inInstructions, std::vector<Association>& inVariables){
		std::string finalString = "";
		for(size_t i = 0; i < inVariables.size(); i++){
			finalString += inVariables[i].name + " = " + std::to_string(inInstructions[inVariables[i].position].fixed64) + ";\n";
		}

		return finalString;
	}


int main(){
	
	//Load the sample file into a string.
	std::string thing = readFile("sample.fl");
	Lexer FloridaLexer = Lexer(thing);

	//Load the first token and begin the while loop.
	Token thisToken = FloridaLexer.next();
	std::vector<Token> theList;

	// Generate tokens up until EOF is reached.
	while(thisToken.getType() != FloridaType::eof) {
		theList.push_back(thisToken);
		//std::cout << "Lexed token: " << thisToken.name << " type: " << thisToken.getType() << "\n";
		thisToken = FloridaLexer.next();
	}

	//Parse over the list and make sure that the expression is acceptable.
	Parser FloridaParser = Parser(theList, 10000);
	Node* result = FloridaParser.parse();

	//Parse the trees for variables.
	std::vector<Association> variableVector;
	result->GetVariables(variableVector);

	std::vector<Instruction> instructionVector;
	std::vector<types> computationVector;
	computationVector.resize(32);
	size_t position = 0;

	//if statement is only to disable code execution;
	if(true){
		//Begin making the instruction vector;
		result->FLVMCodeGen(instructionVector, variableVector);
		types left;
		types right;

		//x++ returns x + 1;
		//++x returns x;
		for(size_t i = 0; i < instructionVector.size(); i++){
			Operation currInst = instructionVector[i].oper;
			switch (currInst){
				case Operation::fetch:
					//Push the value into the stack.
					computationVector[position++] = computationVector[instructionVector[i].literal.fixed64];
					continue;
				case Operation::pop:
					position--;
					continue;
				case Operation::assign:
					if(instructionVector[i].literal.fixed64 == -1){
						printf("\nError: bad assignment position\n");
					}
					computationVector[instructionVector[i].literal.fixed64] = computationVector[position - 1];
					continue;
				case Operation::push:
					computationVector[position++] = instructionVector[i].literal;
					continue;
				case Operation::add:
					//Get the right operand;
					right = computationVector[--position];
					//Get the left operand;
					left = computationVector[--position];
					//Operate and push;
					computationVector[position++].fixed64 = left.fixed64 + right.fixed64;
					continue;
				case Operation::subtract:
					//Get the right operand;
					right = computationVector[--position];
					//Get the left operand;
					left = computationVector[--position];
					//Operate and push;
					computationVector[position++].fixed64 = left.fixed64 - right.fixed64;
					continue;
				case Operation::negate:
					//Negate the value;
					computationVector[position].fixed64 = -computationVector[position].fixed64;
				case Operation::multiply:
					//Get the right operand;
					right = computationVector[--position];
					//Get the left operand;
					left = computationVector[--position];
					//Operate and push;
					computationVector[position++].fixed64 = left.fixed64 * right.fixed64;
					continue;
				case Operation::divide:
					//Get the right operand;
					right = computationVector[--position];
					//Get the left operand;
					left = computationVector[--position];
					//Operate and push;
					computationVector[position++].fixed64 = left.fixed64 / right.fixed64;
					continue;
				default:
					break;
			}
		}
	}

	std::cout << printValues(computationVector, variableVector) << "\n";

	return 0;
}