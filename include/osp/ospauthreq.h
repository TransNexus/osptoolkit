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
 *  ospauthreq.h - OSP authorisation request objects
 */

#ifndef _OSPAUTHREQ_H
#define _OSPAUTHREQ_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospaltinfo.h"


typedef struct
{
    OSPTTIME          ospmAuthReqTimestamp;
    unsigned char    *ospmAuthReqMessageId;
    unsigned char    *ospmAuthReqComponentId;
    OSPTLIST          ospmAuthReqCallId;
    unsigned char     ospmAuthReqSourceNumber[OSPC_E164NUMSIZE];
    unsigned char     ospmAuthReqDestNumber[OSPC_E164NUMSIZE];
    OSPTLIST          ospmAuthReqSourceAlternate;
    OSPTLIST          ospmAuthReqDestinationAlternate;
    unsigned          ospmAuthReqMaxDest;
    unsigned long     ospmAuthReqTNCustId;
    unsigned long     ospmAuthReqTNDeviceId;
    OSPTLIST          ospmAuthReqDeviceInfo;
}
OSPTAUTHREQ;

#ifdef __cplusplus
extern "C" 
{
#endif
    /**/
    /*-----------------------------------------------------------------------*
     *  function prototypes
     *-----------------------------------------------------------------------*/

    OSPTAUTHREQ   *OSPPAuthReqNew(void);
    void           OSPPAuthReqDelete(OSPTAUTHREQ **);
    int            OSPPAuthReqToElement(OSPTAUTHREQ *, OSPTXMLELEM **, void *);
    unsigned       OSPPAuthReqHasMessageId(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasTimestamp(OSPTAUTHREQ *);
    void           OSPPAuthReqSetTimestamp(OSPTAUTHREQ *, OSPTTIME);
    OSPTTIME       OSPPAuthReqGetTimestamp(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasComponentId(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasCallId(OSPTAUTHREQ *);
    OSPTCALLID    *OSPPAuthReqFirstCallId(OSPTAUTHREQ *);
    OSPTCALLID    *OSPPAuthReqNextCallId(OSPTAUTHREQ *, OSPTCALLID *);

    unsigned       OSPPAuthReqHasSourceNumber(OSPTAUTHREQ *);
    void           OSPPAuthReqSetSourceNumber(OSPTAUTHREQ *, const unsigned char *);
    unsigned char *OSPPAuthReqGetSourceNumber(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasSourceAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqFirstSourceAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqNextSourceAlt(OSPTAUTHREQ *, OSPTALTINFO  *);

    unsigned       OSPPAuthReqHasDestNumber(OSPTAUTHREQ *);
    void           OSPPAuthReqSetDestNumber(OSPTAUTHREQ *, const unsigned char *);
    unsigned char *OSPPAuthReqGetDestNumber(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasDestinationAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqFirstDestinationAlt(OSPTAUTHREQ *);
    OSPTALTINFO   *OSPPAuthReqNextDestinationAlt(OSPTAUTHREQ *, OSPTALTINFO  *);

    void           OSPPAuthReqSetMaxDest(OSPTAUTHREQ *, unsigned);
    unsigned       OSPPAuthReqGetMaxDest(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasTNCustId(OSPTAUTHREQ *);
    void           OSPPAuthReqSetTNCustId(OSPTAUTHREQ *, unsigned long);
    unsigned long  OSPPAuthReqGetTNCustId(OSPTAUTHREQ *);

    unsigned       OSPPAuthReqHasTNDeviceId(OSPTAUTHREQ *ospvAuthReq);
    void           OSPPAuthReqSetTNDeviceId(OSPTAUTHREQ *, unsigned long);
    unsigned long  OSPPAuthReqGetTNDeviceId(OSPTAUTHREQ *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHREQ_H */