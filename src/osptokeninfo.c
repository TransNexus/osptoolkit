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
 * osptokeninfo.c - OSP token info functions
 */

#include "osp.h"
#include "osperrno.h"
#include "ospbfr.h"
#include "osplist.h"
#include "ospxmlattr.h"
#include "ospxmlelem.h"
#include "ospmsgattr.h"
#include "ospmsgelem.h"
#include "osptokeninfo.h"
#include "ospusage.h"
#include "osputils.h"


/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoSetSourceNumber() - set the source number
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPTokenInfoSetSourceNumber(
    OSPTTOKENINFO *ospvTokenInfo,     /* token info to set */
    const unsigned char *ospvSourceNumber /* source number to set to */
)
{
    if (ospvTokenInfo != OSPC_OSNULL)
    {
        if (ospvSourceNumber  != OSPC_OSNULL)
        {
            OSPM_STRNCPY((ospvTokenInfo)->ospmTokenInfoSourceNumber, (ospvSourceNumber), OSPC_E164NUMSIZE-1);
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetSourceNumber() - returns the source number for token info
 *-----------------------------------------------------------------------*/
unsigned char *
OSPPTokenInfoGetSourceNumber(
    OSPTTOKENINFO *ospvTokenInfo                     /* token info */
)
{
    unsigned char *ospvSourceNumber = OSPC_OSNULL;
    if (ospvTokenInfo != OSPC_OSNULL)
    {
        ospvSourceNumber = ospvTokenInfo->ospmTokenInfoSourceNumber;
    }
    return ospvSourceNumber;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoSetDestNumber() - set the destination number
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPTokenInfoSetDestNumber(
    OSPTTOKENINFO *ospvTokenInfo,     /* token info to set */
    const unsigned char *ospvDestNumber /* destination number to set to */
)
{
    if (ospvTokenInfo != OSPC_OSNULL)
    {
        if (ospvDestNumber != OSPC_OSNULL)
        {
            OSPM_STRNCPY((ospvTokenInfo)->ospmTokenInfoDestNumber, (ospvDestNumber), OSPC_E164NUMSIZE-1);
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetDestNumber() - returns the destination number for a token info
 *-----------------------------------------------------------------------*/
unsigned char *
OSPPTokenInfoGetDestNumber(
    OSPTTOKENINFO *ospvTokenInfo                     /* token info */
)
{
    unsigned char *ospvDestNumber = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL)
    {
        ospvDestNumber = ospvTokenInfo->ospmTokenInfoDestNumber;
    }
    return ospvDestNumber;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoHasCallId() - is the call id set ?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPTokenInfoHasCallId(
    OSPTTOKENINFO *ospvTokenInfo              /* Token Info effected */
)
{
    unsigned ospvHasCallId = OSPC_FALSE;

    if (ospvTokenInfo != OSPC_OSNULL) 
    {
        ospvHasCallId = ((ospvTokenInfo)->ospmTokenInfoCallId != OSPC_OSNULL);
    }
    return ospvHasCallId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoSetCallId() - set the call id
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPTokenInfoSetCallId(
    OSPTTOKENINFO *ospvTokenInfo,     /* token info to set */
    OSPTCALLID *ospvCallId          /* call id to set to */
)
{
    if (ospvTokenInfo   != OSPC_OSNULL) 
    {
        if ((ospvCallId) != OSPC_OSNULL)
        {
            if ((ospvTokenInfo)->ospmTokenInfoCallId != OSPC_OSNULL)
            {
                OSPPCallIdDelete(&((ospvTokenInfo)->ospmTokenInfoCallId));
            }
            (ospvTokenInfo)->ospmTokenInfoCallId = 
                OSPPCallIdNew((ospvCallId)->ospmCallIdLen, (ospvCallId)->ospmCallIdVal); 
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetCallId() - returns the call id for a token info
 *-----------------------------------------------------------------------*/
OSPTCALLID *
    OSPPTokenInfoGetCallId(
    OSPTTOKENINFO *ospvTokenInfo                     /* token info */
    )
{
    OSPTCALLID *ospvCallId = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL) 
    {
        ospvCallId = ospvTokenInfo->ospmTokenInfoCallId;
    }
    return ospvCallId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetCallIdValue() - returns the call id for a token info
 *-----------------------------------------------------------------------*/
unsigned char *
    OSPPTokenInfoGetCallIdValue(
    OSPTTOKENINFO *ospvTokenInfo                     /* token info */
    )
{
    unsigned char *ospvCallIdValue = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL)
    {
        ospvCallIdValue = OSPPCallIdGetValue(((ospvTokenInfo)->ospmTokenInfoCallId));
    }
    return(ospvCallIdValue);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoHasValidAfter() - Does TokenInfo have a valid after time?
 *-----------------------------------------------------------------------*/

unsigned                                   /* returns non-zero if time */
OSPPTokenInfoHasValidAfter(
    OSPTTOKENINFO *ospvTokenInfo                     /* TokenInfo in question */
)
{
    unsigned ospvHasValidAfter = OSPC_FALSE;

    if (ospvTokenInfo != OSPC_OSNULL)
    {
        ospvHasValidAfter = ((ospvTokenInfo)->ospmTokenInfoValidAfter != OSPC_TIMEMIN);
    }
    return(ospvHasValidAfter);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoSetValidAfter() - sets the valid after time
 *-----------------------------------------------------------------------*/

void                                       /* nothing returned */
OSPPTokenInfoSetValidAfter(
    OSPTTOKENINFO *ospvTokenInfo,
    OSPTTIME  ospvTime
)
{
    if (ospvTokenInfo != OSPC_OSNULL)
    {
        (ospvTokenInfo)->ospmTokenInfoValidAfter = (ospvTime);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetValidAfter() - returns valid after time
 *-----------------------------------------------------------------------*/

OSPTTIME                                   /* returns the time value */
    OSPPTokenInfoGetValidAfter(
    OSPTTOKENINFO *ospvTokenInfo           /* TokenInfo in question */
    )
{
    OSPTTIME ospvValidAfter = 0;

    if (ospvTokenInfo != OSPC_OSNULL)
    {
        ospvValidAfter = (ospvTokenInfo)->ospmTokenInfoValidAfter;
    }
    return(ospvValidAfter);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoSetValidUntil() - sets valid until time
 *-----------------------------------------------------------------------*/

void                                       /* nothing returned */
OSPPTokenInfoSetValidUntil(
    OSPTTOKENINFO *ospvTokenInfo,                    /* TokenInfo in question */
    OSPTTIME ospvTime                      /* time to set */
)
{
    if (ospvTokenInfo != OSPC_OSNULL)
    {
        (ospvTokenInfo)->ospmTokenInfoValidUntil = (ospvTime);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetValidUntil() - returns valid until time
 *-----------------------------------------------------------------------*/

OSPTTIME                                   /* returns time */
    OSPPTokenInfoGetValidUntil(
    OSPTTOKENINFO *ospvTokenInfo                     /* token in question */
    )
{
    OSPTTIME ospvValidUntil = 0;

    if(ospvTokenInfo != OSPC_OSNULL)
    {
        ospvValidUntil = (ospvTokenInfo)->ospmTokenInfoValidUntil;
    }
    return(ospvValidUntil);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoSetTrxId() - set the transaction id
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPTokenInfoSetTrxId(
    OSPTTOKENINFO *ospvTokenInfo,     /* token info to set */
    OSPTTRXID ospvTrxId    /* transaction id to set to */
)
{
    if (ospvTokenInfo != OSPC_OSNULL) 
    {
        if (ospvTrxId  != (OSPTTRXID)OSPC_OSNULL) 
        {
            ospvTokenInfo->ospmTokenInfoTrxId = ospvTrxId;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetTrxId() - returns the trans id for a token info
 *-----------------------------------------------------------------------*/
OSPTTRXID
    OSPPTokenInfoGetTrxId(
    OSPTTOKENINFO *ospvTokenInfo                     /* token info */
    )
{
    OSPTTRXID ospvTrxId = 0;
    if (ospvTokenInfo != OSPC_OSNULL) 
    {
        ospvTrxId = ospvTokenInfo->ospmTokenInfoTrxId;
    }
    return ospvTrxId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoSetDuration() - set the duration
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPTokenInfoSetDuration(
    OSPTTOKENINFO *ospvTokenInfo,     /* token info to set */
    int ospvDuration /* duration to set to */
)
{
    if (ospvTokenInfo != OSPC_OSNULL)
    {
        if (ospvDuration  >= 0)
        {
            ospvTokenInfo->ospmTokenInfoDuration = ospvDuration;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoGetDuration() - returns the duration for a token info
 *-----------------------------------------------------------------------*/
int
OSPPTokenInfoGetDuration(
    OSPTTOKENINFO *ospvTokenInfo                     /* token info */
)
{
    int ospvDuration = 0;

    if (ospvTokenInfo != OSPC_OSNULL)
    {
        ospvDuration = ospvTokenInfo->ospmTokenInfoDuration;
    }
    return ospvDuration;
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoNew() - creates a new (empty) token info object
 *-----------------------------------------------------------------------*/
OSPTTOKENINFO *                                 /* returns pointer or NULL */
    OSPPTokenInfoNew()
{
    OSPTTOKENINFO *ospvTokenInfo;

    OSPM_MALLOC(ospvTokenInfo, OSPTTOKENINFO, sizeof(OSPTTOKENINFO));
    if (ospvTokenInfo != OSPC_OSNULL)
    {
        ospvTokenInfo->ospmTokenInfoSourceNumber[0] = '\0';
        ospvTokenInfo->ospmTokenInfoDestNumber[0] = '\0';
        ospvTokenInfo->ospmTokenInfoCallId = (OSPTCALLID *)OSPC_OSNULL;
        ospvTokenInfo->ospmTokenInfoValidAfter = OSPC_TIMEMIN;
        ospvTokenInfo->ospmTokenInfoValidUntil = OSPC_TIMEMAX;
        ospvTokenInfo->ospmTokenInfoTrxId = (OSPTTRXID)OSPC_OSNULL;
        ospvTokenInfo->ospmTokenInfoDuration = -1;
    }

    return ospvTokenInfo;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoDelete() - deletes a token info object
 *-----------------------------------------------------------------------*/
void
OSPPTokenInfoDelete(OSPTTOKENINFO **ospvTokenInfo)
{
    if (*ospvTokenInfo)
    {
        if (OSPPTokenInfoHasCallId(*ospvTokenInfo))
            OSPPCallIdDelete(&((*ospvTokenInfo)->ospmTokenInfoCallId));
        OSPM_FREE(*ospvTokenInfo);
        *ospvTokenInfo = OSPC_OSNULL;
    }
    return;

}

/**/
/*-----------------------------------------------------------------------*
 * OSPPTokenInfoFromElement() - get token info from an XML element
 *-----------------------------------------------------------------------*/

unsigned                          /* returns error code */
OSPPTokenInfoFromElement(
    OSPTXMLELEM  *ospvElem,       /* input is XML element */
    OSPTTOKENINFO **ospvTokenInfo   /* where to put token info pointer */
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM  *elem = OSPC_OSNULL;
    OSPTTOKENINFO *tokeninfo = OSPC_OSNULL;
    OSPTCALLID   *callId;
    OSPTTIME      t;
    OSPTTRXID     trxid;
    unsigned      duration;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvTokenInfo == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_TOKEN;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* create the token info object */
        tokeninfo = OSPPTokenInfoNew();

        if (tokeninfo == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_DATA_NO_TOKENINFO;
        }
    }
    /*
     * The Token Info element should consist of several child
     * elements. We'll run through what's there and pick out
     * the information we need.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        for ( elem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != (OSPTXMLELEM *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPTXMLELEM *)OSPPXMLElemNextChild(ospvElem, elem) )
        {
            switch (OSPPMsgGetElemPart(OSPPXMLElemGetName(elem)))
            {
                case ospeElemMessage:
                break;

                case ospeElemTokenInfo:
                break;

                case ospeElemSrcInfo:
                OSPPTokenInfoSetSourceNumber(tokeninfo, (const unsigned char *)OSPPXMLElemGetValue(elem));
                break;

                case ospeElemDestInfo:
                OSPPTokenInfoSetDestNumber(tokeninfo, (const unsigned char *)OSPPXMLElemGetValue(elem));
                break;

                case ospeElemCallId:
                ospvErrCode = OSPPCallIdFromElement(elem, &callId);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPTokenInfoSetCallId(tokeninfo, callId);
                    OSPPCallIdDelete(&callId);
                }
                break;

                case ospeElemTransId:
                ospvErrCode = OSPPMsgTXFromElement(elem, &trxid);
                OSPPTokenInfoSetTrxId(tokeninfo, trxid);
                break;

                case ospeElemValidAfter:
                ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPTokenInfoSetValidAfter(tokeninfo,t);
                }
                break;

                case ospeElemValidUntil:
                ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPTokenInfoSetValidUntil(tokeninfo,t);
                }
                break;

                case ospeElemUsageDetail:
                OSPPUsageFromElement(elem, &duration);
                OSPPTokenInfoSetDuration(tokeninfo, (int)duration);
                break;

                default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem))
                {
                    ospvErrCode = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        *ospvTokenInfo = tokeninfo;
    }

    return ospvErrCode;
}

