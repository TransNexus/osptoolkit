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

/*
 * OSPPAuthReqHasTimestamp() - Does authorisation request have a valid timestamp?
 */
OSPTBOOL OSPPAuthReqHasTimestamp(   /* returns non-zero if time */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request in question */
{
    OSPTBOOL ospvHasTimestamp = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasTimestamp = (ospvAuthReq->ospmAuthReqTimestamp != OSPC_TIMEMIN);
    }

    return ospvHasTimestamp;
}

/*
 * OSPPAuthReqSetTimestamp() - sets the timestamp for an authorisation request
 */
void OSPPAuthReqSetTimestamp(   /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, 
    OSPTTIME ospvTime)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        (ospvAuthReq)->ospmAuthReqTimestamp = (ospvTime);
    }
}

/*
 * OSPPAuthReqGetTimestamp() - returns timestamp for an authorisation request
 */
OSPTTIME OSPPAuthReqGetTimestamp(   /* returns the time value */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request */
{
    OSPTTIME ospvTimestamp = 0;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvTimestamp = ospvAuthReq->ospmAuthReqTimestamp;
    }

    return ospvTimestamp;
}

/*
 * OSPPAuthReqHasCallId() - does an authorisation request have a Call ID?
 */
OSPTBOOL OSPPAuthReqHasCallId(  /* returns non-zero if exists */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request */
{
    OSPTBOOL ospvHasCallId = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasCallId = (OSPPAuthReqFirstCallId(ospvAuthReq) != OSPC_OSNULL);
    }

    return ospvHasCallId;
}

/*
 * OSPPAuthReqFirstCallId() - gets the First call ID for an authorisation request
 */
OSPT_CALL_ID *OSPPAuthReqFirstCallId(   /* returns call ID pointer */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request */
{
    OSPT_CALL_ID *ospvCallId = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvCallId = (OSPT_CALL_ID *)OSPPListFirst(&(ospvAuthReq->ospmAuthReqCallId));
    }

    return ospvCallId;
}

/*
 * OSPPAuthReqNextCallId() - gets the next call ID for an authorisation request
 */
OSPT_CALL_ID *OSPPAuthReqNextCallId(    /* returns call ID pointer */
    OSPT_AUTH_REQ *ospvAuthReq,         /* authorisation request */
    OSPT_CALL_ID *ospvCallId)           /* current callid */
{
    OSPT_CALL_ID *nextcallid = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        nextcallid = (OSPT_CALL_ID *)OSPPListNext(&(ospvAuthReq->ospmAuthReqCallId), ospvCallId);
    }

    return nextcallid;
}

/*
 * OSPPAuthReqHasSourceNumber() - does the authorisation request have a source number?
 */
OSPTBOOL OSPPAuthReqHasSourceNumber(/* returns non-zero if number exists */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request effected */
{
    OSPTBOOL ospvHasNumber = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasNumber = (ospvAuthReq->ospmAuthReqSourceNumber[0] != '\0');
    }
    
    return ospvHasNumber;
}

/*
 * OSPPAuthReqSetSourceNumber() - set the source number for an authorisation request
 */
void OSPPAuthReqSetSourceNumber(/* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorisation request  to set */
    const char *ospvNum)        /* source number (as string) */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthReq->ospmAuthReqSourceNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthReqGetSourceNumber() - returns the source number for an authorisation request
 */
const char *OSPPAuthReqGetSourceNumber( /* returns number as string */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvNum = ospvAuthReq->ospmAuthReqSourceNumber;
    }

    return ospvNum;
}

/*
 * OSPPAuthReqHasSourceAlt() - does an authorisation request have a Source Alternate?
 */
OSPTBOOL OSPPAuthReqHasSourceAlt(   /* returns non-zero if exists */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request */
{
    OSPTBOOL ospvHasSourceAlt = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasSourceAlt = (OSPPAuthReqFirstSourceAlt(ospvAuthReq) != OSPC_OSNULL);
    }

    return ospvHasSourceAlt;
}

/*
 * OSPPAuthReqFirstSourceAlt() - gets the First Source alternate for an authorisation request 
 */
OSPT_ALTINFO *OSPPAuthReqFirstSourceAlt(/* returns alt info pointer */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request */
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthReq->ospmAuthReqSourceAlternate));
    }

    return ospvAltInfo;
}

