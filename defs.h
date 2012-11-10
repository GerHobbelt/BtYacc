#ifndef __YACC_DEFS_H__
#define __YACC_DEFS_H__

           
           
/* [i_a] */
#ifndef HAS_MSVC_2005_ISO_RTL
#if defined(_MSC_VER)
#if _MSC_VER >= 1400 /* VS.NET 2005 or above: 'fix' those deprecated functions */
#define HAS_MSVC_2005_ISO_RTL     1
#endif
#endif

#ifndef HAS_MSVC_2005_ISO_RTL
#define HAS_MSVC_2005_ISO_RTL     0
#endif

#if HAS_MSVC_2005_ISO_RTL
#pragma warning(disable : 4996)
// Or just turn off warnings about the newly deprecated CRT functions.
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES     1
#endif
#endif
/* [/i_a] */


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>


/*  machine-dependent definitions			*/
/*  the following definitions are for the Tahoe		*/
/*  they might have to be changed for other machines	*/

/*  MAXCHAR is the largest unsigned character value	*/
/*  MAXSHORT is the largest value of a C short		*/
/*  MINSHORT is the most negative value of a C short	*/
/*  MAXTABLE is the maximum table size			*/
/*  BITS_PER_WORD is the number of bits in a C unsigned	*/
/*  WORDSIZE computes the number of words needed to	*/
/*	store n bits					*/
/*  BIT returns the value of the n-th bit starting	*/
/*	from r (0-indexed)				*/
/*  SETBIT sets the n-th bit starting from r		*/

#define	MAXCHAR		255




#if defined(__MSDOS__) /* || defined(WIN32) || defined(__WIN32) */
#define BITS_PER_WORD   16
#define LOG2_BPW    4
#define	MAXSHORT	((int)0x7FFF)
#define MINSHORT	((int)0x8000)
#define MAXTABLE	4000
#else    /* Real computers... */
#define BITS_PER_WORD	32
#define LOG2_BPW    5
#define	MAXSHORT	((int)0x7FFFFFFF)
#define MINSHORT	((int)0x80000000)
#define MAXTABLE	120000
#endif
#define BITS_PER_WORD_1 (BITS_PER_WORD-1)

#define WORDSIZE(n) (((n)+(BITS_PER_WORD_1))/BITS_PER_WORD)
#define BIT(r, n)   ((((r)[(n)>>LOG2_BPW])>>((n)&BITS_PER_WORD_1))&1)
#define SETBIT(r, n) ((r)[(n)>>LOG2_BPW]|=((unsigned)1<<((n)&BITS_PER_WORD_1)))

/* VM: this is a 32-bit replacement for original 16-bit short */
typedef int Yshort;


/*  character names  */

#define	NUL		'\0'    /*  the null character  */
#define	NEWLINE		'\n'    /*  line feed  */
#define	SP		' '     /*  space  */
#define	BS		'\b'    /*  backspace  */
#define	HT		'\t'    /*  horizontal tab  */
#define	VT		'\013'  /*  vertical tab  */
#define	CR		'\r'    /*  carriage return  */
#define	FF		'\f'    /*  form feed  */
#define	QUOTE		'\''    /*  single quote  */
#define	DOUBLE_QUOTE	'\"'    /*  double quote  */
#define	BACKSLASH	'\\'    /*  backslash  */


/* defines for constructing filenames */

#define DEFINES_SUFFIX  ".tab.h"
#define OUTPUT_SUFFIX   ".tab.c"
#define CODE_SUFFIX     ".code.c"
#define VERBOSE_SUFFIX  ".output"

/* keyword codes */

#define TOKEN 0
#define LEFT 1
#define RIGHT 2
#define NONASSOC 3
#define MARK 4
#define TEXT 5
#define TYPE 6
#define START 7
#define UNION 8
#define IDENT 9

/* [i_a] bison emulation additions */
#define BISON_DEBUG			110
#define BISON_DEFINE		111
#define BISON_DEFINES		112
#define BISON_LOCATIONS		113
#define BISON_PURE			114
#define BISON_NAME_PREFIX	115
#define BISON_FILE_PREFIX	116
#define BISON_ERR_VERBOSE	117
#define BISON_EXPECT		118
#define BISON_GLR_PARSER	119
#define BISON_LEX_PARAM		120
#define BISON_NO_LINES		121
#define BISON_OUTPUT		122
#define BISON_PARSE_PARAM	123
#define BISON_SKELETON		124
#define BISON_TOKEN_TABLE	125
#define BISON_VERBOSE		126
#define BISON_YACC			127
#define BISON_DESTRUCTOR	128
#define BISON_PRINTER		129
#define BISON_NTERM			130
#define BISON_DPREC			131
#define BISON_MERGE			132
#define BISON_PREC			133


/*  symbol classes  */

#define UNKNOWN 0
#define TERM 1
#define NONTERM 2
#define ACTION 3
#define ARGUMENT 4


