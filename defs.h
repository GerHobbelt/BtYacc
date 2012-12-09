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


#if defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>


/*  machine-dependent definitions                       */
/*  the following definitions are for the Tahoe         */
/*  they might have to be changed for other machines    */

/*  MAXCHAR is the largest unsigned character value     */
/*  MAXSHORT is the largest value of a C short          */
/*  MINSHORT is the most negative value of a C short    */
/*  MAXTABLE is the maximum table size                  */
/*  BITS_PER_WORD is the number of bits in a C unsigned */
/*  WORDSIZE computes the number of words needed to     */
/*      store n bits                                    */
/*  BIT returns the value of the n-th bit starting      */
/*      from r (0-indexed)                              */
/*  SETBIT sets the n-th bit starting from r            */

#define MAXCHAR         255




#if defined(__MSDOS__) /* || defined(WIN32) || defined(__WIN32) */
#define BITS_PER_WORD   16
#define LOG2_BPW        4
#define MAXSHORT        ((int)0x7FFF)
#define MINSHORT        ((int)0x8000)
#define MAXTABLE        4000
#else    /* Real computers... */
#define BITS_PER_WORD   32
#define LOG2_BPW        5
#define MAXSHORT        ((int)0x7FFFFFFF)
#define MINSHORT        ((int)0x80000000)
#define MAXTABLE        120000
#endif
#define BITS_PER_WORD_1 (BITS_PER_WORD - 1)

#define WORDSIZE(n)     (((n) + (BITS_PER_WORD_1)) / BITS_PER_WORD)
#define BIT(r, n)       ((((r)[(n) >> LOG2_BPW]) >> ((n) & BITS_PER_WORD_1)) & 1)
#define SETBIT(r, n)    ((r)[(n) >> LOG2_BPW] |= ((unsigned)1 << ((n) & BITS_PER_WORD_1)))

/* VM: this is a 32-bit replacement for original 16-bit short */
typedef int Yshort;


/*  character names  */

#define NUL             '\0'    /*  the null character  */
#define NEWLINE         '\n'    /*  line feed  */
#define SP              ' '     /*  space  */
#define BS              '\b'    /*  backspace  */
#define HT              '\t'    /*  horizontal tab  */
#define VT              '\013'  /*  vertical tab  */
#define CR              '\r'    /*  carriage return  */
#define FF              '\f'    /*  form feed  */
#define QUOTE           '\''    /*  single quote  */
#define DOUBLE_QUOTE    '\"'    /*  double quote  */
#define BACKSLASH       '\\'    /*  backslash  */



typedef enum keyword_code_enumeration
{
    NOT_A_KEYWORD                   = -1,

    TOKEN                           =  0,
    LEFT                            =  1,
    RIGHT                           =  2,
    NONASSOC                        =  3,
    MARK                            =  4,
    TEXT                            =  5,
    TYPE                            =  6,
    START                           =  7,
    UNION                           =  8,
    IDENT                           =  9,
    SCANNERLESS_ZERO_ASCII          = 10,
    PREFER                          = 11,

    /* [i_a] bison emulation additions */
    BISON_DEBUG                     = 110,
    BISON_DEFINE                    = 111,
    BISON_DEFINES                   = 112,
    BISON_LOCATIONS                 = 113,
    BISON_PURE                      = 114,
    BISON_NAME_PREFIX               = 115,
    BISON_FILE_PREFIX               = 116,
    BISON_ERR_VERBOSE               = 117,
    BISON_EXPECT                    = 118,
    BISON_GLR_PARSER                = 119,
    BISON_LEX_PARAM                 = 120,
    BISON_NO_LINES                  = 121,
    BISON_OUTPUT                    = 122,
    BISON_PARSE_PARAM               = 123,
    BISON_SKELETON                  = 124,
    BISON_TOKEN_TABLE               = 125,
    BISON_VERBOSE                   = 126,
    BISON_YACC                      = 127,
    BISON_DESTRUCTOR                = 128,
    BISON_PRINTER                   = 129,
    NTERM                           = 130,
    DPREC_PRIO                      = 131,
    BISON_MERGE                     = 132,
    PREC                            = 133
} BtYacc_keyword_code;

