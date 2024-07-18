#include "Lexer.hpp"
#include "Helpers.hpp"
#include <iostream>

Lexer::Lexer(const std::string& inString):file(inString){};

// Determine if the EOF token has been reached.
bool Lexer::isEOF() { return file.length() + 1 == count; }

char Lexer::get(){

	if(count == file.length()){
		column++;
		return EOF;
	}

	char currChar = file[count];
	count++;

	if(currChar == '\n'){
		row++;
	} else {
		column++;
	}

	return currChar;
}

void Lexer::unget(){
	count--;
	char currChar = file[count];

	if(currChar == '\n'){
		row--;
	} 
	if(currChar == '\t'){
		column -= 4;
	} else {
		column--;
	}

}

void Lexer::get(char& inChar){

	if(count == file.length()){
		column++;
		inChar = EOF;
	}

	char currChar = file[count];
	count++;

	if(currChar == '\n'){
		row++;
	} 
	if(currChar == '\t'){
		column += 4;
	} else {
		column++;
	}

	inChar = currChar;
}

char Lexer::peek(){
	if(count == file.length()){
		return EOF;
	} else {
		return file[count];
	}
}

Token Lexer::next() {
	// Grab the first character, and then decide what to do with it.
	Token inToken;
	char currChar;
	this->get(currChar);


	if (this->isEOF()) {
		inToken = Token(eof, currChar, row, column);
		return inToken;
	}



	// Catch and delete page related characters.
	// Whitespaces, tabs, and newlines.
	if (PagerQ(currChar)) {
		while (PagerQ(currChar) && !this->isEOF()){
			switch (currChar) {
			case '\n':
				this->get(currChar);
				continue;
			case '\t':
				this->get(currChar);
				continue;
			case ' ':
				this->get(currChar);
				continue;
			}
		}

		this->unget();
		return next();

	}



	// Deal with those bastardized comments
	if (currChar == '(' && this->peek() == '*') {
		// If the first couple characters read are specifically (*
		// then it will continually consume characters until reaching *).
		// Otherwise it ungets the end character and then sets the currChar to
		// (.
		// Column is decremented by two to make up for (*
		inToken = Token(Comment, '*', row, column - 2);
		// While in a comment, check for *, if star is found, check to see
		// if the next character is ). If it is, then return. If not,
		// continue.
		while (!this->isEOF()) {
			this->get(currChar);
			//If the comment ends, it will call on next() again to return the next viable token.
			if (currChar == '*' && this->peek() == ')') {
				this->get();
				return next();
			}
		}
	}



	// Deal with numbers
	if (NumberQ(currChar)) {
		//The token type here isn't the default, it's simply a placeholder.
		//Coincidentally, fix8 will be any number that doesn't have a period or letter.
		inToken = Token(FloridaType::fix8, currChar, row, column);
		this->get(currChar);

		while (!this->isEOF()) {

			//If the character is a number, append it.
			if (NumberQ(currChar)) {
				inToken.append(currChar);

				this->get(currChar);

				continue;
			}

			// Numbers with decimals will default to type fix8.
			if (currChar == '.') {
				inToken.append(currChar);
				inToken.changeType(FloridaType::float8);

				this->get(currChar);

				continue;
			}

			// If the number ends with f, i, e, or E it will change the type
			// of number.
			switch (currChar) {
			//This case handles floating point values.
			case 'f':
				if(this->peek() == '4'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::float4);
					//Consume the 4 character
					this->get(currChar);

					return inToken;
				}
				if(this->peek() == '8'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::float8);
					//Consume the 8 character
					this->get(currChar);

					return inToken;
				}
				if(this->peek() == 'n'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::floatn);
					//Consume the n character
					this->get(currChar);

					return inToken;
				}
				else{
					inToken.append(currChar);
					inToken.changeType(FloridaType::float8);

					return inToken;
				}
			//This case handles fixed point values.
			case 'i':
				if(this->peek() == '2'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fix2);
					//Consume the 2 character
					this->get(currChar);

					return inToken;
				}
				if(this->peek() == '4'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fix4);
					//Consume the 4 character
					this->get(currChar);

					return inToken;
				}
				if(this->peek() == '8'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fix8);
					//Consume the 8 character
					this->get(currChar);

					return inToken;
				}
				if(this->peek() == 'n'){
					inToken.append(currChar);
					inToken.changeType(FloridaType::fixn);
					//Consume the n character
					this->get(currChar);

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
				this->get(currChar);

				//While the next characters are numeric, append them.
				while(NumberQ(currChar) && !this->isEOF()){
					inToken.append(currChar);
					this->get(currChar);
				}
				//When a non-numerical character is encountered, break the loop and unget.
				this->unget();

				return inToken;
			case 'E':
				inToken.append(currChar);
				inToken.changeType(FloridaType::scifixn);
				//Consume the E character and move to the next character.
				//Scifix numbers take the form 1.23E123 to represent 1.23 * 10^123
				this->get(currChar);

				//While the next characters are numeric, append them.
				while(NumberQ(currChar) && !this->isEOF()){
					inToken.append(currChar);
					this->get(currChar);
				}
				//When a non-numerical character is encountered, break the loop and unget.
				this->unget();

				return inToken;
			default:
				break;

			} // End Switch statement

			// If there is nothing that matches, the while loop breaks.
			break;

		} // End while Loop

		this->unget();

		// In case a number is not specified, it returns as is and ungets.
		return inToken;

	} // End if statement



	// Identifers: Functions, variables, methods, patterns, and character based
	// operators.
	if (AlphaQ(currChar)) {
		inToken = Token(FloridaType::Identifier, currChar, row, column);
		this->get(currChar);

		while (!this->isEOF()) {

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

				this->get(currChar);
				continue;
			} else {
				this->unget();
				return inToken;
			}
		}
	}



	// Strings of operators
	if (OperatorQ(currChar)) {
		inToken = Token(FloridaType::Operator, currChar, row, column);
		this->get(currChar);

		//While every consecutive character is an operator character add them to the token.
		while(OperatorQ(currChar) && !this->isEOF()){
			inToken.append(currChar);

			this->get(currChar);
		}

		this->unget();
		return inToken;
	}



	// Catch strings
	if (currChar == '"') {
		inToken = Token(Strings, '"', row, column);

		this->get(currChar);

		while (currChar != '"' && !this->isEOF()) {
			switch (currChar) {
			// In the event the next character is the escape sequence
			// it will be lexed properly.
			// By default, it will just append the character as is.
			case '\\':
				if (this->peek() == 'n') {
					this->get();
					this->get();

					inToken.append('\n');
					continue;
				}
				if (this->peek() == '\\') {
					this->get();
					this->get();

					inToken.append('\\');
					continue;
				}
				if(this->peek() == 't'){
					this->get();
					this->get();

					inToken.append('t');
					continue;
				}
			default:
				inToken.append(currChar);
				this->get(currChar);
				continue;
			}
		}

		inToken.append('"');
		this->get();

		return inToken;
	}



	// Catch compiler related commands
	if (currChar == '#') {
		inToken = Token(Hash, '#', row, column);

		this->get(currChar);

		// Compiler commands are whole tokens. An example is '#include'.
		while ((currChar != ' ') & (currChar != '\n') & !this->isEOF()) {
			inToken.append(currChar);

			this->get(currChar);
		}

		// No more characters to add to the compiler command
		this->unget();
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
		return inToken;
	case ';':
		inToken = Token(Semicolon, ';', row, column);
		return inToken;
	}

	return Token(FloridaType::BadToken, '.', row, column);
	
}