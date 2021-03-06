#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "mstring.h"
#include "write.h"

/*  The line size must be a positive integer.  One hundred was chosen   */
/*  because few lines in Yacc input grammars exceed 100 characters.     */
/*  Note that if a line exceeds LINESIZE characters, the line buffer    */
/*  will be expanded to accomodate it.                                  */

#define LINESIZE 100

/* the maximum nember of arguments (inherited attributes) to a non-terminal */
/* this is a hard limit, but seems more than adequate */
#define MAXARGS 20

static char *cache = NULL;
static int cinc = 0;
static int cache_size = 0;

int ntags = 0;
static int tagmax = 0;
static int havetags = 0;
static char **tag_table = NULL;

static char saw_eof = 0;

char unionized = 0;
char *cptr = NULL;
char *line = NULL;
static int linesize = 0;

FILE *inc_file = NULL;
char  inc_file_name[LINESIZE] = "???";
static int unsigned inc_save_lineno = 0;

static int in_ifdef = 0;
static int ifdef_skip = 0;

#define MAX_DEFD_VARS 1000
char *defd_vars[MAX_DEFD_VARS] = {NULL};

static bucket *goal = NULL;
static Yshort precedence_max = 0;
static int gensym = 0;
static char last_was_action = 0;

static int maxitems = 0;
static bucket **pitem = NULL;

static int maxrules = 0;
static bucket **plhs = NULL;

static int name_pool_size = 0;
static char *name_pool = NULL;

char const *line_format = NULL;

int cachec(int c)
{
    assert(cinc >= 0);
    if (cinc >= cache_size)
    {
        cache = REALLOC(cache, cache_size += 256, cache[0]);
        if (!cache) no_space();
    }
    return cache[cinc++] = c;
}

/*
 * Get line
 * Return: 1 - reg. line finished with '\n'.
 *         0 - EOF.
 */
char *get_line(void) {
  FILE *f;
  int c;
  int i;

  /* VM: input from main or include file */
NextLine:
  f = inc_file ? inc_file : input_file;
  i = 0;

  if (saw_eof || (c = getc(f)) == EOF)
  {
    /* VM: end of include file */
    if (inc_file)
    {
      if (fclose(inc_file))
      {
         perror("get_line: fclose");
         abort();
      }

      inc_file = NULL;
      lineno = inc_save_lineno;
      goto NextLine;
    }

    FREE(line);
    saw_eof = 1;
    return line = cptr = 0;
  }
  if (line == 0 || linesize != (LINESIZE + 1))
  {
    FREE(line);
    linesize = LINESIZE + 1;
    line = MALLOC(linesize);
    if (!line) no_space();
  }
  ++lineno;
  while ((line[i] = c) != '\n')
  {
    if (++i + 1 >= linesize)
    {
      line = REALLOC(line, linesize += LINESIZE, line[0]);
      if (!line)
        no_space();
    }
    if ((c = getc(f)) == EOF)
    {
      c = '\n';
      saw_eof = 1;
    }
  }
  line[i+1] = 0;

  /* VM: process %ifdef line */
  if (strncmp(&line[0], "%ifdef ", 7) == 0)
  {
    char var_name[80];
    int ii = 0;
    char **ps;

    for (i = 7; line[i] != '\n' && line[i] != ' '; ++i, ++ii)
    {
      var_name[ii] = line[i];
    }
    var_name[ii] = 0;
    if (in_ifdef)
    {
      error(lineno, 0, 0, "Cannot have nested %%ifdef");
    }
    /* Find the preprocessor variable */
    for (ps = &defd_vars[0]; *ps; ++ps)
    {
      if (strcmp(*ps, var_name) == 0)
      {
        break;
      }
    }
    in_ifdef = 1;
    if (*ps)
    {
      ifdef_skip = 0;
    }
    else
    {
      ifdef_skip = 1;
    }
    goto NextLine;
  }

  /* VM: process %endif line */
  if (strncmp(&line[0], "%endif", 6) == 0)
  {
    if (!in_ifdef)
    {
      error(lineno, 0, 0, "There is no corresponding %%ifdef for %%endif");
    }
    in_ifdef = 0;
    goto NextLine;
  }

  /* VM: skip ordinary lines if ordered by %endif */
  if (in_ifdef && ifdef_skip)
  {
    goto NextLine;
  }

  /* VM: Process %include line */
  if (strncmp(&line[0], "%include ", 9) == 0)
  {
    int ii = 0;

    for (i = 9; line[i] != '\n' && line[i] != ' '; ++i, ++ii)
    {
      inc_file_name[ii] = line[i];
    }
    inc_file_name[ii] = 0;
    if (inc_file)
    {
      error(lineno, 0, 0, "Nested include lines are not allowed");
    }
    inc_file = fopen(inc_file_name, "r");
    if (!inc_file)
    {
      error(lineno, 0, 0, "Cannot open include file %s", inc_file_name);
    }
    inc_save_lineno = lineno;
    lineno = 0;
    goto NextLine;
  }

  /* VM: process %define line */
  if (strncmp(&line[0], "%define ", 8) == 0)
  {
    char var_name[80];
    int ii = 0;
    char **ps;
    for (i = 8; line[i] != '\n' && line[i] != ' '; ++i, ++ii) {
      var_name[ii] = line[i];
    }
    var_name[ii] = 0;
    /* Find the preprocessor variable */
    for (ps = &defd_vars[0]; *ps; ++ps)
    {
      if (strcmp(*ps, var_name) == 0)
      {
        error(lineno, 0, 0, "Preprocessor variable %s already defined", var_name);
      }
    }
    *ps = STRDUP(var_name);
    if (*ps == 0)
       no_space();

    *++ps = NULL;
    goto NextLine;
  }

  if (Eflag)
  {
    printf("YPP: %s", line);
  }

  return cptr = line;
}

char *dup_line(void)
{
    register char *p, *s, *t;

    if (line == 0) return (0);
    s = line;
    while (*s != '\n')
        ++s;
    p = MALLOC(s - line + 1);
    if (!p) no_space();
    s = line;
    t = p;
    while ((*t++ = *s++) != '\n')
        ;
    return (p);
}

char *skip_comment(void)
{
    register char *s;

    int unsigned st_lineno = lineno;
    char *st_line = dup_line();
    char *st_cptr = st_line + (cptr - line);

    s = cptr + 2;
    while (s[0] != '*' || s[1] != '/')
    {
        if (*s == '\n')
        {
            if ((s = get_line()) == 0)
                unterminated_comment(st_lineno, st_line, st_cptr);
        }
        else
            ++s;
    }
    FREE(st_line);
    return cptr = s + 2;
}

int nextc(int only_skip_whitespace)
{
    register char *s;

    if (line == 0 && get_line() == 0)
        return (EOF);
    s = cptr;
    for (;;)
    {
        switch (*s)
        {
        case '\n':
            if ((s = get_line()) == 0) return EOF;
            break;
        case ' ':
        case '\t':
        case '\f':
        case '\r':
        case '\v':
            ++s;
            break;
        case '\\':
            cptr = s;
            return ('%');
        case '/':
            if (s[1] == '*')
            {
                cptr = s;
                s = skip_comment();
                break;
            }
            else if (s[1] == '/')
            {
                if ((s = get_line()) == 0) return EOF;
                break;
            }

            if (0 /* fall through */)
            {
        case ',':
        case ';':
                if (!only_skip_whitespace)
                {
                    ++s;
                    break;
                }
            }
        default:
            cptr = s;
            return *s;
        }
    }
}

