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
 * ospauthcnf.c - OSP authorisation confirmation functions
 */

#include "osp.h"
#include "osperrno.h"
#include "ospbfr.h"
#include "ospxmlattr.h"
#include "ospxmlelem.h"
#include "ospmsgattr.h"
#include "ospmsgelem.h"
#include "ospauthcnf.h"
#include "ospstatus.h"
#include "osputils.h"


/*-----------------------------------------------------------------------*
 * OSPPAuthCnfDelete() - destroy specified AuthCnf object
 *-----------------------------------------------------------------------*/

void
OSPPAuthCnfDelete(
    OSPTAUTHCNF **ospvAuthCnf
)
{

    if (*ospvAuthCnf != OSPC_OSNULL)
    {
        if (OSPPAuthCnfHasStatus(*ospvAuthCnf))
        {
            OSPPStatusDelete(&((*ospvAuthCnf)->ospmAuthCnfStatus));
        }
        OSPM_FREE(*ospvAuthCnf);
        *ospvAuthCnf = OSPC_OSNULL;
    }
}

#ifdef OSPC_DEBUG
/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthCnfHasStatus() - does the authorisation confirmation have
 * a status?
 *-----------------------------------------------------------------------*/

unsigned                            /* returns non-zero if number exists */
OSPPAuthCnfHasStatus(
    OSPTAUTHCNF *ospvAuthCnf       /* authorisation confirmation effected */
)
{
    unsigned ospvHasStatus = OSPC_FALSE;

    if(ospvAuthCnf != OSPC_OSNULL)
    {
        ospvHasStatus = ((ospvAuthCnf)->ospmAuthCnfStatus != OSPC_OSNULL);
    }
    return(ospvHasStatus);
}

#endif /* OSPC_DEBUG */

