%{

%}

%token i o v n s b
%define lr.type canonical-lr

%%

S: i T S | i '{' B '}' S | i S | /*Empty*/ ;

T: V T | V | N ;
B: E o E ;

V: E | '(' A ')' | '{' M '}';
N: '{' S '}' ;
E: n | s | v | b ;

A: E | A ',' E | A E ;
M: V ',' M | V M | V ',' | V ;

%%

void yyerror(const char *msg)
{
    printf("%s\n",s);
}

int main()
{
    return yyparse();
}
