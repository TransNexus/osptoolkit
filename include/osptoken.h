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
 * osptoken.h - OSP token objects
 */

#ifndef _OSPTOKEN_H
#define _OSPTOKEN_H

#include "osp.h"
#include "osplist.h"
#include "ospxmlelem.h"


/* max size of a token (bytes) */

#define OSPC_TOKENMAXSIZE        3200
#define OSPC_TOKENMAXSTRINGSIZE  6*OSPC_TOKENMAXSIZE

/* the basic token structure */

typedef struct
{
    OSPTLISTLINK   ospmTokenLink;
    unsigned       ospmTokenLen;
    unsigned char *ospmTokenVal;
}
OSPTTOKEN;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

#ifndef OSPC_DEBUG
#define OSPPTokenGetSize(ospvToken)  ((ospvToken)?((ospvToken)->ospmTokenLen):0)
#define OSPPTokenGetValue(ospvToken) ((ospvToken)?((ospvToken)->ospmTokenVal):OSPC_OSNULL)
#define OSPPTokenDelete(ospvToken)  OSPM_FREE(*ospvToken); *ospvToken = OSPC_OSNULL;
#define OSPPTokenToElement(ospvToken, ospvElem) \
    OSPPMsgBinToElement(OSPPTokenGetSize(ospvToken), \
        OSPPTokenGetValue(ospvToken), \
        (const unsigned char *)OSPPMsgGetElemName(ospeElemToken), ospvElem, OSPC_TRUE)
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/

    OSPTTOKEN *OSPPTokenNew(unsigned, const unsigned char *);
    unsigned OSPPTokenFromElement(OSPTXMLELEM *, OSPTTOKEN  **);

#ifdef OSPC_DEBUG
    unsigned OSPPTokenGetSize(OSPTTOKEN *);
    const unsigned char *OSPPTokenGetValue(OSPTTOKEN *);
    void OSPPTokenDelete(OSPTTOKEN **);
    unsigned OSPPTokenToElement(OSPTTOKEN *, OSPTXMLELEM **);
#endif

#ifdef __cplusplus
}
#endif

#endif

