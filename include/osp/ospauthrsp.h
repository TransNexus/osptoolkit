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
 * ospauthrsp.h - OSP authorisation response objects
 */

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

typedef struct {
    OSPTTIME ospmAuthRspTimestamp;
    char *ospmAuthRspMessageId;
    char *ospmAuthRspComponentId;
    OSPTSTATUS *ospmAuthRspStatus;
    OSPTTNAUDIT *ospmAuthRspTNAudit;
    OSPTTRXID ospmAuthRspTrxId;
    OSPTLIST ospmAuthRspDest;
    unsigned ospmAuthRspDelayLimit;
    unsigned ospmAuthRspDelayPref;
    unsigned ospmNumDests;
    OSPTCSAUDIT *ospmAuthRspCSAudit;
    OSPE_ROLE_TYPE ospmAuthRspRole;
    OSPTBOOL ospmAuthRspHasRole;
} OSPTAUTHRSP;

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

    OSPTAUTHRSP *OSPPAuthRspNew(void);
    void OSPPAuthRspDelete(OSPTAUTHRSP **);
    unsigned OSPPAuthRspFromElement(OSPT_XML_ELEM *, OSPTAUTHRSP **);
    int OSPPAuthRspHasDest(OSPTAUTHRSP *ospvAuthRsp);
    OSPTDEST *OSPPAuthRspAddDest(OSPTAUTHRSP *, OSPTDEST *);
    OSPTDEST *OSPPAuthRspFirstDest(OSPTAUTHRSP *);
    OSPTDEST *OSPPAuthRspNextDest(OSPTAUTHRSP *, OSPTDEST *);
    OSPTBOOL OSPPAuthRspHasComponentId(OSPTAUTHRSP *);
    void OSPPAuthRspSetComponentId(OSPTAUTHRSP *, const char *);
    void OSPPAuthRspComponentIdFromElement(OSPT_XML_ELEM *, const char **);
    OSPTBOOL OSPPAuthRspHasMessageId(OSPTAUTHRSP *);
    void OSPPAuthRspSetMessageId(OSPTAUTHRSP *, const char *);
    void OSPPAuthRspMessageIdFromElement(OSPT_XML_ELEM *, const char **);
    const char *OSPPAuthRspGetMessageId(OSPTAUTHRSP *);
    void OSPPAuthRspSetTimestamp(OSPTAUTHRSP *, OSPTTIME);
    int OSPPAuthRspHasStatus(OSPTAUTHRSP *);
    OSPTSTATUS *OSPPAuthRspGetStatus(OSPTAUTHRSP *);
    void OSPPAuthRspSetTrxId(OSPTAUTHRSP *, OSPTTRXID);
    int OSPPAuthRspHasDelayLimit(OSPTAUTHRSP *);
    void OSPPAuthRspSetDelayLimit(OSPTAUTHRSP *, unsigned);
    unsigned OSPPAuthRspGetDelayLimit(OSPTAUTHRSP *);
    int OSPPAuthRspHasDelayPref(OSPTAUTHRSP *);
    void OSPPAuthRspSetDelayPref(OSPTAUTHRSP *, unsigned);
    unsigned OSPPAuthRspHasNumDests(OSPTAUTHRSP *);
    void OSPPAuthRspIncNumDests(OSPTAUTHRSP *);
    unsigned OSPPAuthRspGetNumDests(OSPTAUTHRSP *);
    OSPTTNAUDIT *OSPPAuthRspGetTNAudit(OSPTAUTHRSP *);
    int OSPPAuthRspHasCSAudit(OSPTAUTHRSP *);
    OSPTCSAUDIT *OSPPAuthRspGetCSAudit(OSPTAUTHRSP *);
    int OSPPAuthRspHasRole(OSPTAUTHRSP *);
    void OSPPAuthRspSetRole(OSPTAUTHRSP *, OSPE_ROLE_TYPE);
    OSPE_ROLE_TYPE OSPPAuthRspGetRole(OSPTAUTHRSP *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHRSP_H */