/*
 * OSPPAuthReqNextSourceAlt() - gets the next source alternate for an authorisation request 
 */
OSPT_ALTINFO *OSPPAuthReqNextSourceAlt( /* returns alt info pointer */
    OSPT_AUTH_REQ *ospvAuthReq,         /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthReq->ospmAuthReqSourceAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPAuthReqHasDestinationAlt() - does an authorisation request have a Destination Alternate? 
 */
OSPTBOOL OSPPAuthReqHasDestinationAlt(  /* returns non-zero if exists */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request */
{
    OSPTBOOL ospvHasDestinationAlt = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasDestinationAlt = (OSPPAuthReqFirstDestinationAlt(ospvAuthReq) != OSPC_OSNULL);
    }

    return ospvHasDestinationAlt;
}

/*
 * OSPPAuthReqFirstDestinationAlt() - gets the First Destination alternate for an authorisation request 
 */
OSPT_ALTINFO *OSPPAuthReqFirstDestinationAlt(   /* returns alt info pointer */
    OSPT_AUTH_REQ *ospvAuthReq)                 /* authorisation request */
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthReq->ospmAuthReqDestinationAlternate));
    }

    return ospvAltInfo;
}

/*
 * OSPPAuthReqNextDestinationAlt() - gets the next Destination alternate for an authorisation request 
 */
OSPT_ALTINFO *OSPPAuthReqNextDestinationAlt(    /* returns alt info pointer */
    OSPT_AUTH_REQ *ospvAuthReq,                 /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthReq->ospmAuthReqDestinationAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPAuthReqHasDestNumber() - does the authorisation request include a dest number?
 */
OSPTBOOL OSPPAuthReqHasDestNumber(  /* returns non-zero if number exists */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request effected */
{
    OSPTBOOL ospvHasNumber = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasNumber = (ospvAuthReq->ospmAuthReqDestNumber[0] != '\0');
    }

    return ospvHasNumber;
}

/*
 * OSPPAuthReqSetDestNumber() - set the destination number for an authorisation request
 */
void OSPPAuthReqSetDestNumber(  /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorisation request to set */
    const char *ospvNum)        /* destination number (as string) */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthReq->ospmAuthReqDestNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthReqGetDestNumber() - returns the destination number for an authorisation request
 */
const char *OSPPAuthReqGetDestNumber(   /* returns number as string */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request */
{
    const char *ospvDestNum = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvDestNum = ospvAuthReq->ospmAuthReqDestNumber;
    }

    return ospvDestNum;
}

/*
 * OSPPAuthReqSetMaxDest() - set the maximum destinations for an authorisation request
 */
void OSPPAuthReqSetMaxDest(     /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorisation request to set */
    unsigned ospvNum)           /* maximum destinations */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNum != 0) {
            (ospvAuthReq)->ospmAuthReqMaxDest = ospvNum;
        }
    }
}

/*
 * OSPPAuthReqGetMaxDest() - returns the maximum destinations for an authorisation request
 */
unsigned OSPPAuthReqGetMaxDest( /* returns maximum destionations */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request */
{
    unsigned ospvNum = 0;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvNum = ospvAuthReq->ospmAuthReqMaxDest;
    }

    return ospvNum;
}

/*
 * OSPPAuthReqHasCustId() - Does request have a Customer Id?
 */
OSPTBOOL OSPPAuthReqHasCustId(  /* returns non-zero if time */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request in question */
{
    OSPTBOOL ospvHasCustId = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasCustId = (ospvAuthReq->ospmAuthReqCustId != 0L);
    }

    return ospvHasCustId;
}

/*
 * OSPPAuthReqSetCustId() - Set Customer Id
 */
void OSPPAuthReqSetCustId(      /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, 
    unsigned long ospvCustId)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        ospvAuthReq->ospmAuthReqCustId = ospvCustId;
    }
}

/*
 * OSPPAuthReqGetCustId() - returns Customer Id for an auth request
 */
unsigned long OSPPAuthReqGetCustId( /* returns the time value */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request */
{
    unsigned long ospvCustId = 0L;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvCustId = ospvAuthReq->ospmAuthReqCustId;
    }

    return ospvCustId;
}

