#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "log.h"
#include "write.h"
#include "mstring.h"

static int nvectors = 0;
static int nentries = 0;
static Yshort **froms = NULL;
static Yshort **tos = NULL;
static Yshort *conflicts = NULL;
static int nconflicts = 0;
static Yshort *tally = NULL;
static Yshort *width = NULL;
static Yshort *state_count = NULL;
static Yshort *order = NULL;
static Yshort *base = NULL;
static Yshort *pos = NULL;
static int maxtable = 0;
static Yshort *table = NULL;
static Yshort *check = NULL;
static int lowzero = 0;
static int high = 0;

static char const *get_rflag_prefix(void)
{
    static char *str = NULL;

    if (!str)
    {
        char const *fmt = (!rflag ? get_section("rflag_prefix") : get_section("!rflag_prefix"));
        /* append space when the entry is non-empty */
        if (strlen(fmt) > 0)
        {
            str = MALLOC(strlen(fmt) + 2);
            if (!str) no_space();
            strcpy(str, fmt);
            strcat(str, " ");
        }
        else
        {
            str = strdup(fmt);
        }
    }
    return str;
}

static char const *convert_to_C_string(char const *s)
{
    char *ret = MALLOC(strlen(s) * 4 + 3);
    char *d = ret;

    if (!ret) no_space();

    for (;;)
    {
        switch (*s++)
        {
        case '\'':
            *d++ = '\'';
            continue;

        case '"':
            *d++ = '\\';
            *d++ = '"';
            continue;

        default:
            if (isprint(s[-1]))
            {
                *d++ = s[-1];
            }
            else
            {
                sprintf(d, "\\x%02X", (s[-1] & 0xFF));
                d += strlen(d);
            }
            continue;

        case 0:
            *d = 0;
            break;
        }
        break;
    }
    return ret;
}

static char const *count_newlines(unsigned int outfile_idx, char const *str)
{
    char const *s = str;

    for (;;)
    {
        s = strchr(s, '\n');
        if (!s)
            break;
        ++s;
        ++outline[outfile_idx];
    }
    return str;
}

static void print_one_comment(FILE *outfile, unsigned int outfile_idx, char const * comment)
{
    if (!strchr(comment, '\n'))
    {
        BtYacc_printf(outfile, "%s %s %s",
                    count_newlines(outfile_idx, get_section("comment_start")),
                    comment,
                    count_newlines(outfile_idx, get_section("comment_end")));
    }
    else
    {
        char *cmt = strdup(comment);
        char *cmtline;

        if (!cmt) no_space();

        ++outline[outfile_idx];
        BtYacc_printf(outfile, "%s\n",
                    count_newlines(outfile_idx, get_section("comment_start")));
        for (cmtline = strtok(cmt, "\n"); cmtline; cmtline = strtok(NULL, "\n"))
        {
            ++outline[outfile_idx];
            BtYacc_printf(outfile, "%s %s\n",
                        count_newlines(outfile_idx, get_section("comment_next_line")),
                        cmtline);
        }
        BtYacc_printf(outfile, "%s",
                    count_newlines(outfile_idx, get_section("comment_end")));
		FREE(cmt);
    }
}

static void print_one_table(FILE *outfile, unsigned int outfile_idx, char const * name, int item_count, Yshort *data)
{
    int j;
    int i;

    BtYacc_printf(outfile, count_newlines(outfile_idx, get_section("int_table_start")),
                count_newlines(outfile_idx, get_rflag_prefix()),
                name,
                (item_count ? item_count : 1),
                (data ? data[0] : 0),
                (item_count > 1 ? get_section("int_table_entry_separator") : ""));
    j = 10;
    for (i = 1; i < item_count; ++i)
    {
        if (j >= 10)
        {
            ++outline[outfile_idx];
            BtYacc_putc('\n', outfile);
            j = 1;
        }
        else
        {
            ++j;
        }

        BtYacc_printf(outfile, count_newlines(outfile_idx, get_section("int_table_entry")), data[i],
                (i < item_count - 1 ? get_section("int_table_entry_separator") : "")); /* no trailing comma for last array element: some languages don't like that */
    }
    BtYacc_puts(count_newlines(outfile_idx, get_section("int_table_end")), outfile);
}

void output(void)
{
    free_itemsets();
    free_shifts();
    free_reductions();
    output_stored_text();
    output_defines();
    output_rule_data();
    output_yydefred();
    output_actions();
    free_parser();
    output_debug();
    output_stype();
    if (rflag)
        write_section("tables");
    write_section("header");
    output_trailing_text();
    write_section("body");
    output_semantic_actions();
    write_section("trailer");
}


