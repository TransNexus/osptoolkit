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

/* ospauthreq.c - OSP authorisation request functions */

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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqTimestamp != OSPC_TIMEMIN);
    }

    return ospvHas;
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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (OSPPAuthReqFirstCallId(ospvAuthReq) != OSPC_OSNULL);
    }

    return ospvHas;
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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqSourceNumber[0] != '\0');
    }

    return ospvHas;
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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (OSPPAuthReqFirstSourceAlt(ospvAuthReq) != OSPC_OSNULL);
    }

    return ospvHas;
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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (OSPPAuthReqFirstDestinationAlt(ospvAuthReq) != OSPC_OSNULL);
    }

    return ospvHas;
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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqDestNumber[0] != '\0');
    }

    return ospvHas;
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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqCustId != 0L);
    }

    return ospvHas;
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
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqDeviceId != 0L);
    }

    return ospvHas;
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
    unsigned i;
    char *number;
    char *domain;

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
        if (attr == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
        } else {
            OSPPXMLElemAddAttr(*ospvElem, attr);
            attr = OSPC_OSNULL;
        }

        /* random */
        if ((OSPPUtilGetRandom(random, 0) > 0) && (ospvErrCode == OSPC_ERR_NO_ERROR)) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *)random);
            if (attr == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the authreq element */
        authreqelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_AUTHREQ), "");
        if (authreqelem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            /* now add the attributes to the parent -- in this case the component id */
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID),
                (OSPPAuthReqHasComponentId(ospvAuthReq)) ? (const char *)(ospvAuthReq->ospmAuthReqComponentId) : "NULL");
            if (attr == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(authreqelem, attr);
                attr = OSPC_OSNULL;
            }
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
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasNPRn(ospvAuthReq)) {
            ospvErrCode = OSPPNPRnToElement(OSPPAuthReqGetNPRn(ospvAuthReq), &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the carrier identification code */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasNPCic(ospvAuthReq)) {
            ospvErrCode = OSPPNPCicToElement(OSPPAuthReqGetNPCic(ospvAuthReq), &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the npdi flag */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasNPNpdi(ospvAuthReq)) {
            ospvErrCode = OSPPNPNpdiToElement(OSPPAuthReqGetNPNpdi(ospvAuthReq), &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add diversion */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPAuthReqGetDiversion(ospvAuthReq, &number, &domain);
        }
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (number[0] != '\0')) {
            ospvErrCode = OSPPCallPartyNumToElement(OSPC_MELEM_DIVERSIONSRCINFO, number, OSPC_NFORMAT_E164, &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (domain[0] != '\0')) {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DIVERSIONDEVINFO), domain);
            if (elem == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRANSPORT));
                if (attr == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                } else {
                    OSPPXMLElemAddAttr(elem, attr);
                    attr = OSPC_OSNULL;
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
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

        /* Add the pricing information */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (trans->HasPricingInfo)) {
            for (i = 0; i < trans->NumOfPricingInfoElements; i++) {
                ospvErrCode = OSPPPricingInfoToElement(trans->PricingInfo[i], &elem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                } else {
                    break;
                }
            }
        }

        /* now add the service */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (trans->HasServiceInfo == OSPC_TRUE)) {
            ospvErrCode = OSPPServiceTypeToElement(trans->ServiceType, &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        } else {
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE), "");
                if (elem == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
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
                    if (attr == OSPC_OSNULL) {
                        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(elem, attr);
                        attr = OSPC_OSNULL;
                    }

                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(authreqelem, elem);
                        elem = OSPC_OSNULL;
                    }

                    if (OSPPAuthReqHasDeviceId(ospvAuthReq) && ospvErrCode == OSPC_ERR_NO_ERROR) {
                        ospvErrCode = OSPPMsgNumToElement(OSPPAuthReqGetDeviceId(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_DEVICEID), &elem);
                        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
                            if (attr == OSPC_OSNULL) {
                                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                            } else {
                                OSPPXMLElemAddAttr(elem, attr);
                                attr = OSPC_OSNULL;
                                OSPPXMLElemAddChild(authreqelem, elem);
                                elem = OSPC_OSNULL;
                            }
                        }
                    }
                }
            }
        }

        /* Add user-defined info */
        for (i = 0; i < OSPC_MAX_INDEX; i++) {
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (trans->CustomInfo[i] != OSPC_OSNULL) && (trans->CustomInfo[i][0] != '\0')) {
                ospvErrCode = OSPPCustomInfoToElement(i, trans->CustomInfo[i], &elem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
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
 * OSPPAuthReqSetNumberPortability() - set number portability parameters for an authorisation request
 */
void OSPPAuthReqSetNumberPortability(   /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq,         /* authorisation request  to set */
    const char *ospvNPRn,               /* routing number (as string) */
    const char *ospvNPCic,              /* carrier identification code (as string) */
    int ospvNPNpdi)                     /* npdi */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNPRn != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthReq->ospmAuthReqNPRn, ospvNPRn, sizeof(ospvAuthReq->ospmAuthReqNPRn) - 1);
        }
        if (ospvNPCic != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthReq->ospmAuthReqNPCic, ospvNPCic, sizeof(ospvAuthReq->ospmAuthReqNPCic) - 1);
        }
        if (ospvNPNpdi) {
            ospvAuthReq->ospmAuthReqNPNpdi = OSPC_TRUE;
        } else {
            ospvAuthReq->ospmAuthReqNPNpdi = OSPC_FALSE;
        }
    }
}

