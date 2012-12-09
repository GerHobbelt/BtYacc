#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "write.h"
#include <stdarg.h>


static Yshort *null_rules = NULL;

static void print_tokens(void);

void verbose(void)
{
    register int i;

    if (!vflag) return;

    null_rules = (Yshort *) NEW2(nrules, null_rules[0]);
    if (null_rules == 0) no_space();

    BtYacc_puts("\f\n", verbose_file);

    for (i = 0; i < nstates; ++i)
        print_state(i);
    FREE(null_rules);

    if (nunused)
        log_unused();
    if (SRtotal || RRtotal)
        log_conflicts();

    print_tokens();

    BtYacc_printf(verbose_file, "\n\n%d terminals, %d nonterminals\n%d grammar rules, %d states\n",
                  ntokens, nvars, nrules - 2, nstates);
}


void log_unused(void)
{
    register int i;
    register Yshort *p;

    BtYacc_puts("\n\nRules never reduced:\n", verbose_file);

    for (i = 3; i < nrules; ++i)
    {
        if (!rules_used[i])
        {
            BtYacc_printf(verbose_file, "\t%s :", symbol_name[rlhs[i]]);

            for (p = ritem + rrhs[i]; *p >= 0; ++p)
                BtYacc_printf(verbose_file, " %s", symbol_name[*p]);

            BtYacc_printf(verbose_file, "  (%d)\n", i - 2);
        }
    }
}


void log_conflicts(void)
{
    register int i;

    BtYacc_puts("\n\n", verbose_file);

    for (i = 0; i < nstates; ++i)
    {
        if (SRconflicts[i] || RRconflicts[i])
        {
            BtYacc_printf(verbose_file, "State %d contains ", i);

            if (SRconflicts[i] == 1)
                BtYacc_puts("1 shift/reduce conflict", verbose_file);
            else if (SRconflicts[i] > 1)
                BtYacc_printf(verbose_file, "%d shift/reduce conflicts", SRconflicts[i]);

            if (SRconflicts[i] && RRconflicts[i])
                BtYacc_puts(", ", verbose_file);

            if (RRconflicts[i] == 1)
                BtYacc_puts("1 reduce/reduce conflict", verbose_file);
            else if (RRconflicts[i] > 1)
                BtYacc_printf(verbose_file, "%d reduce/reduce conflicts", RRconflicts[i]);

            BtYacc_puts(".\n", verbose_file);
        }
    }
}

static const char *get_symbol_assoc_str(BtYacc_keyword_code a)
{
    const char *str = get_keyword_as_string(a);

    if (str == NULL)
        return "???";
    return str;
}

static void print_tokens(void)
{
    register int i;

    BtYacc_printf(verbose_file, "\n\n\nSymbols:\n========\n\n%-30s | value | prec | assoc\n",
            "name");
    BtYacc_printf(verbose_file, "-------------------------------+-------+------+-----------\n");
    for (i = 0; i < nsyms; ++i)
    {
        BtYacc_keyword_code a = symbol_assoc[i];

        BtYacc_printf(verbose_file, "%-30s | %5d | %4d | %s %%%s%*s%s\n",
            symbol_name[i],
            symbol_value[i],
            symbol_prec[i],
            (a == LEFT ? "<L<" : a == RIGHT ? ">R>" : a == NONASSOC ? "!X!" : " x "),
            get_symbol_assoc_str(a),
            (i == start_symbol ? 10 - (int)strlen(get_symbol_assoc_str(a)) : 0), "",
            (i == start_symbol ? " (START SYMBOL)" : ""));
    }
    BtYacc_printf(verbose_file, "-------------------------------+-------+------+-----------\n\n\n");
}



void print_state(int state)
{
    if (state)
        BtYacc_puts("\n\n", verbose_file);

    if (SRconflicts[state] || RRconflicts[state])
        print_conflicts(state);

    BtYacc_printf(verbose_file, "state %d\n", state);
    print_core(state);
    print_nulls(state);
    print_actions(state);
}


