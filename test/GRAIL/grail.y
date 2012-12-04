%{
#include &ltstdio.h>
%}
%token MAIN END ID VARIABLE ARRAY NUM INTNUM OF STRUCTURE INTEGER REAL
%token IF THEN ELSE FOR DO WHILE REPEAT UNTIL AT BY WITH POINTPOINT NOT
%token FUNCTION PROCEDURE ASSIGNOP MULOP RELOP TO
%token LT LE EQ NE GT GE RETURN
%token ADDOP COMMENT COLON SEMICOLON TYPEDEF AS STRUCTID
%token LPAR RPAR

%right '='
%left '-' '+'
%left '*' '/'

%%
program:
        opt_typedef_declarations opt_declarations subprogram_declarations MAIN opt_declarations optional_statements END
        ;

opt_typedef_declarations:
        | typedef_declarations
        ;

typedef_declarations:
        typedef_declaration SEMICOLON
        | typedef_declarations typedef_declaration SEMICOLON
        ;

typedef_declaration:
        TYPEDEF ID AS type
        ;

idlist:
        ID
        | idlist ',' ID
        ;

opt_declarations:
        | declarations
        ;

declarations:
        declaration SEMICOLON
        |declarations declaration SEMICOLON
        ;

declaration:
        VARIABLE idlist COLON type
        ;

type:
        ID /* Must check that this type has been defined with typedef */
        | standard_type
        | ARRAY '[' INTNUM POINTPOINT INTNUM ']' OF type
        | STRUCTURE struct_tags END
        ;

struct_tags:
        ID COLON type SEMICOLON
        | struct_tags ID COLON type SEMICOLON
        ;

standard_type:
        INTEGER
        | REAL
        ;

subprogram_declarations:
        | subprogram_declarations subprogram_pro_declaration SEMICOLON
        | subprogram_declarations subprogram_fun_declaration SEMICOLON
        ;

subprogram_fun_declaration:
        subprogram_fun_head opt_declarations optional_statements RETURN expression END
        ;

subprogram_pro_declaration:
        subprogram_pro_head opt_declarations optional_statements END
        ;

subprogram_fun_head:
        FUNCTION ID arguments COLON standard_type
        ;

subprogram_pro_head:
        PROCEDURE ID arguments
        ;

arguments:
        | LPAR parameter_list RPAR
        ;

parameter_list:
        | idlist COLON type
        | parameter_list SEMICOLON idlist COLON type
        ;

optional_statements:
        | statements
        ;

statements:
        statement SEMICOLON
        | statements statement SEMICOLON
        ;

statement:
        ID ASSIGNOP expression
        | procedure_statement
        | IF expression THEN optional_statements ELSE optional_statements END
        | IF expression THEN optional_statements END
        | FOR ID ASSIGNOP expression TO expression DO optional_statements END
        | FOR ID ASSIGNOP expression TO expression BY expression DO optional_statements END
        | WHILE expression DO optional_statements END
        | REPEAT optional_statements UNTIL expression END
        | AT time_expression DO optional_statements END
        | AT time_expression UNTIL time_expression DO optional_statements END
        | AT time_expression UNTIL time_expression BY expression DO optional_statements END
        | WITH station_expression DO optional_statements END
        ;

time_expression:
        expression COLON expression COLON expression COLON expression COLON expression COLON expression;

station_expression:
        expression POINTPOINT expression
        | station_expression ',' expression POINTPOINT expression
        ;


var:
        ID
        | ID '[' expression ']'
        | STRUCTID
        ;

procedure_statement:
        ID
        | ID LPAR expression_list RPAR
        ;

expression_list:
        expression
        | expression_list ',' expression
        ;

expression:
        simple_expression
        | simple_expression RELOP simple_expression
        ;

simple_expression:
        term
        | sign term
        | simple_expression ADDOP term
        ;

term:
        factor
        | term MULOP factor
        ;

factor:
        ID
        | STRUCTID
        | ID LPAR expression_list RPAR
        | NUM
        | LPAR expression RPAR
        | NOT factor
        ;

sign: ADDOP;
%%
int lineno=1;

main (int argc, char** argv)
{
    char*
    progname = argv[1];
    extern yydebug;

    yydebug = 1;
    if (!freopen(progname,"r",stdin))
    {
        fprintf(stderr,"Couldn't open %s\n", progname);
        exit(1);
    }
    yyparse();
}

yyerror(char* s)
{
    fprintf(stderr,"%s line = %d\n",s,lineno);
}

