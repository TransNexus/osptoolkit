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

/* ospdest.c - OSP destination functions */

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
#include "osp/ospaltinfo.h"

/* Array that associates protocols and names */
const OSPT_MSG_DESC OSPV_PROTNAME_DESCS[OSPC_PROTNAME_NUMBER] = {
    { OSPC_PROTNAME_SIP,   "sip" },
    { OSPC_PROTNAME_Q931,  "h323-Q931" },
    { OSPC_PROTNAME_LRQ,   "h323-LRQ" },
    { OSPC_PROTNAME_IAX,   "iax" },
    { OSPC_PROTNAME_T37,   "fax-T37" },
    { OSPC_PROTNAME_T38,   "fax-T38" },
    { OSPC_PROTNAME_SKYPE, "skype" },
    { OSPC_PROTNAME_SMPP,  "smpp" },
    { OSPC_PROTNAME_XMPP,  "xmpp" },
    { OSPC_PROTNAME_SMS,   "sms" }
};

/*
 * OSPPDestSetOSPVersion() - set the OSP Version for a destination
 */
void OSPPDestSetOSPVersion(     /* nothing returned */
    OSPT_DEST *ospvDest,        /* destination to set */
    const char *ospvVersion)    /* Version (as string) */
{
    if (ospvDest != OSPC_OSNULL) {
        if (!(OSPM_STRCMP(ospvVersion, DEST_OSP_DIABLED))) {
            ospvDest->OSPVersion = OSPC_DOSP_FALSE;
        } else {
            if (!(OSPM_STRCMP(ospvVersion, DEST_OSP_UNKNOWN))) {
                ospvDest->OSPVersion = OSPC_DOSP_UNKNOWN;
            } else {
                ospvDest->OSPVersion = OSPC_DOSP_TRUE;
            }
        }
    }
}

/*
 * OSPPDestSetProtocol() - set the protocol for a destination
 */
void OSPPDestSetProtocol(   /* Nothing returned */
    OSPT_DEST *ospvDest,    /* Destination to set */
    const char *ospvProt)   /* Protocol (as string) */
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->Protocol = OSPPDestProtocolGetPart(ospvProt);
    }
}

/*
 * OSPPDestHasNumber() - does the destination include a called number?
 */
OSPTBOOL OSPPDestHasNumber( /* returns non-zero if number exists */
    OSPT_DEST *ospvDest)    /* destination effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->DestinationNumber[0] != '\0');
    }

    return has;
}

/*
 * OSPPDestHasSrcNumber() - does the destination include a calling number?
 */
OSPTBOOL OSPPDestHasSrcNumber(  /* returns non-zero if number exists */
    OSPT_DEST *ospvDest)        /* destination effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->SourceNumber[0] != '\0');
    }

    return has;
}

/*
 * OSPPDestSetSrcNumber() - set the calling number for a destination
 */
void OSPPDestSetSrcNumber(  /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination to set */
    const char *ospvNum)    /* calling number (as string) */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->SourceNumber, ospvNum, tr_min(OSPC_SIZE_E164NUM, OSPM_STRLEN(ospvNum) + 1));
        }
    }
}

/*
 * OSPPDestGetSrcNumber() - returns the calling number for a destination
 */
const char *OSPPDestGetSrcNumber(   /* returns number as string */
    OSPT_DEST *ospvDest)            /* destination */
{
    const char *num = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        num = ospvDest->SourceNumber;
    }

    return num;
}

/*
 * OSPPDestSetNumber() - set the called number for a destination
 */
void OSPPDestSetNumber(     /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination to set */
    const char *ospvNum)    /* called number (as string) */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->DestinationNumber, ospvNum, tr_min(OSPC_SIZE_E164NUM, OSPM_STRLEN(ospvNum) + 1));
        }
    }
}

/*
 * OSPPDestGetNumber() - returns the called number for a destination
 */
const char *OSPPDestGetNumber(  /* returns number as string */
    OSPT_DEST *ospvDest)        /* destination */
{
    const char *num = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        num = ospvDest->DestinationNumber;
    }

    return num;
}

/*
 * OSPPDestHasAddr() - does a destination have a Addr?
 */
