%left LO '+' '-'
%left HI '*' '/' '%'
%nonassoc UNARY

%debug

%%

expr: expr op1 expr %prec LO
    | expr op2 expr %prec HI
    | unary expr %prec UNARY
    ;

op1 : '+'
    | '-'
    ;

op2 : '*'
    | '/'
    | '%'
    ;

unary : '+' | '-' | '*' | '&' ;



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
	fprintf(stderr, "Enter a + - * & / %% sequence\n");
	yydebug = 1;
	fprintf(stderr, "yyparse = %d\n", yyparse());
	return 0;
}
