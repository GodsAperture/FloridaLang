#include "Debugger.hpp"
#include <stdio.h>

Debugger::Debugger(Node* inAST){
    AST = inAST;
    setbuf(stdout, NULL);
}

Debugger::~Debugger(){
    
    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
}

void Debugger::up(){
    if(row > 1){
        row -= 1;
        printf("\x1b[1A");
    }
}

void Debugger::up(uint64_t inUp){
    if(row > inUp){
        row -= inUp;
    } else {
        row = 1;
    }
    printf("\x1b[%ldA", inUp);
}

void Debugger::down(){
    row += 1;
    printf("\x1b[1B");
}

void Debugger::down(uint64_t inDown){
    row += inDown;
    printf("\x1b[%ldB", inDown);
}

void Debugger::left(){
    if(column > 1){
        column -= 1;
    }
    printf("\x1b[1C");
}

void Debugger::left(uint64_t inLeft){
    if(column > inLeft){
        column -= inLeft;
    } else {
        column = 1;
    }
    printf("\x1b[%ldC", inLeft);
}

void Debugger::right(){
    column += 1;
    printf("\x1b[1D");
}

void Debugger::right(uint64_t inRight){
    column += inRight;
    printf("\x1b[%ldD", inRight);
};

void Debugger::move(uint64_t inRow, uint64_t inColumn){
    row = inRow;
    column = inColumn;
    printf("\x1b[%ld;%ldH", inRow, inColumn);
}



void Debugger::clearRow(){
    printf("\x1b[2K\r");
}