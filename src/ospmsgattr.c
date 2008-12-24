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
 * ospmsgattr.c - OSP attribute processing.
 */

#include "osp/osp.h"
#include "osp/ospmsgdesc.h"
#include "osp/ospmsgattr.h"

/*
 * global variables
 */

/* Array that associates attribute and names */
const OSPT_MSG_DESC OSPV_MATTR_DESCS[OSPC_MATTR_NUMBER] = {
    { OSPC_MATTR_MESSAGEID,     "messageId" },
    { OSPC_MATTR_COMPONENTID,   "componentId" },
    { OSPC_MATTR_RANDOM,        "random" },
    { OSPC_MATTR_CRITICAL,      "critical" },
    { OSPC_MATTR_ENCODING,      "encoding" },
    { OSPC_MATTR_TYPE,          "type" },
    { OSPC_MATTR_VERSION,       "version" },
    { OSPC_MATTR_DIRECTION,     "direction" }
};

/* Array that associates role and names */
const OSPT_MSG_DESC OSPV_RTYPE_DESCS[OSPC_ROLE_NUMBER] = {
    { OSPC_ROLE_DESTINATION,   "destination" },
    { OSPC_ROLE_SOURCE,        "source" },
    { OSPC_ROLE_OTHER,         "other" },
    { OSPC_ROLE_RADSRCSTART,   "radsrcstart" },
    { OSPC_ROLE_RADDESTSTART,  "raddeststart" },
    { OSPC_ROLE_RADSRCSTOP,    "radsrcstop" },
    { OSPC_ROLE_RADDESTSTOP,   "raddeststop" }
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

