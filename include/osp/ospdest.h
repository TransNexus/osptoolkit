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
 * ospdest.h - OSP destination objects
 */

#ifndef _OSPDEST_H
#define _OSPDEST_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"

/*
 * Destination Protocol Data Type
 */
typedef enum {
    OSPE_DEST_PROT_UNDEFINED = 0,   /* Not Configured at Server */
    OSPE_DEST_PROT_SIP = 2,         /* Destination Protocol - SIP */
    OSPE_DEST_PROT_H323_LRQ = 4,    /* Destination Protocol - H323, Send LRQ to GK to Complete Call */
    OSPE_DEST_PROT_H323_SETUP = 8,  /* Destination Protocol - H323, Send Setup to Complete Call */
    OSPE_DEST_PROT_IAX = 16,        /* Destination Protocol - IAX */
    OSPE_DEST_PROT_UNKNOWN = 32     /* Could not be understood by the Client as Sent by the Server */
} OSPE_DEST_PROT;

#define DEST_PROT_IAX       "iax"
#define DEST_PROT_SIP       "sip"
#define DEST_PROT_H323_LRQ  "h323-LRQ"
#define DEST_PROT_H323_Q931 "h323-Q931"

/*-----------------------------------------------------------------------*
 * Destination OSP Enabled Data Type
 *-----------------------------------------------------------------------*/
typedef enum {
    OSPE_OSP_UNDEFINED = 0,     /* Not Configured at Server */
    OSPE_OSP_TRUE = 128,        /* Destination is OSP Enabled */
    OSPE_OSP_FALSE = 256,       /* Destination os Not OSP Enabled */
    OSPE_OSP_UNKNOWN = 512      /* Could not be understood by the Client as Sent by the Server */
} OSPE_DEST_OSP_ENABLED;

#define DEST_OSP_DIABLED                "0.0.0"
#define DEST_OSP_UNKNOWN                ""
#define DEFAULT_GETNEXTDEST_NO_ERROR    99999

#define OSPC_NETWORKIDSIZE  OSPC_E164NUMSIZE

typedef struct {
    OSPTLISTLINK ospmDestLink;
    unsigned char ospmDestNumber[OSPC_E164NUMSIZE];
    unsigned char ospmSrcNumber[OSPC_E164NUMSIZE];
    unsigned char ospmDestAddr[OSPC_SIGNALADDRSIZE];
    unsigned char ospmDestDevAddr[OSPC_SIGNALADDRSIZE];
    OSPTLIST ospmUpdatedSourceAddr;
    OSPTLIST ospmUpdatedDeviceInfo;
    OSPTTIME ospmDestValidAfter;
    OSPTTIME ospmDestValidUntil;
    OSPTLIST ospmDestTokens;
    unsigned char ospmDestAuthority[OSPC_URLSIZE];
    unsigned ospmDestHasLimit;
    unsigned ospmDestLimit;
    OSPTCALLID *ospmDestCallId;
    unsigned ospmDestTNFailReason;
    unsigned ospmDestTNFailReasonInd;
    OSPE_DEST_PROT ospmDestProtocol;
    OSPE_DEST_OSP_ENABLED ospmDestOSPVersion;
    unsigned char ospmDestNetworkId[OSPC_NETWORKIDSIZE];
    unsigned ospmDestDestinationCount;
} OSPTDEST;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTDEST *OSPPDestNew(void);
    void OSPPDestDelete(OSPTDEST **);
    unsigned OSPPDestFromElement(OSPTXMLELEM *, OSPTDEST **);
    void OSPPDestSetCallId(OSPTDEST *, const unsigned char *, unsigned);
    void OSPPDestSetProtocol(OSPTDEST *, const unsigned char *);
    void OSPPDestSetOSPVersion(OSPTDEST *, const unsigned char *);
    unsigned OSPPDestHasNumber(OSPTDEST *ospvDest);
    void OSPPDestSetNumber(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestGetNumber(OSPTDEST *);
    unsigned OSPPDestHasSrcNumber(OSPTDEST *ospvDest);
    void OSPPDestSetSrcNumber(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestGetSrcNumber(OSPTDEST *);
    unsigned OSPPDestHasAddr(OSPTDEST *);
    void OSPPDestSetAddr(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestGetAddr(OSPTDEST *);
    unsigned OSPPDestDevHasAddr(OSPTDEST *);
    void OSPPDestDevSetAddr(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestDevGetAddr(OSPTDEST *);
    unsigned OSPPDestHasNetworkAddr(OSPTDEST *);
    void OSPPDestSetNetworkAddr(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestGetNetworkAddr(OSPTDEST *);
    unsigned OSPPDestHasValidAfter(OSPTDEST *);
    void OSPPDestSetValidAfter(OSPTDEST *, OSPTTIME);
    OSPTTIME OSPPDestGetValidAfter(OSPTDEST *);
    unsigned OSPPDestHasValidUntil(OSPTDEST *);
    void OSPPDestSetValidUntil(OSPTDEST *, OSPTTIME);
    OSPTTIME OSPPDestGetValidUntil(OSPTDEST *);
    unsigned OSPPDestHasAuthority(OSPTDEST *);
    void OSPPDestSetAuthority(OSPTDEST *, const unsigned char *);
    unsigned OSPPDestHasCallId(OSPTDEST *);
    OSPTCALLID *OSPPDestGetCallId(OSPTDEST *);
    unsigned OSPPDestHasToken(OSPTDEST *);
    void OSPPDestAddToken(OSPTDEST *, OSPTTOKEN *);
    OSPTTOKEN *OSPPDestFirstToken(OSPTDEST *);
    OSPTTOKEN *OSPPDestNextToken(OSPTDEST *, OSPTTOKEN *);
    unsigned OSPPDestHasLimit(OSPTDEST *);
    unsigned OSPPDestGetLimit(OSPTDEST *);
    void OSPPDestSetLimit(OSPTDEST *, unsigned);
    unsigned OSPPDestHasTNFailReason(OSPTDEST *);
    void OSPPDestSetTNFailReason(OSPTDEST *, unsigned);
    unsigned OSPPDestGetTNFailReason(OSPTDEST *);
    void OSPPDestSetDestinationCount(OSPTDEST *, unsigned);
    unsigned OSPPDestGetDestinationCount(OSPTDEST *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPDEST_H */
