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

#include "test-framework-header.h"

#define SHOW_USAGE()  usage()
void usage(void)
{
    printf("Enter a series of 'a' tokens and braces:\n");
}

#include "test-framework-tail.h"
