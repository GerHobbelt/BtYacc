%{
#include "test-framework-header.h"
%}

%debug
%verbose
%scannerless

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
    | expr '&' expr			%right	
    | expr '%' expr			%left		
    | expr '=' '=' expr		%right	
    | expr '!' '=' expr		%right	
    | expr '<' expr			%right		
    | expr '>' expr			%right		
    | '-' expr					
    | '+' expr				
    /*| '+' '+' expr		/* this one is reduce/reduce with previous: ambiguous grammar rule! */
    | expr '+' '+' 				
    | '!' expr				%prefer		
    | '&' expr				%prefer			
    | expr '%'				
	| value						
    ;

value: 
	  NUMBER;

%%

#include "test-framework-tail.h"
