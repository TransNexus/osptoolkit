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
 * osptoken.c - OSP token functions
 */

#include "osp.h"
#include "osperrno.h"
#include "osplist.h"
#include "ospxmldoc.h"
#include "ospxmlattr.h"
#include "ospxmlelem.h"
#include "ospmsgattr.h"
#include "ospmsgelem.h"
#include "osptoken.h"
#include "ospmsg.h"


/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenNew() - create a new token object
 *-----------------------------------------------------------------------*/

OSPTTOKEN *                           /* returns ptr to token or null */
    OSPPTokenNew(
    unsigned             ospvLen,      /* size of token */
    const unsigned char *ospvValue     /* token value */
    )
{
    OSPTTOKEN     *ospvToken = OSPC_OSNULL;
    unsigned char *valptr;

    /*
     * Token objects are actually two parts -- the first is the Token
     * structure and the second is the token value. The two parts are
     * contiguous in memory, and are created (and destroyed) at the same
     * time. Here's a rough picture:
     *
     *    Memory:
     *      +--------+
     *      |        |  This part is the structure itself. It's actually
     *      |        |  visible to the functions (and macros).
     *      +--------+
     *      |        |
     *      |        |  This part contains the actual value. It's not directly
     *      |        |  accessible, but pointers to the value within
     *      |        |  the structure are set to point to it.
     *      +--------+
     *
     * We do it this way to conveniently manage variable size tokens
     * If we put the value in a visible part of the structure,
     * then we'd have to define that field, and that would require
     * defining a size for it. Since we can't predict the size of
     * token values , picking the size for the structure would be pretty
     * difficult.
     *
     * Note that this technique does use dynamic memory allocation.
     * If memory fragmentation is a concern, one possible optimization
     * would be to define a pool of Token objects in which the name and
     * value sizes were fixed to some reasonable size. Then you could
     * allocate from the pool in most cases, and fall back to this
     * approach only if the name/value was too big for the pool objects
     * (or, perhaps, if the pool was empty).
     */

    if (ospvLen > 0)
    {
        if (ospvValue != OSPC_OSNULL)
        {

            /* try to allocate the memory for the entire object */
            OSPM_MALLOC(ospvToken, OSPTTOKEN,sizeof(OSPTTOKEN) + ospvLen + 1);

            /* make sure the allocation succeeded before proceeding */
            if (ospvToken != OSPC_OSNULL)
            {
                /* calculate where the "hidden" values will go */
                valptr = ((unsigned char *)ospvToken) + sizeof(OSPTTOKEN);

                /* copy the values into their hidden location */
                OSPM_MEMCPY(valptr, ospvValue, ospvLen);

                /* fill in the structure fields */
                OSPPListLinkNew(&(ospvToken->ospmTokenLink));
                ospvToken->ospmTokenLen = ospvLen;
                ospvToken->ospmTokenVal = valptr;
                ospvToken->ospmTokenVal[ospvLen] = '\0';
            }
        }
    }

    return(ospvToken);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenFromElement() - create a token from an XML element
 *-----------------------------------------------------------------------*/
unsigned                          /* returns error code */
OSPPTokenFromElement(
    OSPTXMLELEM *ospvElem,        /* input is XML element */
    OSPTTOKEN  **ospvToken        /* where to put token pointer */
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char *tokenValue = OSPC_OSNULL;
    unsigned      tokenLen = 0;

    /* start by assuming we will fail */
    *ospvToken = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvToken == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_TOKEN;
    }
    if (OSPPMsgGetElemPart(OSPPXMLElemGetName(ospvElem)) !=
        ospeElemToken) 
    {
        ospvErrCode = OSPC_ERR_DATA_INVALID_TYPE;
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        ospvErrCode = OSPPMsgBinFromElement(ospvElem, &tokenLen, &tokenValue);
    }
    /* create the Token structure */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        *ospvToken = OSPPTokenNew(tokenLen, tokenValue);
        if (*ospvToken == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_DATA_NO_TOKEN;
        }
    }

    if(tokenValue != OSPC_OSNULL)
    {
        OSPM_FREE(tokenValue);
    }


    return(ospvErrCode);
}

#ifdef OSPC_DEBUG
/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenDelete() - destroy a token object
 *-----------------------------------------------------------------------*/
void                                  /* no return */
OSPPTokenDelete(
    OSPTTOKEN **ospvToken            /* Token to destroy */
)
{
    if (*ospvToken != OSPC_OSNULL)
    {
        OSPM_FREE(*ospvToken);
        *ospvToken = OSPC_OSNULL;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenGetSize() - returns size of token value
 *-----------------------------------------------------------------------*/
unsigned
OSPPTokenGetSize(
    OSPTTOKEN *ospvToken
)
{
    unsigned ospvSize = 0;
    if (ospvToken != OSPC_OSNULL)
    {
        ospvSize = ospvToken->ospmTokenLen;
    }
    return(ospvSize);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenGetValue() - returns pointer to token value
 *-----------------------------------------------------------------------*/
const unsigned char *
OSPPTokenGetValue(
    OSPTTOKEN *ospvToken
)
{
    const unsigned char *ospvVal = OSPC_OSNULL;
    if (ospvToken != OSPC_OSNULL)
    {
        ospvVal = ospvToken->ospmTokenVal;
    }
    return(ospvVal);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenToElement() - create an XML element from a token
 *-----------------------------------------------------------------------*/
unsigned                           /* returns error code */
OSPPTokenToElement(
    OSPTTOKEN    *ospvToken,       /* token */
    OSPTXMLELEM **ospvElem         /* where to put XML element pointer */
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvToken == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_TOKEN;
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        ospvErrCode = OSPPMsgBinToElement(OSPPTokenGetSize(ospvToken),
            (unsigned char *) OSPPTokenGetValue(ospvToken),
            (const unsigned char *)OSPPMsgGetElemName(ospeElemToken),
            ospvElem, OSPC_TRUE);
    }
    return(ospvErrCode);
}
#endif

