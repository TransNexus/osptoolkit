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

/* osptokeninfo.c - OSP token info functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/osptokeninfo.h"
#include "osp/ospusage.h"
#include "osp/osputils.h"

/*
 * OSPPTokenInfoSetLookAheadDestAlt() - sets the look ahead route
 */
void OSPPTokenInfoSetLookAheadDestAlt(              /* nothing returned */
    OSPTTOKENLOOKAHEADINFO *ospvTokenLookAheadInfo, /* token info to set */
    const char *ospvLookAheadRoute)                 /* Look Ahead Route */
{
    if (ospvTokenLookAheadInfo != OSPC_OSNULL) {
        if (ospvLookAheadRoute != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvTokenLookAheadInfo->lookAheadDest, ospvLookAheadRoute, OSPC_SIZE_SIGNALADDR - 1);
        }
    }
}

/*
 * OSPPTokenInfoGetLookAheadDestAlt() - returns the look ahead route.
 */
const char *OSPPTokenInfoGetLookAheadDestAlt(
    OSPTTOKENLOOKAHEADINFO *ospvTokenLookAheadInfo)     /* token info */
{
    const char *destination = OSPC_OSNULL;

    if (ospvTokenLookAheadInfo != OSPC_OSNULL) {
        destination = ospvTokenLookAheadInfo->lookAheadDest;
    }

    return destination;
}

/*
 * OSPPTokenInfoSetLookAheadProtocol() - sets the signaling protocol for the look ahead route.
 */
void OSPPTokenInfoSetLookAheadProtocol(                 /* nothing returned */
    OSPTTOKENLOOKAHEADINFO *ospvTokenLookAheadInfo,     /* token info to set */
    const char *ospvProtocol)
{
    if (ospvTokenLookAheadInfo != OSPC_OSNULL) {
        ospvTokenLookAheadInfo->lookAheadProt = OSPPDestProtocolGetPart(ospvProtocol);
    }
}

/*
 * OSPPTokenInfoGetLookAheadProtocol() - returns the signaling protocol for the look ahead route.
 */
OSPE_PROTOCOL_NAME OSPPTokenInfoGetLookAheadProtocol(
    OSPTTOKENLOOKAHEADINFO *ospvTokenLookAheadInfo)     /* token info */
{
    OSPE_PROTOCOL_NAME protocol = OSPC_PROTNAME_UNDEFINED;

    if (ospvTokenLookAheadInfo != OSPC_OSNULL) {
        protocol = ospvTokenLookAheadInfo->lookAheadProt;
    }

    return protocol;
}

/*
 * OSPPTokenInfoSetLookAheadOSPVersion() - sets the destination OSPVersion for the look ahead route.
 */
void OSPPTokenInfoSetLookAheadOSPVersion(           /* nothing returned */
    OSPTTOKENLOOKAHEADINFO *ospvTokenLookAheadInfo, /* token info to set */
    const char *ospvLookAheadDestOSPStatus)         /* Look Ahead Dest OSPVer */
{
    if (ospvTokenLookAheadInfo != OSPC_OSNULL) {
        if (!(OSPM_STRCMP(ospvLookAheadDestOSPStatus, DEST_OSP_DIABLED))) {
            ospvTokenLookAheadInfo->lookAheadDestOSPStatus = OSPC_DOSP_FALSE;
        } else if (!(OSPM_STRCMP(ospvLookAheadDestOSPStatus, DEST_OSP_UNKNOWN))) {
            ospvTokenLookAheadInfo->lookAheadDestOSPStatus = OSPC_DOSP_UNKNOWN;
        } else {
            ospvTokenLookAheadInfo->lookAheadDestOSPStatus = OSPC_DOSP_TRUE;
        }
    }
}

/*
 * OSPPTokenInfoGetLookAheadOSPVersion() - returns the destination OSPVersion for the look ahead route.
 */