OSPTBOOL OSPPDestHasAddr(   /* returns non-zero if exists */
    OSPT_DEST *ospvDest)    /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->DestinationAddr[0] != '\0');
    }

    return has;
}

/*
 * OSPPDestSetAddr() - sets the signalling address for a destination
 */
void OSPPDestSetAddr(       /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination */
    const char *ospvAddr)   /* signal address as string */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAddr != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->DestinationAddr, ospvAddr, tr_min(OSPC_SIZE_SIGNALADDR, OSPM_STRLEN(ospvAddr) + 1));
        }
    }
}

/*
 * OSPPDestGetAddr() - returns the signalling address for a destination
 */
const char *OSPPDestGetAddr(    /* returns address as string */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    const char *addr = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        addr = ospvDest->DestinationAddr;
    }

    return addr;
}

/*
 * OSPPDestDevHasAddr() - does a destination have a Dev Addr?
 */
OSPTBOOL OSPPDestDevHasAddr(    /* returns non-zero if exists */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->DestinationDevAddr[0] != '\0');
    }

    return has;
}

/*
 * OSPPDestHasNetworkAddr() - does a destination have a Network Addr?
 */
OSPTBOOL OSPPDestHasNetworkAddr(    /* returns non-zero if exists */
    OSPT_DEST *ospvDest)            /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->NetworkId[0] != '\0');
    }

    return has;
}

/*
 * OSPPDestSetNetworkAddr() - sets the network address for a destination
 *  device
 */
void OSPPDestSetNetworkAddr(    /* nothing returned */
    OSPT_DEST *ospvDest,        /* destination */
    const char *ospvAddr)       /* network address as string */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAddr != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->NetworkId, ospvAddr, tr_min(OSPC_SIZE_NORID, OSPM_STRLEN(ospvAddr) + 1));
        }
    }
}

/*
 * OSPPDestGetNetworkAddr() - returns the network address for a destination device
 */
const char *OSPPDestGetNetworkAddr( /* returns address as string */
    OSPT_DEST *ospvDest)            /* destination in question */
{
    const char *nid = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        nid = ospvDest->NetworkId;
    }

    return nid;
}

/*
 * OSPPDestDevSetAddr() - sets the signalling address for a destination device
 */
void OSPPDestDevSetAddr(    /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination */
    const char *ospvAddr)   /* signal address as string */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAddr != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->DestinationDevAddr, ospvAddr, tr_min(OSPC_SIZE_SIGNALADDR, OSPM_STRLEN(ospvAddr) + 1));
        }
    }
}

/*
 * OSPPDestDevGetAddr() - returns the signaling address for a destination device
 */
const char *OSPPDestDevGetAddr( /* returns address as string */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    const char *addr = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        addr = ospvDest->DestinationDevAddr;
    }

    return addr;
}

/*
 * OSPPDestHasValidAfter() - Does a destination have a valid after time?
 */
OSPTBOOL OSPPDestHasValidAfter( /* returns non-zero if time */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->ValidAfter != OSPC_TIMEMIN);
    }

    return has;
}

/*
 * OSPPDestSetValidAfter() - sets the valid after time for a destination
 */
void OSPPDestSetValidAfter(     /* nothing returned */
    OSPT_DEST *ospvDest,
    OSPTTIME ospvTime)
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->ValidAfter = (ospvTime);
    }
}

/*
 * OSPPDestGetValidAfter() - returns valid after time for a destination
 */
OSPTTIME OSPPDestGetValidAfter( /* returns the time value */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    OSPTTIME time = 0;

    if (ospvDest != OSPC_OSNULL) {
        time = ospvDest->ValidAfter;
    }

    return time;
}

/*
 * OSPPDestHasValidUntil() - does destination have a valid until time?
 */
OSPTBOOL OSPPDestHasValidUntil( /* returns non-zero if time */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->ValidUntil != OSPC_TIMEMAX);
    }

    return has;
}

/*
 * OSPPDestSetValidUntil() - sets valid until time for destination
 */
void OSPPDestSetValidUntil( /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination in question */
    OSPTTIME ospvTime)      /* time to set */
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->ValidUntil = ospvTime;
    }
}

/*
 * OSPPDestGetValidUntil() - returns valid until time for destination
 */
