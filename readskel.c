#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include <stdarg.h>

/* amount of memeory to allocate at once */
#define CHUNK   8192

static char     *cp = NULL, *cp_end = NULL;
static char const * * ap = NULL;
static char const * * ap_start = NULL;
static char const * * ap_end = NULL;

static void add_ptr(char const * const p)
{
    if (ap == ap_end)
    {
        size_t size = CHUNK;
        char * * nap;

        while ((ap - ap_start) * sizeof(*nap) >= size)
            size = size * 2;
        nap = (char **)malloc(size);
        if (!nap)
            no_space();
        if (ap > ap_start)
            memcpy(nap, ap_start, (ap-ap_start) * sizeof(*nap));
        ap = (char const **)nap + (ap - ap_start);
        ap_start = (char const **)nap;
        ap_end = (char const **)nap + size/sizeof(*nap);
    }
    *ap++ = p;
}

static void add_string(char const * s)
{
    int     len = strlen(s)+1;

    if (len > cp_end - cp)
    {
        int size = len > CHUNK ? len : CHUNK;

        cp = malloc(size);
        if (!cp)
            no_space();
        cp_end = cp + size;
    }
    memcpy(cp, s, len);
    add_ptr(cp);
    cp += len;
}

static void add_fmt(char const * fmt, ...)
{
    va_list args;
    char    buf[256];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    add_string(buf);
}

static char const * const * fin_section(void)
{
  char const * const * rv;

  add_ptr(0);
  rv = ap_start;
  ap_start = ap;
  return rv;
}

struct section *read_skel(char const * name)
{
char    buf[256];
int     section = -2;
int     line = 0, sline = 1, eline = 1;
int     i;
FILE    *fp;
int     max_sections = section_list_btyaccpa_count;
struct section *section_list = (struct section *)calloc(max_sections + 1, sizeof(section_list[0]));

    if (!section_list) no_space();

    fp = fopen(name, "r");
    if (!fp)
        open_error(name);
    while(fgets(buf, 255, fp))
    {
        sline = eline;
        if (sline)
            ++line;

        if ((i = (int)strlen(buf)) == 0)
            continue;
        if (buf[i-1] == '\n')
        {
            buf[--i] = 0;
            eline = 1;
        }
        else
        {
            buf[i++] = '\\';
            buf[i] = 0;
            eline = 0;
        }
        if (sline && buf[0] == '%' && buf[1] == '%')
        {
            char *p = buf+2;
            if (section >= 0)
            {
              section_list[section].ptr = fin_section();
            }
            section = -1;

            while(*p && isspace(*p)) ++p;

            if (isprint(*p))
            {
              char *e = p + 1;
              while(isprint(*e) && !isspace(*e))
                  e++;
              *e = 0;

              // match section name with the one in the default template:
              for (i = 0; section_list_btyaccpa[i].name; ++i)
              {
                if (!strcmp(section_list_btyaccpa[i].name, p))
                {
                  section = i;
                  section_list[section].name = section_list_btyaccpa[i].name;
                }
              }
            }
            if (section >= 0)
              add_fmt("#line %d \"%s\"", line+1, name);
            else if (*p)
            {
              // ignore '---' sections:
              char *e = p;
              while(*e == '-')
                  e++;
              if (!*e)
                  section = -1;
              else
                  error(0, buf, p, "line %d of \"%s\", bad section name",
                        line, name);
            }
        }
        else if (section >= 0)
        {
            add_string(buf);
        }
    }
    if (section >= 0)
        section_list[section].ptr = fin_section();
    if (section == -2)
        error(0, 0, 0, "No sections found in skeleton file \"%s\"", name);

    /* and copy the default sections which were not specced in the custom template: */
    for (i = 0; section_list_btyaccpa[i].name; ++i)
    {
        if (!section_list[i].name)
        {
            section_list[i] = section_list_btyaccpa[i];
        }
    }

    return section_list;
}
