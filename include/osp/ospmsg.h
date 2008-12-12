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
 * ospmsg.h - Main OSP message definitions
 */

#ifndef _OSPMSG_H
#define _OSPMSG_H

#include "osp/osp.h"
#include "osp/ospostime.h"
#include "osp/osplist.h"

typedef enum {
    OSPC_MSG_UNKNOWN = 0,
    OSPC_MSG_AREQ,
    OSPC_MSG_ARESP,
    OSPC_MSG_AREZP,
    OSPC_MSG_AIND,
    OSPC_MSG_ACNF,
    OSPC_MSG_UIND,
    OSPC_MSG_UCNF,
    OSPC_MSG_REAREQ,
    OSPC_MSG_REARESP,
    OSPC_MSG_TOKINFO,
    OSPC_MSG_CAPIND,
    OSPC_MSG_CAPCNF,
    /* Number of message types */
    OSPC_MSG_NUMBER
} OSPE_MSG_TYPE;

typedef enum {
    OSPC_MROLE_UNDEFINED = 0,    /* Not a known role */
    OSPC_MROLE_DESTINATION,
    OSPC_MROLE_SOURCE,
    OSPC_MROLE_OTHER,
    OSPC_MROLE_RADSRCSTART,
    OSPC_MROLE_RADDSTSTART,
    OSPC_MROLE_RADSRCSTOP,
    OSPC_MROLE_RADDSTSTOP
} OSPE_MSG_ROLE;

/* general constants */

#define OSPC_SIZE_E164NUM    132    /* max digits in E.164 number */
#define OSPC_SIZE_SIGNALADDR 262    /* max characters in [name]:port */
#define OSPC_SIZE_URL        512    /* max characters in URL */
#define OSPC_SIZE_ROLESTR    16     /* max characters in role string */

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    unsigned OSPPMsgBinFromElement(OSPT_XML_ELEM *, unsigned *, unsigned char **);
    unsigned OSPPMsgBinToElement(unsigned, unsigned char *, const char *, OSPT_XML_ELEM **, OSPTBOOL);
    unsigned OSPPMsgNumFromElement(OSPT_XML_ELEM *, unsigned long *);
    unsigned OSPPMsgNumToElement(unsigned long, const char *, OSPT_XML_ELEM **);
    int OSPPMsgFloatFromElement(OSPT_XML_ELEM *, float *);
    int OSPPMsgFloatToElement(float, const char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgCodeFromElement(OSPT_XML_ELEM *, unsigned long *);
    unsigned OSPPMsgCodeToElement(unsigned long, const unsigned char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgTXToElement(OSPTTRXID, const char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgTXFromElement(OSPT_XML_ELEM *, OSPTTRXID *);
    unsigned OSPPMsgTimeToElement(OSPTTIME, const char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgElemIsCritical(OSPT_XML_ELEM *);
    unsigned OSPPMsgRoleToElement(unsigned, const char *, OSPT_XML_ELEM **);
    int OSPPOSRoleValToString(unsigned, char *ospvRolestr);
    int OSPPBase64DecodeWrap(const unsigned char *, unsigned *, unsigned char *);
    unsigned OSPPMsgTimeFromElement(OSPT_XML_ELEM *, OSPTTIME *);
    unsigned OSPPMsgBinFromASCIIElement(unsigned char *, unsigned *, unsigned char **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPMSG_H */
