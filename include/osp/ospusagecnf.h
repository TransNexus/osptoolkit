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

/* ospusagecnf.h - OSP Usage Confirm Objects */
#ifndef _OSPUSAGECNF_H
#define _OSPUSAGECNF_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospstatus.h"
#include "osp/osptnaudit.h"
#include "osp/ospciscoext.h"

typedef struct {
    OSPTLISTLINK Link;
    OSPTTIME Timestamp;
    char *MessageId;
    char *ComponentId;
    OSPT_STATUS *Status;
    OSPT_TN_AUDIT *TNAudit;
    OSPTCSAUDIT *CSAudit;
} OSPT_USAGE_CNF;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_USAGE_CNF *OSPPUsageCnfNew(void);
    unsigned OSPPUsageCnfFromElement(OSPT_XML_ELEM *, OSPTLIST *);
    void OSPPUsageCnfDelete(OSPT_USAGE_CNF **);
    const char *OSPPUsageCnfGetComponentId(OSPT_USAGE_CNF *);
    OSPTBOOL OSPPUsageCnfHasComponentId(OSPT_USAGE_CNF *);
    void OSPPUsageCnfSetComponentId(OSPT_USAGE_CNF *, const char *);
    void OSPPUsageCnfComponentIdFromElement(OSPT_XML_ELEM *, const char **);
    void OSPPUsageCnfMessageIdFromElement(OSPT_XML_ELEM *, const char **);
    OSPTBOOL OSPPUsageCnfHasMessageId(OSPT_USAGE_CNF *);
    void OSPPUsageCnfSetMessageId(OSPT_USAGE_CNF *, const char *);
    void OSPPUsageCnfSetTimestamp(OSPT_USAGE_CNF *, OSPTTIME);
    OSPTBOOL OSPPUsageCnfHasStatus(OSPT_USAGE_CNF *ospvUsageCnf);
    OSPT_STATUS *OSPPUsageCnfGetStatus(OSPT_USAGE_CNF *);
    OSPT_TN_AUDIT *OSPPUsageCnfGetTNAudit(OSPT_USAGE_CNF *);
    OSPTCSAUDIT *OSPPUsageCnfGetCSAudit(OSPT_USAGE_CNF *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPUSAGECNF_H */
