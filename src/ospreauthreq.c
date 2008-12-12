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
 * ospreauthreq.c - OSP authorisation request functions
 */

#include <assert.h>

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospcallid.h"
#include "osp/osptoken.h"
#include "osp/ospdest.h"
#include "osp/ospusage.h"
#include "osp/ospreauthreq.h"
#include "osp/ospcallid.h"
#include "osp/osputils.h"
#include "osp/osptrans.h"

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasTimestamp() - Does authorisation request have a valid timestamp?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPReauthReqHasTimestamp(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request in question */
)
{
    unsigned ospvHasTime = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasTime = (ospvReauthReq->ospmReauthReqTimestamp != OSPC_TIMEMIN);
    }
    return(ospvHasTime);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetTimestamp() - sets the timestamp for an authorisation request
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqSetTimestamp(
    OSPTREAUTHREQ *ospvReauthReq,
    OSPTTIME  ospvTime
)
{
    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvReauthReq->ospmReauthReqTimestamp = ospvTime;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetTimestamp() - returns timestamp for an authorisation request
 *-----------------------------------------------------------------------*/
OSPTTIME                                   /* returns the time value */
    OSPPReauthReqGetTimestamp(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request */
    )
{
    OSPTTIME ospvTime = 0;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvTime = ospvReauthReq->ospmReauthReqTimestamp;
    }
    return(ospvTime);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasCallId() - does an authorisation request have a Call ID?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPReauthReqHasCallId(
    OSPTREAUTHREQ *ospvReauthReq                  /* authorisation request */
)
{
    unsigned ospvHasId = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasId = (ospvReauthReq->ospmReauthReqCallId != OSPC_OSNULL);
    }
    return(ospvHasId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetCallId() - gets the call ID for an authorisation request
 *-----------------------------------------------------------------------*/
OSPTCALLID *                               /* returns call ID pointer */
    OSPPReauthReqGetCallId(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request */
    )
{
    OSPTCALLID *ospvCallId = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvCallId = ospvReauthReq->ospmReauthReqCallId;
    }
    return(ospvCallId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetSourceNumber() - set the source number for an
 * authorisation request
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqSetSourceNumber(
    OSPTREAUTHREQ         *ospvReauthReq,      /* authorisation request  to set */
    const char *ospvNum           /* source number (as string) */
)
{
    if (ospvReauthReq != OSPC_OSNULL)
    {
        if (ospvNum  != OSPC_OSNULL)
        {
            if (OSPM_STRLEN(ospvNum) < OSPC_SIZE_E164NUM)
            {
                OSPM_STRNCPY(ospvReauthReq->ospmReauthReqSourceNumber, ospvNum, OSPC_SIZE_E164NUM - 1);
            }
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetSourceNumber() - returns the source number for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
const unsigned char *                      /* returns number as string */
OSPPReauthReqGetSourceNumber(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request */
)
{
    const unsigned char *ospvNum = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvNum = ospvReauthReq->ospmReauthReqSourceNumber;
    }
    return(ospvNum);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetDestNumber() - set the destination number for an
 * authorisation request
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqSetDestNumber(
    OSPTREAUTHREQ         *ospvReauthReq,      /* authorisation request to set */
    const char *ospvNum           /* destination number (as string) */
)
{
    if (ospvReauthReq != OSPC_OSNULL)
    {
        if (ospvNum  != OSPC_OSNULL)
        {
            if (OSPM_STRLEN(ospvNum) < OSPC_SIZE_E164NUM)
            {
                OSPM_STRNCPY(ospvReauthReq->ospmReauthReqDestNumber, ospvNum, OSPC_SIZE_E164NUM - 1);
            }
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetDestNumber() - returns the destination number for an
 * authorisation request
 *-----------------------------------------------------------------------*/
const unsigned char *                      /* returns number as string */
OSPPReauthReqGetDestNumber(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request */
)
{
    const unsigned char *ospvNum = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvNum = ospvReauthReq->ospmReauthReqDestNumber;
    }
    return(ospvNum);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasTrxId() - does an authorisation request have a Transaction ID?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPReauthReqHasTrxId(
    OSPTREAUTHREQ *ospvReauthReq                  /* authorisation request */
)
{
    unsigned ospvHasId = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasId = (ospvReauthReq->ospmReauthReqTrxId != 0);
    }
    return(ospvHasId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetTrxId() - sets the transaction ID for an authorisation
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqSetTrxId(
    OSPTREAUTHREQ   *ospvReauthReq,            /* authorisation request */
    OSPTTRXID ospvTrxId                 /* transaction ID to set */
)
{

    if (ospvReauthReq   != OSPC_OSNULL)
    {
        ospvReauthReq->ospmReauthReqTrxId = ospvTrxId;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetTrxId() - gets the transaction ID for an authorisation request
 *-----------------------------------------------------------------------*/
OSPTTRXID                                /* returns transaction ID pointer */
    OSPPReauthReqGetTrxId(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request */
    )
{
    OSPTTRXID ospvTrxId = 0;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvTrxId = ospvReauthReq->ospmReauthReqTrxId;
    }
    return(ospvTrxId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasDuration() - is the duration set ?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPReauthReqHasDuration(
    OSPTREAUTHREQ *ospvReauthReq              /* Usage Indication effected */
)
{
    unsigned ospvHasDuration = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasDuration = (ospvReauthReq->ospmReauthReqDuration >= 0);
    }
    return ospvHasDuration;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetDuration() - set the duration
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPReauthReqSetDuration(
    OSPTREAUTHREQ *ospvReauthReq,     /* usage indication to set */
    int ospvDuration /* duration to set to */
)
{
    if (ospvReauthReq != OSPC_OSNULL)
    {
        if (ospvDuration  >= 0)
        {
            ospvReauthReq->ospmReauthReqDuration = ospvDuration;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetDuration() - returns the duration for a usage ind
 *-----------------------------------------------------------------------*/
int
OSPPReauthReqGetDuration(
    OSPTREAUTHREQ *ospvReauthReq                     /* usage ind */
)
{
    int ospvDuration = 0;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvDuration = ospvReauthReq->ospmReauthReqDuration;
    }
    return ospvDuration;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqAddToken() - adds a token to an authorisation indication
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqAddToken(
    OSPTREAUTHREQ  *ospvReauthReq,             /* authorisation indication */
    OSPTTOKEN *ospvToken                   /* token to add */
)
{
    if (ospvReauthReq != OSPC_OSNULL)
    {
        OSPPListAppend(&(ospvReauthReq->ospmReauthReqTokens), ospvToken);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqFirstToken() - gets first token for authorisation indication
 *-----------------------------------------------------------------------*/
OSPTTOKEN *                                /* returns null if none */
    OSPPReauthReqFirstToken(
    OSPTREAUTHREQ *ospvReauthReq
    )
{
    OSPTTOKEN *ospvToken = OSPC_OSNULL;

    if(ospvReauthReq != OSPC_OSNULL)
    {
        ospvToken = (OSPTTOKEN *)OSPPListFirst(&(ospvReauthReq->ospmReauthReqTokens));
    }
    return(ospvToken);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqNextToken() - gets next token (in list) for authorisation indication
 *-----------------------------------------------------------------------*/
OSPTTOKEN *                                /* returns NULL if no more */
    OSPPReauthReqNextToken(
    OSPTREAUTHREQ  *ospvReauthReq,             /* authorisation indication */
    OSPTTOKEN *ospvToken                   /* current token */
    )
{
    OSPTTOKEN *ospvNextToken = OSPC_OSNULL;

    if (ospvReauthReq  != OSPC_OSNULL)
    {
        if (ospvToken != OSPC_OSNULL)
        {
            ospvNextToken = (OSPTTOKEN *)OSPPListNext(&(ospvReauthReq->ospmReauthReqTokens), ospvToken);
        }
    }
    return(ospvNextToken);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasComponentId() - is the component id set ?
 *-----------------------------------------------------------------------*/
unsigned                    /* returns non-zero if component id is set */
OSPPReauthReqHasComponentId(
    OSPTREAUTHREQ *ospvReauthReq
)
{
  return (ospvReauthReq->ospmReauthReqComponentId != OSPC_OSNULL);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetComponentId() - returns a new copy of the component id.
 *-----------------------------------------------------------------------*/
unsigned char  *
OSPPReauthReqGetComponentId(
    OSPTREAUTHREQ *ospvReauthReq
)
{
    unsigned char   *componentstring   = OSPC_OSNULL;
    int             len                = 0;

    if (OSPPReauthReqHasComponentId(ospvReauthReq))
    {
        len = OSPM_STRLEN((const char *)ospvReauthReq->ospmReauthReqComponentId);
        OSPM_MALLOC(componentstring, unsigned char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvReauthReq->ospmReauthReqComponentId, len);
    }

    return componentstring;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetCallId() - sets the call ID for a reauthorisation
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqSetCallId(
    OSPTREAUTHREQ   *ospvReauthReq,        /* authorisation request */
    OSPTCALLID *ospvCallId                 /* call ID to set */
)
{
    if (ospvReauthReq   != OSPC_OSNULL) 
    {
        if ((ospvCallId) != OSPC_OSNULL)
        {
            if ((ospvReauthReq)->ospmReauthReqCallId != OSPC_OSNULL)
            {
                OSPPCallIdDelete(&((ospvReauthReq)->ospmReauthReqCallId));
            }
            (ospvReauthReq)->ospmReauthReqCallId = 
                OSPPCallIdNew((ospvCallId)->ospmCallIdLen, (ospvCallId)->ospmCallIdVal); 
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqAddSourceAlt() - add a source alternate to an 
 * authorisation request
 *-----------------------------------------------------------------------*/
void 
OSPPReauthReqAddSourceAlt(
    OSPTREAUTHREQ *ospvReauthReq,           /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo            /* altinfo to add */
    )
{
    if ((ospvReauthReq != OSPC_OSNULL) &&
        (ospvAltInfo != OSPC_OSNULL))
    {

        OSPPListAppend(&(ospvReauthReq->ospmReauthReqSourceAlternate), (ospvAltInfo));
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasSourceAlt() - does an authorisation request have a 
 * Source Alternate?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPReauthReqHasSourceAlt(
    OSPTREAUTHREQ *ospvReauthReq                  /* authorisation request */
)
{
    unsigned ospvHasSourceAlt = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasSourceAlt = (OSPPReauthReqFirstSourceAlt(ospvReauthReq) != OSPC_OSNULL);
    }
    return(ospvHasSourceAlt);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqFirstSourceAlt() - gets the First Source alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPT_ALTINFO *                              /* returns alt info pointer */
    OSPPReauthReqFirstSourceAlt(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request */
    )
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPT_ALTINFO *)OSPPListFirst(&((ospvReauthReq)->ospmReauthReqSourceAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqNextSourceAlt() - gets the next source alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPT_ALTINFO *                               /* returns alt info pointer */
    OSPPReauthReqNextSourceAlt(
    OSPTREAUTHREQ *ospvReauthReq,               /* authorisation request */
    OSPT_ALTINFO  *ospvAltInfo
    )
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        altinfo = 
            (OSPT_ALTINFO *)OSPPListNext(&((ospvReauthReq)->ospmReauthReqSourceAlternate), 
            ospvAltInfo);
    }
    return(altinfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqAddDestinationAlt() - add a destination alternate to an 
 * authorisation request
 *-----------------------------------------------------------------------*/
void 
OSPPReauthReqAddDestinationAlt(
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo    /* altinfo struct */        
    )
{

    if ((ospvReauthReq != OSPC_OSNULL) &&
        (ospvAltInfo != OSPC_OSNULL))
    {

        OSPPListAppend(&(ospvReauthReq->ospmReauthReqDestinationAlternate), (ospvAltInfo));
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasDestinationAlt() - does an authorisation request have a 
 * Destination Alternate?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPReauthReqHasDestinationAlt(
    OSPTREAUTHREQ *ospvReauthReq                  /* authorisation request */
)
{
    unsigned ospvHasDestinationAlt = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasDestinationAlt = (OSPPReauthReqFirstDestinationAlt(ospvReauthReq) != OSPC_OSNULL);
    }
    return(ospvHasDestinationAlt);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqFirstDestinationAlt() - gets the First Destination alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPT_ALTINFO *                              /* returns alt info pointer */
    OSPPReauthReqFirstDestinationAlt(
    OSPTREAUTHREQ *ospvReauthReq               /* authorisation request */
    )
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPT_ALTINFO *)OSPPListFirst(&((ospvReauthReq)->ospmReauthReqDestinationAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqNextDestinationAlt() - gets the next Destination alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPT_ALTINFO *                               /* returns alt info pointer */
    OSPPReauthReqNextDestinationAlt(
    OSPTREAUTHREQ *ospvReauthReq,               /* authorisation request */
    OSPT_ALTINFO  *ospvAltInfo
    )
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        altinfo = 
            (OSPT_ALTINFO *)OSPPListNext(&((ospvReauthReq)->ospmReauthReqDestinationAlternate), 
            ospvAltInfo);
    }
    return(altinfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetDestinationAltValue() - gets the Destination alternate value for 
 * an authorisation request
 *-----------------------------------------------------------------------*/
unsigned char *                             /* returns alt info value */
    OSPPReauthReqGetDestinationAltValue(
    OSPT_ALTINFO *ospvAltInfo                /* Alt info ptr */
    )
{
    unsigned char *ospvAltInfoValue = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL)
    {
        ospvAltInfoValue = (unsigned char *)OSPPAltInfoGetValue(ospvAltInfo);
    }
    return(ospvAltInfoValue);
}

/*-----------------------------------------------------------------------*
 * OSPPReauthReqDelete() - deletes an authorisation request structure
 *-----------------------------------------------------------------------*/
void 
OSPPReauthReqDelete(OSPTREAUTHREQ **ospvReauthReq)
{
    OSPT_ALTINFO *altinfo    = OSPC_OSNULL;
    OSPTTOKEN   *token      = OSPC_OSNULL;

    if (*ospvReauthReq)
    {
        if((*ospvReauthReq)->ospmReauthReqCallId != OSPC_OSNULL)
        {
            OSPPCallIdDelete(&((*ospvReauthReq)->ospmReauthReqCallId));
        }  

        while(!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqSourceAlternate)))
        {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqSourceAlternate));
            if(altinfo != OSPC_OSNULL)
            {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqSourceAlternate));

        while(!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqDevInfo)))
        {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqDevInfo));
            if(altinfo != OSPC_OSNULL)
            {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqDevInfo));

        while(!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqDestinationAlternate)))
        {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqDestinationAlternate));
            if(altinfo != OSPC_OSNULL)
            {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqDestinationAlternate));

        while(!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqTokens)))
        {
            token = (OSPTTOKEN *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqTokens));
            if(token != OSPC_OSNULL)
            {
                OSPPTokenDelete(&token);
            }
        }  

        if(OSPPReauthReqHasMessageId(*ospvReauthReq))
        {
            OSPM_FREE((*ospvReauthReq)->ospmReauthReqMessageId);
        }

        if(OSPPReauthReqHasComponentId(*ospvReauthReq))
        {
            OSPM_FREE((*ospvReauthReq)->ospmReauthReqComponentId);
        }

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqTokens));

        OSPM_FREE(*ospvReauthReq);
        *ospvReauthReq = OSPC_OSNULL;
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqNew() - creates a new (empty) authorisation request
 *-----------------------------------------------------------------------*/

OSPTREAUTHREQ *                              /* returns pointer or NULL */
    OSPPReauthReqNew()
{
    OSPTREAUTHREQ *ospvReauthReq = OSPC_OSNULL;

    OSPM_MALLOC(ospvReauthReq, OSPTREAUTHREQ,sizeof(OSPTREAUTHREQ));
    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvReauthReq->ospmReauthReqTimestamp = OSPC_TIMEMIN;
        ospvReauthReq->ospmReauthReqHasRole = OSPC_FALSE;
        ospvReauthReq->ospmReauthReqCallId = OSPC_OSNULL;
        ospvReauthReq->ospmReauthReqSourceNumber[0]  = '\0';
        ospvReauthReq->ospmReauthReqDestNumber[0]  = '\0';
        OSPPListNew(&ospvReauthReq->ospmReauthReqSourceAlternate);
        OSPPListNew(&ospvReauthReq->ospmReauthReqDevInfo);
        OSPPListNew(&ospvReauthReq->ospmReauthReqDestinationAlternate);
        ospvReauthReq->ospmReauthReqTrxId = 0;
        ospvReauthReq->ospmReauthReqDuration = -1;
        OSPPListNew(&ospvReauthReq->ospmReauthReqTokens);
        ospvReauthReq->ospmReauthReqCustId = 0;
        ospvReauthReq->ospmReauthReqDeviceId = 0;
        ospvReauthReq->ospmReauthReqComponentId = OSPC_OSNULL;
        ospvReauthReq->ospmReauthReqMessageId = OSPC_OSNULL;
    }
    return(ospvReauthReq);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqToElement() - create an XML element from a reauthorisation 
 * request
 *-----------------------------------------------------------------------*/
int                                     /* returns error code */
OSPPReauthReqToElement(
    OSPTREAUTHREQ  *ospvReauthReq,      /* authorisation request value */
    OSPT_XML_ELEM    **ospvElem,           /* where to put XML element pointer */
    void           *ospvtrans
)
{
    int             ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM     *elem       = OSPC_OSNULL,
        *reauthelem = OSPC_OSNULL;
    OSPT_XML_ATTR     *attr       = OSPC_OSNULL;
    OSPTTOKEN       *token      = OSPC_OSNULL;
    OSPTTRXID       trxid       = 0L;
    OSPT_ALTINFO     *altinfo    = OSPC_OSNULL;
    char            random[OSPC_MAX_RANDOM];
    OSPTBOOL        isbase64    = OSPC_TRUE;
    OSPTTRANS       *trans=(OSPTTRANS *)ospvtrans;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    else
    {
        *ospvElem = OSPC_OSNULL;
    }
    if (ospvReauthReq == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_REAUTHREQ;
    }

    if(ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* create the "Message" element as the parent */
    *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");

        if(*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID), 
                OSPPReauthReqHasMessageId(ospvReauthReq) ? (const char *)(ospvReauthReq->ospmReauthReqMessageId) : "NULL");

            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }

            /* random */
            if((OSPPUtilGetRandom(random, 0) > 0) && (ospvErrCode == OSPC_ERR_NO_ERROR)) {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *)random);

                if (attr != OSPC_OSNULL) {
                    OSPPXMLElemAddAttr(*ospvElem, attr);
                    attr = OSPC_OSNULL;
                } else {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* now the message type element */
        reauthelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_REAUTHREQ), "");
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    /* 
     * now add the attributes to the type element -- in this case the component id
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID), 
            OSPPReauthReqHasComponentId(ospvReauthReq) ? (const char *)(ospvReauthReq->ospmReauthReqComponentId) : "NULL");

        if (attr == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddAttr(reauthelem, attr);
    }

    /* now add the children */
    /* add timestamp  */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasTimestamp(ospvReauthReq)) {
        ospvErrCode = OSPPMsgTimeToElement(OSPPReauthReqGetTimestamp(ospvReauthReq), OSPPMsgElemGetName(OSPC_MELEM_TIMESTAMP), &elem);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add role  */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasRole(ospvReauthReq)) {
        ospvErrCode = OSPPMsgRoleToElement(OSPPReauthReqGetRole(ospvReauthReq), OSPPMsgElemGetName(OSPC_MELEM_ROLE), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add the call ID */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasCallId(ospvReauthReq))
    {
        ospvErrCode = OSPPCallIdToElement(OSPPReauthReqGetCallId(ospvReauthReq),
            &elem, isbase64);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add the source number */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SRCINFO),
            (const char *)OSPPReauthReqGetSourceNumber(ospvReauthReq));
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (trans->CallingNumberFormat == OSPC_E164) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ATYPE_E164));
        } else if (trans->CallingNumberFormat == OSPC_SIP) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ATYPE_SIP));
        } else if (trans->CallingNumberFormat == OSPC_URL) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ATYPE_URL));
        }
        if (attr == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddAttr(elem, attr);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add the device info element */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvReauthReq->ospmReauthReqDevInfo != NULL)) {
        for (altinfo = (OSPT_ALTINFO *)OSPPListFirst( &(ospvReauthReq->ospmReauthReqDevInfo));
            ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
            altinfo =  (OSPT_ALTINFO *)OSPPListNext( &(ospvReauthReq->ospmReauthReqDevInfo), altinfo))
        {
            ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DEVICEINFO);
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the source alternates */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && 
        OSPPReauthReqHasSourceAlt(ospvReauthReq))
    {
        for (altinfo = OSPPReauthReqFirstSourceAlt(ospvReauthReq);
            ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
            altinfo = OSPPReauthReqNextSourceAlt(ospvReauthReq, altinfo))
        {
            ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_SRCALT);
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the dest number */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO),
            (const char *)OSPPReauthReqGetDestNumber(ospvReauthReq));
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (trans->CalledNumberFormat == OSPC_E164) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ATYPE_E164));
        } else if (trans->CalledNumberFormat == OSPC_SIP) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ATYPE_SIP));
        } else if (trans->CalledNumberFormat == OSPC_URL) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ATYPE_URL));
        }

        if (attr == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddAttr(elem, attr);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add the destination alternates */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasDestinationAlt(ospvReauthReq)) {
        for (altinfo = OSPPReauthReqFirstDestinationAlt(ospvReauthReq);
            ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
            altinfo = OSPPReauthReqNextDestinationAlt(ospvReauthReq, altinfo))
        {
            ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DESTALT);
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the transaction ID */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasTrxId(ospvReauthReq))
    {
        trxid = OSPPReauthReqGetTrxId(ospvReauthReq);
        ospvErrCode = OSPPMsgTXToElement(trxid, OSPPMsgElemGetName(OSPC_MELEM_TRANSID), &elem);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add usage detail (if appropriate) */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasDuration(ospvReauthReq)) {
        /* Create usage detail element */
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_USAGEDETAIL), "");           	
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    	
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPUsageToElement((unsigned)OSPPReauthReqGetDuration(ospvReauthReq), 
            	0, 0, 0, 0, 0, 0, 0, elem);
        }
        
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* add any tokens */
        for (token = (OSPTTOKEN *)OSPPReauthReqFirstToken(ospvReauthReq);
            ((token != (OSPTTOKEN *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
            token = (OSPTTOKEN *)OSPPReauthReqNextToken(ospvReauthReq, token))
        {
            ospvErrCode = OSPPTokenToElement(token, &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(reauthelem, elem);
            }
        }
    }

    /* now add the transnexus extensions (if available) */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        if (OSPPReauthReqHasCustId(ospvReauthReq))
        {
            ospvErrCode = OSPPMsgNumToElement( 
                OSPPReauthReqGetCustId(ospvReauthReq),
                OSPPMsgElemGetName(OSPC_MELEM_CUSTID),
                &elem);

            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;

                if (OSPPReauthReqHasDeviceId(ospvReauthReq))
                {
                    ospvErrCode = OSPPMsgNumToElement( 
                        OSPPReauthReqGetDeviceId(ospvReauthReq),
                        OSPPMsgElemGetName(OSPC_MELEM_DEVICEID),
                        &elem);

                    if (ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPXMLElemAddChild(reauthelem, elem);
                        elem = OSPC_OSNULL;
                    }
                }
            }
        }
    }

    if(ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* Now add the reauthelem to the main elem */
        OSPPXMLElemAddChild(*ospvElem, reauthelem);
        reauthelem = OSPC_OSNULL;
    }

    /* if for any reason we found an error - destroy any elements created */
    if (ospvErrCode != OSPC_ERR_NO_ERROR)
    {
        if(*ospvElem != OSPC_OSNULL)
        {
            OSPPXMLElemDelete(ospvElem);
        }

        if(elem != OSPC_OSNULL)
        {
            OSPPXMLElemDelete(&elem);
        }
    
        if(reauthelem != OSPC_OSNULL)
        {
            OSPPXMLElemDelete(&reauthelem);
        }
    
        if(attr != OSPC_OSNULL)
        {
            OSPPXMLAttrDelete(&attr);
        }
    }

    return(ospvErrCode);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasRole() - Does reauthrequest have role set?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPReauthReqHasRole(
    OSPTREAUTHREQ *ospvReauthReq             /* reauth request in question */
)
{
    unsigned ospvHasRole = OSPC_FALSE;
    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasRole = ((ospvReauthReq)->ospmReauthReqHasRole != OSPC_FALSE);
    }
    return(ospvHasRole);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetRole() - returns role for an reauth request
 *-----------------------------------------------------------------------*/
