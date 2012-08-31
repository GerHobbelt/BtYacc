/*
** This file generated automatically from 
*/

#include "defs.h"
/*  The banner used here should be replaced with an #ident directive    */
/*  if the target C compiler supports #ident directives.                */
/*                                                                      */
/*  If the skeleton is changed, the banner should be changed so that    */
/*  the altered version can easily be distinguished from the original.  */

char *banner[] =
{
    "#line 8 \"btyaccpa.ske\"",
    "/*",
    "**    \"@(#)btyaccpar, based on byacc 1.8 (Berkeley) 01/20/91\";",
    "*/",
    "#define YYBTYACC 1",
    "",
    "typedef int Yshort;",
    "",
    0
};

char *tables[] =
{
    "#line 16 \"btyaccpa.ske\"",
    "#ifdef __cplusplus",
    "#define _C_ \"C\"",
    "#else",
    "#define _C_ ",
    "#endif",
    "extern _C_ Yshort yylhs[];",
    "extern _C_ Yshort yylen[];",
    "extern _C_ Yshort yydefred[];",
    "extern _C_ Yshort yydgoto[];",
    "extern _C_ Yshort yysindex[];",
    "extern _C_ Yshort yyrindex[];",
    "extern _C_ Yshort yycindex[];",
    "extern _C_ Yshort yygindex[];",
    "extern _C_ Yshort yytable[];",
    "extern _C_ Yshort yycheck[];",
    "extern _C_ Yshort yyctable[];",
    "#if YYDEBUG",
    "extern _C_ char *yyname[];",
    "extern _C_ char *yyrule[];",
    "#endif",
    "",
    0
};

