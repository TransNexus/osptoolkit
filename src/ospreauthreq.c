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

/*
 * OSPPReauthReqHasTimestamp() - Does authorisation request have a valid timestamp?
 */
OSPTBOOL OSPPReauthReqHasTimestamp(     /* returns non-zero if time */
    OSPTREAUTHREQ *ospvReauthReq)       /* authorisation request in question */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (ospvReauthReq->ospmReauthReqTimestamp != OSPC_TIMEMIN);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqSetTimestamp() - sets the timestamp for an authorisation request
 */
void OSPPReauthReqSetTimestamp(     /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq, 
    OSPTTIME ospvTime)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->ospmReauthReqTimestamp = ospvTime;
    }
}

/*
 * OSPPReauthReqGetTimestamp() - returns timestamp for an authorisation request
 */
OSPTTIME OSPPReauthReqGetTimestamp( /* returns the time value */
    OSPTREAUTHREQ *ospvReauthReq)   /* authorisation request */
{
    OSPTTIME ospvTime = 0;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvTime = ospvReauthReq->ospmReauthReqTimestamp;
    }

    return ospvTime;
}

/*
 * OSPPReauthReqHasCallId() - does an authorisation request have a Call ID?
 */
OSPTBOOL OSPPReauthReqHasCallId(    /* returns non-zero if exists */
    OSPTREAUTHREQ *ospvReauthReq)   /* authorisation request */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (ospvReauthReq->ospmReauthReqCallId != OSPC_OSNULL);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqGetCallId() - gets the call ID for an authorisation request
 */
OSPT_CALL_ID *OSPPReauthReqGetCallId( /* returns call ID pointer */
    OSPTREAUTHREQ *ospvReauthReq)   /* authorisation request */
{
    OSPT_CALL_ID *ospvCallId = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvCallId = ospvReauthReq->ospmReauthReqCallId;
    }

    return ospvCallId;
}

/*
 * OSPPReauthReqSetSourceNumber() - set the source number for an authorisation request
 */
void OSPPReauthReqSetSourceNumber(  /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request  to set */
    const char *ospvNum)            /* source number (as string) */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            if (OSPM_STRLEN(ospvNum) < OSPC_SIZE_E164NUM) {
                OSPM_STRNCPY(ospvReauthReq->ospmReauthReqSourceNumber, ospvNum, OSPC_SIZE_E164NUM - 1);
            }
        }
    }
}

/*
 * OSPPReauthReqGetSourceNumber() - returns the source number for an 
 * authorisation request
 */
const char *OSPPReauthReqGetSourceNumber(   /* returns number as string */
    OSPTREAUTHREQ *ospvReauthReq)           /* authorisation request */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvNum = ospvReauthReq->ospmReauthReqSourceNumber;
    }

    return ospvNum;
}

/*
 * OSPPReauthReqSetDestNumber() - set the destination number for an
 * authorisation request
 */
void OSPPReauthReqSetDestNumber(    /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request to set */
    const char *ospvNum)            /* destination number (as string) */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            if (OSPM_STRLEN(ospvNum) < OSPC_SIZE_E164NUM) {
                OSPM_STRNCPY(ospvReauthReq->ospmReauthReqDestNumber, ospvNum, OSPC_SIZE_E164NUM - 1);
            }
        }
    }
}

/*
 * OSPPReauthReqGetDestNumber() - returns the destination number for an
 * authorisation request
 */
const char *OSPPReauthReqGetDestNumber( /* returns number as string */
    OSPTREAUTHREQ *ospvReauthReq)       /* authorisation request */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvNum = ospvReauthReq->ospmReauthReqDestNumber;
    }

    return ospvNum;
}

/*
 * OSPPReauthReqHasTrxId() - does an authorisation request have a Transaction ID?
 */
