%{
#include "test-framework-header.h"
%}

%debug
%verbose

%left '+' '-'
%left '*' '/'
%prec '&'
%prec '%'
%prec '=' '<' '>'

%token NUM

%%

expr: expr '+' expr				
    | expr '-' expr				
    | expr '*' expr				
    | expr '/' expr				
    | expr '&' expr				
    | expr '%' expr				
    | expr '=' '=' expr			
    | expr '!' '=' expr			
    | expr '<' expr				
    | expr '>' expr				
    | '-' expr					
    | '+' expr					
    /*| '+' '+' expr		/* this one is reduce/reduce with previous: ambiguous grammar rule! */			
    | expr '+' '+' 				
    | '!' expr				%prefer		
    | '&' expr					
    | expr '%'					
	| value						
    ;

value: 
	  digit
	| value digit;

digit:
	'0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';

%%

void yyerror(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}
int yylex(void)
{
    int ch;

    do
    {
        ch = getchar();
    } while (ch == ' ' || ch == '\n' || ch == '\t');
    if (ch == EOF || ch == '\x04' /* Control-D */ )
        return 0;
    printf("lex -> $%02X '%c'\n", ch, (isprint(ch) ? ch : '?'));
    return ch;
}
int main(void)
{
    fprintf(stderr, "Enter a sequence\n");
    yydebug = 1;
    fprintf(stderr, "yyparse = %d\n", yyparse());
    return 0;
}
