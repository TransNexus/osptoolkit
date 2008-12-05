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
const OSPT_MSG_DESC OSPC_MATTR_DESCS[] = {
    { OSPC_MATTR_UNKNOWN,       "" },
    { OSPC_MATTR_COMPONENTID,   "componentId" },
    { OSPC_MATTR_CRITICAL,      "critical" },
    { OSPC_MATTR_ENCODING,      "encoding" },
    { OSPC_MATTR_MESSAGEID,     "messageId" },
    { OSPC_MATTR_TYPE,          "type" },
    { OSPC_MATTR_VERSION,       "version" },
    { OSPC_MATTR_RANDOM,        "random" }
};

/*
 * OSPPMsgAttrGetName() - get an attribute name from a part value
 */
const char *OSPPMsgAttrGetName(     /* returns pointer to the name */                  
    OSPE_MSG_ATTR ospvPart)
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvPart != OSPC_MATTR_UNKNOWN) {
        ospvName = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPC_MATTR_DESCS, OSPC_MATTR_NUMBER);
    }

    return (ospvName);
}

/*
 * OSPPMsgAttrGetPart() - get an attribute part ID from its name
 */
OSPE_MSG_ATTR OSPPMsgAttrGetPart(   /* returns part */
    const char *ospvName)
{
    OSPE_MSG_ATTR ospvPart = OSPC_MATTR_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        ospvPart = (OSPE_MSG_ATTR)OSPPMsgDescGetPart(ospvName, OSPC_MATTR_DESCS, OSPC_MATTR_NUMBER);
    }

    return (ospvPart);
}
