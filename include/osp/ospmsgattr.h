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

#include "osp/ospmsgdesc.h"
#include "osp/ospmsgpart.h"

/* attributes defined by the Open Settlement Protocol */

typedef enum {
    ospeAttrUnknown = OSPC_MSGPARTUNKNOWN,
    ospeAttrComponentId,
    ospeAttrCritical,
    ospeAttrEncoding,
    ospeAttrMessageId,
    ospeAttrType,
    ospeAttrVersion,
    ospeAttrRandom
} OSPTMSGATTRPART;

/*
 * externally declared global variables
 */

extern const OSPTMSGDESC OSPVAttrDescs[];
extern const unsigned OSPVNumAttrDesc;

/* Function Prototypes */

const char *OSPPMsgGetAttrName(OSPTMSGATTRPART);
OSPTMSGATTRPART OSPPMsgGetAttrPart(const char *);

#endif /* _OSPMSGATTR_H */
