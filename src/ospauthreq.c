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

/* ospauthreq.c - OSP authorization request functions */

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
 * OSPPAuthReqHasTimestamp() - Does authorization request have a valid timestamp?
 */
OSPTBOOL OSPPAuthReqHasTimestamp(   /* returns non-zero if time */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorization request in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (ospvAuthReq->Timestamp != OSPC_TIMEMIN);
    }

    return has;
}

/*
 * OSPPAuthReqSetTimestamp() - sets the timestamp for an authorization request
 */
void OSPPAuthReqSetTimestamp(   /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq,
    OSPTTIME ospvTime)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        (ospvAuthReq)->Timestamp = (ospvTime);
    }
}

/*
 * OSPPAuthReqGetTimestamp() - returns timestamp for an authorization request
 */
OSPTTIME OSPPAuthReqGetTimestamp(   /* returns the time value */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorization request */
{
    OSPTTIME timestamp = 0;

    if (ospvAuthReq != OSPC_OSNULL) {
        timestamp = ospvAuthReq->Timestamp;
    }

    return timestamp;
}

/*
 * OSPPAuthReqHasCallId() - does an authorization request have a Call ID?
 */
OSPTBOOL OSPPAuthReqHasCallId(  /* returns non-zero if exists */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorization request */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (OSPPAuthReqFirstCallId(ospvAuthReq) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthReqFirstCallId() - gets the First call ID for an authorization request
 */
OSPT_CALL_ID *OSPPAuthReqFirstCallId(   /* returns call ID pointer */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorization request */
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        callid = (OSPT_CALL_ID *)OSPPListFirst(&(ospvAuthReq->CallId));
    }

    return callid;
}

/*
 * OSPPAuthReqNextCallId() - gets the next call ID for an authorization request
 */
OSPT_CALL_ID *OSPPAuthReqNextCallId(    /* returns call ID pointer */
    OSPT_AUTH_REQ *ospvAuthReq,         /* authorization request */
    OSPT_CALL_ID *ospvCallId)           /* current callid */
{
    OSPT_CALL_ID *nextcallid = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        nextcallid = (OSPT_CALL_ID *)OSPPListNext(&(ospvAuthReq->CallId), ospvCallId);
    }

    return nextcallid;
}

/*
 * OSPPAuthReqHasSourceNumber() - does the authorization request have a source number?
 */
OSPTBOOL OSPPAuthReqHasSourceNumber(/* returns non-zero if number exists */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorization request effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (ospvAuthReq->SourceNumber[0] != '\0');
    }

    return has;
}

/*
 * OSPPAuthReqSetSourceNumber() - set the source number for an authorization request
 */
void OSPPAuthReqSetSourceNumber(/* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorization request  to set */
    const char *ospvNum)        /* source number (as string) */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthReq->SourceNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthReqGetSourceNumber() - returns the source number for an authorization request
 */
const char *OSPPAuthReqGetSourceNumber( /* returns number as string */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorization request */
{
    const char *num = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        num = ospvAuthReq->SourceNumber;
    }

    return num;
}

/*
 * OSPPAuthReqHasSourceAlt() - does an authorization request have a Source Alternate?
 */
OSPTBOOL OSPPAuthReqHasSourceAlt(   /* returns non-zero if exists */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorization request */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (OSPPAuthReqFirstSourceAlt(ospvAuthReq) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthReqFirstSourceAlt() - gets the First Source alternate for an authorization request
 */
OSPT_ALTINFO *OSPPAuthReqFirstSourceAlt(/* returns altinfo pointer */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorization request */
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthReq->SourceAlternate));
    }

    return altinfo;
}

/*
 * OSPPAuthReqNextSourceAlt() - gets the next source alternate for an authorization request
 */
