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
 * osptrans.h - Constants and prototypes for global transaction functions.
 */
#ifndef _OSPTRANS_H
#define _OSPTRANS_H

#include "osp.h"
#include "ospauthreq.h"
#include "ospauthrsp.h"
#include "ospmsginfo.h"
#include "ospauthind.h"
#include "ospauthcnf.h"
#include "ospusageind.h"
#include "ospusagecnf.h"
#include "ospreauthreq.h"
#include "ospreauthrsp.h"
#include "ospstatistics.h"
#include "ospmsg.h"
#include "ospfail.h"

/*
 * Transaction States
 */
typedef enum
{
    OSPC_INVALID_STATE,
    OSPC_TRANSNEW,
    OSPC_AUTH_REQUEST_BLOCK,
    OSPC_AUTH_REQUEST_FAIL,
    OSPC_AUTH_REQUEST_SUCCESS,
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

/*-------------------------------------------*/
/* transaction typedef                       */
/*-------------------------------------------*/ 
typedef struct _OSPTTRANS
{
    struct _OSPTPROVIDER        *Provider;
    OSPTAUTHREQ                 *AuthReq;
    OSPTAUTHRSP                 *AuthRsp;
    OSPTDEST                    *CurrentDest;
    OSPTAUTHIND                 *AuthInd;
    OSPTAUTHCNF                 *AuthCnf;
    OSPTLIST                    UsageInd;      /* list of OSPTUSAGEIND*/
    OSPTUSAGECNF                *UsageCnf;
    OSPE_TRANS_STATE            State;
    OSPTUINT64                  TransactionID;
    OSPTBOOL                    HasTransactionID;
    unsigned                    Counter;
    unsigned                    Duration;
    const char                  *NetworkId;
    unsigned                    SizeOfDetailLog;
    void                        *DetailLog;
    OSPTSTATISTICS              *TNStatistics;
    OSPTREAUTHREQ               *ReauthReq;
    OSPTREAUTHRSP               *ReauthRsp;
} OSPTTRANS;

#define OSPC_MAX_TRANS  20000
/*-------------------------------------------*/
/* transaction collection typedef            */
/*-------------------------------------------*/ 
typedef struct _OSPTTRANCOLLECTION
{
    OSPTTRANS   *Trans[OSPC_MAX_TRANS];
    unsigned    NumberOfTrans;
    OSPTMUTEX   TransactionMutex;
} OSPTTRANCOLLECTION;

/* Types for parsing and creating forms */

#define     OSPC_USAGE   0x01    
#define     OSPC_AUTH    0x02
#define     OSPC_REAUTH  0x03


/* Transaction Mask */
#define OSPC_TRANSACTIONMASK    0xFFFFFF

#define OSPC_TRAN_TIME_UNLIMITED    0

#define OSPC_TRAN_HANDLE_INVALID    -1

/* MACROS */
#define OSPM_GET_TRANSACTION_INDEX(e) (e & OSPC_TRANSACTIONMASK);

/* Function Prototypes */
#ifdef __cplusplus
extern "C"
{
#endif

int             OSPPTransactionBuildReauthRequest(OSPTTRANS *, unsigned);
int             OSPPTransactionBuildUsage(OSPTTRANS *, OSPTUSAGEIND **,
                                      OSPTDEST *, OSPE_MSG_DATATYPES);

OSPTTRANS       *OSPPTransactionCollectionGetItem(OSPTTRANCOLLECTION  *, 
                                              OSPTCOLLECTIONINDEX);
OSPTTRANHANDLE OSPPTransactionCollectionGetNewItem(OSPTPROVHANDLE,
    OSPTTRANCOLLECTION *, int *);
void           OSPPTransactionCollectionRemoveItem(OSPTTRANCOLLECTION *,
                                                  OSPTCOLLECTIONINDEX);

void           OSPPTransactionDeleteRequest(OSPTTRANS *);
void           OSPPTransactionDeleteResponse(OSPTTRANS *);
void           OSPPTransactionDeleteAuthInd(OSPTTRANS *);
void           OSPPTransactionDeleteAuthCnf(OSPTTRANS *);
void           OSPPTransactionDeleteUsageInd(OSPTTRANS *);
void           OSPPTransactionDeleteUsageCnf(OSPTTRANS *);
void           OSPPTransactionDeleteStatistics(OSPTTRANS *);
void           OSPPTransactionDeleteReauthReq(OSPTTRANS *);
void           OSPPTransactionDeleteReauthRsp(OSPTTRANS *);

void           OSPPTransactionGetAccumAllowed(OSPTTRANS *, OSPTBOOL *);
OSPTTRANS     *OSPPTransactionGetContext(OSPTTRANHANDLE, int *);
int            OSPPTransactionGetCounter(OSPTTRANS *);
void           OSPPTransactionGetDeleteAllowed(OSPTTRANS *, OSPTBOOL *);
int            OSPPTransactionGetDestAllowed(OSPTTRANS *);
int            OSPPTransactionGetDestination(OSPTTRANS *, enum OSPEFAILREASON,
                   unsigned, char *, char *, unsigned *, unsigned *, void *,
                   unsigned, char *, unsigned, char *, unsigned, char *,
                   unsigned *, void *);
int         OSPPTransactionGetNewContext(OSPTPROVHANDLE, OSPTTRANHANDLE *);
int         OSPPTransactionGetProvider(OSPTTRANS *, struct _OSPTPROVIDER **);
void        OSPPTransactionGetReportUsageAllowed(OSPTTRANS *, OSPTBOOL *);
void        OSPPTransactionGetState(OSPTTRANS *, OSPE_TRANS_STATE *);
void        OSPPTransactionGetStatistics(OSPTTRANS *, OSPTSTATISTICS *);
OSPTBOOL    OSPPTransactionHasStatistics(OSPTTRANS *);

int         OSPPTransactionPrepareAndQueMessage(OSPTTRANS *, unsigned char *, 
                   unsigned, OSPTMSGINFO **);
int         OSPPTransactionProcessReturn(OSPTTRANS *, OSPTMSGINFO *);
int         OSPPTransactionRequestNew(OSPTTRANS *, const char *, const char *,
                   const char *, const char *, const char *, unsigned, 
                   OSPTCALLID *[], const char *[], unsigned *, unsigned *, 
                   void *);
int         OSPPTransactionResponseBuild(OSPTTRANS *, const char *, unsigned,
                   const void *, unsigned, const void *);
int         OSPPTransactionSetProvider(OSPTTRANS *, struct _OSPTPROVIDER *);
void        OSPPTransactionSetState(OSPTTRANS *, OSPE_TRANS_STATE);
void        OSPPTransactionUpdateCounter(OSPTTRANS *);
int         OSPPTransactionValidateTokenCert(OSPTTRANS *, unsigned char *, unsigned);
int         OSPPTransactionVerifyAuthCnf(OSPTAUTHCNF *);
int         OSPPTransactionVerifyUsageCnf(OSPTUSAGECNF *);
#ifdef __cplusplus
}
#endif
#endif

