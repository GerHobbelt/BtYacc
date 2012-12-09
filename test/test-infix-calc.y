/* Infix notation calculator - calc */
%{
#include "test-framework-header.h"
%}

%debug

/* BISON Declarations */
%token NUMBER
%token '-' '+'
%prec '*' '/'
%prec NEG 		/* negation: unary minus */
%prec '^' 		/* exponentiation */
/* Grammar follows */
%%
input: /* empty string */
| input line
;
line: '\n'
| exp '\n' { printf("\t%.10g\n", $1); }
;
exp: NUMBER { $$ = $1; }
| exp '+' exp { $$ = $1 + $3; }					%left
| exp '-' exp { $$ = $1 - $3; }					%left
| exp '*' exp { $$ = $1 * $3; }					%left
| exp '/' exp { $$ = $1 / $3; }					%left
| '-' exp     { $$ = -$2; }				        %nonassoc %prefer
| exp '^' exp { $$ = pow($1, $3); }				%right
| '(' exp ')' { $$ = $2; }
;
%%
#include "test-framework-tail.h"