typedef enum symbol_class_enumeration
{
    UNKNOWN = 0,
    TERM = 1,
    NONTERM = 2,
    ACTION = 3,
    ARGUMENT = 4
} BtYacc_symbol_class;


/*  the undefined value  */

#define UNDEFINED (-1)


typedef enum action_code_enumeration
{
    UNDEFINED_ACTION = 0,

    SHIFT = 1,
    REDUCE = 2
} BtYacc_action_code;


/*  character macros  */

#define IS_IDENT(c)             (isalnum(c) || (c) == '_' || (c) == '.' || (c) == '$')
#define IS_OCTAL(c)             ((c) >= '0' && (c) <= '7')
#define NUMERIC_VALUE(c)        ((c) - '0')


/*  symbol macros  */

#define ISTOKEN(s)              ((s) < start_symbol)
#define ISVAR(s)                ((s) >= start_symbol)


/*  storage allocation macros  */

#define MALLOC(n)       malloc((unsigned)(n))
#define CALLOC(k,n)     calloc((unsigned)(k), (unsigned)(n))
#define STRDUP(s)       strdup(s)
#define FREE(x)         do { free((char*)(x)); x = 0; } while (0)
#define NEW(t)          CALLOC(1, sizeof(t))
#define NEW2(n,t)       ((n) ? CALLOC((n), sizeof(t)) : NULL)
#define REALLOC(p,n,t)  (realloc((char*)(p), (unsigned)((n) * sizeof(t))))
#define RENEW(p,n,t)    (realloc((char*)(p), (unsigned)((n) * sizeof(t))))


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
    Yshort prec;                /* precedence: UNDEFINED or number > 0 */
    BtYacc_symbol_class symbol_class;
    BtYacc_keyword_code assoc;  /* associativity: LEFT/RIGHT/NONASSOC/other */
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


typedef enum suppression_reason
{
    SUPPR_BY_PRECEDENCE = 1,
    SUPPR_BY_LEFT_ASSOC = 2,
    SUPPR_BY_RIGHT_ASSOC = 3,
    SUPPR_NONE_SR_CONFLICT = 4,
    SUPPR_NONE_RR_CONFLICT = 5,
    SUPPR_BY_YACC_DEFAULT1 = 10,
    SUPPR_BY_YACC_DEFAULT2 = 11
} suppression_reason_t;

typedef struct suppression_info suppression_info;
struct suppression_info
{
    struct action *conflicting_entry;
    Yshort state;
    char me_is_old_pref;
    char me_is_new_pref;
    suppression_reason_t reason;
};

/*  the structure used to represent parser actions  */

typedef struct action action;
struct action
{
    struct action *next;
    Yshort symbol;
    Yshort number;
    Yshort prec;                /* precedence: UNDEFINED or number > 0 */
    BtYacc_action_code action_code;
    BtYacc_keyword_code assoc;  /* associativity: LEFT/RIGHT/NONASSOC/other */
    char   suppressed;
    char   rej_count;
    suppression_info *rej_info;
};

struct section {
    char const * name;
    char const * const * ptr;
    char const * cached_multiline_ptr;
};

extern struct section section_list_btyaccpa[];
extern unsigned int section_list_btyaccpa_count;

extern struct section *active_section_list;


/* global variables */

extern char dflag;
extern char lflag;
extern char rflag;
extern char tflag;
extern char vflag;
extern char Eflag;
extern char ZEROflag;


