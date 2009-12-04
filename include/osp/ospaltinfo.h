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
    OSPC_ALTINFO_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_ALTINFO_UNDEFINED,
    /* Attribute type start, for Alt/Info attributes */
    OSPC_ALTINFO_START = OSPC_MPART_START,
    OSPC_ALTINFO_E164 = OSPC_ALTINFO_START,
    OSPC_ALTINFO_H323,
    OSPC_ALTINFO_URL,
    OSPC_ALTINFO_EMAIL,
    OSPC_ALTINFO_TRANSPORT,
    OSPC_ALTINFO_INTERNATIONAL,
    OSPC_ALTINFO_NATIONAL,
    OSPC_ALTINFO_NETWORK,
    OSPC_ALTINFO_SUBSCRIBER,
    OSPC_ALTINFO_ABBREVIATED,
    OSPC_ALTINFO_E164PREFIX,
    OSPC_ALTINFO_SIP,
    OSPC_ALTINFO_XMPP,
    OSPC_ALTINFO_Q850,
    OSPC_ALTINFO_DEVICEID,
    OSPC_ALTINFO_ASSERTEDID,
    OSPC_ALTINFO_NPRN,
    OSPC_ALTINFO_NPCIC,
    OSPC_ALTINFO_NPDI,
    OSPC_ALTINFO_SPID,
    /* For other attributes */
    OSPC_ALTINFO_TRUE,
    OSPC_ALTINFO_FALSE,
    OSPC_ALTINFO_BASE64,
    OSPC_ALTINFO_CDATA,
    OSPC_ALTINFO_FORWARD,
    OSPC_ALTINFO_REVERSE,
    OSPC_ALTINFO_INBOUND,
    OSPC_ALTINFO_OUTBOUND,
    OSPC_ALTINFO_CALLING,
    OSPC_ALTINFO_CALLED,
    OSPC_ALTINFO_PROXY,
    OSPC_ALTINFO_RTP,
    OSPC_ALTINFO_RTCP,
    OSPC_ALTINFO_DOWNSTREAM,
    OSPC_ALTINFO_UPSTREAM,
    /* Number of element attributes */
    OSPC_ALTINFO_NUMBER
} OSPE_ALTINFO;

/*
 * externally declared global variables
 */
extern const OSPT_MSG_DESC OSPV_ATYPE_DESCS[];

/* the basic altinfo structure */
typedef struct {
    OSPTLISTLINK ospmAltInfoLink;
    unsigned ospmAltInfoLen;
    OSPE_ALTINFO ospmAltInfoType;
    char *ospmAltInfoVal;
} OSPT_ALTINFO;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPT_ALTINFO *OSPPAltInfoNew(unsigned, const char *, OSPE_ALTINFO);
    void OSPPAltInfoDelete(OSPT_ALTINFO **);
    unsigned OSPPAltInfoGetSize(OSPT_ALTINFO *);
    const char *OSPPAltInfoGetValue(OSPT_ALTINFO *);
    unsigned OSPPAltInfoToElement(OSPT_ALTINFO *, OSPT_XML_ELEM **, OSPE_MSG_ELEM);
    OSPE_ALTINFO OSPPAltInfoTypeGetPart(OSPT_ALTINFO *);
    const char *OSPPAltInfoTypeGetName(OSPE_ALTINFO);

#ifdef __cplusplus
}
#endif

#endif /* _OSPALTINFO_H */
