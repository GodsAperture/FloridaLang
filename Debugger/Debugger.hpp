#ifndef Debugger_h__
#define Debugger_h__
#include "../Parser/Node.hpp"

class Debugger{
public:
    Node* AST;

    Debugger(Node* inAST);

    //Evaluate the next highlighted section of code.
    bool next();
    //Evalute the current `body` of code.
    bool nextLine();
    //Step into a function instead of stepping over it.
    bool stepInto();
    //Step out of the current function.
    bool stepOut();
    //Finish executing thep program.
    bool run();
};
#endif