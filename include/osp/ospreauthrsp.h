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

/* ospreauthrsp.h - OSP reauthorization response objects */
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
    OSPTTIME Timestamp;
    char *MessageId;
    char *ComponentId;
    OSPT_STATUS *Status;
    OSPTTRXID TrxId;
    OSPT_DEST *Destination;
    OSPT_TN_AUDIT *TNAudit;
} OSPT_REAUTH_RSP;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_REAUTH_RSP *OSPPReauthRspNew(void);
    void OSPPReauthRspDelete(OSPT_REAUTH_RSP **);
    int OSPPReauthRspFromElement(OSPT_XML_ELEM *, OSPT_REAUTH_RSP **);
    void OSPPReauthRspSetComponentId(OSPT_REAUTH_RSP *, const char *);
    OSPTBOOL OSPPReauthRspHasMessageId(OSPT_REAUTH_RSP *);
    void OSPPReauthRspSetMessageId(OSPT_REAUTH_RSP *, const char *);
    void OSPPReauthRspMessageIdFromElement(OSPT_XML_ELEM *, const char **);
    void OSPPReauthRspComponentIdFromElement(OSPT_XML_ELEM *, const char **);
    void OSPPReauthRspSetTimestamp(OSPT_REAUTH_RSP *, OSPTTIME);
    OSPTBOOL OSPPReauthRspHasComponentId(OSPT_REAUTH_RSP *);
    OSPTBOOL OSPPReauthRspHasStatus(OSPT_REAUTH_RSP *ospvReauthRsp);
    OSPT_STATUS *OSPPReauthRspGetStatus(OSPT_REAUTH_RSP *);
    void OSPPReauthRspSetTrxId(OSPT_REAUTH_RSP *, OSPTTRXID);
    OSPTBOOL OSPPReauthRspHasDest(OSPT_REAUTH_RSP *ospvReauthRsp);
    void OSPPReauthRspSetDest(OSPT_REAUTH_RSP *, OSPT_DEST *);
    OSPT_TN_AUDIT *OSPPReauthRspGetTNAudit(OSPT_REAUTH_RSP *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPREAUTHRSP_H */