/*
 * OSPPAuthReqHasNPRn() - does the authorisation request have a routing number?
 */
OSPTBOOL OSPPAuthReqHasNPRn(    /* returns non-zero if routing number exists */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request effected */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqNPRn[0] != '\0');
    }

    return ospvHas;
}

/*
 * OSPPAuthReqGetNPRn() - returns the routing number for an authorisation request
 */
const char *OSPPAuthReqGetNPRn( /* returns number as string */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvNum = ospvAuthReq->ospmAuthReqNPRn;
    }

    return ospvNum;
}

/*
 * OSPPNPRnToElement() - adds routing number to an xml element
 */
unsigned OSPPNPRnToElement(
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
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_NPRN));
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

/*
 * OSPPAuthReqHasNPCic() - does the authorisation request have a carrier identification code?
 */
OSPTBOOL OSPPAuthReqHasNPCic(   /* returns non-zero if carrier identification code exists */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request effected */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqNPCic[0] != '\0');
    }

    return ospvHas;
}

/*
 * OSPPAuthReqGetNPCic() - returns the carrier identification code for an authorisation request
 */
const char *OSPPAuthReqGetNPCic(    /* returns carrier identification code as string */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvNum = ospvAuthReq->ospmAuthReqNPCic;
    }

    return ospvNum;
}

/*
 * OSPPNPCicToElement() - adds carrier identification code to an xml element
 */
unsigned OSPPNPCicToElement(
    const char *Cic,            /* Carrier identification code */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), Cic);
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_NPCIC));
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

/*
 * OSPPAuthReqHasNPNpdi() - does the authorisation request have a npdi flag?
 */
OSPTBOOL OSPPAuthReqHasNPNpdi(  /* returns non-zero if npdi flag exists */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request effected */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = (ospvAuthReq->ospmAuthReqNPNpdi != OSPC_FALSE);
    }

    return ospvHas;
}

/*
 * OSPPAuthReqGetNPNpdi() - returns npdi flag for an authorisation request
 */
int OSPPAuthReqGetNPNpdi(       /* returns npdi */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorisation request */
{
    int ospvNpdi = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvNpdi = ospvAuthReq->ospmAuthReqNPNpdi;
    }

    return ospvNpdi;
}

/*
 * OSPPNPNpdiToElement() - adds npdi flag to an xml element
 */