OSPTTIME OSPPDestGetValidUntil( /* returns time */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    OSPTTIME time = 0;

    if (ospvDest != OSPC_OSNULL) {
        time = ospvDest->ValidUntil;
    }

    return time;
}

/*
 * OSPPDestHasAuthority() - does an authority URL exist for destination?
 */
OSPTBOOL OSPPDestHasAuthority(  /* returns non-zero if exists */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->Authority[0] != '\0');
    }

    return has;
}

/*
 * OSPPDestSetAuthority() - sets authority URL for destination
 */
void OSPPDestSetAuthority(  /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination in question */
    const char *ospvAuth)   /* authority URL */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAuth != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvDest->Authority, ospvAuth, tr_min(OSPM_STRLEN(ospvAuth) + 1, OSPC_SIZE_URL - 1));
        }
    }
}

/*
 * OSPPDestHasCallId() - does a destination have a Call ID?
 */
OSPTBOOL OSPPDestHasCallId( /* returns non-zero if exists */
    OSPT_DEST *ospvDest)    /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (ospvDest->CallId != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPDestGetCallId() - gets the call ID for a destination
 */
OSPT_CALL_ID *OSPPDestGetCallId(/* returns call ID pointer */
    OSPT_DEST *ospvDest)        /* destination in question */
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        callid = ospvDest->CallId;
    }

    return callid;
}

/*
 * OSPPDestHasToken() - does a destination have a Token?
 */
OSPTBOOL OSPPDestHasToken(  /* returns non-zero if exists */
    OSPT_DEST *ospvDest)    /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = (OSPPListFirst(&(ospvDest->Tokens)) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPDestAddToken() - adds a token to a destination
 */
void OSPPDestAddToken(      /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination in question */
    OSPT_TOKEN *ospvToken)  /* token to add */
{
    if (ospvDest != OSPC_OSNULL) {
        OSPPListAppend(&(ospvDest->Tokens), ospvToken);
    }
}

/*
 * OSPPDestFirstToken() - gets first token for destination
 */
OSPT_TOKEN *OSPPDestFirstToken( /* returns null if none */
    OSPT_DEST *ospvDest)
{
    OSPT_TOKEN *token = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        token = (OSPT_TOKEN *)OSPPListFirst(&ospvDest->Tokens);
    }

    return token;
}

/*
 * OSPPDestNextToken() - gets next token (in list) for destination
 */
OSPT_TOKEN *OSPPDestNextToken(  /* returns NULL if no more */
    OSPT_DEST *ospvDest,        /* destination in question */
    OSPT_TOKEN *ospvToken)      /* current token */
{
    OSPT_TOKEN *token = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        if (ospvToken != OSPC_OSNULL) {
            token = (OSPT_TOKEN *)OSPPListNext(&ospvDest->Tokens, ospvToken);
        }
    }

    return token;
}

/*
 * OSPPDestHasLimit() - does a destination have a usage limit?
 */
OSPTBOOL OSPPDestHasLimit(  /* returns non-zero if limit */
    OSPT_DEST *ospvDest)    /* destination in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = ospvDest->HasLimit;
    }

    return has;
}

/*
 * OSPPDestSetLimit() -  sets usage limit for a destination
 */
void OSPPDestSetLimit(      /* nothing returned */
    OSPT_DEST *ospvDest,    /* destination in question */
    unsigned ospvLimit)     /* limit to set */
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->Limit = ospvLimit;
        ospvDest->HasLimit = OSPC_TRUE;
    }
}

/*
 * OSPPDestGetLimit() - returns the usage limit for a destination
 */
unsigned OSPPDestGetLimit(  /* returns usage limit */
    OSPT_DEST *ospvDest)    /* destination in question */
{
    unsigned limit = 0;

    if (ospvDest != OSPC_OSNULL) {
        limit = ospvDest->Limit;
    }

    return limit;
}

/*
 * OSPPDestHasTermCause() - Does destination have the Fail Reason
 */
OSPTBOOL OSPPDestHasTermCause(      /* Returns non-zero if time */
    OSPT_DEST *ospvDest,            /* Destination in question */
    OSPE_TERM_CAUSE ospvType)       /* Termination cause type */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = OSPPHasTermCause(&ospvDest->TermCause, ospvType);
    }

    return has;
}

/*
 * OSPPDestHasTermCauseAny() - Does dest have a Fail Reason
 */
