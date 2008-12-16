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
 * ospmsgelem.h - Definitions for OSP message elements.
 */

#ifndef _OSPMSGELEM_H
#define _OSPMSGELEM_H

#include "osp/ospmsgpart.h"
#include "osp/ospmsgdesc.h"

/* Document types defined by the Open Settlement Protocol */
typedef enum {
    OSPC_MELEM_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_MELEM_ALMOSTOUTOFRESOURCES,
    OSPC_MELEM_AMOUNT,
    OSPC_MELEM_AUTHCNF,
    OSPC_MELEM_AUTHIND,
    OSPC_MELEM_AUTHREQ,
    OSPC_MELEM_AUTHRSP,
    OSPC_MELEM_AUTHRZP,
    OSPC_MELEM_AUTHURL,
    OSPC_MELEM_CALLID,
    OSPC_MELEM_CAPIND,
    OSPC_MELEM_CODE,
    OSPC_MELEM_CURRENCY,
    OSPC_MELEM_DESC,
    OSPC_MELEM_DEST,
    OSPC_MELEM_DESTALT,
    OSPC_MELEM_DESTINFO,
    OSPC_MELEM_DESTSIGADDR,
    OSPC_MELEM_DEVICEINFO,
    OSPC_MELEM_ENDPOINT,
    OSPC_MELEM_INCREMENT,
    OSPC_MELEM_MAXDEST,
    OSPC_MELEM_MESSAGE,
    OSPC_MELEM_PRICINGCNF,
    OSPC_MELEM_PRICINGIND,
    OSPC_MELEM_REAUTHREQ,
    OSPC_MELEM_REAUTHRSP,
    OSPC_MELEM_RESOURCES,
    OSPC_MELEM_ROLE,
    OSPC_MELEM_SERVICE,
    OSPC_MELEM_SRCALT,
    OSPC_MELEM_SRCINFO,
    OSPC_MELEM_SRCSIGADDR,
    OSPC_MELEM_STARTTIME,
    OSPC_MELEM_STATUS,
    OSPC_MELEM_TIMESTAMP,
    OSPC_MELEM_TOKEN,
    OSPC_MELEM_TOKENINFO,
    OSPC_MELEM_TRANSID,
    OSPC_MELEM_UNIT,
    OSPC_MELEM_USAGECNF,
    OSPC_MELEM_USAGEDETAIL,
    OSPC_MELEM_USAGEIND,
    OSPC_MELEM_VALIDAFTER,
    OSPC_MELEM_VALIDUNTIL,
    OSPC_MELEM_CUSTID,
    OSPC_MELEM_DELAYLIMIT,
    OSPC_MELEM_DELAYPREF,
    OSPC_MELEM_DEVICEID,
    OSPC_MELEM_FAILREASON,
    OSPC_MELEM_TCCODE,
    OSPC_MELEM_FRACTION,
    OSPC_MELEM_LOSSRCVD,
    OSPC_MELEM_LOSSSENT,
    OSPC_MELEM_MEAN,
    OSPC_MELEM_MINIMUM,
    OSPC_MELEM_ONEWAYDELAY,
    OSPC_MELEM_PACKETS,
    OSPC_MELEM_ROUNDTRIPDELAY,
    OSPC_MELEM_SAMPLES,
    OSPC_MELEM_STATISTICS,
    OSPC_MELEM_VARIANCE,
    OSPC_MELEM_AUDIT,
    OSPC_MELEM_AUDITURL,
    OSPC_MELEM_AUDITSTATE,
    OSPC_MELEM_AUDITTIMELIMIT,
    OSPC_MELEM_AUDITMAXMESSAGES,
    OSPC_MELEM_CSAUDITTRIGGER,
    OSPC_MELEM_DESTPROTOCOL,
    OSPC_MELEM_DESTOSPVERSION,
    OSPC_MELEM_ENDTIME,
    OSPC_MELEM_ALERTTIME,
    OSPC_MELEM_POSTDIALDELAY,
    OSPC_MELEM_CONNECTTIME,
    OSPC_MELEM_RELEASESOURCE,
    OSPC_MELEM_SERVICETYPE,
    OSPC_MELEM_GROUP,
    OSPC_MELEM_GROUPID,
    /* Number of message elements */
    OSPC_MELEM_NUMBER
} OSPE_MSG_ELEM;

/*
 * externally declared global variables
 */

extern const OSPT_MSG_DESC OSPV_MELEM_DESCS[];

/* Function Prototypes */

OSPE_MSG_ELEM OSPPMsgElemGetPart(const char *);
const char *OSPPMsgElemGetName(OSPE_MSG_ELEM);

#endif /* _OSPMSGELEM_H */
