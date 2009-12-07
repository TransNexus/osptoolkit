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
    OSPE_DEST_PROTOCOL lookAheadDestProt;
    OSPE_DEST_OSPENABLED lookAheadDestOSPStatus;
} OSPTTOKENLOOKAHEADINFO;

typedef struct {
    char ospmTokenInfoSourceNumber[OSPC_SIZE_E164NUM];
    char ospmTokenInfoDestNumber[OSPC_SIZE_E164NUM];
    OSPT_CALL_ID *ospmTokenInfoCallId;
    OSPTTIME ospmTokenInfoValidAfter;
    OSPTTIME ospmTokenInfoValidUntil;
    OSPTTRXID ospmTokenInfoTrxId;
    int ospmTokenInfoDuration;
    OSPTBOOL ospmTokenInfoHasLookAheadInfo;
    OSPTTOKENLOOKAHEADINFO ospmTokenLookAheadInfo;
    OSPTBOOL ospmTokenInfoHasDestNetworkId;
    char dstNetworkId[OSPC_SIZE_NORID];
} OSPTTOKENINFO;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPTTOKENINFO *OSPPTokenInfoNew(void);
    void OSPPTokenInfoDelete(OSPTTOKENINFO **);
    unsigned OSPPTokenInfoFromElement(OSPT_XML_ELEM *, OSPTTOKENINFO **);
    void OSPPTokenInfoSetSourceNumber(OSPTTOKENINFO *, const char *);
    const char *OSPPTokenInfoGetSourceNumber(OSPTTOKENINFO *);
    void OSPPTokenInfoSetDestNumber(OSPTTOKENINFO *, const char *);
    const char *OSPPTokenInfoGetDestNumber(OSPTTOKENINFO *);
    OSPTBOOL OSPPTokenInfoHasCallId(OSPTTOKENINFO *);
    void OSPPTokenInfoSetCallId(OSPTTOKENINFO *, OSPT_CALL_ID *);
    OSPT_CALL_ID *OSPPTokenInfoGetCallId(OSPTTOKENINFO *);
    unsigned char *OSPPTokenInfoGetCallIdValue(OSPTTOKENINFO *);
    unsigned OSPPTokenInfoGetCallIdSize(OSPTTOKENINFO *);
    OSPTBOOL OSPPTokenInfoHasValidAfter(OSPTTOKENINFO *);
    void OSPPTokenInfoSetValidAfter(OSPTTOKENINFO *, OSPTTIME);
    OSPTTIME OSPPTokenInfoGetValidAfter(OSPTTOKENINFO *);
    void OSPPTokenInfoSetValidUntil(OSPTTOKENINFO *, OSPTTIME);
    OSPTTIME OSPPTokenInfoGetValidUntil(OSPTTOKENINFO *);
    void OSPPTokenInfoSetTrxId(OSPTTOKENINFO *, OSPTTRXID);
    OSPTTRXID OSPPTokenInfoGetTrxId(OSPTTOKENINFO *);
    void OSPPTokenInfoSetDuration(OSPTTOKENINFO *, int);
    int OSPPTokenInfoGetDuration(OSPTTOKENINFO *);
    void OSPPTokenInfoSetLookAheadDestAlt(OSPTTOKENLOOKAHEADINFO *, const char *);
    const char *OSPPTokenInfoGetLookAheadDestAlt(OSPTTOKENLOOKAHEADINFO *);
    void OSPPTokenInfoSetLookAheadDestProtocol(OSPTTOKENLOOKAHEADINFO *, const char *);
    OSPE_DEST_PROTOCOL OSPPTokenInfoGetLookAheadDestProtocol(OSPTTOKENLOOKAHEADINFO *);
    void OSPPTokenInfoSetLookAheadOSPVersion(OSPTTOKENLOOKAHEADINFO *, const char *);
    OSPE_DEST_OSPENABLED OSPPTokenInfoGetLookAheadOSPVersion(OSPTTOKENLOOKAHEADINFO *);
    const char *OSPPTokenInfoGetDestNetworkId(OSPTTOKENINFO *);
    unsigned OSPPParseTokenInfoFromASCIIToken(unsigned char *, unsigned, OSPTTOKENINFO **);

#ifdef __cplusplus
}
#endif
#endif /* _OSPTOKENINFO_H */
