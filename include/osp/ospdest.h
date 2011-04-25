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

/* ospdest.h - OSP destination objects */
#ifndef _OSPDEST_H
#define _OSPDEST_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospmsgdesc.h"

/* Signaling Protocol Names */
typedef enum {
    OSPC_PROTNAME_UNKNOWN = OSPC_MPART_UNKNOWN,    /* Could not be understood by the Client as Sent by the Server */
    OSPC_PROTNAME_UNDEFINED,                       /* Not Configured at Server */
    /* Message attribute start */
    OSPC_PROTNAME_START = OSPC_MPART_START,        /* Start */
    OSPC_PROTNAME_SIP = OSPC_PROTNAME_START,       /* SIP */
    OSPC_PROTNAME_Q931,                            /* H323, Send Setup to Complete Call */
    OSPC_PROTNAME_LRQ,                             /* H323, Send LRQ to GK to Complete Call */
    OSPC_PROTNAME_IAX,                             /* IAX */
    OSPC_PROTNAME_T37,                             /* Fax T.37 */
    OSPC_PROTNAME_T38,                             /* Fax T.38 */
    OSPC_PROTNAME_SKYPE,                           /* Skype */
    OSPC_PROTNAME_SMPP,                            /* SMPP */
    OSPC_PROTNAME_XMPP,                            /* XMPP */
    OSPC_PROTNAME_SMS,                             /* SMS */
    /* Number of signaling protocol types */
    OSPC_PROTNAME_NUMBER
} OSPE_PROTOCOL_NAME;

/* externally declared global variables */
extern const OSPT_MSG_DESC OSPV_PROTNAME_DESCS[];

/* Destination OSP Enabled Data Type */
typedef enum {
    OSPC_DOSP_UNKNOWN = 0,    /* Could not be understood by the Client as Sent by the Server */
    OSPC_DOSP_UNDEFINED,      /* Not Configured at Server */
    OSPC_DOSP_TRUE,           /* Destination is OSP Enabled */
    OSPC_DOSP_FALSE           /* Destination is Not OSP Enabled */
} OSPE_DEST_OSPENABLED;

#define DEST_OSP_DIABLED                "0.0.0"
#define DEST_OSP_UNKNOWN                ""

/* Operator Name Type */
typedef enum {
    OSPC_OPNAME_UNKNOWN = OSPC_MPART_UNKNOWN,   /* Could not be understood by the Client as Sent by the Server */
    OSPC_OPNAME_UNDEFINED,                      /* Not Configured at Server */
    OSPC_OPNAME_START = 0,                      /* Operator Name start */
    OSPC_OPNAME_SPID = OSPC_OPNAME_START,       /* Service Provider ID */
    OSPC_OPNAME_OCN,                            /* Operating Company Number */
    OSPC_OPNAME_SPN,                            /* Service Provider Name */
    OSPC_OPNAME_ALTSPN,                         /* Alternate SPN */
    OSPC_OPNAME_MCC,                            /* Mobile Country Code */
    OSPC_OPNAME_MNC,                            /* Mobile Network Code */
    /* Number of operator name types */
    OSPC_OPNAME_NUMBER
} OSPE_OPERATOR_NAME;

#define DEFAULT_GETNEXTDEST_NO_ERROR    99999

