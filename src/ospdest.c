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
 * ospdest.c - OSP destination functions
 */

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

/*
 * OSPPDestSetOSPVersion() - set the OSP Version for a destination
 */
void OSPPDestSetOSPVersion(     /* nothing returned */
    OSPTDEST *ospvDest,         /* destination to set */
    const char *ospvVersion)    /* Version (as string) */
{
    if (!(OSPM_STRCMP(ospvVersion, DEST_OSP_DIABLED))) {
        ospvDest->ospmDestOSPVersion = OSPC_DOSP_FALSE;
    } else {
        if (!(OSPM_STRCMP(ospvVersion, DEST_OSP_UNKNOWN))) {
            ospvDest->ospmDestOSPVersion = OSPC_DOSP_UNKNOWN;
        } else {
            ospvDest->ospmDestOSPVersion = OSPC_DOSP_TRUE;
        }
    }
}

/*
 * OSPPDestSetProtocol() - set the Protocol for a destination
 */
void OSPPDestSetProtocol(   /* nothing returned */
    OSPTDEST *ospvDest,     /* destination to set */
    const char *ospvProt)   /* Protocol (as string) */
{
    if (!(OSPM_STRCMP(ospvProt, OSPC_DPDESC_SIP))) {
        ospvDest->ospmDestProtocol = OSPC_DPROT_SIP;
    } else if (!(OSPM_STRCMP(ospvProt, OSPC_DPDESC_LRQ))) {
        ospvDest->ospmDestProtocol = OSPC_DPROT_LRQ;
    } else if (!(OSPM_STRCMP(ospvProt, OSPC_DPDESC_Q931))) {
        ospvDest->ospmDestProtocol = OSPC_DPROT_Q931;
    } else if (!(OSPM_STRCMP(ospvProt, OSPC_DPDESC_IAX))) {
        ospvDest->ospmDestProtocol = OSPC_DPROT_IAX;
    } else if (!(OSPM_STRCMP(ospvProt, OSPC_DPDESC_XMPP))) {
        ospvDest->ospmDestProtocol = OSPC_DPROT_XMPP;
    } else {
        ospvDest->ospmDestProtocol = OSPC_DPROT_UNKNOWN;
    }
}

/*
 * OSPPDestHasNumber() - does the destination include a called number?
 */
OSPTBOOL OSPPDestHasNumber(/* returns non-zero if number exists */
    OSPTDEST *ospvDest)    /* destination effected */
{
    OSPTBOOL ospvHasNumber = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasNumber = (ospvDest->ospmDestNumber[0] != '\0');
    }

    return ospvHasNumber;
}

/*
 * OSPPDestHasSrcNumber() - does the destination include a calling number?
 */
OSPTBOOL OSPPDestHasSrcNumber(  /* returns non-zero if number exists */
    OSPTDEST *ospvDest)         /* destination effected */
{
    OSPTBOOL ospvHasSrcNumber = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasSrcNumber = (ospvDest->ospmSrcNumber[0] != '\0');
    }

    return ospvHasSrcNumber;
}

/*
 * OSPPDestSetSrcNumber() - set the calling number for a destination
 */
void OSPPDestSetSrcNumber(  /* nothing returned */
    OSPTDEST *ospvDest,     /* destination to set */
    const char *ospvNum)    /* calling number (as string) */
{
    size_t len = 0;

    len = 0;
    if (ospvDest != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->ospmSrcNumber, ospvNum, tr_min(OSPC_SIZE_E164NUM, OSPM_STRLEN(ospvNum) + 1));
        }
    }
}

/*
 * OSPPDestGetSrcNumber() - returns the calling number for a destination
 */
const char *OSPPDestGetSrcNumber(   /* returns number as string */
    OSPTDEST *ospvDest)             /* destination */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        ospvNum = ospvDest->ospmSrcNumber;
    }

    return ospvNum;
}

/*
 * OSPPDestSetNumber() - set the called number for a destination
 */
void OSPPDestSetNumber(     /* nothing returned */
    OSPTDEST *ospvDest,     /* destination to set */
    const char *ospvNum)    /* called number (as string) */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->ospmDestNumber, ospvNum, tr_min(OSPC_SIZE_E164NUM, OSPM_STRLEN(ospvNum) + 1));
        }
    }
}

