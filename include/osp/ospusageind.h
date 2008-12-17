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

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/ospaltinfo.h"
#include "osp/ospstatistics.h"

typedef struct {
    OSPTLISTLINK ospmUsageIndLink;
    OSPTTIME ospmUsageIndTimestamp;
    OSPTTIME ospmUsageIndStartTime;
    OSPTTIME ospmUsageIndEndTime;
    OSPTTIME ospmUsageIndAlertTime;
    OSPTTIME ospmUsageIndConnectTime;
    OSPTTIME ospmUsageIndPostDialDelay;
    OSPTBOOL ospmUsageIndHasPDD;
    unsigned ospmUsageIndReleaseSource;
    char ospmUsageIndConferenceId[OSPC_CONFIDSIZE];    /* This is in chararcters */
    char *ospmUsageIndMessageId;
    char *ospmUsageIndComponentId;
    OSPE_ROLE_TYPE ospmUsageIndRole;
    OSPTBOOL ospmUsageIndHasRole;
    OSPTTRXID ospmUsageIndTransactionId;
    OSPTCALLID *ospmUsageIndCallId;
    int ospmUsageIndDuration;
    char ospmUsageIndSourceNumber[OSPC_SIZE_E164NUM];
    char ospmUsageIndDestNumber[OSPC_SIZE_E164NUM];
    OSPTLIST ospmUsageIndSourceAlternate;
    OSPTLIST ospmUsageIndDeviceInfo;
    OSPTLIST ospmUsageIndDestinationAlternate;
    unsigned long ospmUsageIndCustId;
    unsigned long ospmUsageIndDeviceId;
    OSPT_FAILREASON ospmUsageIndFailReason;
    OSPT_STATISTICS *ospmUsageIndStatistics;
    OSPTBOOL ospmUsageIndIsPricingInfoPresent;
    OSPT_PRICING_INFO osmpUsageIndPricingInfo;
    OSPTBOOL osmpUsageIndIsServiceInfoPresent;
    OSPE_SERVICE_TYPE osmpUsageIndServiceType;
    OSPT_ALTINFO *ospmUsageIndDestinationCount;
    char ospmUsageIndAssertedId[OSPC_SIZE_NORID];
    OSPE_DEST_PROT ospmUsageIndDestProtocol;
    char ospmUsageIndForwardCodec[OSPC_SIZE_CODEC];
    char ospmUsageIndReverseCodec[OSPC_SIZE_CODEC];
    OSPTCALLID *ospmUsageIndInCallId;
    OSPTCALLID *ospmUsageIndOutCallId;
} OSPT_USAGEIND;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPT_USAGEIND *OSPPUsageIndNew(void);
    void OSPPUsageIndDelete(OSPT_USAGEIND **);
    int OSPPUsageIndToElement(OSPTLIST *, OSPT_XML_ELEM **, void *);
    void OSPPUsageIndSetCallId(OSPT_USAGEIND *, OSPTCALLID *);
    void OSPPUsageIndMoveSourceAlt(OSPT_USAGEIND *, OSPTLIST *);
    void OSPPUsageIndMoveDeviceInfo(OSPT_USAGEIND *, OSPTLIST *);
    void OSPPUsageIndMoveDestinationAlt(OSPT_USAGEIND *, OSPTLIST *);
    void OSPPUsageIndCopySourceAlt(OSPT_USAGEIND *, OSPTLIST *);
    void OSPPUsageIndMergeSourceAlt(OSPT_USAGEIND *, OSPTLIST *, OSPTLIST *);
    void OSPPUsageIndCopyDeviceInfo(OSPT_USAGEIND *, OSPTLIST *);
    void OSPPUsageIndSetComponentId(OSPT_USAGEIND *, const char *);
    OSPTBOOL OSPPUsageIndHasMessageId(OSPT_USAGEIND *);
    const char *OSPPUsageIndGetMessageId(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasTimestamp(OSPT_USAGEIND *);
    void OSPPUsageIndSetTimestamp(OSPT_USAGEIND *, OSPTTIME);
    OSPTTIME OSPPUsageIndGetTimestamp(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasComponentId(OSPT_USAGEIND *);
    const char *OSPPUsageIndGetComponentId(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasRole(OSPT_USAGEIND *);
    OSPE_ROLE_TYPE OSPPUsageIndGetRole(OSPT_USAGEIND *);
    void OSPPUsageIndSetRole(OSPT_USAGEIND *, OSPE_ROLE_TYPE);
    OSPTBOOL OSPPUsageIndHasTransactionId(OSPT_USAGEIND *);
    void OSPPUsageIndSetTransactionId(OSPT_USAGEIND *, OSPTTRXID ospvTransactionId);
    OSPTTRXID OSPPUsageIndGetTransactionId(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasCallId(OSPT_USAGEIND *);
    OSPTCALLID *OSPPUsageIndGetCallId(OSPT_USAGEIND *);
    void OSPPUsageIndSetSourceNumber(OSPT_USAGEIND *, const char *);
    char *OSPPUsageIndGetSourceNumber(OSPT_USAGEIND *);
    void OSPPUsageIndSetCurrency(OSPT_USAGEIND *, const char *);
    const char *OSPPUsageIndGetCurrency(OSPT_USAGEIND *);
    void OSPPUsageIndSetDestNumber(OSPT_USAGEIND *, const char *);
    const char *OSPPUsageIndGetDestNumber(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasSourceAlt(OSPT_USAGEIND *);
    OSPT_ALTINFO *OSPPUsageIndFirstSourceAlt(OSPT_USAGEIND *);
    OSPT_ALTINFO *OSPPUsageIndNextSourceAlt(OSPT_USAGEIND *, OSPT_ALTINFO *);
    OSPTBOOL OSPPUsageIndHasDestinationAlt(OSPT_USAGEIND *);
    unsigned OSPPUsageIndGetDestinationAltSize(OSPT_ALTINFO *);
    OSPT_ALTINFO *OSPPUsageIndFirstDestinationAlt(OSPT_USAGEIND *);
    OSPT_ALTINFO *OSPPUsageIndNextDestinationAlt(OSPT_USAGEIND *, OSPT_ALTINFO *);
    OSPTBOOL OSPPUsageIndHasDuration(OSPT_USAGEIND *);
    void OSPPUsageIndSetDuration(OSPT_USAGEIND *, int ospvDuration);
    int OSPPUsageIndGetDuration(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasCustId(OSPT_USAGEIND *ospvUsageInd);
    void OSPPUsageIndSetCustId(OSPT_USAGEIND *, unsigned long);
    unsigned long OSPPUsageIndGetCustId(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasDeviceId(OSPT_USAGEIND *ospvUsageInd);
    void OSPPUsageIndSetDeviceId(OSPT_USAGEIND *, unsigned long);
    unsigned long OSPPUsageIndGetDeviceId(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasFailReason(OSPT_USAGEIND *ospvUsageInd, OSPE_TERM_CAUSE);
    void OSPPUsageIndSetFailReason(OSPT_USAGEIND *, OSPE_TERM_CAUSE, unsigned, const char *);
    unsigned OSPPUsageIndGetTCCode(OSPT_USAGEIND *, OSPE_TERM_CAUSE);
    const char *OSPPUsageIndGetTCDesc(OSPT_USAGEIND *, OSPE_TERM_CAUSE);
    OSPTBOOL OSPPUsageIndHasStatistics(OSPT_USAGEIND *);
    void OSPPUsageIndSetStatistics(OSPT_USAGEIND *, OSPT_STATISTICS *);
    void OSPPUsageIndAddDestinationAlt(OSPT_USAGEIND *ospvUsageInd, OSPT_ALTINFO *ospvAltInfo);
    void OSPPUsageIndSetStartTime(OSPT_USAGEIND *, OSPTTIME ospvStartTime);
    OSPTTIME OSPPUsageIndGetStartTime(OSPT_USAGEIND *);
    void OSPPUsageIndSetEndTime(OSPT_USAGEIND *, OSPTTIME ospvEndTime);
    OSPTTIME OSPPUsageIndGetEndTime(OSPT_USAGEIND *);
    void OSPPUsageIndSetConnectTime(OSPT_USAGEIND *, OSPTTIME ospvEndTime);
    OSPTTIME OSPPUsageIndGetConnectTime(OSPT_USAGEIND *);
    void OSPPUsageIndSetAlertTime(OSPT_USAGEIND *, OSPTTIME ospvEndTime);
    OSPTTIME OSPPUsageIndGetAlertTime(OSPT_USAGEIND *);
    void OSPPUsageIndSetPostDialDelay(OSPT_USAGEIND *, int ospvPostDialDelay);
    int OSPPUsageIndGetPostDialDelay(OSPT_USAGEIND *);
    void OSPPUsageIndSetIsPDDInfoPresent(OSPT_USAGEIND *, OSPTBOOL ospvIsPDDInfoPresent);
    int OSPPUsageIndGetIsPDDInfoPresent(OSPT_USAGEIND *);
    void OSPPUsageIndSetReleaseSource(OSPT_USAGEIND *, unsigned ospvReleaseSource);
    unsigned OSPPUsageIndGetReleaseSource(OSPT_USAGEIND *);
    void OSPPUsageIndSetConferenceId(OSPT_USAGEIND *, const char *);
    const char *OSPPUsageIndGetConferenceId(OSPT_USAGEIND *);
    void OSPPUsageIndSetDestinationCount(OSPT_USAGEIND *, unsigned ospvDestinationCount);
    OSPT_ALTINFO *OSPPUsageIndGetDestinationCount(OSPT_USAGEIND *);
    OSPTBOOL OSPPUsageIndHasAssertedId(OSPT_USAGEIND *);
    const char *OSPPUsageIndGetAssertedId(OSPT_USAGEIND *);
    void OSPPUsageIndSetAssertedId(OSPT_USAGEIND *, const char *);
    OSPTBOOL OSPPUsageIndHasDestProtocol(OSPT_USAGEIND *);
    OSPE_DEST_PROT OSPPUsageIndGetDestProtocol(OSPT_USAGEIND *);
    void OSPPUsageIndSetDestProtocol(OSPT_USAGEIND *, const char *);
    OSPTBOOL OSPPUsageIndHasForwardCodec(OSPT_USAGEIND *);
    const char *OSPPUsageIndGetForwardCodec(OSPT_USAGEIND *);
    void OSPPUsageIndSetForwardCodec(OSPT_USAGEIND *, const char *);
    OSPTBOOL OSPPUsageIndHasReverseCodec(OSPT_USAGEIND *);
    const char *OSPPUsageIndGetReverseCodec(OSPT_USAGEIND *);
    void OSPPUsageIndSetReverseCodec(OSPT_USAGEIND *, const char *);
    OSPTBOOL OSPPUsageIndHasInCallId(OSPT_USAGEIND *);
    OSPTCALLID *OSPPUsageIndGetInCallId(OSPT_USAGEIND *);
    void OSPPUsageIndSetInCallId(OSPT_USAGEIND *, OSPTCALLID *);
    OSPTBOOL OSPPUsageIndHasOutCallId(OSPT_USAGEIND *);
    OSPTCALLID *OSPPUsageIndGetOutCallId(OSPT_USAGEIND *);
    void OSPPUsageIndSetOutCallId(OSPT_USAGEIND *, OSPTCALLID *);
    
#ifdef __cplusplus
}
#endif

#endif /* _OSPUSAGEIND_H */
