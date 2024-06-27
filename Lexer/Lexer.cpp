#ifndef Lexer

#include "Lexer.hpp"
#include "Helpers.hpp"
#include <iostream>

Lexer::Lexer(const std::string& inFile) : fileName(inFile), file(inFile) {
	row = 1;
	column = 1;
};

// Determine if the EOF token has been reached.
bool Lexer::getEOF() { return this->file.eof(); }

Token Lexer::next() {
	// Grab the first character, and then decide what to do with it.
	Token inToken;
	char currChar;
	file.get(currChar);

	// Catch EOF and immediately terminate next().
	if (file.eof()) {
		inToken = Token(eof, currChar, row, column);
		return inToken;
	}



	// Catch and delete page related characters.
	// Whitespaces, tabs, and newlines.
	if (PagerQ(currChar)) {
		while (PagerQ(currChar)){
			switch (currChar) {
			case '\n':
				row++;
				file.get(currChar);
				continue;
			case '\t':
				column += 4;
				file.get(currChar);
				continue;
			case ' ':
				column++;
				file.get(currChar);
				continue;
			}
		}

		file.unget();
		return next();

	}



	// Deal with those bastardized comments
	if (currChar == '(' && file.peek() == '*') {
		// If the first couple characters read are specifically (*
		// then it will continually consume characters until reaching *).
		// Otherwise it ungets the end character and then sets the currChar to
		// (.
		// Column is decremented by two to make up for (*
		inToken = Token(Comment, '*', row, column - 2);
		// While in a comment, check for *, if star is found, check to see
		// if the next character is ). If it is, then return. If not,
		// continue.
		while (!file.eof()) {
			file.get(currChar);
			column++;
			//If the comment ends, it will call on next() again to return the next viable token.
			if (currChar == '*' && file.peek() == ')') {
				file.get();
				column++;
				return next();
			}
		}
	}



	// Deal with numbers
	if (NumberQ(currChar)) {
		//The token type here isn't the default, it's simply a placeholder.
		//Coincidentally, fix8 will be any number that doesn't have a period or letter.
		inToken = Token(FloridaType::fix8, currChar, row, column);
		file.get(currChar);
		column++;

		while (true) {

			//If the character is a number, append it.
			if (NumberQ(currChar)) {
				inToken.append(currChar);

				file.get(currChar);
				column++;

				continue;
			}

			// Numbers with decimals will default to type fix8.
			if (currChar == '.') {
				inToken.append(currChar);
				inToken.changeType(FloridaType::float8);

				file.get(currChar);
				column++;

				continue;
			}

			// If the number ends with f, i, e, or E it will change the type
			// of number.
			switch (currChar) {
			//This case handles floating point values.
			case 'f':
				if(file.peek() == '4'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::float4);
					//Consume the 4 character
					file.get(currChar);
					column++;

					return inToken;
				}
				if(file.peek() == '8'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::float8);
					//Consume the 8 character
					file.get(currChar);
					column++;

					return inToken;
				}
				if(file.peek() == 'n'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::floatn);
					//Consume the n character
					file.get(currChar);
					column++;

					return inToken;
				}
				else{
					inToken.append(currChar);
					inToken.changeType(FloridaType::float8);

					return inToken;
				}
			//This case handles fixed point values.
			case 'i':
				if(file.peek() == '2'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fix2);
					//Consume the 2 character
					file.get(currChar);
					column++;

					return inToken;
				}
				if(file.peek() == '4'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fix4);
					//Consume the 4 character
					file.get(currChar);
					column++;

					return inToken;
				}
				if(file.peek() == '8'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fix8);
					//Consume the 8 character
					file.get(currChar);
					column++;

					return inToken;
				}
				if(file.peek() == 'n'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fixn);
					//Consume the n character
					file.get(currChar);
					column++;

					return inToken;
				}
				else{
					inToken.append(currChar);
					inToken.changeType(FloridaType::fix8);
					return inToken;
				}
			case 'e':
				inToken.append(currChar);
				inToken.changeType(FloridaType::scifix8);
				//Consume the e character and move to the next character.
				file.get(currChar);
				file.get(currChar);
				column++;
				column++;

				//While the next characters are alphanumeric, append them.
				while(AlphaQ(currChar)){
					inToken.append(currChar);
					column++;
				}

				return inToken;
			case 'E':
				inToken.append(currChar);
				inToken.changeType(FloridaType::scifixn);
				//Consume the E character and move to the next character.
				file.get(currChar);
				file.get(currChar);
				column++;
				column++;

				//While the next characters are alphanumeric, append them.
				while(AlphaQ(currChar)){
					inToken.append(currChar);
					column++;
				}

				return inToken;
			default:
				break;

			} // End Switch statement

			// If there is nothing that matches, the while loop breaks.
			break;

		} // End while Loop

		file.unget();
		column--;

		// In case a number is not specified, it returns as is and ungets.
		return inToken;

	} // End if statement



	// Identifers: Functions, variables, methods, patterns, and character based
	// operators.
	if (AlphaQ(currChar)) {
		inToken = Token(FloridaType::Identifier, currChar, row, column);
		file.get(currChar);
		column++;

		while (true) {

			// If the end character is an underscore, it will be appended and
			// the type will be changed to Pattern and return.
			if (currChar == '_') {
				inToken.append(currChar);
				inToken.changeType(Pattern);

				return inToken;
			}

			// If the current character is a letter or a number it is appended, else
			// it will unget the character and return.
			if (AlphaQ(currChar) | NumberQ(currChar)) {
				inToken.append(currChar);

				file.get(currChar);
				column++;
				continue;
			} else {
				file.unget();
				column--;
				return inToken;
			}
		}
	}



	// Strings of operators
	if (OperatorQ(currChar) && currChar) {
		inToken = Token(FloridaType::Operator, currChar, row, column);
		file.get(currChar);
		column++;

		//While every consecutive character is an operator character add them to the token.
		while(OperatorQ(currChar)){
			inToken.append(currChar);

			file.get(currChar);
			column++;
		}

		file.unget();
		column--;
		return inToken;
	}



	// Catch strings
	if (currChar == '"') {
		inToken = Token(Strings, '"', row, column);

		file.get(currChar);
		column++;

		while (currChar != '"' && !file.eof()) {
			switch (currChar) {
			// In the event the next character is the escape sequence
			// it will be lexed properly.
			// By default, it will just append the character as is.
			case '\\':
				if (file.peek() == 'n') {
					file.get();
					file.get();
					column++;
					column++;
					inToken.append('\n');
					continue;
				}
				if (file.peek() == '\\') {
					file.get();
					file.get();
					column++;
					inToken.append('\\');
					continue;
				}
				if(file.peek() == 't'){
					file.get();
					file.get();
					column++;
					column++;
					inToken.append('t');
					continue;
				}
			default:
				inToken.append(currChar);
				file.get(currChar);
				column++;
				continue;
			}
		}

		inToken.append('"');
		column++;

		return inToken;
	}



	// Catch compiler related commands
	if (currChar == '#') {
		inToken = Token(Hash, '#', row, column);

		file.get(currChar);
		column++;

		// Compiler commands are whole tokens. An example is '#include'.
		while ((currChar != ' ') & (currChar != '\n') & !file.eof()) {
			inToken.append(currChar);

			file.get(currChar);
			column++;
		}

		// No more characters to add to the compiler command
		file.unget();
		column--;
		return inToken;
	}


	//Catch all paired operators
	if(POperatorLQ(currChar) || POperatorRQ(currChar)){
		//Check if it's a left operator, otherwise it's a right operator.
		if(POperatorLQ(currChar)){
			return Token(POperatorL, currChar, row, column);
		}

		return Token(POperatorR, currChar, row, column);
	}


	// Catch those other tokens
	switch (currChar) {
	case ',':
		inToken = Token(Comma, ',', row, column);
		column++;
		return inToken;
	case ';':
		inToken = Token(Semicolon, ';', row, column);
		column++;
		return inToken;
	}

	return Token(FloridaType::BadToken, '.', row, column);
	
}

#endif