static struct keyword {
    const char *name;
    BtYacc_keyword_code token;
} keywords[] =
{
    { "binary", NONASSOC },
    { "debug", BISON_DEBUG },                         /* [i_a] bison emulation additions */
    { "define", BISON_DEFINE },                       /* [i_a] bison emulation additions */
    { "defines", BISON_DEFINES },                     /* [i_a] bison emulation additions */
    { "destructor", BISON_DESTRUCTOR },               /* [i_a] bison emulation additions */
    { "dprec", DPREC_PRIO },                          /* %dprec N: assign explicit precedence number N to rule X */
    { "error-verbose", BISON_ERR_VERBOSE },           /* [i_a] bison emulation additions */
    { "error_verbose", BISON_ERR_VERBOSE },           /* [i_a] bison emulation additions */
    { "expect", BISON_EXPECT },                       /* [i_a] bison emulation additions */
    { "file-prefix", BISON_FILE_PREFIX },             /* [i_a] bison emulation additions */
    { "file_prefix", BISON_FILE_PREFIX },             /* [i_a] bison emulation additions */
    { "fixed-output-files", BISON_YACC },             /* [i_a] bison emulation additions */
    { "fixed_output_files", BISON_YACC },             /* [i_a] bison emulation additions */
    { "glr-parser", BISON_GLR_PARSER },               /* [i_a] bison emulation additions */
    { "glr_parser", BISON_GLR_PARSER },               /* [i_a] bison emulation additions */
    { "ident", IDENT },
    { "left", LEFT },
    { "lex-param", BISON_LEX_PARAM },                 /* [i_a] bison emulation additions */
    { "lex_param", BISON_LEX_PARAM },                 /* [i_a] bison emulation additions */
    { "locations", BISON_LOCATIONS },                 /* [i_a] bison emulation additions */
    { "merge", BISON_MERGE },                         /* [i_a] bison emulation additions */
    { "name-prefix", BISON_NAME_PREFIX },             /* [i_a] bison emulation additions */
    { "name_prefix", BISON_NAME_PREFIX },             /* [i_a] bison emulation additions */
    { "no-lines", BISON_NO_LINES },                   /* [i_a] bison emulation additions */
    { "no_lines", BISON_NO_LINES },                   /* [i_a] bison emulation additions */
    { "nonassoc", NONASSOC },
    { "nterm", NTERM },                               /* [i_a] bison emulation additions */
    { "output", BISON_OUTPUT },                       /* [i_a] bison emulation additions */
    { "parse-param", BISON_PARSE_PARAM },             /* [i_a] bison emulation additions */
    { "prec", PREC },                                 /* %prec TOKEN: declare a token with precedence according to order of appearance;
                                                         when used after a rule, it is like %dprec, but referring to terminal TOKEN for association/precedence data */
    { "prefer", PREFER },                             /* %prefer: like %dprec, similar to the %prefer mentioned in the Anna Bekkerman thesis:
                                                         assigns precedence to a rule; later rules get higher precedence. */
    { "printer", BISON_PRINTER },                     /* [i_a] bison emulation additions */
    { "pure-parser", BISON_PURE },                    /* [i_a] bison emulation additions */
    { "pure_parser", BISON_PURE },                    /* [i_a] bison emulation additions */
    { "right", RIGHT },
    { "scannerless", SCANNERLESS_ZERO_ASCII },
    { "skeleton", BISON_SKELETON },                   /* [i_a] bison emulation additions */
    { "start", START },
    { "term", TOKEN },
    { "token", TOKEN },
    { "token-table", BISON_TOKEN_TABLE },             /* [i_a] bison emulation additions */
    { "type", TYPE },
    { "union", UNION },
    { "verbose", BISON_VERBOSE },                     /* [i_a] bison emulation additions */
    { "yacc", BISON_YACC },                           /* [i_a] bison emulation additions */
};

const char *get_keyword_as_string(BtYacc_keyword_code a)
{
    int i;

    for (i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++)
    {
        if (keywords[i].token == a)
            return keywords[i].name;
    }
    return NULL;
}

static int search_strcmp(void const * key, void const * element)
{
    const struct keyword *kwd = (const struct keyword *)element;

    return strcmp((const char *)key, kwd->name);
}

BtYacc_keyword_code keyword(int fail_when_no_keyword)
{
  int c;
  char *t_cptr = cptr;
  const struct keyword *key;

  c = *++cptr;
  if (isalpha(c))
  {
    cinc = 0;
    while (isalnum(c) || c == '_' || c == '-' || c == '.' || c == '$')
    {
      cachec(tolower(c));
      c = *++cptr;
    }
    cachec(NUL);

    key = (const struct keyword *)bsearch(cache, keywords, sizeof(keywords)/sizeof(keywords[0]),
                       sizeof(*key), search_strcmp);
    if (key)
        return key->token;
  }
  else
  {
    ++cptr;
    if (c == '{') return TEXT;
    if (c == '%' || c == '\\') return MARK;
    if (c == '<') return LEFT;
    if (c == '>') return RIGHT;
    if (c == '0') return TOKEN;
    if (c == '2') return NONASSOC;
  }
  if (fail_when_no_keyword)
  {
    syntax_error_ex(lineno, line, t_cptr, "expected a keyword or one of these: [ %%{  %%%%  %%\\  %%<  %%>  %%0  %%2 ]");
  }
  return NOT_A_KEYWORD;
}

void copy_ident(void)
{
    register int c;

    open_output_files();

    if ((c = nextc(0)) == EOF) unexpected_EOF();
    if (c != '"') syntax_error_ex(lineno, line, cptr, "expected a quoted #indent string");
    ++outline[OUTPUT_FILE];
    BtYacc_puts("#ident \"", output_file);

    for (;;)
    {
        c = *++cptr;
        if (c == '\n')
        {
            BtYacc_puts("\"\n", output_file);
            return;
        }

        BtYacc_putc(c, output_file);

        if (c == '"')
        {
            BtYacc_putc('\n', output_file);
            ++cptr;
            return;
        }
    }
}

#define OUTC(c) do { /* output a character on f1 and f2, if non-null */ \
    int _c = (c);                                                       \
    if (putc(_c, f1) == EOF) { perror("putc"); abort(); }               \
    if (f2 && (putc(_c, f2) == EOF) ) { perror("putc"); abort(); }      \
    } while(0)

void copy_string(int quote, FILE *f1, FILE *f2)
{
register int    c;
int unsigned    s_lineno = lineno;
char            *s_line = dup_line();
char            *s_cptr = s_line + (cptr - line - 1);

    for (;;)
    {
        OUTC(c = *cptr++);
        if (c == quote)
        {
            FREE(s_line);
            return;
        }
        if (c == '\n')
            unterminated_string(s_lineno, s_line, s_cptr);
        if (c == '\\')
        {
            OUTC(c = *cptr++);
            if (c == '\n')
            {
                if (get_line() == 0)
                    unterminated_string(s_lineno, s_line, s_cptr);
            }
        }
    }
}

void copy_comment(FILE *f1, FILE *f2)
{
register int    c;

    if ((c = *cptr) == '/')
    {
        OUTC('*');
        while ((c = *++cptr) != '\n')
        {
            OUTC(c);
            if (c == '*' && cptr[1] == '/')
                OUTC(' ');
        }
        OUTC('*');
        OUTC('/');
    }
    else if (c == '*')
    {
        int unsigned c_lineno = lineno;
        char *c_line = dup_line();
        char *c_cptr = c_line + (cptr - line - 1);
        OUTC(c);
        ++cptr;
        while ((c = cptr[0]) != '*' || cptr[1] != '/')
        {
            OUTC(c);
            ++cptr;
            if (c == '\n')
            {
                if (get_line() == 0)
                    unterminated_comment(c_lineno, c_line, c_cptr);
            }
        }
        OUTC(c);
        OUTC('/');
        FREE(c_line);
        cptr += 2;
    }
}

#undef OUTC

void copy_text(void)
{
    register int c;
    register FILE *f;
    int need_newline = 0;
    int unsigned t_lineno = lineno;
    char *t_line = dup_line();
    char *t_cptr = t_line + (cptr - line - 2);

    open_temporary_files();
    f = text_file;

    if (*cptr == '\n')
    {
        if (get_line() == 0)
            unterminated_text(t_lineno, t_line, t_cptr);
    }

    if (!lflag)
       BtYacc_printf(f, line_format, lineno - 1, (inc_file ? inc_file_name : input_file_name));

loop:
    switch (c = *cptr++)
    {
    case '\n':
        BtYacc_putc('\n', f);
        need_newline = 0;
        if (get_line()) goto loop;
        unterminated_text(t_lineno, t_line, t_cptr);
    case '\'':
    case '"':
        BtYacc_putc(c, f);
        copy_string(c, f, 0);
        need_newline = 1;
        goto loop;
    case '/':
        BtYacc_putc(c, f);
        copy_comment(f, 0);
        need_newline = 1;
        goto loop;
    case '%':
    case '\\':
        if (*cptr == '}')
        {
            if (need_newline) BtYacc_putc('\n', f);
            ++cptr;
            FREE(t_line);
            return;
        }
        /* fall through */
    default:
        BtYacc_putc(c, f);
        need_newline = 1;
        goto loop;
      }
}

void copy_union(void)
{
    FILE *dc_file;
    register int c;
    int depth;
    int unsigned u_lineno = lineno;
    char *u_line = dup_line();
    char *u_cptr = u_line + (cptr - line - 6);

    open_output_files();

    if (unionized) over_unionized(cptr - 6);
    unionized = 1;

    /* VM: Print to either code file or defines file but not to both */
    dc_file = dflag ? union_file : text_file;

    if (!lflag)
        BtYacc_printf(dc_file, line_format, lineno - 1, (inc_file ? inc_file_name : input_file_name));

    BtYacc_printf(dc_file, get_section("union_decl_start"));

    depth = 0;
    while ((c = *cptr++))
    {
        BtYacc_putc(c, dc_file);

        switch (c) {
        case '\n':
          get_line();
          if (line == 0) unterminated_union(u_lineno, u_line, u_cptr);
          continue;
        case '{':
          ++depth;
          continue;
        case '}':
          if (--depth == 0)
          {
            BtYacc_printf(dc_file, get_section("union_decl_end"));
            FREE(u_line);
            break;
          }
          continue;
        case '\'':
        case '"':
          copy_string(c, dc_file, 0);
          continue;
        case '/':
          copy_comment(dc_file, 0);
          continue;
        default:
          continue;
        }
        break;
    }
}

