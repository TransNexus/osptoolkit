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
 * ospreauthreq.h - OSP reauthorisation request objects
 */

#ifndef _OSPREAUTHREQ_H
#define _OSPREAUTHREQ_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"
#include "ospusage.h"
#include "ospaltinfo.h"


typedef struct
{
    OSPTTIME          ospmReauthReqTimestamp;
    unsigned char    *ospmReauthReqMessageId;
    unsigned char    *ospmReauthReqComponentId;
    unsigned          ospmReauthReqRole;
    unsigned          ospmReauthReqHasRole;
    OSPTCALLID        *ospmReauthReqCallId;
    unsigned char     ospmReauthReqSourceNumber[OSPC_E164NUMSIZE];
    unsigned char     ospmReauthReqDestNumber[OSPC_E164NUMSIZE];
    OSPTLIST          ospmReauthReqSourceAlternate;
    OSPTLIST          ospmReauthReqDestinationAlternate;
    OSPTTRXID         ospmReauthReqTrxId;
    int               ospmReauthReqDuration;
    OSPTLIST          ospmReauthReqTokens;
    unsigned long     ospmReauthReqTNCustId;
    unsigned long     ospmReauthReqTNDeviceId;
}
OSPTREAUTHREQ;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospreauthreq.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG

#define OSPPReauthReqHasTimestamp(ospvReauthReq) \
    (ospvReauthReq)?((ospvReauthReq)->ospmReauthReqTimestamp != OSPC_TIMEMIN):OSPC_FALSE
#define OSPPReauthReqSetTimestamp(ospvReauthReq,ospvTime) \
    (ospvReauthReq)->ospmReauthReqTimestamp = (ospvTime)
#define OSPPReauthReqGetTimestamp(ospvReauthReq) (ospvReauthReq)->ospmReauthReqTimestamp

/* ComponentId */
#define OSPPReauthReqHasComponentId(ospvReauthReq) \
    ((ospvReauthReq)->ospmReauthReqComponentId != OSPC_OSNULL)
#define OSPPReauthReqGetComponentId(ospvReauthReq) \
    (ospvReauthReq)->ospmReauthReqComponentId

#define OSPPReauthReqHasCallId(ospvReauthReq) \
    (ospvReauthReq)?((ospvReauthReq)->ospmReauthReqCallId != OSPC_OSNULL):OSPC_FALSE

#define OSPPReauthReqGetCallId(ospvReauthReq) ((ospvReauthReq)->ospmReauthReqCallId)

#define OSPPReauthReqSetSourceNumber(ospvReauthReq,ospvNum) \
    OSPM_STRNCPY((ospvReauthReq)->ospmReauthReqSourceNumber, (ospvNum), \
    OSPC_E164NUMSIZE-1)
#define OSPPReauthReqGetSourceNumber(ospvReauthReq) (ospvReauthReq)?((ospvReauthReq)->ospmReauthReqSourceNumber):OSPC_OSNULL

#define OSPPReauthReqSetDestNumber(ospvReauthReq,ospvNum) \
    OSPM_STRNCPY((ospvReauthReq)->ospmReauthReqDestNumber, (ospvNum), \
    OSPC_E164NUMSIZE-1)
#define OSPPReauthReqGetDestNumber(ospvReauthReq) (ospvReauthReq)?((ospvReauthReq)->ospmReauthReqDestNumber):OSPC_OSNULL

#define OSPPReauthReqHasTrxId(ospvReauthReq) (ospvReauthReq)?((ospvReauthReq)->ospmReauthReqTrxId > 0):OSPC_FALSE
#define OSPPReauthReqSetTrxId(ospvReauthReq, ospvNum) \
    (ospvReauthReq)->ospmReauthReqTrxId = ospvNum
#define OSPPReauthReqGetTrxId(ospvReauthReq) ((ospvReauthReq)->ospmReauthReqTrxId)

/* UsageDetail */
#define OSPPReauthReqHasDuration(ospvReauthReq) \
    (ospvReauthReq)?((ospvReauthReq)->ospmReauthReqDuration >= 0):OSPC_FALSE

#define OSPPReauthReqSetDuration(ospvReauthReq,ospvDuration) \
    (ospvReauthReq)->ospmReauthReqDuration = ospvDuration; \
 
#define OSPPReauthReqGetDuration(ospvReauthReq) \
    (ospvReauthReq)?((ospvReauthReq)->ospmReauthReqDuration):0

#define OSPPReauthReqAddToken(ospvReauthReq, ospvToken)   \
    OSPPListAppend(&(ospvReauthReq)->ospmReauthReqTokens, (ospvToken))
#define OSPPReauthReqFirstToken(ospvReauthReq) \
    OSPPListFirst(&(ospvReauthReq)->ospmReauthReqTokens)
#define OSPPReauthReqNextToken(ospvReauthReq, ospvToken) \
    OSPPListNext(&(ospvReauthReq)->ospmReauthReqTokens, (ospvToken))
