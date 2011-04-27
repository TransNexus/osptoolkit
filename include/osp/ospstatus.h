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
    unsigned Code;
    char *Description;
    OSPTBOOL HasCode;
    OSPTBOOL HasDescription;
} OSPT_STATUS;

/* general macros */
#define OSPM_STATUSCODE_SUCCESSFUL(s)       (((s) > 199) &&( (s) < 300))

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_STATUS *OSPPStatusNew(void);
    void OSPPStatusDelete(OSPT_STATUS **ospvStatus);
    unsigned OSPPStatusFromElement(OSPT_XML_ELEM *, OSPT_STATUS **);
    void OSPPStatusSetDesc(OSPT_STATUS *, const char *);
    void OSPPStatusSetCode(OSPT_STATUS *, unsigned);
    OSPTBOOL OSPPStatusHasCode(OSPT_STATUS *);
    unsigned OSPPStatusGetCode(OSPT_STATUS *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTATUS_H */
