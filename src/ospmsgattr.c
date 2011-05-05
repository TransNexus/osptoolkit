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

/* ospmsgattr.c - OSP attribute processing. */

#include "osp/osp.h"
#include "osp/ospmsgdesc.h"
#include "osp/ospmsgattr.h"

/* global variables */

/* Array that associates attribute and names */
const OSPT_MSG_DESC OSPV_MATTR_DESCS[OSPC_MATTR_NUMBER] = {
    { OSPC_MATTR_MESSAGEID,     "messageId" },
    { OSPC_MATTR_COMPONENTID,   "componentId" },
    { OSPC_MATTR_RANDOM,        "random" },
    { OSPC_MATTR_CRITICAL,      "critical" },
    { OSPC_MATTR_ENCODING,      "encoding" },
    { OSPC_MATTR_TYPE,          "type" },
    { OSPC_MATTR_DIR,           "direction" },
    { OSPC_MATTR_VERSION,       "version" },
    { OSPC_MATTR_INDEX,         "index" },
    { OSPC_MATTR_REPORTER,      "reporter" }
};

/* Array that associates role names */
const OSPT_MSG_DESC OSPV_RTYPE_DESCS[OSPC_ROLE_NUMBER] = {
    { OSPC_ROLE_SOURCE,        "source" },
    { OSPC_ROLE_DESTINATION,   "destination" },
    { OSPC_ROLE_OTHER,         "other" },
};

/* Array that associates role info */
const OSPT_MSG_DESC OSPV_RINFO_DESCS[OSPC_RINFO_NUMBER] = {
    { OSPC_RINFO_ACMERADSTART,          "acmeradstart" },
    { OSPC_RINFO_ACMERADSTOP,           "acmeradstop" },
    { OSPC_RINFO_ACMERADINTERIM,        "acmeradinterim" },
    { OSPC_RINFO_ACMECDRSTART,          "acmecdrstart" },
    { OSPC_RINFO_ACMECDRSTOP,           "acmecdrstop" },
    { OSPC_RINFO_ACMECDRINTERIM,        "acmecdrimterim" },
    { OSPC_RINFO_ACMECDRINTERIMFAILED,  "acmecdrinterimfailed" },
    { OSPC_RINFO_GENBANDS3RADSTART1,    "genbands3radstart1" },
    { OSPC_RINFO_GENBANDS3RADSTART2,    "genbands3radstart2" },
    { OSPC_RINFO_GENBANDS3RADSTOP1,     "genbands3radstop1" },
    { OSPC_RINFO_GENBANDS3RADSTOP2,     "genbands3radstop2" },
    { OSPC_RINFO_GENBANDS3RADINTERIM1,  "genbands3radinterim1" },
    { OSPC_RINFO_GENBANDS3RADINTERIM2,  "genbands3radinterim2" },
    { OSPC_RINFO_GENBANDS3CDRSTART1,    "genbands3cdrstart1" },
    { OSPC_RINFO_GENBANDS3CDRSTART2,    "genbands3cdrstart2" },
    { OSPC_RINFO_GENBANDS3CDREND1,      "genbands3cdrend1" },
    { OSPC_RINFO_GENBANDS3CDREND2,      "genbands3cdrend2" },
    { OSPC_RINFO_GENBANDS3CDRHUNT1,     "genbands3cdrhunt1" },
    { OSPC_RINFO_GENBANDS3CDRHUNT2,     "genbands3cdrhunt2" },
    { OSPC_RINFO_GENBANDS3CDRINTERIM,   "genbands3cdrinterim" }
};

/* Array that associates service type and names */
const OSPT_MSG_DESC OSPV_STYPE_DESCS[OSPC_SERVICE_NUMBER] = {
    { OSPC_SERVICE_VOICE,   "voice" },
    { OSPC_SERVICE_VIDEO,   "video" },
    { OSPC_SERVICE_DATA,    "data" },
    { OSPC_SERVICE_NPQUERY, "portedNumberQuery" }
};

/*
 * OSPPMsgAttrGetName() - get an attribute name from a part value
 */
const char *OSPPMsgAttrGetName(     /* returns pointer to the name */
    OSPE_MSG_ATTR ospvPart)
{
    const char *ospvName = OSPC_OSNULL;

    if ((ospvPart >= OSPC_MATTR_START) && (ospvPart < OSPC_MATTR_NUMBER)) {
        ospvName = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_MATTR_DESCS, OSPC_MATTR_NUMBER);
    }

    return ospvName;
}

/*
 * OSPPMsgAttrGetPart() - get an attribute part ID from its name
 */
OSPE_MSG_ATTR OSPPMsgAttrGetPart(   /* returns part */
    const char *ospvName)
{
    OSPE_MSG_ATTR ospvPart = OSPC_MATTR_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        ospvPart = (OSPE_MSG_ATTR)OSPPMsgDescGetPart(ospvName, OSPV_MATTR_DESCS, OSPC_MATTR_NUMBER);
    }

    return ospvPart;
}

/*
 * OSPPServiceGetPart() - get an service type from a part name
 */
OSPE_SERVICE OSPPServiceGetPart(
    const char *ospvName)
{
    OSPE_MSG_ATTR part = OSPC_SERVICE_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        part = (OSPE_SERVICE)OSPPMsgDescGetPart(ospvName, OSPV_STYPE_DESCS, OSPC_SERVICE_NUMBER);
    }

    return part;
}

/*
 * OSPPServiceGetName() - get an service name from a part value
 */
const char *OSPPServiceGetName(     /* returns pointer to the name */
    OSPE_SERVICE ospvType)
{
    const char *name = OSPC_OSNULL;

    if ((ospvType >= OSPC_SERVICE_START) && (ospvType < OSPC_SERVICE_NUMBER)) {
        name = OSPPMsgDescGetName((OSPT_MSG_PART)ospvType, OSPV_STYPE_DESCS, OSPC_SERVICE_NUMBER);
    }

    return name;
}

