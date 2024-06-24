enum Type{
    //Null objects, mainly for the BaseOperator, until I write a better BaseOperator class.
    null,

    //Pesky fucking comments.
    Comment,

    //Compiler commands: #
    //Compiler commands will let the users have some control over the compiler(s).
    Hash,

    //Numbers
    fix2, fix4, fix8, fixn, 
    float4, float8, floatn, 
    //scifix numbers are scientific notation numbers such as 2.718e0 and 1.234E32
    scifix8, scifixn,
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
    Comma, Semicolon
};