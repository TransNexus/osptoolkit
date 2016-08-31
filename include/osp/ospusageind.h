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

/* ospusageind.h - OSP Usage Indication Objects */
#ifndef _OSPUSAGEIND_H
#define _OSPUSAGEIND_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/ospaltinfo.h"
#include "osp/ospstatistics.h"

typedef struct {
    OSPTLISTLINK Link;
    OSPTTIME Timestamp;
    OSPTTIME StartTime;
    OSPTTIME EndTime;
    OSPTTIME AlertTime;
    OSPTTIME ConnectTime;
    OSPTBOOL HasPDD;
    unsigned PostDialDelay;                 /* In milliseconds */
    int ProviderPDD;                        /* In milliseconds */
    OSPE_RELEASE ReleaseSource;
    char ConferenceId[OSPC_SIZE_CONFID];    /* This is in chararcters */
    char *MessageId;
    char *ComponentId;
    OSPE_ROLE Role;
    OSPTBOOL HasRole;
    OSPTTRXID TransactionId;
    OSPT_CALL_ID *CallId;
    int Duration;
    char SourceNumber[OSPC_SIZE_E164NUM];
    char DestinationNumber[OSPC_SIZE_E164NUM];
    OSPTLIST SourceAlternate;
    OSPTLIST DeviceInfo;
    OSPTLIST DestinationAlternate;
    unsigned long CustomerId;
    unsigned long DeviceId;
    OSPT_TERM_CAUSE TermCause;
    OSPT_STATS *Stats;
    OSPTBOOL HasPricingInfo;
    OSPT_PRICING_INFO PricingInfo;
    OSPTBOOL HasServiceInfo;
    OSPE_SERVICE ServiceType;
    OSPT_ALTINFO *DestinationCount;
    int SetupAttempt;
    OSPE_PROTOCOL_NAME Protocol[OSPC_PROTTYPE_NUMBER];
    char Codec[OSPC_SERVICE_NUMBER][OSPC_CODEC_NUMBER][OSPC_SIZE_CODEC];
    OSPT_CALL_ID *SessionId[OSPC_SESSIONID_NUMBER];
    OSPE_ROLE_STATE RoleState;
    OSPE_ROLE_FORMAT RoleFormat;
    OSPE_ROLE_VENDOR RoleVendor;
    OSPT_CALL_PARTY CallParty[OSPC_CPARTY_NUMBER];
    char TransferId[OSPC_SIZE_NORSTR];
    OSPE_TRANSFER_STATUS TransferStatus;
    char DestAudioAddr[OSPC_SIZE_NORSTR];
    char DestVideoAddr[OSPC_SIZE_NORSTR];
    char CallType[OSPC_SIZE_NORSTR];
    char CallCategory[OSPC_SIZE_NORSTR];
    char NetworkType[OSPC_SIZE_NORSTR];
} OSPT_USAGE_IND;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_USAGE_IND *OSPPUsageIndNew(void);
    void OSPPUsageIndDelete(OSPT_USAGE_IND **);
    int OSPPUsageIndToElement(OSPTLIST *, OSPT_XML_ELEM **, void *);
    void OSPPUsageIndMoveSourceAlt(OSPT_USAGE_IND *, OSPTLIST *);
    void OSPPUsageIndMoveDeviceInfo(OSPT_USAGE_IND *, OSPTLIST *);
    void OSPPUsageIndMoveDestinationAlt(OSPT_USAGE_IND *, OSPTLIST *);
    void OSPPUsageIndCopySourceAlt(OSPT_USAGE_IND *, OSPTLIST *);
    void OSPPUsageIndMergeSourceAlt(OSPT_USAGE_IND *, OSPTLIST *, OSPTLIST *);
    void OSPPUsageIndCopyDeviceInfo(OSPT_USAGE_IND *, OSPTLIST *);
    OSPTBOOL OSPPUsageIndHasMessageId(OSPT_USAGE_IND *);
    const char *OSPPUsageIndGetMessageId(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasTimestamp(OSPT_USAGE_IND *);
    void OSPPUsageIndSetTimestamp(OSPT_USAGE_IND *, OSPTTIME);
    OSPTTIME OSPPUsageIndGetTimestamp(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasComponentId(OSPT_USAGE_IND *);
    void OSPPUsageIndSetComponentId(OSPT_USAGE_IND *, const char *);
    const char *OSPPUsageIndGetComponentId(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasRole(OSPT_USAGE_IND *);
    OSPE_ROLE OSPPUsageIndGetRole(OSPT_USAGE_IND *);
    void OSPPUsageIndSetRole(OSPT_USAGE_IND *, OSPE_ROLE);
    OSPTBOOL OSPPUsageIndHasTransactionId(OSPT_USAGE_IND *);
    void OSPPUsageIndSetTransactionId(OSPT_USAGE_IND *, OSPTTRXID ospvTransactionId);
    OSPTTRXID OSPPUsageIndGetTransactionId(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasCallId(OSPT_USAGE_IND *);
    OSPT_CALL_ID *OSPPUsageIndGetCallId(OSPT_USAGE_IND *);
    void OSPPUsageIndSetCallId(OSPT_USAGE_IND *, OSPT_CALL_ID *);
    void OSPPUsageIndSetSourceNumber(OSPT_USAGE_IND *, const char *);
    const char *OSPPUsageIndGetSourceNumber(OSPT_USAGE_IND *);
    void OSPPUsageIndSetCurrency(OSPT_USAGE_IND *, const char *);
    const char *OSPPUsageIndGetCurrency(OSPT_USAGE_IND *);
    void OSPPUsageIndSetDestNumber(OSPT_USAGE_IND *, const char *);
    const char *OSPPUsageIndGetDestNumber(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasSourceAlt(OSPT_USAGE_IND *);
    OSPT_ALTINFO *OSPPUsageIndFirstSourceAlt(OSPT_USAGE_IND *);
    OSPT_ALTINFO *OSPPUsageIndNextSourceAlt(OSPT_USAGE_IND *, OSPT_ALTINFO *);
    OSPTBOOL OSPPUsageIndHasDestinationAlt(OSPT_USAGE_IND *);
    unsigned OSPPUsageIndGetDestinationAltSize(OSPT_ALTINFO *);
    OSPT_ALTINFO *OSPPUsageIndFirstDestinationAlt(OSPT_USAGE_IND *);
    OSPT_ALTINFO *OSPPUsageIndNextDestinationAlt(OSPT_USAGE_IND *, OSPT_ALTINFO *);
    OSPTBOOL OSPPUsageIndHasDuration(OSPT_USAGE_IND *);
    void OSPPUsageIndSetDuration(OSPT_USAGE_IND *, int ospvDuration);
    int OSPPUsageIndGetDuration(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasCustId(OSPT_USAGE_IND *ospvUsageInd);
    void OSPPUsageIndSetCustId(OSPT_USAGE_IND *, unsigned long);
    unsigned long OSPPUsageIndGetCustId(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasDeviceId(OSPT_USAGE_IND *ospvUsageInd);
    void OSPPUsageIndSetDeviceId(OSPT_USAGE_IND *, unsigned long);
    unsigned long OSPPUsageIndGetDeviceId(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasTermCause(OSPT_USAGE_IND *ospvUsageInd, OSPE_TERM_CAUSE);
    void OSPPUsageIndSetTermCause(OSPT_USAGE_IND *, OSPE_TERM_CAUSE, unsigned, const char *);
    void OSPPUsageIndCopyTermCause(OSPT_USAGE_IND *, OSPT_TERM_CAUSE *);
    unsigned OSPPUsageIndGetTCCode(OSPT_USAGE_IND *, OSPE_TERM_CAUSE);
    const char *OSPPUsageIndGetTCDesc(OSPT_USAGE_IND *, OSPE_TERM_CAUSE);
    OSPTBOOL OSPPUsageIndHasStatistics(OSPT_USAGE_IND *);
    void OSPPUsageIndSetStatistics(OSPT_USAGE_IND *, OSPT_STATS *);
    void OSPPUsageIndAddSourceAlt(OSPT_USAGE_IND *ospvUsageInd, OSPT_ALTINFO *ospvAltInfo);
    void OSPPUsageIndAddDestinationAlt(OSPT_USAGE_IND *ospvUsageInd, OSPT_ALTINFO *ospvAltInfo);
    void OSPPUsageIndSetStartTime(OSPT_USAGE_IND *, OSPTTIME ospvStartTime);
    OSPTTIME OSPPUsageIndGetStartTime(OSPT_USAGE_IND *);
    void OSPPUsageIndSetEndTime(OSPT_USAGE_IND *, OSPTTIME ospvEndTime);
    OSPTTIME OSPPUsageIndGetEndTime(OSPT_USAGE_IND *);
    void OSPPUsageIndSetConnectTime(OSPT_USAGE_IND *, OSPTTIME ospvEndTime);
    OSPTTIME OSPPUsageIndGetConnectTime(OSPT_USAGE_IND *);
    void OSPPUsageIndSetAlertTime(OSPT_USAGE_IND *, OSPTTIME ospvEndTime);
    OSPTTIME OSPPUsageIndGetAlertTime(OSPT_USAGE_IND *);
    void OSPPUsageIndSetPostDialDelay(OSPT_USAGE_IND *, unsigned ospvPostDialDelay);
    unsigned OSPPUsageIndGetPostDialDelay(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasPDD(OSPT_USAGE_IND *);
    void OSPPUsageIndSetReleaseSource(OSPT_USAGE_IND *, OSPE_RELEASE ospvReleaseSource);
    OSPE_RELEASE OSPPUsageIndGetReleaseSource(OSPT_USAGE_IND *);
    void OSPPUsageIndSetConferenceId(OSPT_USAGE_IND *, const char *);
    const char *OSPPUsageIndGetConferenceId(OSPT_USAGE_IND *);
    void OSPPUsageIndSetDestinationCount(OSPT_USAGE_IND *, unsigned ospvDestinationCount);
    OSPT_ALTINFO *OSPPUsageIndGetDestinationCount(OSPT_USAGE_IND *);
    OSPTBOOL OSPPUsageIndHasProtocol(OSPT_USAGE_IND *, OSPE_PROTOCOL_TYPE);
    OSPE_PROTOCOL_NAME OSPPUsageIndGetProtocol(OSPT_USAGE_IND *, OSPE_PROTOCOL_TYPE);
    void OSPPUsageIndSetProtocol(OSPT_USAGE_IND *, OSPE_PROTOCOL_TYPE, OSPE_PROTOCOL_NAME);
    OSPTBOOL OSPPUsageIndHasCodec(OSPT_USAGE_IND *, OSPE_SERVICE, OSPE_CODEC_TYPE);
    const char *OSPPUsageIndGetCodec(OSPT_USAGE_IND *, OSPE_SERVICE, OSPE_CODEC_TYPE);
    void OSPPUsageIndSetCodec(OSPT_USAGE_IND *, OSPE_SERVICE, OSPE_CODEC_TYPE, const char *);
    OSPTBOOL OSPPUsageIndHasSessionId(OSPT_USAGE_IND *, OSPE_SESSION_ID);
    OSPT_CALL_ID *OSPPUsageIndGetSessionId(OSPT_USAGE_IND *, OSPE_SESSION_ID);
    void OSPPUsageIndSetSessionId(OSPT_USAGE_IND *, OSPE_SESSION_ID, OSPT_CALL_ID *);
    void OSPPUsageIndSetChargingVevtor(OSPT_USAGE_IND *, const char *, const char *, const char *, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPUSAGEIND_H */
