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

/* ospmsg.h - Main OSP message definitions */
#ifndef _OSPMSG_H
#define _OSPMSG_H

#include "osp/osp.h"
#include "osp/ospostime.h"
#include "osp/osplist.h"
#include "osp/ospmsgpart.h"

typedef enum {
    OSPC_MSG_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_MSG_UNDEFINED,
    /* Message type start */
    OSPC_MSG_START = 0,
    OSPC_MSG_AREQ = OSPC_MSG_START,
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
} OSPE_MESSAGE;

typedef enum {
    OSPC_ROLE_UNKNOWN = OSPC_MPART_UNKNOWN,     /* Not a known role */
    OSPC_ROLE_UNDEFINED,
    /* Role type start */
    OSPC_ROLE_START = 0,
    OSPC_ROLE_SOURCE = OSPC_ROLE_START,
    OSPC_ROLE_DESTINATION,
    OSPC_ROLE_OTHER,
    /* Number of role types */
    OSPC_ROLE_NUMBER
} OSPE_ROLE;

typedef enum {
    OSPC_RSTATE_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_RSTATE_UNDEFINED,
    /* Role state start */
    OSPC_RSTATE_START = 0,
    OSPC_RSTATE_START1,
    OSPC_RSTATE_START2,
    OSPC_RSTATE_STOP,
    OSPC_RSTATE_STOP1,
    OSPC_RSTATE_STOP2,
    OSPC_RSTATE_INTERIM,
    OSPC_RSTATE_INTERIM1,
    OSPC_RSTATE_INTERIM2,
    OSPC_RSTATE_INTERIMFAILED,
    OSPC_RSTATE_END1,
    OSPC_RSTATE_END2,
    OSPC_RSTATE_HUNT1,
    OSPC_RSTATE_HUNT2,
    OSPC_RSTATE_MEDIA,
    OSPC_RSTATE_REDIRECT,
    OSPC_RSTATE_INVITE,
    OSPC_RSTATE_REFER,
    OSPC_RSTATE_REGISTER,
    /* Number of role states */
    OSPC_RSTATE_NUMBER
} OSPE_ROLE_STATE;

typedef enum {
    OSPC_RFORMAT_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_RFORMAT_UNDEFINED,
    /* Role format start */
    OSPC_RFORMAT_START = 0,
    OSPC_RFORMAT_OSP = OSPC_RFORMAT_START,
    OSPC_RFORMAT_RADIUS,
    OSPC_RFORMAT_DIAMETER,
    OSPC_RFORMAT_IPDR,
    OSPC_RFORMAT_CSV,
    OSPC_RFORMAT_TSV,
    OSPC_RFORMAT_ASCII,
    OSPC_RFORMAT_XML,
    OSPC_RFORMAT_FW,
    OSPC_RFORMAT_MESSAGE,
    /* Number of role formats */
    OSPC_RFORMAT_NUMBER
} OSPE_ROLE_FORMAT;

typedef enum {
    OSPC_RVENDOR_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_RVENDOR_UNDEFINED,
    /* Role vendor start */
    OSPC_RVENDOR_START = 0,
    OSPC_RVENDOR_ACME = OSPC_RVENDOR_START,
    OSPC_RVENDOR_ASTERISK,
    OSPC_RVENDOR_AUDIOCODES,
    OSPC_RVENDOR_AVAYA,
    OSPC_RVENDOR_BROADWORKS,
    OSPC_RVENDOR_CISCO,
    OSPC_RVENDOR_CUCM,
    OSPC_RVENDOR_FREESWITCH,
    OSPC_RVENDOR_GENBANDC3,
    OSPC_RVENDOR_GENBANDS3,
    OSPC_RVENDOR_KAMAILIO,
    OSPC_RVENDOR_METASPHERE,
    OSPC_RVENDOR_OPENSIPS,
    OSPC_RVENDOR_PERIMETA,
    OSPC_RVENDOR_SANSAY,
    OSPC_RVENDOR_SIP,
    OSPC_RVENDOR_SONUS,
    OSPC_RVENDOR_TAQUA,
    OSPC_RVENDOR_TELCOBRIDGES,
    /* Number of role vendors */
    OSPC_RVENDOR_NUMBER
} OSPE_ROLE_VENDOR;

typedef enum {
    OSPC_TSTATUS_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_TSTATUS_UNDEFINED,
    /* Transfer status start */
    OSPC_TSTATUS_START = 0,
    OSPC_TSTATUS_TRANSFER = OSPC_TSTATUS_START,
    OSPC_TSTATUS_TRANSFERFROM,
    OSPC_TSTATUS_TRANSFERTO,
    OSPC_TSTATUS_SRCTRANSFERFROM,
    OSPC_TSTATUS_SRCTRANSFERTO,
    OSPC_TSTATUS_DESTTRANSFERFROM,
    OSPC_TSTATUS_DESTTRANSFERTO,
   /* Number of transfer status */
    OSPC_TSTATUS_NUMBER
} OSPE_TRANSFER_STATUS;

