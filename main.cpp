#include <iostream>
#include <vector>
#include <chrono>
#include <typeinfo>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "VirtualMachine/VM.hpp"

	std::string readFile(std::string filePath){
		std::fstream file = std::fstream(filePath);
		std::string val((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return val;
	}

	// //A simple conditionally recursive integration method.
	// double recint(double (fun)(double), double x0, double x2){
	// 	return simp(fun, x0, x2, fun	(x0), fun(x2));
	// }

	// double simp(double (fun)(double), double x0, double x2, double y0, double y2){
	// 	//There now exists x0, x1, and x2, also
	// 	//y0, y1, and y2.
	// 	double x1 = 0.5 * (x0 + x2);
	// 	double y1 = fun(x1);

	// 	//Check to see if the bounds are the same.
	// 	if(x0 == x2){
	// 		return 0;
	// 	}

	// 	//Check to see if the concavity is non-zero.
	// 	//Otherwise, assume the function is simple enough to
	// 	//approximate with a trapezoid.
	// 	if(y2 - 2 * y1 + y0 != 0.0){
	// 		//For the parabola ax^2 + bx + c, the critical point is at -b/(2a).
	// 		//a = 2 (y2 - 2y1 + y0) / (x2 - x0)^2;
	// 		//b = (y2 - y0) / (x2 - x0);
	// 		double criticalPoint = (y2 - y0) * (x2 - x0) / (y2 - 2 * y1 + y0);
	// 		//Check to see if the criticalPoint of the inerpolating parabola is
	// 		//between x0 and x2 OR x2 and x0 (for when x2 < x0).
	// 		if(criticalPoint >= x0 && criticalPoint <= x2 || criticalPoint >= x2 && criticalPoint <= x0){
	// 			//Check to see if the critical point is a maximum.
	// 			//If a is negative, then it is a maximum.
	// 			if((y2 - 2 * y1 + y0) / ((x2 - x0) * (x2 - x0))  <= 0){
	// 				double maximum = y1 - 0.125 * (y0 - y2) * (y0 - y2) / (y2 - 2 * y1 - y0);

	// 			}
	// 		}

	// 		return 1;

	// 	} else {
	// 		//Area approximated by a trapezoid.
	// 		return (y2 + y0) * 0.5 * (x2 - x0);
	// 	}

	// }

int main(){
	//Load the sample file into a string.
	const std::string thing = readFile("sample.fl");
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
	FloridaParser.parse();
	std::cout << FloridaParser.result->ToString("", ";") << "\n";

	//If the error flag has been raised, then
	//terminate the program and print out the errors.
	if(FloridaParser.error){
		FloridaParser.errorPrint();
		return -1;
	}

	//Make the virtual machine.
	FloridaVM Florida = FloridaVM(FloridaParser);
	//Debug the bytecode.
	Florida.printAll();

	//Execute the instructions
	bool successful = true;
	while(successful){
		successful = Florida.next();
	}

	return 0;

}