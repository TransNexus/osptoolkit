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

/* ospmsgattr.h - Definitions for OSP message attributes. */
#ifndef _OSPMSGATTR_H
#define _OSPMSGATTR_H

#include "osp/ospmsgdesc.h"
#include "osp/ospmsgpart.h"

/* Attributes defined by the Open Settlement Protocol */
typedef enum {
    OSPC_MATTR_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_MATTR_UNDEFINED,
    /* Message attribute start */
    OSPC_MATTR_START = OSPC_MPART_START,
    OSPC_MATTR_MESSAGEID = OSPC_MATTR_START,
    OSPC_MATTR_COMPONENTID,
    OSPC_MATTR_RANDOM,
    OSPC_MATTR_CRITICAL,
    OSPC_MATTR_ENCODING,
    OSPC_MATTR_TYPE,
    OSPC_MATTR_DIR,
    OSPC_MATTR_VERSION,
    OSPC_MATTR_INDEX,
    OSPC_MATTR_REPORTER,
    /* Number of message attribtes */
    OSPC_MATTR_NUMBER
} OSPE_MSG_ATTR;

#define OSPC_MAX_INDEX  32

/* Codec types */
typedef enum {
    OSPC_CODEC_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_CODEC_UNDEFINED,
    /* Message attribute start */
    OSPC_CODEC_START = OSPC_MPART_START,
    OSPC_CODEC_SOURCE = OSPC_CODEC_START,
    OSPC_CODEC_DESTINATION,
    /* Number of codec types */
    OSPC_CODEC_NUMBER
} OSPE_CODEC_TYPE;

/* Session ID types */
typedef enum {
    OSPC_SESSIONID_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_SESSIONID_UNDEFINED,
    /* Message attribute start */
    OSPC_SESSIONID_START = OSPC_MPART_START,
    OSPC_SESSIONID_SOURCE = OSPC_SESSIONID_START,
    OSPC_SESSIONID_DESTINATION,
    OSPC_SESSIONID_CORRELATION,
    /* Number of session ID types */
    OSPC_SESSIONID_NUMBER
} OSPE_SESSION_ID;

typedef enum {
    OSPC_SERVICE_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_SERVICE_UNDEFINED,
    /* Message attribute start */
    OSPC_SERVICE_START = OSPC_MPART_START,
    OSPC_SERVICE_VOICE = OSPC_SERVICE_START,
    OSPC_SERVICE_VIDEO,
    OSPC_SERVICE_DATA,
    OSPC_SERVICE_NPQUERY,
    /* Number of service types */
    OSPC_SERVICE_NUMBER
} OSPE_SERVICE;

/* externally declared global variables */
extern const OSPT_MSG_DESC OSPV_MATTR_DESCS[];
extern const OSPT_MSG_DESC OSPV_STYPE_DESCS[];

/* Function Prototypes */
OSPE_MSG_ATTR OSPPMsgAttrGetPart(const char *);
const char *OSPPMsgAttrGetName(OSPE_MSG_ATTR);
OSPE_SERVICE OSPPServiceGetPart(const char *);
const char *OSPPServiceGetName(OSPE_SERVICE);

#endif /* _OSPMSGATTR_H */