typedef enum {
    OSPC_VSTATUS_UNKNOWN = OSPC_MPART_UNKNOWN,
    OSPC_VSTATUS_UNDEFINED,
    /* Verification status start */
    OSPC_VSTATUS_START = 0,
    OSPC_VSTATUS_NO = OSPC_VSTATUS_START,
    OSPC_VSTATUS_FAILED,
    OSPC_VSTATUS_PASSED,
   /* Number of verification status */
    OSPC_VSTATUS_NUMBER
} OSPE_VERIFICATION_STATUS;

/*
 * externally declared global variables
 */
extern const OSPT_MSG_DESC OSPV_RTYPE_DESCS[];
extern const OSPT_MSG_DESC OSPV_RSTATE_DESCS[];
extern const OSPT_MSG_DESC OSPV_RFORMAT_DESCS[];
extern const OSPT_MSG_DESC OSPV_RVENDOR_DESCS[];
extern const OSPT_MSG_DESC OSPV_RELEASE_DESCS[];
extern const OSPT_MSG_DESC OSPV_TSTATUS_DESCS[];
extern const OSPT_MSG_DESC OSPV_VSTATUS_DESCS[];

/* general constants */

#define OSPC_SIZE_E164NUM       132     /* max digits in E.164 number */
#define OSPC_SIZE_SIGNALADDR    262     /* max characters in [name]:port */
#define OSPC_SIZE_URL           512     /* max characters in URL */
#define OSPC_SIZE_ROLESTR       16      /* max characters in role string */
#define OSPC_SIZE_NORSTR        256     /* max characters in normal string */
#define OSPC_SIZE_NORID         256     /* max characters in normal ID */
#define OSPC_SIZE_CODEC         64      /* max characters in codec */
#define OSPC_SIZE_CUSTINFO      256     /* max characters in user-defined info */
#define OSPC_SIZE_HEADER        4096    /* max characters in SIP header */
#define OSPC_SIZE_STATUSHEADER  8       /* max number of SIP status headers */

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    unsigned OSPPMsgBinFromElement(OSPT_XML_ELEM *, unsigned *, unsigned char **);
    unsigned OSPPMsgBinToElement(const char *, unsigned, unsigned char *, const char *, const char *, OSPTBOOL, OSPT_XML_ELEM **);
    unsigned OSPPMsgNumFromElement(OSPT_XML_ELEM *, unsigned long *);
    unsigned OSPPMsgNumToElement(unsigned long, const char *, OSPT_XML_ELEM **);
    int OSPPMsgFloatFromElement(OSPT_XML_ELEM *, float *);
    int OSPPMsgFloatToElement(float, const char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgCodeFromElement(OSPT_XML_ELEM *, unsigned long *);
    unsigned OSPPMsgCodeToElement(unsigned long, const char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgTXToElement(OSPTTRXID, const char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgTXFromElement(OSPT_XML_ELEM *, OSPTTRXID *);
    unsigned OSPPMsgTimeToElement(OSPTTIME, const char *, OSPT_XML_ELEM **);
    unsigned OSPPMsgElemIsCritical(OSPT_XML_ELEM *);
    int OSPPBase64DecodeWrap(const unsigned char *, unsigned *, unsigned char *);
    unsigned OSPPMsgTimeFromElement(OSPT_XML_ELEM *, OSPTTIME *);
    unsigned OSPPMsgBinFromASCIIElement(unsigned char *, unsigned *, unsigned char **);
    OSPE_ROLE OSPPRoleGetPart(const char *);
    const char *OSPPRoleGetName(OSPE_ROLE);
    OSPE_ROLE_STATE OSPPRoleStateGetPart(const char *);
    const char *OSPPRoleStateGetName(OSPE_ROLE_STATE);
    OSPE_ROLE_FORMAT OSPPRoleFormatGetPart(const char *);
    const char *OSPPRoleFormatGetName(OSPE_ROLE_FORMAT);
    OSPE_ROLE_VENDOR OSPPRoleVendorGetPart(const char *);
    const char *OSPPRoleVendorGetName(OSPE_ROLE_VENDOR);
    const char *OSPPTransferStatusGetName(OSPE_TRANSFER_STATUS);
    const char *OSPPVerstatGetName(OSPE_VERIFICATION_STATUS);
    OSPE_VERIFICATION_STATUS OSPPVerstatGetPart(const char *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPMSG_H */
