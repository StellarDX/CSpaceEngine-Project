%{
    #include<stdio.h>
%}

%token Identifier Number String Boolean

%%

Table: Key ValueOrSubTable Table | /*Empty*/ ;
ValueOrSubTable: Value ValueOrSubTable | Value | SubTable ;

Key: Identifier ;

Value: Expression | '(' Array ')' | '{' Matrix '}' ;
Expression: Number | String | Boolean ;
Array: Expression | Array Comma Expression ;
Comma: ',' | /*Empty*/ ;
Matrix: Value Comma Matrix | Value Comma ;

SubTable: '{' Table '}' ;

%%

void yyerror(const char *msg)
{
    printf("%s\n",s);
}

int main()
{
    return yyparse();
}
