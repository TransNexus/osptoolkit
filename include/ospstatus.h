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
 * ospstatus.h - OSP Status Objects
 */

#ifndef _OSPSTATUS_H
#define _OSPSTATUS_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"


typedef struct
{
    unsigned          ospmStatusCode;
    unsigned char     *ospmStatusDesc;
    unsigned          ospmHasCode;
    unsigned          ospmHasDesc;
}
OSPTSTATUS;

/**/
/*-----------------------------------------------------------------------*
 * general macros
 *-----------------------------------------------------------------------*/
#define OSPM_STATUSCODE_SUCCESSFUL(s)      (((s) > 199) &&( (s) < 300))

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

#ifndef OSPC_DEBUG
/*
 * Note: all macros are also implemented as functions in ospstatus.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

/* Code */
#define OSPPStatusHasCode(ospvStatus) \
    (ospvStatus)?((ospvStatus)->ospmHasCode):OSPC_FALSE
#define OSPPStatusGetCode(ospvStatus) \
    (ospvStatus)?((ospvStatus)->ospmStatusCode):0

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
extern "C" 
{
#endif

    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/

    OSPTSTATUS    *OSPPStatusNew(void);
    void           OSPPStatusDelete(OSPTSTATUS **ospvStatus);
    unsigned       OSPPStatusFromElement(OSPTXMLELEM *, OSPTSTATUS **);
    void           OSPPStatusSetDesc(OSPTSTATUS *, unsigned char *);
    void           OSPPStatusSetCode(OSPTSTATUS *, unsigned);

#ifdef OSPC_DEBUG

    unsigned       OSPPStatusHasCode(OSPTSTATUS *);
    unsigned       OSPPStatusGetCode(OSPTSTATUS *);

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTATUS_H */