void output_rule_data(void)
{
    register int i;
    register int j;
    Yshort *rules = CALLOC(nrules, sizeof(rules[0]));

    if (!rules) no_space();

    open_output_files();

    print_one_comment(output_file, OUTPUT_FILE, "[rule] ==> LHS: symbol value");
    rules[0] = symbol_value[start_symbol];
    j = 1;
    for (i = 3; i < nrules; ++i)
    {
        rules[j++] = symbol_value[rlhs[i]];
    }
    print_one_table(output_file, OUTPUT_FILE, "yylhs", j, rules);

    print_one_comment(output_file, OUTPUT_FILE, "[rule] ==> number of right-hand side tokens in the rule");
    rules[0] = 2;
    j = 1;
    for (i = 3; i < nrules; ++i)
    {
        rules[j++] = rrhs[i + 1] - rrhs[i] - 1;
    }
    print_one_table(output_file, OUTPUT_FILE, "yylen", j, rules);

    FREE(rules);
}


void output_yydefred(void)
{
    register int i, j;
    Yshort *states = CALLOC(nstates, sizeof(states[0]));

    if (!states) no_space();

    open_output_files();

    print_one_comment(output_file, OUTPUT_FILE, "[state] ==> default reduction state");
    states[0] = (defred[0] ? defred[0] - 2 : 0);
    j = 1;
    for (i = 1; i < nstates; ++i)
    {
        states[j++] = (defred[i] ? defred[i] - 2 : 0);
    }
    print_one_table(output_file, OUTPUT_FILE, "yydefred", j, states);

    FREE(states);
}

static int find_conflict_base(int cbase)
{
    int i,j;

    for (i = 0; i < cbase; ++i)
    {
        for (j = 0; j + cbase < nconflicts; ++j)
        {
            if (conflicts[i + j] != conflicts[cbase + j])
                break;
        }
        if (j + cbase >= nconflicts)
            return i;
    }
    return cbase;
}

static void token_actions(void)
{
    register int i, j;
    register int shiftcount, reducecount, conflictcount, csym, cbase;
    register int max, min;
    register Yshort *actionrow, *r, *s;
    register action *p;

    actionrow = NEW2(3 * ntokens, Yshort);
    for (i = 0; i < nstates; ++i)
    {
        if (parser[i])
        {
            for (j = 0; j < 3 * ntokens; ++j)
                actionrow[j] = 0;

            shiftcount = 0;
            reducecount = 0;
            conflictcount = 0;
            csym = -1;
            cbase = nconflicts;
            for (p = parser[i]; p; p = p->next)
            {
                if (csym != -1 && csym != p->symbol)
                {
                    ++conflictcount;
                    conflicts[nconflicts++] = -1;
                    j = find_conflict_base(cbase);
                    actionrow[csym + 2 * ntokens] = j + 1;
                    if (j == cbase)
                    {
                        cbase = nconflicts;
                    }
                    else
                    {
                        if (conflicts[cbase] == -1)
                            ++cbase;

                        nconflicts = cbase;
                    }
                    csym = -1;
                }
                if (p->suppressed == 0)
                {
                    if (p->action_code == SHIFT)
                    {
                        ++shiftcount;
                        actionrow[p->symbol] = p->number;
                    }
                    else if (p->action_code == REDUCE &&
                             p->number != defred[i])
                    {
                        ++reducecount;
                        actionrow[p->symbol + ntokens] = p->number;
                    }
                }
                else if (p->suppressed == 1)
                {
                    csym = p->symbol;
                    if (p->action_code == SHIFT)
                    {
                        conflicts[nconflicts++] = p->number;
                    }
                    else if (p->action_code == REDUCE &&
                             p->number != defred[i])
                    {
                        if (cbase == nconflicts)
                        {
                            if (cbase)
                                --cbase;
                            else
                                conflicts[nconflicts++] = -1;
                        }
                        conflicts[nconflicts++] = p->number - 2;
                    }
                }
            }
            if (csym != -1)
            {
                ++conflictcount;
                conflicts[nconflicts++] = -1;
                j = find_conflict_base(cbase);
                actionrow[csym + 2 * ntokens] = j + 1;
                if (j == cbase)
                {
                    cbase = nconflicts;
                }
                else
                {
                    if (conflicts[cbase] == -1)
                        ++cbase;

                    nconflicts = cbase;
                }
            }

            tally[i] = shiftcount;
            tally[nstates + i] = reducecount;
            tally[2 * nstates + i] = conflictcount;
            width[i] = 0;
            width[nstates + i] = 0;
            width[2 * nstates + i] = 0;
            if (shiftcount > 0)
            {
                froms[i] = r = NEW2(shiftcount, Yshort);
                tos[i] = s = NEW2(shiftcount, Yshort);
                min = MAXSHORT;
                max = 0;
                for (j = 0; j < ntokens; ++j)
                {
                    if (actionrow[j])
                    {
                        if (min > symbol_value[j])
                            min = symbol_value[j];
                        if (max < symbol_value[j])
                            max = symbol_value[j];
                        *r++ = symbol_value[j];
                        *s++ = actionrow[j];
                    }
                }
                width[i] = max - min + 1;
            }
            if (reducecount > 0)
            {
                froms[nstates + i] = r = NEW2(reducecount, Yshort);
                tos[nstates + i] = s = NEW2(reducecount, Yshort);
                min = MAXSHORT;
                max = 0;
                for (j = 0; j < ntokens; ++j)
                {
                    if (actionrow[ntokens+j])
                    {
                        if (min > symbol_value[j])
                            min = symbol_value[j];
                        if (max < symbol_value[j])
                            max = symbol_value[j];
                        *r++ = symbol_value[j];
                        *s++ = actionrow[ntokens+j] - 2;
                    }
                }
                width[nstates+i] = max - min + 1;
            }
            if (conflictcount > 0)
            {
                froms[2 * nstates + i] = r = NEW2(conflictcount, Yshort);
                tos[2 * nstates + i] = s = NEW2(conflictcount, Yshort);
                min = MAXSHORT;
                max = 0;
                for (j = 0; j < ntokens; ++j)
                {
                    if (actionrow[2 * ntokens + j])
                    {
                        if (min > symbol_value[j])
                            min = symbol_value[j];
                        if (max < symbol_value[j])
                            max = symbol_value[j];
                        *r++ = symbol_value[j];
                        *s++ = actionrow[2 * ntokens + j] - 1;
                    }
                }
                width[2 * nstates + i] = max - min + 1;
            }
        }
    }
    FREE(actionrow);
}

