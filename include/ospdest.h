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
 * ospdest.h - OSP destination objects
 */

#ifndef _OSPDEST_H
#define _OSPDEST_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"


typedef struct
{
    OSPTLISTLINK      ospmDestLink;
    unsigned char     ospmDestNumber[OSPC_E164NUMSIZE];
    unsigned char     ospmDestAddr[OSPC_SIGNALADDRSIZE];
    unsigned char     ospmDestDevAddr[OSPC_SIGNALADDRSIZE];
    OSPTTIME          ospmDestValidAfter;
    OSPTTIME          ospmDestValidUntil;
    OSPTLIST          ospmDestTokens;
    unsigned char     ospmDestAuthority[OSPC_URLSIZE];
    unsigned          ospmDestHasLimit;
    unsigned          ospmDestLimit;
    OSPTCALLID       *ospmDestCallId;
    unsigned          ospmDestTNFailReason;
    unsigned          ospmDestTNFailReasonInd;

}
OSPTDEST;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/
#ifndef OSPC_DEBUG
/*
 * Note: all macros are also implemented as functions in ospmsgdest.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

#define OSPPDestHasNumber(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestNumber[0] != '\0'):OSPC_FALSE
#define OSPPDestSetNumber(ospvDest,ospvNum) \
            OSPM_MEMCPY((ospvDest)->ospmDestNumber, \
                (ospvNum), \
                min(OSPC_E164NUMSIZE,OSPM_STRLEN((const char *)ospvNum)+1))
#define OSPPDestGetNumber(ospvDest) (ospvDest)?((ospvDest)->ospmDestNumber):OSPC_OSNULL

#define OSPPDestHasAddr(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestAddr[0] != '\0'):OSPC_FALSE
#define OSPPDestSetAddr(ospvDest,ospvAddr) \
            OSPM_MEMCPY((ospvDest)->ospmDestAddr, (ospvAddr), \
                min(OSPC_SIGNALADDRSIZE,OSPM_STRLEN((const char *)ospvAddr)+1))
#define OSPPDestGetAddr(ospvDest) (ospvDest)?((ospvDest)->ospmDestAddr):OSPC_OSNULL

#define OSPPDestDevHasAddr(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestDevAddr[0] != '\0'):OSPC_FALSE
#define OSPPDestDevSetAddr(ospvDest,ospvAddr) \
            OSPM_MEMCPY((ospvDest)->ospmDestDevAddr, (ospvAddr), \
                min(OSPC_SIGNALADDRSIZE,OSPM_STRLEN((const char *)ospvAddr)+1))
#define OSPPDestDevGetAddr(ospvDest) (ospvDest)?((ospvDest)->ospmDestDevAddr):OSPC_OSNULL


#define OSPPDestHasValidAfter(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestValidAfter != OSPC_TIMEMIN):OSPC_FALSE
#define OSPPDestSetValidAfter(ospvDest,ospvTime) \
    (ospvDest)->ospmDestValidAfter = (ospvTime)
#define OSPPDestGetValidAfter(ospvDest) (ospvDest)->ospmDestValidAfter

#define OSPPDestHasValidUntil(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestValidUntil != OSPC_TIMEMAX):OSPC_FALSE
#define OSPPDestSetValidUntil(ospvDest,ospvTime) \
    (ospvDest)->ospmDestValidUntil = (ospvTime)
#define OSPPDestGetValidUntil(ospvDest) (ospvDest)->ospmDestValidUntil

#define OSPPDestHasAuthority(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestAuthority[0] != '\0'):OSPC_FALSE
#define OSPPDestSetAuthority(ospvDest,ospvAuth) \
    OSPM_STRNCPY((char *)(ospvDest)->ospmDestAuthority, (ospvAuth), \
    min(OSPM_STRLEN((const char *)ospvAuth)+1,OSPC_URLSIZE-1))

#define OSPPDestHasCallId(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestCallId != OSPC_OSNULL):OSPC_FALSE
#define OSPPDestGetCallId(ospvDest) ((ospvDest)->ospmDestCallId)

#define OSPPDestHasToken(ospvDest) (ospvDest)?(OSPPListFirst(&(ospvDest)->ospmDestTokens) != OSPC_OSNULL):OSPC_FALSE
#define OSPPDestAddToken(ospvDest, ospvToken)   \
    OSPPListAppend(&(ospvDest)->ospmDestTokens, (ospvToken))
#define OSPPDestFirstToken(ospvDest) \
    OSPPListFirst(&(ospvDest)->ospmDestTokens)
#define OSPPDestNextToken(ospvDest, ospvToken) \
    OSPPListNext(&(ospvDest)->ospmDestTokens, (ospvToken))

#define OSPPDestHasLimit(ospvDest) (ospvDest)?(((OSPTDEST *)ospvDest)->ospmDestHasLimit):OSPC_FALSE
#define OSPPDestGetLimit(ospvDest) (ospvDest)?(((OSPTDEST *)ospvDest)->ospmDestLimit):0
#define OSPPDestSetLimit(ospvDest, ospvLimit) \
    { ((OSPTDEST *)ospvDest)->ospmDestLimit=ospvLimit; \
    ((OSPTDEST *)ospvDest)->ospmDestHasLimit=OSPC_TRUE; }


/* Failure Reason */
#define OSPPDestHasTNFailReason(ospvDest) \
    (ospvDest)?((ospvDest)->ospmDestTNFailReasonInd):OSPC_FALSE
