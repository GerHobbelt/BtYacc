#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "log.h"

int nstates = 0;
core *first_state = NULL;
shifts *first_shift = NULL;
reductions *first_reduction = NULL;

static core **state_set = NULL;
static core *this_state = NULL;
static core *last_state = NULL;
static shifts *last_shift = NULL;
static reductions *last_reduction = NULL;

static int nshifts = 0;
static Yshort *shift_symbol = NULL;

static Yshort *redset = NULL;
static Yshort *shiftset = NULL;

static Yshort **kernel_base = NULL;
static Yshort **kernel_end = NULL;
static Yshort *kernel_items = NULL;


static void allocate_itemsets(void)
{
    register Yshort *itemp;
    register Yshort *item_end;
    register int symbol;
    register int i;
    register int count;
    register int max;
    register Yshort *symbol_count;

    count = 0;
    symbol_count = NEW2(nsyms, symbol_count[0]);

    item_end = ritem + nitems;

    for (itemp = ritem; itemp < item_end; ++itemp)
    {
        symbol = *itemp;
        if (symbol >= 0)
        {
            ++count;
            ++(symbol_count[symbol]);
        }
    }

    kernel_base = NEW2(nsyms, kernel_base[0]);
    kernel_items = NEW2(count, kernel_items[0]);

    count = 0;
    max = 0;

    for (i = 0; i < nsyms; ++i)
    {
        kernel_base[i] = kernel_items + count;
        count += symbol_count[i];
        if (max < symbol_count[i])
            max = symbol_count[i];
    }

    shift_symbol = symbol_count;
    kernel_end = NEW2(nsyms, kernel_end[0]);
}


static void allocate_storage(void)
{
    allocate_itemsets();
    shiftset = NEW2(nsyms, shiftset[0]);
    redset = NEW2(nrules + 1, redset[0]);
    state_set = NEW2(nitems, state_set[0]);
}


static core* new_state(int symbol)
{
    register int n;
    register core *p;
    register Yshort *isp1;
    register Yshort *isp2;
    register Yshort *iend;

    if (tflag > 1)
        BtYacc_logf("LR(0): Entering new_state(%d)\n", symbol);

    if (nstates >= MAXSHORT)
        fatal("too many states");

    isp1 = kernel_base[symbol];
    iend = kernel_end[symbol];
    n = iend - isp1;

    p = (core *) allocate((unsigned) (sizeof(core) + (n - 1) * sizeof(Yshort)));
    p->accessing_symbol = symbol;
    p->number = nstates;
    p->nitems = n;

    isp2 = p->items;
    while (isp1 < iend)
        *isp2++ = *isp1++;

    last_state->next = p;
    last_state = p;

    ++nstates;

    return (p);
}


static int get_state(int symbol)
{
    register int key;
    register Yshort *isp1;
    register Yshort *isp2;
    register Yshort *iend;
    register core *sp;
    register int found;
    register int n;

    if (tflag > 1)
        BtYacc_logf("LR(0): Entering get_state(%d)\n", symbol);

    isp1 = kernel_base[symbol];
    iend = kernel_end[symbol];
    n = (int)(iend - isp1);

    key = *isp1;
    assert(0 <= key && key < nitems);
    sp = state_set[key];
    if (sp)
    {
        found = 0;
        while (!found)
        {
            if (sp->nitems == n)
            {
                found = 1;
                isp1 = kernel_base[symbol];
                isp2 = sp->items;

                while (found && isp1 < iend)
                {
                    if (*isp1++ != *isp2++)
                        found = 0;
                }
            }

            if (!found)
            {
                if (sp->link)
                {
                    sp = sp->link;
                }
                else
                {
                    sp = sp->link = new_state(symbol);
                    found = 1;
                }
            }
        }
    }
    else
    {
        state_set[key] = sp = new_state(symbol);
    }

    return (sp->number);
}


static void append_states(void)
{
    register int i;
    register int j;
    register int symbol;

    if (tflag > 1)
        BtYacc_logf("LR(0): Entering append_states()\n");

    for (i = 1; i < nshifts; ++i)
    {
        symbol = shift_symbol[i];
        j = i;
        while (j > 0 && shift_symbol[j - 1] > symbol)
        {
            shift_symbol[j] = shift_symbol[j - 1];
            --j;
        }
        shift_symbol[j] = symbol;
    }

    for (i = 0; i < nshifts; ++i)
    {
        symbol = shift_symbol[i];
        shiftset[i] = get_state(symbol);
    }
}


