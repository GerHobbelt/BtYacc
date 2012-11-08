#line 8 "btyaccpa.ske"

//
// @(#)btyaccpar, based on byacc 1.8 (Berkeley)
//
#define YYBTYACC 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* [i_a] MSVC doesn't define TRUE in the expected places; do it here to make sure we got it 'right' */
#ifndef TRUE
  #undef FALSE
  #undef TRUE
  
  #define FALSE 0
  #define TRUE (!FALSE)
#endif

typedef int Yshort;

#line 10 "..\flex-2.5.4\parse.y"
/*-
* Copyright (c) 1990 The Regents of the University of California.
* All rights reserved.
*
* This code is derived from software contributed to Berkeley by
* Vern Paxson.
* 
* The United States Government has rights in this work pursuant
* to contract no. DE-AC03-76SF00098 between the United States
* Department of Energy and the University of California.
*
* Redistribution and use in source and binary forms with or without
* modification are permitted provided that: (1) source distributions retain
* this entire copyright notice and comment, and (2) distributions including
* binaries display the following acknowledgement:  ``This product includes
* software developed by the University of California, Berkeley and its
* contributors'' in the documentation or other materials provided with the
* distribution and in all advertising materials mentioning features or use
* of this software.  Neither the name of the University nor the names of
* its contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

/* $Header: /home/daffy/u0/vern/flex/RCS/parse.y,v 2.28 95/04/21 11:51:51 vern Exp $ */

#include "config.h"

/* Some versions of bison are broken in that they use alloca() but don't
* declare it properly.  The following is the patented (just kidding!)
* #ifdef chud to fix the problem, courtesy of Francois Pinard.
*/
#ifdef YYBISON
/* AIX requires this to be the first thing in the file.  What a piece.  */
# ifdef _AIX
 #pragma alloca
# endif
#endif

#include "flexdef.h"
#include "y_tab.h"

/* The remainder of the alloca() cruft has to come after including flexdef.h,
* so HAVE_ALLOCA_H is (possibly) defined.
*/
#ifdef YYBISON
# ifdef __GNUC__
#  ifndef alloca
#   define alloca __builtin_alloca
#  endif
# else
#  if HAVE_ALLOCA_H
#   include <alloca.h>
#  else
#   ifdef __hpux
void *alloca ();
#   else
#    ifdef __TURBOC__
#     include <malloc.h>
#    else
char *alloca ();
#    endif
#   endif
#  endif
# endif
#endif

/* Bletch, ^^^^ that was ugly! */

#ifndef TRUE
#undef FALSE
#undef TRUE
#define FALSE 0
#define TRUE (!FALSE)
#endif


int pat, scnum, eps, headcnt, trailcnt, anyccl, lastchar, i, rulelen;
int trlcontxt, xcluflg, currccl, cclsorted, varlength, variable_trail_rule;

int *scon_stk;
int scon_stk_ptr;

static int madeany = false;  /* whether we've made the '.' character class */
int previous_continued_action;	/* whether the previous rule's action was '|' */

/* Expand a POSIX character class expression. */
#define CCL_EXPR(func) \
	{ \
	int c; \
	for ( c = 0; c < csize; ++c ) \
		if ( isascii(c) && func(c) ) \
			ccladd( currccl, c ); \
	}

/* While POSIX defines isblank(), it's not ANSI C. */
#define IS_BLANK(c) ((c) == ' ' || (c) == '\t')

/* On some over-ambitious machines, such as DEC Alpha's, the default
* token type is "long" instead of "int"; this leads to problems with
* declaring yylval in flexdef.h.  But so far, all the yacc's I've seen
* wrap their definitions of YYSTYPE with "#ifndef YYSTYPE"'s, so the
* following should ensure that the default token type is "int".
*/
#define YYSTYPE int

