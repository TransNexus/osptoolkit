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
 * ospreauthreq.h - OSP reauthorisation request objects
 */

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
    OSPTTIME ospmReauthReqTimestamp;
    unsigned char *ospmReauthReqMessageId;
    unsigned char *ospmReauthReqComponentId;
    unsigned ospmReauthReqRole;
    unsigned ospmReauthReqHasRole;
    OSPTCALLID *ospmReauthReqCallId;
    unsigned char ospmReauthReqSourceNumber[OSPC_SIZE_E164NUM];
    unsigned char ospmReauthReqDestNumber[OSPC_SIZE_E164NUM];
    OSPTLIST ospmReauthReqSourceAlternate;
    OSPTLIST ospmReauthReqDestinationAlternate;
    OSPTTRXID ospmReauthReqTrxId;
    int ospmReauthReqDuration;
    OSPTLIST ospmReauthReqTokens;
    unsigned long ospmReauthReqCustId;
    unsigned long ospmReauthReqDeviceId;
    OSPTLIST ospmReauthReqDevInfo;
} OSPTREAUTHREQ;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTREAUTHREQ *OSPPReauthReqNew(void);
    void OSPPReauthReqDelete(OSPTREAUTHREQ **);
    int OSPPReauthReqToElement(OSPTREAUTHREQ *, OSPT_XML_ELEM **, void *);
    void OSPPReauthReqSetRole(OSPTREAUTHREQ *, OSPE_MSG_ROLE);
    unsigned OSPPReauthReqHasRole(OSPTREAUTHREQ *);
    OSPE_MSG_ROLE OSPPReauthReqGetRole(OSPTREAUTHREQ *);
    void OSPPReauthReqAddSourceAlt(OSPTREAUTHREQ *, OSPT_ALTINFO *);
    void OSPPReauthReqAddDestinationAlt(OSPTREAUTHREQ *, OSPT_ALTINFO *);
    void OSPPReauthReqSetCallId(OSPTREAUTHREQ *, OSPTCALLID *);
    unsigned OSPPReauthReqHasMessageId(OSPTREAUTHREQ *);
    unsigned char *OSPPReauthReqGetMessageId(OSPTREAUTHREQ *);
    unsigned OSPPReauthReqHasTimestamp(OSPTREAUTHREQ *);
    void OSPPReauthReqSetTimestamp(OSPTREAUTHREQ *, OSPTTIME);
    OSPTTIME OSPPReauthReqGetTimestamp(OSPTREAUTHREQ *);
    unsigned OSPPReauthReqHasComponentId(OSPTREAUTHREQ *);
    unsigned char *OSPPReauthReqGetComponentId(OSPTREAUTHREQ *);
    unsigned OSPPReauthReqHasCallId(OSPTREAUTHREQ *);
    OSPTCALLID *OSPPReauthReqGetCallId(OSPTREAUTHREQ *);
    void OSPPReauthReqSetSourceNumber(OSPTREAUTHREQ *, const char *);
    const unsigned char *OSPPReauthReqGetSourceNumber(OSPTREAUTHREQ *);
    void OSPPReauthReqSetDestNumber(OSPTREAUTHREQ *, const char *);
    const unsigned char *OSPPReauthReqGetDestNumber(OSPTREAUTHREQ *);
    unsigned OSPPReauthReqHasTrxId(OSPTREAUTHREQ *);
    void OSPPReauthReqSetTrxId(OSPTREAUTHREQ *, OSPTTRXID);
    OSPTTRXID OSPPReauthReqGetTrxId(OSPTREAUTHREQ *);
    unsigned OSPPReauthReqHasDuration(OSPTREAUTHREQ *);
    void OSPPReauthReqSetDuration(OSPTREAUTHREQ *, int ospvDuration);
    int OSPPReauthReqGetDuration(OSPTREAUTHREQ *);
    void OSPPReauthReqAddToken(OSPTREAUTHREQ *, OSPTTOKEN *);
    OSPTTOKEN *OSPPReauthReqFirstToken(OSPTREAUTHREQ *);
    OSPTTOKEN *OSPPReauthReqNextToken(OSPTREAUTHREQ *, OSPTTOKEN *);
    unsigned OSPPReauthReqHasDestinationAlt(OSPTREAUTHREQ *);
    OSPT_ALTINFO *OSPPReauthReqFirstDestinationAlt(OSPTREAUTHREQ *);
    OSPT_ALTINFO *OSPPReauthReqNextDestinationAlt(OSPTREAUTHREQ *, OSPT_ALTINFO *);
    unsigned char *OSPPReauthReqGetDestinationAltValue(OSPT_ALTINFO *);
    unsigned OSPPReauthReqHasSourceAlt(OSPTREAUTHREQ *);
    OSPT_ALTINFO *OSPPReauthReqFirstSourceAlt(OSPTREAUTHREQ *);
    OSPT_ALTINFO *OSPPReauthReqNextSourceAlt(OSPTREAUTHREQ *, OSPT_ALTINFO *);
    OSPTBOOL OSPPReauthReqHasCustId(OSPTREAUTHREQ *);
    void OSPPReauthReqSetCustId(OSPTREAUTHREQ *, unsigned long);
    unsigned long OSPPReauthReqGetCustId(OSPTREAUTHREQ *);
    OSPTBOOL OSPPReauthReqHasDeviceId(OSPTREAUTHREQ *);
    void OSPPReauthReqSetDeviceId(OSPTREAUTHREQ *, unsigned long);
    unsigned long OSPPReauthReqGetDeviceId(OSPTREAUTHREQ *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPREAUTHREQ_H */
