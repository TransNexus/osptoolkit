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

/* ospreauthreq.c - OSP authorization request functions */

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

/*
 * OSPPReauthReqHasTimestamp() - Does authorization request have a valid timestamp?
 */
OSPTBOOL OSPPReauthReqHasTimestamp(     /* returns non-zero if time */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* authorization request in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (ospvReauthReq->Timestamp != OSPC_TIMEMIN);
    }

    return has;
}

/*
 * OSPPReauthReqSetTimestamp() - sets the timestamp for an authorization request
 */
void OSPPReauthReqSetTimestamp(     /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq,
    OSPTTIME ospvTime)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->Timestamp = ospvTime;
    }
}

/*
 * OSPPReauthReqGetTimestamp() - returns timestamp for an authorization request
 */
OSPTTIME OSPPReauthReqGetTimestamp( /* returns the time value */
    OSPT_REAUTH_REQ *ospvReauthReq) /* authorization request */
{
    OSPTTIME ospvTime = 0;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvTime = ospvReauthReq->Timestamp;
    }

    return ospvTime;
}

/*
 * OSPPReauthReqHasCallId() - does an authorization request have a Call ID?
 */
OSPTBOOL OSPPReauthReqHasCallId(    /* returns non-zero if exists */
    OSPT_REAUTH_REQ *ospvReauthReq) /* authorization request */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (ospvReauthReq->CallId != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPReauthReqGetCallId() - gets the call ID for an authorization request
 */
OSPT_CALL_ID *OSPPReauthReqGetCallId(   /* returns call ID pointer */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* authorization request */
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        callid = ospvReauthReq->CallId;
    }

    return callid;
}

/*
 * OSPPReauthReqSetSourceNumber() - set the source number for an authorization request
 */
void OSPPReauthReqSetSourceNumber(  /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization request  to set */
    const char *ospvNum)            /* source number (as string) */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            if (OSPM_STRLEN(ospvNum) < OSPC_SIZE_E164NUM) {
                OSPM_STRNCPY(ospvReauthReq->SourceNumber, ospvNum, OSPC_SIZE_E164NUM - 1);
            }
        }
    }
}

/*
 * OSPPReauthReqGetSourceNumber() - returns the source number for an
 * authorization request
 */
const char *OSPPReauthReqGetSourceNumber(   /* returns number as string */
    OSPT_REAUTH_REQ *ospvReauthReq)         /* authorization request */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvNum = ospvReauthReq->SourceNumber;
    }

    return ospvNum;
}

/*
 * OSPPReauthReqSetDestNumber() - set the destination number for an
 * authorization request
 */
void OSPPReauthReqSetDestNumber(    /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization request to set */
    const char *ospvNum)            /* destination number (as string) */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            if (OSPM_STRLEN(ospvNum) < OSPC_SIZE_E164NUM) {
                OSPM_STRNCPY(ospvReauthReq->DestinationNumber, ospvNum, OSPC_SIZE_E164NUM - 1);
            }
        }
    }
}

/*
 * OSPPReauthReqGetDestNumber() - returns the destination number for an
 * authorization request
 */
const char *OSPPReauthReqGetDestNumber( /* returns number as string */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* authorization request */
{
    const char *num = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        num = ospvReauthReq->DestinationNumber;
    }

    return num;
}

/*
 * OSPPReauthReqHasTrxId() - does an authorization request have a Transaction ID?
 */
OSPTBOOL OSPPReauthReqHasTrxId(     /* returns non-zero if exists */
    OSPT_REAUTH_REQ *ospvReauthReq) /* authorization request */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (ospvReauthReq->TrxId != 0);
    }

    return has;
}

/*
 * OSPPReauthReqSetTrxId() - sets the transaction ID for an authorization
 */
void OSPPReauthReqSetTrxId(         /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization request */
    OSPTTRXID ospvTrxId)            /* transaction ID to set */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->TrxId = ospvTrxId;
    }
}

/*
 * OSPPReauthReqGetTrxId() - gets the transaction ID for an authorization request
 */