OSPE_DEST_OSPENABLED OSPPTokenInfoGetLookAheadOSPVersion(
    OSPTTOKENLOOKAHEADINFO *ospvTokenLookAheadInfo)         /* token info */
{
    OSPE_DEST_OSPENABLED status = OSPC_DOSP_UNDEFINED;

    if (ospvTokenLookAheadInfo != OSPC_OSNULL) {
        status = ospvTokenLookAheadInfo->lookAheadDestOSPStatus;
    }

    return status;
}

/*
 * OSPPTokenInfoSetSourceNumber() - set the source number
 */
void OSPPTokenInfoSetSourceNumber(  /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,   /* token info to set */
    const char *ospvSourceNumber)   /* source number to set to */
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        if (ospvSourceNumber != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvTokenInfo->ospmTokenInfoSourceNumber, ospvSourceNumber, OSPC_SIZE_E164NUM - 1);
        }
    }
}

/*
 * OSPPTokenInfoGetSourceNumber() - returns the source number for token info
 */
const char *OSPPTokenInfoGetSourceNumber(
    OSPTTOKENINFO *ospvTokenInfo)   /* token info */
{
    const char *number = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL) {
        number = ospvTokenInfo->ospmTokenInfoSourceNumber;
    }

    return number;
}

/*
 * OSPPTokenInfoSetDestNumber() - set the destination number
 */
void OSPPTokenInfoSetDestNumber(    /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,   /* token info to set */
    const char *ospvDestNumber)     /* destination number to set to */
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        if (ospvDestNumber != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvTokenInfo->ospmTokenInfoDestNumber, ospvDestNumber, OSPC_SIZE_E164NUM - 1);
        }
    }
}

/*
 * OSPPTokenInfoGetDestNumber() - returns the destination number for a token info
 */
const char *OSPPTokenInfoGetDestNumber(
    OSPTTOKENINFO *ospvTokenInfo)       /* token info */
{
    const char *number = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL) {
        number = ospvTokenInfo->ospmTokenInfoDestNumber;
    }

    return number;
}

/*
 * OSPPTokenInfoSetDestNetworkId() - set the destination network id
 */
void OSPPTokenInfoSetDestNetworkId( /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,   /* token info to set */
    const char *ospvDestId)         /* destination id to set to */
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        if (ospvDestId != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvTokenInfo->dstNetworkId, ospvDestId, OSPC_SIZE_E164NUM - 1);
        }
    }
}

/*
 * OSPPTokenInfoGetDestNetworkId() - returns the destination network id for a token info
 */
const char *OSPPTokenInfoGetDestNetworkId(
    OSPTTOKENINFO *ospvTokenInfo)           /* token info */
{
    const char *dnid = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL) {
        dnid = ospvTokenInfo->dstNetworkId;
    }

    return dnid;
}

/*
 * OSPPTokenInfoHasCallId() - is the call id set ?
 */
OSPTBOOL OSPPTokenInfoHasCallId(    /* returns non-zero if number exists */
    OSPTTOKENINFO *ospvTokenInfo)   /* Token Info effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvTokenInfo != OSPC_OSNULL) {
        has = (ospvTokenInfo->ospmTokenInfoCallId != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPTokenInfoSetCallId() - set the call id
 */
void OSPPTokenInfoSetCallId(        /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,   /* token info to set */
    OSPT_CALL_ID *ospvCallId)       /* call id to set to */
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        if ((ospvCallId) != OSPC_OSNULL) {
            if ((ospvTokenInfo)->ospmTokenInfoCallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&((ospvTokenInfo)->ospmTokenInfoCallId));
            }
            ospvTokenInfo->ospmTokenInfoCallId = OSPPCallIdNew(ospvCallId->ospmCallIdLen, ospvCallId->ospmCallIdVal);
        }
    }
}

/*
 * OSPPTokenInfoGetCallId() - returns the call id for a token info
 */
