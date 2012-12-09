%{
/*
As per bison manual section 5.7: Mysterious Conflicts
*/

#include "test-framework-header.h"
%}

%token ID

/* include debug code in the generated parser: */
%debug

/* produce a 'scannerless' parser, assign ALL tokens, including the 'ASCII' ones, a token number: */
%scannerless

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

#define SHOW_USAGE()  usage()
static void usage(void)
{
  fprintf(stderr, "Enter a series of: (name) ID, optionally followed by a colon and ID (type),\n"
                  "then followed by a comma:\n");
}

#include "test-framework-tail.h"