OSPTTRXID OSPPReauthReqGetTrxId(    /* returns transaction ID pointer */
    OSPT_REAUTH_REQ *ospvReauthReq) /* authorization request */
{
    OSPTTRXID trxid = 0;

    if (ospvReauthReq != OSPC_OSNULL) {
        trxid = ospvReauthReq->TrxId;
    }

    return trxid;
}

/*
 * OSPPReauthReqHasDuration() - is the duration set ?
 */
OSPTBOOL OSPPReauthReqHasDuration(  /* returns non-zero if number exists */
    OSPT_REAUTH_REQ *ospvReauthReq) /* Usage Indication effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (ospvReauthReq->Duration >= 0);
    }

    return has;
}

/*
 * OSPPReauthReqSetDuration() - set the duration
 */
void OSPPReauthReqSetDuration(      /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq, /* usage indication to set */
    int ospvDuration)               /* duration to set to */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvDuration >= 0) {
            ospvReauthReq->Duration = ospvDuration;
        }
    }
}

/*
 * OSPPReauthReqGetDuration() - returns the duration for a usage
 */
int OSPPReauthReqGetDuration(
    OSPT_REAUTH_REQ *ospvReauthReq)   /* usage */
{
    int duration = 0;

    if (ospvReauthReq != OSPC_OSNULL) {
        duration = ospvReauthReq->Duration;
    }
    return duration;
}

/*
 * OSPPReauthReqAddToken() - adds a token to an authorization indication
 */
void OSPPReauthReqAddToken(         /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization indication */
    OSPT_TOKEN *ospvToken)          /* token to add */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        OSPPListAppend(&(ospvReauthReq->Tokens), ospvToken);
    }
}

/*
 * OSPPReauthReqFirstToken() - gets first token for authorization indication
 */
OSPT_TOKEN *OSPPReauthReqFirstToken(/* returns null if none */
    OSPT_REAUTH_REQ *ospvReauthReq)
{
    OSPT_TOKEN *token = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        token = (OSPT_TOKEN *)OSPPListFirst(&(ospvReauthReq->Tokens));
    }

    return token;
}

/*
 * OSPPReauthReqNextToken() - gets next token (in list) for authorization indication
 */
OSPT_TOKEN *OSPPReauthReqNextToken( /* returns NULL if no more */
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization indication */
    OSPT_TOKEN *ospvToken)          /* current token */
{
    OSPT_TOKEN *nexttoken = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvToken != OSPC_OSNULL) {
            nexttoken = (OSPT_TOKEN *)OSPPListNext(&(ospvReauthReq->Tokens), ospvToken);
        }
    }

    return nexttoken;
}

/*
 * OSPPReauthReqHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPReauthReqHasComponentId(   /* returns non-zero if component id is set */
    OSPT_REAUTH_REQ *ospvReauthReq)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        return (ospvReauthReq->ComponentId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPReauthReqGetComponentId() - returns a new copy of the component id.
 */
const char *OSPPReauthReqGetComponentId(
    OSPT_REAUTH_REQ *ospvReauthReq)
{
    const char *componentstring = OSPC_OSNULL;
    int len;

    if (OSPPReauthReqHasComponentId(ospvReauthReq)) {
        len = OSPM_STRLEN(ospvReauthReq->ComponentId);
        OSPM_MALLOC(componentstring, char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvReauthReq->ComponentId, len);
    }

    return componentstring;
}

/*
 * OSPPReauthReqSetCallId() - sets the call ID for a reauthorization
 */
void OSPPReauthReqSetCallId(        /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization request */
    OSPT_CALL_ID *ospvCallId)       /* call ID to set */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvCallId != OSPC_OSNULL) {
            if (ospvReauthReq->CallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvReauthReq->CallId));
            }
            ospvReauthReq->CallId = OSPPCallIdNew(ospvCallId->Length, ospvCallId->Value);
        }
    }
}

/*
 * OSPPReauthReqAddSourceAlt() - add a source alternate to an
 * authorization request
 */