static void save_column(int symbol, int default_state)
{
    register int i;
    register int m;
    register int n;
    register Yshort *sp;
    register Yshort *sp1;
    register Yshort *sp2;
    register int count;
    register int symno;

    m = goto_map[symbol];
    n = goto_map[symbol + 1];

    count = 0;

    for (i = m; i < n; ++i)
    {
        if (to_state[i] != default_state)
            ++count;
    }
    if (count == 0) return;

    symno = symbol_value[symbol] + 3 * nstates;

    froms[symno] = sp1 = sp = NEW2(count, Yshort);
    tos[symno] = sp2 = NEW2(count, Yshort);

    for (i = m; i < n; ++i)
    {
        if (to_state[i] != default_state)
        {
            *sp1++ = from_state[i];
            *sp2++ = to_state[i];
        }
    }

    tally[symno] = count;
    width[symno] = sp1[-1] - sp[0] + 1;
}

static void pack_table(void)
{
    register int i;
    register int place;
    register int state;

    base = NEW2(nvectors, Yshort);
    pos = NEW2(nentries, Yshort);

    maxtable = 1000;
    table = NEW2(maxtable, Yshort);
    check = NEW2(maxtable, Yshort);

    lowzero = 0;
    high = 0;

    for (i = 0; i < maxtable; ++i)
        check[i] = -1;

    for (i = 0; i < nentries; ++i)
    {
        state = matching_vector(i);

        if (state < 0)
            place = pack_vector(i);
        else
            place = base[state];

        pos[i] = place;
        base[order[i]] = place;
    }

    for (i = 0; i < nvectors; ++i)
    {
        FREE(froms[i]);
        FREE(tos[i]);
    }

    FREE(froms);
    FREE(tos);
    FREE(tally);
    FREE(width);
    FREE(pos);
}

static int default_goto(int symbol)
{
    register int i;
    register int m;
    register int n;
    register int default_state;
    register int max;

    m = goto_map[symbol];
    n = goto_map[symbol + 1];

    if (m == n) return (0);

    for (i = 0; i < nstates; ++i)
        state_count[i] = 0;

    for (i = m; i < n; ++i)
        ++(state_count[to_state[i]]);

    max = 0;
    default_state = 0;

    for (i = 0; i < nstates; ++i)
    {
        if (state_count[i] > max)
        {
            max = state_count[i];
            default_state = i;
        }
    }

    return (default_state);
}

