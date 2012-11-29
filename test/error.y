%%
S: error
%%
int main(void)
{
	printf("yyparse() = %d\n", yyparse());
	return 0;
}
int yylex(void)
{
	return -1;
}
void yyerror(char const *s)
{
	printf("%s\n", s);
}
