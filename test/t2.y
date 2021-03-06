%{
/*
  just faking it so the output will compile...
*/

static void yyerror(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}
static int yylex(void)
{
    return -1;
}
int main(void)
{
    fprintf(stderr, "fake\n");
    return 0;
}

#define ADD 1
#define SUB 2
#define MUL 3
#define MOD 4
#define DIV 5
#define REF 6

typedef struct
{
    int dummy;
} Scope;
typedef  struct
{
    int dummy;
} Expr;
typedef struct
{
    int dummy;
} Expr_List;
typedef struct
{
    int dummy;
} Type;
typedef struct
{
    Type *type;
    Scope *scope;

    int (*istype)(void);
} Decl;
typedef struct
{
    int dummy;
} Decl_List;
typedef struct
{
    int dummy;
} Code;


static Scope *global_scope = NULL;

static Scope *new_scope(Scope *parent)
{
    return 0;
}
static Decl *lookup(Scope *scope, char *id)
{
    return 0;
}
static Scope *start_fn_def(Scope *e, Decl *d)
{
    return 0;
}
static void finish_fn_def(Decl *d, Code *c)
{
}
static Type *type_combine(Type *t1, Type *t2)
{
    return 0;
}
static Type *bare_extern(void)
{
    return 0;
}
static Type *bare_register(void)
{
    return 0;
}
static Type *bare_static(void)
{
    return 0;
}
static Type *bare_const(void)
{
    return 0;
}
static Type *bare_volatile(void)
{
    return 0;
}
static Decl *declare(Scope *e, char *id, Type *t)
{
    return 0;
}
static Decl *make_pointer(Decl *e, Type *t)
{
    return 0;
}
static Decl *make_array(Type *t, Expr *e)
{
    return 0;
}
static Decl *build_function(Decl *t1, Decl_List *t3, Type *t5)
{
    return 0;
}
static Decl_List *append_dlist(Decl_List *l, Decl *d)
{
    return 0;
}
static Decl_List *build_dlist(Decl *d)
{
    return 0;
}
static Expr *build_expr(Expr *e1, int op, Expr *e3)
{
    return 0;
}
static Expr *var_expr(Scope *e1, char *id)
{
    return 0;
}
static Code *build_expr_code(Expr *e1)
{
    return 0;
}
static Code *build_if(Expr *e1, Code *c2, Code *c3)
{
    return 0;
}
static Code *code_append(Code *l, Code *e)
{
    return 0;
}

/* ----------------------------------------------- */
%}

%union {
    Scope       *scope;
    Expr        *expr;
    Expr_List   *elist;
    Type        *type;
    Decl        *decl;
    Decl_List   *dlist;
    Code        *code;
    char        *id;
};

%left '+' '-'
%left '*' '/' '%'
%nonassoc PREFIX
%nonassoc POSTFIX '(' '[' '.'

%token <id>     ID
%token <expr>   CONSTANT
%token          EXTERN REGISTER STATIC CONST VOLATILE IF THEN ELSE CLCL

%type <expr>    expr(<scope>)
%type           decl(<scope>) declarator_list(<scope>, <type>)
                decl_list(<scope>)
%type <code>    statement(<scope>) statement_list(<scope>)
                block_statement(<scope>)
%type <decl>    declarator(<scope>, <type>) formal_arg(<scope>)
%type <type>    decl_specs(<scope>) decl_spec(<scope>) typename(<scope>)
                cv_quals cv_qual
%type <scope>   opt_scope(<scope>)
%type <dlist>   formal_arg_list(<scope>) nonempty_formal_arg_list(<scope>)

%start input

%%

opt_scope($e):                                  [ $$ = $e; ]
    | CLCL                                      [ $$ = global_scope; ]
    | opt_scope ID CLCL                         [ Decl *d = lookup($1, $2);
                                                  if (!d || !d->scope)
                                                    YYERROR;
                                                  $$ = d->scope; ]
    ;