typedef struct {
    OSPTLISTLINK ospmDestLink;
    char ospmDestNumber[OSPC_SIZE_E164NUM];
    char ospmSrcNumber[OSPC_SIZE_E164NUM];
    char ospmDestAddr[OSPC_SIZE_SIGNALADDR];
    char ospmDestDevAddr[OSPC_SIZE_SIGNALADDR];
    OSPTLIST ospmUpdatedSourceAddr;
    OSPTLIST ospmUpdatedDeviceInfo;
    OSPTTIME ospmDestValidAfter;
    OSPTTIME ospmDestValidUntil;
    OSPTLIST ospmDestTokens;
    char ospmDestAuthority[OSPC_SIZE_URL];
    OSPTBOOL ospmDestHasLimit;
    unsigned ospmDestLimit;
    OSPT_CALL_ID *ospmDestCallId;
    OSPT_TERM_CAUSE ospmDestTermCause;
    OSPE_PROTOCOL_NAME ospmProtocol;
    OSPE_DEST_OSPENABLED ospmDestOSPVersion;
    char ospmDestNetworkId[OSPC_SIZE_NORID];
    unsigned ospmDestDestinationCount;
    char ospmNPRn[OSPC_SIZE_E164NUM];
    char ospmNPCic[OSPC_SIZE_NORID];
    int ospmNPNpdi;
    char ospmOpName[OSPC_OPNAME_NUMBER][OSPC_SIZE_NORID];
    OSPTBOOL ospmIsNPQuery;
} OSPT_DEST;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_DEST *OSPPDestNew(void);
    void OSPPDestDelete(OSPT_DEST **);
    int OSPPDestFromElement(OSPT_XML_ELEM *, OSPT_DEST **);
    void OSPPDestSetCallId(OSPT_DEST *, const unsigned char *, unsigned);
    void OSPPDestSetProtocol(OSPT_DEST *, const char *);
    void OSPPDestSetOSPVersion(OSPT_DEST *, const char *);
    OSPTBOOL OSPPDestHasNumber(OSPT_DEST *ospvDest);
    void OSPPDestSetNumber(OSPT_DEST *, const char *);
    const char *OSPPDestGetNumber(OSPT_DEST *);
    OSPTBOOL OSPPDestHasSrcNumber(OSPT_DEST *ospvDest);
    void OSPPDestSetSrcNumber(OSPT_DEST *, const char *);
    const char *OSPPDestGetSrcNumber(OSPT_DEST *);
    OSPTBOOL OSPPDestHasAddr(OSPT_DEST *);
    void OSPPDestSetAddr(OSPT_DEST *, const char *);
    const char *OSPPDestGetAddr(OSPT_DEST *);
    OSPTBOOL OSPPDestDevHasAddr(OSPT_DEST *);
    void OSPPDestDevSetAddr(OSPT_DEST *, const char *);
    const char *OSPPDestDevGetAddr(OSPT_DEST *);
    OSPTBOOL OSPPDestHasNetworkAddr(OSPT_DEST *);
    void OSPPDestSetNetworkAddr(OSPT_DEST *, const char *);
    const char *OSPPDestGetNetworkAddr(OSPT_DEST *);
    OSPTBOOL OSPPDestHasValidAfter(OSPT_DEST *);
    void OSPPDestSetValidAfter(OSPT_DEST *, OSPTTIME);
    OSPTTIME OSPPDestGetValidAfter(OSPT_DEST *);
    OSPTBOOL OSPPDestHasValidUntil(OSPT_DEST *);
    void OSPPDestSetValidUntil(OSPT_DEST *, OSPTTIME);
    OSPTTIME OSPPDestGetValidUntil(OSPT_DEST *);
    OSPTBOOL OSPPDestHasAuthority(OSPT_DEST *);
    void OSPPDestSetAuthority(OSPT_DEST *, const char *);
    OSPTBOOL OSPPDestHasCallId(OSPT_DEST *);
    OSPT_CALL_ID *OSPPDestGetCallId(OSPT_DEST *);
    OSPTBOOL OSPPDestHasToken(OSPT_DEST *);
    void OSPPDestAddToken(OSPT_DEST *, OSPTTOKEN *);
    OSPTTOKEN *OSPPDestFirstToken(OSPT_DEST *);
    OSPTTOKEN *OSPPDestNextToken(OSPT_DEST *, OSPTTOKEN *);
    OSPTBOOL OSPPDestHasLimit(OSPT_DEST *);
    unsigned OSPPDestGetLimit(OSPT_DEST *);
    void OSPPDestSetLimit(OSPT_DEST *, unsigned);
    OSPTBOOL OSPPDestHasTermCause(OSPT_DEST *, OSPE_TERM_CAUSE);
    OSPTBOOL OSPPDestHasTermCauseAny(OSPT_DEST *);
    void OSPPDestSetTermCause(OSPT_DEST *, OSPE_TERM_CAUSE, unsigned, const char *);
    OSPT_TERM_CAUSE *OSPPDestGetTermCause(OSPT_DEST *);
    unsigned OSPPDestGetTCCode(OSPT_DEST *, OSPE_TERM_CAUSE);
    const char *OSPPDestGetTCDesc(OSPT_DEST *, OSPE_TERM_CAUSE);
    void OSPPDestSetDestinationCount(OSPT_DEST *, unsigned);
    unsigned OSPPDestGetDestinationCount(OSPT_DEST *);
    OSPE_PROTOCOL_NAME OSPPDestProtocolGetPart(const char *);
    const char *OSPPDestProtocolGetName(OSPE_PROTOCOL_NAME);
    void OSPPDestProtocolFromElement(OSPT_XML_ELEM *, OSPT_DEST *);
    void OSPPDestInfoFromElement(OSPT_XML_ELEM *, OSPT_DEST *);
    int OSPPUsageDetailFromElement(OSPT_XML_ELEM *, OSPT_DEST *);
    void OSPPServiceFromElement(OSPT_XML_ELEM *, OSPT_DEST *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPDEST_H */
