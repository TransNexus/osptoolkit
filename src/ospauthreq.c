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

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospcallid.h"
#include "osp/ospauthreq.h"
#include "osp/osputils.h"
#include "osp/osptrans.h"

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
                tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_SIZE_E164NUM-1));
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
OSPT_ALTINFO *                              /* returns alt info pointer */
    OSPPAuthReqFirstSourceAlt(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPT_ALTINFO *)OSPPListFirst(&((ospvAuthReq)->ospmAuthReqSourceAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqNextSourceAlt() - gets the next source alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPT_ALTINFO *                               /* returns alt info pointer */
    OSPPAuthReqNextSourceAlt(
    OSPTAUTHREQ *ospvAuthReq,               /* authorisation request */
    OSPT_ALTINFO  *ospvAltInfo
    )
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        altinfo = 
            (OSPT_ALTINFO *)OSPPListNext(&((ospvAuthReq)->ospmAuthReqSourceAlternate), 
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
OSPT_ALTINFO *                              /* returns alt info pointer */
    OSPPAuthReqFirstDestinationAlt(
    OSPTAUTHREQ *ospvAuthReq               /* authorisation request */
    )
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPT_ALTINFO *)OSPPListFirst(&((ospvAuthReq)->ospmAuthReqDestinationAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqNextDestinationAlt() - gets the next Destination alternate for an 
 * authorisation request
 *-----------------------------------------------------------------------*/
OSPT_ALTINFO *                               /* returns alt info pointer */
    OSPPAuthReqNextDestinationAlt(
    OSPTAUTHREQ *ospvAuthReq,               /* authorisation request */
    OSPT_ALTINFO  *ospvAltInfo
    )
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL)
    {
        altinfo = 
            (OSPT_ALTINFO *)OSPPListNext(&((ospvAuthReq)->ospmAuthReqDestinationAlternate), 
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
                tr_min(OSPM_STRLEN((const char *)ospvNum)+1,OSPC_SIZE_E164NUM-1));
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
    OSPT_ALTINFO *altinfo    = OSPC_OSNULL;

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

        while(!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqDeviceInfo)))
        {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqDeviceInfo));
            if(altinfo != OSPC_OSNULL)
            {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqDeviceInfo));

        while(!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqSourceAlternate)))
        {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqSourceAlternate));
            if(altinfo != OSPC_OSNULL)
            {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqSourceAlternate));

        while(!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate)))
        {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate));
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

/*^L*/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqAddServiceInfo() - Add service type to usage detail
 *-----------------------------------------------------------------------*/
unsigned OSPPAuthReqAddServiceInfo(
    OSPT_XML_ELEM **ospvElem,         /* where to put XML element pointer */
    OSPE_SERVICE_TYPE ServiceType
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM  *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE), "");
        if (*ospvElem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        if (ServiceType == OSPC_VOICE)
        {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "voice");
        }
        else
        {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "data");
        }
        if (elem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPXMLElemAddChild(*ospvElem, elem);
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL))
    {
        OSPPXMLElemDelete(ospvElem);
    }

    return(ospvErrCode);

}


/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqAddPricingInfo() - adds pricing info to an xml element
 *-----------------------------------------------------------------------*/
unsigned OSPPAuthReqAddPricingInfo(
    OSPT_XML_ELEM **ospvElem,         /* where to put XML element pointer */
    OSPT_PRICING_INFO PricingInfo
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM  *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_PRICINGIND), "");
        if (*ospvElem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        ospvErrCode = OSPPMsgFloatToElement(PricingInfo.amount,
                (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_AMOUNT), &elem);

        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
           OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* now add the increment */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        ospvErrCode = OSPPMsgNumToElement(PricingInfo.increment,
                (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_INCREMENT), &elem);
    
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
           OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }
 
    /* now we need to add units */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_UNIT),(const char *)PricingInfo.unit);
        if (elem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
 
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* add currency */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CURRENCY),(const char *)PricingInfo.currency);
        if (elem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL))
    {
        OSPPXMLElemDelete(ospvElem);
    }
    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPAuthReqToElement() - create an XML element from an authorisation request
 *-----------------------------------------------------------------------*/