/*
 * OSPPDestGetNumber() - returns the called number for a destination
 */
const char *OSPPDestGetNumber(  /* returns number as string */
    OSPTDEST *ospvDest)         /* destination */
{
    const char *ospvNum = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        ospvNum = ospvDest->ospmDestNumber;
    }

    return ospvNum;
}

/*
 * OSPPDestHasAddr() - does a destination have a Addr?
 */
OSPTBOOL OSPPDestHasAddr(   /* returns non-zero if exists */
    OSPTDEST *ospvDest)     /* destination in question */
{
    OSPTBOOL ospvHasAddr = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasAddr = (ospvDest->ospmDestAddr[0] != '\0');
    }

    return ospvHasAddr;
}

/*
 * OSPPDestSetAddr() - sets the signalling address for a destination
 */
void OSPPDestSetAddr(       /* nothing returned */
    OSPTDEST *ospvDest,     /* destination */
    const char *ospvAddr)   /* signal address as string */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAddr != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->ospmDestAddr, ospvAddr, tr_min(OSPC_SIZE_SIGNALADDR, OSPM_STRLEN(ospvAddr) + 1));
        }
    }
}

/*
 * OSPPDestGetAddr() - returns the signalling address for a destination
 */
const char *OSPPDestGetAddr(    /* returns address as string */
    OSPTDEST *ospvDest)         /* destination in question */
{
    const char *ospvAddr = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        ospvAddr = ospvDest->ospmDestAddr;
    }

    return ospvAddr;
}

/*
 * OSPPDestDevHasAddr() - does a destination have a Dev Addr?
 */
OSPTBOOL OSPPDestDevHasAddr(    /* returns non-zero if exists */
    OSPTDEST *ospvDest)         /* destination in question */
{
    OSPTBOOL ospvHasAddr = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasAddr = (ospvDest->ospmDestDevAddr[0] != '\0');
    }
    return ospvHasAddr;
}

/*
 * OSPPDestHasNetworkAddr() - does a destination have a Network Addr?
 */
OSPTBOOL OSPPDestHasNetworkAddr(    /* returns non-zero if exists */
    OSPTDEST *ospvDest)             /* destination in question */
{
    OSPTBOOL ospvHasAddr = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasAddr = (ospvDest->ospmDestNetworkId[0] != '\0');
    }
    return ospvHasAddr;
}

/*
 * OSPPDestSetNetworkAddr() - sets the network address for a destination 
 *  device
 */
void OSPPDestSetNetworkAddr(    /* nothing returned */
    OSPTDEST *ospvDest,         /* destination */
    const char *ospvAddr)       /* network address as string */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAddr != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->ospmDestNetworkId, ospvAddr, tr_min(OSPC_NETWORKIDSIZE, OSPM_STRLEN(ospvAddr) + 1));
        }
    }
}

/*
 * OSPPDestGetNetworkAddr() - returns the network address for a destination device
 */
const char *OSPPDestGetNetworkAddr( /* returns address as string */
    OSPTDEST *ospvDest)             /* destination in question */
{
    const char *ospvAddr = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        ospvAddr = ospvDest->ospmDestNetworkId;
    }

    return ospvAddr;
}

/*
 * OSPPDestDevSetAddr() - sets the signalling address for a destination device
 */
void OSPPDestDevSetAddr(    /* nothing returned */
    OSPTDEST *ospvDest,     /* destination */
    const char *ospvAddr)   /* signal address as string */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAddr != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvDest->ospmDestDevAddr, ospvAddr, tr_min(OSPC_SIZE_SIGNALADDR, OSPM_STRLEN(ospvAddr) + 1));
        }
    }
}

/*
 * OSPPDestDevGetAddr() - returns the signalling address for a destination device
 */
const char *OSPPDestDevGetAddr( /* returns address as string */
    OSPTDEST *ospvDest)         /* destination in question */
{
    const char *ospvAddr = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        ospvAddr = ospvDest->ospmDestDevAddr;
    }

    return ospvAddr;
}

/*
 * OSPPDestHasValidAfter() - Does a destination have a valid after time?
 */
