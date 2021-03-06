/*
 * Copyright (C) 1991,1992 Erik Schoenfelder (schoenfr@ibr.cs.tu-bs.de)
 *
 * This file is part of NASE A60.
 *
 * NASE A60 is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * NASE A60 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NASE A60; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * a60-parse.y:                     aug '90
 *
 * Erik Schoenfelder (schoenfr@ibr.cs.tu-bs.de)
 *
 * The main part of the Algol 60 parser module.
 *
 * The grammer contains one reduce/reduce conflict (got by the
 * error recovery).
 *
 * The unary '-' usage is still wrong. It's possible to write (and use)
 *  a := 33 + - 7;
 * but that's not allowed in RRA60.
 *
 * The scanner should resolove real values like 1.44 '10' +7. But this
 * is still done by the parser. Not wrong, but not fine.
 */

%start a60program
%union

/* never expected: TCOMMENT. */
%token  TCOMMENT

/* keywords: */
%token TTEN
%token TBEGIN TEND
%token TGOTO TFOR TDO TWHILE TSTEP TUNTIL TIF TTHEN TELSE TSWITCH
%token TPROC TVALUE TCODE
%token TTRUE TFALSE
%token TINTEGER TREAL TBOOL TLABEL TOWN TARRAY TSTRING
%token TPOW TDIV TASSIGN

%token TLESS TNOTGREATER TEQUAL TNOTLESS TGREATER TNOTEQUAL
%token TAND TOR TNOT TIMPL TEQUIV

%token <itype> INUM
%token <rtype> RNUM
%token <str>   NAME STRING

%type <str> identifier label

%type <itype> logical_val signed_inum
%type <rtype> real_value

%type <tree>  program block unlab_block
%type <tree>  unlab_basic_stmt comp_stmt unlab_comp comp_tail
%type <tree>  dummy_stmt basic_stmt uncond_stmt
%type <tree>  stmt cond_stmt for_stmt goto_stmt assign_stmt
%type <tree>  if_stmt tlabel proc_stmt pd_proc_body

%type <sym>   decl type_decl array_decl switch_decl proc_decl type_list
%type <sym>   array_seg array_list
%type <sym>   pd_proc_head pd_proc_hhead pd_form_parmpart
%type <sym>   pd_form_parmlist pd_form_parm
%type <sym>   pd_val_part pd_spec_part pd_spec_idlist pd_ident_list

%type <typ>   type pd_spec pd_proc_type
%type <otype> loc_or_own

%type <expr>  string arith_expr simple_expr mix_expr mix_prim
%type <expr>  subscr_expr relation bool_expr func_desig right_part
%type <expr>  design_expr simp_dexpr switch_des switch_list
%type <expr>  if_clause act_parmpart act_parmlist act_parm

%type <bound> bound_pair bound_pair_list
%type <lhelm> variable left_part left_part_list
%type <mindex> subscr_list

%type <forelm>  for_lelm for_list
%type <forstmt> for_clause

%right  TASSIGN
%left   TEQUIV
%left   TIMPL
%left   TOR
%left   TAND
%left   TLESS TNOTGREATER TEQUAL TNOTLESS TGREATER TNOTEQUAL
%left   '+' '-'
%left   '*' '/' TDIV
%left   TPOW
%left   UNARY

%%

a60program:
    /* empty */

    |

      program

    ;


/* Logical values : */

logical_val
    : TTRUE

    | TFALSE

    ;


/* Identifiers : */

identifier
    : NAME

    ;


/* Strings */

string
    : STRING

    ;


/* Variables : */

variable
    : identifier

    | identifier '[' subscr_list ']'

    ;

subscr_list
    : subscr_expr

    | subscr_list ',' subscr_expr

    ;

subscr_expr
    : arith_expr

    ;


/* function designators : */

/*
 * functions without arguments are recognized as variables.
 */

func_desig
    : identifier '(' act_parmlist ')'
    ;

/* arithmetik expressions : */

arith_expr
    : simple_expr

    | if_clause simple_expr TELSE arith_expr

    ;

simple_expr
    : '+' mix_expr %prec UNARY

    | mix_expr

    ;

mix_expr
    : mix_expr '*' mix_expr

    | mix_expr '/' mix_expr

    | mix_expr '+' mix_expr

    | mix_expr '-' mix_expr

    | mix_expr TPOW mix_expr

    | mix_expr TDIV mix_expr

    | mix_expr TEQUIV mix_expr

    | mix_expr TIMPL mix_expr

    | mix_expr TOR mix_expr

    | mix_expr TAND mix_expr

    | TNOT mix_expr %prec UNARY

    | relation

    | mix_prim

    | '-' mix_prim %prec UNARY

    ;

relation
    : mix_expr TLESS mix_expr

    | mix_expr TNOTGREATER mix_expr

    | mix_expr TEQUAL mix_expr

    | mix_expr TNOTLESS mix_expr

    | mix_expr TGREATER mix_expr

    | mix_expr TNOTEQUAL mix_expr

    ;

mix_prim
    : INUM

    | real_value

    | func_desig

    | variable

    | logical_val

    | '(' simple_expr ')'

    ;

bool_expr: arith_expr;


/* designational expr : */

design_expr
    : simp_dexpr
    | if_clause simp_dexpr TELSE design_expr

    ;

