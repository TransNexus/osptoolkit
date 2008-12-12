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

typedef struct {
    OSPTTIME ospmAuthReqTimestamp;
    unsigned char *ospmAuthReqMessageId;
    unsigned char *ospmAuthReqComponentId;
    OSPTLIST ospmAuthReqCallId;
    char ospmAuthReqSourceNumber[OSPC_SIZE_E164NUM];
    char ospmAuthReqDestNumber[OSPC_SIZE_E164NUM];
    OSPTLIST ospmAuthReqSourceAlternate;
    OSPTLIST ospmAuthReqDestinationAlternate;
    unsigned ospmAuthReqMaxDest;
    unsigned long ospmAuthReqCustId;
    unsigned long ospmAuthReqDeviceId;
    OSPTLIST ospmAuthReqDeviceInfo;
} OSPTAUTHREQ;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTAUTHREQ *OSPPAuthReqNew(void);
    void OSPPAuthReqDelete(OSPTAUTHREQ **);
    int OSPPAuthReqToElement(OSPTAUTHREQ *, OSPT_XML_ELEM **, void *);
    unsigned OSPPAuthReqHasMessageId(OSPTAUTHREQ *);
    unsigned OSPPAuthReqHasTimestamp(OSPTAUTHREQ *);
    void OSPPAuthReqSetTimestamp(OSPTAUTHREQ *, OSPTTIME);
    OSPTTIME OSPPAuthReqGetTimestamp(OSPTAUTHREQ *);
    unsigned OSPPAuthReqHasComponentId(OSPTAUTHREQ *);
    unsigned OSPPAuthReqHasCallId(OSPTAUTHREQ *);
    OSPTCALLID *OSPPAuthReqFirstCallId(OSPTAUTHREQ *);
    OSPTCALLID *OSPPAuthReqNextCallId(OSPTAUTHREQ *, OSPTCALLID *);
    unsigned OSPPAuthReqHasSourceNumber(OSPTAUTHREQ *);
    void OSPPAuthReqSetSourceNumber(OSPTAUTHREQ *, const char *);
    char *OSPPAuthReqGetSourceNumber(OSPTAUTHREQ *);
    unsigned OSPPAuthReqHasSourceAlt(OSPTAUTHREQ *);
    OSPT_ALTINFO *OSPPAuthReqFirstSourceAlt(OSPTAUTHREQ *);
    OSPT_ALTINFO *OSPPAuthReqNextSourceAlt(OSPTAUTHREQ *, OSPT_ALTINFO *);
    unsigned OSPPAuthReqHasDestNumber(OSPTAUTHREQ *);
    void OSPPAuthReqSetDestNumber(OSPTAUTHREQ *, const char *);
    char *OSPPAuthReqGetDestNumber(OSPTAUTHREQ *);
    unsigned OSPPAuthReqHasDestinationAlt(OSPTAUTHREQ *);
    OSPT_ALTINFO *OSPPAuthReqFirstDestinationAlt(OSPTAUTHREQ *);
    OSPT_ALTINFO *OSPPAuthReqNextDestinationAlt(OSPTAUTHREQ *, OSPT_ALTINFO *);
    void OSPPAuthReqSetMaxDest(OSPTAUTHREQ *, unsigned);
    unsigned OSPPAuthReqGetMaxDest(OSPTAUTHREQ *);
    OSPTBOOL OSPPAuthReqHasCustId(OSPTAUTHREQ *);
    void OSPPAuthReqSetCustId(OSPTAUTHREQ *, unsigned long);
    unsigned long OSPPAuthReqGetCustId(OSPTAUTHREQ *);
    OSPTBOOL OSPPAuthReqHasDeviceId(OSPTAUTHREQ *ospvAuthReq);
    void OSPPAuthReqSetDeviceId(OSPTAUTHREQ *, unsigned long);
    unsigned long OSPPAuthReqGetDeviceId(OSPTAUTHREQ *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHREQ_H */
