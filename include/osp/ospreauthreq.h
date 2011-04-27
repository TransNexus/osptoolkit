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

/* ospreauthreq.h - OSP reauthorization request objects */
#ifndef _OSPREAUTHREQ_H
#define _OSPREAUTHREQ_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospusage.h"
#include "osp/ospaltinfo.h"

typedef struct {
    OSPTTIME Timestamp;
    char *MessageId;
    char *ComponentId;
    unsigned Role;
    OSPTBOOL HasRole;
    OSPT_CALL_ID *CallId;
    char SourceNumber[OSPC_SIZE_E164NUM];
    char DestinationNumber[OSPC_SIZE_E164NUM];
    OSPTLIST SourceAlternate;
    OSPTLIST DestinationAlternate;
    OSPTTRXID TrxId;
    int Duration;
    OSPTLIST Tokens;
    unsigned long CustomerId;
    unsigned long DeviceId;
    OSPTLIST DeviceInfo;
} OSPT_REAUTH_REQ;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_REAUTH_REQ *OSPPReauthReqNew(void);
    void OSPPReauthReqDelete(OSPT_REAUTH_REQ **);
    int OSPPReauthReqToElement(OSPT_REAUTH_REQ *, OSPT_XML_ELEM **, void *);
    void OSPPReauthReqSetRole(OSPT_REAUTH_REQ *, OSPE_ROLE);
    OSPTBOOL OSPPReauthReqHasRole(OSPT_REAUTH_REQ *);
    OSPE_ROLE OSPPReauthReqGetRole(OSPT_REAUTH_REQ *);
    void OSPPReauthReqAddSourceAlt(OSPT_REAUTH_REQ *, OSPT_ALTINFO *);
    void OSPPReauthReqAddDestinationAlt(OSPT_REAUTH_REQ *, OSPT_ALTINFO *);
    void OSPPReauthReqSetCallId(OSPT_REAUTH_REQ *, OSPT_CALL_ID *);
    OSPTBOOL OSPPReauthReqHasMessageId(OSPT_REAUTH_REQ *);
    const char *OSPPReauthReqGetMessageId(OSPT_REAUTH_REQ *);
    OSPTBOOL OSPPReauthReqHasTimestamp(OSPT_REAUTH_REQ *);
    void OSPPReauthReqSetTimestamp(OSPT_REAUTH_REQ *, OSPTTIME);
    OSPTTIME OSPPReauthReqGetTimestamp(OSPT_REAUTH_REQ *);
    OSPTBOOL OSPPReauthReqHasComponentId(OSPT_REAUTH_REQ *);
    const char *OSPPReauthReqGetComponentId(OSPT_REAUTH_REQ *);
    OSPTBOOL OSPPReauthReqHasCallId(OSPT_REAUTH_REQ *);
    OSPT_CALL_ID *OSPPReauthReqGetCallId(OSPT_REAUTH_REQ *);
    void OSPPReauthReqSetSourceNumber(OSPT_REAUTH_REQ *, const char *);
    const char *OSPPReauthReqGetSourceNumber(OSPT_REAUTH_REQ *);
    void OSPPReauthReqSetDestNumber(OSPT_REAUTH_REQ *, const char *);
    const char *OSPPReauthReqGetDestNumber(OSPT_REAUTH_REQ *);
    OSPTBOOL OSPPReauthReqHasTrxId(OSPT_REAUTH_REQ *);
    void OSPPReauthReqSetTrxId(OSPT_REAUTH_REQ *, OSPTTRXID);
    OSPTTRXID OSPPReauthReqGetTrxId(OSPT_REAUTH_REQ *);
    OSPTBOOL OSPPReauthReqHasDuration(OSPT_REAUTH_REQ *);
    void OSPPReauthReqSetDuration(OSPT_REAUTH_REQ *, int ospvDuration);
    int OSPPReauthReqGetDuration(OSPT_REAUTH_REQ *);
    void OSPPReauthReqAddToken(OSPT_REAUTH_REQ *, OSPT_TOKEN *);
    OSPT_TOKEN *OSPPReauthReqFirstToken(OSPT_REAUTH_REQ *);
    OSPT_TOKEN *OSPPReauthReqNextToken(OSPT_REAUTH_REQ *, OSPT_TOKEN *);
    OSPTBOOL OSPPReauthReqHasDestinationAlt(OSPT_REAUTH_REQ *);
    OSPT_ALTINFO *OSPPReauthReqFirstDestinationAlt(OSPT_REAUTH_REQ *);
    OSPT_ALTINFO *OSPPReauthReqNextDestinationAlt(OSPT_REAUTH_REQ *, OSPT_ALTINFO *);
    const char *OSPPReauthReqGetDestinationAltValue(OSPT_ALTINFO *);
    OSPTBOOL OSPPReauthReqHasSourceAlt(OSPT_REAUTH_REQ *);
    OSPT_ALTINFO *OSPPReauthReqFirstSourceAlt(OSPT_REAUTH_REQ *);
    OSPT_ALTINFO *OSPPReauthReqNextSourceAlt(OSPT_REAUTH_REQ *, OSPT_ALTINFO *);
    OSPTBOOL OSPPReauthReqHasCustId(OSPT_REAUTH_REQ *);
    void OSPPReauthReqSetCustId(OSPT_REAUTH_REQ *, unsigned long);
    unsigned long OSPPReauthReqGetCustId(OSPT_REAUTH_REQ *);
    OSPTBOOL OSPPReauthReqHasDeviceId(OSPT_REAUTH_REQ *);
    void OSPPReauthReqSetDeviceId(OSPT_REAUTH_REQ *, unsigned long);
    unsigned long OSPPReauthReqGetDeviceId(OSPT_REAUTH_REQ *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPREAUTHREQ_H */