int hexval(int c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    return (-1);
}

bucket *get_literal(int store_literal)
{
    register int c, quote;
    register int i;
    register int n;
    register char *s;
    register bucket *bp;
    int unsigned s_lineno = lineno;
    char *s_line = dup_line();
    char *s_cptr = s_line + (cptr - line);

    quote = *cptr++;
    cinc = 0;
    for (;;)
    {
        c = *cptr++;
        if (c == quote) break;
        if (c == '\n') unterminated_string(s_lineno, s_line, s_cptr);
        if (c == '\\')
        {
            char *c_cptr = cptr - 1;
            c = *cptr++;
            switch (c)
            {
            case '\n':
                get_line();
                if (line == 0) unterminated_string(s_lineno, s_line, s_cptr);
                continue;
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
                n = c - '0';
                c = *cptr;
                if (IS_OCTAL(c))
                {
                    n = (n << 3) + (c - '0');
                    c = *++cptr;
                    if (IS_OCTAL(c))
                    {
                        n = (n << 3) + (c - '0');
                        ++cptr;
                    }
                }
                if (n > MAXCHAR) illegal_character(c_cptr);
                c = n;
                break;
            case 'x':
                c = *cptr++;
                n = hexval(c);
                if (n < 0 || n >= 16)
                    illegal_character(c_cptr);
                for (;;)
                {
                    c = *cptr;
                    i = hexval(c);
                    if (i < 0 || i >= 16) break;
                    ++cptr;
                    n = (n << 4) + i;
                    if (n > MAXCHAR) illegal_character(c_cptr);
                }
                c = n;
                break;
            case 'a': c = 7; break;
            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 'n': c = '\n'; break;
            case 'r': c = '\r'; break;
            case 't': c = '\t'; break;
            case 'v': c = '\v'; break;
            }
        }
        cachec(c);
    }
    FREE(s_line);

    n = cinc;
    s = MALLOC(n);
    if (s == 0) no_space();

    for (i = 0; i < n; ++i)
        s[i] = cache[i];

    cinc = 0;
    if (n == 1)
        cachec('\'');
    else
        cachec('"');

    for (i = 0; i < n; ++i)
    {
        c = ((unsigned char *)s)[i];
        if (c == '\\' || c == cache[0])
        {
            cachec('\\');
            cachec(c);
        }
        else if (isprint(c))
            cachec(c);
        else
        {
            cachec('\\');
            switch (c)
            {
            case 7: cachec('a'); break;
            case '\b': cachec('b'); break;
            case '\f': cachec('f'); break;
            case '\n': cachec('n'); break;
            case '\r': cachec('r'); break;
            case '\t': cachec('t'); break;
            case '\v': cachec('v'); break;
            default:
                cachec(((c >> 6) & 7) + '0');
                cachec(((c >> 3) & 7) + '0');
                cachec((c & 7) + '0');
                break;
            }
        }
    }
    if (n == 1)
        cachec('\'');
    else
        cachec('"');

    cachec(NUL);
    if (!store_literal)
    {
        bp = make_bucket(cache);
    }
    else
    {
        bp = lookup(cache);
        bp->symbol_class = TERM;
        if (n == 1 && bp->value == UNDEFINED)
            bp->value = *(unsigned char *)s;
    }
    FREE(s);

    return (bp);
}

int is_reserved(char const * name)
{
    if (strcmp(name, ".") == 0 ||
        strcmp(name, "$accept") == 0 ||
        strcmp(name, "$end") == 0)
        return (1);

    if (name[0] == '$' && name[1] == '$' && isdigit(name[2]))
    {
        char const * s = name + 3;

        while (isdigit(*s)) ++s;

        if (*s == NUL)
            return (1);
    }

    return (0);
}

bucket *get_name(int store_name)
{
    register int c;

    cinc = 0;
    for (c = *cptr; IS_IDENT(c); c = *++cptr)
        cachec(c);
    cachec(NUL);
    if (!store_name)
    {
        return make_bucket(cache);
    }
    else
    {
        if (is_reserved(cache)) used_reserved(cache);
        return lookup(cache);
    }
}

/* Return (possibly negative) integer stored a cptr. On exit, cptr
   points to first character after integer. */
int get_number(void)
{
  int n = 0;
  char c, first = *cptr;

  if (first == '-')
    ++cptr;
  for (c = *cptr; isdigit(c); c = *++cptr)
    n = 10 * n + (c - '0');
  if (first == '-')
    n = -n;

  return n;
}

//
// Date: Mon, 29 Jun 1998 16:36:47 +0200
// From: Matthias Meixner <meixner@mes.th-darmstadt.de>
// Organization: TH Darmstadt, Mikroelektronische Systeme
//
// While using your version of BTYacc (V2.1), I have found a bug.
// It does not correctly
// handle typenames, if one typename is a prefix of another one and
// if this type is used after the longer one. In this case BTYacc
// produces invalid code.
//
// e.g. in:
// --------------------------------------------
// %{
//
// #include <stdlib.h>
//
//    struct List {
//       struct List *next;
//       int foo;
//    };
//
// %}
// %union {
//    struct List *fooList;
//    int foo;
// }
//
// %type <fooList> a
// %type <foo> b
//
// %token <foo> A
//
// %%
//
// a: b   {$$=malloc(sizeof(*$$));$$->next=NULL;$$->foo=$1;}
//  | a b {$$=malloc(sizeof(*$$));$$->next=$1;$$->foo=$2;}
//
// b: A {$$=$1;}
//
static char *cache_tag(char *tag, int len)
{
int     i;
char    *s;

    for (i = 0; i < ntags; ++i)
    {
        if (strncmp(tag, tag_table[i], len) == 0 &&
            // VM: this is bug fix proposed by Matthias Meixner
            tag_table[i][len] == 0)
        {
            return (tag_table[i]);
        }
    }
    if (ntags >= tagmax)
    {
        tagmax += 16;
        tag_table = tag_table ? RENEW(tag_table, tagmax, tag_table[0])
                              : NEW2(tagmax, tag_table[0]);
        if (tag_table == 0) no_space();
    }
    s = MALLOC(len + 1);
    if (s == 0) no_space();
    strncpy(s, tag, len);
    s[len] = 0;
    tag_table[ntags++] = s;
    return s;
}

char *get_tag(void)
{
    register int c;
    int unsigned t_lineno = lineno;
    char *t_line = dup_line();
    char *t_cptr = t_line + (cptr - line);

    ++cptr;
    c = nextc(0);
    if (c == EOF) unexpected_EOF();
    if (!isalpha(c) && c != '_' && c != '$')
        illegal_tag(t_lineno, t_line, t_cptr);

    cinc = 0;
    do {
        cachec(c);
        c = *++cptr;
    } while (IS_IDENT(c));

    c = nextc(0);
    if (c == EOF) unexpected_EOF();
    if (c != '>')
        illegal_tag(t_lineno, t_line, t_cptr);
    ++cptr;

    FREE(t_line);
    havetags = 1;
    return cache_tag(cache, cinc);
}

static char *scan_id(void)
{
char    *b = cptr;

    while (isalnum(*cptr) || *cptr == '_' || *cptr == '$')
        ++cptr;

    return cache_tag(b, (int)(cptr - b));
}

void declare_tokens(BtYacc_keyword_code assoc)
{
    register int c;
    register bucket *bp;
    int value;
    char *tag = 0;

    if (assoc != TOKEN) ++precedence_max;

    c = nextc(0);
    if (c == EOF) unexpected_EOF();
    if (c == '<')
    {
        tag = get_tag();
        c = nextc(0);
        if (c == EOF) unexpected_EOF();
    }

    for (;;)
    {
        if (isalpha(c) || c == '_' || c == '.' || c == '$')
            bp = get_name(1);
        else if (c == '\'' || c == '"')
            bp = get_literal(1);
        else
            return;

        if (bp == goal) tokenized_start(bp->name);
        bp->symbol_class = TERM;

        if (tag)
        {
            if (bp->tag && tag != bp->tag)
                retyped_warning(bp->name);
            bp->tag = tag;
        }

        if (assoc != TOKEN)
        {
            if (bp->prec > 0 && precedence_max != bp->prec)
                reprec_warning(bp->name);
            bp->assoc = assoc;
            bp->prec = precedence_max;
        }

        c = nextc(0);
        if (c == EOF) unexpected_EOF();
        value = UNDEFINED;
        if (isdigit(c))
        {
            value = get_number();
            if (bp->value != UNDEFINED && value != bp->value)
                revalued_warning(bp->name);
            bp->value = value;
            c = nextc(0);
            if (c == EOF) unexpected_EOF();
        }
    }
}

