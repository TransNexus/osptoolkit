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


#ifdef __cplusplus
}
#endif

#endif /* _OSPTOKENINFO_H */