OSPTBOOL OSPPReauthReqHasTrxId(     /* returns non-zero if exists */
    OSPTREAUTHREQ *ospvReauthReq)   /* authorisation request */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (ospvReauthReq->ospmReauthReqTrxId != 0);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqSetTrxId() - sets the transaction ID for an authorisation
 */
void OSPPReauthReqSetTrxId(         /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request */
    OSPTTRXID ospvTrxId)            /* transaction ID to set */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->ospmReauthReqTrxId = ospvTrxId;
    }
}

/*
 * OSPPReauthReqGetTrxId() - gets the transaction ID for an authorisation request
 */
OSPTTRXID OSPPReauthReqGetTrxId(    /* returns transaction ID pointer */
    OSPTREAUTHREQ *ospvReauthReq)   /* authorisation request */
{
    OSPTTRXID ospvTrxId = 0;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvTrxId = ospvReauthReq->ospmReauthReqTrxId;
    }

    return ospvTrxId;
}

/*
 * OSPPReauthReqHasDuration() - is the duration set ?
 */
OSPTBOOL OSPPReauthReqHasDuration(  /* returns non-zero if number exists */
    OSPTREAUTHREQ *ospvReauthReq)   /* Usage Indication effected */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (ospvReauthReq->ospmReauthReqDuration >= 0);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqSetDuration() - set the duration
 */
void OSPPReauthReqSetDuration(      /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq,   /* usage indication to set */
    int ospvDuration)               /* duration to set to */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvDuration >= 0) {
            ospvReauthReq->ospmReauthReqDuration = ospvDuration;
        }
    }
}

/*
 * OSPPReauthReqGetDuration() - returns the duration for a usage ind
 */
int OSPPReauthReqGetDuration(
    OSPTREAUTHREQ *ospvReauthReq)   /* usage ind */
{
    int ospvDuration = 0;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvDuration = ospvReauthReq->ospmReauthReqDuration;
    }
    return ospvDuration;
}

/*
 * OSPPReauthReqAddToken() - adds a token to an authorisation indication
 */
void OSPPReauthReqAddToken(         /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation indication */
    OSPTTOKEN *ospvToken)           /* token to add */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        OSPPListAppend(&(ospvReauthReq->ospmReauthReqTokens), ospvToken);
    }
}

/*
 * OSPPReauthReqFirstToken() - gets first token for authorisation indication
 */
OSPTTOKEN *OSPPReauthReqFirstToken( /* returns null if none */
    OSPTREAUTHREQ *ospvReauthReq)
{
    OSPTTOKEN *ospvToken = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvToken = (OSPTTOKEN *)OSPPListFirst(&(ospvReauthReq->ospmReauthReqTokens));
    }

    return ospvToken;
}

/*
 * OSPPReauthReqNextToken() - gets next token (in list) for authorisation indication
 */
OSPTTOKEN *OSPPReauthReqNextToken(  /* returns NULL if no more */
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation indication */
    OSPTTOKEN *ospvToken)           /* current token */
{
    OSPTTOKEN *ospvNextToken = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvToken != OSPC_OSNULL) {
            ospvNextToken = (OSPTTOKEN *)OSPPListNext(&(ospvReauthReq->ospmReauthReqTokens), ospvToken);
        }
    }

    return ospvNextToken;
}

/*
 * OSPPReauthReqHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPReauthReqHasComponentId(   /* returns non-zero if component id is set */
    OSPTREAUTHREQ *ospvReauthReq)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        return (ospvReauthReq->ospmReauthReqComponentId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPReauthReqGetComponentId() - returns a new copy of the component id.
 */
const char *OSPPReauthReqGetComponentId(
    OSPTREAUTHREQ *ospvReauthReq)
{
    const char *componentstring = OSPC_OSNULL;
    int len;

    if (OSPPReauthReqHasComponentId(ospvReauthReq)) {
        len = OSPM_STRLEN(ospvReauthReq->ospmReauthReqComponentId);
        OSPM_MALLOC(componentstring, char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvReauthReq->ospmReauthReqComponentId, len);
    }

    return componentstring;
}

/*
 * OSPPReauthReqSetCallId() - sets the call ID for a reauthorisation
 */
void OSPPReauthReqSetCallId(        /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request */
    OSPT_CALL_ID *ospvCallId)         /* call ID to set */
{
    if (ospvReauthReq != OSPC_OSNULL) {
        if (ospvCallId != OSPC_OSNULL) {
            if (ospvReauthReq->ospmReauthReqCallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvReauthReq->ospmReauthReqCallId));
            }
            ospvReauthReq->ospmReauthReqCallId = OSPPCallIdNew(ospvCallId->ospmCallIdLen, ospvCallId->ospmCallIdVal);
        }
    }
}