#line 133 "y.tab.c"
static Yshort yylhs[] = {                                        -1,
    0,    1,    2,    2,    2,    2,    3,    6,    6,    7,
    7,    7,    8,    9,    9,   10,   10,   10,    4,    4,
    4,    5,   12,   12,   12,   12,   14,   11,   11,   11,
   15,   15,   15,   16,   13,   13,   13,   13,   18,   18,
   17,   19,   19,   20,   20,   20,   20,   20,   20,   20,
   20,   20,   20,   20,   20,   21,   21,   23,   23,   23,
   23,   24,   24,   24,   24,   24,   24,   24,   24,   24,
   24,   24,   24,   22,   22,
};
static Yshort yylen[] = {                                         2,
    5,    0,    3,    2,    0,    1,    1,    1,    1,    2,
    1,    1,    2,    2,    0,    3,    3,    3,    5,    5,
    0,    0,    2,    1,    1,    1,    0,    4,    3,    0,
    3,    1,    1,    1,    2,    3,    2,    1,    3,    1,
    2,    2,    1,    2,    2,    2,    6,    5,    4,    1,
    1,    1,    3,    3,    1,    3,    4,    4,    2,    2,
    0,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    2,    0,
};
static Yshort yydefred[] = {                                      2,
    0,    0,    6,    0,    7,    8,    9,   15,   21,    0,
    4,    0,    0,   12,   11,    0,    0,    0,    0,   14,
    0,    1,    0,   10,    0,    0,    0,    0,    0,   21,
    0,   16,   17,   18,   29,   33,   34,    0,   32,    0,
   26,   55,   52,   25,    0,   50,   75,    0,    0,    0,
   24,    0,    0,    0,    0,   51,   28,    0,   20,   23,
    0,    0,   61,    0,   19,    0,   37,    0,   41,    0,
    0,   44,   45,   46,   31,   74,   53,   54,    0,    0,
   62,   63,   64,   65,   66,   67,   68,   69,   70,   71,
   72,   73,   56,   60,   36,    0,    0,   57,    0,   49,
    0,   58,    0,   48,   47,
};
static Yshort yydgoto[] = {                                       1,
    2,    4,    9,   13,   22,   10,   16,   11,   12,   20,
   23,   50,   51,   29,   38,   39,   52,   53,   54,   55,
   56,   61,   64,   94,
};
static Yshort yysindex[] = {                                      0,
    0, -235,    0, -191,    0,    0,    0,    0,    0, -207,
    0, -215,  -18,    0,    0, -202,    4,   26,   32,    0,
   41,    0,  -35,    0, -168, -166, -165,   38, -180,    0,
  -30,    0,    0,    0,    0,    0,    0,  -16,    0,  -40,
    0,    0,    0,    0,   -2,    0,    0,   -2,    8,   93,
    0,   -2,  -25,   -2,   15,    0,    0, -153,    0,    0,
  -27,  -26,    0,  -88,    0,  -23,    0,   -2,    0,   15,
 -150,    0,    0,    0,    0,    0,    0,    0,   -3,   65,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   -2,  -21,    0, -145,    0,
 -116,    0,  -12,    0,    0,
};
static Yshort yyrindex[] = {                                      0,
    0, -188,    0,    0,    0,    0,    0,    0,    0,    0,
    0, -154,    1,    0,    0, -140,    0,    0,    0,    0,
 -176,    0,  -28,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -32,
    0,    0,    0,    0,    0,    0,    0,    0,   22,    0,
    0,    0,  106,    7,  -10,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  108,    0,    0,    0,   -7,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   46,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    9,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
static Yshort yycindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
};
static Yshort yygindex[] = {                                      0,
    0,    0,    0,   92,  100,    0,    0,    0,    0,    0,
    0,    0,   81,    0,    0,   69,    0,   27,   60,  -29,
    0,    0,   66,    0,
};
#define YYTABLESIZE 326
static Yshort yytable[] = {                                      43,
   22,   30,   42,   47,   93,   22,   77,   30,  104,   48,
   67,   22,   95,   30,   78,   46,   40,   22,   39,   21,
    3,   69,  101,   43,   70,   43,   42,   58,   42,   43,
   43,   47,   42,   42,   30,   43,   43,   48,   42,   42,
   30,   21,   40,   46,   39,   57,   30,   40,   14,   39,
   17,   18,   19,   40,   15,   39,   72,   73,   30,   24,
   49,   30,   22,   45,   25,   22,   70,    5,    6,    7,
    5,    5,    5,    8,   62,   36,    5,   74,   66,   27,
   43,   37,   28,   42,   59,   27,   26,   30,   49,   98,
   30,   30,   27,   32,   30,   33,   34,   68,   68,   35,
   68,   63,   65,  100,   13,   13,   13,   97,   37,   99,
   13,  102,  105,   43,   61,   38,   42,   35,    3,    3,
    3,   40,   31,   30,    3,   60,   75,   96,   79,    0,
   40,    0,   39,    0,    0,    0,    0,   71,   59,    0,
    0,  103,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   80,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,
   91,   92,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   30,   30,   41,   42,   22,   22,   76,
   30,   30,   43,   44,   22,   22,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   43,    0,    0,   42,
    0,    0,   43,   80,   42,   42,   30,   30,    0,    0,
   43,    0,    0,   30,   30,   81,   82,   83,   84,   85,
   86,   87,   88,   89,   90,   91,   92,    0,   61,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   61,   61,   61,   61,   61,   61,   61,   61,   61,   61,
   61,   61,   59,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   59,   59,   59,   59,   59,   59,
   59,   59,   59,   59,   59,   59,
};
static Yshort yycheck[] = {                                      10,
    0,   34,   10,   34,   93,   34,   34,   40,  125,   40,
   36,   40,   36,   46,   41,   46,   10,   46,   10,   60,
  256,   47,   44,   34,   54,   36,   34,   44,   36,   40,
   41,   34,   40,   41,   34,   46,   47,   40,   46,   47,
   40,   60,   36,   46,   36,   62,   46,   41,  256,   41,
  266,  267,  268,   47,  262,   47,   42,   43,   91,  262,
   91,   94,   91,   94,   61,   94,   96,  259,  260,  261,
  259,  260,  261,  265,   48,  256,  265,   63,   52,  256,
   91,  262,   42,   91,  125,  262,   61,  123,   91,   93,
  123,   91,   61,  262,   94,  262,  262,  124,  124,   62,
  124,   94,   10,  125,  259,  260,  261,  258,  262,   45,
  265,  257,  125,  124,   93,   10,  124,   10,  259,  260,
  261,   30,   23,  123,  265,   45,   58,   68,   63,   -1,
  124,   -1,  124,   -1,   -1,   -1,   -1,  123,   93,   -1,
   -1,  258,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  256,  257,  256,  257,  256,  257,  257,
  263,  264,  263,  264,  263,  264,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,   -1,   -1,  257,
   -1,   -1,  263,  257,  257,  263,  256,  257,   -1,   -1,
  263,   -1,   -1,  263,  264,  269,  270,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,   -1,  257,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  280,  257,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  269,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,
};
static Yshort yyctable[] = {                                      0,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 280
#if YYDEBUG
static char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,"'\\n'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'\"'",0,"'$'",0,0,0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,
0,0,0,0,0,0,0,0,0,0,"'<'","'='","'>'","'?'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,"'['",0,"']'","'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,"'{'","'|'","'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"CHAR","NUMBER","SECTEND",
"SCDECL","XSCDECL","NAME","PREVCCL","EOF_OP","OPTION_OP","OPT_OUTFILE",
"OPT_PREFIX","OPT_YYCLASS","CCE_ALNUM","CCE_ALPHA","CCE_BLANK","CCE_CNTRL",
"CCE_DIGIT","CCE_GRAPH","CCE_LOWER","CCE_PRINT","CCE_PUNCT","CCE_SPACE",
"CCE_UPPER","CCE_XDIGIT",
};
static char *yyrule[] = {
"$accept : goal",
"goal : initlex sect1 sect1end sect2 initforrule",
"initlex :",
"sect1 : sect1 startconddecl namelist1",
"sect1 : sect1 options",
"sect1 :",
"sect1 : error",
"sect1end : SECTEND",
"startconddecl : SCDECL",
"startconddecl : XSCDECL",
"namelist1 : namelist1 NAME",
"namelist1 : NAME",
"namelist1 : error",
"options : OPTION_OP optionlist",
"optionlist : optionlist option",
"optionlist :",
"option : OPT_OUTFILE '=' NAME",
"option : OPT_PREFIX '=' NAME",
"option : OPT_YYCLASS '=' NAME",
"sect2 : sect2 scon initforrule flexrule '\\n'",
"sect2 : sect2 scon '{' sect2 '}'",
"sect2 :",
"initforrule :",
"flexrule : '^' rule",
"flexrule : rule",
"flexrule : EOF_OP",
"flexrule : error",
"scon_stk_ptr :",
"scon : '<' scon_stk_ptr namelist2 '>'",
"scon : '<' '*' '>'",
"scon :",
"namelist2 : namelist2 ',' sconname",
"namelist2 : sconname",
"namelist2 : error",
"sconname : NAME",
"rule : re2 re",
"rule : re2 re '$'",
"rule : re '$'",
"rule : re",
"re : re '|' series",
"re : series",
"re2 : re '/'",
"series : series singleton",
"series : singleton",
"singleton : singleton '*'",
"singleton : singleton '+'",
"singleton : singleton '?'",
"singleton : singleton '{' NUMBER ',' NUMBER '}'",
"singleton : singleton '{' NUMBER ',' '}'",
"singleton : singleton '{' NUMBER '}'",
"singleton : '.'",
"singleton : fullccl",
"singleton : PREVCCL",
"singleton : '\"' string '\"'",
"singleton : '(' re ')'",
"singleton : CHAR",
"fullccl : '[' ccl ']'",
"fullccl : '[' '^' ccl ']'",
"ccl : ccl CHAR '-' CHAR",
"ccl : ccl CHAR",
"ccl : ccl ccl_expr",
"ccl :",
"ccl_expr : CCE_ALNUM",
"ccl_expr : CCE_ALPHA",
"ccl_expr : CCE_BLANK",
"ccl_expr : CCE_CNTRL",
"ccl_expr : CCE_DIGIT",
"ccl_expr : CCE_GRAPH",
"ccl_expr : CCE_LOWER",
"ccl_expr : CCE_PRINT",
"ccl_expr : CCE_PUNCT",
"ccl_expr : CCE_SPACE",
"ccl_expr : CCE_UPPER",
"ccl_expr : CCE_XDIGIT",
"string : string CHAR",
"string :",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#line 51 "btyaccpa.ske"

#ifndef YYERRCODE
#error Make sure you '#include' the generated yacc .h headerfile in the .Y file itself, as it defines YYERRCODE and several other constants.
#endif


//
// YYPOSN is user-defined text position type.
//
#ifndef YYPOSN
#define YYPOSN int
#endif

#ifdef YYREDUCEPOSNFUNC
#define YYCALLREDUCEPOSN(e)                                                 \\
    if(reduce_posn)                                                         \\
    {                                                                       \\
      YYREDUCEPOSNFUNC(yyps->pos, &(yyps->psp)[1-yym], &(yyps->vsp)[1-yym], \\
               yym, yyps->psp - yyps->ps, yychar, yyposn, e);               \\
      reduce_posn = 0;                                                      \\
    }

#ifndef YYCALLREDUCEPOSNARG
#define YYCALLREDUCEPOSNARG yyps->val
#endif


#define YYPOSNARG(n) ((yyps->psp)[1-yym+(n)-1])
#define YYPOSNOUT    (yyps->pos)
#endif

/* If delete function is not defined by the user, do not delete. */
#ifndef YYDELETEVAL
#define YYDELETEVAL(v, t) 
#endif
 
/* If delete function is not defined by the user, do not delete. */
#ifndef YYDELETEPOSN
#define YYDELETEPOSN(v, t) 
#endif

#define yyclearin (yychar=(-1))

#define yyerrok (yyps->errflag=0)

#ifndef YYSTACKGROWTH
#define YYSTACKGROWTH 16
#endif

#ifndef YYDEFSTACKSIZE
#define YYDEFSTACKSIZE 12
#endif

#ifdef YYDEBUG
int yydebug;
#endif

int yynerrs;

/* These value/posn are taken from the lexer */
YYSTYPE yylval;
YYPOSN  yyposn;

/* These value/posn of the root non-terminal are returned to the caller */
YYSTYPE yyretlval;
YYPOSN  yyretposn;

#define YYABORT  goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab
#define YYVALID                                      \\
            do {                                     \\
                if (yyps->save)                      \\
                    goto yyvalid;                    \\
            } while(0)
#define YYVALID_NESTED                               \\
            do {                                     \\
                if (yyps->save                       \\
                    && yyps->save->save == 0)        \\
                    goto yyvalid;                    \\
            } while(0)

struct yyparsestate {
  struct yyparsestate *save; // Previously saved parser state
  int           state;
  int           errflag;
  Yshort       *ssp;         // state stack pointer
  YYSTYPE      *vsp;         // value stack pointer
  YYPOSN       *psp;         // position stack pointer
  YYSTYPE       val;         // value as returned by actions
  YYPOSN        pos;         // position as returned by universal action
  Yshort       *ss;          // state stack base
  YYSTYPE      *vs;          // values stack base
  YYPOSN       *ps;          // position stack base
  int           lexeme;      // index of the conflict lexeme in the lexical queue
  unsigned int  stacksize;   // current maximum stack size
  Yshort        ctry;        // index in yyctable[] for this conflict
};

// Current parser state
static struct yyparsestate *yyps=0;

// yypath!=NULL: do the full parse, starting at *yypath parser state.
static struct yyparsestate *yypath=0;

// Base of the lexical value queue
static YYSTYPE *yylvals=0;

// Current posistion at lexical value queue
static YYSTYPE *yylvp=0;

// End position of lexical value queue
static YYSTYPE *yylve=0;

// The last allocated position at the lexical value queue
static YYSTYPE *yylvlim=0;

// Base of the lexical position queue
static YYPOSN *yylpsns=0;

// Current posistion at lexical position queue
static YYPOSN *yylpp=0;

// End position of lexical position queue
static YYPOSN *yylpe=0;

// The last allocated position at the lexical position queue
static YYPOSN *yylplim=0;

// Current position at lexical token queue
static Yshort *yylexp=0;

static Yshort *yylexemes=0;

//
// For use in generated program
//
#define yytrial (yyps->save)
#define yyvsp   (yyps->vsp)
#define yyval   (yyps->val)
#define yydepth (yyps->ssp - yyps->ss)


//
// Local prototypes.
//
int yyparse(void);

int  YYLex1();
int  yyexpand();
void YYSCopy(YYSTYPE *to, YYSTYPE *from, int size);
void YYPCopy(YYPOSN  *to, YYPOSN  *from, int size);
void YYMoreStack(struct yyparsestate *yyps);
struct yyparsestate *YYNewState(int size);
void YYFreeState(struct yyparsestate *p);


#line 785 "..\flex-2.5.4\parse.y"


/* build_eof_action - build the "<<EOF>>" action for the active start
 *                    conditions
 */

void build_eof_action()
	{
	register int i;
	char action_text[MAXLINE];

	for ( i = 1; i <= scon_stk_ptr; ++i )
		{
		if ( sceof[scon_stk[i]] )
			format_pinpoint_message(
				"multiple <<EOF>> rules for start condition %s",
				scname[scon_stk[i]] );

		else
			{
			sceof[scon_stk[i]] = true;
			sprintf( action_text, "case YY_STATE_EOF(%s):\n",
				scname[scon_stk[i]] );
			add_action( action_text );
			}
		}

	line_directive_out( (FILE *) 0, 1 );

	/* This isn't a normal rule after all - don't count it as
	 * such, so we don't have any holes in the rule numbering
	 * (which make generating "rule can never match" warnings
	 * more difficult.
	 */
	--num_rules;
	++num_eof_rules;
	}


/* format_synerr - write out formatted syntax error */

void format_synerr( msg, arg )
char msg[], arg[];
	{
	char errmsg[MAXLINE];

	(void) sprintf( errmsg, msg, arg );
	synerr( errmsg );
	}


/* synerr - report a syntax error */

void synerr( str )
char str[];
	{
	syntaxerror = true;
	pinpoint_message( str );
	}


/* format_warn - write out formatted warning */

void format_warn( msg, arg )
char msg[], arg[];
	{
	char warn_msg[MAXLINE];

	(void) sprintf( warn_msg, msg, arg );
	warn( warn_msg );
	}


/* warn - report a warning, unless -w was given */

void warn( str )
char str[];
	{
	line_warning( str, linenum );
	}

/* format_pinpoint_message - write out a message formatted with one string,
 *			     pinpointing its location
 */

void format_pinpoint_message( msg, arg )
char msg[], arg[];
	{
	char errmsg[MAXLINE];

	(void) sprintf( errmsg, msg, arg );
	pinpoint_message( errmsg );
	}


/* pinpoint_message - write out a message, pinpointing its location */

void pinpoint_message( str )
char str[];
	{
	line_pinpoint( str, linenum );
	}


/* line_warning - report a warning at a given line, unless -w was given */

void line_warning( str, line )
char str[];
int line;
	{
	char warning[MAXLINE];

	if ( ! nowarn )
		{
		sprintf( warning, "warning, %s", str );
		line_pinpoint( warning, line );
		}
	}


/* line_pinpoint - write out a message, pinpointing it at the given line */

void line_pinpoint( str, line )
char str[];
int line;
	{
	fprintf( stderr, "\"%s\", line %d: %s\n", infilename, line, str );
	}


/* yyerror - eat up an error message from the parser;
 *	     currently, messages are ignore
 */

void yyerror( msg )
char msg[];
	{
	}
#line 713 "y.tab.c"
#line 209 "btyaccpa.ske"


//
// Parser function
//
int yyparse()
{
  int yym, yyn, yystate, yychar, yynewerrflag;
  struct yyparsestate *yyerrctx = NULL;
  int reduce_posn;

#if YYDEBUG
  char *yys;

  if ((yys = getenv("YYDEBUG")))
  {
    yyn = *yys;
    if (yyn >= '0' && yyn <= '9')
      yydebug = yyn - '0';
  }
#endif

  yyps = YYNewState(YYDEFSTACKSIZE);
  yyps->save = 0;
  yynerrs = 0;
  yyps->errflag = 0;
  yychar = (-1);

  yyps->ssp = yyps->ss;
  yyps->vsp = yyps->vs;
  yyps->psp = yyps->ps;
  *(yyps->ssp) = yystate = 0;


  //
  // Main parsing loop
  //
yyloop:
  if ((yyn = yydefred[yystate]))
  {
    goto yyreduce;
  }

  //
  // Read one token
  //
  if (yychar < 0)
  {
    if ((yychar = YYLex1()) < 0)
      yychar = 0;
#if YYDEBUG
    if (yydebug)
    {
      yys = 0;
      if (yychar <= YYMAXTOKEN)
        yys = yyname[yychar];
      if (!yys)
        yys = "illegal-symbol";
      printf("yydebug[%d,%d]: state %d, reading %d (%s)",
             yydepth, (int)yytrial, yystate, yychar, yys);
#ifdef YYDBPR
      printf("<");
      YYDBPR(yylval);
      printf(">");
#endif
      printf("\\n");
    }
#endif
  }

  //
  // Do we have a conflict?
  //
  if ((yyn = yycindex[yystate]) &&
      (yyn += yychar) >= 0 &&
      yyn <= YYTABLESIZE &&
      yycheck[yyn] == yychar)
  {
    int ctry;

    if (yypath)
    {
#if YYDEBUG
      if (yydebug)
      {
        printf("yydebug[%d,%d]: CONFLICT in state %d: following successful trial parse\\n",
               yydepth, (int)yytrial, yystate);
      }
#endif
      // Switch to the next conflict context
	  {
        struct yyparsestate *save = yypath;
        yypath = save->save;
        ctry = save->ctry;
        if (save->state != yystate)
          goto yyabort;
        YYFreeState(save);
	  }
    }
    else
    {
#if YYDEBUG
      if (yydebug)
      {
        printf("yydebug[%d,%d]: CONFLICT in state %d. ",
               yydepth, (int)yytrial, yystate);
        if(yyps->save)
        {
          printf("ALREADY in conflict. Continue trial parse.");
        }
        else
        {
          printf("Start trial parse.");
        }
        printf("\\n");
      }
#endif
	  {
        struct yyparsestate *save = YYNewState(yyps->ssp - yyps->ss);
        save->save    = yyps->save;
        save->state   = yystate;
        save->errflag = yyps->errflag;
        save->ssp     = save->ss + (yyps->ssp - yyps->ss);
        save->vsp     = save->vs + (yyps->vsp - yyps->vs);
        save->psp     = save->ps + (yyps->psp - yyps->ps);
        memcpy (save->ss, yyps->ss, (yyps->ssp - yyps->ss + 1)*sizeof(Yshort));
        YYSCopy(save->vs, yyps->vs, (yyps->ssp - yyps->ss + 1));
        YYPCopy(save->ps, yyps->ps, (yyps->ssp - yyps->ss + 1));
        ctry = yytable[yyn];
        if (yyctable[ctry] == -1)
		{
#if YYDEBUG
          if (yydebug && yychar >= 0)
            printf("yydebug[%d]: backtracking 1 token\\n",
                   (int)yytrial);
#endif
          ctry++;
		}
        save->ctry = ctry;
        if (!yyps->save)
		{
          // If this is a first conflict in the stack, start saving lexemes
          if (!yylexemes)
		  {
#ifdef __cplusplus
            yylexemes = new Yshort[YYSTACKGROWTH];
            yylvals = new YYSTYPE[YYSTACKGROWTH];
            yylpsns = new YYPOSN[YYSTACKGROWTH];
#else
            yylexemes = malloc(sizeof(Yshort)*YYSTACKGROWTH);
            yylvals = malloc(sizeof(YYSTYPE)*YYSTACKGROWTH);
            yylpsns = malloc(sizeof(YYPOSN)*YYSTACKGROWTH);
#endif
            yylplim = yylpsns + YYSTACKGROWTH;
            yylvlim = yylvals + YYSTACKGROWTH;
		  }
          if (yylvp == yylve)
		  {
            yylvp = yylve = yylvals;
            yylpp = yylpe = yylpsns;
            yylexp = yylexemes;
            if (yychar >= 0)
			{
              *yylve++ = yylval;
              *yylpe++ = yyposn;
              *yylexp = yychar;
              yychar = -1;
			}
		  }
		}
        if (yychar >= 0)
		{
          yylvp--, yylpp--, yylexp--;
          yychar = -1;
		}
        save->lexeme = yylvp - yylvals;
        yyps->save = save;
	  }
    }
    if (yytable[yyn] == ctry)
    {
#if YYDEBUG
      if (yydebug)
        printf("yydebug[%d,%d]: state %d, shifting to state %d\\n",
               yydepth, (int)yytrial, yystate, yyctable[ctry]);
#endif
      if (yychar < 0)
        yylvp++, yylpp++, yylexp++;
      yychar = -1;
      if (yyps->errflag > 0)
        --yyps->errflag;
      yystate = yyctable[ctry];
      goto yyshift;
    }
    else
    {
      yyn = yyctable[ctry];
      goto yyreduce;
    }
  }

  //
  // Is action a shift?
  //
  if ((yyn = yysindex[yystate]) &&
      (yyn += yychar) >= 0 &&
      yyn <= YYTABLESIZE &&
      yycheck[yyn] == yychar)
  {
#if YYDEBUG
    if (yydebug)
      printf("yydebug[%d,%d]: state %d, shifting to state %d\\n",
             yydepth, (int)yytrial, yystate, yytable[yyn]);
#endif
    yychar = (-1);
    if (yyps->errflag > 0)
      --yyps->errflag;
    yystate = yytable[yyn];
yyshift:
    if (yyps->ssp >= yyps->ss + yyps->stacksize - 1)
    {
      YYMoreStack(yyps);
    }
    *++(yyps->ssp) = yystate;
    *++(yyps->vsp) = yylval;
    *++(yyps->psp) = yyposn;
    goto yyloop;
  }
  if ((yyn = yyrindex[yystate]) &&
      (yyn += yychar) >= 0 &&
      yyn <= YYTABLESIZE &&
      yycheck[yyn] == yychar)
  {
    yyn = yytable[yyn];
    goto yyreduce;
  }

  //
  // Action: error
  //
  if (yyps->errflag)
    goto yyinrecovery;
  yynewerrflag = 1;
  goto yyerrhandler;

yyerrlab:
  yynewerrflag = 0;
yyerrhandler:
  while (yyps->save)
  {
    int ctry;
    struct yyparsestate *save = yyps->save;
#if YYDEBUG
    if (yydebug)
      printf("yydebug[%d,%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\\n",
             yydepth, (int)yytrial, yystate, yyps->save->state, yylvp - yylvals - yyps->save->lexeme);
#endif
    // Memorize most forward-looking error state in case
    // it's really an error.
    if(yyerrctx==NULL || yyerrctx->lexeme<yylvp-yylvals)
    {
      // Free old saved error context state
      if(yyerrctx) YYFreeState(yyerrctx);
      // Create and fill out new saved error context state
      yyerrctx = YYNewState(yyps->ssp - yyps->ss);
      yyerrctx->save = yyps->save;
      yyerrctx->state = yystate;
      yyerrctx->errflag = yyps->errflag;
      yyerrctx->ssp = yyerrctx->ss + (yyps->ssp - yyps->ss);
      yyerrctx->vsp = yyerrctx->vs + (yyps->vsp - yyps->vs);
      yyerrctx->psp = yyerrctx->ps + (yyps->psp - yyps->ps);
      memcpy (yyerrctx->ss, yyps->ss, (yyps->ssp - yyps->ss + 1)*sizeof(Yshort));
      YYSCopy(yyerrctx->vs, yyps->vs, (yyps->ssp - yyps->ss + 1));
      YYPCopy(yyerrctx->ps, yyps->ps, (yyps->ssp - yyps->ss + 1));
      yyerrctx->lexeme = yylvp - yylvals;
    }
    yylvp  = yylvals   + save->lexeme;
    yylpp  = yylpsns   + save->lexeme;
    yylexp = yylexemes + save->lexeme;
    yychar = -1;
    yyps->ssp = yyps->ss + (save->ssp - save->ss);
    yyps->vsp = yyps->vs + (save->vsp - save->vs);
    yyps->psp = yyps->ps + (save->psp - save->ps);
    memcpy (yyps->ss, save->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));
    YYSCopy(yyps->vs, save->vs,  yyps->vsp - yyps->vs + 1);
    YYPCopy(yyps->ps, save->ps,  yyps->psp - yyps->ps + 1);
    ctry = ++save->ctry;
    yystate = save->state;
    // We tried shift, try reduce now
    if ((yyn = yyctable[ctry]) >= 0)
    {
      goto yyreduce;
    }
    yyps->save = save->save;
    YYFreeState(save);
    //
    // Nothing left on the stack -- error
    //
    if (!yyps->save)
    {
#if YYDEBUG
      if (yydebug)
      {
        printf("yydebug[%d]: trial parse FAILED, entering ERROR mode\\n",
           (int)yytrial);
      }
#endif
      // Restore state as it was in the most forward-advanced error
      yylvp  = yylvals   + yyerrctx->lexeme;
      yylpp  = yylpsns   + yyerrctx->lexeme;
      yylexp = yylexemes + yyerrctx->lexeme;
      yychar = yylexp[-1];
      yylval = yylvp[-1];
      yyposn = yylpp[-1];
      yyps->ssp = yyps->ss + (yyerrctx->ssp - yyerrctx->ss);
      yyps->vsp = yyps->vs + (yyerrctx->vsp - yyerrctx->vs);
      yyps->psp = yyps->ps + (yyerrctx->psp - yyerrctx->ps);
      memcpy (yyps->ss, yyerrctx->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));
      YYSCopy(yyps->vs, yyerrctx->vs,  yyps->vsp - yyps->vs + 1);
      YYPCopy(yyps->ps, yyerrctx->ps,  yyps->psp - yyps->ps + 1);
      yystate = yyerrctx->state;
      YYFreeState(yyerrctx);
      yyerrctx = NULL;
    }
    yynewerrflag = 1;
  }
  if (yynewerrflag)
  {
#ifdef YYERROR_DETAILED
    yyerror_detailed("syntax error", yychar, yylval, yyposn);
#else
    yyerror("syntax error");
#endif
  }
  ++yynerrs;
yyinrecovery:
  if (yyps->errflag < 3)
  {
    yyps->errflag = 3;
    for (;;)
    {
      if ((yyn = yysindex[*(yyps->ssp)]) &&
          (yyn += YYERRCODE) >= 0 &&
          yyn <= YYTABLESIZE &&
          yycheck[yyn] == YYERRCODE)
      {
#if YYDEBUG
        if (yydebug)
          printf("yydebug[%d,%d]: state %d, ERROR recovery shifts to state %d\\n",
                 yydepth, (int)yytrial, *(yyps->ssp), yytable[yyn]);
#endif
        /* Use label yyerrlab, so that compiler does not warn */
        if (yyps->errflag != yyps->errflag)
          goto yyerrlab;
        yystate = yytable[yyn];
        goto yyshift;
      }
      else
      {
#if YYDEBUG
        if (yydebug)
          printf("yydebug[%d,%d]: ERROR recovery discards state %d\\n",
                 yydepth, (int)yytrial, *(yyps->ssp));
#endif
        if (yyps->ssp <= yyps->ss)
        {
          goto yyabort;
        }
        if (!yytrial)
        {
          YYDELETEVAL(yyps->vsp[0],1);
          YYDELETEPOSN(yyps->psp[0],1);
        }
        --(yyps->ssp);
        --(yyps->vsp);
        --(yyps->psp);
      }
    }
  }
  else
  {
    if (yychar == 0)
      goto yyabort;
#if YYDEBUG
    if (yydebug)
    {
      yys = 0;
      if (yychar <= YYMAXTOKEN)
        yys = yyname[yychar];
      if (!yys)
        yys = "illegal-symbol";
      printf("yydebug[%d,%d]: state %d, ERROR recovery discards token %d (%s)\\n",
             yydepth, (int)yytrial, yystate, yychar, yys);
    }
#endif
    if(!yytrial)
    {
      YYDELETEVAL(yylval,0);
      YYDELETEPOSN(yyposn,0);
    }
    yychar = (-1);
    goto yyloop;
  }

  //
  // Reduce the rule
  //
yyreduce:
  yym = yylen[yyn];
#if YYDEBUG
  if (yydebug)
  {
    printf("yydebug[%d,%d]: state %d, reducing by rule %d (%s)",
           yydepth, (int)yytrial, yystate, yyn, yyrule[yyn]);
#ifdef YYDBPR
    if (yym)
    {
      int i;
      printf("<");
      for (i=yym; i>0; i--)
      {
        if (i!=yym)
          printf(", ");
        YYDBPR((yyps->vsp)[1-i]);
      }
      printf(">");
    }
#endif
    printf("\\n");
  }
#endif
  if (yyps->ssp + 1 - yym >= yyps->ss + yyps->stacksize)
  {
    YYMoreStack(yyps);
  }

# ifdef _YACC_DEFAULT_ACTION_
  /* "$$ = NULL" default action */
  memset(&yyps->val, 0, sizeof(yyps->val));
# else
  /* RA: bison compatibility: default action is '$$ = $1;' */
  if (yym > 0) yyps->val = (yyps->vsp)[1 - yym];
# endif

  /* Default reduced position is NULL -- no position at all. No
     position will be assigned at trial time and if no position
     handling is present */
  memset(&yyps->pos, 0, sizeof(yyps->pos));
 
  reduce_posn = 1;

  switch (yyn)
  {

case 1:
  if (!yytrial)
#line 122 "..\flex-2.5.4\parse.y"
{ /* add default rule */
			int def_rule;

			pat = cclinit();
			cclnegate( pat );

			def_rule = mkstate( -pat );

			/* Remember the number of the default rule so we
		 * don't generate "can't match" warnings for it.
		 */
			default_rule = num_rules;

			finish_rule( def_rule, false, 0, 0 );

			for ( i = 1; i <= lastsc; ++i )
				scset[i] = mkbranch( scset[i], def_rule );

			if ( spprdflt )
				add_action(
				"YY_FATAL_ERROR( \"flex scanner jammed\" )" );
			else
				add_action( "ECHO" );

			add_action( ";\n\tYY_BREAK\n" );
			}
#line 1198 "y.tab.c"
break;
case 2:
  if (!yytrial)
#line 151 "..\flex-2.5.4\parse.y"
{ /* initialize for processing rules */

			/* Create default DFA start condition. */
			scinstal( "INITIAL", false );
			}
#line 1208 "y.tab.c"
break;
case 6:
  if (!yytrial)
#line 162 "..\flex-2.5.4\parse.y"
{ synerr( "unknown error processing section 1" ); }
#line 1214 "y.tab.c"
break;
case 7:
  if (!yytrial)
#line 166 "..\flex-2.5.4\parse.y"
{
			check_options();
			scon_stk = allocate_integer_array( lastsc + 1 );
			scon_stk_ptr = 0;
			}
#line 1224 "y.tab.c"
break;
case 8:
  if (!yytrial)
#line 174 "..\flex-2.5.4\parse.y"
{ xcluflg = false; }
#line 1230 "y.tab.c"
break;
case 9:
  if (!yytrial)
#line 177 "..\flex-2.5.4\parse.y"
{ xcluflg = true; }
#line 1236 "y.tab.c"
break;
case 10:
  if (!yytrial)
#line 181 "..\flex-2.5.4\parse.y"
{ scinstal( nmstr, xcluflg ); }
#line 1242 "y.tab.c"
break;
case 11:
  if (!yytrial)
#line 184 "..\flex-2.5.4\parse.y"
{ scinstal( nmstr, xcluflg ); }
#line 1248 "y.tab.c"
break;
case 12:
  if (!yytrial)
#line 187 "..\flex-2.5.4\parse.y"
{ synerr( "bad start condition list" ); }
#line 1254 "y.tab.c"
break;
case 16:
  if (!yytrial)
#line 198 "..\flex-2.5.4\parse.y"
{
			outfilename = copy_string( nmstr );
			did_outfilename = 1;
			}
#line 1263 "y.tab.c"
break;
case 17:
  if (!yytrial)
#line 203 "..\flex-2.5.4\parse.y"
{ prefix = copy_string( nmstr ); }
#line 1269 "y.tab.c"
break;
case 18:
  if (!yytrial)
#line 205 "..\flex-2.5.4\parse.y"
{ yyclass = copy_string( nmstr ); }
#line 1275 "y.tab.c"
break;
case 19:
  if (!yytrial)
#line 209 "..\flex-2.5.4\parse.y"
{ scon_stk_ptr = yyvsp[-3]; }
#line 1281 "y.tab.c"
break;
case 20:
  if (!yytrial)
#line 211 "..\flex-2.5.4\parse.y"
{ scon_stk_ptr = yyvsp[-3]; }
#line 1287 "y.tab.c"
break;
case 22:
  if (!yytrial)
#line 216 "..\flex-2.5.4\parse.y"
{
			/* Initialize for a parse of one rule. */
			trlcontxt = variable_trail_rule = varlength = false;
			trailcnt = headcnt = rulelen = 0;
			current_state_type = STATE_NORMAL;
			previous_continued_action = continued_action;
			in_rule = true;

			new_rule();
			}
#line 1302 "y.tab.c"
break;
case 23:
  if (!yytrial)
#line 229 "..\flex-2.5.4\parse.y"
{
			pat = yyvsp[0];
			finish_rule( pat, variable_trail_rule,
				headcnt, trailcnt );

			if ( scon_stk_ptr > 0 )
				{
				for ( i = 1; i <= scon_stk_ptr; ++i )
					scbol[scon_stk[i]] =
						mkbranch( scbol[scon_stk[i]],
								pat );
				}

			else
				{
				/* Add to all non-exclusive start conditions,
			 * including the default (0) start condition.
			 */

				for ( i = 1; i <= lastsc; ++i )
					if ( ! scxclu[i] )
						scbol[i] = mkbranch( scbol[i],
									pat );
				}

			if ( ! bol_needed )
				{
				bol_needed = true;

				if ( performance_report > 1 )
					pinpoint_message(
			"'^' operator results in sub-optimal performance" );
				}
			}
#line 1341 "y.tab.c"
break;
case 24:
  if (!yytrial)
#line 265 "..\flex-2.5.4\parse.y"
{
			pat = yyvsp[0];
			finish_rule( pat, variable_trail_rule,
				headcnt, trailcnt );

			if ( scon_stk_ptr > 0 )
				{
				for ( i = 1; i <= scon_stk_ptr; ++i )
					scset[scon_stk[i]] =
						mkbranch( scset[scon_stk[i]],
								pat );
				}

			else
				{
				for ( i = 1; i <= lastsc; ++i )
					if ( ! scxclu[i] )
						scset[i] =
							mkbranch( scset[i],
								pat );
				}
			}
#line 1368 "y.tab.c"
break;
case 25:
  if (!yytrial)
#line 289 "..\flex-2.5.4\parse.y"
{
			if ( scon_stk_ptr > 0 )
				build_eof_action();
	
			else
				{
				/* This EOF applies to all start conditions
			 * which don't already have EOF actions.
			 */
				for ( i = 1; i <= lastsc; ++i )
					if ( ! sceof[i] )
						scon_stk[++scon_stk_ptr] = i;

				if ( scon_stk_ptr == 0 )
					warn(
			"all start conditions already have <<EOF>> rules" );

				else
					build_eof_action();
				}
			}
#line 1394 "y.tab.c"
break;
case 26:
  if (!yytrial)
#line 312 "..\flex-2.5.4\parse.y"
{ synerr( "unrecognized rule" ); }
#line 1400 "y.tab.c"
break;
case 27:
  if (!yytrial)
#line 316 "..\flex-2.5.4\parse.y"
{ yyval = scon_stk_ptr; }
#line 1406 "y.tab.c"
break;
case 28:
  if (!yytrial)
#line 320 "..\flex-2.5.4\parse.y"
{ yyval = yyvsp[-2]; }
#line 1412 "y.tab.c"
break;
case 29:
  if (!yytrial)
#line 323 "..\flex-2.5.4\parse.y"
{
			yyval = scon_stk_ptr;

			for ( i = 1; i <= lastsc; ++i )
				{
				int j;

				for ( j = 1; j <= scon_stk_ptr; ++j )
					if ( scon_stk[j] == i )
						break;

				if ( j > scon_stk_ptr )
					scon_stk[++scon_stk_ptr] = i;
				}
			}
#line 1432 "y.tab.c"
break;
case 30:
  if (!yytrial)
#line 340 "..\flex-2.5.4\parse.y"
{ yyval = scon_stk_ptr; }
#line 1438 "y.tab.c"
break;
case 33:
  if (!yytrial)
#line 348 "..\flex-2.5.4\parse.y"
{ synerr( "bad start condition list" ); }
#line 1444 "y.tab.c"
break;
case 34:
  if (!yytrial)
#line 352 "..\flex-2.5.4\parse.y"
{
			if ( (scnum = sclookup( nmstr )) == 0 )
				format_pinpoint_message(
					"undeclared start condition %s",
					nmstr );
			else
				{
				for ( i = 1; i <= scon_stk_ptr; ++i )
					if ( scon_stk[i] == scnum )
						{
						format_warn(
							"<%s> specified twice",
							scname[scnum] );
						break;
						}

				if ( i > scon_stk_ptr )
					scon_stk[++scon_stk_ptr] = scnum;
				}
			}
#line 1469 "y.tab.c"
break;
case 35:
  if (!yytrial)
#line 375 "..\flex-2.5.4\parse.y"
{
			if ( transchar[lastst[yyvsp[0]]] != SYM_EPSILON )
				/* Provide final transition \now/ so it
			 * will be marked as a trailing context
			 * state.
			 */
				yyvsp[0] = link_machines( yyvsp[0],
						mkstate( SYM_EPSILON ) );

			mark_beginning_as_normal( yyvsp[0] );
			current_state_type = STATE_NORMAL;

			if ( previous_continued_action )
				{
				/* We need to treat this as variable trailing
			 * context so that the backup does not happen
			 * in the action but before the action switch
			 * statement.  If the backup happens in the
			 * action, then the rules "falling into" this
			 * one's action will *also* do the backup,
			 * erroneously.
			 */
				if ( ! varlength || headcnt != 0 )
					warn(
		"trailing context made variable due to preceding '|' action" );

				/* Mark as variable. */
				varlength = true;
				headcnt = 0;
				}

			if ( lex_compat || (varlength && headcnt == 0) )
				{ /* variable trailing context rule */
				/* Mark the first part of the rule as the
			 * accepting "head" part of a trailing
			 * context rule.
			 *
			 * By the way, we didn't do this at the
			 * beginning of this production because back
			 * then current_state_type was set up for a
			 * trail rule, and add_accept() can create
			 * a new state ...
			 */
				add_accept( yyvsp[-1],
					num_rules | YY_TRAILING_HEAD_MASK );
				variable_trail_rule = true;
				}
			
			else
				trailcnt = rulelen;

			yyval = link_machines( yyvsp[-1], yyvsp[0] );
			}
#line 1527 "y.tab.c"
break;
case 36:
  if (!yytrial)
#line 430 "..\flex-2.5.4\parse.y"
{ synerr( "trailing context used twice" ); }
#line 1533 "y.tab.c"
break;
case 37:
  if (!yytrial)
#line 433 "..\flex-2.5.4\parse.y"
{
			headcnt = 0;
			trailcnt = 1;
			rulelen = 1;
			varlength = false;

			current_state_type = STATE_TRAILING_CONTEXT;

			if ( trlcontxt )
				{
				synerr( "trailing context used twice" );
				yyval = mkstate( SYM_EPSILON );
				}

			else if ( previous_continued_action )
				{
				/* See the comment in the rule for "re2 re"
			 * above.
			 */
				warn(
		"trailing context made variable due to preceding '|' action" );

				varlength = true;
				}

			if ( lex_compat || varlength )
				{
				/* Again, see the comment in the rule for
			 * "re2 re" above.
			 */
				add_accept( yyvsp[-1],
					num_rules | YY_TRAILING_HEAD_MASK );
				variable_trail_rule = true;
				}

			trlcontxt = true;

			eps = mkstate( SYM_EPSILON );
			yyval = link_machines( yyvsp[-1],
				link_machines( eps, mkstate( '\n' ) ) );
			}
#line 1579 "y.tab.c"
break;
case 38:
  if (!yytrial)
#line 476 "..\flex-2.5.4\parse.y"
{
			yyval = yyvsp[0];

			if ( trlcontxt )
				{
				if ( lex_compat || (varlength && headcnt == 0) )
					/* Both head and trail are
				 * variable-length.
				 */
					variable_trail_rule = true;
				else
					trailcnt = rulelen;
				}
			}
#line 1598 "y.tab.c"
break;
case 39:
  if (!yytrial)
#line 494 "..\flex-2.5.4\parse.y"
{
			varlength = true;
			yyval = mkor( yyvsp[-2], yyvsp[0] );
			}
#line 1607 "y.tab.c"
break;
case 40:
  if (!yytrial)
#line 500 "..\flex-2.5.4\parse.y"
{ yyval = yyvsp[0]; }
#line 1613 "y.tab.c"
break;
case 41:
  if (!yytrial)
#line 505 "..\flex-2.5.4\parse.y"
{
			/* This rule is written separately so the
		 * reduction will occur before the trailing
		 * series is parsed.
		 */

			if ( trlcontxt )
				synerr( "trailing context used twice" );
			else
				trlcontxt = true;

			if ( varlength )
				/* We hope the trailing context is
			 * fixed-length.
			 */
				varlength = false;
			else
				headcnt = rulelen;

			rulelen = 0;

			current_state_type = STATE_TRAILING_CONTEXT;
			yyval = yyvsp[-1];
			}
#line 1642 "y.tab.c"
break;
case 42:
  if (!yytrial)
#line 532 "..\flex-2.5.4\parse.y"
{
			/* This is where concatenation of adjacent patterns
		 * gets done.
		 */
			yyval = link_machines( yyvsp[-1], yyvsp[0] );
			}
#line 1653 "y.tab.c"
break;
case 43:
  if (!yytrial)
#line 540 "..\flex-2.5.4\parse.y"
{ yyval = yyvsp[0]; }
#line 1659 "y.tab.c"
break;
case 44:
  if (!yytrial)
#line 544 "..\flex-2.5.4\parse.y"
{
			varlength = true;

			yyval = mkclos( yyvsp[-1] );
			}
#line 1669 "y.tab.c"
break;
case 45:
  if (!yytrial)
#line 551 "..\flex-2.5.4\parse.y"
{
			varlength = true;
			yyval = mkposcl( yyvsp[-1] );
			}
#line 1678 "y.tab.c"
break;
case 46:
  if (!yytrial)
#line 557 "..\flex-2.5.4\parse.y"
{
			varlength = true;
			yyval = mkopt( yyvsp[-1] );
			}
#line 1687 "y.tab.c"
break;
case 47:
  if (!yytrial)
#line 563 "..\flex-2.5.4\parse.y"
{
			varlength = true;

			if ( yyvsp[-3] > yyvsp[-1] || yyvsp[-3] < 0 )
				{
				synerr( "bad iteration values" );
				yyval = yyvsp[-5];
				}
			else
				{
				if ( yyvsp[-3] == 0 )
					{
					if ( yyvsp[-1] <= 0 )
						{
						synerr(
						"bad iteration values" );
						yyval = yyvsp[-5];
						}
					else
						yyval = mkopt(
							mkrep( yyvsp[-5], 1, yyvsp[-1] ) );
					}
				else
					yyval = mkrep( yyvsp[-5], yyvsp[-3], yyvsp[-1] );
				}
			}
#line 1718 "y.tab.c"
break;
case 48:
  if (!yytrial)
#line 591 "..\flex-2.5.4\parse.y"
{
			varlength = true;

			if ( yyvsp[-2] <= 0 )
				{
				synerr( "iteration value must be positive" );
				yyval = yyvsp[-4];
				}

			else
				yyval = mkrep( yyvsp[-4], yyvsp[-2], INFINITY );
			}
#line 1735 "y.tab.c"
break;
case 49:
  if (!yytrial)
#line 605 "..\flex-2.5.4\parse.y"
{
			/* The singleton could be something like "(foo)",
		 * in which case we have no idea what its length
		 * is, so we punt here.
		 */
			varlength = true;

			if ( yyvsp[-1] <= 0 )
				{
				synerr( "iteration value must be positive" );
				yyval = yyvsp[-3];
				}

			else
				yyval = link_machines( yyvsp[-3],
						copysingl( yyvsp[-3], yyvsp[-1] - 1 ) );
			}
#line 1757 "y.tab.c"
break;
case 50:
  if (!yytrial)
#line 624 "..\flex-2.5.4\parse.y"
{
			if ( ! madeany )
				{
				/* Create the '.' character class. */
				anyccl = cclinit();
				ccladd( anyccl, '\n' );
				cclnegate( anyccl );

				if ( useecs )
					mkeccl( ccltbl + cclmap[anyccl],
						ccllen[anyccl], nextecm,
						ecgroup, csize, csize );

				madeany = true;
				}

			++rulelen;

			yyval = mkstate( -anyccl );
			}
#line 1782 "y.tab.c"
break;
case 51:
  if (!yytrial)
#line 646 "..\flex-2.5.4\parse.y"
{
			if ( ! cclsorted )
				/* Sort characters for fast searching.  We
			 * use a shell sort since this list could
			 * be large.
			 */
				cshell( ccltbl + cclmap[yyvsp[0]], ccllen[yyvsp[0]], true );

			if ( useecs )
				mkeccl( ccltbl + cclmap[yyvsp[0]], ccllen[yyvsp[0]],
					nextecm, ecgroup, csize, csize );

			++rulelen;

			yyval = mkstate( -yyvsp[0] );
			}
#line 1803 "y.tab.c"
break;
case 52:
  if (!yytrial)
#line 664 "..\flex-2.5.4\parse.y"
{
			++rulelen;

			yyval = mkstate( -yyvsp[0] );
			}
#line 1813 "y.tab.c"
break;
case 53:
  if (!yytrial)
#line 671 "..\flex-2.5.4\parse.y"
{ yyval = yyvsp[-1]; }
#line 1819 "y.tab.c"
break;
case 54:
  if (!yytrial)
#line 674 "..\flex-2.5.4\parse.y"
{ yyval = yyvsp[-1]; }
#line 1825 "y.tab.c"
break;
case 55:
  if (!yytrial)
#line 677 "..\flex-2.5.4\parse.y"
{
			++rulelen;

			if ( caseins && yyvsp[0] >= 'A' && yyvsp[0] <= 'Z' )
				yyvsp[0] = clower( yyvsp[0] );

			yyval = mkstate( yyvsp[0] );
			}
#line 1838 "y.tab.c"
break;
case 56:
  if (!yytrial)
#line 688 "..\flex-2.5.4\parse.y"
{ yyval = yyvsp[-1]; }
#line 1844 "y.tab.c"
break;
case 57:
  if (!yytrial)
#line 691 "..\flex-2.5.4\parse.y"
{
			cclnegate( yyvsp[-1] );
			yyval = yyvsp[-1];
			}
#line 1853 "y.tab.c"
break;
case 58:
  if (!yytrial)
#line 698 "..\flex-2.5.4\parse.y"
{
			if ( caseins )
				{
				if ( yyvsp[-2] >= 'A' && yyvsp[-2] <= 'Z' )
					yyvsp[-2] = clower( yyvsp[-2] );
				if ( yyvsp[0] >= 'A' && yyvsp[0] <= 'Z' )
					yyvsp[0] = clower( yyvsp[0] );
				}

			if ( yyvsp[-2] > yyvsp[0] )
				synerr( "negative range in character class" );

			else
				{
				for ( i = yyvsp[-2]; i <= yyvsp[0]; ++i )
					ccladd( yyvsp[-3], i );

				/* Keep track if this ccl is staying in
			 * alphabetical order.
			 */
				cclsorted = cclsorted && (yyvsp[-2] > lastchar);
				lastchar = yyvsp[0];
				}

			yyval = yyvsp[-3];
			}
#line 1884 "y.tab.c"
break;
case 59:
  if (!yytrial)
#line 726 "..\flex-2.5.4\parse.y"
{
			if ( caseins && yyvsp[0] >= 'A' && yyvsp[0] <= 'Z' )
				yyvsp[0] = clower( yyvsp[0] );

			ccladd( yyvsp[-1], yyvsp[0] );
			cclsorted = cclsorted && (yyvsp[0] > lastchar);
			lastchar = yyvsp[0];
			yyval = yyvsp[-1];
			}
#line 1898 "y.tab.c"
break;
case 60:
  if (!yytrial)
#line 737 "..\flex-2.5.4\parse.y"
{
			/* Too hard to properly maintain cclsorted. */
			cclsorted = false;
			yyval = yyvsp[-1];
			}
#line 1908 "y.tab.c"
break;
case 61:
  if (!yytrial)
#line 744 "..\flex-2.5.4\parse.y"
{
			cclsorted = true;
			lastchar = 0;
			currccl = yyval = cclinit();
			}
#line 1918 "y.tab.c"
break;
case 62:
  if (!yytrial)
#line 751 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(isalnum) }
#line 1924 "y.tab.c"
break;
case 63:
  if (!yytrial)
#line 752 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(isalpha) }
#line 1930 "y.tab.c"
break;
case 64:
  if (!yytrial)
#line 753 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(IS_BLANK) }
#line 1936 "y.tab.c"
break;
case 65:
  if (!yytrial)
#line 754 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(iscntrl) }
#line 1942 "y.tab.c"
break;
case 66:
  if (!yytrial)
#line 755 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(isdigit) }
#line 1948 "y.tab.c"
break;
case 67:
  if (!yytrial)
#line 756 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(isgraph) }
#line 1954 "y.tab.c"
break;
case 68:
  if (!yytrial)
#line 757 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(islower) }
#line 1960 "y.tab.c"
break;
case 69:
  if (!yytrial)
#line 758 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(isprint) }
#line 1966 "y.tab.c"
break;
case 70:
  if (!yytrial)
#line 759 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(ispunct) }
#line 1972 "y.tab.c"
break;
case 71:
  if (!yytrial)
#line 760 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(isspace) }
#line 1978 "y.tab.c"
break;
case 72:
  if (!yytrial)
#line 761 "..\flex-2.5.4\parse.y"
{
				if ( caseins )
					CCL_EXPR(islower)
				else
					CCL_EXPR(isupper)
				}
#line 1989 "y.tab.c"
break;
case 73:
  if (!yytrial)
#line 767 "..\flex-2.5.4\parse.y"
{ CCL_EXPR(isxdigit) }
#line 1995 "y.tab.c"
break;
case 74:
  if (!yytrial)
#line 771 "..\flex-2.5.4\parse.y"
{
			if ( caseins && yyvsp[0] >= 'A' && yyvsp[0] <= 'Z' )
				yyvsp[0] = clower( yyvsp[0] );

			++rulelen;

			yyval = link_machines( yyvsp[-1], mkstate( yyvsp[0] ) );
			}
#line 2008 "y.tab.c"
break;
case 75:
  if (!yytrial)
#line 781 "..\flex-2.5.4\parse.y"
{ yyval = mkstate( SYM_EPSILON ); }
#line 2014 "y.tab.c"
break;
#line 2016 "y.tab.c"
#line 664 "btyaccpa.ske"

  default:
    break;
  }

#if YYDEBUG && defined(YYDBPR)
  if (yydebug)
  {
    printf("yydebug[%d]: after reduction, result is ", yytrial);
    YYDBPR(yyps->val);
    printf("\\n");
  }
#endif

  // Perform user-defined position reduction
#ifdef YYREDUCEPOSNFUNC
  if (!yytrial)
  {
    YYCALLREDUCEPOSN(YYREDUCEPOSNFUNCARG);
  }
#endif

  yyps->ssp -= yym;
  yystate = *(yyps->ssp);
  yyps->vsp -= yym;
  yyps->psp -= yym;

  yym = yylhs[yyn];
  if (yystate == 0 && yym == 0)
  {
#if YYDEBUG
    if (yydebug)
    {
      printf("yydebug[%d,%d]: after reduction, shifting from state 0 to state %d\\n",
             yydepth, (int)yytrial, YYFINAL);
    }
#endif
    yystate = YYFINAL;
    *++(yyps->ssp) = YYFINAL;
    *++(yyps->vsp) = yyps->val;
    yyretlval = yyps->val;  // return value of root non-terminal to yylval
    *++(yyps->psp) = yyps->pos;
    yyretposn = yyps->pos;  // return value of root position to yyposn
    if (yychar < 0)
    {
      if ((yychar = YYLex1()) < 0)
      {
        yychar = 0;
      }
#if YYDEBUG
      if (yydebug)
      {
        yys = 0;
        if (yychar <= YYMAXTOKEN)
          yys = yyname[yychar];
        if (!yys)
          yys = "illegal-symbol";
        printf("yydebug[%d,%d]: state %d, reading %d (%s)\\n",
               yydepth, (int)yytrial, YYFINAL, yychar, yys);
      }
#endif
    }
    if (yychar == 0)
      goto yyaccept;
    goto yyloop;
  }

  if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
      yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
  {
    yystate = yytable[yyn];
  }
  else
  {
    yystate = yydgoto[yym];
  }
#if YYDEBUG
  if (yydebug)
    printf("yydebug[%d,%d]: after reduction, shifting from state %d to state %d\\n",
           yydepth, (int)yytrial, *(yyps->ssp), yystate);
#endif
  if (yyps->ssp >= yyps->ss + yyps->stacksize - 1)
  {
    YYMoreStack(yyps);
  }
  *++(yyps->ssp) = yystate;
  *++(yyps->vsp) = yyps->val;
  *++(yyps->psp) = yyps->pos;
  goto yyloop;


  //
  // Reduction declares that this path is valid.
  // Set yypath and do a full parse
  //
yyvalid:
  if (yypath)
  {
    goto yyabort;
  }
  while (yyps->save)
  {
    struct yyparsestate *save = yyps->save;
    yyps->save = save->save;
    save->save = yypath;
    yypath = save;
  }
#if YYDEBUG
  if (yydebug)
    printf("yydebug[%d,%d]: CONFLICT trial successful, backtracking to state %d, %d tokens\\n",
           yydepth, (int)yytrial, yypath->state, yylvp - yylvals - yypath->lexeme);
#endif
  if(yyerrctx)
  {
    YYFreeState(yyerrctx);
    yyerrctx = NULL;
  }
  yychar = -1;
  yyps->ssp = yyps->ss + (yypath->ssp - yypath->ss);
  yyps->vsp = yyps->vs + (yypath->vsp - yypath->vs);
  yyps->psp = yyps->ps + (yypath->psp - yypath->ps);
  memcpy (yyps->ss, yypath->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));
  YYSCopy(yyps->vs, yypath->vs,  yyps->vsp - yyps->vs + 1);
  YYPCopy(yyps->ps, yypath->ps,  yyps->psp - yyps->ps + 1);
  yylvp = yylvals + yypath->lexeme;
  yylpp = yylpsns + yypath->lexeme;
  yylexp = yylexemes + yypath->lexeme;
  yystate = yypath->state;
  goto yyloop;


