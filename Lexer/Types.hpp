#ifndef Types_h__
#define Types_h__
#include <stdint.h>

//2 byte type information
enum FloridaType : int16_t {
    //Void
    tvoid,
    //Default type, for errors;
    Null,
    //Adjectives describe what type an object is.
    //This will prevent the parser from trying to convert "float8", and others, into a number.
    Adjective,

    //Pesky fucking comments.
    Comment,

    //Compiler commands: #
    //Compiler commands will let the users have some control over the compiler(s).
    Hash,

    //Booleans
    Bool,

    //Numbers
    ufixed1, ufixed2, ufixed4, ufixed8,
    fixed1, fixed2, fixed4, fixed8,
    //If float1 and/or float2 ever come into existence:
    //1.) Why did hardware manufactureres do this??
    //2.) These are objectively awful for floating point numbers.
    //Solution? Don't implement these as hardware instructions.
    /*Non-existent*/ float1, /*Non-existent*/ float2,
    float4, float8,
    ufixedn, fixedn, floatn,

    //Keywords
    If, For, Break, While, Return, Object, Try, Catch, Typecast,

    //scifix numbers are scientific notation numbers such as 2.718e0 and 1.234E32
    scifix8, scifixn,
    imaginary,

    //Quotations marks will not be overloadable. They are exclusively for strings.
    //Brackets and Bars are all nestable.
    //An example of nesting is [x], [[x]], [[[x]]]... for different operators.
    //Another example is that if A is a matrix, |A| will return the absolute values of
    //its entries, but ||A|| will return the determinant.
    
    //Paired Operators: (, ), [, ], {, }, |
    POperatorL, POperatorR,

    //Operators are strings of non-alphanumerical characters. 
    Operator,
    //Binary operators operate on the objects to the left and right.
    BinaryOperator,
    //Left Operators operate on the object to the left.
    LeftOperator,
    //Right Operators operate on the object the right.
    RightOperator,

    //End of File
    eof,

    //Strings
    Strings,

    //Identifiers and Patterns
    Identifier, Pattern,

    //Others: ',', ;(Comma, Semicolon)
    //Non-overloadable characters.
    Comma, Semicolon,

    //Bad tokens, only exists to make the lexer not shit itself.
    BadToken
};
#endif