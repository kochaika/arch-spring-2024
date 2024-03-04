grammar MyLang;


program: block EOF ;

block: statement* ;
statement
    : assignment
    | if
    | while
    | expression
    | print
    | declaration
    ;

declaration: LET VARIABLE ;
if: IF LPAREN expression RPAREN LCURLY block RCURLY (ELSE LCURLY block RCURLY)? ;

while: WHILE LPAREN expression RPAREN LCURLY block RCURLY ;

print: PRINT LPAREN expression RPAREN ;

assignment: VARIABLE ASSIGN expression ;

expression
    : LCURLY expression RCURLY             #parenthesizedExpression
    | expression (PLUS|MINUS) expression  #additiveExpression
    | expression (EQ|GREATER|LESSER) expression #relationalExpression
    | expression (AND|OR) expression      #logicalExpression
    | NOT expression                      #notExpression
    | INT                                 #intExpression
    | VARIABLE                            #variableExpression
    | MINUS INT                           #unaryMinusExpression
    ;

LET : 'let' ;

ASSIGN : '=' ;

LPAREN : '(' ;
RPAREN : ')' ;
LCURLY : '{' ;
RCURLY : '}' ;

WHILE : 'while' ;
IF : 'if' ;
ELSE : 'else' ;

PLUS : '+' ;
MINUS : '-' ;
AND : 'AND' ;
OR : 'OR' ;

NOT : 'NOT' ;

GREATER : '>' ;
LESSER : '<' ;
EQ : '==' ;

PRINT : 'print' ;

WS: [ \t\n]+ -> skip ;
COMMENT: '#' .*? '\n' -> skip;

VARIABLE : [a-z]+ ;

INT : [0-9]+ ;


