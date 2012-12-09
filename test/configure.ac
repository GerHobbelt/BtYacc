# btyacc test code framework setup
#
# Run configure to set up the portability settings for your local platform
# so the test*.y test cases will compile without a hitch.
#

AC_PREREQ([2.69])
AC_INIT([btyacc-test-rig], [0.1], [ger@hobbelt.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([test-framework-header.c])
AC_CONFIG_HEADERS([portability-config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h stdlib.h string.h sys/param.h sys/socket.h syslog.h termios.h unistd.h conio.h ctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([alarm memset strncasecmp strcasecmp strpbrk strnicmp stricmp strupr getch getche tcgetattr tcsetattr vsnprintf snprintf])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
