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
 * ospusage.h - OSP usage detail objects
 */

#ifndef _OSPUSAGE_H
#define _OSPUSAGE_H

#include "osp.h"
#include "ospxmlelem.h"


/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

unsigned OSPPUsageFromElement(OSPTXMLELEM *, unsigned *);
unsigned OSPPUsageToElement(unsigned, OSPTTIME, OSPTXMLELEM **);

#endif

