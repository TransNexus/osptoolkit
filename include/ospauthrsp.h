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
 * ospauthrsp.h - OSP authorisation response objects
 */

#ifndef _OSPAUTHRSP_H
#define _OSPAUTHRSP_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"
#include "ospdest.h"
#include "ospstatus.h"
#include "osptnaudit.h"
#include "ospciscoext.h"

typedef struct
{
    OSPTTIME          ospmAuthRspTimestamp;
    unsigned char    *ospmAuthRspMessageId;
    unsigned char    *ospmAuthRspComponentId;
    OSPTSTATUS       *ospmAuthRspStatus;
    OSPTTNAUDIT      *ospmAuthRspTNAudit;
    OSPTTRXID         ospmAuthRspTrxId;
    OSPTLIST          ospmAuthRspDest;
    unsigned          ospmAuthRspTNDelayLimit;
    unsigned          ospmAuthRspTNDelayPref;
    unsigned          ospmNumDests;
    OSPTCSAUDIT      *ospmAuthRspCSAudit;

}
OSPTAUTHRSP;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospauthrsp.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG

#define OSPPAuthRspSetTimestamp(ospvAuthRsp,ospvTime) \
    (ospvAuthRsp)->ospmAuthRspTimestamp = (ospvTime)

/* ComponentId */
#define OSPPAuthRspHasComponentId(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmAuthRspComponentId != OSPC_OSNULL)

#define OSPPAuthRspHasStatus(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmAuthRspStatus != OSPC_OSNULL)
#define OSPPAuthRspGetStatus(ospvAuthRsp) \
    (ospvAuthRsp)->ospmAuthRspStatus

#define OSPPAuthRspSetTrxId(ospvAuthRsp, ospvTrxId) \
    (ospvAuthRsp)->ospmAuthRspTrxId = ospvTrxId

#define OSPPAuthRspFirstDest(ospvAuthRsp) \
    OSPPListFirst((OSPTLIST *) &(ospvAuthRsp->ospmAuthRspDest))
#define OSPPAuthRspHasDest(ospvAuthRsp) \
    ((OSPPAuthRspFirstDest(ospvAuthRsp)) != (void *)OSPC_OSNULL)
#define OSPPAuthRspNextDest(ospvAuthRsp,ospvDest) \
    OSPPListNext((OSPTLIST *)&((ospvAuthRsp)->ospmAuthRspDest), (void *)ospvDest)

#define OSPPAuthRspHasTNDelayLimit(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmAuthRspTNDelayLimit)
#define OSPPAuthRspSetTNDelayLimit(ospvAuthRsp, ospvTNDelayLimit) \
    (ospvAuthRsp)->ospmAuthRspTNDelayLimit = ospvTNDelayLimit
#define OSPPAuthRspGetTNDelayLimit(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmAuthRspTNDelayLimit)

#define OSPPAuthRspHasTNDelayPref(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmAuthRspTNDelayPref)
#define OSPPAuthRspSetTNDelayPref(ospvAuthRsp, ospvTNDelayPref) \
    (ospvAuthRsp)->ospmAuthRspTNDelayPref = ospvTNDelayPref

#define OSPPAuthRspHasNumDests(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmNumDests)
#define OSPPAuthRspIncNumDests(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmNumDests++)
#define OSPPAuthRspGetNumDests(ospvAuthRsp) \
    ((ospvAuthRsp)->ospmNumDests)

