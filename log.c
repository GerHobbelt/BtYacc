#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "log.h"
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void BtYacc_logs(char const * text)
{
    if (fputs(text, stderr) == EOF)
    {
       perror("BtYacc: fputs");
       abort();
    }

	if (vflag && log_file)
	{
		if (fputs(text, log_file) == EOF)
		{
		   perror("BtYacc: fputs");
		   abort();
		}
	}
}

void BtYacc_logf(char const * format, ...)
{
	va_list vl;

	va_start(vl, format);
	if (vfprintf(stderr, format, vl) < 0)
	{
		perror("BtYacc: vfprintf");
		va_end(vl);
		abort();
	}
	va_end(vl);

	if (vflag && log_file)
	{
		va_start(vl, format);
		if (vfprintf(log_file, format, vl) < 0)
		{
			perror("BtYacc: vfprintf");
			va_end(vl);
			abort();
		}
		va_end(vl);
	}
}