void OSPPReauthReqAddSourceAlt(
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization request */
    OSPT_ALTINFO *ospvAltInfo)      /* altinfo to add */
{
    if ((ospvReauthReq != OSPC_OSNULL) && (ospvAltInfo != OSPC_OSNULL)) {
        OSPPListAppend(&(ospvReauthReq->SourceAlternate), ospvAltInfo);
    }
}

/*
 * OSPPReauthReqHasSourceAlt() - does an authorization request have a
 * Source Alternate?
 */
OSPTBOOL OSPPReauthReqHasSourceAlt(     /* returns non-zero if exists */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* authorization request */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (OSPPReauthReqFirstSourceAlt(ospvReauthReq) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPReauthReqFirstSourceAlt() - gets the First Source alternate for an
 * authorization request
 */
OSPT_ALTINFO *OSPPReauthReqFirstSourceAlt(  /* returns altinfo pointer */
    OSPT_REAUTH_REQ *ospvReauthReq)         /* authorization request */
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvReauthReq->SourceAlternate));
    }

    return altinfo;
}

/*
 * OSPPReauthReqNextSourceAlt() - gets the next source alternate for an
 * authorization request
 */
OSPT_ALTINFO *OSPPReauthReqNextSourceAlt(   /* returns altinfo pointer */
    OSPT_REAUTH_REQ *ospvReauthReq,         /* authorization request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvReauthReq->SourceAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPReauthReqAddDestinationAlt() - add a destination alternate to an
 * authorization request
 */
void OSPPReauthReqAddDestinationAlt(
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization request */
    OSPT_ALTINFO *ospvAltInfo)      /* altinfo structure */
{

    if ((ospvReauthReq != OSPC_OSNULL) && (ospvAltInfo != OSPC_OSNULL)) {
        OSPPListAppend(&(ospvReauthReq->DestinationAlternate), ospvAltInfo);
    }
}

/*
 * OSPPReauthReqHasDestinationAlt() - does an authorization request have a
 * Destination Alternate?
 */
OSPTBOOL OSPPReauthReqHasDestinationAlt(/* returns non-zero if exists */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* authorization request */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (OSPPReauthReqFirstDestinationAlt(ospvReauthReq) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPReauthReqFirstDestinationAlt() - gets the First Destination alternate for an
 * authorization request
 */
OSPT_ALTINFO *OSPPReauthReqFirstDestinationAlt(     /* returns altinfo pointer */
    OSPT_REAUTH_REQ *ospvReauthReq)                 /* authorization request */
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvReauthReq->DestinationAlternate));
    }

    return altinfo;
}

/*
 * OSPPReauthReqNextDestinationAlt() - gets the next Destination alternate for an
 * authorization request
 */
OSPT_ALTINFO *OSPPReauthReqNextDestinationAlt(  /* returns altinfo pointer */
    OSPT_REAUTH_REQ *ospvReauthReq,             /* authorization request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvReauthReq->DestinationAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPReauthReqGetDestinationAltValue() - gets the Destination alternate value for an authorization request
 */
const char *OSPPReauthReqGetDestinationAltValue(    /* returns alt info value */
    OSPT_ALTINFO *ospvAltInfo)                      /* altinfo ptr */
{
    const char *ospvAltInfoValue = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvAltInfoValue = OSPPAltInfoGetValue(ospvAltInfo);
    }

    return ospvAltInfoValue;
}

/*
 * OSPPReauthReqDelete() - deletes an authorization request structure
 */