int                                /* returns error code */
OSPPAuthReqToElement(
    OSPTAUTHREQ  *ospvAuthReq,     /* authorisation request value */
    OSPT_XML_ELEM **ospvElem,         /* where to put XML element pointer */
    void   *ospvtrans
)
{
    int      ospvErrCode      = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM  *elem        = OSPC_OSNULL,
        *authreqelem = OSPC_OSNULL;
    OSPT_XML_ATTR  *attr        = OSPC_OSNULL;
    OSPTCALLID   *callid      = OSPC_OSNULL;
    OSPT_ALTINFO  *altinfo     = OSPC_OSNULL;
    char         random[OSPC_MAX_RANDOM];
    OSPTBOOL     isbase64     = OSPC_TRUE;
    OSPTTRANS   *trans = (OSPTTRANS *)ospvtrans;
    int i;

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
    *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");

    if(*ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    else
    {
        attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID), 
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
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_RANDOM),
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
        authreqelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_AUTHREQ), "");
        if (authreqelem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
        /* now add the attributes to the parent -- in this case the component id
         *
         */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID), 
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
                (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_TIMESTAMP), &elem);
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
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SRCINFO),
                (const char *)OSPPAuthReqGetSourceNumber(ospvAuthReq));
            if (elem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            if (trans->CallingNumberFormat == OSPC_E164)
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                    (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_E164));
            }
            else if (trans->CallingNumberFormat == OSPC_SIP)
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                    (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_SIP));
            }
            else if (trans->CallingNumberFormat == OSPC_URL)
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                    (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_URL));
            }

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

        if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
            (ospvAuthReq->ospmAuthReqDeviceInfo != NULL))
        {
            /*
             *
             * Create/Add DeviceInfo elements
             *
             */
            for(altinfo = (OSPT_ALTINFO *)OSPPListFirst( &(ospvAuthReq->ospmAuthReqDeviceInfo));
            altinfo!= (OSPT_ALTINFO *)OSPC_OSNULL;
            altinfo = (OSPT_ALTINFO *)OSPPListNext( &(ospvAuthReq->ospmAuthReqDeviceInfo), altinfo))
            {
                OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DEVICEINFO);
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the source alternates */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && 
            OSPPAuthReqHasSourceAlt(ospvAuthReq))
        {
            for (altinfo = (OSPT_ALTINFO *)OSPPAuthReqFirstSourceAlt(ospvAuthReq);
                ((altinfo != (OSPT_ALTINFO *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                altinfo = (OSPT_ALTINFO *)OSPPAuthReqNextSourceAlt(ospvAuthReq, altinfo))
            {
                ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_SRCALT);
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
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO),
                (const char *)OSPPAuthReqGetDestNumber(ospvAuthReq));
            if (elem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            if (trans->CalledNumberFormat == OSPC_E164)
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                    (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_E164));
            }
            else if (trans->CalledNumberFormat == OSPC_SIP)
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                    (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_SIP));
            }
            else if (trans->CalledNumberFormat == OSPC_URL)
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                    (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_URL));
            }
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
            for (altinfo = (OSPT_ALTINFO *)OSPPAuthReqFirstDestinationAlt(ospvAuthReq);
                ((altinfo != (OSPT_ALTINFO *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                altinfo = (OSPT_ALTINFO *)OSPPAuthReqNextDestinationAlt(ospvAuthReq, altinfo))
            {
                ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DESTALT);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /*
         * Add the pricing information
         */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
            (trans->IsPricingInfoPresent))
        {
            for (i=0;i<trans->NumOfPricingInfoElements;i++)
            {
                ospvErrCode = OSPPAuthReqAddPricingInfo(&elem,trans->PricingInfo[i]);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
                else
                {
                    break;
                }
            }
        }

        /* now add the service */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (trans->IsServiceInfoPresent == OSPC_TRUE))
        {
            ospvErrCode = OSPPAuthReqAddServiceInfo(&elem,trans->ServiceType);
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }
        else
        {
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE),"");
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
        }

        /* now add the max destinations */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            ospvErrCode = OSPPMsgNumToElement( 
                OSPPAuthReqGetMaxDest(ospvAuthReq),
                (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_MAXDEST),
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
                    (const unsigned char *) OSPPMsgElemGetName(OSPC_MELEM_TNCUSTID),
                    &elem);

                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {

                   attr = OSPPXMLAttrNew(
                        (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
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
                            (const unsigned char *) OSPPMsgElemGetName(OSPC_MELEM_TNDEVICEID),
                            &elem);

                        if (ospvErrCode == OSPC_ERR_NO_ERROR)
                        {
                            attr = OSPPXMLAttrNew(
                                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
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