OSPT_ALTINFO *OSPPAuthReqNextSourceAlt( /* returns altinfo pointer */
    OSPT_AUTH_REQ *ospvAuthReq,         /* authorization request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthReq->SourceAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPAuthReqHasDestinationAlt() - does an authorization request have a Destination Alternate?
 */
OSPTBOOL OSPPAuthReqHasDestinationAlt(  /* returns non-zero if exists */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorization request */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (OSPPAuthReqFirstDestinationAlt(ospvAuthReq) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthReqFirstDestinationAlt() - gets the First Destination alternate for an authorization request
 */
OSPT_ALTINFO *OSPPAuthReqFirstDestinationAlt(   /* returns altinfo pointer */
    OSPT_AUTH_REQ *ospvAuthReq)                 /* authorization request */
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthReq->DestinationAlternate));
    }

    return altinfo;
}

/*
 * OSPPAuthReqNextDestinationAlt() - gets the next Destination alternate for an authorization request
 */
OSPT_ALTINFO *OSPPAuthReqNextDestinationAlt(    /* returns altinfo pointer */
    OSPT_AUTH_REQ *ospvAuthReq,                 /* authorization request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthReq->DestinationAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPAuthReqHasDestNumber() - does the authorization request include a destination number?
 */
OSPTBOOL OSPPAuthReqHasDestNumber(  /* returns non-zero if number exists */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorization request effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (ospvAuthReq->DestinationNumber[0] != '\0');
    }

    return has;
}

/*
 * OSPPAuthReqSetDestNumber() - set the destination number for an authorization request
 */
void OSPPAuthReqSetDestNumber(  /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorization request to set */
    const char *ospvNum)        /* destination number (as string) */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthReq->DestinationNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthReqGetDestNumber() - returns the destination number for an authorization request
 */
const char *OSPPAuthReqGetDestNumber(   /* returns number as string */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorization request */
{
    const char *num = OSPC_OSNULL;

    if (ospvAuthReq != OSPC_OSNULL) {
        num = ospvAuthReq->DestinationNumber;
    }

    return num;
}

/*
 * OSPPAuthReqSetMaxDest() - set the maximum destinations for an authorization request
 */
void OSPPAuthReqSetMaxDest(     /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorization request to set */
    unsigned ospvNum)           /* maximum destinations */
{
    if (ospvAuthReq != OSPC_OSNULL) {
        if (ospvNum != 0) {
            ospvAuthReq->MaxDestination = ospvNum;
        }
    }
}

/*
 * OSPPAuthReqGetMaxDest() - returns the maximum destinations for an authorization request
 */
unsigned OSPPAuthReqGetMaxDest( /* returns maximum destinations */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorization request */
{
    unsigned num = 0;

    if (ospvAuthReq != OSPC_OSNULL) {
        num = ospvAuthReq->MaxDestination;
    }

    return num;
}

/*
 * OSPPAuthReqHasCustId() - Does request have a Customer Id?
 */
OSPTBOOL OSPPAuthReqHasCustId(  /* returns non-zero if time */
    OSPT_AUTH_REQ *ospvAuthReq) /* authorization request in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (ospvAuthReq->CustomerId != 0L);
    }

    return has;
}

/*
 * OSPPAuthReqSetCustId() - Set Customer Id
 */
void OSPPAuthReqSetCustId(      /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq,
    unsigned long ospvCustId)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        ospvAuthReq->CustomerId = ospvCustId;
    }
}

/*
 * OSPPAuthReqGetCustId() - returns Customer Id for an auth request
 */
unsigned long OSPPAuthReqGetCustId( /* returns the time value */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorization request */
{
    unsigned long id = 0L;

    if (ospvAuthReq != OSPC_OSNULL) {
        id = ospvAuthReq->CustomerId;
    }

    return id;
}

/*
 * OSPPAuthReqHasDeviceId() - Does request have a Device Id?
 */
OSPTBOOL OSPPAuthReqHasDeviceId(    /* returns non-zero if time */
    OSPT_AUTH_REQ *ospvAuthReq)     /* authorization request in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthReq != OSPC_OSNULL) {
        has = (ospvAuthReq->DeviceId != 0L);
    }

    return has;
}

/*
 * OSPPAuthReqSetDeviceId() - Set Device Id
 */
void OSPPAuthReqSetDeviceId(    /* nothing returned */
    OSPT_AUTH_REQ *ospvAuthReq,
    unsigned long ospvDeviceId)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        ospvAuthReq->DeviceId = ospvDeviceId;
    }
}