/*
 * OSPPReauthReqAddSourceAlt() - add a source alternate to an 
 * authorisation request
 */
void OSPPReauthReqAddSourceAlt(
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo)      /* altinfo to add */
{
    if ((ospvReauthReq != OSPC_OSNULL) && (ospvAltInfo != OSPC_OSNULL)) {
        OSPPListAppend(&(ospvReauthReq->ospmReauthReqSourceAlternate), ospvAltInfo);
    }
}

/*
 * OSPPReauthReqHasSourceAlt() - does an authorisation request have a 
 * Source Alternate?
 */
OSPTBOOL OSPPReauthReqHasSourceAlt(     /* returns non-zero if exists */
    OSPTREAUTHREQ *ospvReauthReq)       /* authorisation request */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (OSPPReauthReqFirstSourceAlt(ospvReauthReq) != OSPC_OSNULL);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqFirstSourceAlt() - gets the First Source alternate for an 
 * authorisation request
 */
OSPT_ALTINFO *OSPPReauthReqFirstSourceAlt(  /* returns alt info pointer */
    OSPTREAUTHREQ *ospvReauthReq)           /* authorisation request */
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO *)OSPPListFirst(&((ospvReauthReq)->ospmReauthReqSourceAlternate));
    }

    return ospvAltInfo;
}

/*
 * OSPPReauthReqNextSourceAlt() - gets the next source alternate for an 
 * authorisation request
 */
OSPT_ALTINFO *OSPPReauthReqNextSourceAlt(   /* returns alt info pointer */
    OSPTREAUTHREQ *ospvReauthReq,           /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&((ospvReauthReq)->ospmReauthReqSourceAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPReauthReqAddDestinationAlt() - add a destination alternate to an 
 * authorisation request
 */
void OSPPReauthReqAddDestinationAlt(
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo)      /* altinfo struct */
{

    if ((ospvReauthReq != OSPC_OSNULL) && (ospvAltInfo != OSPC_OSNULL)) {
        OSPPListAppend(&(ospvReauthReq->ospmReauthReqDestinationAlternate), (ospvAltInfo));
    }
}

/*
 * OSPPReauthReqHasDestinationAlt() - does an authorisation request have a 
 * Destination Alternate?
 */
OSPTBOOL OSPPReauthReqHasDestinationAlt(/* returns non-zero if exists */
    OSPTREAUTHREQ *ospvReauthReq)       /* authorisation request */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (OSPPReauthReqFirstDestinationAlt(ospvReauthReq) != OSPC_OSNULL);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqFirstDestinationAlt() - gets the First Destination alternate for an 
 * authorisation request
 */
OSPT_ALTINFO *OSPPReauthReqFirstDestinationAlt(     /* returns alt info pointer */
    OSPTREAUTHREQ *ospvReauthReq)                   /* authorisation request */
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO *)OSPPListFirst(&((ospvReauthReq)->ospmReauthReqDestinationAlternate));
    }

    return ospvAltInfo;
}

/*
 * OSPPReauthReqNextDestinationAlt() - gets the next Destination alternate for an 
 * authorisation request
 */
