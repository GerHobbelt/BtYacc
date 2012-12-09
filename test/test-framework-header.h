/*
BtYacc test grammars: Framework Header Section
*/

#if defined(_MSC_VER)
#include "portability-config-MSVC.h"
#elif defined(__MINGW32__)
#include "portability-config-MingW.h"
#else
#include "portability-config.h"
#endif

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_MEMORY_H)
#include <memory.h>
#endif
#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#if defined(HAVE_CTYPE_H)
#include <ctype.h>
#endif
#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_CONIO_H)
#include <conio.h>
#endif
#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif
#if defined(HAVE_SYSLOG_H)
#include <syslog.h>
#endif
#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif
#if defined(HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_TERMIOS_H)
#include <termios.h>
#endif
#if defined(HAVE_MATH_H)
#include <math.h>
#endif
#if defined(HAVE_LIMITS_H)
#include <limits.h>
#endif
#if defined(HAVE_FLOAT_H)
#include <float.h>
#endif




#ifndef YYERROR /* using ifdef(YYERROR) to notice whether we were loaded as header or half way through the generated code */
#define YYERROR_DETAILED    1
#else
#define TEST_FRAMEWORK_ALT_INTRO_CODE   1
#endif


#ifndef YYSTYPE
typedef double yystype_t;
#define YYSTYPE yystype_t

#undef YYDBPR
#define YYDBPR(yystype)    BTYACC_DEBUG_LOG_PARTIAL("%f", (double)(yystype))
#endif






/*
------------------------------------------------------------------

support code

------------------------------------------------------------------
*/

/**
Get a key, don't wait for ENTER key to get it.
*/
static int getkey(void);

static int yylex(void);

#include "test-framework-porting-lib.h"

/*
library to buffer character input using a dynamically sized buffer:
*/

struct mstring
{
    char *base;
    char *ptr;
    char *end;
};

static void msprintf(struct mstring *buf, const char *str, ...);
static int mputchar(struct mstring *buf, int ch);

#define mputc(m, ch)    ((m)->ptr == (m)->end ? mputchar(m, ch) : (*(m)->ptr++ = (ch)))

static struct mstring *msnew(void);
static char *msdone(struct mstring *buf);

/**
hash a string
*/
static unsigned int strhash(char const * name);

/**
the structure of a symbol table entry
*/
struct symbol
{
    char *name;
    unsigned int index;         /* unique identifier for quick lookup in the hash table based on a number instead of the string */
    struct symbol *link;        /* chain of buckets for a single hash */
    struct symbol *next;        /* chain of all buckets */

    YYSTYPE value;
};

/**
Lookup a string and produce the bucket which stores that symbol.

If the symbol does not exist yet, it is added to the symbol table.
*/
static struct symbol *lookup_symbol(char const * name);

/**
Lookup a symbol based on the index number previously obtained by lookup()->index
and produce the bucket which stores that symbol.

Returns NULL when the index is unknown.
*/
static struct symbol *lookup_symbol_by_index(unsigned int index);

static void create_symbol_table(void);
static void free_symbol_table(void);


/**
Print a 'out of memory' error message and abort/exit.
*/
static void no_space(void);


