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

/**/
/*-----------------------------------------------------------------------*
 * global variables
 *-----------------------------------------------------------------------*/

/* array that associates attribute and names */

const OSPTMSGDESC OSPVAttrDescs[] =
{
    { ospeAttrUnknown,       "" },
    { ospeAttrComponentId,   "componentId" },
    { ospeAttrCritical,      "critical" },
    { ospeAttrEncoding,      "encoding" },
    { ospeAttrMessageId,     "messageId" },
    { ospeAttrType,          "type" },
    { ospeAttrVersion,       "version"},
    { ospeAttrRandom,        "random"}
};

const unsigned OSPVNumAttrDesc = sizeof(OSPVAttrDescs)/sizeof(OSPTMSGDESC);

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgGetAttrName() - get an attribute name from a part value
 *-----------------------------------------------------------------------*/
const char *                              /* returns pointer to the name */
OSPPMsgGetAttrName(
    OSPTMSGATTRPART ospvPart
)
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvPart!= OSPC_MSGPARTUNKNOWN) 
    {
        ospvName = OSPPMsgDescGetName((OSPTMSGPART)ospvPart, OSPVAttrDescs,
            OSPVNumAttrDesc);
    }
    return(ospvName);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgGetAttrPart() - get an attribute part ID from its name
 *-----------------------------------------------------------------------*/
OSPTMSGATTRPART                           /* returns part */
    OSPPMsgGetAttrPart(
    const char *ospvName
    )
{
    OSPTMSGATTRPART ospvPart = ospeAttrUnknown;

    if (ospvName != OSPC_OSNULL) 
    {
        ospvPart = (OSPTMSGATTRPART) OSPPMsgDescGetPart(ospvName,
            OSPVAttrDescs, OSPVNumAttrDesc);
    }
    return(ospvPart);
}

