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
 * ospmsgattr.h - Definitions for OSP message attributes.
 */

#ifndef _OSPMSGATTR_H
#define _OSPMSGATTR_H

#include "ospmsgdesc.h"
#include "ospmsgpart.h"

/* attributes defined by the Open Settlement Protocol */

typedef enum
{
    ospeAttrUnknown = OSPC_MSGPARTUNKNOWN,
    ospeAttrComponentId,
    ospeAttrCritical,
    ospeAttrEncoding,
    ospeAttrMessageId,
    ospeAttrType,
    ospeAttrVersion,
    ospeAttrRandom
}
OSPTMSGATTRPART;

/**/
/*-----------------------------------------------------------------------*
 * externally declared global variables
 *-----------------------------------------------------------------------*/

extern const OSPTMSGDESC OSPVAttrDescs[];
extern const unsigned    OSPVNumAttrDesc;

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

#ifndef OSPC_DEBUG
/*
 * Note: all macros are also implemented as functions in ospmsgattr.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

#define OSPPMsgGetAttrName(ospvPart) \
    OSPPMsgDescGetName((OSPTMSGPART)(ospvPart), OSPVAttrDescs, \
                       OSPVNumAttrDesc)
#define OSPPMsgGetAttrPart(ospvName) \
   ((OSPTMSGATTRPART) OSPPMsgDescGetPart((ospvName), OSPVAttrDescs, \
                          OSPVNumAttrDesc))

#endif /* OSPC_DEBUG */

/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/
#ifdef OSPC_DEBUG

const char *OSPPMsgGetAttrName(OSPTMSGATTRPART);
OSPTMSGATTRPART OSPPMsgGetAttrPart(const char *);

#endif /* OSPC_DEBUG */

#endif /* _OSPMSGATTR_H */

