/**************************************************************************
*** COPYRIGHT (c) 2002 by TransNexus, Inc.                              ***
***                                                                     ***
*** This software is property of TransNexus, Inc.                       ***
*** This software is freely available under license from TransNexus.    ***
*** The license terms and conditions for free use of this software by   ***
*** third parties are defined in the OSP Toolkit Software License       ***
*** Agreement (LICENSE.txt).  Any use of this software by third         ***
*** parties, which does not comply with the terms and conditions of the ***
*** OSP Toolkit Software License Agreement is prohibited without        ***
*** the prior, express, written consent of TransNexus, Inc.             ***
***                                                                     ***
*** Thank you for using the OSP ToolKit(TM).  Please report any bugs,   ***
*** suggestions or feedback to support@transnexus.com                   ***
***                                                                     ***
**************************************************************************/








/*
 * ospostime.c - OS-specific time functions
 */

#include "assert.h"

#include "time.h"

#ifndef _WIN32
#include <sys/timeb.h>
#else
#include <sys\timeb.h>
#endif

#ifndef _WIN32
#include "sys/time.h"
#include "unistd.h"
#endif

#include "osp.h"
#include "ospostime.h"

OSPTTIME OSPPOSTimeConvertToGMT(int, int, int, int, int, int);



/*  A leap year is defined as 1) multiple of 4; 2) Not a multiple */
/*  of 100, unless 3) it's a multiple of 400                      */

#define OSPM_IS_LEAP(y) ((y) % 4) == 0 && (! ((y) % 100) == 0 || ((y) % 400) == 0)

/**/
/*-----------------------------------------------------------------------*
 * OSPPOSTimeConvertToGMT() - convert calendar time to display string
 *-----------------------------------------------------------------------*/