/*
 * OSPPAuthReqGetDeviceId() - returns Device Id for an auth request
 */
unsigned long OSPPAuthReqGetDeviceId(   /* returns the time value */
    OSPT_AUTH_REQ *ospvAuthReq)         /* authorization request */
{
    unsigned long id = 0L;

    if (ospvAuthReq != OSPC_OSNULL) {
        id = ospvAuthReq->DeviceId;
    }

    return id;
}

/*
 * OSPPAuthReqHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPAuthReqHasComponentId( /* returns non-zero if component id is set */
    OSPT_AUTH_REQ *ospvAuthReq)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        return(ospvAuthReq->ComponentId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthReqNew() - creates a new (empty) authorization request
 */
OSPT_AUTH_REQ *OSPPAuthReqNew(void) /* returns pointer or NULL */
{
    OSPT_AUTH_REQ *authreq = OSPC_OSNULL;

    OSPM_MALLOC(authreq, OSPT_AUTH_REQ, sizeof(OSPT_AUTH_REQ));
    if (authreq != OSPC_OSNULL) {
        OSPM_MEMSET(authreq, 0, sizeof(OSPT_AUTH_REQ));
        authreq->Timestamp = OSPC_TIMEMIN;
    }

    return authreq;
}

/*
 * OSPPAuthReqDelete() - deletes an authorization request structure
 */
void OSPPAuthReqDelete(
    OSPT_AUTH_REQ **ospvAuthReq)
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (*ospvAuthReq) {
        while (!OSPPListEmpty(&((*ospvAuthReq)->CallId))) {
            callid = (OSPT_CALL_ID *)OSPPListRemove(&((*ospvAuthReq)->CallId));

            if (callid != OSPC_OSNULL) {
                OSPM_FREE(callid);
                callid = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthReq)->CallId));

        while (!OSPPListEmpty(&((*ospvAuthReq)->DeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->DeviceInfo));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthReq)->DeviceInfo));

        while (!OSPPListEmpty(&((*ospvAuthReq)->SourceAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->SourceAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthReq)->SourceAlternate));

        while (!OSPPListEmpty(&((*ospvAuthReq)->DestinationAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthReq)->DestinationAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        if (OSPPAuthReqHasComponentId(*ospvAuthReq)) {
            OSPM_FREE((*ospvAuthReq)->ComponentId);
        }


        if (OSPPAuthReqHasMessageId(*ospvAuthReq)) {
            OSPM_FREE((*ospvAuthReq)->MessageId);
        }

        OSPPListDelete(&((*ospvAuthReq)->DestinationAlternate));

        OSPM_FREE(*ospvAuthReq);
        *ospvAuthReq = OSPC_OSNULL;
    }
}

/*
 * OSPPAuthReqToElement() - create an XML element from an authorization request
 */
