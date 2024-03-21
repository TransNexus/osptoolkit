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

/* ospaltinfo.h - OSP Alternate information element */
#ifndef _OSPALTINFO_H
#define _OSPALTINFO_H

#include "osp/osp.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"

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
    OSPC_ALTINFO_NPRN,
    OSPC_ALTINFO_NPCIC,
    OSPC_ALTINFO_NPDI,
    OSPC_ALTINFO_SPID,
    OSPC_ALTINFO_ALTSPID,
    OSPC_ALTINFO_OCN,
    OSPC_ALTINFO_SPN,
    OSPC_ALTINFO_ALTSPN,
    OSPC_ALTINFO_MCC,
    OSPC_ALTINFO_MNC,
    OSPC_ALTINFO_SMS,
    OSPC_ALTINFO_MMS,
    OSPC_ALTINFO_DISPLAYNAME,
    OSPC_ALTINFO_INGRESS,
    OSPC_ALTINFO_EGRESS,
    OSPC_ALTINFO_CNAM,
    OSPC_ALTINFO_SWITCHID,
    OSPC_ALTINFO_ICID,
    OSPC_ALTINFO_GENERATEDAT,
    OSPC_ALTINFO_ORIGIOI,
    OSPC_ALTINFO_TERMIOI,
    OSPC_ALTINFO_ORIGINAL,
    OSPC_ALTINFO_REDIRECT,
    /* For other attributes */
    OSPC_ALTINFO_TRUE,
    OSPC_ALTINFO_FALSE,
    OSPC_ALTINFO_BASE64,
    OSPC_ALTINFO_CDATA,
    OSPC_ALTINFO_SOURCE,
    OSPC_ALTINFO_DESTINATION,
    OSPC_ALTINFO_CORRELATION,
    OSPC_ALTINFO_LOCAL,
    OSPC_ALTINFO_REMOTE,
    OSPC_ALTINFO_NA,
    OSPC_ALTINFO_RTP,
    OSPC_ALTINFO_RTCP,
    OSPC_ALTINFO_SRCREP,
    OSPC_ALTINFO_REPSRC,
    OSPC_ALTINFO_DESTREP,
    OSPC_ALTINFO_REPDEST,
    OSPC_ALTINFO_SRCDEST,
    OSPC_ALTINFO_DESTSRC,
    OSPC_ALTINFO_CALLING,
    OSPC_ALTINFO_CALLED,
    OSPC_ALTINFO_VOICE,
    OSPC_ALTINFO_VIDEO,
    OSPC_ALTINFO_PROXY,
    OSPC_ALTINFO_CAPTCHAINDEX,
    OSPC_ALTINFO_AUTHENTICATION,
    OSPC_ALTINFO_VERIFICATION,
    /* Number of element attributes */
    OSPC_ALTINFO_NUMBER
} OSPE_ALTINFO;

/* externally declared global variables */
extern const OSPT_MSG_DESC OSPV_ATYPE_DESCS[];

/* the basic altinfo structure */
typedef struct {
    OSPTLISTLINK Link;
    unsigned Length;
    OSPE_ALTINFO Type;
    char *Value;
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
    OSPE_ALTINFO OSPPAltInfoGetPart(OSPT_ALTINFO *);
    OSPE_ALTINFO OSPPAltInfoTypeGetPart(const char *);
    const char *OSPPAltInfoTypeGetName(OSPE_ALTINFO);

#ifdef __cplusplus
}
#endif

#endif /* _OSPALTINFO_H */
