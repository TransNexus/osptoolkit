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
 *  ospauthreq.h - OSP authorisation request objects
 */

#ifndef _OSPAUTHREQ_H
#define _OSPAUTHREQ_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"
#include "ospaltinfo.h"


typedef struct
{
    OSPTTIME          ospmAuthReqTimestamp;
    unsigned char    *ospmAuthReqMessageId;
    unsigned char    *ospmAuthReqComponentId;
    OSPTLIST          ospmAuthReqCallId;
    unsigned char     ospmAuthReqSourceNumber[OSPC_E164NUMSIZE];
    unsigned char     ospmAuthReqDestNumber[OSPC_E164NUMSIZE];
    OSPTLIST          ospmAuthReqSourceAlternate;
    OSPTLIST          ospmAuthReqDestinationAlternate;
    unsigned          ospmAuthReqMaxDest;
    unsigned long     ospmAuthReqTNCustId;
    unsigned long     ospmAuthReqTNDeviceId;
}
OSPTAUTHREQ;

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospauthreq.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG

#define OSPPAuthReqHasTimestamp(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqTimestamp != OSPC_TIMEMIN):OSPC_FALSE
#define OSPPAuthReqSetTimestamp(ospvAuthReq,ospvTime) \
    (ospvAuthReq)->ospmAuthReqTimestamp = (ospvTime)
#define OSPPAuthReqGetTimestamp(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqTimestamp):0

/* ComponentId */
#define OSPPAuthReqHasComponentId(ospvAuthReq) \
    ((ospvAuthReq)->ospmAuthReqComponentId != OSPC_OSNULL)

#define OSPPAuthReqHasCallId(ospvAuthReq) \
    (ospvAuthReq)?(OSPPAuthReqFirstCallId(ospvAuthReq) != OSPC_OSNULL): \
    OSPC_OSNULL
#define OSPPAuthReqFirstCallId(ospvAuthReq) \
    (ospvAuthReq)? \
    (OSPTCALLID *)OSPPListFirst(&((ospvAuthReq)->ospmAuthReqCallId)): \
    OSPC_OSNULL
#define OSPPAuthReqNextCallId(ospvAuthReq,ospvCallId) \
    (ospvAuthReq)? \
    (OSPTCALLID *)OSPPListNext((OSPTLIST *)&((ospvAuthReq)->ospmAuthReqCallId), (void *)ospvCallId): \
    OSPC_OSNULL
#define OSPPAuthReqGetCallIdSize(ospvCallId) \
    (ospvCallId)?OSPPCallIdGetSize(ospvCallId):OSPC_OSNULL
#define OSPPAuthReqGetCallIdValue(ospvCallId) \
    (ospvCallId)?OSPPCallIdGetValue(ospvCallId):OSPC_OSNULL

#define OSPPAuthReqHasSourceNumber(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqSourceNumber[0] != '\0'):OSPC_FALSE
#define OSPPAuthReqSetSourceNumber(ospvAuthReq,ospvNum) \
    OSPM_STRNCPY((ospvAuthReq)->ospmAuthReqSourceNumber, (ospvNum), \
    tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_E164NUMSIZE-1))
#define OSPPAuthReqGetSourceNumber(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqSourceNumber):OSPC_OSNULL

#define OSPPAuthReqHasDestNumber(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqDestNumber[0] != '\0'):OSPC_FALSE
#define OSPPAuthReqSetDestNumber(ospvAuthReq,ospvNum) \
    OSPM_STRNCPY((ospvAuthReq)->ospmAuthReqDestNumber, (ospvNum), \
    tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_E164NUMSIZE-1))
#define OSPPAuthReqGetDestNumber(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqDestNumber):OSPC_OSNULL

/* Source Alternate */

#define OSPPAuthReqHasSourceAlt(ospvAuthReq) \
    (ospvAuthReq)?(OSPPAuthReqFirstSourceAlt(ospvAuthReq) != OSPC_OSNULL):OSPC_FALSE

#define OSPPAuthReqFirstSourceAlt(ospvAuthReq) \
    (ospvAuthReq)?((OSPTALTINFO *)OSPPListFirst((OSPTLIST *)&((ospvAuthReq)->ospmAuthReqSourceAlternate))):OSPC_OSNULL

#define OSPPAuthReqNextSourceAlt(ospvAuthReq, ospvAltInfo) \
    (ospvAuthReq)?((OSPTALTINFO *)OSPPListNext((OSPTLIST *)&((ospvAuthReq)->ospmAuthReqSourceAlternate), \
            (void *)ospvAltInfo)):OSPC_OSNULL

/* Destination Alternate */

