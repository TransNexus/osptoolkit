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

/* ospcapcnf.h - OSP capabiliti indication objects */
#ifndef _OSPCAPCNF_H
#define _OSPCAPCNF_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"
#include "osp/ospaltinfo.h"
#include "osp/osptrans.h"

typedef struct {
    OSPT_STATUS *ospmStatus;
} OSPT_CAP_CNF;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    unsigned OSPPCapCnfNew(OSPT_CAP_CNF **);
    void OSPPCapCnfDelete(OSPT_CAP_CNF **);
    unsigned OSPPCapCnfFromElement(OSPT_XML_ELEM *, OSPT_CAP_CNF **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPCAPCNF_H */
