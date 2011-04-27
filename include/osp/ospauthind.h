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

/* ospauthind.h - OSP authorization indication objects */
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
    OSPTTIME Timestamp;
    OSPE_ROLE Role;
    OSPTBOOL HasRole;
    OSPT_CALL_ID *CallId;
    char SourceNumber[OSPC_SIZE_E164NUM];
    char DestinationNumber[OSPC_SIZE_E164NUM];
    OSPTLIST SourceAlternate;
    OSPTLIST DeviceInfo;
    OSPTLIST DestinationAlternate;
    OSPTLIST Tokens;
    OSPTBOOL HasTimeLimit;
    unsigned TimeLimit;
    OSPT_DEST *Destination;
    OSPTBOOL HasDestNetworkIdInToken;
} OSPT_AUTH_IND;

#define OSPPAuthIndTokenGetSize(ospvToken) OSPPTokenGetSize(ospvToken);
#define OSPPAuthIndTokenGetValue(ospvToken) OSPPTokenGetValue(ospvToken);

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    void OSPPAuthIndDelete(OSPT_AUTH_IND **);
    OSPT_AUTH_IND *OSPPAuthIndNew(void);
    void OSPPAuthIndSetCallId(OSPT_AUTH_IND *, OSPT_CALL_ID *);
    void OSPPAuthIndSetTimestamp(OSPT_AUTH_IND *, OSPTTIME);
    OSPTBOOL OSPPAuthIndHasRole(OSPT_AUTH_IND *);
    void OSPPAuthIndSetRole(OSPT_AUTH_IND *, OSPE_ROLE);
    OSPE_ROLE OSPPAuthIndGetRole(OSPT_AUTH_IND *);
    OSPTBOOL OSPPAuthIndHasCallId(OSPT_AUTH_IND *);
    OSPT_CALL_ID *OSPPAuthIndGetCallId(OSPT_AUTH_IND *);
    unsigned OSPPAuthIndGetCallIdSize(OSPT_AUTH_IND *);
    unsigned char *OSPPAuthIndGetCallIdValue(OSPT_AUTH_IND *);
    OSPTBOOL OSPPAuthIndHasSourceNumber(OSPT_AUTH_IND *ospvAuthInd);
    void OSPPAuthIndSetSourceNumber(OSPT_AUTH_IND *, const char *);
    const char *OSPPAuthIndGetSourceNumber(OSPT_AUTH_IND *);
    OSPTBOOL OSPPAuthIndHasDestNumber(OSPT_AUTH_IND *ospvAuthInd);
    void OSPPAuthIndSetDestNumber(OSPT_AUTH_IND *, const char *);
    const char *OSPPAuthIndGetDestNumber(OSPT_AUTH_IND *);
    void OSPPAuthIndSetDest(OSPT_AUTH_IND *, OSPT_DEST *);
    OSPTBOOL OSPPAuthIndHasDest(OSPT_AUTH_IND *);
    OSPTBOOL OSPPAuthIndHasSourceAlt(OSPT_AUTH_IND *);
    OSPT_ALTINFO *OSPPAuthIndFirstSourceAlt(OSPT_AUTH_IND *);
    OSPT_ALTINFO *OSPPAuthIndNextSourceAlt(OSPT_AUTH_IND *, OSPT_ALTINFO *);
    const char *OSPPAuthIndGetSourceAltValue(OSPT_ALTINFO *);
    OSPTBOOL OSPPAuthIndHasDestinationAlt(OSPT_AUTH_IND *);
    unsigned OSPPAuthIndGetDestinationAltSize(OSPT_ALTINFO *);
    OSPT_ALTINFO *OSPPAuthIndFirstDestinationAlt(OSPT_AUTH_IND *);
    OSPT_ALTINFO *OSPPAuthIndNextDestinationAlt(OSPT_AUTH_IND *, OSPT_ALTINFO *);
    const char *OSPPAuthIndGetDestinationAltValue(OSPT_ALTINFO *);
    void OSPPAuthIndSetTimeLimit(OSPT_AUTH_IND *, unsigned);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHIND_H */