yyabort:
  if(yyerrctx)
  {
    YYFreeState(yyerrctx);
    yyerrctx = NULL;
  }

  {
    YYSTYPE *pv;
    for(pv=yyps->vs; pv<yyps->vsp; pv++)
    {
      YYDELETEVAL(*pv,2);
    }
  }
  {
    YYPOSN *pp;
    for(pp=yyps->ps; pp<yyps->psp; pp++)
    {
      YYDELETEPOSN(*pp,2);
    }
  }
  while (yyps)
  {
    struct yyparsestate *save = yyps;
    yyps = save->save;
    YYFreeState(save);
  }
  while (yypath)
  {
    struct yyparsestate *save = yypath;
    yypath = save->save;
    YYFreeState(save);
  }
  return (1);


yyaccept:
  if (yyps->save)
    goto yyvalid;
  if(yyerrctx)
  {
    YYFreeState(yyerrctx);
    yyerrctx = NULL;
  }
  while (yyps)
  {
    struct yyparsestate *save = yyps;
    yyps = save->save;
    YYFreeState(save);
  }
  while (yypath)
  {
    struct yyparsestate *save = yypath;
    yypath = save->save;
    YYFreeState(save);
  }
  return (0);
}


int YYLex1()
{
  if(yylvp<yylve)
  {
    yylval = *yylvp++;
    yyposn = *yylpp++;
    return *yylexp++;
  }
  else
  {
    if(yyps->save)
    {
      if(yylvp==yylvlim)
      {
        yyexpand();
      }
      *yylexp = yylex();
      *yylvp++ = yylval;
      yylve++;
      *yylpp++ = yyposn;
      yylpe++;
      return *yylexp++;
    }
    else
    {
      return yylex();
    }
  }
}