static void goto_actions(void)
{
    register int i, j, k;
    Yshort *states = CALLOC(nstates, sizeof(states[0]));

    if (!states) no_space();

    open_output_files();

    state_count = NEW2(nstates, Yshort);

    print_one_comment(output_file, OUTPUT_FILE, "[state (symbol)] ==> default goto state");
    k = default_goto(start_symbol + 1);
    save_column(start_symbol + 1, k);
    states[0] = k;
    j = 1;
    for (i = start_symbol + 2; i < nsyms; ++i)
    {
        k = default_goto(i);
        save_column(i, k);
        states[j++] = k;
    }
    print_one_table(output_file, OUTPUT_FILE, "yydgoto", j, states);

    FREE(state_count);
    FREE(states);
}

static void sort_actions(void)
{
  register int i;
  register int j;
  register int k;
  register int t;
  register int w;

  order = NEW2(nvectors, Yshort);
  nentries = 0;

  for (i = 0; i < nvectors; ++i)
    {
      if (tally[i] > 0)
        {
          t = tally[i];
          w = width[i];
          j = nentries - 1;

          while (j >= 0 && (width[order[j]] < w))
            --j;

          while (j >= 0 && (width[order[j]] == w) && (tally[order[j]] < t))
            --j;

          for (k = nentries - 1; k > j; --k)
            order[k + 1] = order[k];

          order[j + 1] = i;
          ++nentries;
        }
    }
}

void output_actions(void)
{
    nvectors = 3 * nstates + nvars;

    froms = NEW2(nvectors, Yshort *);
    tos = NEW2(nvectors, Yshort *);
    tally = NEW2(nvectors, Yshort);
    width = NEW2(nvectors, Yshort);
    if (SRtotal + RRtotal)
        conflicts = NEW2(4 * (SRtotal + RRtotal), Yshort);
    else
        conflicts = 0;
    nconflicts = 0;

    token_actions();
    FREE(lookaheads);
    FREE(LA);
    FREE(LAruleno);
    FREE(accessing_symbol);

    goto_actions();
    goto_map += ntokens;
    FREE(goto_map);
    FREE(from_state);
    FREE(to_state);

    sort_actions();
    pack_table();
    output_base();
    output_table();
    output_check();
    output_ctable();
}


/*  The function matching_vector determines if the vector specified by  */
/*  the input parameter matches a previously considered vector.  The    */
/*  test at the start of the function checks if the vector represents   */
/*  a row of shifts over terminal symbols or a row of reductions, or a  */
/*  column of shifts over a nonterminal symbol.  Berkeley Yacc does not */
/*  check if a column of shifts over a nonterminal symbols matches a    */
/*  previously considered vector.  Because of the nature of LR parsing  */
/*  tables, no two columns can match.  Therefore, the only possible     */
/*  match would be between a row and a column.  Such matches are        */
/*  unlikely.  Therefore, to save time, no attempt is made to see if a  */
/*  column matches a previously considered vector.                      */
/*                                                                      */
/*  Matching_vector is poorly designed.  The test could easily be made  */
/*  faster.  Also, it depends on the vectors being in a specific        */
/*  order.                                                              */
/*  Not really any point in checking for matching conflicts -- it is    */
/*  extremely unlikely to occur, and conflicts are (hopefully) rare.    */

int matching_vector(int vector)
{
    register int i;
    register int j;
    register int k;
    register int t;
    register int w;
    register int match;
    register int prev;

    i = order[vector];
    if (i >= 2 * nstates)
        return -1;

    t = tally[i];
    w = width[i];

    for (prev = vector - 1; prev >= 0; --prev)
    {
        j = order[prev];
        if (width[j] != w || tally[j] != t)
            return -1;

        match = 1;

        for (k = 0; match && k < t; ++k)
        {
            if (tos[j][k] != tos[i][k] || froms[j][k] != froms[i][k])
                match = 0;
        }

        if (match)
            return j;
    }

    return -1;
}