OSPT_CALL_ID *OSPPTokenInfoGetCallId(
    OSPTTOKENINFO *ospvTokenInfo)   /* token info */
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL) {
        callid = ospvTokenInfo->ospmTokenInfoCallId;
    }

    return callid;
}

/*
 * OSPPTokenInfoGetCallIdSize() - returns the call id size for a token info
 */
unsigned OSPPTokenInfoGetCallIdSize(
    OSPTTOKENINFO *ospvTokenInfo)   /* token info */
{
    unsigned size = 0;

    if (ospvTokenInfo != OSPC_OSNULL) {
        size = OSPPCallIdGetSize(((ospvTokenInfo)->ospmTokenInfoCallId));
    }

    return size;
}

/*
 * OSPPTokenInfoGetCallIdValue() - returns the call id for a token info
 */
unsigned char *OSPPTokenInfoGetCallIdValue(
    OSPTTOKENINFO *ospvTokenInfo)           /* token info */
{
    unsigned char *value = OSPC_OSNULL;

    if (ospvTokenInfo != OSPC_OSNULL) {
        value = OSPPCallIdGetValue(ospvTokenInfo->ospmTokenInfoCallId);
    }

    return value;
}

/*
 * OSPPTokenInfoHasValidAfter() - Does TokenInfo have a valid after time?
 */
OSPTBOOL OSPPTokenInfoHasValidAfter(/* returns non-zero if time */
    OSPTTOKENINFO *ospvTokenInfo)   /* TokenInfo in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvTokenInfo != OSPC_OSNULL) {
        has = (ospvTokenInfo->ospmTokenInfoValidAfter != OSPC_TIMEMIN);
    }

    return has;
}

/*
 * OSPPTokenInfoSetValidAfter() - sets the valid after time
 */
void OSPPTokenInfoSetValidAfter(    /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,
    OSPTTIME ospvTime)
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        ospvTokenInfo->ospmTokenInfoValidAfter = ospvTime;
    }
}

/*
 * OSPPTokenInfoGetValidAfter() - returns valid after time
 */
OSPTTIME OSPPTokenInfoGetValidAfter(    /* returns the time value */
    OSPTTOKENINFO *ospvTokenInfo)       /* TokenInfo in question */
{
    OSPTTIME after = 0;

    if (ospvTokenInfo != OSPC_OSNULL) {
        after = ospvTokenInfo->ospmTokenInfoValidAfter;
    }

    return after;
}

/*
 * OSPPTokenInfoSetValidUntil() - sets valid until time
 */
void OSPPTokenInfoSetValidUntil(    /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,   /* TokenInfo in question */
    OSPTTIME ospvTime)              /* time to set */
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        ospvTokenInfo->ospmTokenInfoValidUntil = ospvTime;
    }
}

/*
 * OSPPTokenInfoGetValidUntil() - returns valid until time
 */
OSPTTIME OSPPTokenInfoGetValidUntil(/* returns time */
    OSPTTOKENINFO *ospvTokenInfo)   /* token in question */
{
    OSPTTIME until = 0;

    if (ospvTokenInfo != OSPC_OSNULL) {
        until = ospvTokenInfo->ospmTokenInfoValidUntil;
    }
    return until;
}

/*
 * OSPPTokenInfoSetTrxId() - set the transaction id
 */
void OSPPTokenInfoSetTrxId(         /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,   /* token info to set */
    OSPTTRXID ospvTrxId)            /* transaction id to set to */
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        if (ospvTrxId != 0) {
            ospvTokenInfo->ospmTokenInfoTrxId = ospvTrxId;
        }
    }
}

/*
 * OSPPTokenInfoGetTrxId() - returns the trans id for a token info
 */
OSPTTRXID OSPPTokenInfoGetTrxId(
    OSPTTOKENINFO *ospvTokenInfo)   /* token info */
{
    OSPTTRXID trxid = 0;

    if (ospvTokenInfo != OSPC_OSNULL) {
        trxid = ospvTokenInfo->ospmTokenInfoTrxId;
    }

    return trxid;
}

