Florida.out: main.cpp Lexer/LexerObjects/lexer.o
	g++ -c main.cpp Lexer/LexerObjects/lexer.o -o Florida.out

Lexer/LexerObjects/lexer.o: Lexer/LexerObjects/token.o Lexer/LexerObjects/helpers.o
	g++ -c Lexer/lexer.hpp Lexer/LexerObjects/token.o Lexer/LexerObjects/helpers.o -o Lexer/LexerObjects/lexer.o

Lexer/LexerObjects/helpers.o: Lexer/helpers.cpp
	g++ -c Lexer/helpers.hpp -o Lexer/LexerObjects/helpers.o

Lexer/LexerObjects/types.o: Lexer/types.cpp
	g++ -c Lexer/types.cpp -o Lexer/LexerObjects/types.o

Lexer/LexerObjects/token.o: Lexer/token.cpp Lexer/LexerObjects/types.o
	g++ -c Lexer/token.hpp Lexer/LexerObjects/types.o -o Lexer/LexerObjects/token.o

clean:
	rm Lexer/LexerObjects/*.o

run:
	./Florida.out sample.fl