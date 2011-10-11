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

/* osptrans.h - Constants and prototypes for global transaction functions. */
#ifndef _OSPTRANS_H
#define _OSPTRANS_H

#include "osp/osp.h"
#include "osp/ospauthreq.h"
#include "osp/ospauthrsp.h"
#include "osp/ospmsginfo.h"
#include "osp/ospauthind.h"
#include "osp/ospauthcnf.h"
#include "osp/ospusageind.h"
#include "osp/ospusagecnf.h"
#include "osp/ospreauthreq.h"
#include "osp/ospreauthrsp.h"
#include "osp/ospstatistics.h"
#include "osp/ospcapcnf.h"
#include "osp/ospmsg.h"
#include "osp/ospfail.h"
#include "osp/osptokeninfo.h"

/* Transaction States */
typedef enum {
    OSPC_INVALID_STATE,
    OSPC_TRANSNEW,
    OSPC_AUTH_REQUEST_BLOCK,
    OSPC_AUTH_REQUEST_FAIL,
    OSPC_AUTH_REQUEST_SUCCESS,
    OSPC_CAP_IND_BLOCK,
    OSPC_CAP_IND_FAIL,
    OSPC_CAP_IND_SUCCESS,
    OSPC_VALIDATE_AUTH_FAIL,
    OSPC_VALIDATE_AUTH_SUCCESS,
    OSPC_GET_DEST_FAIL,
    OSPC_GET_DEST_SUCCESS,
    OSPC_REPORT_USAGE_BLOCK,
    OSPC_REPORT_USAGE_FAIL,
    OSPC_REPORT_USAGE_SUCCESS,
    OSPC_INITIALIZE_SUCCESS,
    OSPC_INITIALIZE_FAIL,
    OSPC_REINITIALIZE_SUCCESS,
    OSPC_REINITIALIZE_FAIL,
    OSPC_ACCUMULATE_SUCCESS,
    OSPC_ACCUMULATE_FAIL
} OSPE_TRANS_STATE;

#define MAX_PRICING_INFO_ALLOWED 10

/* transaction typedef */
typedef struct _OSPTTRANS {
    struct _OSPTPROVIDER *Provider;
    OSPT_AUTH_REQ *AuthReq;
    OSPT_AUTH_RSP *AuthRsp;
    OSPT_DEST *CurrentDest;
    OSPT_AUTH_IND *AuthInd;
    OSPT_AUTH_CNF *AuthCnf;
    OSPTLIST UsageInd;          /* list of OSPT_USAGEIND */
    OSPT_USAGE_CNF *UsageCnf;
    OSPT_CAP_CNF *CapCnf;
    OSPE_TRANS_STATE State;
    OSPTUINT64 TransactionID;
    OSPTBOOL HasTransactionID;
    OSPTBOOL HasGetDestSucceeded;
    unsigned Counter;
    unsigned Duration;
    /* These two network ID's are only used to set network ID */
    char *SrcNetworkId;
    char *DestNetworkId;
    unsigned SizeOfDetailLog;
    void *DetailLog;
    OSPT_STATS *Statistics;
    OSPT_REAUTH_REQ *ReauthReq;
    OSPT_REAUTH_RSP *ReauthRsp;
    OSPTBOOL WasLookAheadInfoGivenToApp;
    OSPTBOOL TokenInfoHasLookAheadInfo;
    OSPTTOKENLOOKAHEADINFO TokenLookAheadInfo;
    OSPE_NUMBER_FORMAT CallingNumberFormat;
    OSPE_NUMBER_FORMAT CalledNumberFormat;
    OSPTBOOL HasServiceInfo;
    OSPE_SERVICE ServiceType;
    OSPTBOOL HasPricingInfo;
    unsigned NumOfPricingInfoElements;
    unsigned CurrentPricingInfoElement;
    OSPT_PRICING_INFO PricingInfo[MAX_PRICING_INFO_ALLOWED];
    char NPRn[OSPC_SIZE_E164NUM];
    char NPCic[OSPC_SIZE_NORID];
    int NPNpdi;
    char OpName[OSPC_OPNAME_NUMBER][OSPC_SIZE_NORID];
    char DivSrcInfo[OSPC_SIZE_E164NUM];
    char DivDevInfo[OSPC_SIZE_SIGNALADDR];
    OSPE_PROTOCOL_NAME Protocol[OSPC_PROTTYPE_NUMBER];
    char Codec[OSPC_CODEC_NUMBER][OSPC_SIZE_CODEC];
    OSPT_CALL_ID *SessionId[OSPC_SESSIONID_NUMBER];
    char *CustomInfo[OSPC_MAX_INDEX];
    /* This source network ID is only used to report usage */
    char UsageSrcNetworkId[OSPC_SIZE_NORID];
    char SrcRealm[OSPC_SIZE_NORSTR];
    char DestRealm[OSPC_SIZE_NORSTR];
    OSPE_NUMBER_FORMAT AssertedIdFormat;
    char AssertedId[OSPC_SIZE_URL];
    OSPE_NUMBER_FORMAT RemotePartyIdFormat;
    char RemotePartyId[OSPC_SIZE_URL];
    OSPE_NUMBER_FORMAT ChargeInfoFormat;
    char ChargeInfo[OSPC_SIZE_URL];
    char ApplicationId[OSPC_SIZE_NORSTR];
    OSPE_ROLE_STATE RoleState;
    OSPE_ROLE_FORMAT RoleFormat;
    OSPE_ROLE_VENDOR RoleVendor;
} OSPTTRANS;

