#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "log.h"

Yshort *itemset = NULL;
Yshort *itemsetend = NULL;
int unsigned *ruleset = NULL;

static int unsigned *first_derives = NULL;
static int unsigned *EFF = NULL;


void finalize_closure(void)
{
  FREE(itemset);
  FREE(ruleset);
  first_derives += ntokens * WORDSIZE(nrules);
  FREE(first_derives);
}



static void print_closure(int n)
{
  register Yshort *isp;
  int i;

  BtYacc_logf("\n\nclosure: n = %d\n\n", n);

  BtYacc_logs("itemset:\n");
  i = 0;
  for (isp = itemset; isp < itemsetend; ++isp)
  {
    BtYacc_logf(" %5d", *isp);
	++i;
	if (i == 10)
	{
		i = 0;
		BtYacc_logs("\n");
	}
  }
  if (i)
  {
	BtYacc_logs("\n");
  }
  BtYacc_logs("\n");
}


static void print_EFF(void)
{
    register int i, j;
    register unsigned *rowp;
    register unsigned word;
    register unsigned mask;

    BtYacc_logf("\n\nEpsilon Free Firsts\n");

    for (i = start_symbol; i < nsyms; ++i)
    {
        BtYacc_logf("\n%-30s : ", symbol_name[i]);
        rowp = EFF + ((i - start_symbol) * WORDSIZE(nvars));
        word = *rowp++;

        mask = 1;
        for (j = 0; j < nvars; ++j)
        {
            if (word & mask)
                BtYacc_logf("  %s", symbol_name[start_symbol + j]);

            mask <<= 1;
            if (mask == 0)
            {
                word = *rowp++;
                mask = 1;
            }
        }
    }
}


static void print_first_derives(void)
{
  register int i;
  register int j;
  register unsigned *rp;
  register unsigned cword;
  register unsigned mask;
  int kp;

  BtYacc_logf("\n\n\nFirst Derives\n");

  for (i = start_symbol; i < nsyms; ++i)
    {
      BtYacc_logf("\n%-30s derives\n", symbol_name[i]);
      assert(i >= ntokens);
      rp = first_derives + i * WORDSIZE(nrules);
      cword = *rp++;
      mask = 1;
      kp = 0;

      for (j = 0; j <= nrules; ++j)
        {
          if (cword & mask)
          {
            BtYacc_logf(" %5d", j);
            kp++;
            if (kp == 10)
            {
                BtYacc_logf("\n");
                kp = 0;
            }
          }

          mask <<= 1;
          if (mask == 0)
            {
              cword = *rp++;
              mask = 1;
            }
        }

        if (kp)
        {
            BtYacc_logf("\n");
        }
    }

  fflush(stdout);
}



static void set_EFF(void)
{
    register unsigned *row;
    register Yshort symbol;
    register Yshort *sp;
    register int rowsize;
    register int i;
    register int rule;

    rowsize = WORDSIZE(nvars);
    EFF = NEW2(nvars * rowsize, EFF[0]);

    row = EFF;

    for (i = start_symbol; i < nsyms; ++i)
    {
        sp = derives[i];
        for (rule = *sp; rule > 0; rule = *++sp)
        {
            symbol = ritem[rrhs[rule]];
            if (ISVAR(symbol))
            {
                symbol -= start_symbol;
                SETBIT(row, symbol);
            }
        }
        row += rowsize;
    }

    reflexive_transitive_closure(EFF, nvars);

    if (tflag > 1)
        print_EFF();
}


void set_first_derives(void)
{
  register unsigned *rrow;
  register unsigned *vrow;
  register int j;
  register unsigned mask;
  register unsigned cword;
  register Yshort *rp;

  int rule;
  int i;
  int rulesetsize;
  int varsetsize;

  rulesetsize = WORDSIZE(nrules);
  varsetsize = WORDSIZE(nvars);
  first_derives = NEW2(nvars * rulesetsize, first_derives[0]);
  first_derives -= ntokens * rulesetsize;

  set_EFF();

  rrow = first_derives + ntokens * rulesetsize;

  for (i = start_symbol; i < nsyms; ++i)
    {
      vrow = EFF + ((i - ntokens) * varsetsize);
      cword = *vrow++;
      mask = 1;

      for (j = start_symbol; j < nsyms; ++j)
        {
          if (cword & mask)
            {
              rp = derives[j];
              while ((rule = *rp++) >= 0)
                {
                  SETBIT(rrow, rule);
                }
            }

          mask <<= 1;
          if (mask == 0)
            {
              cword = *vrow++;
              mask = 1;
            }
        }

      vrow += varsetsize;
      rrow += rulesetsize;
    }

    if (tflag > 1)
        print_first_derives();

  FREE(EFF);
}


void closure(Yshort* nucleus, int n)
{
    register int ruleno;
    register unsigned word;
    register unsigned mask;
    register Yshort *csp;
    register unsigned *dsp;
    register unsigned *rsp;
    register int rulesetsize;

    Yshort *csend;
    unsigned *rsend;
    int symbol;
    int itemno;

    rulesetsize = WORDSIZE(nrules);
    rsp = ruleset;
    rsend = ruleset + rulesetsize;

    for (rsp = ruleset; rsp < rsend; ++rsp)
        *rsp = 0;

    csend = nucleus + n;
    for (csp = nucleus; csp < csend; ++csp)
    {
        symbol = ritem[*csp];
        if (ISVAR(symbol))
        {
            assert(symbol >= ntokens);
            dsp = first_derives + symbol * rulesetsize;
            rsp = ruleset;
            while (rsp < rsend)
                *rsp++ |= *dsp++;
        }
    }

    ruleno = 0;
    itemsetend = itemset;
    csp = nucleus;
    for (rsp = ruleset; rsp < rsend; ++rsp)
    {
        word = *rsp;
        if (word == 0)
            ruleno += BITS_PER_WORD;
        else
        {
            mask = 1;
            while (mask)
            {
                if (word & mask)
                {
                    itemno = rrhs[ruleno];
                    while (csp < csend && *csp < itemno)
                        *itemsetend++ = *csp++;
                    *itemsetend++ = itemno;
                    while (csp < csend && *csp == itemno)
                        ++csp;
                }

                    mask <<= 1;
                    ++ruleno;
            }
        }
    }

    while (csp < csend)
        *itemsetend++ = *csp++;

    if (tflag > 1)
        print_closure(n);
}

