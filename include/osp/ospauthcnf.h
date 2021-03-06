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

/* ospauthcnf.h - OSP authorization confirmation objects */
#ifndef _OSPAUTHCNF_H
#define _OSPAUTHCNF_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospstatus.h"

typedef struct {
    OSPTTIME Timestamp;
    OSPT_STATUS *Status;
    OSPTTIME ValidAfter;
    OSPTTIME ValidUntil;
} OSPT_AUTH_CNF;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    void OSPPAuthCnfDelete(OSPT_AUTH_CNF **);
    OSPTBOOL OSPPAuthCnfHasStatus(OSPT_AUTH_CNF *ospvAuthCnf);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUTHCNF_H */