OSPTTIME
    OSPPOSTimeConvertToGMT(
    int month,          /* Month (1-12!) */
    int day,            /* Day of month */
    int year,           /* Year (minus 1900) */
    int hour,           /* Hour */
    int minute,         /* Minute */
    int second)         /* Second */
{
    OSPTTIME days;
    static int DaysAtMonth[]={0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    /* Add up number of straight days + number of leap days + days */
    /* up to the month + the day of month. */
    year+=1900;
    days=(year*365)+(year/4)-(year/100)+(year/400)+DaysAtMonth[month-1] + day;

    /* If we haven't hit Feb 29, and this is a leap year, we need to */
    /* subtract out the leap day that was added above for this year */
    if((month<3) && OSPM_IS_LEAP(year))
    {
        --days;
    }
    days -= 719528;     /* Subtract out Jan 1, 1970 base date */
    return((days * 86400) + (hour * 3600) + (minute * 60) + second);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPOSTimeCalToString() - convert calendar time to display string
 *-----------------------------------------------------------------------*/

unsigned                          /* returns error code (0 if no error) */
OSPPOSTimeCalToString(
    OSPTTIME  ospvCalTime,        /* calendar time (in UTC) */
    char     *ospvTimeString      /* where to put time string */
)
{
#ifndef _WIN32
    struct tm t;
#else
    struct tm *t;
#endif

    unsigned  ospvErrCode = OSPC_ERR_NO_ERROR;
    size_t    chars;

    /*
     * This is all pretty much ANSI C standard - but be sure to use
     * thread-safe versions.
     */

    /*
     * Convert to struct tm, preserving UTC (the ignored return is just
     * a pointer to the struct tm).
     */
#ifndef _WIN32
    (void)gmtime_r(&ospvCalTime, &t);
#else
    t = gmtime(&ospvCalTime);
#endif


#ifndef _WIN32
    /* format the output string */
    chars = strftime(ospvTimeString, OSPC_TIMESTRINGSIZE,
        "%Y-%m-%dT%H:%M:%SZ", &t);
#else
    /* format the output string */
    chars = strftime(ospvTimeString, OSPC_TIMESTRINGSIZE,
        "%Y-%m-%dT%H:%M:%SZ", t);
#endif


    /* make sure the strftime() call was okay */
    if (chars == OSPC_TIMESTRINGSIZE)
    {
        ospvErrCode = (unsigned)OSPC_ERR_OS_FAILURE;
    }
    else
    {
        ospvErrCode = OSPC_ERR_NO_ERROR;
    }

    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPOSTimeStringToCal() - convert display string to calendar time
 *-----------------------------------------------------------------------*/

unsigned                          /* returns error code (0 if no error) */
OSPPOSTimeStringToCal(
    const char *ospvTimeString,   /* time string */
    OSPTTIME   *ospvCalTime       /* where to put calendar time */
)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    int      year, month, day, hour, minute, second;

    /*
     * Unfortunately, this function is somewhat of a pain. The real
     * problem is daylight savings time. The only standard way to
     * generate a calendar time value is with the function mktime(),
     * and that function takes a struct tm input, which is in local
     * time (e.g. with time zone and daylight savings time accounted
     * for). So, even though we're starting with UTC and ending
     * with UTC, the intermediate step requires local time.
     */

    /* build up a struct tm from the string */
    /* OSP string time format is YYYY-MM-DDThh:mm:ssZ */
    year   = (ospvTimeString[0] - '0')*1000 + (ospvTimeString[1] - '0')*100 +
        (ospvTimeString[2] - '0')*10   + (ospvTimeString[3] - '0');
    month  = (ospvTimeString[5] - '0')*10   + (ospvTimeString[6] - '0')  - 1;
    day    = (ospvTimeString[8] - '0')*10   + (ospvTimeString[9] - '0');
    hour   = (ospvTimeString[11] - '0')*10  + (ospvTimeString[12] - '0');
    minute = (ospvTimeString[14] - '0')*10  + (ospvTimeString[15] - '0');
    second = (ospvTimeString[17] - '0')*10  + (ospvTimeString[18] - '0');

    *ospvCalTime=OSPPOSTimeConvertToGMT(month+1,day,year-1900,hour,minute,second);

    ospvErrCode=OSPC_ERR_NO_ERROR;
    return(ospvErrCode);
}


int
OSPPOSTimeGetTime(
    OSPTTIME *ospvTime,
    unsigned int *ospvMilliseconds)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if ((ospvTime == OSPC_OSNULL) && (ospvMilliseconds == OSPC_OSNULL))
    {
        errorcode = OSPC_ERR_INVALID_POINTER;
        OSPM_DBGERRORLOG(errorcode, 
            "Null pointers specified for time and mstime");
    }
    else
    {

#ifndef _WIN32 /* UNIX */
        struct timeb   timenow;

        OSPM_MEMSET(&timenow, 0, sizeof(struct timeb));
        errorcode = ftime(&timenow);
#else

        struct _timeb timenow;

        OSPM_MEMSET(&timenow, 0, sizeof(struct _timeb));
        _ftime(&timenow);
#endif

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            if (ospvTime != OSPC_OSNULL)
            {
                *ospvTime = timenow.time;
            }

            if (ospvMilliseconds != OSPC_OSNULL)
            {
                *ospvMilliseconds = timenow.millitm;
            }
        }
        else
        {
            errorcode = OSPC_ERR_TIME_ERROR;
            OSPM_DBGERRORLOG(errorcode, "ftime reported an error");
        }
    }
    return errorcode;
}



int
OSPPOSTimeFormatGMTTime(
    OSPTTIME ospvTime,
    char *ospvFormat,
    char *ospvTimeString)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    int chars = 0;

    /* Converts a time in seconds since the BASETIME and then formats time
        according to the format string. ospvTimeString be provided by the
        caller and must be at least OSPC_TIMESTRINGSIZE characters */

#ifndef _WIN32
    struct tm t;
    OSPM_MEMSET( &t, 0, sizeof(struct tm));
#else
    struct tm *t;
#endif

    if ((ospvFormat == OSPC_OSNULL) || (ospvTimeString == OSPC_OSNULL))
    {
        errorcode = OSPC_ERR_INVALID_POINTER;
        OSPM_DBGERRORLOG(errorcode, 
            "Invalid pointer to format or output string");
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
#ifndef _WIN32
        (void)gmtime_r(&ospvTime, &t);
#else
        t = gmtime(&ospvTime);
#endif

#ifndef _WIN32
        /* format the output string */
        chars = strftime(ospvTimeString, OSPC_TIMESTRINGSIZE,
            ospvFormat, &t);
#else
        /* format the output string */
        chars = strftime(ospvTimeString, OSPC_TIMESTRINGSIZE,
            ospvFormat, t);
#endif

        if (chars == 0) 
        {
            errorcode = OSPC_ERR_TIME_ERROR;
            OSPM_DBGERRORLOG(errorcode, "error occurred formatting time");
        }
    }

    return errorcode;
}