static void free_storage(void)
{
    FREE(shift_symbol);
    FREE(redset);
    FREE(shiftset);
    FREE(kernel_base);
    FREE(kernel_end);
    FREE(kernel_items);
    FREE(state_set);
}


static void initialize_states(void)
{
    register int i;
    register Yshort *start_derives;
    register core *p;

    start_derives = derives[start_symbol];
    for (i = 0; start_derives[i] >= 0; ++i)
        continue;

    p = (core *) MALLOC(sizeof(p[0]) + i * sizeof(p->items[0]));
    if (p == 0) no_space();

    p->next = 0;
    p->link = 0;
    p->number = 0;
    p->accessing_symbol = 0;
    p->nitems = i;

    for (i = 0; start_derives[i] >= 0; ++i)
        p->items[i] = rrhs[start_derives[i]];

    first_state = last_state = this_state = p;
    nstates = 1;
}


static void new_itemsets(void)
{
    register int i;
    register int shiftcount;
    register Yshort *isp;
    register Yshort *ksp;
    register int symbol;

    for (i = 0; i < nsyms; ++i)
        kernel_end[i] = 0;

    shiftcount = 0;
    isp = itemset;
    while (isp < itemsetend)
    {
        i = *isp++;
        symbol = ritem[i];
        if (symbol > 0)
        {
            ksp = kernel_end[symbol];
            if (!ksp)
            {
                shift_symbol[shiftcount++] = symbol;
                ksp = kernel_base[symbol];
            }

            *ksp++ = i + 1;
            kernel_end[symbol] = ksp;
        }
    }

    nshifts = shiftcount;
}


static void save_reductions(void)
{
    register Yshort *isp;
    register Yshort *rp1;
    register Yshort *rp2;
    register int item;
    register int count;
    register reductions *p;
    register Yshort *rend;

    count = 0;

    for (isp = itemset; isp < itemsetend; ++isp)
    {
        item = ritem[*isp];
        if (item < 0)
        {
            redset[count++] = -item;
        }
    }

    if (count)
    {
        p = (reductions *) allocate((unsigned) (sizeof(reductions) +
                                        (count - 1) * sizeof(Yshort)));

        p->number = this_state->number;
        p->nreds = count;

        rp1 = redset;
        rp2 = p->rules;
        rend = rp1 + count;

        while (rp1 < rend)
            *rp2++ = *rp1++;

        if (last_reduction)
        {
            last_reduction->next = p;
            last_reduction = p;
        }
        else
        {
            first_reduction = p;
            last_reduction = p;
        }
    }
}


static void save_shifts(void)
{
    register shifts *p;
    register Yshort *sp1;
    register Yshort *sp2;
    register Yshort *send;

    p = (shifts *) allocate((unsigned) (sizeof(shifts) +
                        (nshifts - 1) * sizeof(Yshort)));

    p->number = this_state->number;
    p->nshifts = nshifts;

    sp1 = shiftset;
    sp2 = p->shift;
    send = shiftset + nshifts;

    while (sp1 < send)
        *sp2++ = *sp1++;

    if (last_shift)
    {
        last_shift->next = p;
        last_shift = p;
    }
    else
    {
        first_shift = p;
        last_shift = p;
    }
}


static void generate_states(void)
{
    allocate_storage();
    itemset = NEW2(nitems, itemset[0]);
    ruleset = NEW2(WORDSIZE(nrules), ruleset[0]);
    set_first_derives();
    initialize_states();

    while (this_state)
    {
        closure(this_state->items, this_state->nitems);
        save_reductions();
        new_itemsets();
        append_states();

        if (nshifts > 0)
            save_shifts();

        this_state = this_state->next;
    }

    finalize_closure();
    free_storage();
}


/* show_cores is used for debugging */

void show_cores(void)
{
    core *p;
    int i, j, k, n;
    int itemno;

    if (tflag < 1) return;

    BtYacc_logf("\n\nLR0: cores:\n\n");

    k = 0;
    for (p = first_state; p; ++k, p = p->next)
    {
        if (k)
            BtYacc_logf("\n");
        BtYacc_logf("state %d, number = %d, accessing symbol = %s\n",
                k, p->number, symbol_name[p->accessing_symbol]);
        n = p->nitems;
        for (i = 0; i < n; ++i)
        {
            itemno = p->items[i];
            BtYacc_logf("%4d  ", itemno);
            j = itemno;
            while (ritem[j] >= 0) ++j;
            BtYacc_logf("%s :", symbol_name[rlhs[-ritem[j]]]);
            j = rrhs[-ritem[j]];
            while (j < itemno)
                BtYacc_logf(" %s", symbol_name[ritem[j++]]);
            BtYacc_logf(" .");
            while (ritem[j] >= 0)
                BtYacc_logf(" %s", symbol_name[ritem[j++]]);
            BtYacc_logf("\n");
        }
    }
}


