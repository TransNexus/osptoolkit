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
    OSPC_DPROT_UNDEFINED = 0,   /* Not Configured at Server */
    OSPC_DPROT_SIP,             /* Destination Protocol - SIP */
    OSPC_DPROT_LRQ,             /* Destination Protocol - H323, Send LRQ to GK to Complete Call */
    OSPC_DPROT_Q931,            /* Destination Protocol - H323, Send Setup to Complete Call */
    OSPC_DPROT_IAX,             /* Destination Protocol - IAX */
    OSPC_DPROT_XMPP,            /* Destination Protocol - XMPP */
    OSPC_DPROT_UNKNOWN          /* Could not be understood by the Client as Sent by the Server */
} OSPE_DEST_PROT;

#define OSPC_DPDESC_SIP     "sip"
#define OSPC_DPDESC_LRQ     "h323-LRQ"
#define OSPC_DPDESC_Q931    "h323-Q931"
#define OSPC_DPDESC_IAX     "iax"
#define OSPC_DPDESC_XMPP    "xmpp"

/*-----------------------------------------------------------------------*
 * Destination OSP Enabled Data Type
 *-----------------------------------------------------------------------*/
typedef enum {
    OSPC_DOSP_UNDEFINED = 0,    /* Not Configured at Server */
    OSPC_DOSP_TRUE,             /* Destination is OSP Enabled */
    OSPC_DOSP_FALSE,            /* Destination os Not OSP Enabled */
    OSPC_DOSP_UNKNOWN           /* Could not be understood by the Client as Sent by the Server */
} OSPE_DEST_OSPENABLED;

#define DEST_OSP_DIABLED                "0.0.0"
#define DEST_OSP_UNKNOWN                ""

#define DEFAULT_GETNEXTDEST_NO_ERROR    99999

#define OSPC_NETWORKIDSIZE  OSPC_SIZE_E164NUM

typedef struct {
    OSPTLISTLINK ospmDestLink;
    unsigned char ospmDestNumber[OSPC_SIZE_E164NUM];
    unsigned char ospmSrcNumber[OSPC_SIZE_E164NUM];
    unsigned char ospmDestAddr[OSPC_SIZE_SIGNALADDR];
    unsigned char ospmDestDevAddr[OSPC_SIZE_SIGNALADDR];
    OSPTLIST ospmUpdatedSourceAddr;
    OSPTLIST ospmUpdatedDeviceInfo;
    OSPTTIME ospmDestValidAfter;
    OSPTTIME ospmDestValidUntil;
    OSPTLIST ospmDestTokens;
    unsigned char ospmDestAuthority[OSPC_SIZE_URL];
    unsigned ospmDestHasLimit;
    unsigned ospmDestLimit;
    OSPTCALLID *ospmDestCallId;
    unsigned ospmDestFailReason;
    OSPTBOOL ospmDestHasFailReason;
    OSPE_DEST_PROT ospmDestProtocol;
    OSPE_DEST_OSPENABLED ospmDestOSPVersion;
    unsigned char ospmDestNetworkId[OSPC_NETWORKIDSIZE];
    unsigned ospmDestDestinationCount;
} OSPTDEST;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTDEST *OSPPDestNew(void);
    void OSPPDestDelete(OSPTDEST **);
    unsigned OSPPDestFromElement(OSPT_XML_ELEM *, OSPTDEST **);
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
    unsigned OSPPDestHasFailReason(OSPTDEST *);
    void OSPPDestSetFailReason(OSPTDEST *, unsigned);
    unsigned OSPPDestGetFailReason(OSPTDEST *);
    void OSPPDestSetDestinationCount(OSPTDEST *, unsigned);
    unsigned OSPPDestGetDestinationCount(OSPTDEST *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPDEST_H */