#define OSPPDestSetTNFailReason(ospvDest,ospvTNFailReason) { \
    (ospvDest)->ospmDestTNFailReason = (ospvTNFailReason); \
    (ospvDest)->ospmDestTNFailReasonInd = 1; \
    }
#define OSPPDestGetTNFailReason(ospvDest) \
(ospvDest)?(ospvDest)->ospmDestTNFailReason:0

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
extern "C" 
{
#endif

    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/
    OSPTDEST      *OSPPDestNew(void);
    void           OSPPDestDelete(OSPTDEST **);
    unsigned       OSPPDestFromElement(OSPTXMLELEM *, OSPTDEST **);
    void           OSPPDestSetCallId(OSPTDEST *, const unsigned char *, unsigned);

#ifdef OSPC_DEBUG
    unsigned       OSPPDestHasNumber(OSPTDEST *ospvDest);
    void           OSPPDestSetNumber(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestGetNumber(OSPTDEST *);

    unsigned       OSPPDestHasAddr(OSPTDEST *);
    void           OSPPDestSetAddr(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestGetAddr(OSPTDEST *);

    unsigned       OSPPDestDevHasAddr(OSPTDEST *);
    void           OSPPDestDevSetAddr(OSPTDEST *, const unsigned char *);
    unsigned char *OSPPDestDevGetAddr(OSPTDEST *);

    unsigned       OSPPDestHasValidAfter(OSPTDEST *);
    void           OSPPDestSetValidAfter(OSPTDEST *, OSPTTIME);
    OSPTTIME       OSPPDestGetValidAfter(OSPTDEST *);

    unsigned       OSPPDestHasValidUntil(OSPTDEST *);
    void           OSPPDestSetValidUntil(OSPTDEST *, OSPTTIME);
    OSPTTIME       OSPPDestGetValidUntil(OSPTDEST *);

    unsigned       OSPPDestHasAuthority(OSPTDEST *);
    void           OSPPDestSetAuthority(OSPTDEST *, const unsigned char *);

    unsigned       OSPPDestHasCallId(OSPTDEST *);
    OSPTCALLID    *OSPPDestGetCallId(OSPTDEST *);

    unsigned       OSPPDestHasToken(OSPTDEST *);
    void           OSPPDestAddToken(OSPTDEST *, OSPTTOKEN *);
    OSPTTOKEN     *OSPPDestFirstToken(OSPTDEST *);
    OSPTTOKEN     *OSPPDestNextToken(OSPTDEST *, OSPTTOKEN *);

    unsigned       OSPPDestHasLimit(OSPTDEST *);
    unsigned       OSPPDestGetLimit(OSPTDEST *);
    void           OSPPDestSetLimit(OSPTDEST *, unsigned);

    unsigned       OSPPDestHasTNFailReason(OSPTDEST *);
    void           OSPPDestSetTNFailReason(OSPTDEST *, unsigned);
    unsigned       OSPPDestGetTNFailReason(OSPTDEST *);

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* _OSPDEST_H */