char *header[] =
{
    "#line 38 \"btyaccpa.ske\"",
    "",
    "#define yyclearin (yychar=(-1))",
    "",
    "#define yyerrok (yyerrflag=0)",
    "",
    "#ifndef YYSTACKGROWTH",
    "#define YYSTACKGROWTH 16",
    "#endif",
    "",
    "#ifndef YYDEFSTACKSIZE",
    "#define YYDEFSTACKSIZE 12",
    "#endif",
    "",
    "/*  #ifdef YYSTACKSIZE",
    "    #ifndef YYMAXDEPTH",
    "    #define YYMAXDEPTH YYSTACKSIZE",
    "    #endif",
    "    #else",
    "    #ifdef YYMAXDEPTH",
    "    #define YYSTACKSIZE YYMAXDEPTH",
    "    #else",
    "    #define YYSTACKSIZE 500",
    "    #define YYMAXDEPTH 500",
    "    #endif",
    "    #endif */",
    "",
    "#ifdef YYDEBUG",
    "int yydebug;",
    "#endif",
    "",
    "int yynerrs;",
    "",
    "YYSTYPE yylval;",
    "",
    "struct yyparsestate {",
    "  struct yyparsestate *save;        // Previously saved parser state",
    "  int                  state;",
    "  int                  errflag;",
    "  Yshort              *ssp;         // state stack pointer",
    "  YYSTYPE             *vsp;         // value stack pointer",
    "  YYSTYPE              val;",
    "  Yshort              *ss;          // state stack base",
    "  YYSTYPE             *vs;          // values stack base",
    "  int                  lexeme;      // index of the conflict lexeme in the lexical queue",
    "  unsigned int         stacksize;   // current maximum stack size",
    "  Yshort               ctry;        // index in yyctable[] for this conflict",
    "};",
    "",
    "// Current parser state",
    "static struct yyparsestate *yyps=0;",
    "",
    "// yypath!=NULL: do the full parse, starting at *yypath parser state.",
    "static struct yyparsestate *yypath=0;",
    "",
    "// Base of the lexical value queue",
    "static YYSTYPE *yylvals=0;",
    "",
    "// Current posistion at lexical value queue",
    "static YYSTYPE *yylvp=0;",
    "",
    "// End position of lexical value queue",
    "static YYSTYPE *yylve=0;",
    "",
    "// The last allocated position at the lexical value queue",
    "static YYSTYPE *yylvlim=0;",
    "",
    "// Current position at lexical token queue",
    "static Yshort *yylexp=0;",
    "",
    "static Yshort *yylexemes=0;",
    "",
    "#define yytrial (yyps->save)",
    "#define yyvsp   (yyps->vsp)",
    "#define yyval   (yyps->val)",
    "",
    "#define YYLEX (yylvp<yylve ? yylval=*yylvp++, *yylexp++ :                    \\",
    "                             yyps->save ? (yylvp==yylvlim ? yyexpand() : 0), \\",
    "                                          *yylexp = yylex(),                 \\",
    "                                          *yylvp++ = yylval, yylve++, *yylexp++ \\",
    "                                        : yylex())",
    "",
    "int yyparse(void);",
    "",
    "",
    "#ifndef __cplusplus",
    "#define YYSCOPY(t, f, s)        memcpy(t, f, (s)*sizeof(YYSTYPE))",
    "",
    "#define YYMORESTACK             \\",
    "  do {                          \\",
    "    int p = (yyps->ssp) - (yyps->ss);                           \\",
    "    yyps->stacksize += YYSTACKGROWTH;                                   \\",
    "    yyps->ss = (Yshort *)realloc(yyps->ss,  yyps->stacksize * sizeof(Yshort)); \\",
    "    yyps->vs = (YYSTYPE *)realloc(yyps->vs, yyps->stacksize * sizeof(YYSTYPE)); \\",
    "    yyps->ssp = (yyps->ss) + p;                                 \\",
    "    yyps->vsp = (yyps->vs) + p;                                 \\",
    "  } while (0)",
    "",
    "#else  /* C++ */",
    "",
    "#define YYSCOPY(to, from, size)         \\",
    "  do {                                  \\",
    "    int _i;                             \\",
    "    for (_i = (size)-1; _i >= 0; _i--)  \\",
    "      (to)[_i] = (from)[_i];            \\",
    "  } while(0)",
    "",
    "#define YYMORESTACK                                             \\",
    "  do {                                                          \\",
    "    int p = yyps->ssp - yyps->ss;                               \\",
    "    Yshort *tss = yyps->ss; YYSTYPE *tvs = (yyps->vs);          \\",
    "    yyps->ss = new Yshort[(yyps->stacksize) + YYSTACKGROWTH];   \\",
    "    yyps->vs = new YYSTYPE[(yyps->stacksize) + YYSTACKGROWTH];  \\",
    "    memcpy(yyps->ss, tss, (yyps->stacksize) * sizeof(Yshort));  \\",
    "    YYSCOPY(yyps->vs, tvs, (yyps->stacksize));                  \\",
    "    yyps->stacksize += YYSTACKGROWTH;                           \\",
    "    delete[] tss;                                               \\",
    "    delete[] tvs;                                               \\",
    "    yyps->ssp = yyps->ss + p;                                   \\",
    "    yyps->vsp = yyps->vs + p;                                   \\",
    "  } while (0)",
    "#endif /* C++ */",
    "",
    "",
    0
};

