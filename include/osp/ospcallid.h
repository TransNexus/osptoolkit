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
 * ospcallid.h - OSP call identifier objects
 */

#ifndef _OSPCALLID_H
#define _OSPCALLID_H

#include "osp/osp.h"
#include "osp/osplist.h"
#include "osp/ospxmlelem.h"
#include "osp/ospaltinfo.h"
#include "osp/ospmsgattr.h"

/* max size of a call ID (bytes) */

#define OSPC_CALLID_MAXSIZE         256

/* the basic call identifier structure */

typedef struct {
    OSPTLISTLINK ospmCallIdLink;
    unsigned ospmCallIdLen;
    unsigned char *ospmCallIdVal;
} OSPT_CALL_ID;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPT_CALL_ID *OSPPCallIdNew(unsigned, const unsigned char *);
    void OSPPCallIdDelete(OSPT_CALL_ID **);
    unsigned OSPPCallIdFromElement(OSPT_XML_ELEM *, OSPT_CALL_ID **);
    unsigned OSPPCallIdToElement(OSPT_CALL_ID *, OSPT_XML_ELEM **, OSPTBOOL);
    unsigned OSPPCallIdGetSize(OSPT_CALL_ID *);
    unsigned char *OSPPCallIdGetValue(OSPT_CALL_ID *);
    unsigned OSPPCallIdFromASCIIElement(unsigned char *, OSPT_CALL_ID **);
    unsigned OSPPSessionIdToElement(OSPT_CALL_ID *, OSPE_DIRECTION, OSPTBOOL, OSPT_XML_ELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPCALLID_H */