simp_dexpr
    : label

    | switch_des

    | '(' design_expr ')'

    ;

switch_des
    : identifier '[' subscr_expr ']'

    ;


/* compound statements and blocks : */

program
    : block

    | comp_stmt

    ;

block
    : unlab_block

    | tlabel block

    ;

comp_stmt
    : unlab_comp

    | tlabel comp_stmt

    ;

unlab_block
    : block_head ';' comp_tail

    ;

unlab_comp
    : TBEGIN

      comp_tail

    ;

block_head
    : TBEGIN

      decl

    | block_head ';' decl

    | error

    ;

comp_tail
    : stmt TEND

    | stmt ';' comp_tail

    ;

stmt
    : uncond_stmt

    | cond_stmt

    | for_stmt

    ;

uncond_stmt
    : basic_stmt

    | comp_stmt

    | block

    ;

basic_stmt
    : unlab_basic_stmt

    | tlabel basic_stmt

    ;

unlab_basic_stmt
    : assign_stmt

    | goto_stmt

    | dummy_stmt

    | proc_stmt

    | error

    ;


/* assignment statements : */

assign_stmt
    : left_part_list right_part

    ;

right_part
    : arith_expr

    ;

left_part_list
    : left_part

    | left_part_list left_part

    ;

left_part
    : variable TASSIGN

    ;


/* goto statements : */

goto_stmt
    : TGOTO design_expr

    ;


/* dummy statements : */

dummy_stmt
    : /* empty */

    ;


/* conditional statements : */

cond_stmt
    : if_stmt

    | if_stmt TELSE stmt

    | if_clause for_stmt

    | tlabel cond_stmt

    ;

if_stmt
    : if_clause uncond_stmt

    ;

if_clause
    : TIF bool_expr TTHEN

    ;


/* for statements : */

for_stmt
    : for_clause stmt

    | tlabel for_stmt

    ;

for_clause
    : TFOR variable TASSIGN for_list TDO

    ;

for_list
    : for_lelm

    | for_list ',' for_lelm

    ;

for_lelm
    : arith_expr

    | arith_expr TSTEP arith_expr TUNTIL arith_expr

    | arith_expr TWHILE bool_expr

    ;


/* Procedure statements : */

proc_stmt
    : identifier act_parmpart

    ;

act_parmpart
    : /* empty */

    | '(' act_parmlist ')'

    ;

act_parmlist
    : act_parm

    | act_parmlist parm_delim act_parm

    ;

parm_delim
    : ','

    | ')' letter_string ':' '('

    ;

letter_string
    : NAME

    ;

act_parm
    : string

    | arith_expr

    ;


/* Declarations : */

decl
    : type_decl

    | array_decl

    | switch_decl

    | proc_decl

    ;

/* Type declarations : */

type_decl
    : loc_or_own type_list

    ;

loc_or_own
    : type

    | TOWN type

    ;

type_list
    : identifier

    | type_list ',' identifier
    ;

type
    : TINTEGER

    | TREAL

    | TBOOL

    ;


/* Array declarations : */

array_decl
    : TARRAY array_list

    | loc_or_own TARRAY array_list

    ;

array_list
    : array_seg

    | array_list ',' array_seg

    ;

array_seg
    : identifier '[' bound_pair_list ']'

    | identifier ',' array_seg

    ;

bound_pair_list
    : bound_pair

    | bound_pair_list ',' bound_pair

    ;

bound_pair
    : arith_expr ':' arith_expr

    ;


/* Switch declarations : */

switch_decl
    : TSWITCH identifier TASSIGN switch_list

    ;

switch_list
    : design_expr

    | switch_list ',' design_expr

    ;


/* Procedure declarations : */

proc_decl
    : pd_proc_head pd_proc_body

    ;

pd_proc_type
    : /* empty */

    | type

    ;

pd_proc_body
    : stmt

    | TCODE

    ;


pd_proc_hhead
    : pd_proc_type TPROC identifier

    ;

pd_proc_head
    : pd_proc_hhead pd_form_parmpart ';' pd_val_part pd_spec_part

    ;

pd_spec_part
    : /* empty */

    | pd_spec_idlist

    ;

pd_spec_idlist
    : pd_spec pd_ident_list ';'

    | pd_spec_idlist pd_spec pd_ident_list ';'

    ;

pd_spec
    : TSTRING

    | type

    | TARRAY

    | type TARRAY

    | TLABEL

    | TSWITCH

    | TPROC

    | type TPROC

    ;

pd_val_part
    : /* empty */

    | TVALUE pd_ident_list ';'

    ;

pd_ident_list
    : identifier

    | pd_ident_list ',' identifier

    ;

pd_form_parmpart
    : /* empty */

    | '(' pd_form_parmlist ')'

    ;

pd_form_parmlist
    : pd_form_parm

    | pd_form_parmlist parm_delim pd_form_parm

    ;

pd_form_parm
    : identifier

    ;


/* Label parsing: */

tlabel
    : label ':'

    ;

label
    : identifier

    | INUM

    ;

/* real value: */

signed_inum
    : INUM

    | '+' INUM

    | '-' INUM

    ;

real_value
    : RNUM

    | RNUM TTEN signed_inum

    | INUM TTEN signed_inum

    | TTEN signed_inum

    ;

%%

/* end of a60-parse.y */