/*
 * OSPPTokenInfoSetDuration() - set the duration
 */
void OSPPTokenInfoSetDuration(      /* nothing returned */
    OSPTTOKENINFO *ospvTokenInfo,   /* token info to set */
    int ospvDuration)               /* duration to set to */
{
    if (ospvTokenInfo != OSPC_OSNULL) {
        if (ospvDuration >= 0) {
            ospvTokenInfo->ospmTokenInfoDuration = ospvDuration;
        }
    }
}

/*
 * OSPPTokenInfoGetDuration() - returns the duration for a token info
 */
int OSPPTokenInfoGetDuration(
    OSPTTOKENINFO *ospvTokenInfo)   /* token info */
{
    int duration = 0;

    if (ospvTokenInfo != OSPC_OSNULL) {
        duration = ospvTokenInfo->ospmTokenInfoDuration;
    }

    return duration;
}

/*
 * OSPPTokenInfoNew() - creates a new (empty) token info object
 */
OSPTTOKENINFO *OSPPTokenInfoNew(void)   /* returns pointer or NULL */
{
    OSPTTOKENINFO *tokeninfo;

    OSPM_MALLOC(tokeninfo, OSPTTOKENINFO, sizeof(OSPTTOKENINFO));
    if (tokeninfo != OSPC_OSNULL) {
        tokeninfo->ospmTokenInfoSourceNumber[0] = '\0';
        tokeninfo->ospmTokenInfoDestNumber[0] = '\0';
        tokeninfo->ospmTokenInfoCallId = OSPC_OSNULL;
        tokeninfo->ospmTokenInfoValidAfter = OSPC_TIMEMIN;
        tokeninfo->ospmTokenInfoValidUntil = OSPC_TIMEMAX;
        tokeninfo->ospmTokenInfoTrxId = 0;
        tokeninfo->ospmTokenInfoDuration = -1;
        tokeninfo->ospmTokenInfoHasLookAheadInfo = OSPC_FALSE;
        tokeninfo->ospmTokenLookAheadInfo.lookAheadProt = OSPC_PROTNAME_UNDEFINED;
        tokeninfo->ospmTokenLookAheadInfo.lookAheadDestOSPStatus = OSPC_DOSP_UNDEFINED;
        tokeninfo->ospmTokenLookAheadInfo.lookAheadDest[0] = '\0';
        tokeninfo->ospmTokenInfoHasDestNetworkId = OSPC_FALSE;
        tokeninfo->dstNetworkId[0] = '\0';
    }

    return tokeninfo;
}

/*
 * OSPPTokenInfoDelete() - deletes a token info object
 */
void OSPPTokenInfoDelete(
    OSPTTOKENINFO **ospvTokenInfo)
{
    if (*ospvTokenInfo) {
        if (OSPPTokenInfoHasCallId(*ospvTokenInfo)) {
            OSPPCallIdDelete(&((*ospvTokenInfo)->ospmTokenInfoCallId));
        }
        OSPM_FREE(*ospvTokenInfo);
        *ospvTokenInfo = OSPC_OSNULL;
    }
}

/*
 * OSPPTokenInfoFromElement() - get token info from an XML element
 */