extern char *myname;
extern char *target_dir;
extern char *file_prefix;
extern char *file_uc_prefix;
extern char *name_prefix;
extern char *name_uc_prefix;
extern char *cptr;
extern char *line;
extern int unsigned lineno;
extern int unsigned CODE_FILE;              /*  y.code.c (used when the -r option is specified) */
extern int unsigned DEFINES_FILE;           /*  y.tab.h                                         */
extern int unsigned OUTPUT_FILE;            /*  y.tab.c                                         */
extern int unsigned outline[3];
extern const char *outfilename[3];

extern char *action_file_name;
extern char *code_file_name;
extern char *defines_file_name;
extern char *input_file_name;
extern char *output_file_name;
extern char *text_file_name;
extern char *union_file_name;
extern char *log_file_name;
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
extern FILE *log_file;
extern FILE *verbose_file;

extern int nitems;
extern int nrules;
extern int nsyms;
extern int ntokens;
extern int nvars;
extern int ntags;

extern char unionized;
extern char const *line_format;

extern Yshort start_symbol;
extern char  **symbol_name;
extern Yshort *symbol_value;
extern Yshort *symbol_prec;
extern BtYacc_keyword_code *symbol_assoc;

extern Yshort *ritem;
extern Yshort *rlhs;
extern Yshort *rrhs;
extern Yshort *rprec;
extern BtYacc_keyword_code *rassoc;

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
extern int unsigned *LA;
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

extern Yshort *itemset;
extern Yshort *itemsetend;
extern int unsigned *ruleset;


/* system variable */
#if !HAS_MSVC_2005_ISO_RTL
#include <errno.h>
#endif

#if defined(__GNUC__)
#  if defined(__ICC)
#    define ATTRIBUTE(x)
#  else
#    if ((__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#      define ATTRIBUTE(x) __attribute__(x)
#      define CAN_USE_ATTRIBUTE 1
#    else
#      define ATTRIBUTE(x)
#    endif
#  endif
#else
#  define ATTRIBUTE(x)
#endif

#define GCC_NO_RETURN ATTRIBUTE((noreturn))

#ifndef S_SPLINT_S
#  define SPLINT_NO_RETURN
#else
#  define SPLINT_NO_RETURN /*@noreturn@*/
#endif

/* global functions */

void os_init_heap(void);

/* closure.c */
void set_first_derives(void);
void closure(Yshort *, int);
void finalize_closure(void);

/* error.c */
extern int read_errs;

SPLINT_NO_RETURN void fatal(char const * msg, ...) GCC_NO_RETURN;
SPLINT_NO_RETURN void no_space(void) GCC_NO_RETURN;
SPLINT_NO_RETURN void open_error(char const * filename) GCC_NO_RETURN;
SPLINT_NO_RETURN void unexpected_EOF(void) GCC_NO_RETURN;
void print_pos(char const * st_line, char const * st_cptr);
void error(int unsigned lineno, char const * line, char const * cptr, char const * msg, ...);
void errorv(int unsigned lineno, char const * line, char const * cptr, char const * msg0, char const * msg, va_list args);
SPLINT_NO_RETURN void syntax_error(int unsigned lineno, char const * line, char const * cptr) GCC_NO_RETURN;
SPLINT_NO_RETURN void syntax_error_ex(int unsigned lineno, char const * line, char const * cptr, char const *msg, ...) GCC_NO_RETURN;
SPLINT_NO_RETURN void unsupported_feature(int unsigned lineno, char const * line, char const * cptr) GCC_NO_RETURN;
SPLINT_NO_RETURN void unterminated_comment(int unsigned lineno, char const * line, char const * cptr) GCC_NO_RETURN;
SPLINT_NO_RETURN void unterminated_string(int unsigned lineno, char const * line, char const * cptr) GCC_NO_RETURN;
SPLINT_NO_RETURN void unterminated_text(int unsigned lineno, char const * line, char const * cptr) GCC_NO_RETURN;
SPLINT_NO_RETURN void unterminated_union(int unsigned lineno, char const * line, char const * cptr) GCC_NO_RETURN;
SPLINT_NO_RETURN void over_unionized(char const * cptr) GCC_NO_RETURN;
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
void terminal_lhs(int unsigned lineno);
void prec_redeclared(void);
void unterminated_action(int unsigned lineno, char const * line, char const * cptr);
void unterminated_arglist(int unsigned lineno, char const * line, char const * cptr);
void bad_formals(void);
void dollar_warning(int unsigned lineno, int i);
void dollar_error(int unsigned lineno, char const * line, char const * cptr);
void untyped_lhs(void);
void untyped_rhs(int i, char const * s);
void unknown_rhs(int);
void default_action_warning(void);
void undefined_goal(char const * s);
void undefined_symbol_warning(char const * s);
void table_too_large_error(int high);
void unknown_section_name_error(char const *section_name);

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
SPLINT_NO_RETURN void done(int) GCC_NO_RETURN;

