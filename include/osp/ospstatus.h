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

/* ospstatus.h - OSP Status Objects */
#ifndef _OSPSTATUS_H
#define _OSPSTATUS_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsg.h"

typedef struct {
    unsigned ospmStatusCode;
    char *ospmStatusDesc;
    OSPTBOOL ospmHasCode;
    OSPTBOOL ospmHasDesc;
} OSPTSTATUS;

/* general macros */
#define OSPM_STATUSCODE_SUCCESSFUL(s)       (((s) > 199) &&( (s) < 300))

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPTSTATUS *OSPPStatusNew(void);
    void OSPPStatusDelete(OSPTSTATUS **ospvStatus);
    unsigned OSPPStatusFromElement(OSPT_XML_ELEM *, OSPTSTATUS **);
    void OSPPStatusSetDesc(OSPTSTATUS *, const char *);
    void OSPPStatusSetCode(OSPTSTATUS *, unsigned);
    OSPTBOOL OSPPStatusHasCode(OSPTSTATUS *);
    unsigned OSPPStatusGetCode(OSPTSTATUS *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTATUS_H */
