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
 * ospmsgdesc.c - generic message descriptor processing functions.
 */

#include "osp/osp.h"
#include "osp/ospmsgdesc.h"

/*
 * OSPPMsgDescGetPart() - find a part from a name
 */
OSPT_MSG_PART OSPPMsgDescGetPart(
    const char *ospvName,           /* name for which to find tag */
    const OSPT_MSG_DESC *ospvDesc,  /* array of descriptors to search */
    unsigned ospvNumDesc)           /* size of array */
{
    OSPT_MSG_PART ospvPart = OSPC_MPART_UNKNOWN;
    unsigned cnt;

    if (ospvName != OSPC_OSNULL) {
        for (cnt = 0; cnt < ospvNumDesc; cnt++) {
            if (OSPM_STRCMP(ospvDesc[cnt].ospmName, ospvName) == 0) {
                ospvPart = ospvDesc[cnt].ospmPart;
                break;
            }
        }
    }

    return (ospvPart);
}

/*
 * OSPPMsgDescGetName() - find a name from a part
 */
const char *OSPPMsgDescGetName(     /* returns the name found or NULL */
    OSPT_MSG_PART ospvPart,         /* part for which to find name */
    const OSPT_MSG_DESC *ospvDesc,  /* array of descriptors to search */
    unsigned ospvNumDesc)           /* size of array */
{
    const char *ospvName = OSPC_OSNULL;
    unsigned cnt;

    for (cnt = 0; cnt < ospvNumDesc; cnt++) {
        if (ospvDesc[cnt].ospmPart == ospvPart) {
            ospvName = ospvDesc[cnt].ospmName;
            break;
        }
    }

    return (ospvName);
}
