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

/* ospacapind.h - OSP capabiliti indication objects */
#ifndef _OSPCAPIND_H
#define _OSPCAPIND_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospaltinfo.h"
#include "osp/osptrans.h"

typedef struct {
    OSPTTIME Timestamp;
    char *MessageId;
    char *ComponentId;
    OSPTLIST DeviceInfo;
    unsigned AlmostOutOfResources;
    OSPTLIST SourceAlternate;
} OSPT_CAP_IND;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    unsigned OSPPGenerateUniqueId(OSPTTRANS *, char **);
    unsigned OSPPCapIndNew(OSPT_CAP_IND **, OSPTTRANS *, const char *, const char *, const char *, unsigned);
    void OSPPCapIndDelete(OSPT_CAP_IND **);
    int OSPPCapIndToElement(OSPT_CAP_IND *, OSPT_XML_ELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPACAPIND_H */