static void declare_argtypes(bucket *bp)
{
char    *tags[MAXARGS];
int     args = 0, c;

    if (bp->args >= 0) retyped_warning(bp->name);

    ++cptr; /* skip open paren */

    for (;;)
    {
        c = nextc(0);
        if (c == EOF) unexpected_EOF();
        if (c != '<') syntax_error_ex(lineno, line, cptr, "expected an <argument_type>");
        tags[args++] = get_tag();
        c = nextc(0);
        if (c == ')') break;
        if (c == EOF) unexpected_EOF();
    }

    ++cptr; /* skip close paren */
    bp->args = args;
    bp->argnames = NEW2(args, bp->argnames[0]);
    bp->argtags = NEW2(args, bp->argtags[0]);
    if (!bp->argnames) no_space();
    if (!bp->argtags) no_space();
    while (--args >= 0)
    {
        bp->argtags[args] = tags[args];
        bp->argnames[args] = 0;
    }
}

void declare_types(void)
{
    register int c;
    register bucket *bp=0;
    char *tag=0;

    c = nextc(0);
    if (c == '<')
    {
        tag = get_tag();
        c = nextc(0);
    }
    if (c == EOF) unexpected_EOF();

    for (;;)
    {
        c = nextc(0);
        if (isalpha(c) || c == '_' || c == '.' || c == '$')
        {
            bp = get_name(1);
            if (nextc(0) == '(')
                declare_argtypes(bp);
            else
                bp->args = 0;
        }
        else if (c == '\'' || c == '"')
        {
            bp = get_literal(1);
            bp->args = 0;
        }
        else
        {
            return;
        }

        if (tag)
        {
            if (bp->tag && tag != bp->tag)
                retyped_warning(bp->name);
            bp->tag = tag;
        }
    }
}

void declare_start(void)
{
    register int c;
    register bucket *bp;

    c = nextc(0);
    if (c == EOF) unexpected_EOF();
    if (!isalpha(c) && c != '_' && c != '.' && c != '$')
        syntax_error_ex(lineno, line, cptr, "expected a %%start terminal token or non-terminal");
    bp = get_name(1);
    if (bp->symbol_class == TERM)
        terminal_start(bp->name);
    if (goal && goal != bp)
        restarted_warning();
    goal = bp;
}

void read_declarations(void)
{
    register int c;
    BtYacc_keyword_code k;
    char *t_cptr;

    cache_size = 256;
    cache = (char *)NEW2(cache_size, cache[0]);
    if (cache == 0) no_space();

    for (;;)
    {
        c = nextc(0);
        if (c == EOF)
            unexpected_EOF();
        if (c != '%')
            syntax_error_ex(lineno, line, cptr, "expected a %%-prefixed declaration command, e.g. %%token, %%left, %%right, %%ident, %%{ ... %%}, %%union");
        t_cptr = cptr;
        switch (k = keyword(1))
        {
        case MARK:
            return;
        case IDENT:
            copy_ident();
            break;
        case TEXT:
            copy_text();
            break;
        case UNION:
            copy_union();
            break;
        case TOKEN:
        case LEFT:
        case RIGHT:
        case NONASSOC:
        case PREC:
            declare_tokens(k);
            break;
        case TYPE:
            declare_types();
            break;
        case START:
            declare_start();
            break;
        case BISON_NO_LINES:
            lflag = 1;
            break;
        case BISON_VERBOSE:
            if (!vflag) vflag = 1;
            break;
        case BISON_DEFINES:
            dflag = 1;
            break;
        case SCANNERLESS_ZERO_ASCII:
            ZEROflag = 1;
            break;
        case BISON_DEBUG:
            if (!tflag) tflag = 1;
            break;
        case BISON_NAME_PREFIX:
            {
                bucket *bp;
                char *prefix;

                /* = prefix */
                c = nextc(0);
                if (c == '=')
                {
                    cptr++;
                    c = nextc(0);
                }
                if (c == '\'' || c == '"')
                {
                    bp = get_literal(0);
                    /* strip quotes */
                    prefix = STRDUP(bp->name + 1);
                    prefix[strlen(prefix)-1] = 0;
                }
                else
                {
                    bp = get_name(0);
                    prefix = STRDUP(bp->name);
                }
                FREE(bp);

                if (prefix && *prefix)
                {
                    name_prefix = sanitize_to_varname(prefix);
                    name_uc_prefix = STRDUP(prefix);
                    strupper(name_uc_prefix);
                }
                else
                {
                     error(lineno, line, cptr, "syntax error: not a valid name prefix following %%name-prefix");
                }
            }
            break;
        case BISON_FILE_PREFIX:
            {
                bucket *bp;
                char *prefix;

                /* = prefix */
                c = nextc(0);
                if (c == '=')
                {
                    cptr++;
                    c = nextc(0);
                }
                if (c == '\'' || c == '"')
                {
                    bp = get_literal(0);
                    /* strip quotes */
                    prefix = STRDUP(bp->name + 1);
                    prefix[strlen(prefix)-1] = 0;
                }
                else
                {
                    bp = get_name(0);
                    prefix = STRDUP(bp->name);
                }
                FREE(bp);

                if (prefix && *prefix)
                {
                    file_prefix = prefix;
                    file_uc_prefix = sanitize_to_varname(file_prefix);
                    strupper(file_uc_prefix);
                }
                else
                {
                     error(lineno, line, cptr, "syntax error: not a valid file prefix following %%file-prefix");
                }
            }
            break;
        case BISON_DEFINE:
#if 0
            {
                char **ps;
                char *var_name = s + 1;

                for (ps = &defd_vars[0]; *ps; ps++)
                {
                    if(strcmp(*ps,var_name) == 0)
                    {
                        error(lineno, line, t_cptr, "Preprocessor variable %s already defined", var_name);
                    }
                }
                *ps = STRDUP(var_name);
                if (!*ps) no_space();
                *++ps = NULL;
            }
            break;
#endif
        default:
            unsupported_feature(lineno, line, t_cptr);
            break;
        }
    }
}

void initialize_grammar(void)
{
    nitems = 4;
    maxitems = 300;
    pitem = NEW2(maxitems, pitem[0]);
    if (pitem == 0) no_space();
    pitem[0] = 0;
    pitem[1] = 0;
    pitem[2] = 0;
    pitem[3] = 0;

    nrules = 3;
    maxrules = 100;
    plhs = NEW2(maxrules, plhs[0]);
    if (plhs == 0) no_space();
    plhs[0] = 0;
    plhs[1] = 0;
    plhs[2] = 0;
    rprec = NEW2(maxrules, rprec[0]);
    if (rprec == 0) no_space();
    rprec[0] = 0;
    rprec[1] = 0;
    rprec[2] = 0;
    rassoc = NEW2(maxrules, rassoc[0]);
    if (rassoc == 0) no_space();
    rassoc[0] = TOKEN;
    rassoc[1] = TOKEN;
    rassoc[2] = TOKEN;
}

void expand_items(void)
{
    maxitems += 300;
    pitem = RENEW(pitem, maxitems, pitem[0]);
    if (pitem == 0) no_space();
}

void expand_rules(void)
{
    maxrules += 100;
    plhs = RENEW(plhs, maxrules, plhs[0]);
    if (plhs == 0) no_space();
    rprec = RENEW(rprec, maxrules, rprec[0]);
    if (rprec == 0) no_space();
    rassoc = RENEW(rassoc, maxrules, rassoc[0]);
    if (rassoc == 0) no_space();
}

/* set in copy_args and incremented by the various routines that will rescan
** the argument list as appropriate */
static int unsigned rescan_lineno;

static char *copy_args(int *alen)
{
struct mstring  *s = msnew();
int             depth = 0, len = 1, c;
char            quote = 0;
int unsigned    a_lineno = lineno;
char            *a_line = dup_line();
char            *a_cptr = a_line + (cptr - line - 1);

    rescan_lineno = lineno;
    while ((c = *cptr++) != ')' || depth || quote)
    {
        if (c == ',' && !quote && !depth)
        {
            ++len;
            mputc(s, 0);
            continue;
        }
        mputc(s, c);
        if (c == '\n')
        {
            get_line();
            if (!line)
            {
                if (quote)
                    unterminated_string(a_lineno, a_line, a_cptr);
                else
                    unterminated_arglist(a_lineno, a_line, a_cptr);
            }
        }
        else if (quote)
        {
            if (c == quote)
            {
                quote = 0;
            }
            else if (c == '\\')
            {
                if (*cptr != '\n')
                {
                    mputc(s, *cptr++);
                }
            }
        }
        else
        {
            if (c == '(')
                ++depth;
            else if (c == ')')
                --depth;
            else if (c == '\"' || c == '\'')
                quote = c;
        }
    }
    if (alen)
        *alen = len;
    FREE(a_line);
    return msdone(s);
}

