%define api.pure
%lex-param{void *scanner}
%parse-param{void *scanner}
%parse-param{struct parser_ctx_t *ctx}
%parse-param{struct scanner_ctx_t *scan_ctx}

%{
#include <stdio.h>
/* everything is string, I will leave its type
   for later module to worry about, but I will
   preserve the flex type of the value.
*/
%}

%token TOK_NUM  TOK_STR
%token TOK_NULL TOK_TRUE TOK_FALSE

%%
json_input  :     /* */
            | json_input json_object
;

json_object : '{' name_pairs '}'
;

name_pairs  : name_value
            | name_pairs ',' name_value
;

name_value  : TOK_STR ':' json_value
;

json_value  : json_object       {/* pop value from stack */}
            | json_array
            | TOK_STR
            | TOK_NUM
            | TOK_NULL
            | TOK_TRUE
            | TOK_FALSE
;

json_array  : '[' some_value ']'
;

some_value  : json_value        { /* push to value stack */ }
            | some_value ',' json_value
;