void print_conflicts(int state)
{
    int symbol;
    BtYacc_action_code act;
    int number;
    action *p;
    int count = 0;

    symbol = number = -1;
    act = UNDEFINED_ACTION;
    for (p = parser[state]; p; p = p->next)
    {
        if (p->suppressed & 2)
            continue;

        if (p->symbol != symbol)
        {
            symbol = p->symbol;
            number = p->number;
            if (p->action_code == SHIFT)
                act = SHIFT;
            else
                act = REDUCE;
        }
        else if (p->suppressed == 1)
        {
            if (state == final_state && symbol == 0)
            {
                BtYacc_printf(verbose_file, "%d: shift/reduce conflict (accept, reduce %d) on $end\n",
                        state, p->number - 2);
            }
            else
            {
                if (act == SHIFT)
                {
                    BtYacc_printf(verbose_file, "%d: shift/reduce conflict (shift %d, reduce %d) on %s\n",
                            state, number, p->number - 2, symbol_name[symbol]);
                }
                else
                {
                    BtYacc_printf(verbose_file, "%d: reduce/reduce conflict (reduce %d, reduce %d) on %s\n",
                            state, number - 2, p->number - 2, symbol_name[symbol]);
                }
            }
            count++;
        }
    }

    if (count > 0)
    {
        BtYacc_printf(verbose_file, "\n");
    }
}


void print_core(int state)
{
    register int i;
    register int k;
    register int rule;
    register core *statep;
    register Yshort *sp;
    register Yshort *sp1;

    statep = state_table[state];
    k = statep->nitems;

    for (i = 0; i < k; ++i)
    {
        sp1 = sp = ritem + statep->items[i];

        while (*sp >= 0) ++sp;
        rule = -(*sp);
        BtYacc_printf(verbose_file, "\t%s : ", symbol_name[rlhs[rule]]);

        for (sp = ritem + rrhs[rule]; sp < sp1; ++sp)
            BtYacc_printf(verbose_file, "%s ", symbol_name[*sp]);

        BtYacc_putc('.', verbose_file);

        while (*sp >= 0)
        {
            BtYacc_printf(verbose_file, " %s", symbol_name[*sp]);
            ++sp;
        }

        BtYacc_printf(verbose_file, "  (%d)\n", -2 - *sp);
    }
}


void print_nulls(int state)
{
    register action *p;
    register int i, j, k, nnulls;

    nnulls = 0;
    for (p = parser[state]; p; p = p->next)
    {
        if (p->action_code == REDUCE && p->suppressed < 2)
        {
            i = p->number;
            if (rrhs[i] + 1 == rrhs[i+1])
            {
                for (j = 0; j < nnulls && i > null_rules[j]; ++j)
                    continue;

                if (j == nnulls)
                {
                    ++nnulls;
                    null_rules[j] = i;
                }
                else if (i != null_rules[j])
                {
                    ++nnulls;
                    for (k = nnulls - 1; k > j; --k)
                        null_rules[k] = null_rules[k-1];
                    null_rules[j] = i;
                }
            }
        }
    }

    for (i = 0; i < nnulls; ++i)
    {
        j = null_rules[i];
        BtYacc_printf(verbose_file, "\t%s : .  (%d)\n", symbol_name[rlhs[j]],
                j - 2);
    }

    BtYacc_puts("\n", verbose_file);
}


void print_actions(int stateno)
{
    register action *p;
    register shifts *sp;
    register int as;

    if (stateno == final_state)
        BtYacc_puts("\t$end  accept\n", verbose_file);

    p = parser[stateno];
    if (p)
    {
        print_shifts(p);
        print_reductions(p, defred[stateno]);
    }

    sp = shift_table[stateno];
    if (sp && sp->nshifts > 0)
    {
        as = accessing_symbol[sp->shift[sp->nshifts - 1]];
        if (ISVAR(as))
            print_gotos(stateno);
    }

    if (tflag > 1)
    {
        BtYacc_puts("\n\n------------ action dump ---------------\n", verbose_file);
        for (p = parser[stateno]; p; p = p->next)
        {
            BtYacc_printf(verbose_file, "\t%-30s  %6s %5d ~ prec %3d, assoc: %3d (%%%-10s), suppress: %d\n",
                        symbol_name[p->symbol],
                        (p->action_code == SHIFT ? "shift" : "reduce"),
                        (p->action_code == SHIFT ? p->number : p->number - 2),
                        p->prec,
                        p->assoc, get_symbol_assoc_str(p->assoc),
                        p->suppressed);
        }
        BtYacc_puts("----------------------------------------\n\n", verbose_file);
    }
}


