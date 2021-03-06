#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "mstring.h"

/* parameters about string length.  START is the starting size and
** START+TAIL should be a power of two */
#define START   24
#define TAIL    8

void msprintf(struct mstring *s, const char *fmt, ...)
{
static char     buf[4096];      /* a big static buffer */
va_list         args;
int             len;

    if (!s || !s->base) return;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    len = (int)strlen(buf);
    if (len > (int)(s->end - s->ptr))
    {
        int     cp = (int)(s->ptr - s->base), cl = (int)(s->end - s->base), nl = cl;
        while (len > (nl - cp))
            nl = nl + nl + TAIL;
        s->base = realloc(s->base, nl);
        if (s->base)
        {
            s->ptr = s->base + cp;
            s->end = s->base + nl;
        }
        else
        {
            s->ptr = s->end = 0;
            return;
        }
    }
    memcpy(s->ptr, buf, len);
    s->ptr += len;
}

int mputchar(struct mstring *s, int ch)
{
    if (!s || !s->base) return ch;
    if (s->ptr == s->end)
    {
        int len = (int)(s->end - s->base);
        s->base = realloc(s->base, len+len+TAIL);
        if (s->base)
        {
            s->ptr = s->base + len;
            s->end = s->base + len+len+TAIL;
        }
        else
        {
            s->ptr = s->end = 0;
            return ch;
        }
    }
    *s->ptr++ = ch;
    return ch;
}

struct mstring *msnew(void) {
    struct mstring *n = malloc(sizeof(struct mstring));

    if (n && (n->base = n->ptr = malloc(START)))
        n->end = n->base + START;
    else
    {
        free(n);
        n = 0;
    }
    return n;
}

char *msdone(struct mstring *s)
{
    char    *r = 0;

    if (s)
    {
        mputc(s, 0);
        r = s->base;
        free(s);
    }
    return r;
}

/* compare two strings, ignoring whitespace, except between two letters or
** digits (and treat all of these as equal) */
int strnscmp(const char *a, const char *b)
{
    for (;;)
    {
        while (isspace(*a)) ++a;

        while (isspace(*b)) ++b;

        while (*a && *a == *b) ++a, ++b;

        if (isspace(*a))
        {
            if (isalnum(a[-1]) && isalnum(*b))
                break;
        }
        else if (isspace(*b))
        {
            if (isalnum(b[-1]) && isalnum(*a))
                break;
        }
        else
            break;
    }
    return *a - *b;
}

unsigned int strnshash(const char *str)
{
    unsigned int    h = 0;
    register unsigned char const *s = (unsigned char const *)str;

    while (*s)
    {
        if (!isspace(*s))
            h = (h << 5) - h + *s;
        ++s;
    }

    return h;
}