static char *parse_id(char *p, char **save)
{
char    *b;

    while (isspace(*p))
    {
        if (*p++ == '\n')
            ++rescan_lineno;
    }

    if (!isalpha(*p) && *p != '_') return 0;
    b = p;
    while (isalnum(*p) || *p == '_' || *p == '$')
        ++p;

    if (save)
    {
        *save = cache_tag(b, (int)(p - b));
    }
    return p;
}

static char *parse_int(char *p, int *save)
{
    int neg = 0, val = 0;

    while (isspace(*p))
    {
        if (*p++ == '\n')
            ++rescan_lineno;
    }

    if (*p == '-')
    {
        neg = 1;
        ++p;
    }

    if (!isdigit(*p)) return 0;
    while (isdigit(*p))
    {
        val = val * 10 + *p++ - '0';
    }
    if (neg) val = -val;
    if (save) *save = val;
    return p;
}

static void parse_arginfo(bucket *a, char *args, int argslen)
{
char    *p = args, *tmp;
int     i, redec = 0;

    if (a->args >= 0)
    {
        if (a->args != argslen)
        {
            error(rescan_lineno, 0, 0, "number of arguments of %s does't "
                  "agree with previous declaration", a->name);
        }
        redec = 1;
    }
    else
    {
        a->args = argslen;
        if (!a->args) return;
        a->argnames = NEW2(argslen, a->argnames[0]);
        a->argtags = NEW2(argslen, a->argtags[0]);
        if (!a->argnames || !a->argtags)
            no_space();
    }
    if (!args) return;

    for (i = 0; i < argslen; ++i)
    {
        while (isspace(*p)) if (*p++ == '\n') ++rescan_lineno;

        if (*p++ != '$') bad_formals();

        while (isspace(*p)) if (*p++ == '\n') ++rescan_lineno;

        if (*p == '<')
        {
            havetags = 1;
            p = parse_id(p+1, &tmp);
            if (!p) bad_formals();

            while (isspace(*p)) if (*p++ == '\n') ++rescan_lineno;

            if (*p++ != '>') bad_formals();
            if (redec)
            {
                if (a->argtags[i] != tmp)
                {
                    error(rescan_lineno, 0, 0, "type of argument %d to %s "
                          "doesn't agree with previous declaration", i+1,
                          a->name);
                }
            }
            else
            {
                a->argtags[i] = tmp;
            }
        }
        else if (!redec)
        {
            a->argtags[i] = 0;
        }

        p = parse_id(p, &a->argnames[i]);
        if (!p) bad_formals();

        while (isspace(*p))
        {
            if (*p++ == '\n')
                ++rescan_lineno;
        }

        if (*p++) bad_formals();
    }
}

static char *compile_arg(char **theptr, char *yyvaltag)
{
    char            *p = *theptr;
    struct mstring  *c = msnew();
    int             i, j, n;
    Yshort          *offsets = 0, maxoffset;
    bucket          **rhs;

    maxoffset = n = 0;
    for (i = nitems - 1; pitem[i]; --i)
    {
        ++n;

        if (pitem[i]->symbol_class != ARGUMENT)
           ++maxoffset;
    }

    if (maxoffset > 0)
    {
        offsets = NEW2(maxoffset+1, offsets[0]);
        if (offsets == 0) no_space();
    }

    for (j = 0, ++i; i < nitems; ++i)
    {
        if (pitem[i]->symbol_class != ARGUMENT)
            offsets[++j] = i - nitems + 1;
    }
    rhs = pitem + nitems - 1;

    if (yyvaltag)
      msprintf(c, get_section("yyval.tag_reference"), yyvaltag);
    else
      msprintf(c, get_section("yyval_reference"));
    msprintf(c, " = ");
    while (*p)
    {
      if (*p == '$')
      {
        char *tag = 0;
        if (*++p == '<')
        {
          p = parse_id(++p, &tag);
          if (!p || *p++ != '>')
            illegal_tag(rescan_lineno, 0, 0);
        }
        if (isdigit(*p) || *p == '-')
        {
          int val = 0;
          p = parse_int(p, &val);
          if (!p)
            dollar_error(rescan_lineno, 0, 0);
          if (val <= 0)
          {
            i = val - n;
          }
          else if (val > maxoffset)
          {
            dollar_warning(rescan_lineno, val);
            i = val - maxoffset;
          }
          else
          {
            i = offsets[val];
            if (!tag && !(tag = rhs[i]->tag) && havetags)
              untyped_rhs(val, rhs[i]->name);
          }
          if (tag)
          {
            msprintf(c, get_section("yyvsp.tag_reference"), i, tag);
          }
          else
          {
            if (havetags)
              unknown_rhs(val);
            msprintf(c, get_section("yyvsp_reference"), i);
          }
        }
        else if (isalpha(*p) || *p == '_')
        {
          char  *arg;
          p = parse_id(p, &arg);
          if (!p)
            dollar_error(rescan_lineno, 0, 0);

          for (i = plhs[nrules]->args - 1; i >= 0; --i)
          {
            if (arg == plhs[nrules]->argnames[i])
              break;
          }
          if (i < 0)
            error(rescan_lineno, 0, 0, "unknown argument $%s", arg);
          if (!tag)
            tag = plhs[nrules]->argtags[i];
          if (tag)
            msprintf(c, get_section("yyvsp.tag_reference"), i - plhs[nrules]->args + 1 - n, tag);
          else
          {
            if (havetags)
              error(rescan_lineno, 0, 0, "untyped argument $%s", arg);
            msprintf(c, get_section("yyvsp_reference"), i - plhs[nrules]->args + 1 - n);
          }
        }
        else
          dollar_error(rescan_lineno, 0, 0);
      }
      else
      {
        if (*p == '\n') ++rescan_lineno;

        mputc(c, *p++);
      }
    }
    *theptr = p;
    if (maxoffset > 0) FREE(offsets);
    return msdone(c);
}

#define ARG_CACHE_SIZE  1024
static struct arg_cache {
    struct arg_cache    *next;
    char                *code;
    int                 rule;
} *arg_cache[ARG_CACHE_SIZE];

static int lookup_arg_cache(char *code)
{
struct arg_cache        *entry;

    entry = arg_cache[strnshash(code) % ARG_CACHE_SIZE];
    while (entry)
    {
        if (!strnscmp(entry->code, code)) return entry->rule;
        entry = entry->next;
    }
    return -1;
}

static void insert_arg_cache(char *code, int rule)
{
struct arg_cache        *entry = NEW(entry[0]);
int                     i;

    if (!entry) no_space();
    i = strnshash(code) % ARG_CACHE_SIZE;
    entry->code = code;
    entry->rule = rule;
    entry->next = arg_cache[i];
    arg_cache[i] = entry;
}

static void clean_arg_cache(void)
{
struct arg_cache        *e, *t;
int                     i;

    for (i = 0; i < ARG_CACHE_SIZE; ++i)
    {
        e = arg_cache[i];
        while (e)
        {
            t = e;
            free(e->code);
            e = e->next;
            FREE(t);
        }
        arg_cache[i] = 0;
    }
}

void advance_to_start(void)
{
    register int c;
    register bucket *bp;
    char *s_cptr;
    int unsigned s_lineno;
    char        *args = 0;
    int         argslen = 0;

    for (;;)
    {
        c = nextc(0);
        if (c != '%') break;
        s_cptr = cptr;
        switch (keyword(1))
        {
        case MARK:
            no_grammar();
            /* fall through */
        case TEXT:
            copy_text();
            break;
        case START:
            declare_start();
            break;
        default:
            syntax_error_ex(lineno, line, s_cptr, "expected a %%start, %%{ ... %%} text snippet or a %% mark");
        }
    }

    c = nextc(0);
    if (!isalpha(c) && c != '_' && c != '.' && c != '_')
        syntax_error_ex(lineno, line, cptr, "expected a terminal token or non-terminal");
    bp = get_name(1);
    if (goal == 0)
    {
        if (bp->symbol_class == TERM)
            terminal_start(bp->name);
        goal = bp;
    }

    s_lineno = lineno;
    c = nextc(0);
    if (c == EOF) unexpected_EOF();
    if (c == '(')
    {
        ++cptr;
        args = copy_args(&argslen);
        if (args == 0) no_space();
        c = nextc(0);
    }
    if (c != ':') syntax_error_ex(lineno, line, cptr, "expected a colon ':' starting the rule definition");
    start_rule(bp, s_lineno);
    parse_arginfo(bp, args, argslen);
    FREE(args);
    ++cptr;
}

