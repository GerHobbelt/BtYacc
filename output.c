#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "write.h"

static int nvectors = 0;
static int nentries = 0;
static Yshort **froms = NULL;
static Yshort **tos = NULL;
static Yshort *conflicts = NULL, nconflicts = 0;
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

    open_output_files();

    BtYacc_printf(output_file, "%sYshort yylhs[] = {%41d,",
            (!rflag ? "static " : ""),
            symbol_value[start_symbol]);

    j = 10;

    for (i = 3; i < nrules; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
        {
            ++j;
        }
        BtYacc_printf(output_file, "%5d,", symbol_value[rlhs[i]]);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);

    BtYacc_printf(output_file, "%sYshort yylen[] = {%41d,",
                (!rflag ? "static " : ""),
                2);

    j = 10;

    for (i = 3; i < nrules; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
        {
            ++j;
        }

        BtYacc_printf(output_file, "%5d,", rrhs[i + 1] - rrhs[i] - 1);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);
}


void output_yydefred(void)
{
    register int i, j;

        open_output_files();

    BtYacc_printf(output_file, "%sYshort yydefred[] = {%38d,",
            (!rflag ? "static " : ""),
            (defred[0] ? defred[0] - 2 : 0));

    j = 10;

    for (i = 1; i < nstates; ++i)
    {
        if (j < 10)
        {
            ++j;
        }
        else
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }

        BtYacc_printf(output_file, "%5d,", (defred[i] ? defred[i] - 2 : 0));
    }

    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);
}