OSPTBOOL OSPPDestHasTermCauseAny(   /* returns non-zero if time */
    OSPT_DEST *ospvDest)            /* dest in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        has = OSPPHasTermCauseAny(&ospvDest->TermCause);
    }

    return has;
}

/*
 * OSPPDestSetTermCause() - Set Fail Reason
 */
void OSPPDestSetTermCause(  /* nothing returned */
    OSPT_DEST *ospvDest,
    OSPE_TERM_CAUSE ospvType,
    unsigned ospvTCCode,
    const char *ospvTCDesc)
{
    if (ospvDest != OSPC_OSNULL) {
        OSPPSetTermCause(&ospvDest->TermCause, ospvType, ospvTCCode, ospvTCDesc);
    }
}

/*
 * OSPPDestGetTermCause() - returns Fail Reason structure for an destination
 */
OSPT_TERM_CAUSE *OSPPDestGetTermCause(
    OSPT_DEST *ospvDest)    /* destination */
{
    OSPT_TERM_CAUSE *cause = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        cause = &ospvDest->TermCause;
    }

    return cause;
}

/*
 * OSPPDestGetTCCode() - returns Fail Reason value for an destination
 */
unsigned OSPPDestGetTCCode(
    OSPT_DEST *ospvDest,        /* destination */
    OSPE_TERM_CAUSE ospvType)   /* fail reason type */
{
    unsigned code = 0;

    if (ospvDest != OSPC_OSNULL) {
        code = OSPPGetTCCode(&ospvDest->TermCause, ospvType);
    }

    return code;
}

/*
 * OSPPDestGetTCDesc() - returns Fail Reason description for an destination
 */
const char *OSPPDestGetTCDesc(
    OSPT_DEST *ospvDest,        /* destination */
    OSPE_TERM_CAUSE ospvType)   /* fail reason type */
{
    const char *desc = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        desc = OSPPGetTCDesc(&ospvDest->TermCause, ospvType);
    }

    return desc;
}

/*
 * OSPPDestNew() - creates a new (empty) destination
 */
OSPT_DEST *OSPPDestNew(void)    /* returns pointer or NULL */
{
    OSPT_DEST *dest = OSPC_OSNULL;

    OSPM_MALLOC(dest, OSPT_DEST, sizeof(OSPT_DEST));

    if (dest != OSPC_OSNULL) {
        /* start with setting all to 0 */
        OSPM_MEMSET(dest, 0, sizeof(OSPT_DEST));
        OSPPListLinkNew(&(dest->Link));
        dest->ValidAfter = OSPC_TIMEMIN;
        dest->ValidUntil = OSPC_TIMEMAX;
        dest->Protocol = OSPC_PROTNAME_UNDEFINED;
        dest->OSPVersion = OSPC_DOSP_UNDEFINED;
        OSPPListNew(&(dest->Tokens));
        OSPPListNew(&(dest->UpdatedSourceAddr));
        OSPPListNew(&(dest->UpdatedDeviceInfo));
        dest->HasLimit = OSPC_FALSE;
        dest->Count = 0;
    }

    return dest;
}

/*
 * OSPPDestDelete() - deletes a Destination structure
 */