OSPT_ALTINFO *OSPPReauthReqNextDestinationAlt(  /* returns alt info pointer */
    OSPTREAUTHREQ *ospvReauthReq,               /* authorisation request */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvReauthReq != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&((ospvReauthReq)->ospmReauthReqDestinationAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPReauthReqGetDestinationAltValue() - gets the Destination alternate value for an authorisation request
 */
const char *OSPPReauthReqGetDestinationAltValue(    /* returns alt info value */
    OSPT_ALTINFO *ospvAltInfo)                      /* Alt info ptr */
{
    const char *ospvAltInfoValue = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvAltInfoValue = OSPPAltInfoGetValue(ospvAltInfo);
    }

    return ospvAltInfoValue;
}

/*
 * OSPPReauthReqDelete() - deletes an authorisation request structure
 */
void OSPPReauthReqDelete(
    OSPTREAUTHREQ **ospvReauthReq)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPTTOKEN *token = OSPC_OSNULL;

    if (*ospvReauthReq) {
        if ((*ospvReauthReq)->ospmReauthReqCallId != OSPC_OSNULL) {
            OSPPCallIdDelete(&((*ospvReauthReq)->ospmReauthReqCallId));
        }

        while (!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqSourceAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqSourceAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqSourceAlternate));

        while (!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqDevInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqDevInfo));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqDevInfo));

        while (!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqDestinationAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqDestinationAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqDestinationAlternate));

        while (!OSPPListEmpty(&((*ospvReauthReq)->ospmReauthReqTokens))) {
            token = (OSPTTOKEN *)OSPPListRemove(&((*ospvReauthReq)->ospmReauthReqTokens));
            if (token != OSPC_OSNULL) {
                OSPPTokenDelete(&token);
            }
        }

        if (OSPPReauthReqHasMessageId(*ospvReauthReq)) {
            OSPM_FREE((*ospvReauthReq)->ospmReauthReqMessageId);
        }

        if (OSPPReauthReqHasComponentId(*ospvReauthReq)) {
            OSPM_FREE((*ospvReauthReq)->ospmReauthReqComponentId);
        }

        OSPPListDelete(&((*ospvReauthReq)->ospmReauthReqTokens));

        OSPM_FREE(*ospvReauthReq);
        *ospvReauthReq = OSPC_OSNULL;
    }
}

/*
 * OSPPReauthReqNew() - creates a new (empty) authorisation request
 */
OSPTREAUTHREQ *OSPPReauthReqNew(void)   /* returns pointer or NULL */

{
    OSPTREAUTHREQ *ospvReauthReq = OSPC_OSNULL;

    OSPM_MALLOC(ospvReauthReq, OSPTREAUTHREQ, sizeof(OSPTREAUTHREQ));
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->ospmReauthReqTimestamp = OSPC_TIMEMIN;
        ospvReauthReq->ospmReauthReqHasRole = OSPC_FALSE;
        ospvReauthReq->ospmReauthReqCallId = OSPC_OSNULL;
        ospvReauthReq->ospmReauthReqSourceNumber[0] = '\0';
        ospvReauthReq->ospmReauthReqDestNumber[0] = '\0';
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

    return ospvReauthReq;
}

/*
 * OSPPReauthReqToElement() - create an XML element from a reauthorisation 
 * request
 */
