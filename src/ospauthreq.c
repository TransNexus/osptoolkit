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
 *  ospauthreq.c - OSP authorisation request functions
 */

#include "osp.h"
#include "osperrno.h"
#include "ospbfr.h"
#include "ospxmlattr.h"
#include "ospxmlelem.h"
#include "ospmsgattr.h"
#include "ospmsgelem.h"
#include "ospcallid.h"
#include "ospauthreq.h"
#include "osputils.h"

#ifdef OSPC_DEBUG
/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasTimestamp() - Does authorisation request have a valid timestamp?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPAuthReqHasTimestamp(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request in question */
)
{
    unsigned ospvHasTimestamp = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasTimestamp = ((ospvAuthReq)->ospmAuthReqTimestamp != OSPC_TIMEMIN);
    }
    return(ospvHasTimestamp);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqSetTimestamp() - sets the timestamp for an authorisation request
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPAuthReqSetTimestamp(
    OSPTAUTHREQ *ospvAuthReq,
    OSPTTIME  ospvTime
)
{
    if (ospvAuthReq != OSPC_OSNULL)
    {
        (ospvAuthReq)->ospmAuthReqTimestamp = (ospvTime);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqGetTimestamp() - returns timestamp for an authorisation request
 *-----------------------------------------------------------------------*/
OSPTTIME                                   /* returns the time value */
    OSPPAuthReqGetTimestamp(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    OSPTTIME ospvTimestamp = 0;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvTimestamp = (ospvAuthReq)->ospmAuthReqTimestamp;
    }
    return(ospvTimestamp);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasCallId() - does an authorisation request have a Call ID?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPAuthReqHasCallId(
    OSPTAUTHREQ *ospvAuthReq                  /* authorisation request */
)
{
    unsigned ospvHasCallId = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasCallId = (OSPPAuthReqFirstCallId(ospvAuthReq) != OSPC_OSNULL);
    }
    return(ospvHasCallId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqFirstCallId() - gets the First call ID for an authorisation request
 *-----------------------------------------------------------------------*/
OSPTCALLID *                               /* returns call ID pointer */
    OSPPAuthReqFirstCallId(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    OSPTCALLID *ospvCallId = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvCallId = (OSPTCALLID *)OSPPListFirst(&(ospvAuthReq->ospmAuthReqCallId));
    }
    return(ospvCallId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqNextCallId() - gets the next call ID for an authorisation request
 *-----------------------------------------------------------------------*/
OSPTCALLID *                               /* returns call ID pointer */
    OSPPAuthReqNextCallId(
    OSPTAUTHREQ *ospvAuthReq,               /* authorisation request */
    OSPTCALLID  *ospvCallId                 /* current callid */
    )
{
    OSPTCALLID *nextcallid = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        nextcallid = (OSPTCALLID *)OSPPListNext(&(ospvAuthReq->ospmAuthReqCallId), ospvCallId);
    }
    return(nextcallid);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasSourceNumber() - does the authorisation request have
 * a source number?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPAuthReqHasSourceNumber(
    OSPTAUTHREQ *ospvAuthReq        /* authorisation request effected */
)
{
    unsigned ospvHasNumber = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasNumber = ((ospvAuthReq)->ospmAuthReqSourceNumber[0] != '\0');
    }
    return(ospvHasNumber);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqSetSourceNumber() - set the source number for an
 * authorisation request
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPAuthReqSetSourceNumber(
    OSPTAUTHREQ         *ospvAuthReq,      /* authorisation request  to set */
    const unsigned char *ospvNum           /* source number (as string) */
)
{
    if (ospvAuthReq != OSPC_OSNULL)
    {
        if (ospvNum  != OSPC_OSNULL)
        {
            OSPM_STRNCPY((char *)(ospvAuthReq)->ospmAuthReqSourceNumber, 
                (const char *)(ospvNum), 
                tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_E164NUMSIZE-1));
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqGetSourceNumber() - returns the source number for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
unsigned char *                      /* returns number as string */
OSPPAuthReqGetSourceNumber(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
)
{
    unsigned char *ospvNum = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvNum = (ospvAuthReq->ospmAuthReqSourceNumber);
    }
    return(ospvNum);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasSourceAlt() - does an authorisation request have a 
 * Source Alternate?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPAuthReqHasSourceAlt(
    OSPTAUTHREQ *ospvAuthReq                  /* authorisation request */
)
{
    unsigned ospvHasSourceAlt = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasSourceAlt = (OSPPAuthReqFirstSourceAlt(ospvAuthReq) != OSPC_OSNULL);
    }
    return(ospvHasSourceAlt);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqFirstSourceAlt() - gets the First Source alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                              /* returns alt info pointer */
    OSPPAuthReqFirstSourceAlt(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    OSPTALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPTALTINFO *)OSPPListFirst(&((ospvAuthReq)->ospmAuthReqSourceAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqNextSourceAlt() - gets the next source alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                               /* returns alt info pointer */
    OSPPAuthReqNextSourceAlt(
    OSPTAUTHREQ *ospvAuthReq,               /* authorisation request */
    OSPTALTINFO  *ospvAltInfo
    )
{
    OSPTALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        altinfo = 
            (OSPTALTINFO *)OSPPListNext(&((ospvAuthReq)->ospmAuthReqSourceAlternate), 
            ospvAltInfo);
    }
    return(altinfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasDestinationAlt() - does an authorisation request have a 
 * Destination Alternate?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPAuthReqHasDestinationAlt(
    OSPTAUTHREQ *ospvAuthReq                  /* authorisation request */
)
{
    unsigned ospvHasDestinationAlt = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasDestinationAlt = (OSPPAuthReqFirstDestinationAlt(ospvAuthReq) != OSPC_OSNULL);
    }
    return(ospvHasDestinationAlt);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqFirstDestinationAlt() - gets the First Destination alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                              /* returns alt info pointer */
    OSPPAuthReqFirstDestinationAlt(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    OSPTALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPTALTINFO *)OSPPListFirst(&((ospvAuthReq)->ospmAuthReqDestinationAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqNextDestinationAlt() - gets the next Destination alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                               /* returns alt info pointer */
    OSPPAuthReqNextDestinationAlt(
    OSPTAUTHREQ *ospvAuthReq,               /* authorisation request */
    OSPTALTINFO  *ospvAltInfo
    )
{
    OSPTALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        altinfo = 
            (OSPTALTINFO *)OSPPListNext(&((ospvAuthReq)->ospmAuthReqDestinationAlternate), 
            ospvAltInfo);
    }
    return(altinfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasDestNumber() - does the authorisation request include a
 * dest number?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPAuthReqHasDestNumber(
    OSPTAUTHREQ *ospvAuthReq        /* authorisation request effected */
)
{
    unsigned ospvHasNumber = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasNumber = ((ospvAuthReq)->ospmAuthReqDestNumber[0] != '\0');
    }
    return(ospvHasNumber);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqSetDestNumber() - set the destination number for an
 * authorisation request
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPAuthReqSetDestNumber(
    OSPTAUTHREQ         *ospvAuthReq,      /* authorisation request to set */
    const unsigned char *ospvNum           /* destination number (as string) */
)
{
    if (ospvAuthReq != OSPC_OSNULL)
    {
        if (ospvNum  != OSPC_OSNULL)
        {
            OSPM_STRNCPY((char *)(ospvAuthReq)->ospmAuthReqDestNumber, 
                (const char *)(ospvNum), 
                tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_E164NUMSIZE-1));
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqGetDestNumber() - returns the destination number for an
 * authorisation request
 *-----------------------------------------------------------------------*/
unsigned char *                      /* returns number as string */
OSPPAuthReqGetDestNumber(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
)
{
    unsigned char *ospvDestNum = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvDestNum = ((ospvAuthReq)->ospmAuthReqDestNumber);
    }
    return(ospvDestNum);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqSetMaxDest() - set the maximum destinations for an
 * authorisation request
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPAuthReqSetMaxDest(
    OSPTAUTHREQ         *ospvAuthReq,      /* authorisation request to set */
    unsigned             ospvNum           /* maximum destinations */
)
{
    if (ospvAuthReq != OSPC_OSNULL)
    {
        if (ospvNum  != 0)
        {
            (ospvAuthReq)->ospmAuthReqMaxDest = ospvNum;
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqGetMaxDest() - returns the maximum destinations for an
 * authorisation request
 *-----------------------------------------------------------------------*/
unsigned                                  /* returns maximum destionations */
OSPPAuthReqGetMaxDest(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
)
{
    unsigned ospvNum = 0;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvNum = ((ospvAuthReq)->ospmAuthReqMaxDest);
    }
    return(ospvNum);
}
/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasTNCustId() - Does request have a TransNexus Customer Id?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPAuthReqHasTNCustId(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request in question */
)
{
    unsigned ospvHasTNCustId = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasTNCustId = ((ospvAuthReq)->ospmAuthReqTNCustId != 0L);
    }
    return(ospvHasTNCustId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqSetTNCustId() - Set TransNexus Customer Id
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPAuthReqSetTNCustId(
    OSPTAUTHREQ   *ospvAuthReq,
    unsigned long ospvTNCustId
)
{
    if (ospvAuthReq != OSPC_OSNULL)
    {
        (ospvAuthReq)->ospmAuthReqTNCustId = (ospvTNCustId);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqGetTNCustId() - returns TN Customer Id for an auth request
 *-----------------------------------------------------------------------*/
unsigned long                              /* returns the time value */
    OSPPAuthReqGetTNCustId(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    unsigned long ospvTNCustId = 0L;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvTNCustId = (ospvAuthReq)->ospmAuthReqTNCustId;
    }
    return(ospvTNCustId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasTNDeviceId() - Does request have a TransNexus Device Id?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPAuthReqHasTNDeviceId(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request in question */
)
{
    unsigned ospvHasTNDeviceId = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvHasTNDeviceId = ((ospvAuthReq)->ospmAuthReqTNDeviceId != 0L);
    }
    return(ospvHasTNDeviceId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqSetTNDeviceId() - Set TransNexus Device Id
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPAuthReqSetTNDeviceId(
    OSPTAUTHREQ    *ospvAuthReq,
    unsigned long  ospvTNDeviceId
)
{
    if (ospvAuthReq != OSPC_OSNULL)
    {
        (ospvAuthReq)->ospmAuthReqTNDeviceId = (ospvTNDeviceId);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqGetTNDeviceId() - returns TN Device Id for an auth request
 *-----------------------------------------------------------------------*/
unsigned long                              /* returns the time value */
    OSPPAuthReqGetTNDeviceId(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    unsigned long ospvTNDeviceId = 0L;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvTNDeviceId = (ospvAuthReq)->ospmAuthReqTNDeviceId;
    }
    return(ospvTNDeviceId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasComponentId() - is the component id set ?
 *-----------------------------------------------------------------------*/
unsigned                    /* returns non-zero if component id is set */
OSPPAuthReqHasComponentId(
    OSPTAUTHREQ *ospvAuthReq
)
{
  return (ospvAuthReq->ospmAuthReqComponentId != OSPC_OSNULL);
}

#endif /* OSPC_DEBUG */

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqNew() - creates a new (empty) authorisation request
 *-----------------------------------------------------------------------*/

OSPTAUTHREQ *                              /* returns pointer or NULL */
    OSPPAuthReqNew()
{
    OSPTAUTHREQ *ospvAuthReq = OSPC_OSNULL;

    OSPM_MALLOC(ospvAuthReq, OSPTAUTHREQ,sizeof(OSPTAUTHREQ));
    if (ospvAuthReq != OSPC_OSNULL)
    {
        OSPM_MEMSET(ospvAuthReq, 0,sizeof(OSPTAUTHREQ));
        ospvAuthReq->ospmAuthReqTimestamp = OSPC_TIMEMIN;
    }
    return(ospvAuthReq);
}

/*-----------------------------------------------------------------------*
 * OSPPAuthReqDelete() - deletes an authorisation request structure
 *-----------------------------------------------------------------------*/
void 
OSPPAuthReqDelete(OSPTAUTHREQ **ospvAuthReq)
{
    OSPTCALLID  *callid     = OSPC_OSNULL;
    OSPTALTINFO *altinfo    = OSPC_OSNULL;

    if (*ospvAuthReq)
    {
        while(!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqCallId)))
        {
            callid = (OSPTCALLID *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqCallId));

            if(callid != OSPC_OSNULL)
            {
                OSPM_FREE(callid);
                callid = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqCallId));

        while(!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqSourceAlternate)))
        {
            altinfo = (OSPTALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqSourceAlternate));
            if(altinfo != OSPC_OSNULL)
            {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqSourceAlternate));

        while(!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate)))
        {
            altinfo = (OSPTALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate));
            if(altinfo != OSPC_OSNULL)
            {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        if(OSPPAuthReqHasComponentId(*ospvAuthReq))
        {
            OSPM_FREE((*ospvAuthReq)->ospmAuthReqComponentId);
        }


        if(OSPPAuthReqHasMessageId(*ospvAuthReq))
        {
            OSPM_FREE((*ospvAuthReq)->ospmAuthReqMessageId);
        }

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate));

        OSPM_FREE(*ospvAuthReq);
        *ospvAuthReq = OSPC_OSNULL;
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqToElement() - create an XML element from an authorisation request
 *-----------------------------------------------------------------------*/

int                                /* returns error code */
OSPPAuthReqToElement(
    OSPTAUTHREQ  *ospvAuthReq,     /* authorisation request value */
    OSPTXMLELEM **ospvElem         /* where to put XML element pointer */
)
{
    int      ospvErrCode      = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM  *elem        = OSPC_OSNULL,
        *authreqelem = OSPC_OSNULL;
    OSPTXMLATTR  *attr        = OSPC_OSNULL;
    OSPTCALLID   *callid      = OSPC_OSNULL;
    OSPTALTINFO  *altinfo     = OSPC_OSNULL;
    char         random[OSPC_MAX_RANDOM];
    OSPTBOOL     isbase64     = OSPC_TRUE;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvAuthReq == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_AUTHREQ;
    }

    /* create the "Message" element as the parent */
    *ospvElem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemMessage), "");

    if(*ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    else
    {
        attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrMessageId), 
            (OSPPAuthReqHasMessageId(ospvAuthReq))?(const unsigned char *)(ospvAuthReq->ospmAuthReqMessageId): (const unsigned char *)"NULL");

        if (attr != OSPC_OSNULL) 
        {
            OSPPXMLElemAddAttr(*ospvElem, attr);
            attr = OSPC_OSNULL;
        }
        else
        {
            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
        }

        /* random */
        if((OSPPUtilGetRandom(random, 0) > 0) &&
            (ospvErrCode == OSPC_ERR_NO_ERROR))
        {
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrRandom),
                (const unsigned char *)random);

            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* create the authreq element */
        authreqelem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemAuthReq), "");
        if (authreqelem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
        /* now add the attributes to the parent -- in this case the component id
         *
         */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrComponentId), 
                (OSPPAuthReqHasComponentId(ospvAuthReq))?(const unsigned char *)(ospvAuthReq->ospmAuthReqComponentId): (const unsigned char *)"NULL");
            if (attr == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddAttr(authreqelem, attr);
            attr = OSPC_OSNULL;
        }

        /* now add the children */
        /* add timestamp  */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasTimestamp(ospvAuthReq))
        {
            ospvErrCode = OSPPMsgTimeToElement(OSPPAuthReqGetTimestamp(ospvAuthReq),
                (const unsigned char *)OSPPMsgGetElemName(ospeElemTimestamp), &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }


        /* add the call ID */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasCallId(ospvAuthReq))
        {
            for (callid = OSPPAuthReqFirstCallId(ospvAuthReq);
                ((callid != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                callid = OSPPAuthReqNextCallId(ospvAuthReq, callid))
            {
                ospvErrCode = OSPPCallIdToElement(callid, &elem, isbase64);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add the source number */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            elem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemSrcInfo),
                (const char *)OSPPAuthReqGetSourceNumber(ospvAuthReq));
            if (elem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType),
                (const unsigned char *)"e164");
            if (attr == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddAttr(elem, attr);
            attr = OSPC_OSNULL;
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddChild(authreqelem, elem);
            elem = OSPC_OSNULL;
        }

        /* add the source alternates */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && 
            OSPPAuthReqHasSourceAlt(ospvAuthReq))
        {
            for (altinfo = (OSPTALTINFO *)OSPPAuthReqFirstSourceAlt(ospvAuthReq);
                ((altinfo != (OSPTALTINFO *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                altinfo = (OSPTALTINFO *)OSPPAuthReqNextSourceAlt(ospvAuthReq, altinfo))
            {
                ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, ospeElemSrcAlt);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add the dest number */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            elem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemDestInfo),
                (const char *)OSPPAuthReqGetDestNumber(ospvAuthReq));
            if (elem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType),
                (const unsigned char *)"e164");
            if (attr == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddAttr(elem, attr);
            attr = OSPC_OSNULL;

            OSPPXMLElemAddChild(authreqelem, elem);
            elem = OSPC_OSNULL;
        }

        /* add the destination alternates */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && 
            OSPPAuthReqHasDestinationAlt(ospvAuthReq))
        {
            altinfo = OSPC_OSNULL;
            for (altinfo = (OSPTALTINFO *)OSPPAuthReqFirstDestinationAlt(ospvAuthReq);
                ((altinfo != (OSPTALTINFO *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                altinfo = (OSPTALTINFO *)OSPPAuthReqNextDestinationAlt(ospvAuthReq, altinfo))
            {
                ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, ospeElemDestAlt);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* now add the service */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            elem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemService),"");
            if (elem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddChild(authreqelem, elem);
            elem = OSPC_OSNULL;
        }

        /* now add the max destinations */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            ospvErrCode = OSPPMsgNumToElement( 
                OSPPAuthReqGetMaxDest(ospvAuthReq),
                (const unsigned char *)OSPPMsgGetElemName(ospeElemMaxDest),
                &elem);
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddChild(authreqelem, elem);
            elem = OSPC_OSNULL;
        }

        /* now add the transnexus extentions (if available) */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            if (OSPPAuthReqHasTNCustId(ospvAuthReq))
            {
                ospvErrCode = OSPPMsgNumToElement( 
                    OSPPAuthReqGetTNCustId(ospvAuthReq),
                    (const unsigned char *) OSPPMsgGetElemName(ospeElemTNCustId),
                    &elem);

                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {

                   attr = OSPPXMLAttrNew(
                        (const unsigned char *)OSPPMsgGetAttrName(ospeAttrCritical),
                        (const unsigned char *)"False");

                    if (attr != OSPC_OSNULL)
                    {
                        OSPPXMLElemAddAttr(elem, attr);
                        attr = OSPC_OSNULL;
                    }
                    else
                    {
                        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                    }

                    if (ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPXMLElemAddChild(authreqelem, elem);
                        elem = OSPC_OSNULL;
                    }

                    if (OSPPAuthReqHasTNDeviceId(ospvAuthReq) && ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        ospvErrCode = OSPPMsgNumToElement( 
                            OSPPAuthReqGetTNDeviceId(ospvAuthReq),
                            (const unsigned char *) OSPPMsgGetElemName(ospeElemTNDeviceId),
                            &elem);

                        if (ospvErrCode == OSPC_ERR_NO_ERROR)
                        {
                            attr = OSPPXMLAttrNew(
                                (const unsigned char *)OSPPMsgGetAttrName(ospeAttrCritical),
                                (const unsigned char *)"False");

                            if (attr != OSPC_OSNULL)
                            {
                                OSPPXMLElemAddAttr(elem, attr);
                                attr = OSPC_OSNULL;
                            }
                            else
                            {
                                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                            }

                            if (ospvErrCode == OSPC_ERR_NO_ERROR)
                            {
                                OSPPXMLElemAddChild(authreqelem, elem);
                                elem = OSPC_OSNULL;
                            }                        
                        }
                    }
                }
            }
        }

        if(ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            /* Now add the authreqelem to the main elem */
            OSPPXMLElemAddChild(*ospvElem, authreqelem);
            authreqelem = OSPC_OSNULL;
        }

        /* if for any reason we found an error - destroy any elements created */
        if (ospvErrCode != OSPC_ERR_NO_ERROR)
        {

            if(elem != OSPC_OSNULL)
            {
                OSPPXMLElemDelete(&elem);
            }

            if(authreqelem != OSPC_OSNULL)
            {
                OSPPXMLElemDelete(&authreqelem);
            }

            if(attr != OSPC_OSNULL)
            {
                OSPPXMLAttrDelete(&attr);
            }

            if(*ospvElem != OSPC_OSNULL)
            {
                OSPPXMLElemDelete(ospvElem);
            }
        }
    }
    return(ospvErrCode);
}

/*-----------------------------------------------------------------------*
 * OSPPAuthReqHasMessageId() - is the message id set ?
 *-----------------------------------------------------------------------*/
unsigned                   /* returns non-zero if message id is set */
OSPPAuthReqHasMessageId(
    OSPTAUTHREQ *ospvAuthReq
)
{
  return (ospvAuthReq->ospmAuthReqMessageId != OSPC_OSNULL);
}
