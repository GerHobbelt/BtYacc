#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"


/* TABLE_SIZE is the number of entries in the symbol table. */
/* TABLE_SIZE must be a power of two.                       */

#define TABLE_SIZE 1024


bucket **symbol_table = NULL;
bucket *first_symbol = NULL;
bucket *last_symbol = NULL;


int hash(char const * name)
{
    register char const * s;
    register int c, k;

    assert(name && *name);
    s = name;
    k = *s;
    while ((c = *++s))
        k = (31 * k + c) & (TABLE_SIZE - 1);

    return (k);
}


bucket* make_bucket(char const * name)
{
    register bucket *bp;

    assert(name);
    bp = (bucket *) NEW(bp[0]);
    if (bp == 0) no_space();
    bp->link = 0;
    bp->next = 0;
    bp->name = strdup(name);
    if (bp->name == 0) no_space();
    bp->tag = 0;
    bp->value = UNDEFINED;
    bp->index = 0;
    bp->prec = 0;
    bp->symbol_class = UNKNOWN;
    bp->assoc = TOKEN;
    bp->args = -1;
    bp->argnames = 0;
    bp->argtags = 0;

    return (bp);
}


bucket* lookup(char const * name)
{
    register bucket *bp, **bpp;

    bpp = symbol_table + hash(name);
    bp = *bpp;

    while (bp)
    {
        if (strcmp(name, bp->name) == 0) return (bp);
        bpp = &bp->link;
        bp = *bpp;
    }

    *bpp = bp = make_bucket(name);
    last_symbol->next = bp;
    last_symbol = bp;

    return (bp);
}


void create_symbol_table(void)
{
    register bucket *bp;

    symbol_table = (bucket **) NEW2(TABLE_SIZE, symbol_table[0]);
    if (symbol_table == 0) no_space();

	/* The symbol_table[] array has been completely zeroed by NEW2() */

    bp = make_bucket("error");
    bp->index = 1;
    bp->symbol_class = TERM;

    first_symbol = bp;
    last_symbol = bp;
    symbol_table[hash("error")] = bp;
}


void free_symbol_table(void)
{
    FREE(symbol_table);
    symbol_table = 0;
}


void free_symbols(void)
{
    register bucket *p, *q;

    for (p = first_symbol; p; p = q)
    {
        q = p->next;
        FREE(p);
    }
}
