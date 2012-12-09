%{
#include "test-framework-header.h"
%}

%debug
%verbose
%scannerless

%token '+' '-'
%token '*' '/'

%prec '=' '<' '>'
%prec '(' ')'

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

    | '!' expr									%prefer		
	  = { $$ = fabs($2) <= EPSILON; };

    | expr '%'					
	  = { $$ = $2 / 100.0; };

	| value						
	  = { $$ = $1; };
    ;

value: 
	  NUMBER
	  = { $$ = $1; };
	;

%%

#include "test-framework-tail.h"
