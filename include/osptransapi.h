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

#include "ospfail.h"
#include "ospdest.h"

/* Authorisation Indicators */
#define OSPC_TRAN_AUTHORISED        0x0001
#define OSPC_TRAN_REAUTHORISED      0x0002
#define OSPC_TRAN_NOT_AUTHORISED    0xefff

#ifdef __cplusplus
extern "C" 
{
#endif

    /* Function Prototypes */

    int OSPPTransactionGetDestProtocol(OSPTTRANHANDLE,OSPE_DEST_PROT *);

    int OSPPTransactionIsDestOSPEnabled(OSPTTRANHANDLE,OSPE_DEST_OSP_ENABLED *);

    int OSPPTransactionSetNetworkId(OSPTTRANHANDLE,const char *);

    int OSPPTransactionAccumulateOneWayDelay(OSPTTRANHANDLE, unsigned, 
            unsigned, unsigned, float);

    int OSPPTransactionAccumulateRoundTripDelay(OSPTTRANHANDLE, unsigned, 
            unsigned, unsigned, float);

    int OSPPTransactionDelete(OSPTTRANHANDLE);

    int OSPPTransactionGetFirstDestination(OSPTTRANHANDLE, unsigned, char *,
            char *, unsigned *, unsigned *, void *, unsigned, char *,
            unsigned, char *, unsigned, char *,unsigned *, void *);

    int OSPPTransactionGetNextDestination(OSPTTRANHANDLE, enum OSPEFAILREASON,
            unsigned, char *, char *, unsigned *, unsigned *, void *, unsigned,
            char *, unsigned, char *, unsigned, char *, unsigned *, void *);

    int OSPPTransactionInitializeAtDevice(OSPTTRANHANDLE, unsigned, const char *,
            const char *, const char *, const char *,const char *,const char *,
            unsigned, const void *, unsigned, const void *,unsigned *,unsigned *,
            unsigned *, void *);

    int OSPPTransactionNew(OSPTPROVHANDLE, OSPTTRANHANDLE *);

    int OSPPTransactionRecordFailure(OSPTTRANHANDLE, enum OSPEFAILREASON);

    int OSPPTransactionReinitializeAtDevice(OSPTTRANHANDLE, enum OSPEFAILREASON,
            unsigned, const char *, const char *, const char *, const char *,
            const char *, const char *, unsigned, const void *, unsigned,
            const void *, unsigned *, unsigned *, unsigned *, void *);

    int OSPPTransactionReportUsage(OSPTTRANHANDLE, unsigned, OSPTTIME, unsigned, signed,
            unsigned, signed, unsigned *, void *);

    int OSPPTransactionRequestAuthorisation(OSPTTRANHANDLE, const char *, 
            const char *, const char *, const char *, const char *, 
            unsigned, OSPTCALLID *[], const char *[], unsigned *, unsigned *, 
            void *);

    int OSPPTransactionIndicateCapabilities(OSPTTRANHANDLE, const char *, 
            const char *, unsigned, unsigned *, void *);

    int OSPPTransactionRequestReauthorisation(OSPTTRANHANDLE, unsigned, unsigned *,           
            void *, unsigned *,unsigned *,unsigned *, void *);

    int OSPPTransactionValidateAuthorisation(OSPTTRANHANDLE, const char *,
            const char *, const char *, const char *, const char *, 
            const char *, unsigned, const void *, unsigned, const void *, 
            unsigned *, unsigned *, unsigned *, void *);

    int OSPPTransactionValidateReAuthorisation(OSPTTRANHANDLE, unsigned, const void *,                 
            unsigned *, unsigned *, unsigned *, void *);            
#ifdef __cplusplus
}
#endif

#endif