int pack_vector(int vector)
{
    register int i, j, k, l;
    register int t;
    register int loc;
    register int ok;
    register Yshort *from;
    register Yshort *to;
    int newmax;

    i = order[vector];
    t = tally[i];
    assert(t);

    from = froms[i];
    to = tos[i];

    j = lowzero - from[0];
    for (k = 1; k < t; ++k)
        if (lowzero - from[k] > j)
            j = lowzero - from[k];
    for (;; ++j)
    {
        if (j == 0)
            continue;
        ok = 1;

        for (k = 0; ok && k < t; ++k)
        {
            loc = j + from[k];
            if (loc >= maxtable)
            {
                if (loc >= MAXTABLE)
                    fatal("maximum table size exceeded");

                newmax = maxtable;
                do { newmax += 200; } while (newmax <= loc);
                table = (Yshort *) REALLOC(table, newmax*sizeof(Yshort));
                if (table == 0) no_space();
                check = (Yshort *) REALLOC(check, newmax*sizeof(Yshort));
                if (check == 0) no_space();
                for (l  = maxtable; l < newmax; ++l)
                {
                    table[l] = 0;
                    check[l] = -1;
                }
                maxtable = newmax;
            }

            if (check[loc] != -1)
                ok = 0;
        }

        for (k = 0; ok && k < vector; ++k)
        {
            if (pos[k] == j)
                ok = 0;
        }
        if (ok)
        {
            for (k = 0; k < t; ++k)
            {
                loc = j + from[k];
                table[loc] = to[k];
                check[loc] = from[k];
                if (loc > high) high = loc;
            }

            while (check[lowzero] != -1)
                ++lowzero;

            return (j);
        }
    }
}



void output_base(void)
{
    open_output_files();

    print_one_comment(output_file, OUTPUT_FILE, "[state] ==> shift");
    print_one_table(output_file, OUTPUT_FILE, "yysindex", nstates, base);

    print_one_comment(output_file, OUTPUT_FILE, "[state] ==> reduce");
    print_one_table(output_file, OUTPUT_FILE, "yyrindex", nstates, &base[nstates]);

    print_one_comment(output_file, OUTPUT_FILE, "[state] ==> conflict (when non-zero)");
    print_one_table(output_file, OUTPUT_FILE, "yycindex", nstates, &base[2 * nstates]);

    print_one_comment(output_file, OUTPUT_FILE, "[state] ==> goto");
    print_one_table(output_file, OUTPUT_FILE, "yygindex", nvectors - 1 - 3 * nstates, &base[3 * nstates]);

    FREE(base);
}



void output_table(void)
{
    open_output_files();

    //++outline[CODE_FILE];

    if (tflag)
        BtYacc_logf("YYTABLESIZE: %d\n", high);

    if (high >= MAXSHORT)
    {
        table_too_large_error(high);
    }

    BtYacc_printf(code_file, get_section("table_size"), high);

    print_one_comment(output_file, OUTPUT_FILE, "[i] ==> shift on conflict");
    print_one_table(output_file, OUTPUT_FILE, "yytable", high + 1, table);

    FREE(table);
}



void output_check(void)
{
    open_output_files();

    print_one_comment(output_file, OUTPUT_FILE, "[i] ==> token check");
    print_one_table(output_file, OUTPUT_FILE, "yycheck", high + 1, check);

    FREE(check);
}

void output_ctable(void)
{
    open_output_files();

    print_one_comment(output_file, OUTPUT_FILE, "[i + 1] ==> reduce on conflict");
    print_one_table(output_file, OUTPUT_FILE, "yyctable", nconflicts, conflicts);

    FREE(conflicts);
}


static int is_C_identifier(char const * name)
{
    register char const * s;
    register int c;

    s = name;
    c = *s;
    if (c == '"')
    {
        c = *++s;
        if (!isalpha(c) && c != '_')
            return (0);
        while ((c = *++s) != '"')
        {
            if (!isalnum(c) && c != '_')
                return (0);
        }
        return (1);
    }

    if (!isalpha(c) && c != '_')
        return (0);
    while ((c = *++s))
    {
        if (!isalnum(c) && c != '_')
            return (0);
    }
    return (1);
}


