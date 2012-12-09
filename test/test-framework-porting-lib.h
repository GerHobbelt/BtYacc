
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_CONIO_H)
#include <conio.h>
#endif
#if defined(HAVE_TERMIOS_H)
#include <termios.h>
#endif


#if defined(HAVE_GETCHE)

int getkey(void)
{
    char ch;

    ch = getche();

    if (ch == '\x04') /* Control-D */
        return EOF;
    return ch;
}

#elif defined(HAVE_TCGETATTR) && defined(HAVE_TCSETATTR)

int getkey(void)
{
    int ch;
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON /* | ECHO */ );
    // as per http://www.daniweb.com/software-development/c/threads/148447/getch-implementation-in-unix: required for HP-UX:
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr (STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    if (ch == '\x04') /* Control-D */
        return EOF;
    return ch;
}

#else

#error "You must provide a getch() equivalent for your platform"

#endif



#ifndef EPSILON
#ifdef FLT_EPSILON
#define EPSILON         FLT_EPSILON
#else
#define EPSILON         1.0e-7
#endif
#endif