void OSPPDestDelete(
    OSPT_DEST **ospvDest)
{
    OSPT_TOKEN *tmptoken = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (*ospvDest != OSPC_OSNULL) {
        /* first delete the callid */
        if (((*ospvDest)->CallId) != OSPC_OSNULL) {
            OSPPCallIdDelete(&((*ospvDest)->CallId));
        }

        while (!OSPPListEmpty((OSPTLIST *)&((*ospvDest)->Tokens))) {
            tmptoken = (OSPT_TOKEN *)OSPPListRemove((OSPTLIST *)&((*ospvDest)->Tokens));
            if (tmptoken != OSPC_OSNULL) {
                OSPPTokenDelete(&tmptoken);
            }
        }
        OSPPListDelete((OSPTLIST *)&((*ospvDest)->Tokens));

        while (!OSPPListEmpty((OSPTLIST *)&((*ospvDest)->UpdatedSourceAddr))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove((OSPTLIST *)&((*ospvDest)->UpdatedSourceAddr));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }
        OSPPListDelete((OSPTLIST *)&((*ospvDest)->UpdatedSourceAddr));

        while (!OSPPListEmpty((OSPTLIST *)&((*ospvDest)->UpdatedDeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove((OSPTLIST *)&((*ospvDest)->UpdatedDeviceInfo));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }
        OSPPListDelete((OSPTLIST *)&((*ospvDest)->UpdatedDeviceInfo));
        /* finally delete the object */
        OSPM_FREE(*ospvDest);
        *ospvDest = OSPC_OSNULL;
    }
}

/*
 * OSPPDestFromElement() - get destination from an XML element
 */
int OSPPDestFromElement(        /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    OSPT_DEST **ospvDest)       /* where to put destination pointer */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPT_TOKEN *token = OSPC_OSNULL;
    OSPT_CALL_ID *callid = OSPC_OSNULL;
    unsigned long failure = 0L;
    OSPTTIME t = 0L;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvDest == OSPC_OSNULL) {
        error = OSPC_ERR_DATA_NO_DEST;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* create the destination object */
        dest = OSPPDestNew();
        if (dest == OSPC_OSNULL) {
            error = OSPC_ERR_DATA_NO_DEST;
        }

        /*
         * The Destination element should consist of several child
         * elements. We'll run through what's there and pick out
         * the information we need.
         */
        for (elem = OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR);
            elem = OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_PROTOCOL:
                OSPPDestProtocolFromElement(elem, dest);
                break;
            case OSPC_MELEM_DESTOSPVERSION:
                OSPPDestSetOSPVersion(dest, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_SRCINFO:
                OSPPDestSetSrcNumber(dest, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_DESTINFO:
                OSPPDestInfoFromElement(elem, dest);
                break;
            case OSPC_MELEM_DESTALT:
                OSPPDestSetNetworkAddr(dest, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_DESTSIGADDR:
                OSPPDestSetAddr(dest, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_TOKEN:
                if ((error = OSPPTokenFromElement(elem, &token)) == OSPC_ERR_NO_ERROR) {
                    OSPPDestAddToken(dest, token);
                }
                break;
            case OSPC_MELEM_VALIDAFTER:
                t = 0;
                if (*(OSPPXMLElemGetValue(elem)) != 0) {
                    error = OSPPMsgTimeFromElement(elem, &t);
                } else {
                    t = OSPC_TIMEMIN;
                }
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPDestSetValidAfter(dest, t);
                }
                break;
            case OSPC_MELEM_VALIDUNTIL:
                t = 0;
                if (*(OSPPXMLElemGetValue(elem)) != 0) {
                    error = OSPPMsgTimeFromElement(elem, &t);
                } else {
                    t = OSPC_TIMEMAX;
                }
                if (error == OSPC_ERR_NO_ERROR) {
                    OSPPDestSetValidUntil(dest, t);
                }
                break;
            case OSPC_MELEM_USAGEDETAIL:
                error = OSPPUsageDetailFromElement(elem, dest);
                break;
            case OSPC_MELEM_AUTHURL:
                OSPPDestSetAuthority(dest, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_CALLID:
                if ((error = OSPPCallIdFromElement(elem, &callid)) == OSPC_ERR_NO_ERROR) {
                    OSPPDestSetCallId(dest, callid->Value, callid->Length);
                    OSPPCallIdDelete(&callid);
                }
                break;
            case OSPC_MELEM_TERMCAUSE:
                if ((error = OSPPMsgNumFromElement(elem, &failure)) == OSPC_ERR_NO_ERROR) {
                    OSPPDestSetTermCause(dest, OSPC_TCAUSE_Q850, (unsigned)failure, OSPC_OSNULL);
                }
                break;
            default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem)) {
                    error = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }

        if (error == OSPC_ERR_NO_ERROR) {
            *ospvDest = dest;
        }
    }

    return error;
}

/*
 * OSPPDestSetCallId() - sets the call ID for a destination
 */
void OSPPDestSetCallId(             /* nothing returned */
    OSPT_DEST *ospvDest,            /* destination */
    const unsigned char *ospvValue, /* call ID value */
    unsigned ospvLen)
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvValue != OSPC_OSNULL) {
            if (ospvDest->CallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvDest->CallId));
            }
            ospvDest->CallId = OSPPCallIdNew(ospvLen, ospvValue);
        }
    }
}