OSPE_MSG_ROLE                         /* returns the role (OGW/TGW) */
    OSPPReauthReqGetRole(
    OSPTREAUTHREQ *ospvReauthReq             /* reauth request */
    )
{
    OSPE_MSG_ROLE ospvRole = OSPC_MROLE_UNDEFINED;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvRole = (OSPE_MSG_ROLE)ospvReauthReq->ospmReauthReqRole;
    }
    return(ospvRole);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetRole() - sets value for role in reauthorisation request
 *-----------------------------------------------------------------------*/
void                              
OSPPReauthReqSetRole(
    OSPTREAUTHREQ       *ospvReauthReq,
    OSPE_MSG_ROLE  ospvRole)
{

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvReauthReq->ospmReauthReqHasRole = OSPC_TRUE;
        ospvReauthReq->ospmReauthReqRole = ospvRole;
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasCustId() - Does reauthreq have a Customer Id?
 *-----------------------------------------------------------------------*/
OSPTBOOL                                   /* returns non-zero if true */
OSPPReauthReqHasCustId(
    OSPTREAUTHREQ *ospvReauthReq           /* reauthreq in question */
)
{
    OSPTBOOL ospvHasCustId = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasCustId = (ospvReauthReq->ospmReauthReqCustId != 0L);
    }
    return ospvHasCustId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetCustId() - Set Customer Id
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqSetCustId(
    OSPTREAUTHREQ   *ospvReauthReq,
    unsigned long ospvCustId
)
{
    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvReauthReq->ospmReauthReqCustId = ospvCustId;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetCustId() - returns Customer Id for a reauthreq
 *-----------------------------------------------------------------------*/
unsigned long                              /* returns the cust id */
    OSPPReauthReqGetCustId(
    OSPTREAUTHREQ *ospvReauthReq           /* usage request */
    )
{
    unsigned long ospvCustId = 0L;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvCustId = ospvReauthReq->ospmReauthReqCustId;
    }
    return ospvCustId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasDeviceId() - Does request have a Device Id?
 *-----------------------------------------------------------------------*/
OSPTBOOL                                   /* returns non-zero if true */
OSPPReauthReqHasDeviceId(
    OSPTREAUTHREQ *ospvReauthReq           /* reauthrequest in question */
)
{
    OSPTBOOL ospvHasDeviceId = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvHasDeviceId = (ospvReauthReq->ospmReauthReqDeviceId != 0L);
    }
    return ospvHasDeviceId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqSetTNDeviceId() - Set Device Id
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthReqSetDeviceId(
    OSPTREAUTHREQ    *ospvReauthReq,
    unsigned long  ospvDeviceId
)
{
    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvReauthReq->ospmReauthReqDeviceId = ospvDeviceId;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthReqGetDeviceId() - returns Device Id for an reauth request
 *-----------------------------------------------------------------------*/
unsigned long                              /* returns the device id */
    OSPPReauthReqGetDeviceId(
    OSPTREAUTHREQ *ospvReauthReq           /* reauth request */
    )
{
    unsigned long ospvDeviceId = 0L;

    if (ospvReauthReq != OSPC_OSNULL)
    {
        ospvDeviceId = (ospvReauthReq)->ospmReauthReqDeviceId;
    }
    return ospvDeviceId;
}

/*-----------------------------------------------------------------------*
 * OSPPReauthReqHasMessageId() - is the message id set ?
 *-----------------------------------------------------------------------*/
unsigned                   /* returns non-zero if message id is set */
OSPPReauthReqHasMessageId(
    OSPTREAUTHREQ *ospvReauthReq
)
{
  return (ospvReauthReq->ospmReauthReqMessageId != OSPC_OSNULL);
}


