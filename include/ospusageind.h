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
 * ospusageind.h - OSP Usage Indication Objects
 */

#ifndef _OSPUSAGEIND_H
#define _OSPUSAGEIND_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "ospaltinfo.h"
#include "ospstatistics.h"

typedef struct
{
    OSPTLISTLINK      ospmUsageIndLink;
    OSPTTIME          ospmUsageIndTimestamp;
    OSPTTIME          ospmUsageIndStartTime;
    unsigned char     *ospmUsageIndMessageId;
    unsigned char     *ospmUsageIndComponentId;
    unsigned          ospmUsageIndRole;
    unsigned          ospmUsageIndHasRole;
    OSPTTRXID         ospmUsageIndTransactionId;
    OSPTCALLID        *ospmUsageIndCallId;
    int               ospmUsageIndDuration;
    unsigned char     ospmUsageIndSourceNumber[OSPC_E164NUMSIZE];
    unsigned char     ospmUsageIndDestNumber[OSPC_E164NUMSIZE];
    OSPTLIST          ospmUsageIndSourceAlternate;
    OSPTLIST          ospmUsageIndDestinationAlternate;
    unsigned long     ospmUsageIndTNCustId;
    unsigned long     ospmUsageIndTNDeviceId;
    unsigned          ospmUsageIndTNFailReason;
    unsigned          ospmUsageIndTNFailReasonInd;
    OSPTSTATISTICS    *ospmUsageIndTNStats;
}
OSPTUSAGEIND;

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/
#ifndef OSPC_DEBUG
/*
 * Note: all macros are also implemented as functions in ospusageind.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

/* Timestamp */
#define OSPPUsageIndHasTimestamp(ospvUsageInd) \
(ospvUsageInd)?((ospvUsageInd)->ospmUsageIndTimestamp):OSPC_FALSE
#define OSPPUsageIndSetTimestamp(ospvUsageInd,ospvTime) \
    (ospvUsageInd)->ospmUsageIndTimestamp = (ospvTime)
#define OSPPUsageIndGetTimestamp(ospvUsageInd) \
    (ospvUsageInd)->ospmUsageIndTimestamp

/* ComponentId */
#define OSPPUsageIndHasComponentId(ospvUsageInd) \
    ((ospvUsageInd)->ospmUsageIndComponentId != OSPC_OSNULL)
#define OSPPUsageIndGetComponentId(ospvUsageInd) \
    (ospvUsageInd)->ospmUsageIndComponentId

/* Role */
#define OSPPUsageIndHasRole(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndRole):OSPC_FALSE
#define OSPPUsageIndGetRole(ospvUsageInd) \
    (ospvUsageInd)->ospmUsageIndRole
#define OSPPUsageIndSetRole(ospvUsageInd,ospvRole) \
    (ospvUsageInd)->ospmUsageIndRole = (ospvRole); \
    (ospvUsageInd)->ospmUsageIndHasRole = OSPC_TRUE;

/* TransactionId */
#define OSPPUsageIndHasTransactionId(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndTransactionId != 0):OSPC_FALSE
#define OSPPUsageIndSetTransactionId(ospvUsageInd,ospvTransactionId) \
    (ospvUsageInd)->ospmUsageIndTransactionId = ospvTransactionId
#define OSPPUsageIndGetTransactionId(ospvUsageInd) \
    ((ospvUsageInd)->ospmUsageIndTransactionId)

/* CallId */
#define OSPPUsageIndHasCallId(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndCallId != OSPC_OSNULL):OSPC_FALSE
#define OSPPUsageIndGetCallId(ospvUsageInd) \
    ((ospvUsageInd)->ospmUsageIndCallId)

/* SourceInfo */
#define OSPPUsageIndSetSourceNumber(ospvUsageInd,ospvSourceNumber) \
    OSPM_STRNCPY((char *)(ospvUsageInd)->ospmUsageIndSourceNumber, \
        (const char *)(ospvSourceNumber), \
        min(OSPC_E164NUMSIZE-1, OSPM_STRLEN((const char *) ospvSourceNumber)+1))
#define OSPPUsageIndGetSourceNumber(ospvUsageInd) \
        (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndSourceNumber):OSPC_OSNULL

/* Source Alternate */
#define OSPPUsageIndHasSourceAlt(ospvUsageInd) \
    (ospvUsageInd)?(OSPPUsageIndFirstSourceAlt(ospvUsageInd) != OSPC_OSNULL):OSPC_FALSE

#define OSPPUsageIndFirstSourceAlt(ospvUsageInd) \
    (const char *)OSPPListFirst(&((ospvUsageInd)->ospmUsageIndSourceAlternate))

