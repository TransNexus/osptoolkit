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
 * ospreauthrsp.h - OSP reauthorisation response objects
 */

#ifndef _OSPREAUTHRSP_H
#define _OSPREAUTHRSP_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"
#include "ospdest.h"
#include "ospstatus.h"
#include "osptnaudit.h"

typedef struct
{
    OSPTTIME          ospmReauthRspTimestamp;
    unsigned char    *ospmReauthRspMessageId;
    unsigned char    *ospmReauthRspComponentId;
    OSPTSTATUS       *ospmReauthRspStatus;
    OSPTTRXID         ospmReauthRspTrxId;
    OSPTDEST         *ospmReauthRspDest;
    OSPTTNAUDIT      *ospmReauthRspTNAudit;
}
OSPTREAUTHRSP;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospreauthrsp.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG

#define OSPPReauthRspSetTimestamp(ospvReauthRsp,ospvTime) \
    (ospvReauthRsp)->ospmReauthRspTimestamp = (ospvTime)

/* ComponentId */
#define OSPPReauthRspHasComponentId(ospvReauthRsp) \
    ((ospvReauthRsp)->ospmReauthRspComponentId != OSPC_OSNULL)

#define OSPPReauthRspHasStatus(ospvReauthRsp) \
    (ospvReauthRsp)?((ospvReauthRsp)->ospmReauthRspStatus != OSPC_OSNULL):OSPC_FALSE
#define OSPPReauthRspGetStatus(ospvReauthRsp) ((ospvReauthRsp)->ospmReauthRspStatus)

#define OSPPReauthRspSetTrxId(ospvReauthRsp, ospvNum) \
    (ospvReauthRsp)->ospmReauthRspTrxId = ospvNum

#define OSPPReauthRspHasDest(ospvReauthRsp) \
    (ospvReauthRsp)?((ospvReauthRsp)->ospmReauthRspDest != OSPC_OSNULL):OSPC_FALSE
#define OSPPReauthRspSetDest(ospvReauthRsp,ospvDest) \
    (ospvReauthRsp)->ospmReauthRspDest = (ospvDest)
#endif
/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

    OSPTREAUTHRSP   *OSPPReauthRspNew(void);
    void            OSPPReauthRspDelete(OSPTREAUTHRSP **);
    int             OSPPReauthRspFromElement(OSPTXMLELEM *, OSPTREAUTHRSP **);
    void            OSPPReauthRspSetComponentId(OSPTREAUTHRSP *, unsigned char *);
    unsigned        OSPPReauthRspHasMessageId(OSPTREAUTHRSP *);
    void            OSPPReauthRspSetMessageId(OSPTREAUTHRSP *, unsigned char *);
    void            OSPPReauthRspMessageIdFromElement(OSPTXMLELEM *, unsigned char **);
    void            OSPPReauthRspComponentIdFromElement(OSPTXMLELEM *, unsigned char **);

#ifdef OSPC_DEBUG

    void            OSPPReauthRspSetTimestamp(OSPTREAUTHRSP *, OSPTTIME);
    unsigned        OSPPReauthRspHasComponentId(OSPTREAUTHRSP *);

    unsigned        OSPPReauthRspHasStatus(OSPTREAUTHRSP *ospvReauthRsp);
    OSPTSTATUS      *OSPPReauthRspGetStatus(OSPTREAUTHRSP *);
    void            OSPPReauthRspSetTrxId(OSPTREAUTHRSP *, OSPTTRXID);
    unsigned        OSPPReauthRspHasDest(OSPTREAUTHRSP *ospvReauthRsp);
    void            OSPPReauthRspSetDest(OSPTREAUTHRSP *, OSPTDEST *);
#endif

    OSPTTNAUDIT     *OSPPReauthRspGetTNAudit(OSPTREAUTHRSP *);

#ifdef __cplusplus
}
#endif

#endif

