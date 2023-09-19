Florida.out: main.cpp Lexer/LexerObjects/lexer.o
	g++ -g main.cpp Lexer/LexerObjects/lexer.o -o Florida.out

Lexer/LexerObjects/lexer.o: Lexer/LexerObjects/token.o Lexer/LexerObjects/helpers.o
	g++ -c Lexer/lexer.cpp Lexer/LexerObjects/token.o Lexer/LexerObjects/helpers.o -o Lexer/LexerObjects/lexer.o

Lexer/LexerObjects/helpers.o: Lexer/helpers.cpp
	g++ -c Lexer/helpers.cpp -o Lexer/LexerObjects/

Lexer/LexerObjects/types.o: Lexer/types.cpp
	g++ -c Lexer/types.cpp -o Lexer/LexerObjects/types.o

Lexer/LexerObjects/token.o: Lexer/token.cpp Lexer/LexerObjects/types.o
	g++ -c Lexer/token.cpp Lexer/LexerObjects/types.o -o Lexer/LexerObjects/token.o