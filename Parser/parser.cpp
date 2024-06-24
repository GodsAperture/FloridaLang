#include <vector>
#include "../Lexer/token.hpp"

class Parser{
private:
//Iterator points to current token
    ulong iter = 0;

//0 priority
    bool p0();

//1 priority
    bool add();
    bool subtract();
    bool p1();

//2 priority
    bool multiply();
    bool divide();
    bool p2();

public:
bool parse(std::vector<Token> input);

};

bool Parser::p0(){
    
}