static int find_conflict_base(int cbase)
{
    int i,j;

    for (i = 0; i < cbase; ++i)
    {
        for (j = 0; j + cbase < nconflicts; ++j)
        {
            if (conflicts[i+j] != conflicts[cbase+j])
                break;
        }
        if (j+cbase >= nconflicts)
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

    actionrow = NEW2(3*ntokens, Yshort);
    for (i = 0; i < nstates; ++i) {
        if (parser[i]) {
            for (j = 0; j < 3*ntokens; ++j)
            actionrow[j] = 0;

            shiftcount = 0;
            reducecount = 0;
            conflictcount = 0;
            csym = -1;
            cbase = nconflicts;
            for (p = parser[i]; p; p = p->next) {
                if (csym != -1 && csym != p->symbol) {
                    ++conflictcount;
                    conflicts[nconflicts++] = -1;
                    j = find_conflict_base(cbase);
                    actionrow[csym + 2*ntokens] = j + 1;
                    if (j == cbase) {
                        cbase = nconflicts; }
                    else {
                        if (conflicts[cbase] == -1) ++cbase;

                        nconflicts = cbase; }
                    csym = -1; }
                if (p->suppressed == 0) {
                    if (p->action_code == SHIFT) {
                        ++shiftcount;
                        actionrow[p->symbol] = p->number; }
                    else if (p->action_code == REDUCE &&
                             p->number != defred[i]) {
                        ++reducecount;
                        actionrow[p->symbol + ntokens] = p->number; } }
                else if (p->suppressed == 1) {
                    csym = p->symbol;
                    if (p->action_code == SHIFT) {
                        conflicts[nconflicts++] = p->number; }
                    else if (p->action_code == REDUCE &&
                             p->number != defred[i]) {
                        if (cbase == nconflicts) {
                            if (cbase) --cbase;
                            else       conflicts[nconflicts++] = -1; }
                        conflicts[nconflicts++] = p->number - 2; } } }
            if (csym != -1) {
                ++conflictcount;
                conflicts[nconflicts++] = -1;
                j = find_conflict_base(cbase);
                actionrow[csym + 2*ntokens] = j + 1;
                if (j == cbase) {
                    cbase = nconflicts; }
                else {
                    if (conflicts[cbase] == -1) ++cbase;

                    nconflicts = cbase; } }

            tally[i] = shiftcount;
            tally[nstates+i] = reducecount;
            tally[2*nstates+i] = conflictcount;
            width[i] = 0;
            width[nstates+i] = 0;
            width[2*nstates+i] = 0;
            if (shiftcount > 0) {
                froms[i] = r = NEW2(shiftcount, Yshort);
                tos[i] = s = NEW2(shiftcount, Yshort);
                min = MAXSHORT;
                max = 0;
                for (j = 0; j < ntokens; ++j) {
                    if (actionrow[j]) {
                        if (min > symbol_value[j])
                            min = symbol_value[j];
                        if (max < symbol_value[j])
                            max = symbol_value[j];
                        *r++ = symbol_value[j];
                        *s++ = actionrow[j]; } }
                width[i] = max - min + 1; }
            if (reducecount > 0) {
                froms[nstates+i] = r = NEW2(reducecount, Yshort);
                tos[nstates+i] = s = NEW2(reducecount, Yshort);
                min = MAXSHORT;
                max = 0;
                for (j = 0; j < ntokens; ++j) {
                    if (actionrow[ntokens+j]) {
                        if (min > symbol_value[j])
                            min = symbol_value[j];
                        if (max < symbol_value[j])
                            max = symbol_value[j];
                        *r++ = symbol_value[j];
                        *s++ = actionrow[ntokens+j] - 2; } }
                width[nstates+i] = max - min + 1; }
            if (conflictcount > 0) {
                froms[2*nstates+i] = r = NEW2(conflictcount, Yshort);
                tos[2*nstates+i] = s = NEW2(conflictcount, Yshort);
                min = MAXSHORT;
                max = 0;
                for (j = 0; j < ntokens; ++j) {
                    if (actionrow[2*ntokens+j]) {
                        if (min > symbol_value[j])
                            min = symbol_value[j];
                        if (max < symbol_value[j])
                            max = symbol_value[j];
                        *r++ = symbol_value[j];
                        *s++ = actionrow[2*ntokens+j] - 1; } }
                width[2*nstates+i] = max - min + 1; } } }
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

    symno = symbol_value[symbol] + 3*nstates;

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

    open_output_files();

    state_count = NEW2(nstates, Yshort);

    k = default_goto(start_symbol + 1);

    BtYacc_printf(output_file, "%sYshort yydgoto[] = {%39d,",
                (!rflag ? "static " : ""),
                k);
    save_column(start_symbol + 1, k);

    j = 10;

    for (i = start_symbol + 2; i < nsyms; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        k = default_goto(i);
        BtYacc_printf(output_file, "%5d,", k);
        save_column(i, k);
    }

    outline[OUTPUT_FILE] += 2;
    BtYacc_puts("\n};\n", output_file);
    FREE(state_count);
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
    nvectors = 3*nstates + nvars;

    froms = NEW2(nvectors, Yshort *);
    tos = NEW2(nvectors, Yshort *);
    tally = NEW2(nvectors, Yshort);
    width = NEW2(nvectors, Yshort);
    if (SRtotal+RRtotal)
        conflicts = NEW2(4*(SRtotal+RRtotal), Yshort);
    else
        conflicts = 0;
    nconflicts = 0;

    token_actions();
    FREE(lookaheads);
    FREE(LA);
    FREE(LAruleno);
    FREE(accessing_symbol);

    goto_actions();
    FREE(goto_map + ntokens);
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
    if (i >= 2*nstates)
        return (-1);

    t = tally[i];
    w = width[i];

    for (prev = vector - 1; prev >= 0; --prev)
    {
        j = order[prev];
        if (width[j] != w || tally[j] != t)
            return (-1);

        match = 1;

        for (k = 0; match && k < t; ++k)
        {
            if (tos[j][k] != tos[i][k] || froms[j][k] != froms[i][k])
                match = 0;
        }

        if (match)
            return (j);
    }

    return (-1);
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
    register int i, j;

        open_output_files();

    BtYacc_printf(output_file, "%sYshort yysindex[] = {%38d,",
                (!rflag ? "static " : ""),
                base[0]);
    j = 10;

    for (i = 1; i < nstates; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        BtYacc_printf(output_file, "%5d,", base[i]);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);

    BtYacc_printf(output_file, "%sYshort yyrindex[] = {%38d,",
                (!rflag ? "static " : ""),
                base[nstates]);
    j = 10;

    for (i = nstates + 1; i < 2 * nstates; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];

            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        BtYacc_printf(output_file, "%5d,", base[i]);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);

    BtYacc_printf(output_file, "%sYshort yycindex[] = {%38d,",
                (!rflag ? "static " : ""),
                base[2*nstates]);
    j = 10;

    for (i = 2 * nstates + 1; i < 3 * nstates; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        BtYacc_printf(output_file, "%5d,", base[i]);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);

    BtYacc_printf(output_file, "%sYshort yygindex[] = {%38d,",
                (!rflag ? "static " : ""),
                base[3*nstates]);
    j = 10;

    for (i = 3 * nstates + 1; i < nvectors - 1; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        BtYacc_printf(output_file, "%5d,", base[i]);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);
    FREE(base);
}



