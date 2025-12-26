	#include "Lexer.hpp"
	#include "Helpers.hpp"
	#include <exception>
	#include <iostream>

	Lexer::Lexer(const std::string& inString):file(inString){};

	class BadNumberFormatException : public std::exception {
	public: 
		BadNumberFormatException(std::string inString, unsigned long inRow, unsigned long inCol){
			std::cout << "(" + std::to_string(inRow) + ", " + std::to_string(inCol) + "): Did you mean to put a " + inString + "?";
		}
	};


	// Determine if the EOF token has been reached.
	bool Lexer::isEOF() { return file.length() + 1 == count; }

	char Lexer::get(){

		if(count >= file.length()){
			return EOF;
		}

		char currChar = file[count];
		count++;

		if(currChar == '\n'){
			row++;
			column = 1;
		} else {
			column++;
		}

		return currChar;
	}

	bool Lexer::compare(std::string inString){
		//Just check to see if a particular match is found.
		for(size_t i = 0; i < inString.size(); i++){
			if(file.size() == count + i || file[count + i] != inString[i]){
				return false;
			}
		}

		//Catch edge cases like such as:
		//forLoop is not the same as for.
		if(isalnum(file[count + inString.size()])){
			return false;
		}

		//Increment count and column by the size of the string.
		count += inString.size();
		column += inString.size();

		return true;
	}

	void Lexer::unget(){
		count--;
		char currChar = file[count];

		if(currChar == '\n'){
			row--;
			int currentCount = count - 1;

			//Find out how long the row is.
			while(file[currentCount] != '\n'){
				currentCount--;
			}

			//Reset the column count.
			column = count - currentCount + 1;

		} 
		if(currChar == '\t'){
			column -= 4;
		} else {
			column--;
		}

	}

	void Lexer::get(char& inChar){
		//If we have reached the end of the file,
		//then return the EOF character.
		if(count == file.length()){
			column++;
			inChar = EOF;
			return;
		}

		char currChar = file[count];
		count++;

		if(currChar == '\n'){
			row++;
			column = 1;
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
		uint64_t start = count;
		//Check for particular keywords.
		if(compare("object")){
			return Token(FloridaType::Object, std::string_view(file.data() + start, count - start), row, column - 6);
		}
		if(compare("for")){
			return Token(FloridaType::For, std::string_view(file.data() + start, count - start), row, column - 3);
		}

		if(compare("if")){
			return Token(FloridaType::If, std::string_view(file.data() + start, count - start), row, column - 2);
		}

		if(compare("while")){
			return Token(FloridaType::While, std::string_view(file.data() + start, count - start), row, column - 5);
		}
		
		if(compare("ufixed1")){
			return Token(FloridaType::ufixed1, std::string_view(file.data() + start, count - start), row, column - 7);
		}

		if(compare("ufixed2")){
			return Token(FloridaType::ufixed2, std::string_view(file.data() + start, count - start), row, column - 7);
		}

		if(compare("ufixed4")){
			return Token(FloridaType::ufixed4, std::string_view(file.data() + start, count - start), row, column - 7);
		}

		if(compare("ufixed8")){
			return Token(FloridaType::ufixed8, std::string_view(file.data() + start, count - start), row, column - 7);
		}

		if(compare("fixed1")){
			return Token(FloridaType::fixed1, std::string_view(file.data() + start, count - start), row, column - 6);
		}

		if(compare("fixed2")){
			return Token(FloridaType::fixed2, std::string_view(file.data() + start, count - start), row, column - 6);
		}

		if(compare("fixed4")){
			return Token(FloridaType::fixed4, std::string_view(file.data() + start, count - start), row, column - 6);
		}

		if(compare("fixed8")){
			return Token(FloridaType::fixed8, std::string_view(file.data() + start, count - start), row, column - 6);
		}

		if(compare("float4")){
			return Token(FloridaType::float4, std::string_view(file.data() + start, count - start), row, column - 6);
		}

		if(compare("float8")){
			return Token(FloridaType::float8, std::string_view(file.data() + start, count - start), row, column - 6);
		}

		if(compare("boolean")){
			return Token(FloridaType::Bool, std::string_view(file.data() + start, count - start), row, column - 7);
		}

		if(compare("true")){
			return Token(FloridaType::Bool, std::string_view(file.data() + start, count - start), row, column - 4);
		}

		if(compare("false")){	
			return Token(FloridaType::Bool, std::string_view(file.data() + start, count - start), row, column - 5);
		}

		if(compare("return")){
			return Token(FloridaType::Return, std::string_view(file.data() + start, count - start), row, column - 6);
		}

		// Grab the first character, and then decide what to do with it.
		char currChar;
		FloridaType currType;
		this->get(currChar);
		
		//Catch EOF token, first and foremost.
		//Just check to see if we've reached the end
		//of the file.
		if(currChar == EOF){
			return Token(FloridaType::eof, EOF, row, column);
		}
		// Catch and delete page related characters.
		// Whitespaces, tabs, and newlines.
		if (PagerQ(currChar)) {
			while (PagerQ(currChar) && !this->isEOF()){
				switch (currChar) {
				case '\n':
					this->get(currChar);
					start++;
					continue;
				case '\t':
					this->get(currChar);
					start++;
					continue;
				case ' ':
					this->get(currChar);
					start++;
					continue;
				}
			}

			//Move back so the lexer doesn't get fucked up.
			this->unget();
			return next();
		}



		// Deal with those bastardized comments
		if (currChar == '(' && this->peek() == '*') {
			// If the first couple characters read are specifically (*
			// then it will continually consume characters until reaching *).
			// Otherwise it ungets the end character and then sets the currChar to
			// (.
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
			//Assuming a simple string of numbers, fix64 will be the default type.
			currType = FloridaType::fixed8;
			this->get(currChar);

			while (!this->isEOF()) {

				//If the character is a number, append it.
				if (NumberQ(currChar)) {
					this->get(currChar);

					continue;
				}

				// Numbers with decimals will default to type fix8.
				if (currChar == '.') {
					currType = FloridaType::float8;

					this->get(currChar);

					continue;
				}

				// If the number ends with u, f, i, e, or E it will change its type.
				// f: floating point type
				// i: fixed point type
				// e: Scientific notation
				// E: Scientific notation for fixn
				switch (currChar) {
				//This case handles unsigned fix point values
				case 'u':
					//Check to see if it's 1.
					if(this->peek() == '1'){
						this->get();
						currType =  FloridaType::ufixed8;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					}
					//Check to see if it's 2.
					if(this->peek() == '2'){
						this->get();
						currType = FloridaType::ufixed2;
						
						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					}
					//Check to see if it's 4.
					if(this->peek() == '4'){
						this->get();
						currType = FloridaType::ufixed4;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					}
					//Check to see if it's 8.
					if(this->peek() == '8'){
						this->get();
						currType = FloridaType::ufixed8;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					} else {
						throw new BadNumberFormatException(std::string(file.data() + start, count - start) + "u8", column, row);
					}
				//This case handles floating point values.
				case 'f':
					//Check to see if it's 32;
					if(this->peek() == '4'){
						this->get();
						currType = FloridaType::float4;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					}
					//Check to see if it's 64;
					if(this->peek() == '8'){
						this->get();
						currType = FloridaType::float8;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					} else {
						throw new BadNumberFormatException(std::string(file.data() + start, count - start) + "f8", column, row);
					}
				//This case handles fixed point values.
				case 'i':
					//Check to see if it's 1;
					if(this->peek() == '1'){
						this->get();
						currType = FloridaType::fixed8;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					}
					//Check to see if it's 16;
					if(this->peek() == '2'){
						this->get();
						currType = FloridaType::fixed2;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					}
					//Check to see if it's 4;
					if(this->peek() == '4'){
						this->get();
						currType = FloridaType::fixed4;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					}
					//Check to see if it's 8;
					if(this->peek() == '8'){
						this->get();
						currType = FloridaType::fixed8;

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					} else {
						throw new BadNumberFormatException(std::string(file.data() + start, count - start) + "i4", column, row);
					}
				case 'e':
					if(AlphaQ(this->peek()) || this->peek() == '-'){
						currType = FloridaType::scifix8;
						//Consume the e character and move to the next character.
						//Scifix numbers take the form 1.23e123 to represent 1.23 * 10^123
						this->get(currChar);

						//While the next characters are numeric, append them.
						while(NumberQ(currChar) && currChar != EOF){
							this->get(currChar);
						}
						//When a non-numerical character is encountered, break the loop and unget.
						this->unget();

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					} else {
						throw new BadNumberFormatException(std::string(file.data() + start, count - start) + "e4", column, row);
					}
				case 'E':
					if(AlphaQ(this->peek()) || this->peek() == '-'){
						currType = FloridaType::scifixn;
						//Consume the E character and move to the next character.
						//Scifixn numbers take the form 1.23E123 to represent 1.23 * 10^123
						this->get(currChar);

						//While the next characters are numeric, append them.
						while(NumberQ(currChar) && !this->isEOF()){
							this->get(currChar);
						}
						//When a non-numerical character is encountered, break the loop and unget.
						this->unget();

						return Token(currType, std::string_view(file.data() + start, count - start), row, column);
					} else {
						throw new BadNumberFormatException(std::string(file.data() + start, count - start) + "E4", column, row);
					}
				default:
					break;

				} // End Switch statement

				// If there is nothing that matches, the while loop breaks.
				break;

			} // End while Loop

			this->unget();

			// In case a number is not specified, it returns as is and ungets.
			return Token(currType, std::string_view(file.data() + start, count - start), row, column);

		} // End if statement



		// Identifers: Functions, variables, methods, patterns, and character based
		// operators.
		if (AlphaQ(currChar)) {
			currType = FloridaType::Identifier;
			this->get(currChar);

			while (!this->isEOF()) {

				// If the end character is an underscore, it will be appended and
				// the type will be changed to Pattern and return.
				if (currChar == '_') {
					currType = Pattern;

					break;
				}

				// If the current character is a letter or a number it is appended, else
				// it will unget the character and return.
				if (AlphaQ(currChar) || NumberQ(currChar)) {

					this->get(currChar);
					continue;
				} else {
					this->unget();
					break;
				}
			}

			return Token(currType, std::string_view(file.data() + start, count - start), row, column);

		}



		// Strings of operators
		if (OperatorQ(currChar)) {
			currType = FloridaType::Operator;
			this->get(currChar);

			//While every consecutive character is an operator character add them to the token.
			while(OperatorQ(currChar) && !this->isEOF() && currChar != '(' && currChar != ')'){

				this->get(currChar);
			}

			this->unget();
			return Token(currType, std::string_view(file.data() + start, count - start), row, column);
		}



		// Catch strings
		if (currChar == '"') {
			get(currChar);
			if(currChar == '"'){
				return Token(FloridaType::Strings, std::string_view(file.data() + start, count - start), row, column);
			}

			currType = Strings;

			get(currChar);

			while (currChar != '"' && !this->isEOF()) {
				switch (currChar) {
				// In the event the next character is the escape sequence
				// it will be lexed properly.
				// By default, it will just append the character as is.
				case '\\':
					if (this->peek() == 'n') {
						this->get();
						this->get();

						continue;
					}
					if (this->peek() == '\\') {
						this->get();
						this->get();

						continue;
					}
					if(this->peek() == 't'){
						this->get();
						this->get();

						continue;
					} else {
						continue;//This gets rid of the [-Wimplict-fallthrough=] error.
					}
				default:
					this->get(currChar);
					continue;
				}
			}

			unget();

			return Token(currType, std::string_view(file.data() + start, count - start), row, column);
		}



		// Catch compiler related commands
		if (currChar == '#') {
			currType = Hash;

			this->get(currChar);

			// Compiler commands are whole tokens. An example is '#include'.
			while ((currChar != ' ') & (currChar != '\n') & !this->isEOF()) {

				this->get(currChar);
			}

			// No more characters to add to the compiler command
			this->unget();
			return Token(currType, std::string_view(file.data() + start, count - start), row, column);
		}


		//Catch all paired operators
		if(POperatorLQ(currChar) or POperatorRQ(currChar)){
			//Check if it's a left operator, otherwise it's a right operator.
			if(POperatorLQ(currChar)){
				return Token(POperatorL, std::string_view(file.data() + start, count - start), row, column);
			} else {
				return Token(POperatorR, std::string_view(file.data() + start, count - start), row, column);
			}
		}


		// Catch those other tokens
		switch (currChar) {
		case ',':
			return Token(Comma, std::string_view(file.data() + start, count - start), row, column);
		case ';':
			return Token(Semicolon, std::string_view(file.data() + start, count - start), row, column);
		}

		return Token(FloridaType::BadToken, '.', row, column);
		
	}