int OSPPReauthReqToElement(         /* returns error code */
    OSPTREAUTHREQ *ospvReauthReq,   /* authorisation request value */
    OSPT_XML_ELEM **ospvElem,       /* where to put XML element pointer */
    void *ospvtrans)
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL, *reauthelem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    OSPTTOKEN *token = OSPC_OSNULL;
    OSPTTRXID trxid = 0L;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    char random[OSPC_MAX_RANDOM];
    OSPTBOOL isbase64 = OSPC_TRUE;
    OSPTTRANS *trans = (OSPTTRANS *)ospvtrans;
    unsigned i;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        *ospvElem = OSPC_OSNULL;
    }
    if (ospvReauthReq == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_REAUTHREQ;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the "Message" element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");

        if (*ospvElem == OSPC_OSNULL) {
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
            if ((OSPPUtilGetRandom(random, 0) > 0) && (ospvErrCode == OSPC_ERR_NO_ERROR)) {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *) random);

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
        ospvErrCode = OSPPStringToElement(OSPC_MELEM_ROLE, OSPPRoleGetName(OSPPReauthReqGetRole(ospvReauthReq)), 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add the call ID */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasCallId(ospvReauthReq)) {
        ospvErrCode = OSPPCallIdToElement(OSPPReauthReqGetCallId(ospvReauthReq), &elem, isbase64);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(reauthelem, elem);
    }

    /* add the source number */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPCallPartyNumToElement(OSPC_MELEM_SRCINFO, OSPPReauthReqGetSourceNumber(ospvReauthReq), trans->CallingNumberFormat, &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add the device info element */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvReauthReq->ospmReauthReqDevInfo != NULL)) {
        for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvReauthReq->ospmReauthReqDevInfo));
             ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
             altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvReauthReq->ospmReauthReqDevInfo), altinfo)) {
            ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DEVICEINFO);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the source alternates */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasSourceAlt(ospvReauthReq)) {
        for (altinfo = OSPPReauthReqFirstSourceAlt(ospvReauthReq);
             ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR)); altinfo = OSPPReauthReqNextSourceAlt(ospvReauthReq, altinfo)) {
            ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_SRCALT);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the dest number */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPCallPartyNumToElement(OSPC_MELEM_DESTINFO, OSPPReauthReqGetDestNumber(ospvReauthReq), trans->CalledNumberFormat, &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add the destination alternates */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasDestinationAlt(ospvReauthReq)) {
        for (altinfo = OSPPReauthReqFirstDestinationAlt(ospvReauthReq);
             ((altinfo != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR)); altinfo = OSPPReauthReqNextDestinationAlt(ospvReauthReq, altinfo)) {
            ospvErrCode = OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DESTALT);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* add the transaction ID */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPReauthReqHasTrxId(ospvReauthReq)) {
        trxid = OSPPReauthReqGetTrxId(ospvReauthReq);
        ospvErrCode = OSPPMsgTXToElement(trxid, OSPPMsgElemGetName(OSPC_MELEM_TRANSID), &elem);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
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
            ospvErrCode = OSPPUsageToElement((unsigned)OSPPReauthReqGetDuration(ospvReauthReq), 0, 0, 0, 0, 0, 0, 0, elem);
        }

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(reauthelem, elem);
            elem = OSPC_OSNULL;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* add any tokens */
        for (token = (OSPTTOKEN *)OSPPReauthReqFirstToken(ospvReauthReq);
             ((token != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR)); token = 
             (OSPTTOKEN *)OSPPReauthReqNextToken(ospvReauthReq, token)) 
        {
            ospvErrCode = OSPPTokenToElement(token, &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
            }
        }
    }

    /* now add the transnexus extensions (if available) */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (OSPPReauthReqHasCustId(ospvReauthReq)) {
            ospvErrCode = OSPPMsgNumToElement(OSPPReauthReqGetCustId(ospvReauthReq), OSPPMsgElemGetName(OSPC_MELEM_CUSTID), &elem);

            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;

                if (OSPPReauthReqHasDeviceId(ospvReauthReq)) {
                    ospvErrCode = OSPPMsgNumToElement(OSPPReauthReqGetDeviceId(ospvReauthReq), OSPPMsgElemGetName(OSPC_MELEM_DEVICEID), &elem);

                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(reauthelem, elem);
                        elem = OSPC_OSNULL;
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
                OSPPXMLElemAddChild(reauthelem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }
    
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* Now add the reauthelem to the main elem */
        OSPPXMLElemAddChild(*ospvElem, reauthelem);
        reauthelem = OSPC_OSNULL;
    }

    /* if for any reason we found an error - destroy any elements created */
    if (ospvErrCode != OSPC_ERR_NO_ERROR) {
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

    return ospvErrCode;
}

/*
 * OSPPReauthReqHasRole() - Does reauthrequest have role set?
 */
OSPTBOOL OSPPReauthReqHasRole(      /* returns non-zero if time */
    OSPTREAUTHREQ *ospvReauthReq)   /* reauth request in question */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (ospvReauthReq->ospmReauthReqHasRole != OSPC_FALSE);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqGetRole() - returns role for an reauth request
 */
OSPE_ROLE OSPPReauthReqGetRole(     /* returns the role (OGW/TGW) */
    OSPTREAUTHREQ *ospvReauthReq)   /* reauth request */
{
    OSPE_ROLE ospvRole = OSPC_ROLE_UNDEFINED;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvRole = (OSPE_ROLE)ospvReauthReq->ospmReauthReqRole;
    }

    return ospvRole;
}

