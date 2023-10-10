#include "lexer.hpp"
#include "helpers.hpp"
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

	// Catch EOF and immediately terminate next.
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
		inToken = Token(integer, currChar, row, column);
		file.get(currChar);
		column++;

		while (true) {

			if (NumberQ(currChar)) {
				inToken.append(currChar);

				file.get(currChar);
				column++;

				continue;
			}

			// Numbers with decimals will default to type double.
			if (currChar == '.') {
				inToken.append(currChar);
				inToken.changeType(double8);

				file.get(currChar);
				column++;

				continue;
			}

			// If the number ends with f, d, i, r, or l it will change the type
			// of number.
			switch (currChar) {
			case 'f':
				inToken.append(currChar);
				inToken.changeType(float4);
				return inToken;
			case 'd':
				inToken.append(currChar);
				inToken.changeType(double8);
				return inToken;
			case 'i':
				inToken.append(currChar);
				inToken.changeType(imaginary);
				return inToken;
			case 'r':
				inToken.append(currChar);
				inToken.changeType(real);
				return inToken;
			case 'l':
				inToken.append(currChar);
				inToken.changeType(long8);
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
		inToken = Token(Identifier, currChar, row, column);
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
	if (SOperatorQ(currChar)) {
		inToken = Token(VOperator, currChar, row, column);
		file.get(currChar);
		column++;

		// While the first few characters are shared operators, append them
		// and assume it could be a math related operator until proven
		// otherwise.
		while (SOperatorQ(currChar)) {
			inToken.append(currChar);
			file.get(currChar);
			column++;
		}

		// If the operator is an arbitrary type operator, change its type to
		// AOperator and continually append shared and arbitrary operators.
		if (AOperatorQ(currChar)) {
			inToken.append(currChar);
			inToken.changeType(AOperator);

			file.get(currChar);
			column++;
			while (SOperatorQ(currChar) | AOperatorQ(currChar)) {
				inToken.append(currChar);
				file.get(currChar);
				column++;
			}

			// There are no more arbitrary operator or shared operator
			// characters to be appended, finish and return.
			return inToken;
		}

		// If the operator is a math type operator, change its type to MOperator
		// and continually append shared and math operators.
		if (MOperatorQ(currChar)) {
			inToken.append(currChar);
			inToken.changeType(MOperator);

			file.get(currChar);
			column++;
			while (SOperatorQ(currChar) | MOperatorQ(currChar)) {
				inToken.append(currChar);
				file.get(currChar);
				column++;
			}

			// There are no more math operator or shared operator characters
			// to be appended, finish and return.
			return inToken;
		}

		// If the token is nothing but arbitrary characters,
		// then the type will remain VOperator and it shall be returned as is.
		return inToken;
	}



	// Catch strings
	if (currChar == '"') {
		inToken = Token(Strings, '"', row, column);

		file.get(currChar);
		column++;

		while (currChar != '"' && !file.eof()) {

			switch (currChar) {
			// In the event the next two characters are the escape sequence
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



	// Catch those other tokens
	switch (currChar) {
	case ':':
		if (file.peek() == ':') {
			inToken = Token(DColon, ':', row, column);
			inToken.append(':');
			column++;
			column++;
			file.get();
			return inToken;
		} else {
			inToken = Token(Colon, ':', row, column);
			return inToken;
		}
	case ',':
		inToken = Token(Comma, ',', row, column);
		column++;
		return inToken;
	case ';':
		inToken = Token(Semicolon, ';', row, column);
		column++;
		return inToken;
	default:
		std::cout << "\t\nMystery character, please add " << currChar << "!\n";
		std::abort();
		// I hope this is unreachable.
		return inToken;
	}

	
}