#define OSPPUsageIndNextSourceAlt(ospvUsageInd, ospvAltInfo) \
    (OSPTALTINFO *)OSPPListNext((OSPTLIST *)&((ospvUsageInd)->ospmUsageIndSourceAlternate), \
                        (void *)ospvAltInfo)

/* DestinationInfo */
#define OSPPUsageIndSetDestNumber(ospvUsageInd,ospvDestNumber) \
    OSPM_STRNCPY((char *)(ospvUsageInd)->ospmUsageIndDestNumber, \
        (const char *)(ospvDestNumber),  \
        min(OSPC_E164NUMSIZE-1, OSPM_STRLEN((const char *) ospvDestNumber)+1))
#define OSPPUsageIndGetDestNumber(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndDestNumber):OSPC_OSNULL

/* Destination Alternate */

#define OSPPUsageIndHasDestinationAlt(ospvUsageInd) \
    (ospvUsageInd)?(OSPPUsageIndFirstDestinationAlt(ospvUsageInd) != OSPC_OSNULL):OSPC_FALSE

#define OSPPUsageIndFirstDestinationAlt(ospvUsageInd) \
    (const char *)OSPPListFirst(&((ospvUsageInd)->ospmUsageIndDestinationAlternate))

#define OSPPUsageIndNextDestinationAlt(ospvUsageInd, ospvAltInfo) \
    (OSPTALTINFO *)OSPPListNext((OSPTLIST *)&((ospvUsageInd)->ospmUsageIndDestinationAlternate), \
                        (void *)ospvAltInfo)

#define OSPPUsageIndGetDestinationAltSize(ospvAltInfo) \
    (ospvAltInfo)?OSPPAltInfoGetSize(ospvAltInfo):0

/* UsageDetail */
#define OSPPUsageIndHasDuration(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndDuration >= 0):OSPC_FALSE
#define OSPPUsageIndSetDuration(ospvUsageInd,ospvDuration) \
    (ospvUsageInd)->ospmUsageIndDuration = ospvDuration;
#define OSPPUsageIndGetDuration(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndDuration):0

/* Transnexus Extensions */
/* Customer ID */
#define OSPPUsageIndHasTNCustId(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndTNCustId != 0L):OSPC_FALSE
#define OSPPUsageIndSetTNCustId(ospvUsageInd,ospvTNCustId) \
    (ospvUsageInd)->ospmUsageIndTNCustId = (ospvTNCustId)
#define OSPPUsageIndGetTNCustId(ospvUsageInd) \
    (ospvUsageInd)?(ospvUsageInd)->ospmUsageIndTNCustId:0L

/* Device ID */
#define OSPPUsageIndHasTNDeviceId(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndTNDeviceId != 0L):OSPC_FALSE
#define OSPPUsageIndSetTNDeviceId(ospvUsageInd,ospvTNDeviceId) \
    (ospvUsageInd)->ospmUsageIndTNDeviceId = (ospvTNDeviceId)
#define OSPPUsageIndGetTNDeviceId(ospvUsageInd) \
    (ospvUsageInd)?(ospvUsageInd)->ospmUsageIndTNDeviceId:0L

/* Failure Reason */
#define OSPPUsageIndHasTNFailReason(ospvUsageInd) \
    (ospvUsageInd)?((ospvUsageInd)->ospmUsageIndTNFailReasonInd):OSPC_FALSE
#define OSPPUsageIndSetTNFailReason(ospvUsageInd,ospvTNFailReason) { \
    (ospvUsageInd)->ospmUsageIndTNFailReason = (ospvTNFailReason); \
    (ospvUsageInd)->ospmUsageIndTNFailReasonInd = 1; \
    }
#define OSPPUsageIndGetTNFailReason(ospvUsageInd) \
    (ospvUsageInd)?(ospvUsageInd)->ospmUsageIndTNFailReason:0

#endif

