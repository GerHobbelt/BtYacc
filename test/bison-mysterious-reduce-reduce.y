%{
/*
As per bison manual section 5.7: Mysterious Conflicts
*/

#define YYERROR_DETAILED

%}

%token ID

%debug

%%
%{
/* second section */
%}

def: param_spec return_spec ',';

param_spec:
      type
    | name_list ':' type
    ;

return_spec:
      type
    | name ':' type
    ;

type: ID;

name: ID;

name_list:
      name
    | name ',' name_list
    ;

%%
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void)
{
  yydebug = 1;
  fprintf(stderr, "Enter a series of: (name) ID, optionally followed by a colon and ID (type),\n"
                  "then followed by a comma:\n");
  fprintf(stderr, "yyparse() = %d\n", yyparse());
  return 0;
}

int yylex(void)
{
    int ch;
    int tok;

    do
    {
        ch = getchar();
    } while (ch == ' ' || ch == '\n' || ch == '\t');
    if (ch == EOF || ch == '\x04' /* Control-D */ )
        return 0;

    tok = YYERRCODE;
    if (isalpha(ch))
    {
        tok = ID;
    }
    else if (ch == ':' || ch == ',')
    {
        tok = ch;
    }
    fprintf(stderr, "lex -> $%02X '%c' [%s]\n", ch, (isprint(ch) ? ch : '?'), (tok < YYMAXTOKEN ? yyname[tok] : "illegal-token"));
    return tok;
}

void yyerror_detailed(char const *msg, int lexed_token, YYSTYPE yylval, YYPOSN yyposn)
{
  fprintf(stderr, "%s (when parsing token '%s' at input position %d)\n", msg, ((lexed_token >= 0 && lexed_token < YYMAXTOKEN) ? yyname[lexed_token] : "illegal-token"), yyposn);
}
