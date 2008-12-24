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

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospaltinfo.h"

/* Function Prototypes */

unsigned OSPPUsageFromElement(OSPT_XML_ELEM *, unsigned *);
unsigned OSPPUsageToElement(unsigned, OSPTTIME, OSPTTIME, OSPTTIME, OSPTTIME, unsigned, unsigned, unsigned, OSPT_XML_ELEM *);
unsigned OSPPAddConfIdToUsageElement(const char *, OSPT_XML_ELEM **);
unsigned OSPPAddServiceTypeToUsageElement(OSPE_SERVICE, OSPT_XML_ELEM **);
unsigned OSPPAddPricingInfoToUsageElement(OSPT_PRICING_INFO, OSPT_XML_ELEM **);

unsigned OSPPCallPartyNumToElement(OSPE_MSG_ELEM, const char *, OSPE_NUMBER_FORMAT, OSPT_XML_ELEM **);
unsigned OSPPTermCauseToElement(OSPE_TERM_CAUSE, unsigned, const char *, OSPT_XML_ELEM **);
unsigned OSPPStringToElement(OSPE_MSG_ELEM, const char *, unsigned, OSPE_MSG_ATTR *, OSPE_ALTINFO *, OSPT_XML_ELEM **);

#endif /* _OSPUSAGE_H */