/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" 
{
#endif

    OSPTUSAGEIND    *OSPPUsageIndNew(void);
    void            OSPPUsageIndDelete( OSPTUSAGEIND **);
    int             OSPPUsageIndToElement(OSPTLIST *, OSPTXMLELEM **);
    void            OSPPUsageIndSetCallId(OSPTUSAGEIND *, OSPTCALLID *);

    void      OSPPUsageIndMoveSourceAlt(OSPTUSAGEIND *, OSPTLIST *);
    void      OSPPUsageIndMoveDestinationAlt(OSPTUSAGEIND *, OSPTLIST *);
    void      OSPPUsageIndCopySourceAlt(OSPTUSAGEIND *, OSPTLIST *);
    void      OSPPUsageIndSetComponentId(OSPTUSAGEIND *, unsigned char *);

    unsigned       OSPPUsageIndHasMessageId(OSPTUSAGEIND *);
    unsigned char  *OSPPUsageIndGetMessageId(OSPTUSAGEIND *);

#ifdef OSPC_DEBUG

    unsigned       OSPPUsageIndHasTimestamp(OSPTUSAGEIND *);
    void           OSPPUsageIndSetTimestamp(OSPTUSAGEIND *, OSPTTIME);
    OSPTTIME       OSPPUsageIndGetTimestamp(OSPTUSAGEIND *);

    unsigned       OSPPUsageIndHasComponentId(OSPTUSAGEIND *);
    unsigned char  *OSPPUsageIndGetComponentId(OSPTUSAGEIND *);

    unsigned            OSPPUsageIndHasRole(OSPTUSAGEIND *);
    OSPE_MSG_ROLETYPES  OSPPUsageIndGetRole(OSPTUSAGEIND *);
    void                OSPPUsageIndSetRole(OSPTUSAGEIND *, unsigned);

    unsigned       OSPPUsageIndHasTransactionId(OSPTUSAGEIND *);
    void           OSPPUsageIndSetTransactionId(OSPTUSAGEIND *, OSPTTRXID ospvTransactionId);
    OSPTTRXID      OSPPUsageIndGetTransactionId(OSPTUSAGEIND *);

    unsigned       OSPPUsageIndHasCallId(OSPTUSAGEIND *);
    OSPTCALLID    *OSPPUsageIndGetCallId(OSPTUSAGEIND *);

    void           OSPPUsageIndSetSourceNumber(OSPTUSAGEIND *,unsigned char *);
    unsigned char *OSPPUsageIndGetSourceNumber(OSPTUSAGEIND *);

    void           OSPPUsageIndSetDestNumber(OSPTUSAGEIND *,unsigned char *);
    unsigned char *OSPPUsageIndGetDestNumber(OSPTUSAGEIND *);

    unsigned       OSPPUsageIndHasSourceAlt(OSPTUSAGEIND *);
    OSPTALTINFO   *OSPPUsageIndFirstSourceAlt(OSPTUSAGEIND *);
    OSPTALTINFO   *OSPPUsageIndNextSourceAlt(OSPTUSAGEIND *, OSPTALTINFO  *);

    unsigned       OSPPUsageIndHasDestinationAlt(OSPTUSAGEIND *);
    unsigned       OSPPUsageIndGetDestinationAltSize(OSPTALTINFO *);
    OSPTALTINFO   *OSPPUsageIndFirstDestinationAlt(OSPTUSAGEIND *);
    OSPTALTINFO   *OSPPUsageIndNextDestinationAlt(OSPTUSAGEIND *, OSPTALTINFO  *);

    unsigned       OSPPUsageIndHasDuration(OSPTUSAGEIND *);
    void           OSPPUsageIndSetDuration(OSPTUSAGEIND *, int ospvDuration);
    int            OSPPUsageIndGetDuration(OSPTUSAGEIND *);

    unsigned       OSPPUsageIndHasTNCustId(OSPTUSAGEIND *ospvUsageInd);
    void           OSPPUsageIndSetTNCustId(OSPTUSAGEIND *, unsigned long);
    unsigned long  OSPPUsageIndGetTNCustId(OSPTUSAGEIND *);

    unsigned       OSPPUsageIndHasTNDeviceId(OSPTUSAGEIND *ospvUsageInd);
    void           OSPPUsageIndSetTNDeviceId(OSPTUSAGEIND *, unsigned long);
    unsigned long  OSPPUsageIndGetTNDeviceId(OSPTUSAGEIND *);

    unsigned       OSPPUsageIndHasTNFailReason(OSPTUSAGEIND *ospvUsageInd);
    void           OSPPUsageIndSetTNFailReason(OSPTUSAGEIND *, unsigned);
    unsigned       OSPPUsageIndGetTNFailReason(OSPTUSAGEIND *);


#endif /* OSPC_DEBUG */

    OSPTBOOL       OSPPUsageIndHasTNStatistics(OSPTUSAGEIND *);
    void           OSPPUsageIndSetTNStatistics(OSPTUSAGEIND *, OSPTSTATISTICS *);
    void           OSPPUsageIndAddDestinationAlt(OSPTUSAGEIND *ospvUsageInd, OSPTALTINFO *ospvAltInfo);

    void           OSPPUsageIndSetStartTime(OSPTUSAGEIND *, OSPTTIME ospvStartTime);
    OSPTTIME       OSPPUsageIndGetStartTime(OSPTUSAGEIND *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPUSAGEIND_H */
