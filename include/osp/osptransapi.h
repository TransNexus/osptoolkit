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
 * osptransapi.h - Prototypes for transaction object API functions.
 */

#ifndef _OSPTRANSAPI_H
#define _OSPTRANSAPI_H

#include "osp/ospfail.h"
#include "osp/ospdest.h"

/* Authorisation Indicators */
#define OSPC_TRAN_AUTHORISED        0x0001
#define OSPC_TRAN_REAUTHORISED      0x0002
#define OSPC_TRAN_NOT_AUTHORISED    0xefff

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPTransactionGetDestProtocol(OSPTTRANHANDLE, OSPE_DEST_PROTOCOL *);
    int OSPPTransactionIsDestOSPEnabled(OSPTTRANHANDLE, OSPE_DEST_OSPENABLED *);
    int OSPPTransactionSetNetworkIds(OSPTTRANHANDLE, const char *, const char *);
    int OSPPTransactionAccumulateOneWayDelay(OSPTTRANHANDLE, unsigned, unsigned, unsigned, float);
    int OSPPTransactionAccumulateRoundTripDelay(OSPTTRANHANDLE, unsigned, unsigned, unsigned, float);
    int OSPPTransactionDelete(OSPTTRANHANDLE);
    int OSPPTransactionGetFirstDestination(OSPTTRANHANDLE, unsigned, char *, char *, unsigned *, unsigned *, void *,
            unsigned, char *, unsigned, char *, unsigned, char *, unsigned, char *, unsigned *, void *);
    int OSPPTransactionGetNextDestination(OSPTTRANHANDLE, OSPEFAILREASON, unsigned, char *, char *, unsigned *,
            unsigned *, void *, unsigned, char *, unsigned, char *, unsigned, char *, unsigned, char *, unsigned *, void *);
    int OSPPTransactionInitializeAtDevice(OSPTTRANHANDLE, unsigned, const char *, const char *, const char *, const char *,
            const char *, OSPE_NUMBER_FORMAT, const char *, OSPE_NUMBER_FORMAT, unsigned, const void *, unsigned, const void *,
            unsigned *, unsigned *, unsigned *, void *, unsigned);
    int OSPPTransactionNew(OSPTPROVHANDLE, OSPTTRANHANDLE *);
    int OSPPTransactionRecordFailure(OSPTTRANHANDLE, OSPEFAILREASON);
    int OSPPTransactionReinitializeAtDevice(OSPTTRANHANDLE, OSPEFAILREASON, unsigned, const char *, const char *,
            const char *, const char *, const char *, const char *, unsigned, const void *, unsigned, const void *, unsigned *,
            unsigned *, unsigned *, void *, unsigned);
    int OSPPTransactionReportUsage(OSPTTRANHANDLE, unsigned, OSPTTIME, OSPTTIME, OSPTTIME, OSPTTIME, unsigned, unsigned, unsigned,
            const char *, unsigned, signed, unsigned, signed, unsigned *, void *);
    int OSPPTransactionRequestAuthorisation(OSPTTRANHANDLE, const char *, const char *, const char *, OSPE_NUMBER_FORMAT, const char *,
            OSPE_NUMBER_FORMAT, const char *, unsigned, OSPT_CALL_ID *[], const char *[], unsigned *, unsigned *, void *);
    int OSPPTransactionIndicateCapabilities(OSPTTRANHANDLE, const char *, const char *, const char *, unsigned, unsigned *, void *);
    int OSPPTransactionRequestReauthorisation(OSPTTRANHANDLE, unsigned, unsigned *, void *, unsigned *, unsigned *, unsigned *, void *);
    int OSPPTransactionValidateAuthorisation(OSPTTRANHANDLE, const char *, const char *, const char *, const char *, const char *, OSPE_NUMBER_FORMAT,
            const char *, OSPE_NUMBER_FORMAT, unsigned, const void *, unsigned, const void *, unsigned *, unsigned *, unsigned *, void *, unsigned);
    int OSPPTransactionValidateReAuthorisation(OSPTTRANHANDLE, unsigned, const void *, unsigned *, unsigned *, unsigned *, void *, unsigned);
    int OSPPTransactionBuildUsageFromScratch(OSPTTRANHANDLE ospvTransaction,/* In - Transaction handle */
        OSPTUINT64 ospvServerTransactionId,                                 /* In - OSP Server Transaction Id */
        unsigned ospvRole,                                                  /* In - ogw, tgw or RADIUS */
        const char *ospvSource,                                             /* In - Source of call */
        const char *ospvDestination,                                        /* In - Destination for call */
        const char *ospvSourceDevice,                                       /* In - SourceDevice */
        const char *ospvDestinationDevice,                                  /* In - DestinationDevice */
        const char *ospvCallingNumber,                                      /* In - Calling number */
        OSPE_NUMBER_FORMAT ospvCallingNumberFormat,                         /* In - Calling number formaat : sip/e.164/url */
        const char *ospvCalledNumber,                                       /* In - Called number */
        OSPE_NUMBER_FORMAT ospvCalledNumberFormat,                          /* In - Called number formaat : sip/e.164/url */
        unsigned ospvSizeOfCallId,                                          /* In - Size of Callid */
        const void *ospvCallId,                                             /* In - Call identifier */
        OSPEFAILREASON ospvFailureReason,
        unsigned *ospvSizeOfDetailLog,                                      /* In/Out - Max size of detail log\ Actual size of detail log */
        void *ospvDetailLog);                                               /* In - Pointer to storage for detail log */
    int OSPPTransactionSetDestinationCount(OSPTTRANHANDLE ospvTransaction,  /*In - Transaction handle */
        unsigned ospvDestinationCount);                                     /*In - Optional Destination Count, 0 if n/a */
    int OSPPTransactionGetLookAheadInfoIfPresent(OSPTTRANHANDLE ospvTransaction,/* In - Transaction handle */
        OSPTBOOL *ospvHasLookAheadInfo,                                         /* Out */
        char *ospvLookAheadDestination,                                         /* Out */
        OSPE_DEST_PROTOCOL *ospvLookAheadDestProt,                              /* Out */
        OSPE_DEST_OSPENABLED *ospvLookAheadDestOSPStatus);                      /* Out */
    int OSPPTransactionModifyDeviceIdentifiers(OSPTTRANHANDLE ospvTransaction,  /* In - Transaction handle */
        const char *ospvSource,                                                 /* In - optional */
        const char *ospvSourceDevice,                                           /* In - optional */
        const char *ospvDestination,                                            /* In - optional */
        const char *ospvDestinationDevice);                                     /* In - optional */
    int OSPPTransactionGetDestNetworkId(OSPTTRANHANDLE ospvTransaction,         /* In - Transaction handle */
        char *ospvNetworkId);
    int OSPPTransactionSetServiceAndPricingInfo(OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
        OSPE_SERVICE ospvServiceType,                                           /* In- type of service, 0-voice, 1-data */
        OSPT_PRICING_INFO *ospvPricingInfo[]);                                  /* In- Pricing Info */
    int OSPPTransactionSetRoutingNumber(OSPTTRANHANDLE, const char *);
    int OSPPTransactionSetTermCause(OSPTTRANHANDLE, OSPE_TERM_CAUSE, unsigned, const char *);
    int OSPPTransactionSetAssertedId(OSPTTRANHANDLE, const char *);
    int OSPPTransactionSetDestProtocol(OSPTTRANHANDLE, OSPE_DEST_PROTOCOL);
    int OSPPTransactionSetForwardCodec(OSPTTRANHANDLE, const char *);
    int OSPPTransactionSetReverseCodec(OSPTTRANHANDLE, const char *);
    int OSPPTransactionSetSessionId(OSPTTRANHANDLE, OSPE_DIRECTION, OSPT_CALL_ID *);
    int OSPPTransactionSetCustomInfo(OSPTTRANHANDLE, unsigned, const char *);
    int OSPPTransactionSetDelayMean(OSPTTRANHANDLE, OSPE_DIRECTION, unsigned);
    int OSPPTransactionSetJitterMean(OSPTTRANHANDLE, OSPE_DIRECTION, unsigned);
    int OSPPTransactionSetPackLossMean(OSPTTRANHANDLE, OSPE_DIRECTION, unsigned);
    int OSPPTransactionSetSrcNetworkId(OSPTTRANHANDLE, const char *);
    int OSPPTransactionSetDestNetworkId(OSPTTRANHANDLE, const char *);
    int OSPPTransactionSetRFactor(OSPTTRANHANDLE, OSPE_DIRECTION, float);
    int OSPPTransactionSetMOS(OSPTTRANHANDLE, OSPE_DIRECTION, float);

#ifdef __cplusplus
}
#endif

#endif /* _OSPTRANSAPI_H */
