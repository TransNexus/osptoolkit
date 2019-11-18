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

/* ospauthrsp.h - OSP authorization response objects */
#ifndef _OSPAUTHRSP_H
#define _OSPAUTHRSP_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospmsgattr.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospdest.h"
#include "osp/ospstatus.h"
#include "osp/osptnaudit.h"
#include "osp/ospciscoext.h"
#include "osp/ospstir.h"

typedef struct {
    OSPTTIME Timestamp;
    char *MessageId;
    char *ComponentId;
    OSPT_STATUS *Status;
    OSPT_TN_AUDIT *TNAudit;
    OSPTTRXID TrxId;
    OSPTLIST Destination;
    unsigned DelayLimit;
    unsigned DelayPref;
    unsigned NumDestination;
    OSPTCSAUDIT *CSAudit;
    OSPE_ROLE Role;
    OSPTBOOL HasRole;
    char *Identity;
    OSPT_TERM_CAUSE TermCause;
    OSPE_VERIFICATION_STATUS Verstat;
    char AttestInfo[OSPC_SIZE_NORSTR];
    char OrigId[OSPC_SIZE_NORID];
    char JurisdictionType[OSPC_SIZE_NORID];
} OSPT_AUTH_RSP;

#define OSPPAuthRspDestHasNumber(ospvDest) OSPPDestHasNumber(ospvDest)
#define OSPPAuthRspDestSetNumber(ospvDest,ospvNum) OSPPDestSetNumber(ospvDest,ospvNum)
#define OSPPAuthRspDestGetNumber(ospvDest) OSPPDestGetNumber(ospvDest)
#define OSPPAuthRspDestHasAddr(ospvDest) OSPPDestHasAddr(ospvDest)
#define OSPPAuthRspDestSetAddr(ospvDest,ospvAddr) OSPPDestSetAddr(ospvDest,ospvAddr)
#define OSPPAuthRspDestGetAddr(ospvDest) OSPPDestGetAddr(ospvDest)
#define OSPPAuthRspDestHasValidAfter(ospvDest) OSPPDestHasValidAfter(ospvDest)
#define OSPPAuthRspDestSetValidAfter(ospvDest,ospvTime) OSPPDestSetValidAfter(ospvDest,ospvTime)
#define OSPPAuthRspDestGetValidAfter(ospvDest) OSPPDestGetValidAfter(ospvDest)
#define OSPPAuthRspDestHasValidUntil(ospvDest) OSPPDestHasValidUntil(ospvDest)
#define OSPPAuthRspDestSetValidUntil(ospvDest,ospvTime) OSPPDestSetValidUntil(ospvDest,ospvTime)
#define OSPPAuthRspDestGetValidUntil(ospvDest) OSPPDestGetValidUntil(ospvDest)
#define OSPPAuthRspDestHasAuthority(ospvDest) OSPPDestHasAuthority(ospvDest)
#define OSPPAuthRspDestSetAuthority(ospvDest,ospvAuth) OSPPDestSetAuthority(ospvDest,ospvAuth)
#define OSPPAuthRspDestGetAuthority(ospvDest) OSPPDestGetAuthority(ospvDest)
#define OSPPAuthRspDestHasCallId(ospvDest) OSPPDestHasCallId(ospvDest)
#define OSPPAuthRspDestGetCallId(ospvDest) OSPPDestGetCallId(ospvDest)
#define OSPPAuthRspDestGetCallIdSize(ospvDest) OSPPDestGetCallIdSize(ospvDest)
#define OSPPAuthRspDestGetCallIdValue(ospvDest) OSPPDestGetCallIdValue(ospvDest)
#define OSPPAuthRspDestHasToken(ospvDest) OSPPDestHasToken(ospvDest)
#define OSPPAuthRspDestAddToken(ospvDest, ospvToken) OSPPDestAddToken(ospvDest, ospvToken)
#define OSPPAuthRspDestFirstToken(ospvDest) OSPPDestFirstToken(ospvDest)
#define OSPPAuthRspDestNextToken(ospvDest, ospvToken) OSPPDestNextToken(ospvDest, ospvToken)
#define OSPPAuthRspDestTokenGetSize(ospvToken) OSPPTokenGetSize(ospvToken)
#define OSPPAuthRspDestTokenGetValue(ospvToken) OSPPTokenGetValue(ospvToken)
#define OSPPAuthRspDestHasLimit(ospvDest) OSPPDestHasLimit(ospvDest)
#define OSPPAuthRspDestGetLimit(ospvDest) OSPPDestGetLimit(ospvDest)
#define OSPPAuthRspDestSetLimit(ospvDest, ospvLimit) OSPPDestSetLimit(ospvDest, ospvLimit)

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_AUTH_RSP *OSPPAuthRspNew(void);
    void OSPPAuthRspDelete(OSPT_AUTH_RSP **);
    unsigned OSPPAuthRspFromElement(OSPT_XML_ELEM *, OSPT_AUTH_RSP **);
    OSPTBOOL OSPPAuthRspHasDest(OSPT_AUTH_RSP *ospvAuthRsp);
    OSPT_DEST *OSPPAuthRspAddDest(OSPT_AUTH_RSP *, OSPT_DEST *);
    OSPT_DEST *OSPPAuthRspFirstDest(OSPT_AUTH_RSP *);
    OSPT_DEST *OSPPAuthRspNextDest(OSPT_AUTH_RSP *, OSPT_DEST *);
    OSPTBOOL OSPPAuthRspHasComponentId(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetComponentId(OSPT_AUTH_RSP *, const char *);
    void OSPPAuthRspComponentIdFromElement(OSPT_XML_ELEM *, const char **);
    OSPTBOOL OSPPAuthRspHasMessageId(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetMessageId(OSPT_AUTH_RSP *, const char *);
    void OSPPAuthRspMessageIdFromElement(OSPT_XML_ELEM *, const char **);
    const char *OSPPAuthRspGetMessageId(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetTimestamp(OSPT_AUTH_RSP *, OSPTTIME);
    OSPTBOOL OSPPAuthRspHasStatus(OSPT_AUTH_RSP *);
    OSPT_STATUS *OSPPAuthRspGetStatus(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetTrxId(OSPT_AUTH_RSP *, OSPTTRXID);
    OSPTBOOL OSPPAuthRspHasDelayLimit(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetDelayLimit(OSPT_AUTH_RSP *, unsigned);
    unsigned OSPPAuthRspGetDelayLimit(OSPT_AUTH_RSP *);
    OSPTBOOL OSPPAuthRspHasDelayPref(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetDelayPref(OSPT_AUTH_RSP *, unsigned);
    OSPTBOOL OSPPAuthRspHasNumDests(OSPT_AUTH_RSP *);
    void OSPPAuthRspIncNumDests(OSPT_AUTH_RSP *);
    unsigned OSPPAuthRspGetNumDests(OSPT_AUTH_RSP *);
    OSPT_TN_AUDIT *OSPPAuthRspGetTNAudit(OSPT_AUTH_RSP *);
    OSPTBOOL OSPPAuthRspHasCSAudit(OSPT_AUTH_RSP *);
    OSPTCSAUDIT *OSPPAuthRspGetCSAudit(OSPT_AUTH_RSP *);
    OSPTBOOL OSPPAuthRspHasRole(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetRole(OSPT_AUTH_RSP *, OSPE_ROLE);
    OSPE_ROLE OSPPAuthRspGetRole(OSPT_AUTH_RSP *);
    OSPTBOOL OSPPAuthRspHasIdentity(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetIdentity(OSPT_AUTH_RSP *, const char *);   
    void OSPPAuthRspSetTermCause(OSPT_AUTH_RSP *, OSPE_TERM_CAUSE, unsigned, const char *);
    OSPTBOOL OSPPAuthRspHasVerstat(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetVerstat(OSPT_AUTH_RSP *, OSPE_VERIFICATION_STATUS);
    OSPTBOOL OSPPAuthRspHasAttestInfo(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetAttestInfo(OSPT_AUTH_RSP *, const char *);
    OSPTBOOL OSPPAuthRspHasOrigId(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetOrigId(OSPT_AUTH_RSP *, const char *);
    OSPTBOOL OSPPAuthRspHasJurisdictionType(OSPT_AUTH_RSP *);
    void OSPPAuthRspSetJurisdictionType(OSPT_AUTH_RSP *, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHRSP_H */
