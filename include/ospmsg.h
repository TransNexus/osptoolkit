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

#include "osp.h"
#include "ospostime.h"
#include "osplist.h"

typedef enum {
    OSPC_MSG_LOWER_BOUND=10,
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
    OSPC_MSG_UPPER_BOUND
} OSPE_MSG_DATATYPES;

typedef enum {
    OSPC_UNDEFINED_ROLE=0,      /* Not a known role */
    OSPC_DESTINATION,
    OSPC_SOURCE,   
    OSPC_OTHER
} OSPE_MSG_ROLETYPES;


/* general constants */

#define OSPC_E164NUMSIZE      132    /* max digits in E.164 number */
#define OSPC_SIGNALADDRSIZE   262    /* max characters in [name]:port */
#define OSPC_URLSIZE          512    /* max characters in URL */
#define OSPC_ROLESTRSZ         12    /* max characters in role string */

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/
#ifndef OSPC_DEBUG
/*
 * Note: all macros are also implemented as functions in ospmsg.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

#define OSPPMsgTimeFromElement(ospvElem, ospvTime) \
    OSPPOSTimeStringToCal(OSPPXMLElemGetValue(ospvElem), (ospvTime))
#endif


/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" 
{
#endif

unsigned OSPPMsgBinFromElement(OSPTXMLELEM *, unsigned *, unsigned char **);
unsigned OSPPMsgBinToElement(unsigned, unsigned char *,
             const unsigned char *, OSPTXMLELEM **, OSPTBOOL);

unsigned OSPPMsgNumFromElement(OSPTXMLELEM *, unsigned long *);
unsigned OSPPMsgNumToElement(unsigned long, const unsigned char *,
             OSPTXMLELEM **);

int     OSPPMsgFloatFromElement(OSPTXMLELEM *, float *);
int     OSPPMsgFloatToElement(float, const unsigned char *, OSPTXMLELEM **);

unsigned OSPPMsgCodeFromElement(OSPTXMLELEM *, unsigned long *);
unsigned OSPPMsgCodeToElement(unsigned long, const unsigned char *,
             OSPTXMLELEM **);

unsigned OSPPMsgTXToElement(OSPTTRXID, const unsigned char *, OSPTXMLELEM **);
unsigned OSPPMsgTXFromElement(OSPTXMLELEM *, OSPTTRXID *);

unsigned OSPPMsgTimeToElement(OSPTTIME, const unsigned char *,
             OSPTXMLELEM **);
unsigned OSPPMsgElemIsCritical(OSPTXMLELEM *);

unsigned OSPPMsgRoleToElement(unsigned, const unsigned char *, OSPTXMLELEM **);

int OSPPOSRoleValToString(unsigned, char *ospvRolestr);

int OSPPBase64DecodeWrap(const unsigned char *, unsigned *, unsigned char *);

#ifdef OSPC_DEBUG
unsigned OSPPMsgTimeFromElement(OSPTXMLELEM *, OSPTTIME *);
#endif

#ifdef __cplusplus
}
#endif

#endif

