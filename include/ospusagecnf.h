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

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospstatus.h"
#include "osptnaudit.h"
#include "ospciscoext.h"


typedef struct
{
    OSPTLISTLINK    ospmUsageCnfLink;
    OSPTTIME        ospmUsageCnfTimestamp;
    unsigned char   *ospmUsageCnfMessageId;
    unsigned char   *ospmUsageCnfComponentId;
    OSPTSTATUS      *ospmUsageCnfStatus;
    OSPTTNAUDIT     *ospmUsageCnfTNAudit;
    OSPTCSAUDIT     *ospmUsageCnfCSAudit;
}
OSPTUSAGECNF;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospusageind.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

#ifndef OSPC_DEBUG
/* Timestamp */
#define OSPPUsageCnfSetTimestamp(ospvUsageCnf,ospvTime) \
    (ospvUsageCnf)->ospmUsageCnfTimestamp = (ospvTime)

/* Status */
#define OSPPUsageCnfHasStatus(ospvUsageCnf) \
    (ospvUsageCnf)?((ospvUsageCnf)->ospmUsageCnfStatus != OSPC_OSNULL):OSPC_FALSE
#define OSPPUsageCnfGetStatus(ospvUsageCnf) (ospvUsageCnf)->ospmUsageCnfStatus


#endif /* OSPC_DEBUG */

#ifdef __cplusplus
extern "C" 
{
#endif

    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/

    OSPTUSAGECNF   *OSPPUsageCnfNew(void);
    unsigned        OSPPUsageCnfFromElement(OSPTXMLELEM *, OSPTLIST *);
    void            OSPPUsageCnfDelete(OSPTUSAGECNF **);

    unsigned char  *OSPPUsageCnfGetComponentId(OSPTUSAGECNF *);
    unsigned        OSPPUsageCnfHasComponentId(OSPTUSAGECNF *);
    void            OSPPUsageCnfSetComponentId(OSPTUSAGECNF  *, unsigned char *);

    void            OSPPUsageCnfComponentIdFromElement(OSPTXMLELEM *, const unsigned char **);

    void           OSPPUsageCnfMessageIdFromElement(OSPTXMLELEM *, unsigned char **);

    unsigned       OSPPUsageCnfHasMessageId(OSPTUSAGECNF *);
    void           OSPPUsageCnfSetMessageId(OSPTUSAGECNF *, unsigned char *);

#ifdef OSPC_DEBUG
    void          OSPPUsageCnfSetTimestamp(OSPTUSAGECNF *, OSPTTIME);

    unsigned     OSPPUsageCnfHasStatus(OSPTUSAGECNF *ospvUsageCnf);

    OSPTSTATUS  *OSPPUsageCnfGetStatus(OSPTUSAGECNF *);

#endif /* OSPC_DEBUG */

    OSPTTNAUDIT *OSPPUsageCnfGetTNAudit(OSPTUSAGECNF *);
    OSPTCSAUDIT *OSPPUsageCnfGetCSAudit(OSPTUSAGECNF *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPUSAGECNF_H */