/*
 * OSPPAuthReqHasDeviceId() - Does request have a Device Id?
 */
OSPTBOOL OSPPAuthReqHasDeviceId(    /* returns non-zero if time */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request in question */
{
    OSPTBOOL ospvHasDeviceId = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasDeviceId = (ospvAuthReq->ospmAuthReqDeviceId != 0L);
    }

    return ospvHasDeviceId;
}

/*
 * OSPPAuthReqSetDeviceId() - Set Device Id
 */
void OSPPAuthReqSetDeviceId(    /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, 
    unsigned long ospvDeviceId)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        ospvAuthReq->ospmAuthReqDeviceId = ospvDeviceId;
    }
}

/*
 * OSPPAuthReqGetDeviceId() - returns Device Id for an auth request
 */
unsigned long OSPPAuthReqGetDeviceId(   /* returns the time value */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request */
{
    unsigned long ospvDeviceId = 0L;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvDeviceId = ospvAuthReq->ospmAuthReqDeviceId;
    }

    return ospvDeviceId;
}

/*
 * OSPPAuthReqHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPAuthReqHasComponentId( /* returns non-zero if component id is set */
    OSPT_AUTH_REQ *ospvAuthReq)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        return(ospvAuthReq->ospmAuthReqComponentId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthReqNew() - creates a new (empty) authorisation request
 */
OSPT_AUTH_REQ *OSPPAuthReqNew(void) /* returns pointer or NULL */
{
    OSPT_AUTH_REQ *ospvAuthReq = OSPC_OSNULL;

    OSPM_MALLOC(ospvAuthReq, OSPT_AUTH_REQ, sizeof(OSPT_AUTH_REQ));
    if (ospvAuthReq != OSPC_OSNULL) {
        OSPM_MEMSET(ospvAuthReq, 0, sizeof(OSPT_AUTH_REQ));
        ospvAuthReq->ospmAuthReqTimestamp = OSPC_TIMEMIN;
    }

    return ospvAuthReq;
}

/*
 * OSPPAuthReqDelete() - deletes an authorisation request structure
 */
void OSPPAuthReqDelete(
    OSPT_AUTH_REQ **ospvAuthReq)
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (*ospvAuthReq) {
        while (!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqCallId))) {
            callid = (OSPT_CALL_ID *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqCallId));

            if (callid != OSPC_OSNULL) {
                OSPM_FREE(callid);
                callid = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqCallId));

        while (!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqDeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqDeviceInfo));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqDeviceInfo));

        while (!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqSourceAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqSourceAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqSourceAlternate));

        while (!OSPPListEmpty(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        if (OSPPAuthReqHasComponentId(*ospvAuthReq)) {
            OSPM_FREE((*ospvAuthReq)->ospmAuthReqComponentId);
        }


        if (OSPPAuthReqHasMessageId(*ospvAuthReq)) {
            OSPM_FREE((*ospvAuthReq)->ospmAuthReqMessageId);
        }

        OSPPListDelete(&((*ospvAuthReq)->ospmAuthReqDestinationAlternate));

        OSPM_FREE(*ospvAuthReq);
        *ospvAuthReq = OSPC_OSNULL;
    }
}

/*
 * OSPPAuthReqAddServiceInfo() - Add service type to usage detail
 */
unsigned OSPPAuthReqAddServiceInfo(
    OSPT_XML_ELEM **ospvElem,   /* where to put XML element pointer */
    OSPE_SERVICE ServiceType)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE), "");
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (ServiceType == OSPC_SERVICE_VOICE) {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "voice");
        } else {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "data");
        }
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(*ospvElem, elem);
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return ospvErrCode;
}

/*
 * OSPPAuthReqAddPricingInfo() - adds pricing info to an xml element
 */
unsigned OSPPAuthReqAddPricingInfo(
    OSPT_XML_ELEM **ospvElem,       /* where to put XML element pointer */
    OSPT_PRICING_INFO PricingInfo)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_PRICINGIND), "");
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgFloatToElement(PricingInfo.amount, OSPPMsgElemGetName(OSPC_MELEM_AMOUNT), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* now add the increment */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgNumToElement(PricingInfo.increment, OSPPMsgElemGetName(OSPC_MELEM_INCREMENT), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* now we need to add units */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_UNIT), (const char *)PricingInfo.unit);
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* add currency */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CURRENCY), (const char *)PricingInfo.currency);
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return ospvErrCode;
}