void OSPPReauthReqDelete(
    OSPT_REAUTH_REQ **ospvReauthReq)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPT_TOKEN *token = OSPC_OSNULL;

    if (*ospvReauthReq) {
        if ((*ospvReauthReq)->CallId != OSPC_OSNULL) {
            OSPPCallIdDelete(&((*ospvReauthReq)->CallId));
        }

        while (!OSPPListEmpty(&((*ospvReauthReq)->SourceAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->SourceAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvReauthReq)->SourceAlternate));

        while (!OSPPListEmpty(&((*ospvReauthReq)->DeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->DeviceInfo));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvReauthReq)->DeviceInfo));

        while (!OSPPListEmpty(&((*ospvReauthReq)->DestinationAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->DestinationAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvReauthReq)->DestinationAlternate));

        while (!OSPPListEmpty(&((*ospvReauthReq)->Tokens))) {
            token = (OSPT_TOKEN *)OSPPListRemove(&((*ospvReauthReq)->Tokens));
            if (token != OSPC_OSNULL) {
                OSPPTokenDelete(&token);
            }
        }

        if (OSPPReauthReqHasMessageId(*ospvReauthReq)) {
            OSPM_FREE((*ospvReauthReq)->MessageId);
        }

        if (OSPPReauthReqHasComponentId(*ospvReauthReq)) {
            OSPM_FREE((*ospvReauthReq)->ComponentId);
        }

        OSPPListDelete(&((*ospvReauthReq)->Tokens));

        OSPM_FREE(*ospvReauthReq);
        *ospvReauthReq = OSPC_OSNULL;
    }
}

/*
 * OSPPReauthReqNew() - creates a new (empty) authorization request
 */
OSPT_REAUTH_REQ *OSPPReauthReqNew(void)   /* returns pointer or NULL */

{
    OSPT_REAUTH_REQ *reauthreq = OSPC_OSNULL;

    OSPM_MALLOC(reauthreq, OSPT_REAUTH_REQ, sizeof(OSPT_REAUTH_REQ));
    if (reauthreq != OSPC_OSNULL) {
        reauthreq->Timestamp = OSPC_TIMEMIN;
        reauthreq->HasRole = OSPC_FALSE;
        reauthreq->CallId = OSPC_OSNULL;
        reauthreq->SourceNumber[0] = '\0';
        reauthreq->DestinationNumber[0] = '\0';
        OSPPListNew(&reauthreq->SourceAlternate);
        OSPPListNew(&reauthreq->DeviceInfo);
        OSPPListNew(&reauthreq->DestinationAlternate);
        reauthreq->TrxId = 0;
        reauthreq->Duration = -1;
        OSPPListNew(&reauthreq->Tokens);
        reauthreq->CustomerId = 0;
        reauthreq->DeviceId = 0;
        reauthreq->ComponentId = OSPC_OSNULL;
        reauthreq->MessageId = OSPC_OSNULL;
    }

    return reauthreq;
}

/*
 * OSPPReauthReqToElement() - create an XML element from a reauthorization
 * request
 */