void output_defines(void)
{
    register int c, i;
    register char *s;
    FILE *dc_file;
    int dc_f_idx;

    open_output_files();

    /* VM: Print to either code file or defines file but not to both */
    dc_file = dflag ? defines_file : code_file;
    dc_f_idx = dflag ? DEFINES_FILE : CODE_FILE;

    if (dflag) {
      BtYacc_printf(dc_file, count_newlines(dc_f_idx, get_section("defines_file_protection_start")));
    }

    for (i = 2; i < ntokens; ++i)
    {
        char *d = NULL;

        s = symbol_name[i];
        if (is_C_identifier(s))
        {
            c = *s;
            if (c == '"')
            {
                char *d;

                s++;
                s = strdup(s);
                if (!s) no_space();

                d = s;
                while (*d != '"')
                    d++;
                *d = 0;
            }
        }
        else
        {
            /* turn this token into something that's acceptable for the given output language: */
            char const *cset = get_section("token_charset");
            char *dp;
            char e = 0;

            d = MALLOC(strlen(s) * 20 + 16 + strlen(file_prefix));
            if (!d) no_space();
            sprintf(d, "BTYACC_SYMBOL_%s_", file_uc_prefix);
            dp = d + strlen(d);
            c = *s;
            if (c == '"')
            {
                e = '"';
                s++;
            }
            else if (c == '\'')
            {
                e = '\'';
                s++;
            }
            while (*s != e && *s)
            {
                if (strchr(cset, *s))
                {
                    *dp++ = *s;
                }
                else
                {
                    static struct
                    {
                        char c;
                        const char *str;
                    } const character_names[] =
                    {
                        { '~', "tilde" },
                        { '`', "backquote" },
                        { '!', "exclamation_mark" },
                        { '@', "at" },
                        { '#', "hashtag" },
                        { '$', "dollar" },
                        { '%', "percent" },
                        { '^', "circonflex" },
                        { '&', "ampersand" },
                        { '*', "star" },
                        { '(', "open_brace" },
                        { ')', "close_brace" },
                        { '_', "underscore" },
                        { '-', "minus" },
                        { '+', "plus" },
                        { '=', "is" },
                        { '[', "open_square_bracket" },
                        { ']', "close_square_bracket" },
                        { '{', "open_curly_bracket" },
                        { '}', "close_curly_bracket" },
                        { ':', "colon" },
                        { ';', "semicolon" },
                        { '\'', "quote" },
                        { '"', "doublequote" },
                        { '<', "less_than" },
                        { '>', "greater_than" },
                        { ',', "comma" },
                        { '.', "period" },
                        { '?', "question_mark" },
                        { '/', "slash" },
                        { '\\', "backslash" },
                        { '|', "pipe" },
                        { 0, 0 }
                    };
                    int i;
                    for (i = 0; character_names[i].c; i++)
                    {
                        if (character_names[i].c == *s)
                        {
                            strcpy(dp, character_names[i].str);
                            strupr(dp);
                            dp += strlen(character_names[i].str);
                            goto done_this_char;
                        }
                    }
                    *dp++ = "0123456789ABCDEF"[(*s >> 4) & 0x0F];
                    *dp++ = "0123456789ABCDEF"[*s & 0x0F];
done_this_char:
                    if (s[1] && (s[1] != e || s[2]))
                        *dp++ = '_';
                }
                s++;
            }
            *dp = 0;
            s = d;
        }

        BtYacc_printf(dc_file, count_newlines(dc_f_idx, get_section("define_token")), s, symbol_value[i]);
        FREE(d);
    }

    BtYacc_printf(dc_file, count_newlines(dc_f_idx, get_section("define_token")), "YYERRCODE", symbol_value[1]);

    outline[dc_f_idx] += 3;
    BtYacc_puts("\n\n\n", dc_file);

    if (dflag && unionized)
    {
        if (fclose(union_file))
        {
           perror("output_defines: fclose");
           abort();
        }

        union_file = fopen(union_file_name, "r");
        if (union_file == NULL) open_error(union_file_name);
        while ((c = getc(union_file)) != EOF)
        {
          if (c == '\n')
              ++outline[DEFINES_FILE];
          BtYacc_putc(c, defines_file);
        }

        BtYacc_puts(count_newlines(DEFINES_FILE, get_section("yystype_extern_decl")), defines_file);
    }

    if (dflag) {
      BtYacc_printf(dc_file, count_newlines(dc_f_idx, get_section("defines_file_protection_end")));
    }
}


static void output_templated_text(FILE *in)
{
    register int c;
    register int state; /* 0=middle of line, 1=start of line, 2=seen '#' */

    state = 1;

    if ((c = getc(in)) == EOF)
        return;
    do
    {
        if (c == '\n')
        {
            ++outline[CODE_FILE];
            if (state == 2)
            {
                BtYacc_printf(code_file, line_format, outline[CODE_FILE] + 1, code_file_name);
                state = 1;
                continue;
            }
            state = 1;
        }
        else if (state == 1 && c == '#')
        {
            state = 2;
            continue;
        }
        else if (c == '\x01')
        {
            // read rule # from input:
            int rule_nr = 0;

            for (;;)
            {
                c = getc(in);
                if (c == EOF) fatal("internal error while processing rule template element");

                if (c == '\x1f')
                {
                    char const *descr = get_rule_description(rule_nr);
                    BtYacc_printf(code_file, "%s", descr);
                    FREE(descr);
                    break;
                }
                rule_nr *= 10;
                rule_nr += (c - '0');
            }
            continue;
        }
        else
        {
            if (state == 2)
            {
                BtYacc_putc('#', code_file);
            }
            state = 0;
        }

        BtYacc_putc(c, code_file);
    } while ((c = getc(in)) != EOF);

    if (state == 2)
    {
        ++outline[CODE_FILE];
        BtYacc_puts("#\n", code_file);
    }
    else if (state != 1)
    {
        ++outline[CODE_FILE];
        BtYacc_putc('\n', code_file);
    }
}


