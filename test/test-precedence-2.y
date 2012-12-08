%{
#include <stdlib.h>

typedef double yystype_t;
#define YYSTYPE	yystype_t

#define YYDBPR(yystype)    BTYACC_DEBUG_LOG_PARTIAL("%f", (double)(yystype))
%}

%debug
%verbose
%scannerless

%token '+' '-'
%token '*' '/'
%prec '=' '<' '>'
%prec '(' ')'

%token NUM

%%

expr: expr '+' expr								%left
	  = { $$ = $1 + $3; };
    | expr '-' expr								%left				
	  = { $$ = $1 - $3; };
    | expr '*' expr								%left				
	  = { $$ = $1 * $3; };
    | expr '/' expr								%left				
	  = { $$ = $1 / $3; };
    | expr '=' '=' expr			
	  = { $$ = fabs($1 - $3) <= EPSILON; };
    | expr '!' '=' expr			
	  = { $$ = fabs($1 - $3) > EPSILON; };
    | expr '<' expr				
	  = { $$ = $1 < $3; };
    | expr '>' expr				
	  = { $$ = $1 > $3; };
    | '(' expr ')'				
	  = { $$ = $2; };
    | '-' expr					
	  = { $$ = -$2; };
    | '+' expr					
	  = { $$ = $2; };
    | '!' expr				%prefer		
	  = { $$ = fabs($2) <= EPSILON; };
    | expr '%'					
	  = { $$ = $2 / 100.0; };
	| value						
	  = { $$ = $1; };
    ;

value: 
	  digit
	  = { $$ = $1; };
	| value digit
	  = { $$ = $1 * 10 + $2; };
	;

digit:
	'0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
	  = { $$ = $1 - '0'; };
	;

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