/* show_ritems is used for debugging */

void show_ritems(void)
{
    int i;

    if (tflag < 1) return;

    BtYacc_logf("\n\nLR0: ritems (right-hand items):\n\n");

    for (i = 0; i < nitems; ++i)
    {
        Yshort item = ritem[i];

        if (item >= 0)
            BtYacc_logf("ritem[%5d] = %5d :                %s\n", i, item, symbol_name[item]);
        else
            BtYacc_logf("ritem[%5d] = %5d : rlhs = %5d : %s\n", i, item, rlhs[-item], symbol_name[rlhs[-item]]);
    }
}


/* show_rrhs is used for debugging */

void show_rrhs(void)
{
    int i;

    if (tflag < 1) return;

    BtYacc_logf("\n\nLR0: rrhs:\n\n");

    for (i = 0; i < nrules; ++i)
        BtYacc_logf("rrhs[%d] = %d\n", i, rrhs[i]);
}


/* show_shifts is used for debugging */

void show_shifts(void)
{
    shifts *p;
    int i, j, k, l;

    if (tflag < 1) return;

    BtYacc_logf("\n\nLR0: shifts:\n\n");

    k = 0;
    for (p = first_shift; p; ++k, p = p->next)
    {
        if (k)
            BtYacc_logf("\n");
        BtYacc_logf("shift %d, number = %d, nshifts = %d\n", k, p->number,
                p->nshifts);
        j = p->nshifts;
        l = 0;
        for (i = 0; i < j; ++i)
        {
            BtYacc_logf(" %5d", p->shift[i]);
            if (++l == 10)
            {
                l = 0;
                BtYacc_logs("\n");
            }
        }
        if (l)
            BtYacc_logs("\n");
    }
    BtYacc_logs("\n");
}


static void print_derives(void)
{
    int i;
    int j;
    Yshort *sp;

    if (tflag < 1) return;

    BtYacc_logf("\n\nLR0: DERIVES:\n");

    for (i = start_symbol; i < nsyms; ++i)
    {
        BtYacc_logf("\n%-30s derives\n", symbol_name[i]);

        j = 0;
        for (sp = derives[i]; *sp >= 0; ++sp)
        {
            BtYacc_logf(" %5d", *sp);
            ++j;
            if (j == 10)
            {
                j = 0;
                BtYacc_logf("\n");
            }
        }
        if (j)
            BtYacc_logf("\n");
    }

    BtYacc_logf("\n");
}


static void set_derives(void)
{
    register int i, k;
    register int lhs;
    register Yshort *rules;

    derives = NEW2(nsyms, derives[0]);
    rules = NEW2(nvars + nrules, rules[0]);

    k = 0;

    for (lhs = start_symbol; lhs < nsyms; ++lhs)
    {
        derives[lhs] = rules + k;

        for (i = 0; i < nrules; ++i)
        {
            if (rlhs[i] == lhs)
            {
                rules[k] = i;
                k++;
            }
        }
        rules[k] = -1;
        k++;
    }

    print_derives();
}

void free_derives(void)
{
    if (derives)
        FREE(derives[start_symbol]);
    FREE(derives);
}


static void set_nullable(void)
{
    register int i, j;
    register int empty;
    int done;

    nullable = NEW2(nsyms, nullable[0]);
    if (nullable == 0) no_space();

    for (i = 0; i < nsyms; ++i)
        nullable[i] = 0;

    done = 0;
    while (!done)
    {
        done = 1;

        for (i = 1; i < nitems; ++i)
        {
            empty = 1;
            while ((j = ritem[i]) >= 0)
            {
                if (!nullable[j])
                    empty = 0;
                ++i;
            }
            if (empty)
            {
                j = rlhs[-j];
                if (!nullable[j])
                {
                    nullable[j] = 1;
                    done = 0;
                }
            }
        }
    }

    if (tflag > 1)
    {
        for (i = 0; i < nsyms; ++i)
        {
            if (nullable[i])
                BtYacc_logf("%-30s is nullable\n", symbol_name[i]);
            else
                BtYacc_logf("%-30s is not nullable\n", symbol_name[i]);
        }
    }
}


void free_nullable(void)
{
    FREE(nullable);
}


void lr0(void)
{
    set_derives();
    set_nullable();
    generate_states();

    show_cores();
    show_ritems();
    show_rrhs();
    show_shifts();
}