void OSPPDestSetDestinationCount(
    OSPT_DEST *ospvDest,
    unsigned ospvCount)
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->Count = ospvCount;
    }
}

unsigned OSPPDestGetDestinationCount(
    OSPT_DEST *ospvDest)
{
    unsigned count = 0;

    if (ospvDest != OSPC_OSNULL) {
        count = ospvDest->Count;
    }

    return count;
}

OSPE_PROTOCOL_NAME OSPPDestProtocolGetPart(
    const char *ospvName)
{
    OSPE_PROTOCOL_NAME part = OSPC_PROTNAME_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        part = (OSPE_PROTOCOL_NAME)OSPPMsgDescGetPart(ospvName, OSPV_PROTNAME_DESCS, OSPC_PROTNAME_NUMBER);
    }

    return part;
}

const char *OSPPDestProtocolGetName(
    OSPE_PROTOCOL_NAME ospvPart)
{
    const char *name = OSPC_OSNULL;

    if ((ospvPart >= OSPC_PROTNAME_START) && (ospvPart < OSPC_PROTNAME_NUMBER)) {
        name = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_PROTNAME_DESCS, OSPC_PROTNAME_NUMBER);
    }

    return name;
}

void OSPPDestProtocolFromElement(
    OSPT_XML_ELEM *ospvElem,
    OSPT_DEST *ospvDest)
{
    OSPT_XML_ATTR* attr = OSPC_OSNULL;
    OSPE_PROTOCOL_TYPE type = OSPC_ALTINFO_DESTINATION;

    for (attr = (OSPT_XML_ATTR*)OSPPXMLElemFirstAttr(ospvElem);
        (attr != OSPC_OSNULL);
        attr = (OSPT_XML_ATTR*)OSPPXMLElemNextAttr(ospvElem, attr))
    {
        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_TYPE) {
            type = OSPPAltInfoTypeGetPart(OSPPXMLAttrGetValue(attr));
        }
    }
    switch (type) {
    case OSPC_ALTINFO_DESTINATION:
    case OSPC_ALTINFO_NA:
        OSPPDestSetProtocol(ospvDest, OSPPXMLElemGetValue(ospvElem));
        break;
    case OSPC_ALTINFO_SOURCE:
    default:
        break;
    }
}

void OSPPDestInfoFromElement(
    OSPT_XML_ELEM *ospvElem,
    OSPT_DEST *ospvDest)
{
    OSPT_XML_ATTR* attr = OSPC_OSNULL;

    for (attr = (OSPT_XML_ATTR*)OSPPXMLElemFirstAttr(ospvElem);
        (attr != OSPC_OSNULL);
        attr = (OSPT_XML_ATTR*)OSPPXMLElemNextAttr(ospvElem, attr))
    {
        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_TYPE) {
            switch (OSPPAltInfoTypeGetPart(OSPPXMLAttrGetValue(attr))) {
            case OSPC_ALTINFO_E164:
                OSPM_STRNCPY(ospvDest->DestinationNumber, OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->DestinationNumber) - 1);
                break;
            case OSPC_ALTINFO_NPRN:
                OSPM_STRNCPY(ospvDest->NPRn, OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->NPRn) - 1);
                break;
            case OSPC_ALTINFO_NPCIC:
                OSPM_STRNCPY(ospvDest->NPCic, OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->NPCic) - 1);
                break;
            case OSPC_ALTINFO_NPDI:
                if (OSPM_STRCASECMP(OSPPXMLElemGetValue(ospvElem), OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRUE)) == 0) {
                    ospvDest->NPNpdi = OSPC_TRUE;
                } else {
                    ospvDest->NPNpdi = OSPC_FALSE;
                }
                break;
            case OSPC_ALTINFO_SPID:
                OSPM_STRNCPY(ospvDest->OpName[OSPC_OPNAME_SPID], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->OpName[OSPC_OPNAME_SPID]) - 1);
                break;
            case OSPC_ALTINFO_OCN:
                OSPM_STRNCPY(ospvDest->OpName[OSPC_OPNAME_OCN], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->OpName[OSPC_OPNAME_OCN]) - 1);
                break;
            case OSPC_ALTINFO_SPN:
                OSPM_STRNCPY(ospvDest->OpName[OSPC_OPNAME_SPN], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->OpName[OSPC_OPNAME_SPN]) - 1);
                break;
            case OSPC_ALTINFO_ALTSPN:
                OSPM_STRNCPY(ospvDest->OpName[OSPC_OPNAME_ALTSPN], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->OpName[OSPC_OPNAME_ALTSPN]) - 1);
                break;
            case OSPC_ALTINFO_MCC:
                OSPM_STRNCPY(ospvDest->OpName[OSPC_OPNAME_MCC], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->OpName[OSPC_OPNAME_MCC]) - 1);
                break;
            case OSPC_ALTINFO_MNC:
                OSPM_STRNCPY(ospvDest->OpName[OSPC_OPNAME_MNC], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->OpName[OSPC_OPNAME_MNC]) - 1);
                break;
            case OSPC_ALTINFO_SIP:
                OSPM_STRNCPY(ospvDest->URL[OSPC_URL_SIP], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->URL[OSPC_URL_SIP]) - 1);
                break;
            case OSPC_ALTINFO_SMS:
                OSPM_STRNCPY(ospvDest->URL[OSPC_URL_SMS], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->URL[OSPC_URL_SMS]) - 1);
                break;
            case OSPC_ALTINFO_MMS:
                OSPM_STRNCPY(ospvDest->URL[OSPC_URL_MMS], OSPPXMLElemGetValue(ospvElem), sizeof(ospvDest->URL[OSPC_URL_MMS]) - 1);
                break;
            default:
                break;
            }
        }
    }
}

