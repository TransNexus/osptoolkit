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
 *  ospauthind.h - OSP authorisation indication objects
 */

#ifndef _OSPAUTHIND_H
#define _OSPAUTHIND_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospcallid.h"
#include "osptoken.h"
#include "ospaltinfo.h"
#include "ospdest.h"


typedef struct
{
    OSPTTIME          ospmAuthIndTimestamp;
    unsigned          ospmAuthIndRole;
    unsigned          ospmAuthIndHasRole;
    OSPTCALLID       *ospmAuthIndCallId;
    unsigned char     ospmAuthIndSourceNumber[OSPC_E164NUMSIZE];
    unsigned char     ospmAuthIndDestNumber[OSPC_E164NUMSIZE];
    OSPTLIST          ospmAuthIndSourceAlternate;
    OSPTLIST          ospmAuthIndDestinationAlternate;
    OSPTLIST          ospmAuthIndTokens;
    OSPTBOOL          ospmAuthIndHasTimeLimit;
    unsigned          ospmAuthIndTimeLimit;
    OSPTDEST          *ospmAuthIndDest;
}
OSPTAUTHIND;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospauthind.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG

#define OSPPAuthIndFirstDestinationAlt(ospvAuthInd) \
    (const char *)OSPPListFirst(&((ospvAuthInd)->ospmAuthIndDestinationAlternate))

#define OSPPAuthIndFirstSourceAlt(ospvAuthInd) \
    (const char *)OSPPListFirst(&((ospvAuthInd)->ospmAuthIndSourceAlternate))

#define OSPPAuthIndGetCallId(ospvAuthInd) \
    ((ospvAuthInd)->ospmAuthIndCallId)

#define OSPPAuthIndGetCallIdSize(ospvAuthInd) \
    OSPPCallIdGetSize(((ospvAuthInd)->ospmAuthIndCallId))

#define OSPPAuthIndGetCallIdValue(ospvAuthInd) \
    OSPPCallIdGetValue(((ospvAuthInd)->ospmAuthIndCallId))

#define OSPPAuthIndGetDestinationAltValue(ospvAltInfo) \
    OSPPAltInfoGetValue(ospvAltInfo)

#define OSPPAuthIndGetDestNumber(ospvAuthInd) \
    ((ospvAuthInd)->ospmAuthIndDestNumber)

#define OSPPAuthIndGetSourceAltValue(ospvAltInfo) \
    OSPPAltInfoGetValue(ospvAltInfo)

#define OSPPAuthIndGetSourceNumber(ospvAuthInd) \
    (ospvAuthInd->ospmAuthIndSourceNumber)

#define OSPPAuthIndHasCallId(ospvAuthInd) \
    ((ospvAuthInd)->ospmAuthIndCallId != OSPC_OSNULL)

#define OSPPAuthIndHasDestinationAlt(ospvAuthInd) \
    (OSPPAuthIndFirstDestinationAlt(ospvAuthInd) != OSPC_OSNULL)

#define OSPPAuthIndHasDestNumber(ospvAuthInd) \
    ((ospvAuthInd)->ospmAuthIndDestNumber[0] != '\0')

#define OSPPAuthIndHasDest(ospvAuthInd) \
    (ospvAuthInd->ospmAuthIndDest != OSPC_OSNULL)

#define OSPPAuthIndHasSourceAlt(ospvAuthInd) \
    (OSPPAuthIndFirstSourceAlt(ospvAuthInd) != OSPC_OSNULL)

#define OSPPAuthIndHasSourceNumber(ospvAuthInd) \
    ((ospvAuthInd)->ospmAuthIndSourceNumber[0] != '\0')

#define OSPPAuthIndNextDestinationAlt(ospvAuthInd, ospvAltInfo) \
    (OSPTALTINFO *)OSPPListNext((OSPTLIST *)&((ospvAuthInd)->ospmAuthIndDestinationAlternate), \
                        (void *)ospvAltInfo)

#define OSPPAuthIndNextSourceAlt(ospvAuthInd, ospvAltInfo) \
    (OSPTALTINFO *)OSPPListNext((OSPTLIST *)&((ospvAuthInd)->ospmAuthIndSourceAlternate), \
                        (void *)ospvAltInfo)

