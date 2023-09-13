
//Check if the given character is any of the paired operators.
inline bool POperatorQ(char in){

    switch(in){

        case '(':
            return true;
        case ')':
            return true;
        case '[':
            return true;
        case ']':
            return true;
        case '{':
            return true;
        case '}':
            return true;
        default:
            return false;
    }

}


//Check if the given character is any of the math operators.
inline bool MOperatorQ(char in){

    switch(in){

        case '!':
            return true;
        case '>':
            return true;
        case '<':
            return true;
        case '=':
            return true;
        case '&':
            return true;
        case '|':
            return true;
        case '+':
            return true;
        case '-':
            return true;
        case '*':
            return true;
        case '/':
            return true;
        case '\\':
            return true;
        case '%':
            return true;
        case '.':
            return true;
        default:
            return false;

    }

}


//Check if the given chracter is any of the arbitrary operators.
inline bool AOperatorQ(char in){

    switch(in){

        case '`':
            return true;
        case '~':
            return true;
        case '!':
            return true;
        case '@':
            return true;
        case '$':
            return true;
        case '%':
            return true;
        case '^':
            return true;
        case '&':
            return true;
        case '*':
            return true;
        case '?':
            return true;
        default:
            return false;

    }

}


//Check if the operator is a character shared between the Math set and the Arbitrary set.
inline bool SOperatorQ(char in){

    switch(in){

        case '%':
            return true;
        case '^':
            return true;
        case '*':
            return true;
        default:
            return false;

    }

}




//Check if the given character is an alphabetical character 
inline bool AlphaQ(char in){

    if((in >= 'a' & in <= 'z') | (in >= 'A' & in <= 'Z')){
        return true;
    } else {
        return false;
    }

}


//Check if the given character is a number.
inline bool NumberQ(char in){

    if(in >= '0' & in <= '9'){
        return true;
    } else {
        return false;
    }

}


//Check if the given character is any page related character.
inline bool PagerQ(char in){

    switch(in){

        case '\t':
            return true;
        case '\n':
            return true;
        case ' ':
            return true;
        default:
            return false;

    }

}
