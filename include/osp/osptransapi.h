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
            const char *, const char *, const char *,const char *,OSPE_NUMBERING_FORMAT, const char *,
            OSPE_NUMBERING_FORMAT, unsigned, const void *, unsigned, const void *,unsigned *,unsigned *,
            unsigned *, void *,unsigned);

    int OSPPTransactionNew(OSPTPROVHANDLE, OSPTTRANHANDLE *);

    int OSPPTransactionRecordFailure(OSPTTRANHANDLE, enum OSPEFAILREASON);

    int OSPPTransactionReinitializeAtDevice(OSPTTRANHANDLE, enum OSPEFAILREASON,
            unsigned, const char *, const char *, const char *, const char *,
            const char *,const char *,unsigned, const void *, unsigned,
            const void *, unsigned *, unsigned *, unsigned *, void *,unsigned);

    int OSPPTransactionReportUsage(OSPTTRANHANDLE, unsigned, OSPTTIME, unsigned, signed,
            unsigned, signed, unsigned *, void *);

    int OSPPTransactionRequestAuthorisation(OSPTTRANHANDLE, const char *, 
            const char *, const char *, OSPE_NUMBERING_FORMAT, const char *, OSPE_NUMBERING_FORMAT, const char *, 
            unsigned, OSPTCALLID *[], const char *[], unsigned *, unsigned *, 
            void *);

    int OSPPTransactionIndicateCapabilities(OSPTTRANHANDLE, const char *, 
            const char *, unsigned, unsigned *, void *);

    int OSPPTransactionRequestReauthorisation(OSPTTRANHANDLE, unsigned, unsigned *,           
            void *, unsigned *,unsigned *,unsigned *, void *);

    int OSPPTransactionValidateAuthorisation(OSPTTRANHANDLE, const char *,
            const char *, const char *, const char *, const char *, OSPE_NUMBERING_FORMAT, 
            const char *, OSPE_NUMBERING_FORMAT, unsigned, const void *, unsigned, const void *, 
            unsigned *, unsigned *, unsigned *, void *,unsigned);

    int OSPPTransactionValidateReAuthorisation(OSPTTRANHANDLE, unsigned, const void *,                 
            unsigned *, unsigned *, unsigned *, void *,unsigned);            

    int OSPPTransactionBuildUsageFromScratch(
    OSPTTRANHANDLE  ospvTransaction,        /*In - Transaction handle */
    OSPTUINT64  ospvServerTransactionId, /*In - OSP Server Transaction Id */
    unsigned        ospvIsSource,           /*In - Is this the ogw or tgw */
    const char     *ospvSource,             /*In - Source of call */
    const char     *ospvDestination,        /*In - Destination for call */
    const char     *ospvSourceDevice,       /*In - SourceDevice */
    const char     *ospvDestinationDevice,  /*In - DestinationDevice */
    const char     *ospvCallingNumber,      /*In - Calling number */
    OSPE_NUMBERING_FORMAT ospvCallingNumberFormat, /* In - Calling number formaat : sip/e.164/url */
    const char     *ospvCalledNumber,       /*In - Called number */
    OSPE_NUMBERING_FORMAT ospvCalledNumberFormat, /* In - Called number formaat : sip/e.164/url */
    unsigned        ospvSizeOfCallId,       /*In - Size of Callid */
    const void     *ospvCallId,             /*In - Call identifier */
    enum OSPEFAILREASON  ospvFailureReason,
    unsigned   *ospvSizeOfDetailLog,   /* In/Out - Max size of detail log\ Actual size of detail log */
    void           *ospvDetailLog);         /* In - Pointer to storage for detail log */

    int
    OSPPTransactionGetLookAheadInfoIfPresent(
        OSPTTRANHANDLE  ospvTransaction,    /* In - Transaction handle             */
        OSPTBOOL    *ospvIsLookAheadInfoPresent, /* Out */
        char        *ospvLookAheadDestination, /* Out */
        OSPE_DEST_PROT      *ospvLookAheadDestProt, /* Out */
        OSPE_DEST_OSP_ENABLED       *ospvLookAheadDestOSPStatus); /* Out */

    int
    OSPPTransactionModifyDeviceIdentifiers(
        OSPTTRANHANDLE  ospvTransaction,    /* In - Transaction handle             */
        const char      *ospvSource,        /* In - optional */
        const char      *ospvSourceDevice,  /* In - optional */
        const char      *ospvDestination,   /* In - optional */
        const char      *ospvDestinationDevice);  /* In - optional */


#ifdef __cplusplus
}
#endif

#endif

