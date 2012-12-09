#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "log.h"
#include "write.h"

#if defined(__MSDOS__) || defined(WIN32) || defined(__WIN32)
#include <io.h> /* mktemp() */
#undef HAVE_MKSTEMP
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define HAVE_MKSTEMP 1
#endif

char dflag = 0;
char lflag = 0;
char rflag = 0;
char tflag = 0;
char vflag = 0;
char Eflag = 0;
char ZEROflag = 0;

char *target_dir = NULL;
char *file_prefix = NULL;
char *file_uc_prefix = NULL;    /* file prefix in uppercase form, suitable for use as part of variable names */
char *name_prefix = "yy";       /* suitable for use as part of variable names, i.e. [:alpha:][:alnum:_]*[:alnum:]? */
char *name_uc_prefix = NULL;    /* name prefix in uppercase form, suitable for use as part of variable names */
char *myname = "btyacc";
#if defined(__MSDOS__) || defined(WIN32) || defined(__WIN32)
#define DIR_CHAR '\\'
#define DEFAULT_TMPDIR "."
#else  /* Unix */
#define DIR_CHAR '/'
#define DEFAULT_TMPDIR "/tmp"
#endif
static char const temp_form[] = "yacc_t_XXXXXX";

int unsigned lineno = 0;
int unsigned CODE_FILE = 0;             /*  y.code.c (used when the -r option is specified) */
int unsigned DEFINES_FILE = 1;          /*  y.tab.h                                         */
int unsigned OUTPUT_FILE = 2;           /*  y.tab.c                                         */
int unsigned outline[3] = {0};
const char *outfilename[3] = {0};
FILE *outfile[3] = {0};


char *action_file_name = NULL;
char *code_file_name = NULL;
char *defines_file_name = NULL;
char *input_file_name = "";
char *output_file_name = NULL;
char *text_file_name = NULL;
char *union_file_name = NULL;
char *log_file_name = NULL;
char *verbose_file_name = NULL;

FILE *action_file = NULL;       /*  a temp file, used to save actions associated    */
                                /*  with rules until the parser is written          */
FILE *code_file = NULL;         /*  y.code.c (used when the -r option is specified) */
FILE *defines_file = NULL;      /*  y.tab.h                                         */
FILE *input_file = NULL;        /*  the input file                                  */
FILE *output_file = NULL;       /*  y.tab.c                                         */
FILE *text_file = NULL;         /*  a temp file, used to save text until all        */
                                /*  symbols have been defined                       */
FILE *union_file = NULL;        /*  a temp file, used to save the union             */
                                /*  definition until all symbol have been           */
                                /*  defined                                         */
FILE *log_file = NULL;          /*  a temp file, used to save LR/LALR/... log info  */
FILE *verbose_file = NULL;      /*  y.output                                        */

int nitems = 0;
int nrules = 0;
int nsyms = 0;                  /* the total number of TERMinals and non-terminals */
int ntokens = 0;                /* the total number of TERMinals */
int nvars = 0;                  /* the total number of non-terminals */

Yshort start_symbol = 0;
char  **symbol_name = NULL;
Yshort *symbol_value = NULL;
Yshort *symbol_prec = NULL;
BtYacc_keyword_code *symbol_assoc = NULL;

Yshort *ritem = NULL;
Yshort *rlhs = NULL;           /* array of left-hand side token for rules (rule number is the array index) */
Yshort *rrhs = NULL;
Yshort *rprec = NULL;
BtYacc_keyword_code *rassoc = NULL;
Yshort **derives = NULL;
char *nullable = NULL;


/*
convert an input string to make it usable as a variable name, i.e.
output string contains only [:alpha:][:alnum:_]*[:alnum:]?
*/
char *sanitize_to_varname(const char *in_str)
{
    char *s = STRDUP(in_str);
    char *s2 = s;

    if (!s) no_space();

    if (*s && !isalpha(*s))
    {
        *s = 'y';
    }
    for (++s; *s; s++)
    {
        if (!isalnum(*s) && *s != '_')
        {
            *s = '_';
        }
    }
    for (s--; s >= s2; s--)
    {
        if (*s == '_')
            *s = 0;
        else
            break;
    }
    return s2;
}

/*
strupper() is not known on all platforms, so we provide our own equivalent:

convert the input string to all-uppercase
*/
void strupper(char *str)
{
	while (*str)
	{
		*str = toupper(*str);
		++str;
	}
}