/*  the undefined value  */

#define UNDEFINED (-1)


/*  action codes  */

#define SHIFT 1
#define REDUCE 2


/*  character macros  */

#define IS_IDENT(c)	(isalnum(c) || (c) == '_' || (c) == '.' || (c) == '$')
#define	IS_OCTAL(c)	((c) >= '0' && (c) <= '7')
#define	NUMERIC_VALUE(c)	((c) - '0')


/*  symbol macros  */

#define ISTOKEN(s)	((s) < start_symbol)
#define ISVAR(s)	((s) >= start_symbol)


/*  storage allocation macros  */

#define CALLOC(k,n)	(calloc((unsigned)(k),(unsigned)(n)))
#define	FREE(x)		(free((char*)(x)))
#define MALLOC(n)	(malloc((unsigned)(n)))
#define	NEW(t)		((t*)allocate(sizeof(t)))
#define	NEW2(n,t)	((t*)allocate((unsigned)((n)*sizeof(t))))
#define REALLOC(p,n)	(realloc((char*)(p),(unsigned)(n)))
#define RENEW(p,n,t)	((t*)realloc((char*)(p),(unsigned)((n)*sizeof(t))))


/*  the structure of a symbol table entry  */

typedef struct bucket bucket;
struct bucket
{
    struct bucket *link;
    struct bucket *next;
    char *name;
    char *tag;
    char **argnames;
    char **argtags;
    Yshort args;
    Yshort value;
    Yshort index;
    Yshort prec;
    char class;
    char assoc;
};


/*  the structure of the LR(0) state machine  */

typedef struct core core;
struct core
{
    struct core *next;
    struct core *link;
    Yshort number;
    Yshort accessing_symbol;
    Yshort nitems;
    Yshort items[1];
};


/*  the structure used to record shifts  */

typedef struct shifts shifts;
struct shifts
{
    struct shifts *next;
    Yshort number;
    Yshort nshifts;
    Yshort shift[1];
};


/*  the structure used to store reductions  */

typedef struct reductions reductions;
struct reductions
{
    struct reductions *next;
    Yshort number;
    Yshort nreds;
    Yshort rules[1];
};


/*  the structure used to represent parser actions  */

typedef struct action action;
struct action
{
    struct action *next;
    Yshort symbol;
    Yshort number;
    Yshort prec;
    char   action_code;
    char   assoc;
    char   suppressed;
};

struct section {
    char const * name;
    char const * const * ptr;
};

extern struct section section_list[];


/* global variables */

extern char dflag;
extern char lflag;
extern char rflag;
extern char tflag;
extern char vflag;

extern char *myname;
extern char *file_prefix;
extern char *name_prefix;
extern char *cptr;
extern char *line;
extern int lineno;
extern int outline;

extern char const * const banner[];
extern char const * const tables[];
extern char const * const header[];
extern char const * const body[];
extern char const * const trailer[];

extern char *action_file_name;
extern char *code_file_name;
extern char *defines_file_name;
extern char *input_file_name;
extern char *output_file_name;
extern char *text_file_name;
extern char *union_file_name;
extern char *verbose_file_name;

extern FILE *inc_file;
extern char  inc_file_name[];

extern FILE *action_file;
extern FILE *code_file;
extern FILE *defines_file;
extern FILE *input_file;
extern FILE *output_file;
extern FILE *text_file;
extern FILE *union_file;
extern FILE *verbose_file;

extern int nitems;
extern int nrules;
extern int nsyms;
extern int ntokens;
extern int nvars;
extern int ntags;

extern char unionized;
extern char const line_format[];

extern int   start_symbol;
extern char  **symbol_name;
extern Yshort *symbol_value;
extern Yshort *symbol_prec;
extern char  *symbol_assoc;

extern Yshort *ritem;
extern Yshort *rlhs;
extern Yshort *rrhs;
extern Yshort *rprec;
extern char  *rassoc;

extern Yshort **derives;
extern char *nullable;

extern bucket *first_symbol;
extern bucket *last_symbol;

extern int nstates;
extern core *first_state;
extern shifts *first_shift;
extern reductions *first_reduction;
extern Yshort *accessing_symbol;
extern core **state_table;
extern shifts **shift_table;
extern reductions **reduction_table;
extern unsigned *LA;
extern Yshort *LAruleno;
extern Yshort *lookaheads;
extern Yshort *goto_map;
extern Yshort *from_state;
extern Yshort *to_state;

extern action **parser;
extern int SRtotal;
extern int RRtotal;
extern Yshort *SRconflicts;
extern Yshort *RRconflicts;
extern Yshort *defred;
extern Yshort *rules_used;
extern Yshort nunused;
extern Yshort final_state;

/* system variable */
#if !HAS_MSVC_2005_ISO_RTL
#include <errno.h>
#endif

/* global functions */

/* closure.c */
void set_first_derives(void);
void closure(Yshort *, int);
void finalize_closure(void);

