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
 * ospcallid.h - OSP call identifier objects
 */

#ifndef _OSPCALLID_H
#define _OSPCALLID_H

#include "osp.h"
#include "osplist.h"
#include "ospxmlelem.h"


/* max size of a call ID (bytes) */

#define OSPC_CALLID_MAXSIZE        256

/* the basic call identifier structure */

typedef struct
{
    OSPTLISTLINK   ospmCallIdLink;
    unsigned       ospmCallIdLen;
    unsigned char *ospmCallIdVal;
}
OSPTCALLID;


/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

    OSPTCALLID    *OSPPCallIdNew(unsigned, const unsigned char *);
    void           OSPPCallIdDelete(OSPTCALLID **);
    unsigned       OSPPCallIdFromElement(OSPTXMLELEM *, OSPTCALLID  **);
    unsigned       OSPPCallIdToElement(OSPTCALLID *, OSPTXMLELEM **, OSPTBOOL);

    unsigned       OSPPCallIdGetSize(OSPTCALLID *);
    unsigned char *OSPPCallIdGetValue(OSPTCALLID *);


#ifdef __cplusplus
}
#endif

#endif /* _OSPCALLID_H */