char *body[] =
{
    "#line 162 \"btyaccpa.ske\"",
    "",
    "",
    "#ifndef YYNEWSTATE",
    "#ifdef __oldc",
    "struct yyparsestate *YYNEWSTATE(size) {",
    "int size;",
    "#else",
    "struct yyparsestate *YYNEWSTATE(int size) {",
    "#endif /* __oldc */",
    "  struct yyparsestate *p;",
    "",
    "#ifndef __cplusplus",
    "  p = (struct yyparsestate *)malloc(sizeof(struct yyparsestate));",
    "  p->stacksize = size+4;",
    "  p->ss = (Yshort *)malloc((size+4)*sizeof(Yshort));",
    "  p->vs = (YYSTYPE *)malloc((size+4)*sizeof(YYSTYPE));",
    "#else /* C++ */",
    "  p = new yyparsestate;",
    "  p->stacksize = size+4;",
    "  p->ss = new Yshort[size + 4];",
    "  p->vs = new YYSTYPE[size + 4];",
    "#endif /* C++ */",
    "  { unsigned i;",
    "    for(i=0; i<(size+4)*sizeof(YYSTYPE); i++) {",
    "      ((char*)p->vs)[i] = 0;",
    "    }",
    "  }",
    "  return p;",
    "}",
    "#endif /* YYNEWSTATE */",
    "",
    "",
    "#ifndef YYFREESTATE",
    "#ifndef __cplusplus",
    "#define YYFREESTATE(p) (free((p)->ss), free((p)->vs), free(p))",
    "#else /* C++ */",
    "#define YYFREESTATE(p) (delete[] (p)->ss, delete[] (p)->vs, delete (p))",
    "#endif /* C++ */",
    "#endif /* YYFREESTATE */",
    "",
    "",
    "static int yyexpand() {",
    "  int p = yylvp-yylvals;",
    "  int s = yylvlim-yylvals;",
    "  s += YYSTACKGROWTH;",
    "#ifndef __cplusplus",
    "  yylvals = (YYSTYPE *)realloc(yylvals, s*sizeof(YYSTYPE));",
    "  yylexemes = (Yshort *)realloc(yylexemes, s*sizeof(Yshort));",
    "#else /* C++ */",
    "  { Yshort *tl = yylexemes; YYSTYPE *tv = yylvals;",
    "    yylvals = new YYSTYPE[s];",
    "    yylexemes = new Yshort[s];",
    "    memcpy(yylexemes, tl, (s-YYSTACKGROWTH)*sizeof(Yshort));",
    "    YYSCOPY(yylvals, tv, s-YYSTACKGROWTH);",
    "    delete[] tl;",
    "    delete[] tv; ",
    "  }",
    "#endif /* C++ */",
    "  yylvp = yylve = yylvals + p;",
    "  yylvlim = yylvals + s;",
    "  yylexp = yylexemes + p;",
    "  return 0;",
    "}",
    "",
    "#define YYABORT  goto yyabort",
    "#define YYACCEPT goto yyaccept",
    "#define YYERROR  goto yyerrlab",
    "#define YYVALID         do { if (yyps->save)                        goto yyvalid; } while(0)",
    "#define YYVALID_NESTED  do { if (yyps->save && yyps->save->save==0) goto yyvalid; } while(0)",
    "",
    "#ifdef __cplusplus",
    "#include <stdlib.h>",
    "#else",
    "extern char *getenv();",
    "#endif",
    "",
    "",
    "//",
    "// Parser function",
    "//",
    "int yyparse() {",
    "  int yym, yyn, yystate, yychar, yynewerrflag;",
    "  struct yyparsestate *yyerrctx = NULL;",
    "",
    "#if YYDEBUG",
    "  char *yys;",
    "  ",
    "  if ((yys = getenv(\"YYDEBUG\"))) {",
    "    yyn = *yys;",
    "    if (yyn >= '0' && yyn <= '9')",
    "      yydebug = yyn - '0'; ",
    "  }",
    "#endif",
    "  ",
    "  yyps = YYNEWSTATE(YYDEFSTACKSIZE);",
    "  yyps->save = 0;",
    "  yynerrs = 0;",
    "  yyps->errflag = 0;",
    "  yychar = (-1);",
    "  ",
    "  yyps->ssp = yyps->ss;",
    "  yyps->vsp = yyps->vs;",
    "  *(yyps->ssp) = yystate = 0;",
    "  ",
    "",
    "  //",
    "  // Main parsing loop",
    "  //",
    " yyloop:",
    "  if ((yyn = yydefred[yystate])) {",
    "    goto yyreduce;",
    "  }",
    "",
    "  //",
    "  // Read one token",
    "  //",
    "  if (yychar < 0) {",
    "    if ((yychar = YYLEX) < 0) yychar = 0;",
    "#if YYDEBUG",
    "    if (yydebug) {",
    "      yys = 0;",
    "      if (yychar <= YYMAXTOKEN) yys = yyname[yychar];",
    "      if (!yys) yys = \"illegal-symbol\";",
    "      printf(\"yydebug: state %d, reading %d (%s)\", yystate, yychar, yys);",
    "#ifdef YYDBPR",
    "      printf(\"<\");",
    "      YYDBPR(yylval);",
    "      printf(\">\");",
    "#endif",
    "      printf(\"\\n\"); ",
    "    }",
    "#endif",
    "  }",
    "",
    "  //",
    "  // Do we have a conflict?",
    "  //",
    "  if ((yyn = yycindex[yystate]) &&",
    "      (yyn += yychar) >= 0 &&",
    "      yyn <= YYTABLESIZE &&",
    "      yycheck[yyn] == yychar) {",
    "    int ctry;",
    "",
    "    if (yypath) {",
    "#if YYDEBUG",
    "      if (yydebug) {",
    "        printf(\"yydebug: CONFLICT in state %d: following successful trial parse\\n\", yystate);",
    "      }",
    "#endif",
    "      // Switch to the next conflict context",
    "      struct yyparsestate *save = yypath;",
    "      yypath = save->save;",
    "      ctry = save->ctry;",
    "      if (save->state != yystate) ",
    "        goto yyabort;",
    "      YYFREESTATE(save); ",
    "",
    "    } else {",
    "",
    "#if YYDEBUG",
    "      if (yydebug) {",
    "        printf(\"yydebug: CONFLICT in state %d. \", yystate);",
    "        if(yyps->save) {",
    "          printf(\"ALREADY in conflict. Continue trial parse.\");",
    "        } else {",
    "          printf(\"Start trial parse.\");",
    "        }",
    "        printf(\"\\n\");",
    "      }",
    "      if(yyps->save) {",
    "        void yyerror_detailed(char *text, int err_token, YYSTYPE err_token_value);",
    "      }",
    "#endif",
    "      struct yyparsestate *save = YYNEWSTATE(yyps->ssp - yyps->ss);",
    "      save->save    = yyps->save;",
    "      save->state   = yystate;",
    "      save->errflag = yyps->errflag;",
    "      save->ssp     = save->ss + (yyps->ssp - yyps->ss);",
    "      save->vsp     = save->vs + (yyps->vsp - yyps->vs);",
    "      memcpy (save->ss, yyps->ss, (yyps->ssp - yyps->ss + 1)*sizeof(Yshort));",
    "      YYSCOPY(save->vs, yyps->vs, (yyps->ssp - yyps->ss + 1));",
    "      ctry = yytable[yyn];",
    "      if (yyctable[ctry] == -1) {",
    "#if YYDEBUG",
    "        if (yydebug && yychar >= 0)",
    "          printf(\"yydebug: backtracking 1 token\\n\");",
    "#endif",
    "        ctry++; ",
    "      }",
    "      save->ctry = ctry;",
    "      if (!yyps->save) {",
    "        // If this is a first conflict in the stack, start saving lexemes",
    "        if (!yylexemes) {",
    "#ifndef __cplusplus",
    "          yylexemes = (Yshort *)malloc(YYSTACKGROWTH*sizeof(Yshort));",
    "          yylvals = (YYSTYPE *)malloc(YYSTACKGROWTH*sizeof(YYSTYPE));",
    "#else  /* C++ */",
    "          yylexemes = new Yshort[YYSTACKGROWTH];",
    "          yylvals = new YYSTYPE[YYSTACKGROWTH];",
    "#endif /* C++ */",
    "          yylvlim = yylvals + YYSTACKGROWTH; ",
    "        }",
    "        if (yylvp == yylve) {",
    "          yylvp = yylve = yylvals;",
    "          yylexp = yylexemes;",
    "          if (yychar >= 0) {",
    "            *yylve++ = yylval;",
    "            *yylexp = yychar;",
    "            yychar = -1; ",
    "          } ",
    "        } ",
    "      }",
    "      if (yychar >= 0) {",
    "        yylvp--, yylexp--;",
    "        yychar = -1; ",
    "      }",
    "      save->lexeme = yylvp - yylvals;",
    "      yyps->save = save; ",
    "    }",
    "    if (yytable[yyn] == ctry) {",
    "#if YYDEBUG",
    "      if (yydebug)",
    "        printf(\"yydebug: state %d, shifting to state %d\\n\",",
    "               yystate, yyctable[ctry]);",
    "#endif",
    "      if (yychar < 0)",
    "        yylvp++, yylexp++;",
    "      yychar = -1;",
    "      if (yyps->errflag > 0) --yyps->errflag;",
    "      yystate = yyctable[ctry];",
    "      goto yyshift; ",
    "    } else {",
    "      yyn = yyctable[ctry];",
    "      goto yyreduce; ",
    "    } ",
    "  }",
    "",
    "  //",
    "  // Is action a shift?",
    "  //",
    "  if ((yyn = yysindex[yystate]) &&",
    "      (yyn += yychar) >= 0 &&",
    "      yyn <= YYTABLESIZE &&",
    "      yycheck[yyn] == yychar) {",
    "#if YYDEBUG",
    "    if (yydebug)",
    "      printf(\"yydebug: state %d, shifting to state %d\\n\",",
    "             yystate, yytable[yyn]);",
    "#endif",
    "    yychar = (-1);",
    "    if (yyps->errflag > 0)  --yyps->errflag;",
    "    yystate = yytable[yyn];",
    "  yyshift:",
    "    if (yyps->ssp >= yyps->ss + yyps->stacksize - 1) {",
    "      YYMORESTACK;",
    "    }",
    "    *++(yyps->ssp) = yystate;",
    "    *++(yyps->vsp) = yylval;",
    "    goto yyloop;",
    "  }",
    "  if ((yyn = yyrindex[yystate]) &&",
    "      (yyn += yychar) >= 0 &&",
    "      yyn <= YYTABLESIZE &&",
    "      yycheck[yyn] == yychar) {",
    "    yyn = yytable[yyn];",
    "    goto yyreduce;",
    "  }",
    "",
    "  //",
    "  // Action: error",
    "  //",
    "  if (yyps->errflag) goto yyinrecovery;",
    "  yynewerrflag = 1;",
    "  goto yyerrhandler;",
    "yyerrlab:",
    "  yynewerrflag = 0;",
    "yyerrhandler:",
    "  while (yyps->save) { ",
    "    int ctry; ",
    "    struct yyparsestate *save = yyps->save;",
    "#if YYDEBUG",
    "    if (yydebug)",
    "      printf(\"yydebug: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\\n\",",
    "             yystate, yyps->save->state, yylvp - yylvals - yyps->save->lexeme);",
    "#endif",
    "    // Memorize most forward-looking error state in case",
    "    // it's really an error.",
    "    if(yyerrctx==NULL || yyerrctx->lexeme<yylvp-yylvals) {",
    "      // Free old saved error context state",
    "      if(yyerrctx) YYFREESTATE(yyerrctx);",
    "      // Create and fill out new saved error context state",
    "      yyerrctx = YYNEWSTATE(yyps->ssp - yyps->ss);",
    "      yyerrctx->save = yyps->save;",
    "      yyerrctx->state = yystate;",
    "      yyerrctx->errflag = yyps->errflag;",
    "      yyerrctx->ssp = yyerrctx->ss + (yyps->ssp - yyps->ss);",
    "      yyerrctx->vsp = yyerrctx->vs + (yyps->vsp - yyps->vs);",
    "      memcpy (yyerrctx->ss, yyps->ss, (yyps->ssp - yyps->ss + 1)*sizeof(Yshort));",
    "      YYSCOPY(yyerrctx->vs, yyps->vs, (yyps->ssp - yyps->ss + 1));",
    "      yyerrctx->lexeme = yylvp - yylvals;",
    "    }",
    "    yylvp  = yylvals   + save->lexeme;",
    "    yylexp = yylexemes + save->lexeme;",
    "    yychar = -1;",
    "    yyps->ssp = yyps->ss + (save->ssp - save->ss);",
    "    yyps->vsp = yyps->vs + (save->vsp - save->vs);",
    "    memcpy (yyps->ss, save->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));",
    "    YYSCOPY(yyps->vs, save->vs,  yyps->vsp - yyps->vs + 1);",
    "    ctry = ++save->ctry;",
    "    yystate = save->state;",
    "    // We tried shift, try reduce now",
    "    if ((yyn = yyctable[ctry]) >= 0) {",
    "      goto yyreduce;",
    "    }",
    "    yyps->save = save->save;",
    "    YYFREESTATE(save);",
    "    //",
    "    // Nothing left on the stack -- error",
    "    //",
    "    if (!yyps->save) {",
    "#if YYDEBUG",
    "      if (yydebug) {",
    "        printf(\"yydebug: trial parse FAILED, entering ERROR mode\\n\");",
    "      }",
    "#endif",
    "      // Restore state as it was in the most forward-advanced error",
    "      yylvp  = yylvals   + yyerrctx->lexeme;",
    "      yylexp = yylexemes + yyerrctx->lexeme;",
    "      yychar = yylexp[-1];",
    "      yylval = yylvp[-1];",
    "      yyps->ssp = yyps->ss + (yyerrctx->ssp - yyerrctx->ss);",
    "      yyps->vsp = yyps->vs + (yyerrctx->vsp - yyerrctx->vs);",
    "      memcpy (yyps->ss, yyerrctx->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));",
    "      YYSCOPY(yyps->vs, yyerrctx->vs,  yyps->vsp - yyps->vs + 1);",
    "      yystate = yyerrctx->state;",
    "      YYFREESTATE(yyerrctx);",
    "      yyerrctx = NULL;",
    "    }",
    "    yynewerrflag = 1; ",
    "  }",
    "  if (yynewerrflag) {",
    "#ifdef YYERROR_DETAILED",
    "    yyerror_detailed(\"syntax error\", yychar, yylval);",
    "#else",
    "    yyerror(\"syntax error\");",
    "#endif",
    "  }",
    "  ++yynerrs;",
    " yyinrecovery:",
    "  if (yyps->errflag < 3) {",
    "    yyps->errflag = 3;",
    "    for (;;) {",
    "      if ((yyn = yysindex[*(yyps->ssp)]) && (yyn += YYERRCODE) >= 0 &&",
    "          yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE) {",
    "#if YYDEBUG",
    "        if (yydebug)",
    "          printf(\"yydebug: state %d, ERROR recovery %s state %d\\n\",",
    "                 *(yyps->ssp), \"shifting to\", yytable[yyn]);",
    "#endif",
    "        /* Use label yyerrlab, so that compiler does not warn */",
    "        if(yyps->errflag != yyps->errflag) goto yyerrlab;",
    "        yystate = yytable[yyn];",
    "        goto yyshift; ",
    "      } else {",
    "#if YYDEBUG",
    "        if (yydebug)",
    "          printf(\"yydebug: ERROR recovery discarding state %d\\n\",",
    "                 *(yyps->ssp));",
    "#endif",
    "        if ((yyps->ssp) <= (yyps->ss)) goto yyabort;",
    "        --(yyps->ssp);",
    "        --(yyps->vsp);",
    "      }",
    "    }",
    "  } else {",
    "    if (yychar == 0) goto yyabort;",
    "#if YYDEBUG",
    "    if (yydebug) {",
    "      yys = 0;",
    "      if (yychar <= YYMAXTOKEN) yys = yyname[yychar];",
    "      if (!yys) yys = \"illegal-symbol\";",
    "      printf(\"yydebug: state %d, ERROR recovery discards token %d (%s)\\n\",",
    "             yystate, yychar, yys); ",
    "    }",
    "#endif",
    "    yychar = (-1);",
    "    goto yyloop;",
    "  }",
    "",
    "  //",
    "  // Reduce",
    "  //",
    "yyreduce:",
    "  yym = yylen[yyn];",
    "#if YYDEBUG",
    "  if (yydebug) {",
    "    printf(\"yydebug: state %d, reducing by rule %d (%s)\",",
    "           yystate, yyn, yyrule[yyn]);",
    "#ifdef YYDBPR",
    "    if (yym) {",
    "      int i;",
    "      printf(\"<\");",
    "      for (i=yym; i>0; i--) {",
    "        if (i!=yym) printf(\", \");",
    "        YYDBPR((yyps->vsp)[1-i]);",
    "      }",
    "      printf(\">\");",
    "    }",
    "#endif",
    "    printf(\"\\n\");",
    "  }",
    "#endif",
    "  if (yyps->ssp + 1 - yym >= yyps->ss + yyps->stacksize) {",
    "    YYMORESTACK;",
    "  }",
    "  /* $$ = $1 default action */",
    "  if(yym > 0) {",
    "    yyps->val = (yyps->vsp)[1-yym];",
    "  }",
    "  switch (yyn) {",
    "",
    0
};