OSPTBOOL OSPPDestHasValidAfter( /* returns non-zero if time */
    OSPTDEST *ospvDest)         /* destination in question */
{
    OSPTBOOL ospvHasTime = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasTime = (ospvDest->ospmDestValidAfter != OSPC_TIMEMIN);
    }

    return ospvHasTime;
}

/*
 * OSPPDestSetValidAfter() - sets the valid after time for a destination
 */
void OSPPDestSetValidAfter(     /* nothing returned */
    OSPTDEST *ospvDest,
    OSPTTIME ospvTime)
{
    if (ospvDest != OSPC_OSNULL) {
        (ospvDest)->ospmDestValidAfter = (ospvTime);
    }
}

/*
 * OSPPDestGetValidAfter() - returns valid after time for a destination
 */
OSPTTIME OSPPDestGetValidAfter( /* returns the time value */
    OSPTDEST *ospvDest)         /* destination in question */
{
    OSPTTIME ospvTime = 0;

    if (ospvDest != OSPC_OSNULL) {
        ospvTime = ospvDest->ospmDestValidAfter;
    }

    return ospvTime;
}

/*
 * OSPPDestHasValidUntil() - does destination have a valid until time?
 */
OSPTBOOL OSPPDestHasValidUntil( /* returns non-zero if time */
    OSPTDEST *ospvDest)         /* destination in question */
{
    OSPTBOOL ospvHasTime = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasTime = (ospvDest->ospmDestValidUntil != OSPC_TIMEMAX);
    }
    return ospvHasTime;
}

/*
 * OSPPDestSetValidUntil() - sets valid until time for destination
 */
void OSPPDestSetValidUntil( /* nothing returned */
    OSPTDEST *ospvDest,     /* destination in question */
    OSPTTIME ospvTime)      /* time to set */
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->ospmDestValidUntil = ospvTime;
    }
}

/*
 * OSPPDestGetValidUntil() - returns valid until time for destination
 */
OSPTTIME OSPPDestGetValidUntil( /* returns time */
    OSPTDEST *ospvDest)         /* destination in question */
{
    OSPTTIME ospvTime = 0;

    if (ospvDest != OSPC_OSNULL) {
        ospvTime = ospvDest->ospmDestValidUntil;
    }
    return ospvTime;
}

/*
 * OSPPDestHasAuthority() - does an authority URL exist for destination?
 */
OSPTBOOL OSPPDestHasAuthority(  /* returns non-zero if exists */
    OSPTDEST *ospvDest)         /* destination in question */
{
    OSPTBOOL ospvHasAuth = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasAuth = (ospvDest->ospmDestAuthority[0] != '\0');
    }
    return ospvHasAuth;
}

/*
 * OSPPDestSetAuthority() - sets authority URL for destination
 */
void OSPPDestSetAuthority(  /* nothing returned */
    OSPTDEST *ospvDest,     /* destination in question */
    const char *ospvAuth)   /* authority URL */
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvAuth != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvDest->ospmDestAuthority, ospvAuth, tr_min(OSPM_STRLEN(ospvAuth) + 1, OSPC_SIZE_URL - 1));
        }
    }
}

/*
 * OSPPDestHasCallId() - does a destination have a Call ID?
 */
OSPTBOOL OSPPDestHasCallId( /* returns non-zero if exists */
    OSPTDEST *ospvDest      /* destination in question */
    )
{
    OSPTBOOL ospvHasId = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasId = (ospvDest->ospmDestCallId != OSPC_OSNULL);
    }

    return ospvHasId;
}

/*
 * OSPPDestGetCallId() - gets the call ID for a destination
 */
OSPTCALLID *OSPPDestGetCallId(  /* returns call ID pointer */
    OSPTDEST *ospvDest          /* destination in question */
    )
{
    OSPTCALLID *ospvCallId = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        ospvCallId = ospvDest->ospmDestCallId;
    }

    return ospvCallId;
}

/*
 * OSPPDestHasToken() - does a destination have a Token?
 */
OSPTBOOL OSPPDestHasToken(  /* returns non-zero if exists */
    OSPTDEST *ospvDest)     /* destination in question */
{
    OSPTBOOL ospvHasToken = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasToken = (OSPPListFirst(&(ospvDest->ospmDestTokens)) != OSPC_OSNULL);
    }

    return ospvHasToken;
}