void start_rule(bucket *bp, int unsigned s_lineno)
{
    if (bp->symbol_class == TERM)
        terminal_lhs(s_lineno);
    bp->symbol_class = NONTERM;
    assert(bp->assoc == TOKEN || bp->assoc == NTERM);
    bp->assoc = NTERM;
    if (!bp->index)
        bp->index = nrules;
    if (nrules >= maxrules)
        expand_rules();
    plhs[nrules] = bp;
    rprec[nrules] = UNDEFINED;
    rassoc[nrules] = NTERM;
}

void end_rule(void)
{
    register int i;

    if (!last_was_action && plhs[nrules]->tag)
    {
        for (i = nitems - 1; pitem[i]; --i)
            continue;
        if (pitem[i+1] == 0 || pitem[i+1]->tag != plhs[nrules]->tag)
            default_action_warning();
    }

    last_was_action = 0;
    if (nitems >= maxitems)
        expand_items();
    pitem[nitems] = 0;
    ++nitems;
    ++nrules;
}

void insert_empty_rule(void)
{
    register bucket *bp, **bpp;

    assert(cache);
    sprintf(cache, "$$%d", ++gensym);
    bp = make_bucket(cache);
    last_symbol->next = bp;
    last_symbol = bp;
    bp->tag = plhs[nrules]->tag;
    bp->symbol_class = ACTION;
    bp->args = 0;

    if ((nitems += 2) > maxitems)
        expand_items();
    bpp = pitem + nitems - 1;
    *bpp-- = bp;
    while ((bpp[0] = bpp[-1]))
        --bpp;

    if (++nrules >= maxrules)
        expand_rules();
    plhs[nrules] = plhs[nrules-1];
    plhs[nrules-1] = bp;
    rprec[nrules] = rprec[nrules-1];
    rprec[nrules-1] = UNDEFINED;
    rassoc[nrules] = rassoc[nrules-1];
    rassoc[nrules-1] = NTERM;
}

static char *insert_arg_rule(char *arg, char *tag)
{
    int     lineno;
    char    *code;
    int     rule;
    FILE    *f;

    open_temporary_files();

    lineno = rescan_lineno;
    code = compile_arg(&arg, tag);
    rule = lookup_arg_cache(code);
    f = action_file;

    if (rule < 0)
    {
        rule = nrules;
        insert_arg_cache(code, rule);
        BtYacc_printf(f, get_section("action_case_start"), rule - 2);
        BtYacc_printf(f, "    %s \x01%u\x1f %s", get_section("comment_start"), rule, get_section("comment_end"));

        if (!lflag)
            BtYacc_printf(f, line_format, lineno - 1, (inc_file ? inc_file_name : input_file_name));

        BtYacc_printf(f, get_section("action_code"), code);

        BtYacc_printf(f, get_section("action_case_end"));
        insert_empty_rule();
        plhs[rule]->tag = tag;
        plhs[rule]->symbol_class = ARGUMENT;
    }
    else
    {
        if (++nitems > maxitems)
            expand_items();
        pitem[nitems-1] = plhs[rule];
        free(code);
    }
    return arg+1;
}

void add_symbol(void)
{
    register int c;
    register bucket *bp;
    int unsigned s_lineno = lineno;
    char *args = 0;
    int argslen = 0;

    c = *cptr;
    if (c == '\'' || c == '"')
        bp = get_literal(1);
    else
        bp = get_name(1);

    c = nextc(0);
    if (c == '(')
    {
        ++cptr;
        args = copy_args(&argslen);
        if (args == 0) no_space();
        c = nextc(0);
    }
    if (c == ':')
    {
        end_rule();
        start_rule(bp, s_lineno);
        parse_arginfo(bp, args, argslen);
        FREE(args);
        ++cptr;
        return;
    }

    if (last_was_action)
        insert_empty_rule();
    last_was_action = 0;

    if (bp->args < 0)
        bp->args = argslen;
    if (argslen == 0 && bp->args > 0 && pitem[nitems-1] == 0)
    {
        int     i;
        if (plhs[nrules]->args != bp->args)
        {
            error(lineno, line, cptr, "Wrong number of default arguments "
                  "for %s", bp->name);
        }

        for (i = bp->args - 1; i >= 0; --i)
        {
            if (plhs[nrules]->argtags[i] != bp->argtags[i])
            {
                error(lineno, line, cptr, "Wrong type for default argument "
                      "%d to %s", i+1, bp->name);
            }
        }
    }
    else if (bp->args != argslen)
    {
        error(lineno, line, cptr, "wrong number of arguments for %s",
                                  bp->name);
    }

    if (args != 0)
    {
        char    *ap;
        int     i;

        for (ap = args, i = 0; i < argslen; ++i)
        {
            ap = insert_arg_rule(ap, bp->argtags[i]);
        }
        FREE(args);
    }

    if (++nitems > maxitems)
        expand_items();
    pitem[nitems-1] = bp;
}

