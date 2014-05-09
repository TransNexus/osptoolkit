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

/* Array that associates role state */
const OSPT_MSG_DESC OSPV_RSTATE_DESCS[OSPC_RSTATE_NUMBER] = {
    { OSPC_RSTATE_START,            "start" },
    { OSPC_RSTATE_START1,           "start1" },
    { OSPC_RSTATE_START2,           "start2" },
    { OSPC_RSTATE_STOP,             "stop" },
    { OSPC_RSTATE_STOP1,            "stop1" },
    { OSPC_RSTATE_STOP2,            "stop2" },
    { OSPC_RSTATE_INTERIM,          "interim" },
    { OSPC_RSTATE_INTERIM1,         "interim1" },
    { OSPC_RSTATE_INTERIM2,         "interim2" },
    { OSPC_RSTATE_INTERIMFAILED,    "interimfailed" },
    { OSPC_RSTATE_END1,             "end1" },
    { OSPC_RSTATE_END2,             "end2" },
    { OSPC_RSTATE_HUNT1,            "hunt1" },
    { OSPC_RSTATE_HUNT2,            "hunt2" },
    { OSPC_RSTATE_REDIRECT,         "redirect" }
};

/* Array that associates role state */
const OSPT_MSG_DESC OSPV_RFORMAT_DESCS[OSPC_RFORMAT_NUMBER] = {
    { OSPC_RFORMAT_OSP,         "osp" },
    { OSPC_RFORMAT_RADIUS,      "radius" },
    { OSPC_RFORMAT_DIAMETER,    "diameter" },
    { OSPC_RFORMAT_IPDR,        "ipdr" },
    { OSPC_RFORMAT_CSV,         "textcsv" },
    { OSPC_RFORMAT_TSV,         "texttsv" },
    { OSPC_RFORMAT_ASCII,       "ascii" },
    { OSPC_RFORMAT_XML,         "xml" }
};

/* Array that associates role vendor */
const OSPT_MSG_DESC OSPV_RVENDOR_DESCS[OSPC_RVENDOR_NUMBER] = {
    { OSPC_RVENDOR_ACME,        "acme" },
    { OSPC_RVENDOR_ASTERISK,    "asterisk" },
    { OSPC_RVENDOR_BROADWORKS,  "broadworks" },
    { OSPC_RVENDOR_CUCM,        "cucm" },
    { OSPC_RVENDOR_FREESWITCH,  "freeswitch" },
    { OSPC_RVENDOR_GENBANDS3,   "genbands3" },
    { OSPC_RVENDOR_KAMAILIO,    "kamailio" },
    { OSPC_RVENDOR_METASPHERE,  "metasphere" },
    { OSPC_RVENDOR_OPENSIPS,    "opensips" },
    { OSPC_RVENDOR_PERIMETA,    "perimeta" },
    { OSPC_RVENDOR_SANSAY,      "sansay" },
    { OSPC_RVENDOR_SONUS,       "sonus" },
    { OSPC_RVENDOR_TAQUA,       "taqua" }
};

/* Array that associates service type and names */
const OSPT_MSG_DESC OSPV_STYPE_DESCS[OSPC_SERVICE_NUMBER] = {
    { OSPC_SERVICE_VOICE,   "voice" },
    { OSPC_SERVICE_VIDEO,   "video" },
    { OSPC_SERVICE_DATA,    "data" },
    { OSPC_SERVICE_NPQUERY, "portedNumberQuery" }
};

/* Array that associates release source type and names */
const OSPT_MSG_DESC OSPV_RELEASE_DESCS[OSPC_RELEASE_NUMBER + 1] = {
    { OSPC_RELEASE_UNKNOWN,     "unknown" },
    { OSPC_RELEASE_SOURCE,      "source" },
    { OSPC_RELEASE_DESTINATION, "destination" },
    { OSPC_RELEASE_INTERNAL,    "internal" },
    { OSPC_RELEASE_EXTERNAL,    "external" }
};

/* Array that associates transfer status type and names */
const OSPT_MSG_DESC OSPV_TSTATUS_DESCS[OSPC_TSTATUS_NUMBER] = {
    { OSPC_TSTATUS_TRANSFER,            "transfer" },
    { OSPC_TSTATUS_TRANSFERFROM,        "transferfrom" },
    { OSPC_TSTATUS_TRANSFERTO,          "transferto" },
    { OSPC_TSTATUS_SRCTRANSFERFROM,     "sourcetransferfrom" },
    { OSPC_TSTATUS_SRCTRANSFERTO,       "sourcetransferto" },
    { OSPC_TSTATUS_DESTTRANSFERFROM,    "destinationtransferfrom" },
    { OSPC_TSTATUS_DESTTRANSFERTO,      "destinationtransferto" }
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