/*
 * OSPPReauthReqSetRole() - sets value for role in reauthorisation request
 */
void OSPPReauthReqSetRole(OSPTREAUTHREQ *ospvReauthReq, OSPE_ROLE ospvRole)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->ospmReauthReqHasRole = OSPC_TRUE;
        ospvReauthReq->ospmReauthReqRole = ospvRole;
    }
}

/*
 * OSPPReauthReqHasCustId() - Does reauthreq have a Customer Id?
 */
OSPTBOOL OSPPReauthReqHasCustId(    /* returns non-zero if true */
    OSPTREAUTHREQ *ospvReauthReq)   /* reauthreq in question */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (ospvReauthReq->ospmReauthReqCustId != 0L);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqSetCustId() - Set Customer Id
 */
void OSPPReauthReqSetCustId(    /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq, 
    unsigned long ospvCustId)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->ospmReauthReqCustId = ospvCustId;
    }
}

/*
 * OSPPReauthReqGetCustId() - returns Customer Id for a reauthreq
 */
unsigned long OSPPReauthReqGetCustId(   /* returns the cust id */
    OSPTREAUTHREQ *ospvReauthReq)       /* usage request */
{
    unsigned long ospvCustId = 0L;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvCustId = ospvReauthReq->ospmReauthReqCustId;
    }

    return ospvCustId;
}

/*
 * OSPPReauthReqHasDeviceId() - Does request have a Device Id?
 */
OSPTBOOL OSPPReauthReqHasDeviceId(      /* returns non-zero if true */
    OSPTREAUTHREQ *ospvReauthReq)       /* reauthrequest in question */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvHas = (ospvReauthReq->ospmReauthReqDeviceId != 0L);
    }

    return ospvHas;
}

/*
 * OSPPReauthReqSetTNDeviceId() - Set Device Id
 */
void OSPPReauthReqSetDeviceId(    /* nothing returned */
    OSPTREAUTHREQ *ospvReauthReq, 
    unsigned long ospvDeviceId)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        ospvReauthReq->ospmReauthReqDeviceId = ospvDeviceId;
    }
}

/*
 * OSPPReauthReqGetDeviceId() - returns Device Id for an reauth request
 */
unsigned long OSPPReauthReqGetDeviceId( /* returns the device id */
    OSPTREAUTHREQ *ospvReauthReq)       /* reauth request */
{
    unsigned long ospvDeviceId = 0L;

    if (ospvReauthReq != OSPC_OSNULL) {
        ospvDeviceId = (ospvReauthReq)->ospmReauthReqDeviceId;
    }

    return ospvDeviceId;
}

/*
 * OSPPReauthReqHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPReauthReqHasMessageId( /* returns non-zero if message id is set */
    OSPTREAUTHREQ *ospvReauthReq)
{
    if (ospvReauthReq != OSPC_OSNULL) {
        return (ospvReauthReq->ospmReauthReqMessageId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}