static void file_deletion(FILE* f, char const * name)
{
   if (f)
   {
      if (fclose(f))
      {
         perror("file_deletion: fclose");
         abort();
      }

      if (unlink(name))
      {
         perror("file_deletion: unlink");
         abort();
      }
   }
}



/*
 * memory cleanup routine which is called at the end of the crm114 run.
 *
 * Note: this routine *also* called when an error occurred (e.g. out of memory)
 *    so tread carefully here: do not assume all these pointers are filled.
 */
static void final_cleanup(void)
{
	FREE(defred);
	free_action_order();
	FREE(rules_used);
	FREE(SRconflicts);
	FREE(RRconflicts);
	free_nullable();
	free_derives();
	free_section_cache();
    FREE(ritem);
	FREE(rlhs);
	FREE(rrhs);
	FREE(rprec);
	FREE(rassoc);
    FREE(symbol_name);
    FREE(symbol_value);
    FREE(symbol_prec);
    FREE(symbol_assoc);
	free_reader_buffers();

    if (outfile[DEFINES_FILE])
    {
        fclose(outfile[DEFINES_FILE]);
        outfile[DEFINES_FILE] = NULL;
        defines_file = NULL;
    }
    if (outfile[OUTPUT_FILE])
    {
        fclose(outfile[OUTPUT_FILE]);
        outfile[OUTPUT_FILE] = NULL;
        output_file = NULL;
    }
    if (outfile[CODE_FILE])
    {
        fclose(outfile[CODE_FILE]);
        outfile[CODE_FILE] = NULL;
        code_file = NULL;
    }
    if (verbose_file)
    {
        int c;

		if (log_file)
		{
			if (fclose(log_file))
			{
			   perror("log_file: fclose");
			   abort();
			}
		}

		log_file = fopen(log_file_name, "r");
		if (log_file == NULL) open_error(log_file_name);
		while ((c = getc(log_file)) != EOF)
		{
			BtYacc_putc(c, verbose_file);
		}

        fclose(verbose_file);
        verbose_file = NULL;
    }
	FREE(code_file_name);
	FREE(defines_file_name);
	FREE(output_file_name);
	FREE(verbose_file_name);

    file_deletion(action_file, action_file_name);
    file_deletion(text_file, text_file_name);
    file_deletion(union_file, union_file_name);
    file_deletion(log_file, log_file_name);
    action_file = NULL;
    FREE(action_file_name);
    text_file = NULL;
    FREE(text_file_name);
    union_file = NULL;
    FREE(union_file_name);
    log_file = NULL;
    FREE(log_file_name);
}

void done(int k)
{
    final_cleanup();
    exit(k);
}


#ifdef BTYACC_USE_SIGNAL_HANDLING
sig_atomic_t BtYacc_is_interrupted = 0;

void BtYacc_stop_test(void)
{
    if (BtYacc_is_interrupted)
    {
       (void) fprintf(stderr, "BtYacc execution was interrupted.\n");
       done(BtYacc_is_interrupted);
    }
}


static void on_interruption(int x)
{
   BtYacc_is_interrupted = 255;
   (void) x;
}


static void set_signal_handler(int number)
{
    void (*function)(int) = signal(number, SIG_IGN);

    if (function == SIG_ERR)
    {
       perror("btyacc: set_signal_handler() failed to query a setting");
       exit(254);
    }
    else
       if (  (function != SIG_IGN)
          && (signal(number, &on_interruption) == SIG_ERR) )
       {
          perror("btyacc: set_signal_handler() failed to register a new function");
          exit(253);
       }
}


static void signal_setup(void)
{
#ifdef SIGINT
    set_signal_handler(SIGINT);
#endif
#ifdef SIGTERM
    set_signal_handler(SIGTERM);
#endif
#ifdef SIGHUP
    set_signal_handler(SIGHUP);
#endif
}
#endif


static SPLINT_NO_RETURN void usage(void) GCC_NO_RETURN;

static void usage(void)
{
  BtYacc_logf("usage: %s [OPTIONS] file\n", myname);
  BtYacc_logf(
            "  -b prefix    Change `y' into `prefix' in all output filenames\n"
            "  -d           Generate header file `y.tab.h'\n"
            "  -DNAME       Define btyacc preprocessor variable NAME\n"
            "  -E           Print preprocessed grammar to stdout\n"
            "  -l           Do not insert #line directives into generated code\n"
            "  -o path      Write all files to directory <path> instead of current working directory\n"
            "  -p prefix    Change `yy' into `prefix' in all output code.\n"
            "               Note: '@' means we use the filename as a prefix.\n"
            "  -r           Write tables to `y.tab.c', code to `y.code.c'\n"
            "  -S x.skel    Select parser skeleton\n"
            "  -t           Include debugging code in generated parser\n"
            "  -v           Write description of parser to `y.output'\n"
            "  -0           Generate a 'scannerless' parser, i.e. all tokens are assigned\n"
            "               numbers, even the tokens identified by ASCII characters.\n");
  done(1);
}