#define OSPC_MAX_TRANS  20000

/* transaction collection typedef */
typedef struct _OSPTTRANCOLLECTION {
    OSPTTRANS *Trans[OSPC_MAX_TRANS];
    unsigned NumberOfTrans;
    OSPTMUTEX TransactionMutex;
} OSPTTRANCOLLECTION;

/* Types for parsing and creating forms */
#define OSPC_USAGE      0x01
#define OSPC_AUTH       0x02
#define OSPC_REAUTH     0x03

/* Transaction Mask */
#define OSPC_TRANSACTIONMASK        0xFFFFFF

#define OSPC_TRAN_TIME_UNLIMITED    0

#define OSPC_TRAN_HANDLE_INVALID    -1

/* MACROS */
#define OSPM_GET_TRANSACTION_INDEX(e)   (e & OSPC_TRANSACTIONMASK);

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    int OSPPTransactionBuildReauthRequest(OSPTTRANS *, unsigned);
    int OSPPTransactionBuildUsage(OSPTTRANS *, OSPT_USAGE_IND **, OSPT_DEST *, OSPE_MESSAGE);
    OSPTTRANS *OSPPTransactionCollectionGetItem(OSPTTRANCOLLECTION *, OSPTCOLLECTIONINDEX);
    OSPTTRANHANDLE OSPPTransactionCollectionGetNewItem(OSPTPROVHANDLE, OSPTTRANCOLLECTION *, int *);
    void OSPPTransactionCollectionRemoveItem(OSPTTRANCOLLECTION *, OSPTCOLLECTIONINDEX);
    void OSPPTransactionDeleteRequest(OSPTTRANS *);
    void OSPPTransactionDeleteResponse(OSPTTRANS *);
    void OSPPTransactionDeleteAuthInd(OSPTTRANS *);
    void OSPPTransactionDeleteAuthCnf(OSPTTRANS *);
    void OSPPTransactionDeleteUsageInd(OSPTTRANS *);
    void OSPPTransactionDeleteUsageCnf(OSPTTRANS *);
    void OSPPTransactionDeleteStatistics(OSPTTRANS *);
    void OSPPTransactionDeleteReauthReq(OSPTTRANS *);
    void OSPPTransactionDeleteReauthRsp(OSPTTRANS *);
    void OSPPTransactionDeleteCapCnf(OSPTTRANS *);
    void OSPPTransactionGetAccumAllowed(OSPTTRANS *, OSPTBOOL *);
    OSPTTRANS *OSPPTransactionGetContext(OSPTTRANHANDLE, int *);
    int OSPPTransactionGetCounter(OSPTTRANS *);
    void OSPPTransactionGetDeleteAllowed(OSPTTRANS *, OSPTBOOL *);
    void OSPPTransactionGetIsModifyDeviceIdAllowed(OSPTTRANS *, OSPTBOOL *);
    int OSPPTransactionGetDestAllowed(OSPTTRANS *);
    int OSPPTransactionGetDestination(OSPTTRANS *, OSPEFAILREASON, unsigned, char *, char *, unsigned *, unsigned *,
            void *, unsigned, char *, unsigned, char *, unsigned, char *, unsigned, char *, unsigned *, void *);
    int OSPPTransactionGetNewContext(OSPTPROVHANDLE, OSPTTRANHANDLE *);
    int OSPPTransactionGetProvider(OSPTTRANS *, struct _OSPTPROVIDER **);
    void OSPPTransactionGetReportUsageAllowed(OSPTTRANS *, OSPTBOOL *);
    void OSPPTransactionGetState(OSPTTRANS *, OSPE_TRANS_STATE *);
    void OSPPTransactionGetStatistics(OSPTTRANS *, OSPT_STATS *);
    OSPTBOOL OSPPTransactionHasStatistics(OSPTTRANS *);
    int OSPPTransactionPrepareAndQueMessage(OSPTTRANS *, unsigned char *, unsigned, OSPT_MSG_INFO **);
    int OSPPTransactionProcessReturn(OSPTTRANS *, OSPT_MSG_INFO *);
    int OSPPTransactionRequestNew(OSPTTRANS *, const char *, const char *, const char *, const char *, const char *, unsigned,
            OSPT_CALL_ID *[], const char *[], unsigned *, unsigned *, void *);
    int OSPPTransactionResponseBuild(OSPTTRANS *, const char *, const char *, unsigned, const void *, unsigned, const void *);
    int OSPPTransactionSetProvider(OSPTTRANS *, struct _OSPTPROVIDER *);
    void OSPPTransactionSetState(OSPTTRANS *, OSPE_TRANS_STATE);
    void OSPPTransactionUpdateCounter(OSPTTRANS *);
    int OSPPTransactionValidateTokenCert(OSPTTRANS *, unsigned char *, unsigned);
    int OSPPTransactionVerifyAuthCnf(OSPT_AUTH_CNF *);
    int OSPPTransactionVerifyUsageCnf(OSPT_USAGE_CNF *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPTRANS_H */
