#ifdef BTYACC_BUILD_USE_CONFIGURATION_HEADER
#include "build.h"   // System settings from the build configuration
#endif

#include "defs.h"
#include "log.h"
#include "write.h"


#if defined(_MSC_VER) && (defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(WIN64))


#if defined (_DEBUG)

_CrtMemState memdbg_state_snapshot1;
int trigger_memdump = 0;

int trigger_debugger = 0;


/*
 * Define our own reporting function.
 * We'll hook it into the debug reporting
 * process later using _CrtSetReportHook.
 */
int dbg_report_function(int report_type, char *usermsg, int *retval)
{
    /*
     * By setting retVal to zero, we are instructing _CrtDbgReport
     * to continue with normal execution after generating the report.
     * If we wanted _CrtDbgReport to start the debugger, we would set
     * retVal to one.
     */
    *retval = !!trigger_debugger;

    /*
     * When the report type is for an ASSERT,
     * we'll report some information, but we also
     * want _CrtDbgReport to get called -
     * so we'll return TRUE.
     *
     * When the report type is a WARNing or ERROR,
     * we'll take care of all of the reporting. We don't
     * want _CrtDbgReport to get called -
     * so we'll return FALSE.
     */
    switch (report_type)
    {
    default:
    case _CRT_WARN:
    case _CRT_ERROR:
    case _CRT_ERRCNT:
        fputs(usermsg, stderr);
        fflush(stderr);
        return 0;

    case _CRT_ASSERT:
        fputs(usermsg, stderr);
        fflush(stderr);
        break;
    }
    return 1;
}


void report_mem_analysis(void)
{
    _CrtMemState msNow;

    if (!_CrtCheckMemory())
    {
        fprintf(stderr, ">>>Failed to validate memory heap<<<\n");
    }

    /* only dump leaks when there are in fact leaks */
    _CrtMemCheckpoint(&msNow);

    if (msNow.lCounts[_CLIENT_BLOCK] != 0
        || msNow.lCounts[_NORMAL_BLOCK] != 0
        || (_crtDbgFlag & _CRTDBG_CHECK_CRT_DF
            && msNow.lCounts[_CRT_BLOCK] != 0)
    )
    {
        /* difference detected: dump objects since start. */
        _RPT0(_CRT_WARN, "============== Detected memory leaks! ====================\n");

        _CrtMemDumpAllObjectsSince(&memdbg_state_snapshot1);
        _CrtMemDumpStatistics(&memdbg_state_snapshot1);
    }
}

#endif



#endif /* (defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(WIN64)) */



void os_init_heap(void)
{
#if defined(_MSC_VER) && (defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(WIN64)) && defined(_DEBUG)
    int i;

    /*
     * Hook in our client-defined reporting function.
     * Every time a _CrtDbgReport is called to generate
     * a debug report, our function will get called first.
     */
    _CrtSetReportHook(dbg_report_function);

    /*
     * Define the report destination(s) for each type of report
     * we are going to generate.  In this case, we are going to
     * generate a report for every report type: _CRT_WARN,
     * _CRT_ERROR, and _CRT_ASSERT.
     * The destination(s) is defined by specifying the report mode(s)
     * and report file for each report type.
     */
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

    // Store a memory checkpoint in the s1 memory-state structure
    _CrtMemCheckpoint(&memdbg_state_snapshot1);

    atexit(report_mem_analysis);

    // Get the current bits
    i = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    // Set the debug-heap flag so that freed blocks are kept on the
    // linked list, to catch any inadvertent use of freed memory
#if 01
    i |= _CRTDBG_DELAY_FREE_MEM_DF;
#endif

    // Set the debug-heap flag so that memory leaks are reported when
    // the process terminates. Then, exit.
    //i |= _CRTDBG_LEAK_CHECK_DF;

    // Clear the upper 16 bits and OR in the desired freqency
    //i = (i & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_16_DF;

    i |= _CRTDBG_CHECK_ALWAYS_DF;

    // Set the new bits
    _CrtSetDbgFlag(i);

//    // set a malloc marker we can use it in the leak dump at the end of the program:
//    (void)_calloc_dbg(1, 1, _CLIENT_BLOCK, __FILE__, __LINE__);
#endif
}

