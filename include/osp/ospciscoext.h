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
 * ospciscoext.h - Cisco Audit elements.
 */

#ifndef _OSPCISCOEXT_H
#define _OSPCISCOEXT_H

#include "osp/osp.h"

typedef struct {
    unsigned char *ospmAuditTrigger;
} OSPTCSAUDIT;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif
    void OSPPCSAuditDelete(OSPTCSAUDIT **ospvCSAudit);
    int OSPPCSAuditFromElement(OSPT_XML_ELEM *, OSPTCSAUDIT **);
    unsigned char *OSPPCSAuditGetTrigger(OSPTCSAUDIT *);
    unsigned OSPPCSAuditHasTrigger(OSPTCSAUDIT *);
    OSPTCSAUDIT *OSPPCSAuditNew(void);
    void OSPPCSAuditSetTrigger(OSPTCSAUDIT *, unsigned char *);
    int OSPPCSAuditToElement(OSPTCSAUDIT *, OSPT_XML_ELEM **);
#ifdef __cplusplus
}
#endif

#endif /* _OSPCISCOEXT_H */
