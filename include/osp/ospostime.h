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
 * ospostime.h - OS-specific time structures and prototypes
 */

#ifndef _OSPOSTIME_H
#define _OSPOSTIME_H

#include "time.h"

#include "osp/osp.h"


/* time limits */

#define OSPC_TIMEMIN    ((OSPTTIME)0)
#define OSPC_TIMEMAX ((OSPTTIME)LONG_MAX)


/* OSP string time format is YYYY-MM-DDThh:mm:ssZ */

#define OSPC_TIMESTRINGSIZE (sizeof("YYYY-MM-DDThh:mm:ssZ") + 1)

/**/
/*-----------------------------------------------------------------------*
 * Function prototypes
 *-----------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" 
{
#endif

    unsigned OSPPOSTimeCalToString(OSPTTIME, char *);
    unsigned OSPPOSTimeStringToCal(const char *, OSPTTIME *);

    int
    OSPPOSTimeGetTime(
        OSPTTIME *ospvTime,
        unsigned int *ospvTimeMS);

    int
    OSPPOSTimeFormatGMTTime(
        OSPTTIME ospvTime,
        char *ospvFormat,
        char *ospvTimeString);


#ifdef __cplusplus
}
#endif



#endif

