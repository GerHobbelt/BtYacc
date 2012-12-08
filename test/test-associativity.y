%{
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

typedef double yystype_t;
#define YYSTYPE	yystype_t

#define YYDBPR(yystype)    BTYACC_DEBUG_LOG_PARTIAL("%f", (double)(yystype))
%}

%debug
%verbose
%scannerless

%token '+'

%token NUM

%%

expr: expr '+' expr				%left /* tell rule it's left-associative: this should resolve the S/R conflict */
	  = { $$ = $1 + $3; };
	| NUM			
	  = { $$ = $1; };
    ;

%%

void yyerror(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}
int yylex(void)
{
    int ch;
	yystype_t val;

	yylval = 0;
    do
    {
        ch = getchar();
    } while (ch == ' ' || ch == '\n' || ch == '\t');
    if (ch == EOF || ch == '\x04' /* Control-D */ )
        return 0;
    //printf("lex -> $%02X '%c'\n", ch, (isprint(ch) ? ch : '?'));
	if (isdigit(ch))
	{
		yylval = ch - '0';
		return NUM;
	}
	if (ch == '+')
		return PLUS;
    return ch;
}
int main(void)
{
    fprintf(stderr, "Enter a sequence\n");
    yydebug = 1;
    fprintf(stderr, "yyparse = %d\n", yyparse());
    return 0;
}