int yyexpand()
{
  int p = yylvp-yylvals;
  int s = yylvlim-yylvals;
  s += YYSTACKGROWTH;
  {
    Yshort  *tl = yylexemes;
    YYSTYPE *tv = yylvals;
    YYPOSN  *tp = yylpsns;
#ifdef __cplusplus
    yylvals = new YYSTYPE[s];
    yylpsns = new YYPOSN[s];
    yylexemes = new Yshort[s];
#else
    yylvals = malloc(sizeof(YYSTYPE)*s);
    yylpsns = malloc(sizeof(YYPOSN)*s);
    yylexemes = malloc(sizeof(Yshort)*s);
#endif
    memcpy(yylexemes, tl, (s-YYSTACKGROWTH)*sizeof(Yshort));
    YYSCopy(yylvals, tv, s-YYSTACKGROWTH);
    YYPCopy(yylpsns, tp, s-YYSTACKGROWTH);
#ifdef __cplusplus
    delete[] tl;
    delete[] tv;
    delete[] tp;
#else
    free(tl);
    free(tv);
    free(tp);
#endif
  }
  yylvp = yylve = yylvals + p;
  yylvlim = yylvals + s;
  yylpp = yylpe = yylpsns + p;
  yylplim = yylpsns + s;
  yylexp = yylexemes + p;
  return 0;
}