void output_stored_text(void)
{
    open_output_files();

    if (fclose(text_file))
    {
       perror("output_stored_text: fclose");
       abort();
    }

    text_file = fopen(text_file_name, "r");
    if (text_file == NULL)
        open_error(text_file_name);

    output_templated_text(text_file);

    if (!lflag)
    {
        ++outline[CODE_FILE];
        BtYacc_printf(code_file, line_format, outline[CODE_FILE] + 1, code_file_name);
    }
}


void output_debug(void)
{
    register int i, j, k, max;
    char **symnam;

    open_output_files();

    BtYacc_printf(code_file, count_newlines(CODE_FILE, get_section("define_yyfinal")), final_state);
    BtYacc_printf(code_file, count_newlines(CODE_FILE, get_section("define_yydebug")), tflag);

    if (rflag)
    {
        BtYacc_printf(output_file, count_newlines(OUTPUT_FILE, get_section("define_yydebug")), tflag);
    }

    max = 0;
    for (i = 2; i < ntokens; ++i)
    {
        if (symbol_value[i] > max)
            max = symbol_value[i];
    }
    BtYacc_printf(code_file, count_newlines(CODE_FILE, get_section("define_yymaxtoken")), max);

    symnam = (char **) MALLOC((max+1)*sizeof(char *));
    if (symnam == 0) no_space();

    /* Note that it is  not necessary to initialize the element         */
    /* symnam[max].                                                     */
    for (i = 0; i < max; ++i)
        symnam[i] = 0;
    for (i = ntokens - 1; i >= 2; --i)
        symnam[symbol_value[i]] = symbol_name[i];
    symnam[0] = "end-of-file";

    BtYacc_printf(output_file, count_newlines(OUTPUT_FILE, get_section("debug_yyname_strings_start")), get_rflag_prefix());
    j = 0;
    for (i = 0; i <= max; ++i)
    {
        const char *s = symnam[i];
        const char *sep = (i < max ? get_section("debug_yyname_strings_separator") : "");
        const char *m;
        int quoted = 1;

        if (s)
        {
            s = convert_to_C_string(s);
        }
        else if (i == 1)
        {
            s = "error";
        }
        else
        {
            s = get_section("debug_yy_null");
            quoted = 0;
        }
        k = strlen(s) + 2 * quoted;
        m = strrchr(sep, '\n');
        if (m)
        {
            j = strlen(m + 1);
        }
        else
        {
            k += strlen(sep);
            j += k;
            if (j > 80)
            {
                ++outline[OUTPUT_FILE];
                BtYacc_putc('\n', output_file);
                j = k;
            }
        }
        BtYacc_printf(output_file, "%s%s%s%s", (quoted ? "\"" : ""), s, (quoted ? "\"" : ""), count_newlines(OUTPUT_FILE, sep));
    }

    if (j > 0)
    {
        ++outline[OUTPUT_FILE];
        BtYacc_putc('\n', output_file);
    }
    BtYacc_printf(output_file, count_newlines(OUTPUT_FILE, get_section("debug_yyname_strings_end")));
    FREE(symnam);

    BtYacc_printf(output_file, count_newlines(OUTPUT_FILE, get_section("debug_yyrule_strings_start")), get_rflag_prefix());

    for (i = 2; i < nrules; ++i)
    {
        const char *s = convert_to_C_string(symbol_name[rlhs[i]]);
        const char *sep = (i < nrules - 1 ? get_section("debug_yyname_strings_separator") : "");

        BtYacc_printf(output_file, "\"%s :", s);

        for (j = rrhs[i]; ritem[j] > 0; ++j)
        {
            s = convert_to_C_string(symbol_name[ritem[j]]);

            BtYacc_printf(output_file, " %s", s);
        }
        ++outline[OUTPUT_FILE];
        BtYacc_printf(output_file, "\"%s\n", count_newlines(OUTPUT_FILE, sep));
    }

    BtYacc_printf(output_file, count_newlines(OUTPUT_FILE, get_section("debug_yyrule_strings_end")));
}


void output_stype(void)
{
    open_output_files();

    if (!unionized && ntags == 0)
    {
        BtYacc_puts(count_newlines(CODE_FILE, get_section("define_default_yystype")), code_file);
    }
}


