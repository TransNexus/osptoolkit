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

/* osptoken.h - OSP token objects */
#ifndef _OSPTOKEN_H
#define _OSPTOKEN_H

#include "osp/osp.h"
#include "osp/osplist.h"
#include "osp/ospxmlelem.h"

/* max size of a token (bytes) */
#define OSPC_TOKENMAXSIZE           3200
#define OSPC_TOKENMAXSTRINGSIZE     6 * OSPC_TOKENMAXSIZE

/* the basic token structure */
typedef struct {
    OSPTLISTLINK Link;
    unsigned Length;
    unsigned char *Value;
} OSPT_TOKEN;

typedef enum {
    TOKEN_ALGO_SIGNED,
    TOKEN_ALGO_UNSIGNED,
    TOKEN_ALGO_BOTH
} token_algo_t;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPT_TOKEN *OSPPTokenNew(unsigned, const unsigned char *);
    unsigned OSPPTokenFromElement(OSPT_XML_ELEM *, OSPT_TOKEN **);
    unsigned OSPPTokenGetSize(OSPT_TOKEN *);
    const unsigned char *OSPPTokenGetValue(OSPT_TOKEN *);
    void OSPPTokenDelete(OSPT_TOKEN **);
    unsigned OSPPTokenToElement(OSPT_TOKEN *, OSPT_XML_ELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPTOKEN_H */