void copy_action(void)
{
    register int c;
    register int i, j, n;
    int depth;
    int trialaction = 0;
    int haveyyval = 0;
    char *tag;
    register FILE *f;
    int unsigned a_lineno = lineno;
    char *a_line = dup_line();
    char *a_cptr = a_line + (cptr - line);
    Yshort *offsets = 0, maxoffset;
    bucket **rhs;

    open_temporary_files();
    f = action_file;

    if (last_was_action)
        insert_empty_rule();
    last_was_action = 1;

    BtYacc_printf(f, get_section("action_case_start"), nrules - 2);
    BtYacc_printf(f, "    %s \x01%u\x1f %s", get_section("comment_start"), nrules, get_section("comment_end"));

    if (*cptr != '[')
        BtYacc_printf(f, get_section("action_if_!yytrial_start"));
    else
        trialaction = 1;

    if (!lflag)
        BtYacc_printf(f, line_format, lineno - 1, (inc_file ? inc_file_name : input_file_name));

    if (*cptr == '=') ++cptr;

    maxoffset = n = 0;
    for (i = nitems - 1; pitem[i]; --i)
    {
        ++n;

        if (pitem[i]->symbol_class != ARGUMENT)
           ++maxoffset;
    }

    if (maxoffset > 0)
    {
        offsets = NEW2(maxoffset+1, offsets[0]);
        if (offsets == 0) no_space();
    }

    for (j = 0, ++i; i < nitems; ++i)
    {
        if (pitem[i]->symbol_class != ARGUMENT)
            offsets[++j] = i - nitems + 1;
    }
    rhs = pitem + nitems - 1;

    depth = 0;
loop:
    c = *cptr;
    if (c == '$')
    {
        if (cptr[1] == '<')
        {
            int unsigned d_lineno = lineno;
            char *d_line = dup_line();
            char *d_cptr = d_line + (cptr - line);

            ++cptr;
            tag = get_tag();
            c = *cptr;
            if (c == '$')
            {
                BtYacc_printf(f, get_section("yyval.tag_reference"), tag);
                ++cptr;
                FREE(d_line);
                goto loop;
            }
            else if (isdigit(c) || (c == '-' && isdigit(cptr[1])))
            {
                i = get_number();
                if (i <= 0)
                {
                    BtYacc_printf(f, get_section("yyvsp.tag_reference"), i - n, tag);
                }
                else if (i > maxoffset)
                {
                    dollar_warning(d_lineno, i);
                    BtYacc_printf(f, get_section("yyvsp.tag_reference"), i - maxoffset, tag);
                }
                else
                    BtYacc_printf(f, get_section("yyvsp.tag_reference"), offsets[i], tag);
                FREE(d_line);
                goto loop;
            }
            else if (isalpha(c) || c == '_')
            {
                char *arg = scan_id();

                for (i = plhs[nrules]->args - 1; i >= 0; --i)
                {
                    if (arg == plhs[nrules]->argnames[i])
                        break;
                }
                if (i < 0)
                    error(d_lineno, d_line, d_cptr, "unknown argument %s", arg);

                BtYacc_printf(f, get_section("yyvsp.tag_reference"), i - plhs[nrules]->args + 1 - n, tag);
                FREE(d_line);
                goto loop;
            }
            else
                dollar_error(d_lineno, d_line, d_cptr);
        }
        else if (cptr[1] == '$')
        {
            if (havetags)
            {
                tag = plhs[nrules]->tag;
                if (tag == 0) untyped_lhs();

                BtYacc_printf(f, get_section("yyval.tag_reference"), tag);
            }
            else
                BtYacc_printf(f, get_section("yyval_reference"));

            cptr += 2;
            haveyyval = 1;
            goto loop;
        }
        else if (isdigit(cptr[1]))
        {
            ++cptr;
            i = get_number();
            if (havetags)
            {
                if (i <= 0 || i > maxoffset)
                    unknown_rhs(i);
                tag = rhs[offsets[i]]->tag;
                if (tag == 0)
                    untyped_rhs(i, rhs[offsets[i]]->name);

                BtYacc_printf(f, get_section("yyvsp.tag_reference"), offsets[i], tag);
            }
            else
            {
                if (i > n)
                {
                    dollar_warning(lineno, i);
                    BtYacc_printf(f, get_section("yyvsp_reference"), i - maxoffset);
                }
                else
                    BtYacc_printf(f, get_section("yyvsp_reference"), offsets[i]);
            }
            goto loop;
        }
        else if (cptr[1] == '-')
        {
            cptr += 2;
            i = get_number();
            if (havetags)
                unknown_rhs(-i);

            BtYacc_printf(f, get_section("yyvsp_reference"), -i - n);
            goto loop;
        }
        else if (isalpha(cptr[1]) || cptr[1] == '_')
        {
            char *arg;
            ++cptr;
            arg = scan_id();

            for (i = plhs[nrules]->args - 1; i >= 0; --i)
            {
                if (arg == plhs[nrules]->argnames[i])
                    break;
            }
            if (i < 0)
                error(lineno, line, cptr, "unknown argument %s", arg);
            tag = plhs[nrules]->argtags[i];
            if (tag)
            {
                BtYacc_printf(f, get_section("yyvsp.tag_reference"), i - plhs[nrules]->args + 1 - n, tag);
            }
            else
            {
                if (havetags)
                    error(lineno, 0, 0, "untyped argument $%s", arg);
                BtYacc_printf(f, get_section("yyvsp_reference"), i - plhs[nrules]->args + 1 - n);
            }
            goto loop;
        }
    }
    if (isalpha(c) || c == '_' || c == '$')
    {
        do
        {
            BtYacc_putc(c, f);
            c = *++cptr;
        } while (isalnum(c) || c == '_' || c == '$');
        goto loop;
    }
    ++cptr;
    if (trialaction && c == '[' && depth == 0)
    {
        ++depth;
        BtYacc_printf(f, "    %s", get_section("action_block_start"));
        goto loop;
    }
    if (trialaction && c == ']' && depth == 1)
    {
        --depth;
        BtYacc_printf(f, "\n    %s", get_section("action_block_end"));
        c = nextc(0);
        if (c == '[' && !haveyyval)
        {
            goto loop;
        }
        else if (c == '{' && !haveyyval)
        {
            BtYacc_puts("\n", f);

            if (!lflag) BtYacc_puts("#\n", f);

            BtYacc_printf(f, get_section("action_if_!yytrial_start"));

            if (!lflag)
                BtYacc_printf(f, line_format, lineno - 1, (inc_file ? inc_file_name : input_file_name));
            trialaction = 0;
            goto loop;
        }
        else
        {
            BtYacc_puts("\n", f);

            if (!lflag) BtYacc_puts("#\n", f);

            BtYacc_printf(f, get_section("action_case_end"));
            FREE(a_line);
            if (maxoffset > 0) FREE(offsets);
            return;
        }
    }

    BtYacc_putc(c, f);

    switch (c)
    {
    case '\n':
        get_line();
        if (line) goto loop;
        unterminated_action(a_lineno, a_line, a_cptr);

    case ';':
        if (depth > 0) goto loop;

        if (!trialaction)
        {
            BtYacc_printf(f, get_section("action_if_!yytrial_end"));
        }
        BtYacc_puts("\n", f);

        if (!lflag) BtYacc_puts("#\n", f);

        BtYacc_printf(f, get_section("action_case_end"));
        FREE(a_line);
        if (maxoffset > 0) FREE(offsets);
        return;

    case '[':
        ++depth;
        goto loop;

    case ']':
        --depth;
        goto loop;

    case '{':
        ++depth;
        goto loop;

    case '}':
        if (--depth > 0) goto loop;
        BtYacc_printf(f, get_section("action_if_!yytrial_end"));

        c = nextc(0);
        if (c == '[' && !haveyyval)
        {
            trialaction = 1;
            goto loop;
        }
        else if (c == '{' && !haveyyval)
        {
            BtYacc_puts("\n", f);

            if (!lflag) BtYacc_puts("#\n", f);

            BtYacc_printf(f, get_section("action_if_!yytrial_start"));

            if (!lflag)
                BtYacc_printf(f, line_format, lineno - 1, (inc_file ? inc_file_name : input_file_name));

            goto loop;
        }
        else
        {
            BtYacc_puts("\n", f);

            if (!lflag) BtYacc_puts("#\n", f);

            BtYacc_printf(f, get_section("action_case_end"));
            FREE(a_line);
            if (maxoffset > 0) FREE(offsets);
            return;
        }

    case '\'':
    case '"':
        copy_string(c, f, 0);
        goto loop;

    case '/':
        copy_comment(f, 0);
        goto loop;

    default:
        goto loop;
    }
}

/*
Decode %... token following a rule.

Expected tokens are:
  %%
  %\
  %prec <terminal>
  %= <terminal>
  %dprec <number>
*/
int mark_symbol(void)
{
    int c;
    bucket defbp = {0};
    bucket *bp = &defbp;
    BtYacc_keyword_code k = PREC;
    char *t_cptr = cptr;

    c = cptr[1];
    if (c == '%' || c == '\\')
    {
        cptr += 2;
        return (1);
    }

    if (c == '=')
    {
        cptr += 2;
    }
    else
    {
        k = keyword(0);
        switch (k)
        {
        case PREC:
        case DPREC_PRIO:
        case LEFT:
        case RIGHT:
        case NONASSOC:
        case PREFER:
            break;

        default:
            syntax_error_ex(lineno, line, t_cptr, "expected a PRECedence");
        }
    }

    defbp.assoc = k;
    defbp.prec = UNDEFINED;

    switch (k)
    {
    case PREFER:
        // 'prefer' does assign a default, high, precedence: the last one wins.
        defbp.prec = ++precedence_max;
    case LEFT:
    case RIGHT:
    case NONASSOC:
    case PREC:
    case DPREC_PRIO:        /* this one is followed by a numeric value: */
        c = nextc(1);
        if (isalpha(c) || c == '_' || c == '.' || c == '$')
            bp = get_name(1);
        else if (c == '\'' || c == '"')
            bp = get_literal(1);
        else if (isdigit(c))
        {
            bp->prec = get_number();
            if (bp->prec <= 0)
                syntax_error_ex(lineno, line, t_cptr, "expected a positive, non-zero rule's precedence value following the %% keyword: %s", t_cptr);
        }
        else if (k == PREC)
        {
            syntax_error_ex(lineno, line, t_cptr, "expected a terminal, non-terminal token or simple token literal string identifying this rule's precedence");
            /*NOTREACHED*/
            return 0;
        }

        if (bp)
        {
            switch (k)
            {
            // these override the associativity of the given precedence token:
            case LEFT:
            case RIGHT:
            case NONASSOC:
                if (bp != &defbp)
                    defbp = *bp;
                bp = &defbp;
                defbp.assoc = k;
                break;

            case PREFER:
                break;

            default:
                if (bp->prec <= 0)
                    syntax_error_ex(lineno, line, t_cptr, "expected a token with an associated positive, non-zero precedence to follow %%%s", bp->name);
                break;
            }
        }
        else
        {
            bp = &defbp;
        }
        break;

    default:
        fatal("internal error in precedence handler: unexpected %%%s", (bp ? bp->name : "???"));
    }

    if (rprec[nrules] != UNDEFINED && bp->prec != rprec[nrules])
        prec_redeclared();

    rprec[nrules] = bp->prec;
    rassoc[nrules] = bp->assoc;
    return (0);
}

void read_grammar(void)
{
    register int c;

    initialize_grammar();
    advance_to_start();

    for (;;)
    {
        c = nextc(0);
        if (c == EOF) break;
        if (isalpha(c) || c == '_' || c == '.' || c == '$' || c == '\'' ||
                c == '"')
        {
            add_symbol();
        }
        else if (c == '{' || c == '=' || c == '[')
        {
            copy_action();
        }
        else if (c == '|')
        {
            end_rule();
            start_rule(plhs[nrules-1], 0);
            ++cptr;
        }
        else if (c == '%')
        {
            if (mark_symbol())
                break;
        }
        else
        {
            syntax_error(lineno, line, cptr);
        }
    }
    end_rule();
    if (goal->args > 0)
        error(0, 0, 0, "start symbol %s requires arguments", goal->name);
}

void free_tags(void)
{
    register int i;

    if (tag_table == 0) return;

    for (i = 0; i < ntags; ++i)
    {
        FREE(tag_table[i]);
    }
    FREE(tag_table);
}