/*
 * OSPPAuthReqToElement() - create an XML element from an authorisation request
 */
int OSPPAuthReqToElement(       /* returns error code */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorisation request value */
    OSPT_XML_ELEM **ospvElem,   /* where to put XML element pointer */
    void *ospvtrans)
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL, *authreqelem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    OSPT_CALL_ID *callid = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    char random[OSPC_MAX_RANDOM];
    OSPTBOOL isbase64 = OSPC_TRUE;
    OSPTTRANS *trans = (OSPTTRANS *)ospvtrans;
    int i;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvAuthReq == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_AUTHREQ;
    }

    /* create the "Message" element as the parent */
    *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");

    if (*ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID),
            (OSPPAuthReqHasMessageId(ospvAuthReq)) ? (const char *)(ospvAuthReq->ospmAuthReqMessageId) : "NULL");

        if (attr != OSPC_OSNULL) {
            OSPPXMLElemAddAttr(*ospvElem, attr);
            attr = OSPC_OSNULL;
        } else {
            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
        }

        /* random */
        if ((OSPPUtilGetRandom(random, 0) > 0) && (ospvErrCode == OSPC_ERR_NO_ERROR)) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *)random);

            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the authreq element */
        authreqelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_AUTHREQ), "");
        if (authreqelem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
        /* now add the attributes to the parent -- in this case the component id */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID),
                (OSPPAuthReqHasComponentId(ospvAuthReq)) ? (const char *)(ospvAuthReq->ospmAuthReqComponentId) : "NULL");
            if (attr == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddAttr(authreqelem, attr);
            attr = OSPC_OSNULL;
        }

        /* now add the children */
        /* add timestamp  */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasTimestamp(ospvAuthReq)) {
            ospvErrCode = OSPPMsgTimeToElement(OSPPAuthReqGetTimestamp(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_TIMESTAMP), &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }


        /* add the call ID */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasCallId(ospvAuthReq)) {
            for (callid = OSPPAuthReqFirstCallId(ospvAuthReq);
                ((callid != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR)); 
                callid = OSPPAuthReqNextCallId(ospvAuthReq, callid)) 
            {
                ospvErrCode = OSPPCallIdToElement(callid, &elem, isbase64);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add the source number */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPCallPartyNumToElement(OSPC_MELEM_SRCINFO, OSPPAuthReqGetSourceNumber(ospvAuthReq), trans->CallingNumberFormat, &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvAuthReq->ospmAuthReqDeviceInfo != NULL)) {
            /*
             * Create/Add DeviceInfo elements
             */
            for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthReq->ospmAuthReqDeviceInfo));
                altinfo != OSPC_OSNULL; 
                altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthReq->ospmAuthReqDeviceInfo), altinfo))
            {
                OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DEVICEINFO);
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the source alternates */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasSourceAlt(ospvAuthReq)) {
            for (altinfo = (OSPT_ALTINFO *)OSPPAuthReqFirstSourceAlt(ospvAuthReq);
                ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                altinfo = (OSPT_ALTINFO *)OSPPAuthReqNextSourceAlt(ospvAuthReq, altinfo)) 
            {
                ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_SRCALT);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add the dest number */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPCallPartyNumToElement(OSPC_MELEM_DESTINFO, OSPPAuthReqGetDestNumber(ospvAuthReq), trans->CalledNumberFormat, &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the routing number */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasRoutingNumber(ospvAuthReq)) {
            ospvErrCode = OSPPRoutingNumToElement(OSPPAuthReqGetRoutingNumber(ospvAuthReq), &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }
        
        /* add the destination alternates */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasDestinationAlt(ospvAuthReq)) {
            altinfo = OSPC_OSNULL;
            for (altinfo = (OSPT_ALTINFO *)OSPPAuthReqFirstDestinationAlt(ospvAuthReq);
                ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                altinfo = (OSPT_ALTINFO *)OSPPAuthReqNextDestinationAlt(ospvAuthReq, altinfo)) 
            {
                ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DESTALT);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /*
         * Add the pricing information
         */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (trans->IsPricingInfoPresent)) {
            for (i = 0; i < trans->NumOfPricingInfoElements; i++) {
                ospvErrCode = OSPPAuthReqAddPricingInfo(&elem, trans->PricingInfo[i]);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                } else {
                    break;
                }
            }
        }

        /* now add the service */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (trans->IsServiceInfoPresent == OSPC_TRUE)) {
            ospvErrCode = OSPPAuthReqAddServiceInfo(&elem, trans->ServiceType);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        } else {
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE), "");
                if (elem == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* now add the max destinations */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPMsgNumToElement(OSPPAuthReqGetMaxDest(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_MAXDEST), &elem);
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(authreqelem, elem);
            elem = OSPC_OSNULL;
        }

        /* now add the transnexus extentions (if available) */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (OSPPAuthReqHasCustId(ospvAuthReq)) {
                ospvErrCode = OSPPMsgNumToElement(OSPPAuthReqGetCustId(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_CUSTID), &elem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));

                    if (attr != OSPC_OSNULL) {
                        OSPPXMLElemAddAttr(elem, attr);
                        attr = OSPC_OSNULL;
                    } else {
                        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                    }

                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(authreqelem, elem);
                        elem = OSPC_OSNULL;
                    }

                    if (OSPPAuthReqHasDeviceId(ospvAuthReq) && ospvErrCode == OSPC_ERR_NO_ERROR) {
                        ospvErrCode = OSPPMsgNumToElement(OSPPAuthReqGetDeviceId(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_DEVICEID), &elem);
                        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
                            if (attr != OSPC_OSNULL) {
                                OSPPXMLElemAddAttr(elem, attr);
                                attr = OSPC_OSNULL;
                            } else {
                                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                            }

                            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                                OSPPXMLElemAddChild(authreqelem, elem);
                                elem = OSPC_OSNULL;
                            }
                        }
                    }
                }
            }
        }

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            /* Now add the authreqelem to the main elem */
            OSPPXMLElemAddChild(*ospvElem, authreqelem);
            authreqelem = OSPC_OSNULL;
        }

        /* if for any reason we found an error - destroy any elements created */
        if (ospvErrCode != OSPC_ERR_NO_ERROR) {

            if (elem != OSPC_OSNULL) {
                OSPPXMLElemDelete(&elem);
            }

            if (authreqelem != OSPC_OSNULL) {
                OSPPXMLElemDelete(&authreqelem);
            }

            if (attr != OSPC_OSNULL) {
                OSPPXMLAttrDelete(&attr);
            }

            if (*ospvElem != OSPC_OSNULL) {
                OSPPXMLElemDelete(ospvElem);
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPAuthReqHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPAuthReqHasMessageId(   /* returns non-zero if message id is set */
    OSPT_AUTH_REQ *ospvAuthReq)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        return(ospvAuthReq->ospmAuthReqMessageId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthReqHasRoutingNumber() - does the authorisation request have a routing number?
 */
OSPTBOOL OSPPAuthReqHasRoutingNumber(   /* returns non-zero if number exists */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request effected */
{                            
    OSPTBOOL ospvHasNumber = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHasNumber = (ospvAuthReq->ospmAuthReqRoutingNumber[0] != '\0');
    }

    return ospvHasNumber;
}

/*
 * OSPPAuthReqSetRoutingNumber() - set the routing number for an authorisation request
 */
void OSPPAuthReqSetRoutingNumber(   /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq,     /* authorisation request  to set */
    const char *ospvNum)            /* source number (as string) */
{                       
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthReq->ospmAuthReqRoutingNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthReqGetRoutingNumber() - returns the routing number for an authorisation request
 */
const char *OSPPAuthReqGetRoutingNumber(/* returns number as string */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorisation request */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvNum = ospvAuthReq->ospmAuthReqRoutingNumber;
    }

    return ospvNum;
}

/*
 * OSPPRoutingNumToElement() - adds routing number to an xml element
 */
unsigned OSPPRoutingNumToElement(
    const char *RoutingNum,         /* Routing number */
    OSPT_XML_ELEM **ospvElem)       /* where to put XML element pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    
    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), RoutingNum);
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_ROUTINGNUM));
            if (attr == OSPC_OSNULL) {
                OSPPXMLElemDelete(ospvElem);
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }        	
    }

    return ospvErrCode;
}

