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


    OSPTBOOL       OSPPUsageIndHasTNStatistics(OSPTUSAGEIND *);
    void           OSPPUsageIndSetTNStatistics(OSPTUSAGEIND *, OSPTSTATISTICS *);
    void           OSPPUsageIndAddDestinationAlt(OSPTUSAGEIND *ospvUsageInd, OSPTALTINFO *ospvAltInfo);

    void           OSPPUsageIndSetStartTime(OSPTUSAGEIND *, OSPTTIME ospvStartTime);
    OSPTTIME       OSPPUsageIndGetStartTime(OSPTUSAGEIND *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPUSAGEIND_H */