void pack_names(void)
{
    register bucket *bp;
    register char *p, *s, *t;

    name_pool_size = sizeof("$end") + sizeof("$accept");
    for (bp = first_symbol; bp; bp = bp->next)
        name_pool_size += (int)strlen(bp->name) + 1;
    name_pool = MALLOC(name_pool_size);
    if (name_pool == 0) no_space();

    t = name_pool;
    strcpy(t, "$accept");
    t += strlen(t) + 1;
    strcpy(t, "$end");
    t += strlen(t) + 1;
    for (bp = first_symbol; bp; bp = bp->next)
    {
        p = t;
        s = bp->name;
        while ((*t++ = *s++))
            continue;
        FREE(bp->name);
        bp->name = p;
    }
}

void check_symbols(void)
{
    register bucket *bp;

    if (goal->symbol_class == UNKNOWN)
        undefined_goal(goal->name);

    for (bp = first_symbol; bp; bp = bp->next)
    {
        if (bp->symbol_class == UNKNOWN)
        {
            undefined_symbol_warning(bp->name);
            bp->symbol_class = TERM;
        }
    }
}

void pack_symbols(void)
{
    register bucket *bp;
    register bucket **v;
    register int i, j, k, n;
    const int TOKEN_IS_ASCII_CHAR_RANGE = (ZEROflag ? 1 : 256);

    nsyms = 2;
    ntokens = 1;
    for (bp = first_symbol; bp; bp = bp->next)
    {
        ++nsyms;
        if (bp->symbol_class == TERM) ++ntokens;
    }
    start_symbol = ntokens;
    nvars = nsyms - ntokens;

    symbol_name = NEW2(nsyms, symbol_name[0]);
    if (symbol_name == 0) no_space();
    symbol_value = NEW2(nsyms, symbol_value[0]);
    if (symbol_value == 0) no_space();
    symbol_prec = NEW2(nsyms, symbol_prec[0]);
    if (symbol_prec == 0) no_space();
    symbol_assoc = NEW2(nsyms, symbol_assoc[0]);
    if (symbol_assoc == 0) no_space();

    v = NEW2(nsyms, v[0]);
    if (v == 0) no_space();

    v[0] = 0;
    v[start_symbol] = 0;

    i = 1;
    j = start_symbol + 1;
    for (bp = first_symbol; bp; bp = bp->next)
    {
        if (bp->symbol_class == TERM)
            v[i++] = bp;
        else
            v[j++] = bp;
    }
    assert(i == ntokens && j == nsyms);

    for (i = 1; i < ntokens; ++i)
        v[i]->index = i;

    goal->index = start_symbol + 1;
    k = start_symbol + 2;
    while (++i < nsyms)
    {
        if (v[i] != goal)
        {
            v[i]->index = k;
            ++k;
        }
    }
    goal->value = 0;
    k = 1;
    for (i = start_symbol + 1; i < nsyms; ++i)
    {
        if (v[i] != goal)
        {
            v[i]->value = k;
            ++k;
        }
    }
    k = 0;
    if (TOKEN_IS_ASCII_CHAR_RANGE > 'Z' + 1)
    {
        for (i = 1; i < ntokens; ++i)
        {
            n = v[i]->value;
            if (n > TOKEN_IS_ASCII_CHAR_RANGE)
            {
                for (j = k++; j > 0 && symbol_value[j-1] > n; --j)
                    symbol_value[j] = symbol_value[j-1];
                symbol_value[j] = n;
            }
        }
    }
    else
    {
        for (i = 1; i < ntokens; ++i)
        {
            v[i]->value = UNDEFINED;
            ++k;
        }
    }
    /* 'error' token: */
    if (v[1]->value == UNDEFINED)
        v[1]->value = TOKEN_IS_ASCII_CHAR_RANGE;
    j = 0;
    n = TOKEN_IS_ASCII_CHAR_RANGE + 1;
    for (i = 2; i < ntokens; ++i)
    {
        if (v[i]->value == UNDEFINED)
        {
            while (j < k && n == symbol_value[j])
            {
                while (++j < k && n == symbol_value[j])
                    continue;
                ++n;
            }
            v[i]->value = n;
            ++n;
        }
    }
    symbol_name[0] = name_pool + 8; /* "$end" */
    symbol_value[0] = 0;
    symbol_prec[0] = 0;
    symbol_assoc[0] = TOKEN;
    for (i = 1; i < ntokens; ++i)
    {
        symbol_name[i] = v[i]->name;
        symbol_value[i] = v[i]->value;
        symbol_prec[i] = v[i]->prec;
        symbol_assoc[i] = v[i]->assoc;
    }
    symbol_name[start_symbol] = name_pool; /* "$accept" */
    symbol_value[start_symbol] = -1;
    symbol_prec[start_symbol] = 0;
    symbol_assoc[start_symbol] = TOKEN;
    for (++i; i < nsyms; ++i)
    {
        k = v[i]->index;
        symbol_name[k] = v[i]->name;
        symbol_value[k] = v[i]->value;
        symbol_prec[k] = v[i]->prec;
        symbol_assoc[k] = v[i]->assoc;
    }
    FREE(v);
}

void pack_grammar(void)
{
    register int i, j;
    BtYacc_keyword_code assoc;
    Yshort prec;

    ritem = NEW2(nitems, ritem[0]);
    if (ritem == 0) no_space();
    rlhs = NEW2(nrules, rlhs[0]);
    if (rlhs == 0) no_space();
    rrhs = NEW2(nrules+1, rrhs[0]);
    if (rrhs == 0) no_space();
    rprec = RENEW(rprec, nrules, rprec[0]);
    if (rprec == 0) no_space();
    rassoc = RENEW(rassoc, nrules, rassoc[0]);
    if (rassoc == 0) no_space();

    ritem[0] = -1;
    ritem[1] = goal->index;
    ritem[2] = 0;
    ritem[3] = -2;
    rlhs[0] = 0;
    rlhs[1] = 0;
    rlhs[2] = start_symbol;
    rrhs[0] = 0;
    rrhs[1] = 0;
    rrhs[2] = 1;

    j = 4;
    for (i = 3; i < nrules; ++i)
    {
        if (plhs[i]->args > 0)
        {
          if (plhs[i]->argnames)
          {
            FREE(plhs[i]->argnames);
            plhs[i]->argnames = 0;
          }
          if (plhs[i]->argtags)
          {
            FREE(plhs[i]->argtags);
            plhs[i]->argtags = 0;
          }
        }
        rlhs[i] = plhs[i]->index;
        rrhs[i] = j;
        assoc = rassoc[i] /* was: TOKEN, but may be LEFT/RIGHT/NONASSOC when rule had an explicit %left/%right/%nonassoc precedence modifier */;
        prec = rprec[i] /* UNDEFINED or 0 */;
        while (pitem[j])
        {
            ritem[j] = pitem[j]->index;
            if (pitem[j]->symbol_class == TERM)
            {
                /*
                Only determine rule precedence and associativity by taking those
                values from the right-most terminal when the rule itself hasn't
                already been 'overridden' via %prefer/%prec/%dprec and / or
                %left/%right/%nonassoc
                */
                if (rprec[i] == UNDEFINED)
                {
                    prec = pitem[j]->prec;
                }
                if (!is_assigned_explicit_associativity(rassoc[i]))
                {
                    assoc = pitem[j]->assoc;
                }
            }
            ++j;
        }
        ritem[j] = -i;
        ++j;
        rprec[i] = prec;
        rassoc[i] = assoc;
    }
    rrhs[i] = j;
    FREE(plhs);
    FREE(pitem);
    clean_arg_cache();
}

void print_grammar(void)
{
    register int i, j, k;
    int spacing = 0;
    register FILE *f;

    open_output_files();
    f = verbose_file;

    if (!vflag) return;

    k = 1;
    for (i = 2; i < nrules; ++i)
    {
        if (rlhs[i] != rlhs[i-1])
        {
            if (i != 2) BtYacc_puts("\n", f);

            BtYacc_printf(f, "%4d  %s :", i - 2, symbol_name[rlhs[i]]);
            spacing = (int)strlen(symbol_name[rlhs[i]]) + 1;
        }
        else
        {
            BtYacc_printf(f, "%4d  ", i - 2);
            j = spacing;
            while (--j >= 0) BtYacc_putc(' ', f);

            BtYacc_putc('|', f);
        }

        while (ritem[k] >= 0)
        {
            BtYacc_printf(f, " %s", symbol_name[ritem[k]]);
            ++k;
        }
        ++k;
        BtYacc_putc('\n', f);
    }
}

void free_reader_buffers(void)
{
    FREE(line);
    FREE(cache);
    FREE(name_pool);
}

void reader(void)
{
  create_symbol_table();
  read_declarations();
  read_grammar();
  if(read_errs) done(1);
  free_symbol_table();
  free_tags();
  pack_names();
  check_symbols();
  pack_symbols();
  pack_grammar();
  free_symbols();
  print_grammar();
}
