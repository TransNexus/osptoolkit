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
 * osptokeninfo.h - OSP token info objects
 */

#ifndef _OSPTOKENINFO_H
#define _OSPTOKENINFO_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"

typedef struct
{
    unsigned char     ospmTokenInfoSourceNumber[OSPC_E164NUMSIZE];
    unsigned char     ospmTokenInfoDestNumber[OSPC_E164NUMSIZE];
    OSPTCALLID       *ospmTokenInfoCallId;
    OSPTTIME          ospmTokenInfoValidAfter;
    OSPTTIME          ospmTokenInfoValidUntil;
    OSPTTRXID         ospmTokenInfoTrxId;
    int               ospmTokenInfoDuration;
}
OSPTTOKENINFO;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospauthrsp.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG

#define OSPPTokenInfoSetSourceNumber(ospvTokenInfo,ospvNum) \
    OSPM_STRNCPY((ospvTokenInfo)->ospmTokenInfoSourceNumber, (ospvNum), \
    OSPC_E164NUMSIZE-1)
#define OSPPTokenInfoGetSourceNumber(ospvTokenInfo) \
    (ospvTokenInfo)?((ospvTokenInfo)->ospmTokenInfoSourceNumber):OSPC_OSNULL

#define OSPPTokenInfoSetDestNumber(ospvTokenInfo,ospvNum) \
    OSPM_STRNCPY((ospvTokenInfo)->ospmTokenInfoDestNumber, (ospvNum), \
    OSPC_E164NUMSIZE-1)
#define OSPPTokenInfoGetDestNumber(ospvTokenInfo) \
    (ospvTokenInfo)?((ospvTokenInfo)->ospmTokenInfoDestNumber):OSPC_OSNULL

#define OSPPTokenInfoHasCallId(ospvTokenInfo) \
    (ospvTokenInfo)?((ospvTokenInfo)->ospmTokenInfoCallId != OSPC_OSNULL):OSPC_FALSE

#define OSPPTokenInfoSetCallId(ospvTokenInfo,ospvCallId)  { \
    if ((ospvTokenInfo)->ospmTokenInfoCallId != OSPC_OSNULL) \
    { \
        OSPPCallIdDelete(&((ospvTokenInfo)->ospmTokenInfoCallId)); \
    } \
    (ospvTokenInfo)->ospmTokenInfoCallId = \
        OSPPCallIdNew((ospvCallId)->ospmCallIdLen, (ospvCallId)->ospmCallIdVal); \
}

#define OSPPTokenInfoGetCallId(ospvTokenInfo) \
    ((ospvTokenInfo)->ospmTokenInfoCallId)

#define OSPPTokenInfoGetCallIdValue(ospvTokenInfo) \
    (ospvTokenInfo)?OSPPCallIdGetValue(((ospvTokenInfo)->ospmTokenInfoCallId)):OSPC_OSNULL

#define OSPPTokenInfoHasValidAfter(ospvTokenInfo) \
    (ospvTokenInfo)?((ospvTokenInfo)->ospmTokenInfoValidAfter != OSPC_TIMEMIN):OSPC_FALSE
#define OSPPTokenInfoSetValidAfter(ospvTokenInfo,ospvTime) \
    (ospvTokenInfo)->ospmTokenInfoValidAfter = (ospvTime)
#define OSPPTokenInfoGetValidAfter(ospvTokenInfo) \
    (ospvTokenInfo)->ospmTokenInfoValidAfter

#define OSPPTokenInfoSetValidUntil(ospvTokenInfo,ospvTime) \
    (ospvTokenInfo)->ospmTokenInfoValidUntil = (ospvTime)
#define OSPPTokenInfoGetValidUntil(ospvTokenInfo) \
    (ospvTokenInfo)->ospmTokenInfoValidUntil

#define OSPPTokenInfoSetTrxId(ospvTokenInfo, ospvTrxId) \
    (ospvTokenInfo)->ospmTokenInfoTrxId = ospvTrxId
#define OSPPTokenInfoGetTrxId(ospvTokenInfo) \
    ((ospvTokenInfo)->ospmTokenInfoTrxId)

#define OSPPTokenInfoSetDuration(ospvTokenInfo,ospvDuration) \
    (ospvTokenInfo)->ospmTokenInfoDuration = ospvDuration; 
#define OSPPTokenInfoGetDuration(ospvTokenInfo) \
    (ospvTokenInfo)?((ospvTokenInfo)->ospmTokenInfoDuration):0

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
extern "C" 
{
#endif


    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/

    OSPTTOKENINFO *OSPPTokenInfoNew(void);
    void           OSPPTokenInfoDelete(OSPTTOKENINFO **);
    unsigned       OSPPTokenInfoFromElement(OSPTXMLELEM *, OSPTTOKENINFO **);

#ifdef OSPC_DEBUG

    void           OSPPTokenInfoSetSourceNumber(OSPTTOKENINFO *, const unsigned char *);
    unsigned char *OSPPTokenInfoGetSourceNumber(OSPTTOKENINFO *);

    void           OSPPTokenInfoSetDestNumber(OSPTTOKENINFO *, const unsigned char *);
    unsigned char *OSPPTokenInfoGetDestNumber(OSPTTOKENINFO *);

    unsigned       OSPPTokenInfoHasCallId(OSPTTOKENINFO *);
    void           OSPPTokenInfoSetCallId(OSPTTOKENINFO *, OSPTCALLID *);
    OSPTCALLID    *OSPPTokenInfoGetCallId(OSPTTOKENINFO *);
    unsigned char *OSPPTokenInfoGetCallIdValue(OSPTTOKENINFO *);

    unsigned       OSPPTokenInfoHasValidAfter(OSPTTOKENINFO *);
    void           OSPPTokenInfoSetValidAfter(OSPTTOKENINFO *, OSPTTIME);
    OSPTTIME       OSPPTokenInfoGetValidAfter(OSPTTOKENINFO *);

    void           OSPPTokenInfoSetValidUntil(OSPTTOKENINFO *, OSPTTIME);
    OSPTTIME       OSPPTokenInfoGetValidUntil(OSPTTOKENINFO *);

    void           OSPPTokenInfoSetTrxId(OSPTTOKENINFO *, OSPTTRXID);
    OSPTTRXID      OSPPTokenInfoGetTrxId(OSPTTOKENINFO *);

    void           OSPPTokenInfoSetDuration(OSPTTOKENINFO *, int);
    int            OSPPTokenInfoGetDuration(OSPTTOKENINFO *);

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* _OSPTOKENINFO_H */
