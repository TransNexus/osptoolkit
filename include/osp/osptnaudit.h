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

/* osptnaudit.h - TNAudit elements. */
#ifndef _OSPTNAUDIT_H
#define _OSPTNAUDIT_H

#include "osp/osp.h"

typedef struct {
    unsigned State;
    char *URL;
    unsigned TimeLimit;
    unsigned MaxMessages;
} OSPT_TN_AUDIT;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    void OSPPTNAuditDelete(OSPT_TN_AUDIT **ospvTNAudit);
    int OSPPTNAuditFromElement(OSPT_XML_ELEM *, OSPT_TN_AUDIT **);
    unsigned OSPPTNAuditGetMaxMessages(OSPT_TN_AUDIT *);
    OSPE_AUDIT_STATE OSPPTNAuditGetState(OSPT_TN_AUDIT *);
    unsigned OSPPTNAuditGetTimeLimit(OSPT_TN_AUDIT *);
    const char *OSPPTNAuditGetURL(OSPT_TN_AUDIT *);
    OSPTBOOL OSPPTNAuditHasMaxMessages(OSPT_TN_AUDIT *);
    OSPTBOOL OSPPTNAuditHasState(OSPT_TN_AUDIT *);
    OSPTBOOL OSPPTNAuditHasTimeLimit(OSPT_TN_AUDIT *);
    OSPTBOOL OSPPTNAuditHasURL(OSPT_TN_AUDIT *);
    OSPT_TN_AUDIT *OSPPTNAuditNew(void);
    void OSPPTNAuditSetMaxMessages(OSPT_TN_AUDIT *, unsigned);
    void OSPPTNAuditSetState(OSPT_TN_AUDIT *, OSPE_AUDIT_STATE);
    void OSPPTNAuditSetTimeLimit(OSPT_TN_AUDIT *, unsigned);
    void OSPPTNAuditSetURL(OSPT_TN_AUDIT *, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPTNAUDIT_H */
