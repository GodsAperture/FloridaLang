//Check if the given character is any of the left paired operators.
bool POperatorLQ(char in){

    switch(in){

        case '(':
            return true;
        case '[':
            return true;
        case '{':
            return true;
        default:
            return false;
    }

}

//Check if the given character is any of the left paired operators.
bool POperatorRQ(char in){

    switch(in){

        case ')':
            return true;
        case ']':
            return true;
        case '}':
            return true;
        default:
            return false;
    }

}

//Check if the given character is an alphabetical character 
bool AlphaQ(char in){

    if(((in >= 'a') & (in <= 'z')) | ((in >= 'A') & (in <= 'Z'))){
        return true;
    } else {
        return false;
    }

}

//Check if the given character is a number.
bool NumberQ(char in){

    if((in >= '0') & (in <= '9')){
        return true;
    } else {
        return false;
    }

}

//Check if the given character is any page related character.
bool PagerQ(char in){

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

//Check if the given character is any of the operator characters.
bool OperatorQ(char in){
    return !AlphaQ(in) && !NumberQ(in) && !PagerQ(in) && in != ';' && in != ',' && in != '_' && in != '"';
}