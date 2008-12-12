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
    OSPE_MSG_ROLE ospmAuthIndRole;
    OSPTBOOL ospmAuthIndHasRole;
    OSPTCALLID *ospmAuthIndCallId;
    char ospmAuthIndSourceNumber[OSPC_SIZE_E164NUM];
    char ospmAuthIndDestNumber[OSPC_SIZE_E164NUM];
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
    void OSPPAuthIndSetRole(OSPTAUTHIND *, OSPE_MSG_ROLE);
    OSPE_MSG_ROLE OSPPAuthIndGetRole(OSPTAUTHIND *);
    int OSPPAuthIndHasCallId(OSPTAUTHIND *);
    OSPTCALLID *OSPPAuthIndGetCallId(OSPTAUTHIND *);
    unsigned OSPPAuthIndGetCallIdSize(OSPTAUTHIND *);
    unsigned char *OSPPAuthIndGetCallIdValue(OSPTAUTHIND *);
    int OSPPAuthIndHasSourceNumber(OSPTAUTHIND *ospvAuthInd);
    void OSPPAuthIndSetSourceNumber(OSPTAUTHIND *, const char *);
    char *OSPPAuthIndGetSourceNumber(OSPTAUTHIND *);
    int OSPPAuthIndHasDestNumber(OSPTAUTHIND *ospvAuthInd);
    void OSPPAuthIndSetDestNumber(OSPTAUTHIND *, const char *);
    char *OSPPAuthIndGetDestNumber(OSPTAUTHIND *);
    void OSPPAuthIndSetDest(OSPTAUTHIND *, OSPTDEST *);
    int OSPPAuthIndHasDest(OSPTAUTHIND *);
    int OSPPAuthIndHasSourceAlt(OSPTAUTHIND *);
    OSPT_ALTINFO *OSPPAuthIndFirstSourceAlt(OSPTAUTHIND *);
    OSPT_ALTINFO *OSPPAuthIndNextSourceAlt(OSPTAUTHIND *, OSPT_ALTINFO *);
    unsigned char *OSPPAuthIndGetSourceAltValue(OSPT_ALTINFO *);
    int OSPPAuthIndHasDestinationAlt(OSPTAUTHIND *);
    unsigned OSPPAuthIndGetDestinationAltSize(OSPT_ALTINFO *);
    OSPT_ALTINFO *OSPPAuthIndFirstDestinationAlt(OSPTAUTHIND *);
    OSPT_ALTINFO *OSPPAuthIndNextDestinationAlt(OSPTAUTHIND *, OSPT_ALTINFO *);
    unsigned char *OSPPAuthIndGetDestinationAltValue(OSPT_ALTINFO *);
    void OSPPAuthIndSetTimeLimit(OSPTAUTHIND *, unsigned);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHIND_H */