int OSPPUsageDetailFromElement(
    OSPT_XML_ELEM *ospvElem,
    OSPT_DEST *ospvDest)
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPTBOOL hasamount = OSPC_FALSE;
    OSPTBOOL hasincrement = OSPC_FALSE;
    OSPTBOOL hasunit = OSPC_FALSE;
    unsigned long amount = 0;
    unsigned long increment = 0;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvDest == OSPC_OSNULL) {
            error = OSPC_ERR_DATA_NO_DEST;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        for (elem = OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR);
            elem = OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_AMOUNT:
                if ((error = OSPPMsgNumFromElement(elem, &amount)) == OSPC_ERR_NO_ERROR) {
                    hasamount = OSPC_TRUE;
                }
                break;
            case OSPC_MELEM_INCREMENT:
                if ((error = OSPPMsgNumFromElement(elem, &increment)) == OSPC_ERR_NO_ERROR) {
                    hasincrement = OSPC_TRUE;
                }
                break;
            case OSPC_MELEM_UNIT:
                /* The only unit we support is seconds. If this is different, then we declare an error. */
                if (OSPM_STRCMP(OSPPXMLElemGetValue(elem), "s") != 0) {
                    error = OSPC_ERR_BAD_SERVICE;
                } else {
                    hasunit = OSPC_TRUE;
                }
                break;
            case OSPC_MELEM_SERVICE:
                OSPPServiceFromElement(elem, ospvDest);
                break;
            default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem)) {
                    error = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (hasamount && hasincrement && hasunit) {
            OSPPDestSetLimit(ospvDest, (unsigned)(increment * amount));
        } else {
            if (ospvDest->IsNPQuery) {
                OSPPDestSetLimit(ospvDest, 0);
            } else {
                error = OSPC_ERR_XML_BAD_ELEMENT;
            }
        }
    }

    return error;
}

void OSPPServiceFromElement(
    OSPT_XML_ELEM *ospvElem,
    OSPT_DEST *ospvDest)
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR* attr = OSPC_OSNULL;

    for (elem = OSPPXMLElemFirstChild(ospvElem);
        (elem != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR);
        elem = OSPPXMLElemNextChild(ospvElem, elem))
    {
        switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
        case OSPC_MELEM_SERVICETYPE:
            for (attr = OSPPXMLElemFirstAttr(elem); (attr != OSPC_OSNULL); attr = OSPPXMLElemNextAttr(elem, attr)) {
                if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_TYPE) {
                    switch (OSPPServiceGetPart(OSPPXMLAttrGetValue(attr))) {
                    case OSPC_SERVICE_NPQUERY:
                        ospvDest->IsNPQuery = OSPC_TRUE;
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
        default:
            break;
        }
    }
}