void YYSCopy(YYSTYPE *to, YYSTYPE *from, int size)
{
  int i;
  for (i = size-1; i >= 0; i--)
  {
    to[i] = from[i];
  }
}

void YYPCopy(YYPOSN *to, YYPOSN *from, int size)
{
  int i;
  for (i = size-1; i >= 0; i--)
  {
    to[i] = from[i];
  }
}

void YYMoreStack(struct yyparsestate *yyps)
{
  int p = yyps->ssp - yyps->ss;
  Yshort  *tss = yyps->ss;
  YYSTYPE *tvs = yyps->vs;
  YYPOSN  *tps = yyps->ps;
#ifdef __cplusplus
  yyps->ss = new Yshort [yyps->stacksize + YYSTACKGROWTH];
  yyps->vs = new YYSTYPE[yyps->stacksize + YYSTACKGROWTH];
  yyps->ps = new YYPOSN [yyps->stacksize + YYSTACKGROWTH];
#else
  yyps->ss = malloc(sizeof(Yshort)*(yyps->stacksize + YYSTACKGROWTH));
  yyps->vs = malloc(sizeof(YYSTYPE)*(yyps->stacksize + YYSTACKGROWTH));
  yyps->ps = malloc(sizeof(YYPOSN)*(yyps->stacksize + YYSTACKGROWTH));
#endif
  memcpy(yyps->ss, tss, yyps->stacksize * sizeof(Yshort));
  YYSCopy(yyps->vs, tvs, yyps->stacksize);
  YYPCopy(yyps->ps, tps, yyps->stacksize);
  yyps->stacksize += YYSTACKGROWTH;
#ifdef __cplusplus
  delete[] tss;
  delete[] tvs;
  delete[] tps;
#else
  free(tss);
  free(tvs);
  free(tps);
#endif
  yyps->ssp = yyps->ss + p;
  yyps->vsp = yyps->vs + p;
  yyps->psp = yyps->ps + p;
}

struct yyparsestate *YYNewState(int size)
{
  struct yyparsestate *p;
#ifdef __cplusplus
  p = new struct yyparsestate;
  p->ss = new Yshort [size + 4];
  p->vs = new YYSTYPE[size + 4];
  p->ps = new YYPOSN [size + 4];
#else
  p = malloc(sizeof(struct yyparsestate));
  p->ss = malloc(sizeof(Yshort)*(size + 4));
  p->vs = malloc(sizeof(YYSTYPE)*(size + 4));
  p->ps = malloc(sizeof(YYPOSN)*(size + 4));
#endif
  p->stacksize = size+4;
  memset(&p->vs[0], 0, (size+4)*sizeof(YYSTYPE));
  memset(&p->ps[0], 0, (size+4)*sizeof(YYPOSN));
  return p;
}

void YYFreeState(struct yyparsestate *p)
{
#ifdef __cplusplus
  delete[] p->ss;
  delete[] p->vs;
  delete[] p->ps;
  delete p;
#else
  free(p->ss);
  free(p->vs);
  free(p->ps);
  free(p);
#endif
}
