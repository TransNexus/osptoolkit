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
 * ospauthcnf.h - OSP authorisation confirmation objects
 */

#ifndef _OSPAUTHCNF_H
#define _OSPAUTHCNF_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospstatus.h"

typedef struct
{
    OSPTTIME          ospmAuthCnfTimestamp;
    OSPTSTATUS       *ospmAuthCnfStatus;
    OSPTTIME          ospmAuthCnfValidAfter;
    OSPTTIME          ospmAuthCnfValidUntil;
}
OSPTAUTHCNF;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospauthcnf.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG

#define OSPPAuthCnfHasStatus(ospvAuthCnf) \
                (ospvAuthCnf)? \
                ((unsigned)((ospvAuthCnf)->ospmAuthCnfStatus != OSPC_OSNULL)) \
                :OSPC_FALSE

#endif /* OSPC_DEBUG */
/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

    void         OSPPAuthCnfDelete(OSPTAUTHCNF **);

#ifdef OSPC_DEBUG

    unsigned     OSPPAuthCnfHasStatus(OSPTAUTHCNF *ospvAuthCnf);

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHCNF_H */