void output_trailing_text(void)
{
    register int c, last;
    register FILE *in;

    if (line == 0)
        return;

    open_output_files();

    in = input_file;
    c = *cptr;
    if (c == '\n')
    {
        ++lineno;
        if ((c = getc(in)) == EOF)
            return;
        if (!lflag)
        {
            ++outline[CODE_FILE];
            BtYacc_printf(code_file, line_format, lineno, (inc_file ? inc_file_name : input_file_name));
        }
        if (c == '\n')
            ++outline[CODE_FILE];

        BtYacc_putc(c, code_file);
        last = c;
    }
    else
    {
        if (!lflag)
        {
            ++outline[CODE_FILE];
            BtYacc_printf(code_file, line_format, lineno, (inc_file ? inc_file_name : input_file_name));
        }
        do {
            BtYacc_putc(c, code_file);
        } while ((c = *++cptr) != '\n');
        ++outline[CODE_FILE];
        BtYacc_putc('\n', code_file);
        last = '\n';
    }

    while ((c = getc(in)) != EOF)
    {
        if (c == '\n')
            ++outline[CODE_FILE];

        BtYacc_putc(c, code_file);
        last = c;
    }

    if (last != '\n')
    {
        ++outline[CODE_FILE];
        BtYacc_putc('\n', code_file);
    }
    if (!lflag)
    {
        ++outline[CODE_FILE];
        BtYacc_printf(code_file, line_format, outline[CODE_FILE] + 1, code_file_name);
    }
}


void output_semantic_actions(void)
{
    open_output_files();

    if (fclose(action_file))
    {
         perror("output_semantic_actions: fclose");
         abort();
    }

    action_file = fopen(action_file_name, "r");
    if (action_file == NULL)
        open_error(action_file_name);

    output_templated_text(action_file);

    if (!lflag)
    {
        ++outline[CODE_FILE];
        BtYacc_printf(code_file, line_format, outline[CODE_FILE] + 1, code_file_name);
    }
}


void free_itemsets(void)
{
    register core *cp, *next;

    FREE(state_table);
    for (cp = first_state; cp; cp = next)
    {
        next = cp->next;
        FREE(cp);
    }
}


void free_shifts(void)
{
    register shifts *sp, *next;

    FREE(shift_table);
    for (sp = first_shift; sp; sp = next)
    {
        next = sp->next;
        FREE(sp);
    }
}



void free_reductions(void)
{
    register reductions *rp, *next;

    FREE(reduction_table);
    for (rp = first_reduction; rp; rp = next)
    {
        next = rp->next;
        FREE(rp);
    }
}



void write_section(char const * section_name)
{
    char const * const * section;
    int i;
    struct section *sl;

    open_output_files();

    sl = lookup_section_name(section_name);
    section = sl->ptr;
    for (i = lflag ? 1 : 0; section[i]; ++i)
    {
        ++outline[CODE_FILE];
        BtYacc_printf(code_file, "%s\n", section[i]);
    }
}


struct section *lookup_section_name(char const * section_name)
{
    struct section *sl;

    for (sl = &active_section_list[0]; sl->name; ++sl)
    {
      if (strcmp(sl->name, section_name) == 0)
      {
        break;
      }
    }
    if (sl->name == 0)
    {
      unknown_section_name_error(section_name);
    }
    return sl;
}


char const *get_section(char const * section_name)
{
    char const * const * section;
    int i;
    size_t nl_count, char_count;
    struct section *sl;
    register char * str;

    /* open_output_files(); */

    sl = lookup_section_name(section_name);

    if (sl->cached_multiline_ptr)
    {
        return sl->cached_multiline_ptr;
    }

    /* count number of lines, then subtract the last one to determine how many newlines we really need to add: */
    section = sl->ptr;
    char_count = 0;
    for (nl_count = 1; section[nl_count]; ++nl_count)
        char_count += strlen(section[nl_count]);

    str = (char *)MALLOC(nl_count + char_count + 1);
    sl->cached_multiline_ptr = str;
    for (i = 1 /* don't bother with lflag */; section[i]; ++i)
    {
        strcpy(str, section[i]);
        str += strlen(str);
        if (section[i + 1])
            *str++ = '\n';
    }
    *str = 0;

    return sl->cached_multiline_ptr;
}


char const *get_rule_description(int rule_nr)
{
    register int j;
    struct mstring *buf = msnew();
    const char *s = convert_to_C_string(symbol_name[rlhs[rule_nr]]);

    if (!buf) no_space();

    msprintf(buf, "%s :", s);

    for (j = rrhs[rule_nr]; ritem[j] > 0; ++j)
    {
        s = convert_to_C_string(symbol_name[ritem[j]]);

        msprintf(buf, " %s", s);
    }
    return msdone(buf);
}