unsigned OSPPTokenInfoFromElement(  /* returns error code */
    OSPT_XML_ELEM *ospvElem,        /* input is XML element */
    OSPTTOKENINFO **ospvTokenInfo)  /* where to put token info pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPTTOKENINFO *tokeninfo = OSPC_OSNULL;
    OSPT_CALL_ID *callid;
    OSPTTIME t;
    OSPTTRXID trxid;
    unsigned duration;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvTokenInfo == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NO_TOKEN;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the token info object */
        tokeninfo = OSPPTokenInfoNew();

        if (tokeninfo == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_NO_TOKENINFO;
        }
    }
    /*
     * The Token Info element should consist of several child
     * elements. We'll run through what's there and pick out
     * the information we need.
     */
    if (errcode == OSPC_ERR_NO_ERROR) {
        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
             (elem != OSPC_OSNULL) &&
             (errcode == OSPC_ERR_NO_ERROR);
             elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, elem)) {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_MESSAGE:
                break;
            case OSPC_MELEM_TOKENINFO:
                break;
            case OSPC_MELEM_DESTALT:
                /*
                 * We need to check the "ospmXMLAttrValue"
                 * to decide whether it is the destination Signaling
                 * address or the network addr
                 */
                if (OSPPXMLAttrGetValue
                    ((OSPT_XML_ATTR *)elem->ospmXMLElemAttrs)) {
                    if (!OSPM_STRCMP("network", OSPPXMLAttrGetValue((OSPT_XML_ATTR *)OSPPListFirst(&elem->ospmXMLElemAttrs)))) {
                        /*
                         * This is network information
                         */
                        tokeninfo->ospmTokenInfoHasDestNetworkId = OSPC_TRUE;
                        OSPPTokenInfoSetDestNetworkId(tokeninfo, OSPPXMLElemGetValue(elem));
                    } else {
                        /*
                         * This must be the IP Address
                         */
                        tokeninfo->ospmTokenInfoHasLookAheadInfo = OSPC_TRUE;
                        OSPPTokenInfoSetLookAheadDestAlt(&(tokeninfo->ospmTokenLookAheadInfo), OSPPXMLElemGetValue(elem));
                    }
                }
                break;
            case OSPC_MELEM_PROTOCOL:
                tokeninfo->ospmTokenInfoHasLookAheadInfo = OSPC_TRUE;
                OSPPTokenInfoSetLookAheadProtocol(&(tokeninfo->ospmTokenLookAheadInfo), OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_DESTOSPVERSION:
                tokeninfo->ospmTokenInfoHasLookAheadInfo = OSPC_TRUE;
                OSPPTokenInfoSetLookAheadOSPVersion(&(tokeninfo->ospmTokenLookAheadInfo), OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_SRCINFO:
                OSPPTokenInfoSetSourceNumber(tokeninfo, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_DESTINFO:
                OSPPTokenInfoSetDestNumber(tokeninfo, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_CALLID:
                errcode = OSPPCallIdFromElement(elem, &callid);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPTokenInfoSetCallId(tokeninfo, callid);
                    OSPPCallIdDelete(&callid);
                }
                break;
            case OSPC_MELEM_TRANSID:
                errcode = OSPPMsgTXFromElement(elem, &trxid);
                OSPPTokenInfoSetTrxId(tokeninfo, trxid);
                break;
            case OSPC_MELEM_VALIDAFTER:
                errcode = OSPPMsgTimeFromElement(elem, &t);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPTokenInfoSetValidAfter(tokeninfo, t);
                }
                break;
            case OSPC_MELEM_VALIDUNTIL:
                errcode = OSPPMsgTimeFromElement(elem, &t);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPTokenInfoSetValidUntil(tokeninfo, t);
                }
                break;
            case OSPC_MELEM_USAGEDETAIL:
                OSPPUsageFromElement(elem, &duration);
                OSPPTokenInfoSetDuration(tokeninfo, (int)duration);
                break;
            default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem)) {
                    errcode = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        *ospvTokenInfo = tokeninfo;
    }

    return errcode;
}

/*
 * OSPPParseTokenInfoFromASCIIToken() - get token info from an ASCII Token
 */
