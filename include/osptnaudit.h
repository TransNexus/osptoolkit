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
 * osptnaudit.h - TNAudit elements.
 */
#ifndef _OSPTNAUDIT_H
#define _OSPTNAUDIT_H

#include "osp.h"

typedef struct
{
    unsigned          ospmAuditState;
    unsigned char     *ospmAuditURL;
    unsigned          ospmAuditTimeLimit;
    unsigned          ospmAuditMaxMessages;
}
OSPTTNAUDIT;


/* Function prototypes */
#ifdef __cplusplus
extern "C" 
{
#endif
    void           OSPPTNAuditDelete(OSPTTNAUDIT **ospvTNAudit);
    int            OSPPTNAuditFromElement(OSPTXMLELEM *, OSPTTNAUDIT **);

    unsigned            OSPPTNAuditGetMaxMessages(OSPTTNAUDIT *);
    OSPE_AUDIT_STATE    OSPPTNAuditGetState(OSPTTNAUDIT *);
    unsigned            OSPPTNAuditGetTimeLimit(OSPTTNAUDIT *);
    unsigned char       *OSPPTNAuditGetURL(OSPTTNAUDIT *);
    
    unsigned       OSPPTNAuditHasMaxMessages(OSPTTNAUDIT *);
    unsigned       OSPPTNAuditHasState(OSPTTNAUDIT *);
    unsigned       OSPPTNAuditHasTimeLimit(OSPTTNAUDIT *);
    unsigned       OSPPTNAuditHasURL(OSPTTNAUDIT *);

    OSPTTNAUDIT    *OSPPTNAuditNew(void);

    void           OSPPTNAuditSetMaxMessages(OSPTTNAUDIT *, unsigned);
    void           OSPPTNAuditSetState(OSPTTNAUDIT *, OSPE_AUDIT_STATE);
    void           OSPPTNAuditSetTimeLimit(OSPTTNAUDIT *, unsigned);
    void           OSPPTNAuditSetURL(OSPTTNAUDIT *, unsigned char *);

#ifdef __cplusplus
}
#endif

#endif