#endif /* OSPC_DEBUG */
#define OSPPAuthRspDestHasNumber(ospvDest) OSPPDestHasNumber(ospvDest)
#define OSPPAuthRspDestSetNumber(ospvDest,ospvNum) OSPPDestSetNumber(ospvDest,ospvNum)
#define OSPPAuthRspDestGetNumber(ospvDest) OSPPDestGetNumber(ospvDest)
#define OSPPAuthRspDestHasAddr(ospvDest) OSPPDestHasAddr(ospvDest)
#define OSPPAuthRspDestSetAddr(ospvDest,ospvAddr) OSPPDestSetAddr(ospvDest,ospvAddr)
#define OSPPAuthRspDestGetAddr(ospvDest) OSPPDestGetAddr(ospvDest)
#define OSPPAuthRspDestHasValidAfter(ospvDest) OSPPDestHasValidAfter(ospvDest)
#define OSPPAuthRspDestSetValidAfter(ospvDest,ospvTime) OSPPDestSetValidAfter(ospvDest,ospvTime)
#define OSPPAuthRspDestGetValidAfter(ospvDest) OSPPDestGetValidAfter(ospvDest)
#define OSPPAuthRspDestHasValidUntil(ospvDest) OSPPDestHasValidUntil(ospvDest)
#define OSPPAuthRspDestSetValidUntil(ospvDest,ospvTime) OSPPDestSetValidUntil(ospvDest,ospvTime)
#define OSPPAuthRspDestGetValidUntil(ospvDest) OSPPDestGetValidUntil(ospvDest)
#define OSPPAuthRspDestHasAuthority(ospvDest) OSPPDestHasAuthority(ospvDest)
#define OSPPAuthRspDestSetAuthority(ospvDest,ospvAuth) OSPPDestSetAuthority(ospvDest,ospvAuth)
#define OSPPAuthRspDestGetAuthority(ospvDest) OSPPDestGetAuthority(ospvDest)
#define OSPPAuthRspDestHasCallId(ospvDest) OSPPDestHasCallId(ospvDest)
#define OSPPAuthRspDestGetCallId(ospvDest) OSPPDestGetCallId(ospvDest)
#define OSPPAuthRspDestGetCallIdSize(ospvDest) OSPPDestGetCallIdSize(ospvDest)
#define OSPPAuthRspDestGetCallIdValue(ospvDest) OSPPDestGetCallIdValue(ospvDest)
#define OSPPAuthRspDestHasToken(ospvDest) OSPPDestHasToken(ospvDest) 
#define OSPPAuthRspDestAddToken(ospvDest, ospvToken) OSPPDestAddToken(ospvDest, ospvToken)
#define OSPPAuthRspDestFirstToken(ospvDest) OSPPDestFirstToken(ospvDest)
#define OSPPAuthRspDestNextToken(ospvDest, ospvToken) OSPPDestNextToken(ospvDest, ospvToken)
#define OSPPAuthRspDestTokenGetSize(ospvToken) OSPPTokenGetSize(ospvToken)
#define OSPPAuthRspDestTokenGetValue(ospvToken) OSPPTokenGetValue(ospvToken)
#define OSPPAuthRspDestHasLimit(ospvDest) OSPPDestHasLimit(ospvDest) 
#define OSPPAuthRspDestGetLimit(ospvDest) OSPPDestGetLimit(ospvDest) 
#define OSPPAuthRspDestSetLimit(ospvDest, ospvLimit) OSPPDestSetLimit(ospvDest, ospvLimit)

#ifdef __cplusplus
extern "C" 
{
#endif


    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/

    OSPTAUTHRSP *OSPPAuthRspNew(void);
    void        OSPPAuthRspDelete(OSPTAUTHRSP **);
    unsigned    OSPPAuthRspFromElement(OSPTXMLELEM *, OSPTAUTHRSP **);
    OSPTDEST    *OSPPAuthRspAddDest(OSPTAUTHRSP *, OSPTDEST *);
    void        OSPPAuthRspSetComponentId(OSPTAUTHRSP *, unsigned char *);
    void        OSPPAuthRspComponentIdFromElement(OSPTXMLELEM *, unsigned char **);
    unsigned    OSPPAuthRspHasMessageId(OSPTAUTHRSP *);
    void        OSPPAuthRspSetMessageId(OSPTAUTHRSP *, unsigned char *);
    void        OSPPAuthRspMessageIdFromElement(OSPTXMLELEM *, unsigned char **);
    unsigned char *OSPPAuthRspGetMessageId(OSPTAUTHRSP *);

#ifdef OSPC_DEBUG

    void        OSPPAuthRspSetTimestamp(OSPTAUTHRSP *, OSPTTIME);

    unsigned       OSPPAuthRspHasComponentId(OSPTAUTHRSP *);

    unsigned     OSPPAuthRspHasStatus(OSPTAUTHRSP *);
    OSPTSTATUS  *OSPPAuthRspGetStatus(OSPTAUTHRSP *);

    void        OSPPAuthRspSetTrxId(OSPTAUTHRSP *, OSPTTRXID);

    unsigned    OSPPAuthRspHasDest(OSPTAUTHRSP *ospvAuthRsp);
    OSPTDEST   *OSPPAuthRspFirstDest(OSPTAUTHRSP *);
    OSPTDEST   *OSPPAuthRspNextDest(OSPTAUTHRSP *, OSPTDEST *);

    unsigned     OSPPAuthRspHasTNDelayLimit(OSPTAUTHRSP *);
    void         OSPPAuthRspSetTNDelayLimit(OSPTAUTHRSP *, unsigned);
    unsigned     OSPPAuthRspGetTNDelayLimit(OSPTAUTHRSP *);

    unsigned     OSPPAuthRspHasTNDelayPref(OSPTAUTHRSP *);
    void         OSPPAuthRspSetTNDelayPref(OSPTAUTHRSP *, unsigned);

    unsigned    OSPPAuthRspHasNumDests(OSPTAUTHRSP *);
    void        OSPPAuthRspIncNumDests(OSPTAUTHRSP *);
    unsigned    OSPPAuthRspGetNumDests(OSPTAUTHRSP *);

#endif /* OSPC_DEBUG */

    OSPTTNAUDIT *OSPPAuthRspGetTNAudit(OSPTAUTHRSP *);
    OSPTCSAUDIT *OSPPAuthRspGetCSAudit(OSPTAUTHRSP *);
    int         OSPPAuthRspHasCSAudit(OSPTAUTHRSP *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHRSP_H */
