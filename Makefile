this: main.cpp Lexer/*.cpp Parser/*.cpp
	g++ Lexer/*.cpp Parser/*.cpp -o build/*.o
	g++ main.cpp build/*.o -o build/main.out
	./build/main.out