/*
 * OSPPDestAddToken() - adds a token to a destination
 */
void OSPPDestAddToken(      /* nothing returned */
    OSPTDEST *ospvDest,     /* destination in question */
    OSPTTOKEN *ospvToken)   /* token to add */
{
    if (ospvDest != OSPC_OSNULL) {
        OSPPListAppend(&(ospvDest->ospmDestTokens), ospvToken);
    }
}

/*
 * OSPPDestFirstToken() - gets first token for destination
 */
OSPTTOKEN *OSPPDestFirstToken(  /* returns null if none */
    OSPTDEST *ospvDest)
{
    OSPTTOKEN *ospvToken = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        ospvToken = (OSPTTOKEN *)OSPPListFirst(&ospvDest->ospmDestTokens);
    }

    return ospvToken;
}

/*
 * OSPPDestNextToken() - gets next token (in list) for destination
 */
OSPTTOKEN *OSPPDestNextToken(   /* returns NULL if no more */
    OSPTDEST *ospvDest,         /* destination in question */
    OSPTTOKEN *ospvToken)       /* current token */
{
    OSPTTOKEN *ospvNextToken = OSPC_OSNULL;

    if (ospvDest != OSPC_OSNULL) {
        if (ospvToken != OSPC_OSNULL) {
            ospvToken = (OSPTTOKEN *)OSPPListNext(&ospvDest->ospmDestTokens, ospvToken);
        }
    }

    return ospvNextToken;
}

/*
 * OSPPDestHasLimit() - does a destination have a usage limit?
 */
OSPTBOOL OSPPDestHasLimit(  /* returns non-zero if limit */
    OSPTDEST *ospvDest)     /* destination in question */
{
    OSPTBOOL ospvHasLimit = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasLimit = ospvDest->ospmDestHasLimit;
    }

    return ospvHasLimit;
}

/*
 * OSPPDestSetLimit() -  sets usage limit for a destination
 */
void OSPPDestSetLimit(      /* nothing returned */
    OSPTDEST *ospvDest,     /* destination in question */
    unsigned ospvLimit)     /* limit to set */
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->ospmDestLimit = ospvLimit;
        ospvDest->ospmDestHasLimit = OSPC_TRUE;
    }
}

/*
 * OSPPDestGetLimit() - returns the usage limit for a destination
 */
unsigned OSPPDestGetLimit(  /* returns usage limit */
    OSPTDEST *ospvDest)     /* destination in question */
{
    unsigned ospvLimit = 0;

    if (ospvDest != OSPC_OSNULL) {
        ospvLimit = ospvDest->ospmDestLimit;
    }

    return ospvLimit;
}

/*
 * OSPPDestHasFailReason() - Does dest have a Fail Reason
 */
OSPTBOOL OSPPDestHasFailReason(     /* returns non-zero if time */
    OSPTDEST *ospvDest)             /* dest in question */
{
    OSPTBOOL ospvHasFailReason = OSPC_FALSE;

    if (ospvDest != OSPC_OSNULL) {
        ospvHasFailReason = ospvDest->ospmDestHasFailReason;
    }

    return ospvHasFailReason;
}

/*
 * OSPPDestSetFailReason() - Set Fail Reason
 */
void OSPPDestSetFailReason( /* nothing returned */
    OSPTDEST *ospvDest,
    unsigned ospvFailReason)
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->ospmDestFailReason = ospvFailReason;
        ospvDest->ospmDestHasFailReason = OSPC_TRUE;
    }
}

/*
 * OSPPDestGetFailReason() - returns Fail Reason for a dest
 */
unsigned OSPPDestGetFailReason( /* returns the fail reason value */
    OSPTDEST *ospvDest)         /* usage request */
{
    unsigned ospvFailReason = 0;

    if (ospvDest != OSPC_OSNULL) {
        ospvFailReason = ospvDest->ospmDestFailReason;
    }

    return ospvFailReason;
}

/*
 * OSPPDestNew() - creates a new (empty) destination
 */