/* error.c */
void fatal(char const * msg);
void no_space(void);
void open_error(char const * filename);
void unexpected_EOF(void);
void print_pos(char const * st_line, char const * st_cptr);
void error(int unsigned lineno, char const * line, char const * cptr, char const * msg, ...);
void syntax_error(int unsigned lineno, char const * line, char const * cptr);
void unsupported_feature(int unsigned lineno, char const * line, char const * cptr);
void unterminated_comment(int unsigned lineno, char const * line, char const * cptr);
void unterminated_string(int unsigned lineno, char const * line, char const * cptr);
void unterminated_text(int unsigned lineno, char const * line, char const * cptr);
void unterminated_union(int unsigned lineno, char const * line, char const * cptr);
void over_unionized(char const * cptr);
void illegal_tag(int unsigned lineno, char const * line, char const * cptr);
void illegal_character(char const * cptr);
void used_reserved(char const * s);
void tokenized_start(char const * s);
void retyped_warning(char const * s);
void reprec_warning(char const * s);
void revalued_warning(char const * s);
void terminal_start(char const * s);
void restarted_warning(void);
void no_grammar(void);
void terminal_lhs(int);
void prec_redeclared(void);
void unterminated_action(int unsigned lineno, char const * line, char const * cptr);
void unterminated_arglist(int unsigned lineno, char const * line, char const * cptr);
void bad_formals(void);
void dollar_warning(int, int);
void dollar_error(int unsigned lineno, char const * line, char const * cptr);
void untyped_lhs(void);
void untyped_rhs(int i, char const * s);
void unknown_rhs(int);
void default_action_warning(void);
void undefined_goal(char const * s);
void undefined_symbol_warning(char const * s);

/* lalr.c */
void lalr(void);

/* lr0.c */
void show_cores(void);
void show_ritems(void);
void show_rrhs(void);
void show_shifts(void);
void free_derives(void);
void free_nullable(void);
void lr0(void);

/* main.c */
void done(int);

#if defined(SIGINT) || defined(SIGTERM) || defined(SIGHUP)
#define BTYACC_USE_SIGNAL_HANDLING
void BtYacc_stop_test(void);
#define BTYACC_INTERRUPTION_CHECK BtYacc_stop_test();
#else
#define BTYACC_INTERRUPTION_CHECK
#endif

char *allocate(unsigned);
int main(int, char **);

/* mkpar.c */
void make_parser(void);
action *parse_actions(int);
action *get_shifts(int);
action *add_reductions(int, action *);
action *add_reduce(action *, int, int);
int sole_reduction(int);
void free_action_row(action *);
void free_parser(void);

/* output.c */
void output(void);
void output_rule_data(void);
void output_yydefred(void);
void output_actions(void);
int matching_vector(int);
int pack_vector(int);
void output_base(void);
void output_table(void);
void output_check(void);
void output_ctable(void);
int is_C_identifier(char const * name);
void output_defines(void);
void output_stored_text(void);
void output_debug(void);
void output_stype(void);
void output_trailing_text(void);
void output_semantic_actions(void);
void free_itemsets(void);
void free_shifts(void);
void free_reductions(void);
void write_section(char const * section_name);

/* reader.c */
int cachec(int);
char *get_line(void);
char *dup_line(void);
char *skip_comment(void);
int nextc(void);
int keyword(void);
void copy_ident(void);
void copy_string(int, FILE *, FILE *);
void copy_comment(FILE *, FILE *);
void copy_text(void);
void copy_union(void);
int hexval(int);
bucket *get_literal(void);
int is_reserved(char const * name);
bucket *get_name(void);
int get_number(void);
char *get_tag(void);
void declare_tokens(int);
void declare_types(void);
void declare_start(void);
void read_declarations(void);
void initialize_grammar(void);
void expand_items(void);
void expand_rules(void);
void advance_to_start(void);
void start_rule(bucket *, int);
void end_rule(void);
void insert_empty_rule(void);
void add_symbol(void);
void copy_action(void);
int mark_symbol(void);
void read_grammar(void);
void free_tags(void);
void pack_names(void);
void check_symbols(void);
void pack_symbols(void);
void pack_grammar(void);
void print_grammar(void);
void reader(void);

/* readskel.c */
void read_skel(char const * name);

/* symtab.c */
int hash(char const * name);
bucket* make_bucket(char const * name);
bucket* lookup(char const * name);
void create_symbol_table(void);
void free_symbol_table(void);
void free_symbols(void);

/* verbose.c */
void verbose(void);
void log_unused(void);
void log_conflicts(void);
void print_state(int);
void print_conflicts(int);
void print_core(int);
void print_nulls(int);
void print_actions(int);
void print_shifts(action const * p);
void print_reductions(action const * p, int defred);
void print_gotos(int);

/* warshall.c */
void transitive_closure(unsigned *, int);
void reflexive_transitive_closure(unsigned *, int);



#endif
