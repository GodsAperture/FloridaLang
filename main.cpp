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

	//A simple conditionally recursive integration method.
	double recint(double (fun)(double), double x0, double x2){
		return simp(fun, x0, x2, fun(x0), fun(x2));
	}

	double simp(double (fun)(double), double x0, double x2, double y0, double y2){
		//There now exists x0, x1, and x2, also
		//y0, y1, and y2.
		double x1 = 0.5 * (x0 + x2);
		double y1 = fun(x1);

		//Check to see if the bounds are the same.
		if(x0 == x2){
			return 0;
		}

		//Check to see if the concavity is non-zero.
		//Otherwise, assume the function is simple enough to
		//approximate with a trapezoid.
		if(y2 - 2 * y1 + y0 != 0.0){
			//For the parabola ax^2 + bx + c, the critical point is at -b/(2a).
			//a = 2 (y2 - 2y1 + y0) / (x2 - x0)^2;
			//b = (y2 - y0) / (x2 - x0);
			double criticalPoint = (y2 - y0) * (x2 - x0) / (y2 - 2 * y1 + y0);
			//Check to see if the criticalPoint of the inerpolating parabola is
			//between x0 and x2 OR x2 and x0 (for when x2 < x0).
			if(criticalPoint >= x0 && criticalPoint <= x2 || criticalPoint >= x2 && criticalPoint <= x0){
				//Check to see if the critical point is a maximum.
				//If a is negative, then it is a maximum.
				if((y2 - 2 * y1 + y0) / ((x2 - x0) * (x2 - x0))  <= 0){
					double maximum = y1 - 0.125 * (y0 - y2) * (y0 - y2) / (y2 - 2 * y1 - y0);

				}
			}

			return 1;

		} else {
			//Area approximated by a trapezoid.
			return (y2 + y0) * 0.5 * (x2 - x0);
		}

	}

int main(){
	
	//Load the sample file into a string.
	std::string thing = readFile("sample.fl");
	Lexer FloridaLexer = Lexer(thing);

	//Load the first token and begin the while loop.
	Token thisToken = FloridaLexer.next();
	std::vector<Token> theList;

	//Generate tokens up until EOF is reached.
	while(thisToken.getType() != FloridaType::eof) {
		theList.push_back(thisToken);
		//std::cout << "Lexed token: " << thisToken.name << " type: " << thisToken.getType() << "\n";
		thisToken = FloridaLexer.next();
	}

	//Parse over the list and make sure that the expression is acceptable.
	Parser FloridaParser = Parser(theList, 10000);
	Node* result = FloridaParser.parse();

	//If the error flag has been raised, then
	//terminate the program and print out the errors.
	if(FloridaParser.error){
		//TODO: Set up error handler.
		return -1;
	}

	//Generate the instructions vector.
	std::vector<Instruction> instructionVector;
	std::vector<types> computationVector;
	computationVector.resize(32);
	//The stack will start at the offset to not disrupt the variables.
	size_t position = FloridaParser.scopeStack[0].objects.size();

	//Begin making the instruction vector;
	result->FLVMCodeGen(instructionVector);
	types left;
	types right;

	std::cout << result->ToString("") << "\n";

	//x++ returns x + 1;
	//++x returns x;
	//instNum stands for "Instruction Number".
	for(size_t instNum = 0; instNum < instructionVector.size(); instNum++){
		Operation currInst = instructionVector[instNum].oper;
		switch (currInst){
			case Operation::cjump:
				//If it's true, then don't skip
				if(!computationVector[position - 1].boolean){
					//This is to adjust the position of the instruction number.
					instNum = instructionVector[instNum].literal.fixed64 - 1;
					//Move the position of the stack pointer back to "delete" the-
					//boolean being used in the prior slot.
					position--;
				}
				continue;
			case Operation::jump:
				//Subtract one, because of the instNum++.
				instNum = instructionVector[instNum].literal.fixed64 - 1;
				continue;
			case Operation::fetch:
				//Push the value into the stack.
				computationVector[position++] = computationVector[instructionVector[instNum].literal.fixed64];
				continue;
			case Operation::assign:
				if(instructionVector[instNum].literal.fixed64 == -1){
					printf("Error: bad assignment position\n");
					return -1;
					//If there isn't another assignment instruction next, then-
					//the current value is no longer needed.
					if(instructionVector[instNum + 1].oper != Operation::assign){
						position--;
					}
				}
				computationVector[instructionVector[instNum].literal.fixed64] = computationVector[position - 1];
				continue;
			case Operation::push:
				computationVector[position++] = instructionVector[instNum].literal;
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
				std::cout << "Error: Unknown instruction given.\nThe instruction number is " + currInst;
				return -1;
		}
	}

	return 0;

}