void output_table(void)
{
    register int i;
    register int j;

    open_output_files();

    ++outline[CODE_FILE];

    if (tflag)
        BtYacc_printf(stderr, "YYTABLESIZE: %d\n", high);

    if(high >= MAXSHORT) {
      BtYacc_printf(stderr, "Table is longer than %d elements. It's not gonna fly.\n", MAXSHORT);
      exit(1);
    }

    BtYacc_printf(code_file, "#define YYTABLESIZE %d\n", high);

    BtYacc_printf(output_file, "%sYshort yytable[] = {%39d,",
                (!rflag ? "static " : ""),
                table[0]);

    j = 10;

    for (i = 1; i <= high; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        BtYacc_printf(output_file, "%5d,", table[i]);
    }

    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);
    FREE(table);
}



void output_check(void)
{
    register int i;
    register int j;

        open_output_files();

    BtYacc_printf(output_file, "%sYshort yycheck[] = {%39d,",
                (!rflag ? "static " : ""),
                check[0]);

    j = 10;

    for (i = 1; i <= high; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        BtYacc_printf(output_file, "%5d,", check[i]);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);
    FREE(check);
}

void output_ctable(void)
{
    register int i;
    register int j;

    open_output_files();

    BtYacc_printf(output_file, "%sYshort yyctable[] = {%38d,",
                (!rflag ? "static " : ""),
                (conflicts ? conflicts[0] : 0));

    j = 10;

    for (i = 1; i < nconflicts; ++i)
    {
        if (j >= 10)
        {
            ++outline[OUTPUT_FILE];
            BtYacc_putc('\n', output_file);
            j = 1;
        }
        else
            ++j;

        BtYacc_printf(output_file, "%5d,", conflicts[i]);
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);

        FREE(conflicts);
}


