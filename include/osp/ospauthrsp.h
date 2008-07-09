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
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospdest.h"
#include "osp/ospstatus.h"
#include "osp/osptnaudit.h"
#include "osp/ospciscoext.h"

typedef struct {
    OSPTTIME ospmAuthRspTimestamp;
    unsigned char* ospmAuthRspMessageId;
    unsigned char* ospmAuthRspComponentId;
    OSPTSTATUS* ospmAuthRspStatus;
    OSPTTNAUDIT* ospmAuthRspTNAudit;
    OSPTTRXID ospmAuthRspTrxId;
    OSPTLIST ospmAuthRspDest;
    unsigned ospmAuthRspTNDelayLimit;
    unsigned ospmAuthRspTNDelayPref;
    unsigned ospmNumDests;
    OSPTCSAUDIT* ospmAuthRspCSAudit;
    unsigned ospmAuthRspRole;
    unsigned ospmAuthRspHasRole;
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

#ifdef __cplusplus
extern "C" 
{
#endif

    /**/
    /*
     *-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*
     */

    OSPTAUTHRSP* OSPPAuthRspNew(void);
    void OSPPAuthRspDelete(OSPTAUTHRSP**);

    unsigned OSPPAuthRspFromElement(OSPTXMLELEM*, OSPTAUTHRSP**);

    int OSPPAuthRspHasDest(OSPTAUTHRSP *ospvAuthRsp);
    OSPTDEST* OSPPAuthRspAddDest(OSPTAUTHRSP*, OSPTDEST*);
    OSPTDEST* OSPPAuthRspFirstDest(OSPTAUTHRSP*);
    OSPTDEST* OSPPAuthRspNextDest(OSPTAUTHRSP*, OSPTDEST*);

    int OSPPAuthRspHasComponentId(OSPTAUTHRSP*);
    void OSPPAuthRspSetComponentId(OSPTAUTHRSP*, unsigned char*);
    void OSPPAuthRspComponentIdFromElement(OSPTXMLELEM*, unsigned char**);

    int OSPPAuthRspHasMessageId(OSPTAUTHRSP*);
    void OSPPAuthRspSetMessageId(OSPTAUTHRSP*, unsigned char*);
    void OSPPAuthRspMessageIdFromElement(OSPTXMLELEM*, unsigned char**);
    unsigned char* OSPPAuthRspGetMessageId(OSPTAUTHRSP*);

    void OSPPAuthRspSetTimestamp(OSPTAUTHRSP*, OSPTTIME);

    int OSPPAuthRspHasStatus(OSPTAUTHRSP*);
    OSPTSTATUS* OSPPAuthRspGetStatus(OSPTAUTHRSP*);

    void OSPPAuthRspSetTrxId(OSPTAUTHRSP*, OSPTTRXID);

    int OSPPAuthRspHasTNDelayLimit(OSPTAUTHRSP*);
    void OSPPAuthRspSetTNDelayLimit(OSPTAUTHRSP*, unsigned);
    unsigned OSPPAuthRspGetTNDelayLimit(OSPTAUTHRSP*);

    int OSPPAuthRspHasTNDelayPref(OSPTAUTHRSP*);
    void OSPPAuthRspSetTNDelayPref(OSPTAUTHRSP*, unsigned);

    unsigned OSPPAuthRspHasNumDests(OSPTAUTHRSP*);
    void OSPPAuthRspIncNumDests(OSPTAUTHRSP*);
    unsigned OSPPAuthRspGetNumDests(OSPTAUTHRSP*);

    OSPTTNAUDIT* OSPPAuthRspGetTNAudit(OSPTAUTHRSP*);

    int OSPPAuthRspHasCSAudit(OSPTAUTHRSP*);
    OSPTCSAUDIT* OSPPAuthRspGetCSAudit(OSPTAUTHRSP*);

    int OSPPAuthRspHasRole(OSPTAUTHRSP*);
    void OSPPAuthRspSetRole(OSPTAUTHRSP*, OSPE_MSG_ROLETYPES);
    OSPE_MSG_ROLETYPES OSPPAuthRspGetRole(OSPTAUTHRSP*);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHRSP_H */
