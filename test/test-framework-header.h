/*
BtYacc test grammars: Framework Header Section
*/

#if defined(_MSC_VER)
#include "portability-config-MSVC.h"
#elif defined(__MINGW32__)
#include "portability-config-MingW.h"
#else
#include "portability-config.h"
#endif

#include <stdio.h>
#include <assert.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_MEMORY_H)
#include <memory.h>
#endif
#if defined(HAVE_STDINT_H)
#include <stdint.h> 
#endif
#if defined(HAVE_CTYPE_H)
#include <ctype.h>
#endif
#if defined(HAVE_STRINGS_H)
#include <strings.h> 
#endif
#if defined(HAVE_STRING_H)
#include <string.h> 
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_CONIO_H)
#include <conio.h>
#endif
#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h> 
#endif
#if defined(HAVE_SYSLOG_H)
#include <syslog.h> 
#endif
#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h> 
#endif
#if defined(HAVE_SYS_SOCKET_H)
#include <sys/socket.h> 
#endif
#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h> 
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h> 
#endif
#if defined(HAVE_TERMIOS_H)
#include <termios.h> 
#endif




#define YYERROR_DETAILED




typedef double yystype_t;
#define YYSTYPE	yystype_t

#define YYDBPR(yystype)    BTYACC_DEBUG_LOG_PARTIAL("%f", (double)(yystype))





/**
Get a key, don't wait for ENTER key to get it.
*/
int getkey(void);

#include "test-framework-porting-lib.h"


