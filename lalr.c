#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"

typedef
  struct shorts
    {
      struct shorts *next;
      Yshort value;
    }
  shorts;

int tokensetsize = 0;
Yshort *lookaheads = NULL;
Yshort *LAruleno = NULL;
int unsigned *LA = NULL;
Yshort *accessing_symbol = NULL;
core **state_table = NULL;
shifts **shift_table = NULL;
reductions **reduction_table = NULL;
Yshort *goto_map = NULL;
Yshort *from_state = NULL;
Yshort *to_state = NULL;

static int infinity = 0;
static int maxrhs = 0;
static int ngotos = 0;
static unsigned *F = NULL;
static Yshort **includes = NULL;
static shorts **lookback = NULL;
static Yshort **R = NULL;
static Yshort *INDEX = NULL;
static Yshort *VERTICES = NULL;
static int top = 0;



static void set_state_table(void)
{
    register core *sp;

    state_table = NEW2(nstates, state_table[0]);
    for (sp = first_state; sp; sp = sp->next)
        state_table[sp->number] = sp;
}



static void set_accessing_symbol(void)
{
    register core *sp;

    accessing_symbol = NEW2(nstates, accessing_symbol[0]);
    for (sp = first_state; sp; sp = sp->next)
        accessing_symbol[sp->number] = sp->accessing_symbol;
}



static void set_shift_table(void)
{
    register shifts *sp;

    shift_table = NEW2(nstates, shift_table[0]);
    for (sp = first_shift; sp; sp = sp->next)
        shift_table[sp->number] = sp;
}



static void set_reduction_table(void)
{
    register reductions *rp;

    reduction_table = NEW2(nstates, reduction_table[0]);
    for (rp = first_reduction; rp; rp = rp->next)
        reduction_table[rp->number] = rp;
}



static void set_maxrhs(void)
{
  register Yshort *itemp;
  register Yshort *item_end;
  register int length;
  register int max;

  length = 0;
  max = 0;
  item_end = ritem + nitems;

  for (itemp = ritem; itemp < item_end; ++itemp)
    {
      if (*itemp >= 0)
        {
          ++length;
        }
      else
        {
          if (length > max) max = length;
          length = 0;
        }
    }

  maxrhs = max;
}



static void initialize_LA(void)
{
  register int i, j, k;
  register reductions *rp;

  lookaheads = NEW2(nstates + 1, lookaheads[0]);

  k = 0;

  for (i = 0; i < nstates; ++i)
    {
      lookaheads[i] = k;
      rp = reduction_table[i];
      if (rp)
        k += rp->nreds;
    }
  lookaheads[nstates] = k;

  LA = NEW2(k * tokensetsize, LA[0]);
  LAruleno = NEW2(k, LAruleno[0]);
  lookback = NEW2(k, lookback[0]);

  k = 0;

  for (i = 0; i < nstates; ++i)
    {
      rp = reduction_table[i];
      if (rp)
        {
          for (j = 0; j < rp->nreds; j++)
            {
              LAruleno[k] = rp->rules[j];
              ++k;
            }
        }
    }
}


static void set_goto_map(void)
{
    register shifts *sp;
    register int i;
    register int symbol;
    register int k;
    register Yshort *temp_map;
    register int state2;
    register int state1;

    goto_map = NEW2(nvars + 1, goto_map[0]);
    goto_map -= ntokens;
    temp_map = NEW2(nvars + 1, temp_map[0]);
    temp_map -= ntokens;

    ngotos = 0;
    for (sp = first_shift; sp; sp = sp->next)
    {
        for (i = sp->nshifts - 1; i >= 0; --i)
        {
          symbol = accessing_symbol[sp->shift[i]];

          if (ISTOKEN(symbol)) break;

          if (ngotos >= MAXSHORT)
            fatal("too many gotos");

          ++ngotos;
          ++(goto_map[symbol]);
        }
    }

    k = 0;
    for (i = ntokens; i < nsyms; ++i)
    {
      temp_map[i] = k;
      k += goto_map[i];
    }

  for (i = ntokens; i < nsyms; ++i)
    goto_map[i] = temp_map[i];

  goto_map[nsyms] = ngotos;
  temp_map[nsyms] = ngotos;

  from_state = NEW2(ngotos, from_state[0]);
  to_state = NEW2(ngotos, to_state[0]);

  for (sp = first_shift; sp; sp = sp->next)
    {
      state1 = sp->number;

      for (i = sp->nshifts - 1; i >= 0; --i)
        {
          state2 = sp->shift[i];
          symbol = accessing_symbol[state2];

          if (ISTOKEN(symbol)) break;

          k = temp_map[symbol]++;
          from_state[k] = state1;
          to_state[k] = state2;
        }
    }

  temp_map += ntokens;
  FREE(temp_map);
}



