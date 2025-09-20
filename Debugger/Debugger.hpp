#ifndef Debugger_h__
#define Debugger_h__
#include "../Parser/Node.hpp"

class Debugger{
public:
    Node* AST = nullptr;

    Debugger(Node* inAST);
    ~Debugger();

    uint64_t row = 1;
    uint64_t column = 0;

    

    //Terminal cursor related methods.
    void up();
    void up(uint64_t inUp);
    void down();
    void down(uint64_t inDown);
    void left();
    void left(uint64_t inLeft);
    void right();
    void right(uint64_t inRight);
    void move(uint64_t inRow, uint64_t inColumn);

    //Clear the row on the terminal
    void clearRow();

    //Evaluate the next highlighted section of code.
    bool next();
    //Evalute the current `body` of code.
    bool nextLine();
    //Step into a function instead of stepping over it.
    bool stepInto();
    //Step out of the current function.
    bool stepOut();
    //Finish executing the program.
    bool run();
};
#endif