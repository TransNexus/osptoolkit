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

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"
#include "ospdest.h"
#include "ospstatus.h"
#include "osptnaudit.h"

typedef struct
{
    OSPTTIME          ospmReauthRspTimestamp;
    unsigned char    *ospmReauthRspMessageId;
    unsigned char    *ospmReauthRspComponentId;
    OSPTSTATUS       *ospmReauthRspStatus;
    OSPTTRXID         ospmReauthRspTrxId;
    OSPTDEST         *ospmReauthRspDest;
    OSPTTNAUDIT      *ospmReauthRspTNAudit;
}
OSPTREAUTHRSP;


/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

    OSPTREAUTHRSP   *OSPPReauthRspNew(void);
    void            OSPPReauthRspDelete(OSPTREAUTHRSP **);
    int             OSPPReauthRspFromElement(OSPTXMLELEM *, OSPTREAUTHRSP **);
    void            OSPPReauthRspSetComponentId(OSPTREAUTHRSP *, unsigned char *);
    unsigned        OSPPReauthRspHasMessageId(OSPTREAUTHRSP *);
    void            OSPPReauthRspSetMessageId(OSPTREAUTHRSP *, unsigned char *);
    void            OSPPReauthRspMessageIdFromElement(OSPTXMLELEM *, unsigned char **);
    void            OSPPReauthRspComponentIdFromElement(OSPTXMLELEM *, unsigned char **);


    void            OSPPReauthRspSetTimestamp(OSPTREAUTHRSP *, OSPTTIME);
    unsigned        OSPPReauthRspHasComponentId(OSPTREAUTHRSP *);

    unsigned        OSPPReauthRspHasStatus(OSPTREAUTHRSP *ospvReauthRsp);
    OSPTSTATUS      *OSPPReauthRspGetStatus(OSPTREAUTHRSP *);
    void            OSPPReauthRspSetTrxId(OSPTREAUTHRSP *, OSPTTRXID);
    unsigned        OSPPReauthRspHasDest(OSPTREAUTHRSP *ospvReauthRsp);
    void            OSPPReauthRspSetDest(OSPTREAUTHRSP *, OSPTDEST *);

    OSPTTNAUDIT     *OSPPReauthRspGetTNAudit(OSPTREAUTHRSP *);

#ifdef __cplusplus
}
#endif

#endif