int is_C_identifier(char const * name)
{
    register char const * s;
    register int c;

    s = name;
    c = *s;
    if (c == '"')
    {
        c = *++s;
        if (!isalpha(c) && c != '_' && c != '$')
            return (0);
        while ((c = *++s) != '"')
        {
            if (!isalnum(c) && c != '_' && c != '$')
                return (0);
        }
        return (1);
    }

    if (!isalpha(c) && c != '_' && c != '$')
        return (0);
    while ((c = *++s))
    {
        if (!isalnum(c) && c != '_' && c != '$')
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

    if(dflag) {
      BtYacc_puts("#ifndef _yacc_defines_h_\n", defines_file);
      BtYacc_puts("#define _yacc_defines_h_\n\n", defines_file);
    }

    /* VM: Print to either code file or defines file but not to both */
    dc_file = dflag ? defines_file : code_file;
    dc_f_idx = dflag ? DEFINES_FILE : CODE_FILE;

    for (i = 2; i < ntokens; ++i)
    {
        s = symbol_name[i];
        if (is_C_identifier(s))
        {
            BtYacc_puts("#define ", dc_file);
            c = *s;
            if (c == '"')
            {
                while ((c = *++s) != '"')
                {
                    BtYacc_putc(c, dc_file);
                }
            }
            else
            {
                do
                {
                    BtYacc_putc(c, dc_file);
                }
                while ((c = *++s));
            }
            ++outline[dc_f_idx];
            BtYacc_printf(dc_file, " %d\n", symbol_value[i]);
        }
    }

    ++outline[dc_f_idx];
    BtYacc_printf(dc_file, "#define YYERRCODE %d\n", symbol_value[1]);

    if (dflag && unionized)
    {
        if (fclose(union_file))
        {
           perror("output_defines: fclose");
           abort();
        }

        union_file = fopen(union_file_name, "r");
        if (union_file == NULL) open_error(union_file_name);
        while ((c = getc(union_file)) != EOF) {
          BtYacc_putc(c, defines_file);
        }

        BtYacc_puts("extern YYSTYPE yylval;\n", defines_file);
    }

    if(dflag) {
      BtYacc_puts("\n#endif\n", defines_file);
    }
}


void output_stored_text(void)
{
    register int c;
    register FILE *in;
    register int state; /* 0=middle of line, 1=start of line, 2=seen '#' */

    open_output_files();

    state = 1;

    if (fclose(text_file))
    {
       perror("output_stored_text: fclose");
       abort();
    }

    text_file = fopen(text_file_name, "r");
    if (text_file == NULL)
        open_error(text_file_name);
    in = text_file;
    if ((c = getc(in)) == EOF)
        return;
    do {
        if (c == '\n') {
            ++outline[CODE_FILE];
            if (state == 2) {
                BtYacc_printf(code_file, line_format+1, outline[CODE_FILE] + 1, code_file_name);
                state = 1;
                continue; }
            state = 1; }
        else if (state == 1 && c == '#')
            state = 2;
        else
            state = 0;

        BtYacc_putc(c, code_file);
    } while ((c = getc(in)) != EOF);
    if (!lflag)
    {
        ++outline[CODE_FILE];
        BtYacc_printf(code_file, line_format, outline[CODE_FILE] + 1, code_file_name);
    }
}


void output_debug(void)
{
    register int i, j, k, max;
    char **symnam, *s;

    open_output_files();

    outline[CODE_FILE] += 4;
    BtYacc_printf(code_file, "#define YYFINAL %d\n#ifndef YYDEBUG\n#define YYDEBUG %d\n#endif\n",
            final_state, tflag);

    if (rflag)
    {
        outline[OUTPUT_FILE] += 3;
        BtYacc_printf(output_file, "#ifndef YYDEBUG\n#define YYDEBUG %d\n#endif\n",
                tflag);
    }

    max = 0;
    for (i = 2; i < ntokens; ++i)
        if (symbol_value[i] > max)
            max = symbol_value[i];
    ++outline[CODE_FILE];
    BtYacc_printf(code_file, "#define YYMAXTOKEN %d\n", max);

    symnam = (char **) MALLOC((max+1)*sizeof(char *));
    if (symnam == 0) no_space();

    /* Note that it is  not necessary to initialize the element         */
    /* symnam[max].                                                     */
    for (i = 0; i < max; ++i)
        symnam[i] = 0;
    for (i = ntokens - 1; i >= 2; --i)
        symnam[symbol_value[i]] = symbol_name[i];
    symnam[0] = "end-of-file";

    ++outline[OUTPUT_FILE];
    BtYacc_puts("#if YYDEBUG\n", output_file);

    BtYacc_printf(output_file, "%schar *yyname[] = {",
                (!rflag ? "static " : ""));
    j = 80;
    for (i = 0; i <= max; ++i)
    {
        if ((s = symnam[i]))
        {
            if (s[0] == '"')
            {
                k = 7;
                while (*++s != '"')
                {
                    ++k;
                    if (*s == '\\')
                    {
                        k += 2;
                        if (*++s == '\\')
                            ++k;
                    }
                }
                j += k;
                if (j > 80)
                {
                    ++outline[OUTPUT_FILE];
                    BtYacc_putc('\n', output_file);
                    j = k;
                }

                BtYacc_puts("\"\\\"", output_file);
                s = symnam[i];
                while (*++s != '"')
                {
                    if (*s == '\\')
                    {
                        BtYacc_puts("\\\\", output_file);

                        if (*++s == '\\')
                            BtYacc_puts("\\\\", output_file);
                        else
                            BtYacc_putc(*s, output_file);
                    }
                    else
                        BtYacc_putc(*s, output_file);
                }

                BtYacc_puts("\\\"\",", output_file);
            }
            else if (s[0] == '\'')
            {
                if (s[1] == '"')
                {
                    j += 7;
                    if (j > 80)
                    {
                        ++outline[OUTPUT_FILE];
                        BtYacc_putc('\n', output_file);
                        j = 7;
                    }

                    BtYacc_puts("\"'\\\"'\",", output_file);
                }
                else
                {
                    k = 5;
                    while (*++s != '\'')
                    {
                        ++k;
                        if (*s == '\\')
                        {
                            k += 2;
                            if (*++s == '\\')
                                ++k;
                        }
                    }
                    j += k;
                    if (j > 80)
                    {
                        ++outline[OUTPUT_FILE];
                        BtYacc_putc('\n', output_file);
                        j = k;
                    }

                    BtYacc_puts("\"'", output_file);
                    s = symnam[i];
                    while (*++s != '\'')
                    {
                        if (*s == '\\')
                        {
                            BtYacc_puts("\\\\", output_file);

                            if (*++s == '\\')
                                BtYacc_puts("\\\\", output_file);
                            else
                                BtYacc_putc(*s, output_file);
                        }
                        else
                            BtYacc_putc(*s, output_file);
                    }

                    BtYacc_puts("'\",", output_file);
                }
            }
            else
            {
                k = (int)strlen(s) + 3;
                j += k;
                if (j > 80)
                {
                    ++outline[OUTPUT_FILE];
                    BtYacc_putc('\n', output_file);
                    j = k;
                }

                BtYacc_putc('"', output_file);

                do { BtYacc_putc(*s, output_file); } while (*++s);

                BtYacc_puts("\",", output_file);
            }
        }
        else
        {
            j += 2;
            if (j > 80)
            {
                ++outline[OUTPUT_FILE];
                BtYacc_putc('\n', output_file);
                j = 2;
            }

            BtYacc_puts("0,", output_file);
        }
    }
    outline[OUTPUT_FILE] += 2;

    BtYacc_puts("\n};\n", output_file);
    FREE(symnam);

    ++outline[OUTPUT_FILE];

    BtYacc_printf(output_file, "%schar *yyrule[] = {\n",
                (!rflag ? "static " : ""));

    for (i = 2; i < nrules; ++i)
    {
        BtYacc_printf(output_file, "\"%s :", symbol_name[rlhs[i]]);

        for (j = rrhs[i]; ritem[j] > 0; ++j)
        {
            s = symbol_name[ritem[j]];
            if (s[0] == '"')
            {
                BtYacc_puts(" \\\"", output_file);

                while (*++s != '"')
                {
                    if (*s == '\\')
                    {
                        if (s[1] == '\\')
                            BtYacc_puts("\\\\\\\\", output_file);
                        else
                            BtYacc_printf(output_file, "\\\\%c", s[1]);
                        ++s;
                    }
                    else
                        BtYacc_putc(*s, output_file);
                }

                BtYacc_puts("\\\"", output_file);
            }
            else if (s[0] == '\'')
            {
                if (s[1] == '"')
                    BtYacc_puts(" '\\\"'", output_file);
                else if (s[1] == '\\')
                {
                    if (s[2] == '\\')
                        BtYacc_puts(" '\\\\\\\\", output_file);
                    else
                        BtYacc_printf(output_file, " '\\\\%c", s[2]);

                    s += 2;
                    while (*++s != '\'')
                        BtYacc_putc(*s, output_file);

                    BtYacc_putc('\'', output_file);
                }
                else
                    BtYacc_printf(output_file, " '%c'", s[1]);
            }
            else
                BtYacc_printf(output_file, " %s", s);
        }
        ++outline[OUTPUT_FILE];
        BtYacc_puts("\",\n", output_file);
    }

    outline[OUTPUT_FILE] += 2;
    BtYacc_puts("};\n#endif\n", output_file);
}


void output_stype(void)
{
    open_output_files();

    if (!unionized && ntags == 0)
    {
        outline[CODE_FILE] += 3;
        BtYacc_puts("#ifndef YYSTYPE\ntypedef int YYSTYPE;\n#endif\n", code_file);
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
            BtYacc_printf(code_file, line_format, lineno, (inc_file?inc_file_name:input_file_name));
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
            BtYacc_printf(code_file, line_format, lineno, (inc_file?inc_file_name:input_file_name));
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
    register int c, last;
    register int state; /* 0=middle of line, 1=start of line, 2=seen '#' */

        open_output_files();

    state = 1;

    if (fclose(action_file))
      {
         perror("output_semantic_actions: fclose");
         abort();
      }

    action_file = fopen(action_file_name, "r");
    if (action_file == NULL)
                open_error(action_file_name);

    if ((c = getc(action_file)) == EOF)
                return;

    do {
        last = c;
        if (c == '\n') {
            ++outline[CODE_FILE];
            if (state == 2) {
                BtYacc_printf(code_file, line_format+1, outline[CODE_FILE] + 1, code_file_name);
                state = 1;
                continue; }
            state = 1; }
        else if (state == 1 && c == '#')
            state = 2;
        else
            state = 0;

        BtYacc_putc(c, code_file);
    } while ((c = getc(action_file)) != EOF);

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

    for (sl = &active_section_list[0]; sl->name; ++sl) {
      if(strcmp(sl->name,section_name)==0) {
        break;
      }
    }
    if(sl->name==0) {
      BtYacc_printf(stderr, "Cannot find section '%s' in your skeleton file\n", section_name);
      exit(1);
    }

    section =  sl->ptr;
    for (i = lflag ? 1 : 0; section[i]; ++i)
    {
        ++outline[CODE_FILE];
        BtYacc_printf(code_file, "%s\n", section[i]);
    }
}
