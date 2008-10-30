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
 *  ospauthind.h - OSP authorisation indication objects
 */

#ifndef _OSPAUTHIND_H
#define _OSPAUTHIND_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospaltinfo.h"
#include "osp/ospdest.h"

typedef struct {
    OSPTTIME ospmAuthIndTimestamp;
    OSPE_MSG_ROLETYPES ospmAuthIndRole;
    OSPTBOOL ospmAuthIndHasRole;
    OSPTCALLID *ospmAuthIndCallId;
    unsigned char ospmAuthIndSourceNumber[OSPC_E164NUMSIZE];
    unsigned char ospmAuthIndDestNumber[OSPC_E164NUMSIZE];
    OSPTLIST ospmAuthIndSourceAlternate;
    OSPTLIST ospmAuthIndDeviceInfo;
    OSPTLIST ospmAuthIndDestinationAlternate;
    OSPTLIST ospmAuthIndTokens;
    OSPTBOOL ospmAuthIndHasTimeLimit;
    unsigned ospmAuthIndTimeLimit;
    OSPTDEST *ospmAuthIndDest;
    OSPTBOOL ospmAuthIndHasDestNetworkIdInToken;
} OSPTAUTHIND;

#define OSPPAuthIndTokenGetSize(ospvToken) OSPPTokenGetSize(ospvToken);
#define OSPPAuthIndTokenGetValue(ospvToken) OSPPTokenGetValue(ospvToken);

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    void OSPPAuthIndDelete(OSPTAUTHIND **);
    OSPTAUTHIND *OSPPAuthIndNew(void);
    void OSPPAuthIndSetCallId(OSPTAUTHIND *, OSPTCALLID *);
    void OSPPAuthIndSetTimestamp(OSPTAUTHIND *, OSPTTIME);
    int OSPPAuthIndHasRole(OSPTAUTHIND *);
    void OSPPAuthIndSetRole(OSPTAUTHIND *, OSPE_MSG_ROLETYPES);
    OSPE_MSG_ROLETYPES OSPPAuthIndGetRole(OSPTAUTHIND *);
    int OSPPAuthIndHasCallId(OSPTAUTHIND *);
    OSPTCALLID *OSPPAuthIndGetCallId(OSPTAUTHIND *);
    unsigned OSPPAuthIndGetCallIdSize(OSPTAUTHIND *);
    unsigned char *OSPPAuthIndGetCallIdValue(OSPTAUTHIND *);
    int OSPPAuthIndHasSourceNumber(OSPTAUTHIND *ospvAuthInd);
    void OSPPAuthIndSetSourceNumber(OSPTAUTHIND *, const unsigned char *);
    unsigned char *OSPPAuthIndGetSourceNumber(OSPTAUTHIND *);
    int OSPPAuthIndHasDestNumber(OSPTAUTHIND *ospvAuthInd);
    void OSPPAuthIndSetDestNumber(OSPTAUTHIND *, const unsigned char *);
    unsigned char *OSPPAuthIndGetDestNumber(OSPTAUTHIND *);
    void OSPPAuthIndSetDest(OSPTAUTHIND *, OSPTDEST *);
    int OSPPAuthIndHasDest(OSPTAUTHIND *);
    int OSPPAuthIndHasSourceAlt(OSPTAUTHIND *);
    OSPTALTINFO *OSPPAuthIndFirstSourceAlt(OSPTAUTHIND *);
    OSPTALTINFO *OSPPAuthIndNextSourceAlt(OSPTAUTHIND *, OSPTALTINFO *);
    unsigned char *OSPPAuthIndGetSourceAltValue(OSPTALTINFO *);
    int OSPPAuthIndHasDestinationAlt(OSPTAUTHIND *);
    unsigned OSPPAuthIndGetDestinationAltSize(OSPTALTINFO *);
    OSPTALTINFO *OSPPAuthIndFirstDestinationAlt(OSPTAUTHIND *);
    OSPTALTINFO *OSPPAuthIndNextDestinationAlt(OSPTAUTHIND *, OSPTALTINFO *);
    unsigned char *OSPPAuthIndGetDestinationAltValue(OSPTALTINFO *);
    void OSPPAuthIndSetTimeLimit(OSPTAUTHIND *, unsigned);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHIND_H */
