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

#include "osp.h"
#include "ospmsgdesc.h"

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgDescGetPart() - find a part from a name
 *-----------------------------------------------------------------------*/
OSPTMSGPART
    OSPPMsgDescGetPart(
    const char        *ospvName,           /* name for which to find tag */
    const OSPTMSGDESC *ospvDesc,       /* array of descriptors to search */
    unsigned           ospvNumDesc                      /* size of array */
    )
{
    unsigned    cnt;
    OSPTMSGPART ospvPart = OSPC_MSGPARTUNKNOWN;

    if (ospvName != OSPC_OSNULL) 
    {
        /* now just search through the array looking for the name */
        for (cnt=0; cnt<ospvNumDesc; cnt++)
        {
            if (OSPM_STRCMP(ospvDesc->ospmMsgDescName, ospvName) == 0)
            {
                ospvPart = ospvDesc->ospmMsgDescPart;
                break;
            }
            ospvDesc++;
        }
    }
    return(ospvPart);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgDescGetName() - find a name from a part
 *-----------------------------------------------------------------------*/
const char *                           /* returns the name found or NULL */
OSPPMsgDescGetName(
    OSPTMSGPART        ospvPart,          /* part for which to find name */
    const OSPTMSGDESC *ospvDesc,       /* array of descriptors to search */
    unsigned           ospvNumDesc                      /* size of array */
)
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvPart != OSPC_MSGPARTUNKNOWN) 
    {

        /* alternate approach - faster, but array must be built appropriately */
        if ((unsigned)ospvPart < ospvNumDesc) 
        {
            ospvName = ospvDesc[(int)ospvPart].ospmMsgDescName;
        }

    }
    return(ospvName);
}
