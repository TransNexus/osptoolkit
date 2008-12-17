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
#include "osp/ospmsgattr.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospaltinfo.h"
#include "osp/ospdest.h"

typedef struct {
    OSPTTIME ospmAuthIndTimestamp;
    OSPE_ROLE_TYPE ospmAuthIndRole;
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
    OSPTBOOL OSPPAuthIndHasRole(OSPTAUTHIND *);
    void OSPPAuthIndSetRole(OSPTAUTHIND *, OSPE_ROLE_TYPE);
    OSPE_ROLE_TYPE OSPPAuthIndGetRole(OSPTAUTHIND *);
    OSPTBOOL OSPPAuthIndHasCallId(OSPTAUTHIND *);
    OSPTCALLID *OSPPAuthIndGetCallId(OSPTAUTHIND *);
    unsigned OSPPAuthIndGetCallIdSize(OSPTAUTHIND *);
    unsigned char *OSPPAuthIndGetCallIdValue(OSPTAUTHIND *);
    OSPTBOOL OSPPAuthIndHasSourceNumber(OSPTAUTHIND *ospvAuthInd);
    void OSPPAuthIndSetSourceNumber(OSPTAUTHIND *, const char *);
    const char *OSPPAuthIndGetSourceNumber(OSPTAUTHIND *);
    OSPTBOOL OSPPAuthIndHasDestNumber(OSPTAUTHIND *ospvAuthInd);
    void OSPPAuthIndSetDestNumber(OSPTAUTHIND *, const char *);
    const char *OSPPAuthIndGetDestNumber(OSPTAUTHIND *);
    void OSPPAuthIndSetDest(OSPTAUTHIND *, OSPTDEST *);
    OSPTBOOL OSPPAuthIndHasDest(OSPTAUTHIND *);
    OSPTBOOL OSPPAuthIndHasSourceAlt(OSPTAUTHIND *);
    OSPT_ALTINFO *OSPPAuthIndFirstSourceAlt(OSPTAUTHIND *);
    OSPT_ALTINFO *OSPPAuthIndNextSourceAlt(OSPTAUTHIND *, OSPT_ALTINFO *);
    const char *OSPPAuthIndGetSourceAltValue(OSPT_ALTINFO *);
    int OSPPAuthIndHasDestinationAlt(OSPTAUTHIND *);
    unsigned OSPPAuthIndGetDestinationAltSize(OSPT_ALTINFO *);
    OSPT_ALTINFO *OSPPAuthIndFirstDestinationAlt(OSPTAUTHIND *);
    OSPT_ALTINFO *OSPPAuthIndNextDestinationAlt(OSPTAUTHIND *, OSPT_ALTINFO *);
    const char *OSPPAuthIndGetDestinationAltValue(OSPT_ALTINFO *);
    void OSPPAuthIndSetTimeLimit(OSPTAUTHIND *, unsigned);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHIND_H */
