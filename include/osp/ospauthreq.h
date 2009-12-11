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

/* ospauthreq.h - OSP authorisation request objects */
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
    char *ospmAuthReqMessageId;
    char *ospmAuthReqComponentId;
    OSPTLIST ospmAuthReqCallId;
    char ospmAuthReqSourceNumber[OSPC_SIZE_E164NUM];
    char ospmAuthReqDestNumber[OSPC_SIZE_E164NUM];
    OSPTLIST ospmAuthReqSourceAlternate;
    OSPTLIST ospmAuthReqDestinationAlternate;
    unsigned ospmAuthReqMaxDest;
    unsigned long ospmAuthReqCustId;
    unsigned long ospmAuthReqDeviceId;
    OSPTLIST ospmAuthReqDeviceInfo;
} OSPT_AUTH_REQ;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_AUTH_REQ *OSPPAuthReqNew(void);
    void OSPPAuthReqDelete(OSPT_AUTH_REQ **);
    int OSPPAuthReqToElement(OSPT_AUTH_REQ *, OSPT_XML_ELEM **, void *);
    OSPTBOOL OSPPAuthReqHasMessageId(OSPT_AUTH_REQ *);
    OSPTBOOL OSPPAuthReqHasTimestamp(OSPT_AUTH_REQ *);
    void OSPPAuthReqSetTimestamp(OSPT_AUTH_REQ *, OSPTTIME);
    OSPTTIME OSPPAuthReqGetTimestamp(OSPT_AUTH_REQ *);
    OSPTBOOL OSPPAuthReqHasComponentId(OSPT_AUTH_REQ *);
    OSPTBOOL OSPPAuthReqHasCallId(OSPT_AUTH_REQ *);
    OSPT_CALL_ID *OSPPAuthReqFirstCallId(OSPT_AUTH_REQ *);
    OSPT_CALL_ID *OSPPAuthReqNextCallId(OSPT_AUTH_REQ *, OSPT_CALL_ID *);
    OSPTBOOL OSPPAuthReqHasSourceNumber(OSPT_AUTH_REQ *);
    void OSPPAuthReqSetSourceNumber(OSPT_AUTH_REQ *, const char *);
    const char *OSPPAuthReqGetSourceNumber(OSPT_AUTH_REQ *);
    OSPTBOOL OSPPAuthReqHasSourceAlt(OSPT_AUTH_REQ *);
    OSPT_ALTINFO *OSPPAuthReqFirstSourceAlt(OSPT_AUTH_REQ *);
    OSPT_ALTINFO *OSPPAuthReqNextSourceAlt(OSPT_AUTH_REQ *, OSPT_ALTINFO *);
    OSPTBOOL OSPPAuthReqHasDestNumber(OSPT_AUTH_REQ *);
    void OSPPAuthReqSetDestNumber(OSPT_AUTH_REQ *, const char *);
    const char *OSPPAuthReqGetDestNumber(OSPT_AUTH_REQ *);
    OSPTBOOL OSPPAuthReqHasDestinationAlt(OSPT_AUTH_REQ *);
    OSPT_ALTINFO *OSPPAuthReqFirstDestinationAlt(OSPT_AUTH_REQ *);
    OSPT_ALTINFO *OSPPAuthReqNextDestinationAlt(OSPT_AUTH_REQ *, OSPT_ALTINFO *);
    void OSPPAuthReqSetMaxDest(OSPT_AUTH_REQ *, unsigned);
    unsigned OSPPAuthReqGetMaxDest(OSPT_AUTH_REQ *);
    OSPTBOOL OSPPAuthReqHasCustId(OSPT_AUTH_REQ *);
    void OSPPAuthReqSetCustId(OSPT_AUTH_REQ *, unsigned long);
    unsigned long OSPPAuthReqGetCustId(OSPT_AUTH_REQ *);
    OSPTBOOL OSPPAuthReqHasDeviceId(OSPT_AUTH_REQ *ospvAuthReq);
    void OSPPAuthReqSetDeviceId(OSPT_AUTH_REQ *, unsigned long);
    unsigned long OSPPAuthReqGetDeviceId(OSPT_AUTH_REQ *);
    unsigned OSPPNPRnToElement(const char *, OSPT_XML_ELEM **);
    unsigned OSPPNPCicToElement(const char *, OSPT_XML_ELEM **);
    unsigned OSPPNPNpdiToElement(int, OSPT_XML_ELEM **);
    unsigned OSPPOperatorNameToElement(OSPE_OPERATOR_NAME, const char *, OSPT_XML_ELEM **);
    unsigned OSPPServiceTypeToElement(OSPE_SERVICE, OSPT_XML_ELEM **);
    unsigned OSPPPricingInfoToElement(OSPT_PRICING_INFO, OSPT_XML_ELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHREQ_H */