static void getargs(int argc, char **argv)
{
    register int i;
    register char *s;

    if (argc > 0) {
        char *ds1, *ds2;
        myname = argv[0];
        // skip MSDOS and UNIX path elements: executable NAME only!
        ds1 = strrchr(myname, ':');
        if (!ds1) ds1 = myname; else ++ds1;
        ds2 = strrchr(ds1, '/');
        if (!ds2) ds2 = ds1; else ++ds2;
        ds1 = strrchr(ds2, '\\');
        if (!ds1) ds1 = ds2; else ++ds1;
        myname = ds1;
        ds1 = strrchr(myname, '.');
        if (ds1) *ds1 = 0; // nuke '.exe' or similar filename extension
    }
    for (i = 1; i < argc; ++i)
    {
        s = argv[i];
        if (*s != '-') break;
        switch (*++s)
        {
        case '\0':
            input_file = stdin;
            if (i + 1 < argc) usage();
            return;

        case '-':
            ++i;
            goto no_more_options;

        case 'b':
            if (*++s)
                file_prefix = s;
            else if (++i < argc)
                file_prefix = argv[i];
            else
                usage();
            continue;

        case 'p':
            if (*++s)
                name_prefix = s;
            else if (++i < argc)
                name_prefix = argv[i];
            else
                usage();
            continue;

        case 'd':
            dflag = 1;
            break;

        case 'D':
            /* Find the preprocessor variable */
            {
              char **ps;
              char *var_name = NULL;
              extern char *defd_vars[];

              if (*++s)
                var_name = s;
              else if (++i < argc)
                var_name = argv[i];
              else
                usage();

              for (ps = &defd_vars[0]; *ps; ++ps)
              {
                if (strcmp(*ps, var_name) == 0)
                {
                  error(lineno, 0, 0, "Preprocessor variable %s already defined", var_name);
                }
              }
              *ps = STRDUP(var_name);

              if (*ps == 0)
                 no_space();
            }
            continue;

        case 'E':
            Eflag = 1;
            break;

        case 'l':
            lflag = 1;
            break;

        case 'o':
            if (*++s)
                target_dir = s;
            else if (++i < argc)
                target_dir = argv[i];
            else
                usage();
            continue;

        case 'r':
            rflag = 1;
            break;

        case 't':
            tflag++;
            break;

        case 'v':
            vflag++;
            break;

        case 'S':
            if (*++s)
                active_section_list = read_skel(s);
            else if (++i < argc)
                active_section_list = read_skel(argv[i]);
            else
                usage();
            continue;

        case '0':
            ZEROflag = 1;
            break;

        default:
            usage();
        }

        for (;;)
        {
            switch (*++s)
            {
            case '\0':
                goto end_of_option;

            case 'd':
                dflag = 1;
                break;

            case 'l':
                lflag = 1;
                break;

            case 'r':
                rflag = 1;
                break;

            case 't':
                tflag++;
                break;

            case 'v':
                vflag++;
                break;

            case '0':
                ZEROflag = 1;
                break;

            default:
                usage();
            }
        }
end_of_option:;
    }

no_more_options:;
    if (i + 1 != argc) usage();
    input_file_name = argv[i];

    if (!file_prefix || !*file_prefix) {
      if (input_file_name) {
        char *s2;

        file_prefix = STRDUP(input_file_name);

        if (!file_prefix) no_space();

        // strip off any leading directories from the input file path:
        s = strrchr(file_prefix, '/');
        if (!s) s = file_prefix; else s++;
        s2 = strrchr(s, '\\');
        if (!s2) s2 = s; else s2++;
        s = strrchr(s2, ':');
        if (!s) s = s2; else s++;

        s2 = strrchr(s, '.');
        if (s2)
          *s2 = 0;

        memmove(file_prefix, s, strlen(s) + 1);
      }
    }
    if (!file_prefix || !*file_prefix) {
        file_prefix = "y";
    }
    file_uc_prefix = sanitize_to_varname(file_prefix);
    strupper(file_uc_prefix);

    if (name_prefix && *name_prefix)
    {
        if (0 == strcmp(name_prefix, "@") && 0 != strcmp(file_prefix, "y"))
        {
            name_prefix = file_prefix;
        }

        /* sanitize the prefix to be suitable as part of variable names: */
        name_prefix = sanitize_to_varname(name_prefix);
    }
    if (!name_prefix || !*name_prefix) {
        name_prefix = "yy";
    }
    name_uc_prefix = STRDUP(name_prefix);
    strupper(name_uc_prefix);

    if (target_dir && *target_dir)
    {
        if (0 == strcmp(target_dir, "@"))
        {
            if (!input_file_name)
            {
                target_dir = "";
            }
            else
            {
                char *s2;
                char *s;

                target_dir = STRDUP(input_file_name);
                if (!target_dir) no_space();

                // strip off filename from the input file path:
                s = strrchr(target_dir, '/');
                if (!s) s = target_dir; else s++;
                s2 = strrchr(s, '\\');
                if (!s2) s2 = s; else s2++;
                s = strrchr(s2, ':');
                if (!s) s = s2; else s++;

                *s2 = 0;
            }
        }
        else
        {
            char *p;
            size_t l;

            l = strlen(target_dir);

            p = (char *)MALLOC(l + 2);
            if (!p) no_space();
            strcpy(p, target_dir);
            if (p[l - 1] != '/' && p[l - 1] != '\\' && p[l - 1] != ':')
            {
                p[l++] = '/';
                p[l] = 0;
            }
            target_dir = p;
        }
    }
    if (!target_dir || !*target_dir)
        target_dir = "";
}

