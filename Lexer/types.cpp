enum Type{

    //Pesky fucking comments.
    Comment,

    //Compiler commands: #
    //Compiler commands will let the users have some control over the compiler(s).
    Hash,

    //Numbers
    integer, long4, long8, real, imaginary, double8, float4, 

    //Quotations marks will not be overloadable. They are exclusively for strings.
    //Brackets and Bars are all nestable.
    //An example of nesting is [x], [[x]], [[[x]]]... for different operators.
    //Another example is that if A is a matrix, |A| will return the absolute values of
    //its entries, but ||A|| will return the determinant.
    
    //Paired Operators: (, ), [, ], {, }, |, "
    POperator,

    //Comparison Operators: AND, OR, XAND, XOR, NAND, NOR, XNAND, XNOR
    //For more obvious reasons, these are not concatenatable.
    COperator,

    //Concatenated Math Operators: +, -, *, /, ^, \, %, .
    MOperator,

    //Arbitrary Operators: `, ~, !, @, $, %, ^, &, *, ?
    AOperator,

    //Vague operator: %, ^, *
    //If by chance the operator is made of only shared operators, this it is vague.
    VOperator,

    //End of File
    eof,

    //Strings
    Strings,

    //Apostrophes and Colons.
    Apostrophe, Colons,

    //Identifiers and Patterns
    Identifier, Pattern,

    //Others: ',', ;, :, :: (Comma, Period, Semicolon, Colon, DColon)
    //Non-overloadable characters.
    Comma, Semicolon, Colon, DColon
};