#define OSPPAuthReqHasDestinationAlt(ospvAuthReq) \
    (ospvAuthReq)? \
    (OSPPAuthReqFirstDestinationAlt(ospvAuthReq) != OSPC_OSNULL): \
    OSPC_FALSE

#define OSPPAuthReqFirstDestinationAlt(ospvAuthReq) \
    (ospvAuthReq)? \
    ((OSPTALTINFO *)OSPPListFirst(&((ospvAuthReq)->ospmAuthReqDestinationAlternate))): \
    OSPC_OSNULL

#define OSPPAuthReqNextDestinationAlt(ospvAuthReq, ospvAltInfo) \
    (ospvAltInfo)? \
    ((OSPTALTINFO *)OSPPListNext((OSPTLIST *)&((ospvAuthReq)->ospmAuthReqDestinationAlternate), \
            (void *)ospvAltInfo)):OSPC_OSNULL

#define OSPPAuthReqSetMaxDest(ospvAuthReq, ospvNum) \
    (ospvAuthReq)->ospmAuthReqMaxDest = ospvNum
#define OSPPAuthReqGetMaxDest(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqMaxDest):0

#define OSPPAuthReqHasTNCustId(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqTNCustId != 0L):OSPC_FALSE
#define OSPPAuthReqSetTNCustId(ospvAuthReq,ospvTNCustId) \
    (ospvAuthReq)->ospmAuthReqTNCustId = (ospvTNCustId)
#define OSPPAuthReqGetTNCustId(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqTNCustId):0L

#define OSPPAuthReqHasTNDeviceId(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqTNDeviceId != 0L):OSPC_FALSE
#define OSPPAuthReqSetTNDeviceId(ospvAuthReq,ospvTNDeviceId) \
    (ospvAuthReq)->ospmAuthReqTNDeviceId = (ospvTNDeviceId)
#define OSPPAuthReqGetTNDeviceId(ospvAuthReq) \
    (ospvAuthReq)?((ospvAuthReq)->ospmAuthReqTNDeviceId):0L

#endif /* NOT OSPC_DEBUG */

#ifdef __cplusplus
extern "C" 
{
#endif
    /**/
    /*-----------------------------------------------------------------------*
     *  function prototypes
     *-----------------------------------------------------------------------*/

    OSPTAUTHREQ   *OSPPAuthReqNew(void);
    void           OSPPAuthReqDelete(OSPTAUTHREQ **);
    int            OSPPAuthReqToElement(OSPTAUTHREQ *, OSPTXMLELEM **);
    unsigned       OSPPAuthReqHasMessageId(OSPTAUTHREQ *);

#ifdef OSPC_DEBUG
    unsigned       OSPPAuthReqHasTimestamp(OSPTAUTHREQ *);
    void           OSPPAuthReqSetTimestamp(OSPTAUTHREQ *, OSPTTIME);
    OSPTTIME       OSPPAuthReqGetTimestamp(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasComponentId(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasCallId(OSPTAUTHREQ *);
    OSPTCALLID    *OSPPAuthReqFirstCallId(OSPTAUTHREQ *);
    OSPTCALLID    *OSPPAuthReqNextCallId(OSPTAUTHREQ *, OSPTCALLID *);

    unsigned char *OSPPAuthReqGetCallIdValue(OSPTCALLID *);

    unsigned       OSPPAuthReqHasSourceNumber(OSPTAUTHREQ *);
    void           OSPPAuthReqSetSourceNumber(OSPTAUTHREQ *, const unsigned char *);
    unsigned char *OSPPAuthReqGetSourceNumber(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasSourceAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqFirstSourceAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqNextSourceAlt(OSPTAUTHREQ *, OSPTALTINFO  *);

    unsigned       OSPPAuthReqHasDestNumber(OSPTAUTHREQ *);
    void           OSPPAuthReqSetDestNumber(OSPTAUTHREQ *, const unsigned char *);
    unsigned char *OSPPAuthReqGetDestNumber(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasDestinationAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqFirstDestinationAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqNextDestinationAlt(OSPTAUTHREQ *, OSPTALTINFO  *);

    void           OSPPAuthReqSetMaxDest(OSPTAUTHREQ *, unsigned);
    unsigned       OSPPAuthReqGetMaxDest(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasTNCustId(OSPTAUTHREQ *);
    void           OSPPAuthReqSetTNCustId(OSPTAUTHREQ *, unsigned long);
    unsigned long  OSPPAuthReqGetTNCustId(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasTNDeviceId(OSPTAUTHREQ *ospvAuthReq);
    void           OSPPAuthReqSetTNDeviceId(OSPTAUTHREQ *, unsigned long);
    unsigned long  OSPPAuthReqGetTNDeviceId(OSPTAUTHREQ *);
#endif /* OSPC_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHREQ_H */
