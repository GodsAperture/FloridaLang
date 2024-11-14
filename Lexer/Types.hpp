#ifndef Types_h__
#define Types_h__
enum FloridaType{
    //Default type, for errors;
    Null,

    //Keywords
    If, For, Break, While, Return, Object, Try, Catch,

    //Pesky fucking comments.
    Comment,

    //Compiler commands: #
    //Compiler commands will let the users have some control over the compiler(s).
    Hash,

    //Numbers
    ufixed8, ufixed16, ufixed32, ufixed64,
    fixed8, fixed16, fixed32, fixed64,
    float32, float64,
    ufixedn, fixedn, floatn,

    //scifix numbers are scientific notation numbers such as 2.718e0 and 1.234E32
    scifix64, scifixn,
    imaginary,

    //Quotations marks will not be overloadable. They are exclusively for strings.
    //Brackets and Bars are all nestable.
    //An example of nesting is [x], [[x]], [[[x]]]... for different operators.
    //Another example is that if A is a matrix, |A| will return the absolute values of
    //its entries, but ||A|| will return the determinant.
    
    //Paired Operators: (, ), [, ], {, }, |, "
    POperatorL, POperatorR,

    //Operators are strings of non-alphanumerical characters. 
    Operator,

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