%{
#include "test-framework-header.h"
%}

%debug
%verbose
%scannerless

%token '+'

%token NUMBER

%%

expr: expr '+' expr				%left /* tell rule it's left-associative: this should resolve the S/R conflict */
	  = { $$ = $1 + $3; };
	| NUMBER			
	  = { $$ = $1; };
    ;

%%

#include "test-framework-tail.h"