int OSPPReauthReqToElement(         /* returns error code */
    OSPT_REAUTH_REQ *ospvReauthReq, /* authorization request value */
    OSPT_XML_ELEM **ospvElem,       /* where to put XML element pointer */
    void *ospvtrans)
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL, *reauthelem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    OSPT_TOKEN *token = OSPC_OSNULL;
    OSPTTRXID trxid = 0L;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    char random[OSPC_MAX_RANDOM];
    OSPTBOOL isbase64 = OSPC_TRUE;
    OSPTTRANS *trans = (OSPTTRANS *)ospvtrans;
    unsigned i;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        *ospvElem = OSPC_OSNULL;
    }
    if (ospvReauthReq == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NO_REAUTHREQ;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the "Message" element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID),
                OSPPReauthReqHasMessageId(ospvReauthReq) ? (const char *)(ospvReauthReq->MessageId) : "NULL");
            if (attr == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }

            /* random */
            if ((OSPPUtilGetRandom(random, 0) > 0) && (errcode == OSPC_ERR_NO_ERROR)) {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *)random);
                if (attr == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ATTR;
                } else {
                    OSPPXMLElemAddAttr(*ospvElem, attr);
                    attr = OSPC_OSNULL;
                }
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* now the message type element */
        reauthelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_REAUTHREQ), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    /*
     * now add the attributes to the type element -- in this case the component id
     */
    if (errcode == OSPC_ERR_NO_ERROR) {
        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID),
            OSPPReauthReqHasComponentId(ospvReauthReq) ? (const char *)(ospvReauthReq->ComponentId) : "NULL");
        if (attr == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ATTR;
        } else {
            OSPPXMLElemAddAttr(reauthelem, attr);
        }
    }

    /* now add the children */
    /* add timestamp  */
    if ((errcode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasTimestamp(ospvReauthReq)) {
        errcode = OSPPMsgTimeToElement(OSPPReauthReqGetTimestamp(ospvReauthReq), OSPPMsgElemGetName(OSPC_MELEM_TIMESTAMP), &elem);
    }
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add role  */
    if ((errcode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasRole(ospvReauthReq)) {
        errcode = OSPPStringToElement(OSPC_MELEM_ROLE, OSPPRoleGetName(OSPPReauthReqGetRole(ospvReauthReq)), 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add the call ID */
    if ((errcode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasCallId(ospvReauthReq)) {
        errcode = OSPPCallIdToElement(OSPPReauthReqGetCallId(ospvReauthReq), &elem, isbase64);
    }
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add the source number */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPCallPartyNumToElement(OSPC_MELEM_SRCINFO, OSPPReauthReqGetSourceNumber(ospvReauthReq), trans->CallingNumberFormat, &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add the device info element */
    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvReauthReq->DeviceInfo != NULL)) {
        for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvReauthReq->DeviceInfo));
             ((altinfo != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR));
             altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvReauthReq->DeviceInfo), altinfo)) {
            errcode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DEVICEINFO);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the source alternates */
    if ((errcode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasSourceAlt(ospvReauthReq)) {
        for (altinfo = OSPPReauthReqFirstSourceAlt(ospvReauthReq);
             ((altinfo != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)); altinfo = OSPPReauthReqNextSourceAlt(ospvReauthReq, altinfo)) {
            errcode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_SRCALT);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the dest number */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPCallPartyNumToElement(OSPC_MELEM_DESTINFO, OSPPReauthReqGetDestNumber(ospvReauthReq), trans->CalledNumberFormat, &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add the destination alternates */
    if ((errcode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasDestinationAlt(ospvReauthReq)) {
        for (altinfo = OSPPReauthReqFirstDestinationAlt(ospvReauthReq);
             ((altinfo != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)); altinfo = OSPPReauthReqNextDestinationAlt(ospvReauthReq, altinfo)) {
            errcode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DESTALT);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the transaction ID */
    if ((errcode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasTrxId(ospvReauthReq)) {
        trxid = OSPPReauthReqGetTrxId(ospvReauthReq);
        errcode = OSPPMsgTXToElement(trxid, OSPPMsgElemGetName(OSPC_MELEM_TRANSID), &elem);
    }
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add usage detail (if appropriate) */
    if ((errcode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasDuration(ospvReauthReq)) {
        /* Create usage detail element */
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_USAGEDETAIL), "");
        if (elem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            errcode = OSPPUsageToElement((unsigned)OSPPReauthReqGetDuration(ospvReauthReq), 0, 0, 0, 0, 0, 0, 0, elem);
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* add any tokens */
        for (token = (OSPT_TOKEN *)OSPPReauthReqFirstToken(ospvReauthReq);
             ((token != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)); token =
             (OSPT_TOKEN *)OSPPReauthReqNextToken(ospvReauthReq, token))
        {
            errcode = OSPPTokenToElement(token, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
            }
        }
    }

    /* now add the transnexus extensions (if available) */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (OSPPReauthReqHasCustId(ospvReauthReq)) {
            errcode = OSPPMsgNumToElement(OSPPReauthReqGetCustId(ospvReauthReq), OSPPMsgElemGetName(OSPC_MELEM_CUSTID), &elem);

            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;

                if (OSPPReauthReqHasDeviceId(ospvReauthReq)) {
                    errcode = OSPPMsgNumToElement(OSPPReauthReqGetDeviceId(ospvReauthReq), OSPPMsgElemGetName(OSPC_MELEM_DEVICEID), &elem);

                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(reauthelem, elem);
                        elem = OSPC_OSNULL;
                    }
                }
            }
        }
    }

    /* Add user-defined info */
    for (i = 0; i < OSPC_MAX_INDEX; i++) {
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans->CustomInfo[i] != OSPC_OSNULL) && (trans->CustomInfo[i][0] != '\0')) {
            errcode = OSPPCustomInfoToElement(i, trans->CustomInfo[i], &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* Now add the reauthelem to the main elem */
        OSPPXMLElemAddChild(*ospvElem, reauthelem);
        reauthelem = OSPC_OSNULL;
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }

        if (elem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&elem);
        }

        if (reauthelem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&reauthelem);
        }

        if (attr != OSPC_OSNULL) {
            OSPPXMLAttrDelete(&attr);
        }
    }

    return errcode;
}