int OSPPAuthReqToElement(       /* returns error code */
    OSPT_AUTH_REQ *ospvAuthReq, /* authorization request value */
    OSPT_XML_ELEM **ospvElem,   /* where to put XML element pointer */
    void *ospvtrans)
{
    int error = OSPC_ERR_NO_ERROR;
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
        error = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvAuthReq == OSPC_OSNULL) {
        error = OSPC_ERR_DATA_NO_AUTHREQ;
    }

    /* create the "Message" element as the parent */
    *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");
    if (*ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID),
            (OSPPAuthReqHasMessageId(ospvAuthReq)) ? (const char *)(ospvAuthReq->MessageId) : "NULL");
        if (attr == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ATTR;
        } else {
            OSPPXMLElemAddAttr(*ospvElem, attr);
            attr = OSPC_OSNULL;
        }

        /* random */
        if ((OSPPUtilGetRandom(random, 0) > 0) && (error == OSPC_ERR_NO_ERROR)) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *)random);
            if (attr == OSPC_OSNULL) {
                error = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* create the authreq element */
        authreqelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_AUTHREQ), "");
        if (authreqelem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            /* now add the attributes to the parent -- in this case the component id */
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID),
                (OSPPAuthReqHasComponentId(ospvAuthReq)) ? (const char *)(ospvAuthReq->ComponentId) : "NULL");
            if (attr == OSPC_OSNULL) {
                error = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(authreqelem, attr);
                attr = OSPC_OSNULL;
            }
        }

        /* now add the children */
        /* add timestamp  */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasTimestamp(ospvAuthReq)) {
            error = OSPPMsgTimeToElement(OSPPAuthReqGetTimestamp(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_TIMESTAMP), &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the call ID */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasCallId(ospvAuthReq)) {
            for (callid = OSPPAuthReqFirstCallId(ospvAuthReq);
                ((callid != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR));
                callid = OSPPAuthReqNextCallId(ospvAuthReq, callid))
            {
                error = OSPPCallIdToElement(callid, &elem, isbase64);
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add the source number */
        if (error == OSPC_ERR_NO_ERROR) {
            error = OSPPCallPartyNumToElement(OSPC_MELEM_SRCINFO, OSPPAuthReqGetSourceNumber(ospvAuthReq), trans->CallingNumberFormat, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        if ((error == OSPC_ERR_NO_ERROR) && (ospvAuthReq->DeviceInfo != NULL)) {
            /*
             * Create/Add DeviceInfo elements
             */
            for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthReq->DeviceInfo));
                altinfo != OSPC_OSNULL;
                altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthReq->DeviceInfo), altinfo))
            {
                OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DEVICEINFO);
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the source alternates */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasSourceAlt(ospvAuthReq)) {
            for (altinfo = (OSPT_ALTINFO *)OSPPAuthReqFirstSourceAlt(ospvAuthReq);
                ((altinfo != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR));
                altinfo = (OSPT_ALTINFO *)OSPPAuthReqNextSourceAlt(ospvAuthReq, altinfo))
            {
                error = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_SRCALT);
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add the dest number */
        if (error == OSPC_ERR_NO_ERROR) {
            error = OSPPCallPartyNumToElement(OSPC_MELEM_DESTINFO, OSPPAuthReqGetDestNumber(ospvAuthReq), trans->CalledNumberFormat, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the routing number */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->NPRn[0] != '\0')) {
            error = OSPPNPRnToElement(trans->NPRn, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the carrier identification code */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->NPCic[0] != '\0')) {
            error = OSPPNPCicToElement(trans->NPCic, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the npdi flag */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->NPNpdi == OSPC_TRUE)) {
            error = OSPPNPNpdiToElement(OSPC_TRUE, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* add the operator names */
        for (i = 0; i < OSPC_OPNAME_NUMBER; i++) {
            if ((error == OSPC_ERR_NO_ERROR) && trans->OpName[i][0] != '\0') {
                error = OSPPOperatorNameToElement(i, trans->OpName[i], &elem);
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add diversion */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->DivSrcInfo[0] != '\0')) {
            error = OSPPCallPartyNumToElement(OSPC_MELEM_DIVSRCINFO, trans->DivSrcInfo, OSPC_NFORMAT_E164, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }
        if ((error == OSPC_ERR_NO_ERROR) && (trans->DivDevInfo[0] != '\0')) {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DIVDEVINFO), trans->DivDevInfo);
            if (elem == OSPC_OSNULL) {
                error = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRANSPORT));
                if (attr == OSPC_OSNULL) {
                    error = OSPC_ERR_XML_NO_ATTR;
                } else {
                    OSPPXMLElemAddAttr(elem, attr);
                    attr = OSPC_OSNULL;
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* add the destination alternates */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPAuthReqHasDestinationAlt(ospvAuthReq)) {
            altinfo = OSPC_OSNULL;
            for (altinfo = (OSPT_ALTINFO *)OSPPAuthReqFirstDestinationAlt(ospvAuthReq);
                ((altinfo != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR));
                altinfo = (OSPT_ALTINFO *)OSPPAuthReqNextDestinationAlt(ospvAuthReq, altinfo))
            {
                error = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DESTALT);
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* Add the pricing information */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->HasPricingInfo)) {
            for (i = 0; i < trans->NumOfPricingInfoElements; i++) {
                error = OSPPPricingInfoToElement(trans->PricingInfo[i], &elem);
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                } else {
                    break;
                }
            }
        }

        /* now add the service */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->HasServiceInfo == OSPC_TRUE)) {
            error = OSPPServiceTypeToElement(trans->ServiceType, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        } else {
            if (error == OSPC_ERR_NO_ERROR) {
                elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE), "");
                if (elem == OSPC_OSNULL) {
                    error = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* now add the max destinations */
        if (error == OSPC_ERR_NO_ERROR) {
            error = OSPPMsgNumToElement(OSPPAuthReqGetMaxDest(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_MAXDEST), &elem);
        }
        if (error == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(authreqelem, elem);
            elem = OSPC_OSNULL;
        }

        /* now add the transnexus extentions (if available) */
        if (error == OSPC_ERR_NO_ERROR) {
            if (OSPPAuthReqHasCustId(ospvAuthReq)) {
                error = OSPPMsgNumToElement(OSPPAuthReqGetCustId(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_CUSTID), &elem);
                if (error == OSPC_ERR_NO_ERROR) {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
                    if (attr == OSPC_OSNULL) {
                        error = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(elem, attr);
                        attr = OSPC_OSNULL;
                    }

                    if (error == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(authreqelem, elem);
                        elem = OSPC_OSNULL;
                    }

                    if (OSPPAuthReqHasDeviceId(ospvAuthReq) && error == OSPC_ERR_NO_ERROR) {
                        error = OSPPMsgNumToElement(OSPPAuthReqGetDeviceId(ospvAuthReq), OSPPMsgElemGetName(OSPC_MELEM_DEVICEID), &elem);
                        if (error == OSPC_ERR_NO_ERROR) {
                            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
                            if (attr == OSPC_OSNULL) {
                                error = OSPC_ERR_XML_NO_ATTR;
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
            if ((error == OSPC_ERR_NO_ERROR) &&
                ((trans->CustomInfo[i] != OSPC_OSNULL) && (trans->CustomInfo[i][0] != '\0')))
            {
                error = OSPPCustomInfoToElement(i, trans->CustomInfo[i], &elem);
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(authreqelem, elem);
                    elem = OSPC_OSNULL;
                }
            }
        }

        /* Add source realm info */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->SrcRealm[0] != '\0')) {
            error = OSPPStringToElement(OSPC_MELEM_SRCREALM, trans->SrcRealm, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* Add destination realm info */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->DestRealm[0] != '\0')) {
            error = OSPPStringToElement(OSPC_MELEM_DESTREALM, trans->DestRealm, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* Add asserted ID */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->AssertedId[0] != '\0')) {
            error = OSPPCallPartyNumToElement(OSPC_MELEM_ASSERTEDID, trans->AssertedId, trans->AssertedIdFormat, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* Add remote party ID */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->RemotePartyId[0] != '\0')) {
            error = OSPPCallPartyNumToElement(OSPC_MELEM_RPID, trans->RemotePartyId, trans->RemotePartyIdFormat, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* Add charge info */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->ChargeInfo[0] != '\0')) {
            error = OSPPCallPartyNumToElement(OSPC_MELEM_CHARGEINFO, trans->ChargeInfo, trans->ChargeInfoFormat, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        /* Add application ID */
        if ((error == OSPC_ERR_NO_ERROR) && (trans->ApplicationId[0] != '\0')) {
            error = OSPPStringToElement(OSPC_MELEM_APPLID, trans->ApplicationId, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(authreqelem, elem);
                elem = OSPC_OSNULL;
            }
        }

        if (error == OSPC_ERR_NO_ERROR) {
            /* Now add the authreqelem to the main elem */
            OSPPXMLElemAddChild(*ospvElem, authreqelem);
            authreqelem = OSPC_OSNULL;
        }

        /* if for any reason we found an error - destroy any elements created */
        if (error != OSPC_ERR_NO_ERROR) {

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

    return error;
}

/*
 * OSPPAuthReqHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPAuthReqHasMessageId(   /* returns non-zero if message id is set */
    OSPT_AUTH_REQ *ospvAuthReq)
{
    if (ospvAuthReq != OSPC_OSNULL) {
        return(ospvAuthReq->MessageId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPNPRnToElement() - adds routing number to an xml element
 */
int OSPPNPRnToElement(
    const char *RoutingNum,         /* Routing number */
    OSPT_XML_ELEM **ospvElem)       /* where to put XML element pointer */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), RoutingNum);
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_NPRN));
            if (attr == OSPC_OSNULL) {
                OSPPXMLElemDelete(ospvElem);
                error = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }
    }

    return error;
}

/*
 * OSPPNPCicToElement() - adds carrier identification code to an xml element
 */
int OSPPNPCicToElement(
    const char *Cic,            /* Carrier identification code */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), Cic);
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_NPCIC));
            if (attr == OSPC_OSNULL) {
                OSPPXMLElemDelete(ospvElem);
                error = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }
    }

    return error;
}

