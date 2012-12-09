/* Infix notation calculator - calc - cf. bison 1.20 manual */
%{
#include "test-framework-header.h"
%}
%debug

/* BISON Declarations */
%token NUMBER
%left '-' '+'
%left '*' '/'
%left NEG 		/* negation: unary minus */
%right '^' 		/* exponentiation */
/* Grammar follows */
%%
input: /* empty string */
| input line
;
line: '\n'
| exp '\n' { printf("\t%.10g\n", $1); }
;
exp: NUMBER { $$ = $1; }
| exp '+' exp { $$ = $1 + $3; }
| exp '-' exp { $$ = $1 - $3; }
| exp '*' exp { $$ = $1 * $3; }
| exp '/' exp { $$ = $1 / $3; }
| '-' exp %prec NEG { $$ = -$2; }
| exp '^' exp { $$ = pow($1, $3); }
| '(' exp ')' { $$ = $2; }
;
%%
#include "test-framework-tail.h"
