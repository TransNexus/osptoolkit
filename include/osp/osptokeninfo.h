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

/* osptokeninfo.h - OSP token info objects */
#ifndef _OSPTOKENINFO_H
#define _OSPTOKENINFO_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"

typedef struct {
    char lookAheadDest[OSPC_SIZE_SIGNALADDR];
    OSPE_PROTOCOL_NAME lookAheadProt;
    OSPE_DEST_OSPENABLED lookAheadDestOSPStatus;
} OSPTTOKENLOOKAHEADINFO;

typedef struct {
    char SourceNumber[OSPC_SIZE_E164NUM];
    char DestinationNumber[OSPC_SIZE_E164NUM];
    OSPT_CALL_ID *CallId;
    OSPTTIME ValidAfter;
    OSPTTIME ValidUntil;
    OSPTTRXID TrxId;
    int Duration;
    OSPTBOOL HasLookAheadInfo;
    OSPTTOKENLOOKAHEADINFO LookAheadInfo;
    OSPTBOOL HasDestinationNetworkId;
    char DestinationNetworkId[OSPC_SIZE_NORID];
} OSPT_TOKEN_INFO;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_TOKEN_INFO *OSPPTokenInfoNew(void);
    void OSPPTokenInfoDelete(OSPT_TOKEN_INFO **);
    unsigned OSPPTokenInfoFromElement(OSPT_XML_ELEM *, OSPT_TOKEN_INFO **);
    void OSPPTokenInfoSetSourceNumber(OSPT_TOKEN_INFO *, const char *);
    const char *OSPPTokenInfoGetSourceNumber(OSPT_TOKEN_INFO *);
    void OSPPTokenInfoSetDestNumber(OSPT_TOKEN_INFO *, const char *);
    const char *OSPPTokenInfoGetDestNumber(OSPT_TOKEN_INFO *);
    OSPTBOOL OSPPTokenInfoHasCallId(OSPT_TOKEN_INFO *);
    void OSPPTokenInfoSetCallId(OSPT_TOKEN_INFO *, OSPT_CALL_ID *);
    OSPT_CALL_ID *OSPPTokenInfoGetCallId(OSPT_TOKEN_INFO *);
    unsigned char *OSPPTokenInfoGetCallIdValue(OSPT_TOKEN_INFO *);
    unsigned OSPPTokenInfoGetCallIdSize(OSPT_TOKEN_INFO *);
    OSPTBOOL OSPPTokenInfoHasValidAfter(OSPT_TOKEN_INFO *);
    void OSPPTokenInfoSetValidAfter(OSPT_TOKEN_INFO *, OSPTTIME);
    OSPTTIME OSPPTokenInfoGetValidAfter(OSPT_TOKEN_INFO *);
    void OSPPTokenInfoSetValidUntil(OSPT_TOKEN_INFO *, OSPTTIME);
    OSPTTIME OSPPTokenInfoGetValidUntil(OSPT_TOKEN_INFO *);
    void OSPPTokenInfoSetTrxId(OSPT_TOKEN_INFO *, OSPTTRXID);
    OSPTTRXID OSPPTokenInfoGetTrxId(OSPT_TOKEN_INFO *);
    void OSPPTokenInfoSetDuration(OSPT_TOKEN_INFO *, int);
    int OSPPTokenInfoGetDuration(OSPT_TOKEN_INFO *);
    void OSPPTokenInfoSetLookAheadDestAlt(OSPTTOKENLOOKAHEADINFO *, const char *);
    const char *OSPPTokenInfoGetLookAheadDestAlt(OSPTTOKENLOOKAHEADINFO *);
    void OSPPTokenInfoSetLookAheadProtocol(OSPTTOKENLOOKAHEADINFO *, const char *);
    OSPE_PROTOCOL_NAME OSPPTokenInfoGetLookAheadProtocol(OSPTTOKENLOOKAHEADINFO *);
    void OSPPTokenInfoSetLookAheadOSPVersion(OSPTTOKENLOOKAHEADINFO *, const char *);
    OSPE_DEST_OSPENABLED OSPPTokenInfoGetLookAheadOSPVersion(OSPTTOKENLOOKAHEADINFO *);
    const char *OSPPTokenInfoGetDestNetworkId(OSPT_TOKEN_INFO *);
    unsigned OSPPParseTokenInfoFromASCIIToken(unsigned char *, unsigned, OSPT_TOKEN_INFO **);

#ifdef __cplusplus
}
#endif
#endif /* _OSPTOKENINFO_H */