OSPTDEST *OSPPDestNew(void) /* returns pointer or NULL */
{
    OSPTDEST *ospvDest = OSPC_OSNULL;

    OSPM_MALLOC(ospvDest, OSPTDEST, sizeof(OSPTDEST));

    if (ospvDest != OSPC_OSNULL) {
        /* start with setting all to 0 */
        OSPM_MEMSET(ospvDest, 0, sizeof(OSPTDEST));
        OSPPListLinkNew(&(ospvDest->ospmDestLink));
        ospvDest->ospmDestValidAfter = OSPC_TIMEMIN;
        ospvDest->ospmDestValidUntil = OSPC_TIMEMAX;
        ospvDest->ospmDestProtocol = OSPC_DPROT_UNDEFINED;
        ospvDest->ospmDestOSPVersion = OSPC_DOSP_UNDEFINED;
        OSPPListNew(&(ospvDest->ospmDestTokens));
        OSPPListNew(&(ospvDest->ospmUpdatedSourceAddr));
        OSPPListNew(&(ospvDest->ospmUpdatedDeviceInfo));
        ospvDest->ospmDestHasLimit = OSPC_FALSE;
        ospvDest->ospmDestDestinationCount = 0;
    }

    return ospvDest;
}

/*
 * OSPPDestDelete() - deletes a Destination structure
 */
