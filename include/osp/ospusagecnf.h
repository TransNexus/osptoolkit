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
 * ospusagecnf.h - OSP Usage Confirm Objects
 */

#ifndef _OSPUSAGECNF_H
#define _OSPUSAGECNF_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospstatus.h"
#include "osp/osptnaudit.h"
#include "osp/ospciscoext.h"

typedef struct {
    OSPTLISTLINK ospmUsageCnfLink;
    OSPTTIME ospmUsageCnfTimestamp;
    unsigned char *ospmUsageCnfMessageId;
    unsigned char *ospmUsageCnfComponentId;
    OSPTSTATUS *ospmUsageCnfStatus;
    OSPTTNAUDIT *ospmUsageCnfTNAudit;
    OSPTCSAUDIT *ospmUsageCnfCSAudit;
} OSPTUSAGECNF;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTUSAGECNF *OSPPUsageCnfNew(void);
    unsigned OSPPUsageCnfFromElement(OSPT_XML_ELEM *, OSPTLIST *);
    void OSPPUsageCnfDelete(OSPTUSAGECNF **);
    unsigned char *OSPPUsageCnfGetComponentId(OSPTUSAGECNF *);
    unsigned OSPPUsageCnfHasComponentId(OSPTUSAGECNF *);
    void OSPPUsageCnfSetComponentId(OSPTUSAGECNF *, unsigned char *);
    void OSPPUsageCnfComponentIdFromElement(OSPT_XML_ELEM *, const unsigned char **);
    void OSPPUsageCnfMessageIdFromElement(OSPT_XML_ELEM *, unsigned char **);
    unsigned OSPPUsageCnfHasMessageId(OSPTUSAGECNF *);
    void OSPPUsageCnfSetMessageId(OSPTUSAGECNF *, unsigned char *);
    void OSPPUsageCnfSetTimestamp(OSPTUSAGECNF *, OSPTTIME);
    unsigned OSPPUsageCnfHasStatus(OSPTUSAGECNF *ospvUsageCnf);
    OSPTSTATUS *OSPPUsageCnfGetStatus(OSPTUSAGECNF *);
    OSPTTNAUDIT *OSPPUsageCnfGetTNAudit(OSPTUSAGECNF *);
    OSPTCSAUDIT *OSPPUsageCnfGetCSAudit(OSPTUSAGECNF *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPUSAGECNF_H */
