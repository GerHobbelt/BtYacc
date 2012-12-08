/*
Tail Section, which includes all the scafolding to compile and run the generated parser
*/


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
    do
    {
        yylval = 0;
        fprintf(stderr, "yyparse = %d, yylval = %f\n", yyparse(), yylval);
    return 0;
}