void OSPPDestDelete(
    OSPTDEST ** ospvDest)
{
    /*   OSPTTOKEN *token, *otoken = OSPC_OSNULL; */
    OSPTTOKEN *tmptoken = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (*ospvDest != OSPC_OSNULL) {
        /* first delete the callid */
        if (((*ospvDest)->ospmDestCallId) != OSPC_OSNULL) {
            OSPPCallIdDelete(&((*ospvDest)->ospmDestCallId));
        }

        while (!OSPPListEmpty((OSPTLIST *) & ((*ospvDest)->ospmDestTokens))) {
            tmptoken = (OSPTTOKEN *)OSPPListRemove((OSPTLIST *)&((*ospvDest)->ospmDestTokens));
            if (tmptoken != (OSPTTOKEN *) OSPC_OSNULL) {
                OSPPTokenDelete(&tmptoken);
            }
        }
        OSPPListDelete((OSPTLIST *)&((*ospvDest)->ospmDestTokens));

        while (!OSPPListEmpty((OSPTLIST *) & ((*ospvDest)->ospmUpdatedSourceAddr))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove((OSPTLIST *)&((*ospvDest)->ospmUpdatedSourceAddr));
            if (altinfo != (OSPT_ALTINFO *) OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }
        OSPPListDelete((OSPTLIST *) & ((*ospvDest)->ospmUpdatedSourceAddr));

        while (!OSPPListEmpty((OSPTLIST *)&((*ospvDest)->ospmUpdatedDeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove((OSPTLIST *)&((*ospvDest)->ospmUpdatedDeviceInfo));
            if (altinfo != (OSPT_ALTINFO *) OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }
        OSPPListDelete((OSPTLIST *)&((*ospvDest)->ospmUpdatedDeviceInfo));
        /* finally delete the object */
        OSPM_FREE(*ospvDest);
        *ospvDest = OSPC_OSNULL;
    }
}

/*
 * OSPPDestFromElement() - get destination from an XML element
 */
unsigned OSPPDestFromElement(   /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    OSPTDEST **ospvDest)        /* where to put destination pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPTDEST *dest = OSPC_OSNULL;
    OSPTTOKEN *token = OSPC_OSNULL;
    OSPTCALLID *callId = OSPC_OSNULL;
    unsigned limit = 0;
    unsigned long failure = 0L;
    OSPTTIME t = 0L;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvDest == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_DEST;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the destination object */
        dest = OSPPDestNew();

        if (dest == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_DATA_NO_DEST;
        }

        /*
         * The Destination element should consist of several child
         * elements. We'll run through what's there and pick out
         * the information we need.
         */

        for (elem = (OSPT_XML_ELEM *) OSPPXMLElemFirstChild(ospvElem);
            (elem != (OSPT_XML_ELEM *) OSPC_OSNULL) &&
            (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPT_XML_ELEM *) OSPPXMLElemNextChild(ospvElem, elem)) 
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
                case OSPC_MELEM_DESTPROTOCOL:
                    OSPPDestSetProtocol(dest, OSPPXMLElemGetValue(elem));
                    break;
                case OSPC_MELEM_DESTOSPVERSION:
                    OSPPDestSetOSPVersion(dest, OSPPXMLElemGetValue(elem));
                    break;
                case OSPC_MELEM_SRCINFO:
                    OSPPDestSetSrcNumber(dest, OSPPXMLElemGetValue(elem));
                    break;
                case OSPC_MELEM_DESTINFO:
                    OSPPDestSetNumber(dest, OSPPXMLElemGetValue(elem));
                    break;
                case OSPC_MELEM_DESTALT:
                    /* OSPPDestSetAddr(dest, OSPPXMLElemGetValue(elem)); */
                    OSPPDestSetNetworkAddr(dest, OSPPXMLElemGetValue(elem));
                    break;
                case OSPC_MELEM_DESTSIGADDR:
                    /* OSPPDestDevSetAddr(dest, OSPPXMLElemGetValue(elem)); */
                    OSPPDestSetAddr(dest, OSPPXMLElemGetValue(elem));
                    break;
                case OSPC_MELEM_TOKEN:
                    ospvErrCode = OSPPTokenFromElement(elem, &token);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPDestAddToken(dest, token);
                    }
                    break;
                case OSPC_MELEM_VALIDAFTER:
                    t = 0;
                    if (*(OSPPXMLElemGetValue(elem)) != 0) {
                        ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                    } else {
                        t = OSPC_TIMEMIN;
                    }
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPDestSetValidAfter(dest, t);
                    }
                    break;
                case OSPC_MELEM_VALIDUNTIL:
                    t = 0;
                    if (*(OSPPXMLElemGetValue(elem)) != 0) {
                        ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                    } else {
                        t = OSPC_TIMEMAX;
                    }
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPDestSetValidUntil(dest, t);
                    }
                    break;
                case OSPC_MELEM_USAGEDETAIL:
                    ospvErrCode = OSPPUsageFromElement(elem, &limit);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPDestSetLimit(dest, limit);
                    }
                    break;
                case OSPC_MELEM_AUTHURL:
                    OSPPDestSetAuthority(dest, OSPPXMLElemGetValue(elem));
                    break;
                case OSPC_MELEM_CALLID:
                    ospvErrCode = OSPPCallIdFromElement(elem, &callId);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPDestSetCallId(dest, callId->ospmCallIdVal, callId->ospmCallIdLen);
                        OSPPCallIdDelete(&callId);
                    }
                    break;
                case OSPC_MELEM_FAILREASON:
                    ospvErrCode = OSPPMsgNumFromElement(elem, &failure);
                    OSPPDestSetFailReason(dest, (unsigned)failure);
                    break;
                default:
                    /*
                     * This is an element we don't understand. If it's
                     * critical, then we have to report an error.
                     * Otherwise we can ignore it.
                     */
                    if (OSPPMsgElemIsCritical(elem)) {
                        ospvErrCode = OSPC_ERR_XML_BAD_ELEMENT;
                    }
                    break;
            }
        }

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            *ospvDest = dest;
        }
    }

    return ospvErrCode;
}

/*
 * OSPPDestSetCallId() - sets the call ID for a destination
 */
void OSPPDestSetCallId(             /* nothing returned */
    OSPTDEST *ospvDest,             /* destination */
    const unsigned char *ospvValue, /* call ID value */
    unsigned ospvLen)
{
    if (ospvDest != OSPC_OSNULL) {
        if (ospvValue != OSPC_OSNULL) {
            if (ospvDest->ospmDestCallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvDest->ospmDestCallId));
            }
            ospvDest->ospmDestCallId = OSPPCallIdNew(ospvLen, ospvValue);
        }
    }
}

void OSPPDestSetDestinationCount(
    OSPTDEST *ospvDest,
    unsigned ospvDestinationCount)
{
    if (ospvDest != OSPC_OSNULL) {
        ospvDest->ospmDestDestinationCount = ospvDestinationCount;
    }
}

unsigned OSPPDestGetDestinationCount(OSPTDEST *ospvDest)
{
    unsigned ospvTheValue = 0;

    if (ospvDest != OSPC_OSNULL) {
        ospvTheValue = ospvDest->ospmDestDestinationCount;
    }

    return ospvTheValue;
}