/*  Map_goto maps a state/symbol pair into its numeric representation.  */

static int map_goto(int state, int symbol)
{
    register int high;
    register int low;
    register int middle;
    register int s;

    low = goto_map[symbol];
    high = goto_map[symbol + 1];

    for (;;)
    {
        assert(low <= high);
        middle = (low + high) >> 1;
        s = from_state[middle];
        if (s == state)
            return (middle);
        else if (s < state)
            low = middle + 1;
        else
            high = middle - 1;
    }
}


static void traverse(int i)
{
  register unsigned *fp1;
  register unsigned *fp2;
  register unsigned *fp3;
  register int j;
  register Yshort *rp;

  int height;
  unsigned *base;

  VERTICES[++top] = i;
  INDEX[i] = height = top;

  base = F + i * tokensetsize;
  fp3 = base + tokensetsize;

  rp = R[i];
  if (rp)
    {
      while ((j = *rp++) >= 0)
        {
          if (INDEX[j] == 0)
            traverse(j);

          if (INDEX[i] > INDEX[j])
            INDEX[i] = INDEX[j];

          fp1 = base;
          fp2 = F + j * tokensetsize;

          while (fp1 < fp3)
            *fp1++ |= *fp2++;
        }
    }

  if (INDEX[i] == height)
    {
      for (;;)
        {
          j = VERTICES[top--];
          INDEX[j] = infinity;

          if (i == j)
            break;

          fp1 = base;
          fp2 = F + j * tokensetsize;

          while (fp1 < fp3)
            *fp2++ = *fp1++;
        }
    }
}


static void digraph(Yshort** relation)
{
  register int i;

  infinity = ngotos + 2;
  INDEX = NEW2(ngotos + 1, INDEX[0]);
  VERTICES = NEW2(ngotos + 1, VERTICES[0]);
  top = 0;

  R = relation;

  for (i = 0; i < ngotos; ++i)
    INDEX[i] = 0;

  for (i = 0; i < ngotos; ++i)
    {
      if (INDEX[i] == 0 && R[i])
        traverse(i);
    }

  FREE(INDEX);
  FREE(VERTICES);
}


static void initialize_F(void)
{
  register int i;
  register int j;
  register int k;
  register shifts *sp;
  register Yshort *edge;
  register unsigned *rowp;
  register Yshort *rp;
  register Yshort **reads;
  register int nedges;
  register int stateno;
  register int symbol;
  register int nwords;

  nwords = ngotos * tokensetsize;
  F = NEW2(nwords, F[0]);

  reads = NEW2(ngotos, reads[0]);
  edge = NEW2(ngotos + 1, edge[0]);
  nedges = 0;

  rowp = F;
  for (i = 0; i < ngotos; ++i)
    {
      stateno = to_state[i];
      sp = shift_table[stateno];

      if (sp)
        {
          k = sp->nshifts;

          for (j = 0; j < k; ++j)
            {
              symbol = accessing_symbol[sp->shift[j]];
              if (ISVAR(symbol))
                break;
              SETBIT(rowp, symbol);
            }

          for (; j < k; ++j)
            {
              symbol = accessing_symbol[sp->shift[j]];
              if (nullable[symbol])
                edge[nedges++] = map_goto(stateno, symbol);
            }

          if (nedges)
            {
              reads[i] = rp = NEW2(nedges + 1, rp[0]);

              for (j = 0; j < nedges; ++j)
                rp[j] = edge[j];

              rp[nedges] = -1;
              nedges = 0;
            }
        }

      rowp += tokensetsize;
    }

  SETBIT(F, 0);
  digraph(reads);

  for (i = 0; i < ngotos; ++i)
    {
        FREE(reads[i]);
    }

  FREE(reads);
  FREE(edge);
}


static void add_lookback_edge(int stateno, int ruleno, int gotono)
{
    register int i, k;
    register int found;
    register shorts *sp;

    i = lookaheads[stateno];
    k = lookaheads[stateno + 1];
    found = 0;
    while (!found && i < k)
    {
        if (LAruleno[i] == ruleno)
            found = 1;
        else
            ++i;
    }
    assert(found);

    sp = NEW(sp[0]);
    sp->next = lookback[i];
    sp->value = gotono;
    lookback[i] = sp;
}