void *allocate(unsigned n)
{
    register char *p;

    p = NULL;
    if (n)
    {
        /* VM: add a few bytes here, cause
         * Linux calloc does not like sizes like 32768 */
        p = (char *)CALLOC(1, n+10);
        if (!p) no_space();
    }
    return (p);
}

static FILE* create_temporary_file(char* fname_template)
{
#if defined(HAVE_MKSTEMP)
    int descriptor = mkstemp(fname_template);

    if (descriptor == -1)
    {
       perror("btyacc: Cannot create temporary file");
       (void) fprintf(stderr, "name template: %s\n", fname_template);
       done(EXIT_FAILURE);
    }
    else
    {
       FILE* pointer = fdopen(descriptor, "w");

       if (pointer)
       {
          return pointer;
       }
       else
       {
          perror("btyacc: A stream can not be associated with an existing file descriptor.");
          done(EXIT_FAILURE);
       }
    }
#else
    char *fn = mktemp(fname_template);
    FILE *fh = (fn ? fopen(fn, "w") : NULL);

    if (!fh)
    {
       perror("btyacc: Cannot create temporary file");
       (void) fprintf(stderr, "name template: %s\n", fname_template);
       done(EXIT_FAILURE);
    }
    return fh;
#endif
}

// Return path to temp dir without trailing '/' DIRSEP
static char *getenv_tempdir(void)
{
    static char const *TEMPORARY_DIR_ENV_VAR[] = { "TMPDIR", "TEMP", "TMP", NULL };
    size_t i, len;
    char* tmpdir = NULL;

    for (i = 0; TEMPORARY_DIR_ENV_VAR[i]; i++)
    {
        tmpdir = getenv(TEMPORARY_DIR_ENV_VAR[i]);
        if (tmpdir && *tmpdir)
            break;
    }

    if (!tmpdir || !*tmpdir)
       tmpdir = DEFAULT_TMPDIR;

    len = strlen(tmpdir);
    if (len && tmpdir[len - 1] == DIR_CHAR)
        tmpdir[len - 1] = 0;
    return tmpdir;
}

void create_temporary_files(void)
{
    size_t i, len;
    char* tmpdir = getenv_tempdir();

    len = strlen(tmpdir);
    i = len + strlen(temp_form) + 2;

    action_file_name = (char *)MALLOC(i);

    if (action_file_name == 0)
       no_space();

    text_file_name = (char *)MALLOC(i);

    if (text_file_name == 0)
       no_space();

    union_file_name = (char *)MALLOC(i);

    if (union_file_name == 0)
       no_space();

    log_file_name = (char *)MALLOC(i);

    if (log_file_name == 0)
       no_space();

    strcpy(action_file_name, tmpdir);
    strcpy(text_file_name, tmpdir);
    strcpy(union_file_name, tmpdir);
    strcpy(log_file_name, tmpdir);

    if (len)
    {
        action_file_name[len] = DIR_CHAR;
        text_file_name[len] = DIR_CHAR;
        union_file_name[len] = DIR_CHAR;
        log_file_name[len] = DIR_CHAR;
        ++len;
    }

    strcpy(action_file_name + len, temp_form);
    strcpy(text_file_name + len, temp_form);
    strcpy(union_file_name + len, temp_form);
    strcpy(log_file_name + len, temp_form);

    action_file_name[len + 5] = 'a';
    text_file_name[len + 5] = 't';
    union_file_name[len + 5] = 'u';
    log_file_name[len + 5] = 'l';

    action_file = create_temporary_file(action_file_name);
    text_file = create_temporary_file(text_file_name);
    union_file = create_temporary_file(union_file_name);
    log_file = create_temporary_file(log_file_name);
}