typename($e): opt_scope ID
                                                [ Decl *d = lookup($1, $2);
                                                  if (!d || !d->istype())
                                                    YYERROR;
                                                  $$ = d->type; ]
    ;

input: decl_list(global_scope = new_scope(0)) ;
decl_list($e):
    | decl_list decl($e) ;
decl($e):
      decl_specs declarator_list($e, $1) ';'    [YYVALID;]
    | decl_specs declarator($e, $1) block_statement(start_fn_def($e, $2))
                                                { finish_fn_def($2, $3); }
    ;

decl_specs($e):
      decl_spec                                 [ $$ = $1; ]
    | decl_specs decl_spec($e)                  [ $$ = type_combine($1, $2); ]
    ;

cv_quals:                                       [ $$ = 0; ]
    | cv_quals cv_qual                          [ $$ = type_combine($1, $2); ]
    ;

decl_spec($e):
      cv_qual                                   [ $$ = $1; ]
    | typename                                  [ $$ = $1; ]
    | EXTERN                                    [ $$ = bare_extern(); ]
    | REGISTER                                  [ $$ = bare_register(); ]
    | STATIC                                    [ $$ = bare_static(); ]
    ;

cv_qual:
      CONST                                     [ $$ = bare_const(); ]
    | VOLATILE                                  [ $$ = bare_volatile(); ]
    ;

declarator_list($e, $t):
      declarator_list ',' declarator($e, $t)
    | declarator
    ;

declarator($e, $t):
      /* empty */                               [ if (!$t) YYERROR; ]
                                                { $$ = declare($e, 0, $t); }
    | ID                                        { $$ = declare($e, $1, $t); }
    | '(' declarator($e, $t) ')'                { $$ = $2; }
    | '*' cv_quals declarator($e, $t) %prec PREFIX
                                                { $$ = make_pointer($3, $2); }
    | declarator '[' expr($e) ']'
                                                { $$ = make_array($1->type, $3); }
    | declarator '(' formal_arg_list($e) ')' cv_quals
                                                { $$ = build_function($1, $3, $5); }
    ;

formal_arg_list($e):                            { $$ = 0; }
    | nonempty_formal_arg_list                  { $$ = $1; }
    ;
nonempty_formal_arg_list($e):
      nonempty_formal_arg_list ',' formal_arg($e)
                                                { $$ = append_dlist($1, $3); }
    | formal_arg                                { $$ = build_dlist($1); }
    ;
formal_arg($e):
    decl_specs declarator($e, $1)               { $$ = $2; }
    ;

expr($e):
      expr '+' expr($e)                         { $$ = build_expr($1, ADD, $3); }
    | expr '-' expr($e)                         { $$ = build_expr($1, SUB, $3); }
    | expr '*' expr($e)                         { $$ = build_expr($1, MUL, $3); }
    | expr '%' expr($e)                         { $$ = build_expr($1, MOD, $3); }
    | expr '/' expr($e)                         { $$ = build_expr($1, DIV, $3); }
    | '*' expr($e) %prec PREFIX                 { $$ = build_expr(0, REF, $2); }
    | ID                                        { $$ = var_expr($e, $1); }
    | CONSTANT                                  { $$ = $1; }
    ;

statement($e):
      decl                                      { $$ = 0; }
    | expr($e) ';'                              [YYVALID;]
                                                { $$ = build_expr_code($1); }
    | IF '(' expr($e) ')' THEN statement($e) ELSE statement($e)
                                                [YYVALID;]
                                                { $$ = build_if($3, $6, $8); }
    | IF '(' expr($e) ')' THEN statement($e)    [YYVALID;]
                                                { $$ = build_if($3, $6, 0); }
    | block_statement(new_scope($e))            [YYVALID;]{ $$ = $1; }
    ;

statement_list($e):                             { $$ = 0; }
    | statement_list statement($e)              { $$ = code_append($1, $2); }
    ;

block_statement($e):
    '{' statement_list($e) '}'                  { $$ = $2; }
    ;