static void print_suppressions(const action *p, BtYacc_action_code act, int default_reduction)
{
    if (tflag > 1)
    {
        int count = 0;

        for ( ; p; p = p->next)
        {
            if (p->action_code == act && p->suppressed > 0)
            {
                const char *name = symbol_name[p->symbol];
                const suppression_info *info = p->rej_info;
                int info_count = p->rej_count;
                int i;
                int mode;

                if (count == 0)
                {
                    BtYacc_printf(verbose_file, "\n\t\tSUPRESSED %s actions:\n"
                                                "\t\t------------------------\n",
                                                (act == REDUCE ? "reduce" : "shift"));
                }
                ++count;
                BtYacc_printf(verbose_file, "\t\t%s  %-6s %5d      %*s",
                            ((act == REDUCE && p->number == default_reduction) ? "." : name),
                            (act == REDUCE ? "reduce" : "shift"),
                            (act == REDUCE ? p->number - 2 : p->number),
                            ((act == REDUCE && p->number == default_reduction) ? (int)strlen(name) : 1), "");
                i = 0;
                if (act == REDUCE && p->number == default_reduction)
                {
                    BtYacc_printf(verbose_file, "(DEFAULT REDUCTION for %s)\n", name);
                    i++;
                }
                mode = 0;
                if (p->suppressed & 2)
                {
                    int j;

                    for (j = 0; j < info_count; j++)
                    {
                        switch (info[i].reason)
                        {
                        case SUPPR_BY_PRECEDENCE:
                            mode |= 1;
                            continue;

                        case SUPPR_BY_LEFT_ASSOC:
                            mode |= 2;
                            continue;

                        case SUPPR_BY_RIGHT_ASSOC:
                            mode |= 4;
                            continue;

                        default:
                            mode |= 8;
                            continue;
                        }
                    }

                    if (i != 0)
                    {
                        BtYacc_printf(verbose_file, "\t\t%*s                     ", (int)strlen(name), "");
                    }
                    if (mode != 1 && mode != 2 && mode != 4 && mode != 6)
                        BtYacc_printf(verbose_file, "SUPRESSED by explicit (user-supplied) precedence or associativity\n");
                    else if (mode == 1)
                        BtYacc_printf(verbose_file, "SUPRESSED by explicit (user-supplied) precedence\n");
                    else if (mode == 6)
                        BtYacc_printf(verbose_file, "SUPRESSED by explicit (user-supplied) associativity\n");
                    else if (mode == 2)
                        BtYacc_printf(verbose_file, "SUPRESSED by explicit (user-supplied) left associativity\n");
                    else if (mode == 4)
                        BtYacc_printf(verbose_file, "SUPRESSED by explicit (user-supplied) right associativity\n");
                    i++;
                }
                if (p->suppressed & 1)
                {
                    if (i != 0)
                    {
                        BtYacc_printf(verbose_file, "\t\t%*s                     ", (int)strlen(name), "");
                    }
                    BtYacc_printf(verbose_file, "%s\n",
                            "SUPRESSED by YACC due to default CONFLICT resolve mechanism");
                    i++;
                }

                // now dump the infos:
                for (i = 0; i < info_count; i++, info++)
                {
                    const action *pref = (info->me_is_old_pref ? p : info->conflicting_entry);
                    const action *put = (info->me_is_old_pref ? info->conflicting_entry : p);
                    int l = 0;

                    BtYacc_printf(verbose_file, "\t\t%*s                     ", (int)strlen(name), "");
                    switch (info->reason)
                    {
                    case SUPPR_BY_PRECEDENCE:
                        if (mode != 1)
                        {
                            BtYacc_printf(verbose_file, ": SUPRESSED by explicit (user-supplied) precedence\n");
                            l++;
                        }
                        break;
                    case SUPPR_BY_LEFT_ASSOC:
                        if (mode != 2)
                        {
                            BtYacc_printf(verbose_file, ": SUPRESSED by explicit (user-supplied) left associativity\n");
                            l++;
                        }
                        break;
                    case SUPPR_BY_RIGHT_ASSOC:
                        if (mode != 4)
                        {
                            BtYacc_printf(verbose_file, ": SUPRESSED by explicit (user-supplied) right associativity\n");
                            l++;
                        }
                        break;
                    case SUPPR_NONE_SR_CONFLICT:
                        BtYacc_printf(verbose_file, ": processed by YACC through the default Shift/Reduce CONFLICT resolve mechanism\n");
                        l++;
                        break;
                    case SUPPR_NONE_RR_CONFLICT:
                        BtYacc_printf(verbose_file, ": processed by YACC through the default Reduce/Reduce CONFLICT resolve mechanism\n");
                        l++;
                        break;
                    case SUPPR_BY_YACC_DEFAULT1:
                        BtYacc_printf(verbose_file, ": processed by YACC through the default Shift/Reduce CONFLICT resolve mechanism as this rule's explicit precedence equaled the precedence of the competing rule\n");
                        l++;
                        break;
                    default:
                    case SUPPR_BY_YACC_DEFAULT2:
                        BtYacc_printf(verbose_file, ": processed by YACC through the default Shift/Reduce CONFLICT resolve mechanism as this rule's explicit precedence equaled the precedence of the competing rule\n");
                        l++;
                        break;
                    }

                    if (l != 0)
                        BtYacc_printf(verbose_file, "\t\t%*s                     ", (int)strlen(name), "");
                    BtYacc_printf(verbose_file, "  -> %s/%s conflict (%s%s %d, %s%s %d) on %s\n",
                            (pref->action_code == SHIFT ? "shift" : "reduce"),
                            (put->action_code == SHIFT ? "shift" : "reduce"),
                            (info->me_is_new_pref == info->me_is_old_pref ?
                             (pref->suppressed == 1 ? "(BACKTRACK:) TRY " : "") :
                             (pref->suppressed == 1 ? "(BACKTRACK:) TRY " : "DO NOT ")),
                            ((info->state == final_state && pref->symbol == 0) ? "accept" :
                             (pref->action_code == SHIFT ? "shift" : "reduce")),
                            (pref->action_code == SHIFT ? pref->number : pref->number - 2),
                            (info->me_is_new_pref != info->me_is_old_pref ?
                             (put->suppressed == 1 ? "(BACKTRACK:) TRY " : "") :
                             (put->suppressed == 1 ? "(BACKTRACK:) TRY " : "DO NOT ")),
                            ((info->state == final_state && put->symbol == 0) ? "accept" :
                             (put->action_code == SHIFT ? "shift" : "reduce")),
                            (put->action_code == SHIFT ? put->number : put->number - 2),
                            (pref->symbol == 0 ? "$end" : symbol_name[pref->symbol])
                            );
                }
            }
        }
        if (count > 0)
        {
            BtYacc_printf(verbose_file, "\n");
        }
    }
}