void create_output_files(void)
{
    size_t len;

    len = strlen(file_prefix) + strlen(target_dir);

    output_file_name = (char *)MALLOC(len + 256);
    if (output_file_name == 0)
        no_space();
    sprintf(output_file_name, get_section("OUTPUT_FILENAME"), target_dir, file_prefix);

    if (rflag)
    {
        code_file_name = (char *)MALLOC(len + 256);
        if (code_file_name == 0)
            no_space();
        sprintf(code_file_name, get_section("CODE_FILENAME"), target_dir, file_prefix);
    }
    else
    {
        code_file_name = STRDUP(output_file_name);
        CODE_FILE = OUTPUT_FILE;
    }

    if (dflag)
    {
        defines_file_name = (char *)MALLOC(len + 256);
        if (defines_file_name == 0)
            no_space();
        sprintf(defines_file_name, get_section("DEFINES_FILENAME"), target_dir, file_prefix);
    }

    if (vflag)
    {
        verbose_file_name = (char *)MALLOC(len + 256);
        if (verbose_file_name == 0)
            no_space();
        sprintf(verbose_file_name, get_section("VERBOSE_FILENAME"), target_dir, file_prefix);
    }

    outfilename[CODE_FILE] = code_file_name;
    outfilename[DEFINES_FILE] = defines_file_name;
    outfilename[OUTPUT_FILE] = output_file_name;
}


static void open_input_files(void)
{
    if (input_file == 0)
    {
        input_file = fopen(input_file_name, "r");
        if (input_file == 0)
            open_error(input_file_name);
    }
}


void open_temporary_files(void)
{
    /* do this only once, first time is on demand, i.e. as late as possible */
    if (!action_file && !text_file && !union_file)
    {
        create_temporary_files();
    }
}


void open_output_files(void)
{
    open_temporary_files();

    /* do this only once, first time is on demand, i.e. as late as possible */
    if (!verbose_file && !defines_file && !output_file && !code_file)
    {
        create_output_files();

        if (verbose_file_name)
        {
            verbose_file = fopen(verbose_file_name, "w");
            if (verbose_file == 0)
                open_error(verbose_file_name);
        }

        if (defines_file_name)
        {
            outfile[DEFINES_FILE] = defines_file = fopen(defines_file_name, "w");
            if (defines_file == 0)
                open_error(defines_file_name);
        }

        outfile[OUTPUT_FILE] = output_file = fopen(output_file_name, "w");
        if (output_file == 0)
            open_error(output_file_name);

        if (outfile[CODE_FILE] == NULL)
        {
            assert(strcmp(code_file_name, output_file_name) != 0);
            outfile[CODE_FILE] = code_file = fopen(code_file_name, "w");
            if (code_file == 0)
                open_error(code_file_name);
        }
        else
        {
            code_file = outfile[CODE_FILE];
        }

        write_section("banner");
    }
}


struct section *active_section_list = NULL;


int main(int argc, char **argv)
{
#if defined(_MSC_VER) && 0
	os_init_heap();
#endif
	
	atexit(final_cleanup);

#ifdef BTYACC_USE_SIGNAL_HANDLING
    signal_setup();
#endif

    active_section_list = section_list_btyaccpa;
    getargs(argc, argv);
    line_format = get_section("line_position");

    BTYACC_INTERRUPTION_CHECK();
    open_input_files();
    BTYACC_INTERRUPTION_CHECK();
    reader();
    BTYACC_INTERRUPTION_CHECK();
    lr0();
    BTYACC_INTERRUPTION_CHECK();
    lalr();
    BTYACC_INTERRUPTION_CHECK();
    make_parser();
    BTYACC_INTERRUPTION_CHECK();
    verbose();
    BTYACC_INTERRUPTION_CHECK();
    output();
    BTYACC_INTERRUPTION_CHECK();
    done(0);
}
