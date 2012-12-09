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
	  NUMBER;

%%

#include "test-framework-tail.h"