unsigned OSPPNPNpdiToElement(
    int Npdi,                   /* npdi */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        if (Npdi) {
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRUE));
        } else {
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
        }
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_NPDI));
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

/*
 * OSPPAuthReqHasDiversion() - does the authorisation request have a Diversion?
 */
OSPTBOOL OSPPAuthReqHasDiversion(   /* returns non-zero if Diversion exists */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorisation request effected */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        ospvHas = ((ospvAuthReq->ospmAuthReqDiversionSrcInfo[0] != '\0') && (ospvAuthReq->ospmAuthReqDiversionDevInfo[0] != '\0'));
    }

    return ospvHas;
}

/*
 * OSPPAuthReqSetDiversion() - set the Diversion for an authorisation request
 */
void OSPPAuthReqSetDiversion(   /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorisation request  to set */
    const char *ospvNumber,     /* number (as string) */
    const char *ospvDomain)     /* domain (as string) */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if ((ospvNumber != OSPC_OSNULL) && (ospvDomain != OSPC_OSNULL)) {
            OSPM_STRNCPY(ospvAuthReq->ospmAuthReqDiversionSrcInfo, ospvNumber, tr_min(OSPM_STRLEN(ospvNumber) + 1, OSPC_SIZE_E164NUM - 1));
            OSPM_STRNCPY(ospvAuthReq->ospmAuthReqDiversionDevInfo, ospvDomain, tr_min(OSPM_STRLEN(ospvDomain) + 1, OSPC_SIZE_SIGNALADDR - 1));
        }
    }
}

/*
 * OSPPAuthReqGetDiversion() - returns the Diversion for an authorisation request
 */
void OSPPAuthReqGetDiversion(    /* nothing returnd */
    OSPT_AUTH_REQ *ospvAuthReq,     /* authorisation request */
    char **ospvNumber,              /* number */
    char **ospvDomain)              /* domain */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        *ospvNumber = ospvAuthReq->ospmAuthReqDiversionSrcInfo;
        *ospvDomain = ospvAuthReq->ospmAuthReqDiversionDevInfo;
    } else {
        *ospvNumber = OSPC_OSNULL;
        *ospvDomain = OSPC_OSNULL;
    }
}

/*
 * OSPPServiceTypeToElement() - Adds service type to a XML element
 */
unsigned OSPPServiceTypeToElement(
    OSPE_SERVICE ospvType,      /* Service type */
    OSPT_XML_ELEM **ospvElem)   /* Where to put XML element pointer */
{
    unsigned error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE), "");
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (error == OSPC_ERR_NO_ERROR) {
    	switch (ospvType) {
    	case OSPC_SERVICE_VOICE:
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "voice");
            break;
    	case OSPC_SERVICE_DATA:
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "data");
            break;
    	case OSPC_SERVICE_NPQUERY:
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "PortedNumberQuery");
            break;
    	default:
    		break;
    	}
        if (elem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((error != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return error;
}

/*
 * OSPPPricingInfoToElement() - Adds pricing info to a XML element
 */
unsigned OSPPPricingInfoToElement(
    OSPT_PRICING_INFO ospvInfo,     /* Pricing info */
    OSPT_XML_ELEM **ospvElem)       /* Where to put XML element pointer */
{
    unsigned error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_PRICINGIND), "");
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        error = OSPPMsgFloatToElement(ospvInfo.amount, OSPPMsgElemGetName(OSPC_MELEM_AMOUNT), &elem);
        if (error == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* now add the increment */
    if (error == OSPC_ERR_NO_ERROR) {
        error = OSPPMsgNumToElement(ospvInfo.increment, OSPPMsgElemGetName(OSPC_MELEM_INCREMENT), &elem);
        if (error == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* now we need to add units */
    if (error == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_UNIT), (const char *)ospvInfo.unit);
        if (elem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* add currency */
    if (error == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CURRENCY), (const char *)ospvInfo.currency);
        if (elem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((error != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return error;
}