void print_shifts(action const * p)
{
    register action const * q = p;

    for (; p; p = p->next)
    {
        if (p->action_code == SHIFT && p->suppressed == 0)
        {
            BtYacc_printf(verbose_file, "\t%s  shift %d\n",
                        symbol_name[p->symbol], p->number);
        }
    }

    print_suppressions(q, SHIFT, 0);
}


void print_reductions(action const * p, int default_reduction)
{
    register int k, anyreds;
    register action const * q;

    anyreds = 0;
    for (q = p; q ; q = q->next)
    {
        if (q->action_code == REDUCE && q->suppressed < 2)
        {
            anyreds = 1;
            break;
        }
    }
    q = p;

    if (anyreds == 0)
    {
        BtYacc_puts("\t.  error\n", verbose_file);
    }
    else
    {
        for (; p; p = p->next)
        {
            if (p->action_code == REDUCE && p->number != default_reduction)
            {
                k = p->number - 2;
                if (p->suppressed == 0)
                    BtYacc_printf(verbose_file, "\t%s  reduce %d\n",
                            symbol_name[p->symbol], k);
            }
        }

        if (default_reduction > 0)
            BtYacc_printf(verbose_file, "\t.  reduce %d\n", default_reduction - 2);
    }

    print_suppressions(q, REDUCE, default_reduction);
}


void print_gotos(int stateno)
{
    register int i, k;
    register int as;
    register Yshort *to_state;
    register shifts *sp;

    BtYacc_putc('\n', verbose_file);
    sp = shift_table[stateno];
    to_state = sp->shift;
    for (i = 0; i < sp->nshifts; ++i)
    {
        k = to_state[i];
        as = accessing_symbol[k];
        if (ISVAR(as))
            BtYacc_printf(verbose_file, "\t%s  goto %d\n", symbol_name[as], k);
    }
}

