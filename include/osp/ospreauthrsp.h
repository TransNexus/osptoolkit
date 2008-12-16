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
 * ospreauthrsp.h - OSP reauthorisation response objects
 */

#ifndef _OSPREAUTHRSP_H
#define _OSPREAUTHRSP_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospdest.h"
#include "osp/ospstatus.h"
#include "osp/osptnaudit.h"

typedef struct {
    OSPTTIME ospmReauthRspTimestamp;
    char *ospmReauthRspMessageId;
    char *ospmReauthRspComponentId;
    OSPTSTATUS *ospmReauthRspStatus;
    OSPTTRXID ospmReauthRspTrxId;
    OSPTDEST *ospmReauthRspDest;
    OSPTTNAUDIT *ospmReauthRspTNAudit;
} OSPTREAUTHRSP;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTREAUTHRSP *OSPPReauthRspNew(void);
    void OSPPReauthRspDelete(OSPTREAUTHRSP **);
    int OSPPReauthRspFromElement(OSPT_XML_ELEM *, OSPTREAUTHRSP **);
    void OSPPReauthRspSetComponentId(OSPTREAUTHRSP *, const char *);
    OSPTBOOL OSPPReauthRspHasMessageId(OSPTREAUTHRSP *);
    void OSPPReauthRspSetMessageId(OSPTREAUTHRSP *, const char *);
    void OSPPReauthRspMessageIdFromElement(OSPT_XML_ELEM *, const char **);
    void OSPPReauthRspComponentIdFromElement(OSPT_XML_ELEM *, const char **);
    void OSPPReauthRspSetTimestamp(OSPTREAUTHRSP *, OSPTTIME);
    unsigned OSPPReauthRspHasComponentId(OSPTREAUTHRSP *);
    unsigned OSPPReauthRspHasStatus(OSPTREAUTHRSP *ospvReauthRsp);
    OSPTSTATUS *OSPPReauthRspGetStatus(OSPTREAUTHRSP *);
    void OSPPReauthRspSetTrxId(OSPTREAUTHRSP *, OSPTTRXID);
    unsigned OSPPReauthRspHasDest(OSPTREAUTHRSP *ospvReauthRsp);
    void OSPPReauthRspSetDest(OSPTREAUTHRSP *, OSPTDEST *);
    OSPTTNAUDIT *OSPPReauthRspGetTNAudit(OSPTREAUTHRSP *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPREAUTHRSP_H */
