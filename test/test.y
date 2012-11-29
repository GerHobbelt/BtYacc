%{
/* first section */
%}
%%
%{
/* second section */
%}
S : /* empty */ { printf("S -> epsilon\n"); }
  | '(' S ')' S { printf("S -> ( S ) S\n"); }
%ifdef ABC
    /* see how preprocessor can be used */
  | '*'         { printf("S -> *\n"); }
%endif
  | 'a'         { printf("S -> 'a'\n"); }
  ;
%%
#include <stdio.h>

int main(void)
{
  printf("Enter a series of 'a' tokens and braces:\n");
  printf("yyparse() = %d\n", yyparse());
  return 0;
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

void yyerror(char const *s)
{
  printf("%s\n", s);
}