char *trailer[] =
{
    "#line 584 \"btyaccpa.ske\"",
    "",
    "  }",
    "",
    "#if YYDEBUG && defined(YYDBPR)",
    "  if (yydebug) {",
    "    printf(\"yydebug: after reduction, result is \");",
    "    YYDBPR(yyps->val);",
    "    printf(\"\\n\");",
    "  }",
    "#endif",
    "  yyps->ssp -= yym;",
    "  yystate = *(yyps->ssp);",
    "  yyps->vsp -= yym;",
    "  yym = yylhs[yyn];",
    "  if (yystate == 0 && yym == 0) {",
    "#if YYDEBUG",
    "    if (yydebug)",
    "      printf(\"yydebug: after reduction, %s from state 0 to state %d\\n\",",
    "             \"shifting\", YYFINAL);",
    "#endif",
    "    yystate = YYFINAL;",
    "    *++(yyps->ssp) = YYFINAL;",
    "    *++(yyps->vsp) = (yyps->val);",
    "    if (yychar < 0) {",
    "      if ((yychar = YYLEX) < 0)",
    "        yychar = 0;",
    "#if YYDEBUG",
    "      if (yydebug) {",
    "        yys = 0;",
    "        if (yychar <= YYMAXTOKEN) yys = yyname[yychar];",
    "        if (!yys) yys = \"illegal-symbol\";",
    "        printf(\"yydebug: state %d, reading %d (%s)\\n\",",
    "               YYFINAL, yychar, yys); ",
    "      }",
    "#endif",
    "    }",
    "    if (yychar == 0) goto yyaccept;",
    "    goto yyloop;",
    "  }",
    "  if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&",
    "      yyn <= YYTABLESIZE && yycheck[yyn] == yystate)",
    "    yystate = yytable[yyn];",
    "  else",
    "    yystate = yydgoto[yym];",
    "#if YYDEBUG",
    "  if (yydebug)",
    "    printf(\"yydebug: after reduction, %s from state %d to state %d\\n\",",
    "           \"shifting\", *(yyps->ssp), yystate);",
    "#endif",
    "  if (yyps->ssp >= yyps->ss + yyps->stacksize - 1) {",
    "    YYMORESTACK;",
    "  }",
    "  *++(yyps->ssp) = yystate;",
    "  *++(yyps->vsp) = yyps->val;",
    "  goto yyloop;",
    "",
    "",
    "  //",
    "  // Reduction declares that this path is valid.",
    "  // Set yypath and do a full parse",
    "  //",
    "yyvalid:",
    "  if (yypath) {",
    "    goto yyabort;",
    "  }",
    "  while (yyps->save) {",
    "    struct yyparsestate *save = yyps->save;",
    "    yyps->save = save->save;",
    "    save->save = yypath;",
    "    yypath = save;",
    "  }",
    "#if YYDEBUG",
    "  if (yydebug)",
    "    printf(\"yydebug: CONFLICT trial successful, backtracking to state %d, %d tokens\\n\",",
    "           yypath->state, yylvp - yylvals - yypath->lexeme);",
    "#endif",
    "  if(yyerrctx) {",
    "    YYFREESTATE(yyerrctx);",
    "    yyerrctx = NULL;",
    "  }",
    "  yychar = -1;",
    "  yyps->ssp = yyps->ss + (yypath->ssp - yypath->ss);",
    "  yyps->vsp = yyps->vs + (yypath->vsp - yypath->vs);",
    "  memcpy (yyps->ss, yypath->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));",
    "  YYSCOPY(yyps->vs, yypath->vs,  yyps->vsp - yyps->vs + 1);",
    "  yylvp = yylvals + yypath->lexeme;",
    "  yylexp = yylexemes + yypath->lexeme;",
    "  yystate = yypath->state;",
    "  goto yyloop;",
    "",
    "",
    "yyabort:",
    "  while (yyps) {",
    "    struct yyparsestate *save = yyps;",
    "    yyps = save->save;",
    "    YYFREESTATE(save);",
    "  }",
    "  while (yypath) {",
    "    struct yyparsestate *save = yypath;",
    "    yypath = save->save;",
    "    YYFREESTATE(save); ",
    "  }",
    "  return (1);",
    "",
    "",
    "yyaccept:",
    "  if (yyps->save) goto yyvalid;",
    "  while (yyps) {",
    "    struct yyparsestate *save = yyps;",
    "    yyps = save->save;",
    "    YYFREESTATE(save);",
    "  }",
    "  while (yypath) {",
    "    struct yyparsestate *save = yypath;",
    "    yypath = save->save;",
    "    YYFREESTATE(save); ",
    "  }",
    "  return (0);",
    "}",
    0
};

struct section section_list[] = {
	{ "banner", &banner[0] },
	{ "tables", &tables[0] },
	{ "header", &header[0] },
	{ "body", &body[0] },
	{ "trailer", &trailer[0] },
	{ 0, 0 } };