/*
 * OSPPNPNpdiToElement() - adds npdi flag to an xml element
 */
int OSPPNPNpdiToElement(
    int Npdi,                   /* npdi */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        if (Npdi) {
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRUE));
        } else {
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
        }
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_NPDI));
            if (attr == OSPC_OSNULL) {
                OSPPXMLElemDelete(ospvElem);
                error = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }
    }

    return error;
}

/*
 * OSPPOperatorNameToElement() - adds operator name to an xml element
 */
int OSPPOperatorNameToElement(
    OSPE_OPERATOR_NAME ospvType,    /* Operator name type */
    const char *ospvName,           /* Operator name */
    OSPT_XML_ELEM **ospvElem)       /* Where to put XML element pointer */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPE_ALTINFO alt;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        if (((ospvType >= OSPC_OPNAME_START) && (ospvType < OSPC_OPNAME_NUMBER)) &&
            ((ospvName != OSPC_OSNULL) && (*ospvName != '\0')))
        {
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), ospvName);
            if (*ospvElem == OSPC_OSNULL) {
                error = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                switch (ospvType) {
                case OSPC_OPNAME_SPID:
                    alt = OSPC_ALTINFO_SPID;
                    break;
                case OSPC_OPNAME_OCN:
                    alt = OSPC_ALTINFO_OCN;
                    break;
                case OSPC_OPNAME_SPN:
                    alt = OSPC_ALTINFO_SPN;
                    break;
                case OSPC_OPNAME_ALTSPN:
                    alt = OSPC_ALTINFO_ALTSPN;
                    break;
                case OSPC_OPNAME_MCC:
                    alt = OSPC_ALTINFO_MCC;
                    break;
                case OSPC_OPNAME_MNC:
                    alt = OSPC_ALTINFO_MNC;
                    break;
                default:
                    alt = OSPC_ALTINFO_SPID;
                    break;
                }
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(alt));
                if (attr == OSPC_OSNULL) {
                    OSPPXMLElemDelete(ospvElem);
                    error = OSPC_ERR_XML_NO_ATTR;
                } else {
                    OSPPXMLElemAddAttr(*ospvElem, attr);
                }
            }
        } else {
            error = OSPC_ERR_XML_INVALID_TYPE;
        }
    }

    return error;
}

/*
 * OSPPServiceTypeToElement() - Adds service type to a XML element
 */
int OSPPServiceTypeToElement(
    OSPE_SERVICE ospvType,      /* Service type */
    OSPT_XML_ELEM **ospvElem)   /* Where to put XML element pointer */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICE), "");
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SERVICETYPE), "");
        if (elem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if ((ospvType >= OSPC_SERVICE_START) && (ospvType < OSPC_SERVICE_NUMBER)) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPServiceGetName(ospvType));
            if (attr == OSPC_OSNULL) {
                error = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;

                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        } else {
            error = OSPC_ERR_XML_INVALID_TYPE;
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (error != OSPC_ERR_NO_ERROR) {
        if (elem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&elem);
        }

        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return error;
}

/*
 * OSPPPricingInfoToElement() - Adds pricing info to a XML element
 */
int OSPPPricingInfoToElement(
    OSPT_PRICING_INFO ospvInfo,     /* Pricing info */
    OSPT_XML_ELEM **ospvElem)       /* Where to put XML element pointer */
{
    int error = OSPC_ERR_NO_ERROR;
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