unsigned OSPPParseTokenInfoFromASCIIToken(  /* returns error code */
    unsigned char *ospvASCIIMessage,        /* In - ASCII message */
    unsigned ospvSizeOfMessage,             /* In - size of message */
    OSPTTOKENINFO **ospvTokenInfo)          /* where to put token info pointer */
{
    unsigned char val[200];     /* Assume that there wont be any field more than 200 bytes in length */
    unsigned char *ptr = NULL;
    OSPTTOKENINFO *tokeninfo = OSPC_OSNULL;
    OSPT_CALL_ID *callid;
    OSPTTIME t;
    OSPTTRXID trxid;
    unsigned duration;
    unsigned errcode = 0;

    if (ospvASCIIMessage == OSPC_OSNULL) {
        errcode = OSPC_ERR_ASCII_NO_ELEMENT;
    }
    if (ospvTokenInfo == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NO_TOKEN;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the token info object */
        tokeninfo = OSPPTokenInfoNew();

        if (tokeninfo == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_NO_TOKENINFO;
        }
    }

    if ((errcode == OSPC_ERR_NO_ERROR)) {
        ptr = ospvASCIIMessage;

        do {
            sscanf((const char *)ptr, "%s", val);

            switch (val[0]) {
            case 'V':
            case 'r':
            case 'n':
            case 's':
            case 'U':
                break;
            case 'c':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    OSPPTokenInfoSetSourceNumber(tokeninfo, (const char *)(val + 2));
                }
                break;
            case 'C':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    OSPPTokenInfoSetDestNumber(tokeninfo, (const char *)(val + 2));
                }
                break;
            case 'i':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    errcode = OSPPCallIdFromASCIIElement((unsigned char *)(val + 2), &callid);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPTokenInfoSetCallId(tokeninfo, callid);
                        OSPPCallIdDelete(&callid);
                    }
                }
                break;
            case 'a':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    /*
                     * Convert time string to value
                     */
                    errcode = OSPPOSTimeStringToCal((const char *)(val + 2), &t);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPTokenInfoSetValidAfter(tokeninfo, t);
                    }
                }
                break;
            case 'u':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    /*
                     * Convert time string to value
                     */
                    errcode = OSPPOSTimeStringToCal((const char *)(val + 2), &t);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPTokenInfoSetValidUntil(tokeninfo, t);
                    }
                }
                break;
            case 'I':
                if (OSPM_STRLEN((const char *)val) > 2) {
#ifndef _WIN32
                    trxid = atoll((const char *)(val + 2));
#else
                    trxid = _atoi64((const char *)(val + 2));
#endif
                    OSPPTokenInfoSetTrxId(tokeninfo, trxid);
                }
                break;
            case 'd':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    tokeninfo->ospmTokenInfoHasLookAheadInfo = OSPC_TRUE;
                    OSPPTokenInfoSetLookAheadDestAlt(&(tokeninfo->ospmTokenLookAheadInfo), (const char *)(val + 2));
                }
                break;
            case 'D':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    tokeninfo->ospmTokenInfoHasLookAheadInfo = OSPC_TRUE;
                    OSPPTokenInfoSetLookAheadProtocol(&(tokeninfo->ospmTokenLookAheadInfo), (const char *)(val + 2));
                }
                break;
            case 'o':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    tokeninfo->ospmTokenInfoHasLookAheadInfo = OSPC_TRUE;
                    OSPPTokenInfoSetLookAheadOSPVersion(&(tokeninfo->ospmTokenLookAheadInfo), (const char *)(val + 2));
                }
                break;
            case 'e':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    tokeninfo->ospmTokenInfoHasDestNetworkId = OSPC_TRUE;
                    OSPPTokenInfoSetDestNetworkId(tokeninfo, (const char *)(val + 2));
                }
                break;
            case 'A':
                if (OSPM_STRLEN((const char *)val) > 2) {
                    duration = atoi((const char *)(val + 2));
                    OSPPTokenInfoSetDuration(tokeninfo, (int) duration);
                }
                break;
            default:
                errcode = OSPC_ERR_ASCII_BAD_ELEMENT;
                break;
            }

            ptr = ptr + OSPM_STRLEN((const char *)val);
            if (ptr[0] == '\n') {
                ptr++;
            }
        } while ((ptr[0] != '\0') && (errcode == OSPC_ERR_NO_ERROR));
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        *ospvTokenInfo = tokeninfo;
    }

    return errcode;
}
