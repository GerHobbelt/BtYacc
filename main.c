#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "log.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
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
int Eflag = 0;

char *target_dir = NULL;
char *file_prefix = NULL;
char *name_prefix = "yy";
char *name_uc_prefix = NULL;
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

char *action_file_name = NULL;
char *code_file_name = NULL;
char *defines_file_name = NULL;
char *input_file_name = "";
char *output_file_name = NULL;
char *text_file_name = NULL;
char *union_file_name = NULL;
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
FILE *verbose_file = NULL;      /*  y.output                                        */

int nitems = 0;
int nrules = 0;
int nsyms = 0;
int ntokens = 0;
int nvars = 0;

int   start_symbol = 0;
char  **symbol_name = NULL;
Yshort *symbol_value = NULL;
Yshort *symbol_prec = NULL;
char  *symbol_assoc = NULL;

Yshort *ritem = NULL;
Yshort *rlhs = NULL;
Yshort *rrhs = NULL;
Yshort *rprec = NULL;
char  *rassoc = NULL;
Yshort **derives = NULL;
char *nullable = NULL;


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


void done(int k)
{
    file_deletion(action_file, action_file_name);
    file_deletion(text_file, text_file_name);
    file_deletion(union_file, union_file_name);
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


static SPLINT_NO_RETURN void usage(void)
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
            "  -v           Write description of parser to `y.output'\n");
  exit(1);
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
            { char **ps;
              char *var_name = s + 1;
              extern char *defd_vars[];

              for (ps = &defd_vars[0]; *ps; ++ps) {
                if(strcmp(*ps,var_name)==0) {
                  error(lineno, 0, 0, "Preprocessor variable %s already defined", var_name);
                }
              }
              *ps = (char *)MALLOC(strlen(var_name)+1);

              if (*ps == 0)
                 no_space();

              strcpy(*ps, var_name);
              *++ps = NULL;
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
            tflag = 1;
            break;

        case 'v':
            vflag = 1;
            break;

        case 'S':
            if (*++s)
                active_section_list = read_skel(s);
            else if (++i < argc)
                active_section_list = read_skel(argv[i]);
            else
                usage();
            continue;

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
                tflag = 1;
                break;

            case 'v':
                vflag = 1;
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

        file_prefix = strdup(input_file_name);

        if (!file_prefix) no_space();

        // strip off any leading directories from the input file path:
        s = strrchr(file_prefix, '/');
        if (!s) s = file_prefix; else s++;
        s2 = strrchr(s, '\\');
        if (!s2) s2 = s; else s2++;
        s = strrchr(s2, ':');
        if (!s) s = s2; else s++;

        if ((s2 = strrchr(s, '.')))
          *s2 = 0;

        memmove(file_prefix, s, strlen(s) + 1);
      }
    }
    if (!file_prefix || !*file_prefix) {
        file_prefix = "y";
    }

    if (name_prefix && *name_prefix) {
        char *s2;

		if (0 == strcmp(name_prefix, "@") && 0 != strcmp(file_prefix, "y"))
		{
			name_prefix = strdup(file_prefix);
		}

		/* sanitize the prefix to be suitable as part of variable names: */
		s2 = name_prefix;
		if (*s2 && !isalpha(*s2) && *s2 != '_')
		{
			*s2 = 'y';
		}
		for (++s2; *s2; s2++)
		{
			if (!isalnum(*s2) && *s2 != '_')
			{
				*s2 = '_';
			}
		}
    }
    if (!name_prefix || !*name_prefix) {
        name_prefix = "yy";
    }
    name_uc_prefix = strdup(name_prefix);
	strupr(name_uc_prefix);

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

				target_dir = strdup(input_file_name);
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

char *allocate(unsigned n)
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

static void create_union_file(void)
{
    size_t i, len;
    char* tmpdir = getenv_tempdir();

    len = strlen(tmpdir);
    i = len + (sizeof(temp_form));

    union_file_name = (char *)MALLOC(i);

    if (union_file_name == 0)
       no_space();

    strcpy(union_file_name, tmpdir);

    if (len)
    {
        union_file_name[len] = DIR_CHAR;
        ++len;
    }

    strcpy(union_file_name + len, temp_form);
    union_file_name[len + 5] = 'u';
    union_file = create_temporary_file(union_file_name);
}

void create_files(void)
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

    strcpy(action_file_name, tmpdir);
    strcpy(text_file_name, tmpdir);

    if (len)
    {
        action_file_name[len] = DIR_CHAR;
        text_file_name[len] = DIR_CHAR;
        ++len;
    }

    strcpy(action_file_name + len, temp_form);
    strcpy(text_file_name + len, temp_form);

    action_file_name[len + 5] = 'a';
    text_file_name[len + 5] = 't';

    action_file = create_temporary_file(action_file_name);
    text_file = create_temporary_file(text_file_name);

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
        code_file_name = output_file_name;
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


void open_output_files(void)
{
    /* do this only once, first time is on demand, i.e. as late as possible */
    if (!action_file && !text_file && !verbose_file && !defines_file && !output_file && !code_file)
    {
        create_files();

        if (vflag)
        {
            verbose_file = fopen(verbose_file_name, "w");
            if (verbose_file == 0)
                open_error(verbose_file_name);
        }

        if (dflag)
        {
            defines_file = fopen(defines_file_name, "w");
            if (defines_file == 0)
                open_error(defines_file_name);

            create_union_file();
        }

        output_file = fopen(output_file_name, "w");
        if (output_file == 0)
            open_error(output_file_name);

        if (rflag)
        {
            code_file = fopen(code_file_name, "w");
            if (code_file == 0)
                open_error(code_file_name);
        }
        else
        {
            code_file = output_file;
        }

        write_section("banner");
    }
}


struct section *active_section_list = NULL;

#ifdef _DEBUG
void atexit_handler(void)
{
	printf("atexit!");
}
#endif

int main(int argc, char **argv)
{
#ifdef BTYACC_USE_SIGNAL_HANDLING
    signal_setup();
#endif

    active_section_list = section_list_btyaccpa;
    getargs(argc, argv);
	line_format = get_section("line_position");

#ifdef _DEBUG
	atexit(atexit_handler);
#endif

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
