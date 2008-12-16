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
 *  ospaltinfo.h - OSP Alternate information element
 */

#ifndef _OSPALTINFO_H
#define _OSPALTINFO_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"

/* Max size of an altinfo (bytes) */
#define OSPC_ALTINFOMAXSIZE 1024

typedef enum {
    OSPC_ATYPE_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_ATYPE_E164,
    OSPC_ATYPE_H323,
    OSPC_ATYPE_URL,
    OSPC_ATYPE_EMAIL,
    OSPC_ATYPE_TRANSPORT,
    OSPC_ATYPE_INTERNATIONAL,
    OSPC_ATYPE_NATIONAL,
    OSPC_ATYPE_NETWORK,
    OSPC_ATYPE_SUBSCRIBER,
    OSPC_ATYPE_ABBREVIATED,
    OSPC_ATYPE_E164PREFIX,
    OSPC_ATYPE_SIP,
    OSPC_ATYPE_XMPP,
    OSPC_ATYPE_Q850,
    OSPC_ATYPE_DEVICEID,
    OSPC_ATYPE_ASSERTEDID,
    /* Number of element attributes */
    OSPC_ATYPE_NUMBER
} OSPE_ALTINFO_TYPE;

/*
 * externally declared global variables
 */

extern const OSPT_MSG_DESC OSPV_ATYPE_DESCS[];

/* the basic altinfo structure */

typedef struct {
    OSPTLISTLINK ospmAltInfoLink;
    unsigned ospmAltInfoLen;
    OSPE_ALTINFO_TYPE ospmAltInfoType;
    char *ospmAltInfoVal;
} OSPT_ALTINFO;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPT_ALTINFO *OSPPAltInfoNew(unsigned, const char *, OSPE_ALTINFO_TYPE);
    void OSPPAltInfoDelete(OSPT_ALTINFO **);
    unsigned OSPPAltInfoGetSize(OSPT_ALTINFO *);
    OSPE_ALTINFO_TYPE OSPPAltInfoGetType(OSPT_ALTINFO *);
    const char *OSPPAltInfoGetValue(OSPT_ALTINFO *);
    unsigned OSPPAltInfoToElement(OSPT_ALTINFO *, OSPT_XML_ELEM **, OSPE_MSG_ELEM);
    const char *OSPPAltInfoTypeGetName(OSPE_ALTINFO_TYPE);

#ifdef __cplusplus
}
#endif

#endif /* _OSPALTINFO_H */