#if defined(SIGINT) || defined(SIGTERM) || defined(SIGHUP)
#define BTYACC_USE_SIGNAL_HANDLING
void BtYacc_stop_test(void);
#define BTYACC_INTERRUPTION_CHECK() BtYacc_stop_test()
#else
#define BTYACC_INTERRUPTION_CHECK()
#endif

void strupper(char *str);
char *sanitize_to_varname(const char *in_str);
void *allocate(unsigned int size);
void create_temporary_files(void);
void create_output_files(void);
void open_temporary_files(void);
void open_output_files(void);
int main(int, char **);

/* mkpar.c */
int is_assigned_explicit_associativity(BtYacc_keyword_code assoc);
void make_parser(void);
action *parse_actions(int);
action *get_shifts(int);
action *add_reductions(int, action *);
action *add_reduce(action *, int, int);
int sole_reduction(int);
void free_action_row(action *);
void free_parser(void);

/* output.c */
void print_file_line_location(FILE *outfile, unsigned int outfile_idx);
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
struct section *lookup_section_name(char const * section_name);
char const *get_section(char const * section_name);
void free_section_cache(void);
char const *get_rule_description(int rule_nr);
void free_action_order(void);

/* reader.c */
int cachec(int);
char *get_line(void);
char *dup_line(void);
char *skip_comment(void);
int nextc(int only_skip_whitespace);
const char *get_keyword_as_string(BtYacc_keyword_code a);
BtYacc_keyword_code keyword(int fail_when_no_keyword);
void copy_ident(void);
void copy_string(int, FILE *, FILE *);
void copy_comment(FILE *, FILE *);
void copy_text(void);
void copy_union(void);
int hexval(int);
bucket *get_literal(int store_literal);
int is_reserved(char const * name);
bucket *get_name(int store_name);
int get_number(void);
char *get_tag(void);
void declare_tokens(BtYacc_keyword_code assoc);
void declare_types(void);
void declare_start(void);
void read_declarations(void);
void initialize_grammar(void);
void expand_items(void);
void expand_rules(void);
void advance_to_start(void);
void start_rule(bucket *bp, int unsigned lineno);
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
void free_reader_buffers(void);
void reader(void);

/* readskel.c */
struct section *read_skel(char const * name);

/* symtab.c */
unsigned int hash(char const * name);
bucket* make_bucket(char const * name);
bucket* lookup(char const * name);
void create_symbol_table(void);
void free_symbol_table(void);
void free_symbols(void);

/* verbose.c */
void verbose(void);
void log_unused(void);
void log_conflicts(void);
void print_state(int state);
void print_conflicts(int state);
void print_core(int state);
void print_nulls(int state);
void print_actions(int state);
void print_shifts(action const * p);
void print_reductions(action const * p, int defred);
void print_gotos(int state);

/* warshall.c */
void transitive_closure(unsigned *, int);
void reflexive_transitive_closure(unsigned *, int);



#endif