/*
 * OSPPReauthReqHasRole() - Does reauthrequest have role set?
 */
OSPTBOOL OSPPReauthReqHasRole(      /* returns non-zero if time */
    OSPT_REAUTH_REQ *ospvReauthReq)   /* reauth request in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (ospvReauthReq->HasRole != OSPC_FALSE);
    }

    return has;
}

/*
 * OSPPReauthReqGetRole() - returns role for an reauth request
 */
OSPE_ROLE OSPPReauthReqGetRole(     /* returns the role (OGW/TGW) */
    OSPT_REAUTH_REQ *ospvReauthReq) /* reauth request */
{
    OSPE_ROLE role = OSPC_ROLE_UNDEFINED;

    if (ospvReauthReq != OSPC_OSNULL) {
        role = ospvReauthReq->Role;
    }

    return role;
}

/*
 * OSPPReauthReqSetRole() - sets value for role in reauthorization request
 */
void OSPPReauthReqSetRole(
    OSPT_REAUTH_REQ *ospvReauthReq,
    OSPE_ROLE ospvRole)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->HasRole = OSPC_TRUE;
        ospvReauthReq->Role = ospvRole;
    }
}

/*
 * OSPPReauthReqHasCustId() - Does reauthreq have a Customer Id?
 */
OSPTBOOL OSPPReauthReqHasCustId(    /* returns non-zero if true */
    OSPT_REAUTH_REQ *ospvReauthReq) /* reauthreq in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (ospvReauthReq->CustomerId != 0L);
    }

    return has;
}

/*
 * OSPPReauthReqSetCustId() - Set Customer Id
 */
void OSPPReauthReqSetCustId(    /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq,
    unsigned long ospvCustId)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->CustomerId = ospvCustId;
    }
}

/*
 * OSPPReauthReqGetCustId() - returns Customer Id for a reauthreq
 */
unsigned long OSPPReauthReqGetCustId(   /* returns the cust id */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* usage request */
{
    unsigned long customerid = 0L;

    if (ospvReauthReq != OSPC_OSNULL) {
        customerid = ospvReauthReq->CustomerId;
    }

    return customerid;
}

/*
 * OSPPReauthReqHasDeviceId() - Does request have a Device Id?
 */
OSPTBOOL OSPPReauthReqHasDeviceId(      /* returns non-zero if true */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* reauthrequest in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        has = (ospvReauthReq->DeviceId != 0L);
    }

    return has;
}

/*
 * OSPPReauthReqSetTNDeviceId() - Set Device Id
 */
void OSPPReauthReqSetDeviceId(  /* nothing returned */
    OSPT_REAUTH_REQ *ospvReauthReq,
    unsigned long ospvDeviceId)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->DeviceId = ospvDeviceId;
    }
}

/*
 * OSPPReauthReqGetDeviceId() - returns Device Id for an reauth request
 */
unsigned long OSPPReauthReqGetDeviceId( /* returns the device id */
    OSPT_REAUTH_REQ *ospvReauthReq)     /* reauth request */
{
    unsigned long deviceid = 0L;

    if (ospvReauthReq != OSPC_OSNULL) {
        deviceid = (ospvReauthReq)->DeviceId;
    }

    return deviceid;
}

/*
 * OSPPReauthReqHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPReauthReqHasMessageId( /* returns non-zero if message id is set */
    OSPT_REAUTH_REQ *ospvReauthReq)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        return (ospvReauthReq->MessageId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}