#endif  /* NOT OSPC_DEBUG */
/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

    OSPTREAUTHREQ   *OSPPReauthReqNew(void);
    void            OSPPReauthReqDelete(OSPTREAUTHREQ **);
    int             OSPPReauthReqToElement(OSPTREAUTHREQ *, OSPTXMLELEM **);

    void            OSPPReauthReqSetRole(OSPTREAUTHREQ *, OSPE_MSG_ROLETYPES);
    unsigned        OSPPReauthReqHasRole(OSPTREAUTHREQ *);
    OSPE_MSG_ROLETYPES  OSPPReauthReqGetRole(OSPTREAUTHREQ *);

    void            OSPPReauthReqAddSourceAlt(OSPTREAUTHREQ *, OSPTALTINFO *);
    void            OSPPReauthReqAddDestinationAlt(OSPTREAUTHREQ *, OSPTALTINFO *);        
    void            OSPPReauthReqSetCallId(OSPTREAUTHREQ *, OSPTCALLID *);
    unsigned        OSPPReauthReqHasMessageId(OSPTREAUTHREQ *);
    unsigned char  *OSPPReauthReqGetMessageId(OSPTREAUTHREQ *);

#ifdef OSPC_DEBUG

    unsigned    OSPPReauthReqHasTimestamp(OSPTREAUTHREQ *);
    void        OSPPReauthReqSetTimestamp(OSPTREAUTHREQ *, OSPTTIME);
    OSPTTIME    OSPPReauthReqGetTimestamp(OSPTREAUTHREQ *);

    unsigned       OSPPReauthReqHasComponentId(OSPTREAUTHREQ *);
    unsigned char  *OSPPReauthReqGetComponentId(OSPTREAUTHREQ *);

    unsigned    OSPPReauthReqHasCallId(OSPTREAUTHREQ *);
    OSPTCALLID *OSPPReauthReqGetCallId(OSPTREAUTHREQ *);

    void        OSPPReauthReqSetSourceNumber(OSPTREAUTHREQ *, const unsigned char *);
    const unsigned char *OSPPReauthReqGetSourceNumber(OSPTREAUTHREQ *);

    void        OSPPReauthReqSetDestNumber(OSPTREAUTHREQ *, const unsigned char *);
    const unsigned char *OSPPReauthReqGetDestNumber(OSPTREAUTHREQ *);

    unsigned    OSPPReauthReqHasTrxId(OSPTREAUTHREQ *);
    void        OSPPReauthReqSetTrxId(OSPTREAUTHREQ *, OSPTTRXID);
    OSPTTRXID   OSPPReauthReqGetTrxId(OSPTREAUTHREQ *);

    unsigned    OSPPReauthReqHasDuration(OSPTREAUTHREQ *);
    void        OSPPReauthReqSetDuration(OSPTREAUTHREQ *, int ospvDuration);
    int         OSPPReauthReqGetDuration(OSPTREAUTHREQ *);

    void        OSPPReauthReqAddToken(OSPTREAUTHREQ *, OSPTTOKEN *);
    OSPTTOKEN  *OSPPReauthReqFirstToken(OSPTREAUTHREQ *);
    OSPTTOKEN  *OSPPReauthReqNextToken(OSPTREAUTHREQ *, OSPTTOKEN *);

#endif  /* OSPC_DEBUG */

    unsigned       OSPPReauthReqHasDestinationAlt(OSPTREAUTHREQ *);
    OSPTALTINFO   *OSPPReauthReqFirstDestinationAlt(OSPTREAUTHREQ *);
    OSPTALTINFO   *OSPPReauthReqNextDestinationAlt(OSPTREAUTHREQ *, OSPTALTINFO  *);
    unsigned char *OSPPReauthReqGetDestinationAltValue(OSPTALTINFO *);

    unsigned       OSPPReauthReqHasSourceAlt(OSPTREAUTHREQ *);
    OSPTALTINFO   *OSPPReauthReqFirstSourceAlt(OSPTREAUTHREQ *);
    OSPTALTINFO   *OSPPReauthReqNextSourceAlt(OSPTREAUTHREQ *, OSPTALTINFO  *);

    unsigned       OSPPReauthReqHasTNCustId(OSPTREAUTHREQ *);
    void           OSPPReauthReqSetTNCustId(OSPTREAUTHREQ *, unsigned long);
    unsigned long  OSPPReauthReqGetTNCustId(OSPTREAUTHREQ *);

    unsigned       OSPPReauthReqHasTNDeviceId(OSPTREAUTHREQ *);
    void           OSPPReauthReqSetTNDeviceId(OSPTREAUTHREQ *, unsigned long);
    unsigned long  OSPPReauthReqGetTNDeviceId(OSPTREAUTHREQ *);

#ifdef __cplusplus
}
#endif

#endif