static Yshort** transpose(Yshort** R, int n)
{
  register Yshort **new_R;
  register Yshort **temp_R;
  register Yshort *nedges;
  register Yshort *sp;
  register int i;
  register int k;

  nedges = NEW2(n, nedges[0]);

  for (i = 0; i < n; ++i)
    {
      sp = R[i];
      if (sp)
        {
          while (*sp >= 0)
            ++(nedges[*sp++]);
        }
    }

  new_R = NEW2(n, new_R[0]);
  temp_R = NEW2(n, temp_R[0]);

  for (i = 0; i < n; ++i)
    {
      k = nedges[i];
      if (k > 0)
        {
          sp = NEW2(k + 1, sp[0]);
          new_R[i] = sp;
          temp_R[i] = sp;
          sp[k] = -1;
        }
    }

  FREE(nedges);

  for (i = 0; i < n; ++i)
    {
      sp = R[i];
      if (sp)
        {
          while (*sp >= 0)
            *temp_R[*sp++]++ = i;
        }
    }

  FREE(temp_R);

  return new_R;
}


static void build_relations(void)
{
  register int i;
  register int j;
  register int k;
  register Yshort *rulep;
  register Yshort *rp;
  register shifts *sp;
  register int length;
  register int nedges;
  register int done;
  register int state1;
  register int stateno;
  register int symbol1;
  register int symbol2;
  register Yshort *shortp;
  register Yshort *edge;
  register Yshort *states;
  register Yshort **new_includes;

  includes = NEW2(ngotos, includes[0]);
  edge = NEW2(ngotos + 1, edge[0]);
  states = NEW2(maxrhs + 1, states[0]);

  for (i = 0; i < ngotos; ++i)
    {
      nedges = 0;
      state1 = from_state[i];
      symbol1 = accessing_symbol[to_state[i]];

      for (rulep = derives[symbol1]; *rulep >= 0; ++rulep)
        {
          length = 1;
          states[0] = state1;
          stateno = state1;

          for (rp = ritem + rrhs[*rulep]; *rp >= 0; ++rp)
            {
              symbol2 = *rp;
              sp = shift_table[stateno];
              k = sp->nshifts;

              for (j = 0; j < k; ++j)
                {
                  stateno = sp->shift[j];
                  if (accessing_symbol[stateno] == symbol2) break;
                }

              states[length++] = stateno;
            }

          add_lookback_edge(stateno, *rulep, i);

          --length;
          done = 0;
          while (!done)
            {
              done = 1;
              --rp;

              if (ISVAR(*rp))
                {
                  stateno = states[--length];
                  edge[nedges++] = map_goto(stateno, *rp);
                  if (nullable[*rp] && length > 0) done = 0;
                }
            }
        }

      if (nedges)
        {
          includes[i] = shortp = NEW2(nedges + 1, shortp[0]);

          for (j = 0; j < nedges; ++j)
            shortp[j] = edge[j];
          shortp[nedges] = -1;
        }
    }

  new_includes = transpose(includes, ngotos);

  for (i = 0; i < ngotos; ++i)
  {
    if (includes[i])
      FREE(includes[i]);
  }

  FREE(includes);

  includes = new_includes;

  FREE(edge);
  FREE(states);
}



static void compute_FOLLOWS(void)
{
  digraph(includes);
}


static void compute_lookaheads(void)
{
  register int i, n;
  register unsigned *fp1, *fp2, *fp3;
  register shorts *sp, *next;
  register unsigned *rowp;

  rowp = LA;
  n = lookaheads[nstates];

  for (i = 0; i < n; ++i)
    {
      fp3 = rowp + tokensetsize;
      for (sp = lookback[i]; sp; sp = sp->next)
        {
          fp1 = rowp;
          fp2 = F + tokensetsize * sp->value;
          while (fp1 < fp3)
            *fp1++ |= *fp2++;
        }
      rowp = fp3;
    }

  for (i = 0; i < n; ++i)
    for (sp = lookback[i]; sp; sp = next)
      {
        next = sp->next;
        FREE(sp);
      }

  FREE(lookback);
  FREE(F);
}


void lalr(void)
{
    tokensetsize = WORDSIZE(ntokens);

    set_state_table();
    set_accessing_symbol();
    set_shift_table();
    set_reduction_table();
    set_maxrhs();
    initialize_LA();
    set_goto_map();
    initialize_F();
    build_relations();
    compute_FOLLOWS();
    compute_lookaheads();
}