#define OSPPAuthIndSetDest(ospvAuthInd, ospvDest)\
if ( OSPPAuthIndHasDest( ospvAuthInd ) == OSPC_TRUE )  \
{ \
	OSPPDestDelete( &(ospvAuthInd->ospmAuthIndDest ) ); \
} \
ospvAuthInd->ospmAuthIndDest = ospvDest;

#define OSPPAuthIndSetDestNumber(ospvAuthInd,ospvNum) \
    OSPM_STRNCPY((ospvAuthInd)->ospmAuthIndDestNumber, (ospvNum), \
    tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_E164NUMSIZE-1))

#define OSPPAuthIndSetRole(ospvAuthInd,ospvRole) \
    (ospvAuthInd)->ospmAuthIndRole = (ospvRole); \
    (ospvAuthInd)->ospmAuthIndHasRole = OSPC_TRUE;

#define OSPPAuthIndSetSourceNumber(ospvAuthInd,ospvNum) \
    OSPM_STRNCPY((ospvAuthInd)->ospmAuthIndSourceNumber, (ospvNum), \
    tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_E164NUMSIZE-1))

#define OSPPAuthIndSetTimestamp(ospvAuthInd,ospvTime) \
    (ospvAuthInd)->ospmAuthIndTimestamp = (ospvTime)

#endif 

#define OSPPAuthIndTokenGetSize(ospvToken) OSPPTokenGetSize(ospvToken);
#define OSPPAuthIndTokenGetValue(ospvToken) OSPPTokenGetValue(ospvToken);

/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" 
{
#endif

    void           OSPPAuthIndDelete(OSPTAUTHIND **);
    OSPTAUTHIND   *OSPPAuthIndNew(void);
    void           OSPPAuthIndSetCallId(OSPTAUTHIND *, OSPTCALLID *);


#ifdef OSPC_DEBUG

    void           OSPPAuthIndSetTimestamp(OSPTAUTHIND *, OSPTTIME);

    void           OSPPAuthIndSetRole(OSPTAUTHIND *, unsigned);

    unsigned       OSPPAuthIndHasCallId(OSPTAUTHIND *);
    OSPTCALLID    *OSPPAuthIndGetCallId(OSPTAUTHIND *);
    unsigned       OSPPAuthIndGetCallIdSize(OSPTAUTHIND *);
    unsigned char *OSPPAuthIndGetCallIdValue(OSPTAUTHIND *);

    unsigned       OSPPAuthIndHasSourceNumber(OSPTAUTHIND *ospvAuthInd);
    void           OSPPAuthIndSetSourceNumber(OSPTAUTHIND *, const unsigned char *);
    unsigned char *OSPPAuthIndGetSourceNumber(OSPTAUTHIND *);

    unsigned       OSPPAuthIndHasDestNumber(OSPTAUTHIND *ospvAuthInd);
    void           OSPPAuthIndSetDestNumber(OSPTAUTHIND *, const unsigned char *);
    unsigned char *OSPPAuthIndGetDestNumber(OSPTAUTHIND *);

    void           OSPPAuthIndSetDest(OSPTAUTHIND *, OSPTDEST *);
    unsigned       OSPPAuthIndHasDest(OSPTAUTHIND *);

    unsigned       OSPPAuthIndHasSourceAlt(OSPTAUTHIND *);
    OSPTALTINFO   *OSPPAuthIndFirstSourceAlt(OSPTAUTHIND *);
    OSPTALTINFO   *OSPPAuthIndNextSourceAlt(OSPTAUTHIND *, OSPTALTINFO  *);
    unsigned char *OSPPAuthIndGetSourceAltValue(OSPTALTINFO *);

    unsigned       OSPPAuthIndHasDestinationAlt(OSPTAUTHIND *);
    unsigned       OSPPAuthIndGetDestinationAltSize(OSPTALTINFO *);
    OSPTALTINFO   *OSPPAuthIndFirstDestinationAlt(OSPTAUTHIND *);
    OSPTALTINFO   *OSPPAuthIndNextDestinationAlt(OSPTAUTHIND *, OSPTALTINFO  *);
    unsigned char *OSPPAuthIndGetDestinationAltValue(OSPTALTINFO *);

#endif /* OSPC_DEBUG */

    void           OSPPAuthIndSetTimeLimit(OSPTAUTHIND *, unsigned);

#ifdef __cplusplus
}
#endif
#endif /* _OSPCAUTHIND_H */

