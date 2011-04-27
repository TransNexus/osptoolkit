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

/* ospauthcnf.c - OSP authorization confirmation functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospauthcnf.h"
#include "osp/ospstatus.h"
#include "osp/osputils.h"

/*
 * OSPPAuthCnfDelete() - destroy specified AuthCnf object
 */
void OSPPAuthCnfDelete(
    OSPT_AUTH_CNF **ospvAuthCnf)
{
    if (*ospvAuthCnf != OSPC_OSNULL) {
        if (OSPPAuthCnfHasStatus(*ospvAuthCnf)) {
            OSPPStatusDelete(&((*ospvAuthCnf)->Status));
        }
        OSPM_FREE(*ospvAuthCnf);
        *ospvAuthCnf = OSPC_OSNULL;
    }
}

/*
 * OSPPAuthCnfHasStatus() - does the authorization confirmation have a status?
 */
OSPTBOOL OSPPAuthCnfHasStatus(  /* returns non-zero if number exists */
    OSPT_AUTH_CNF *ospvAuthCnf) /* authorization confirmation effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthCnf != OSPC_OSNULL) {
        has = (ospvAuthCnf->Status != OSPC_OSNULL);
    }

    return has;
}
