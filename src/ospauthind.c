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

/* ospauthind.c - OSP authorization indication functions */

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
#include "osp/ospauthind.h"
#include "osp/osputils.h"

/*
 * OSPPAuthIndSetTimestamp() - sets the timestamp for an authorization indication
 */
void OSPPAuthIndSetTimestamp(
    OSPT_AUTH_IND *ospvAuthInd,
    OSPTTIME ospvTime)
{
    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAuthInd->Timestamp = ospvTime;
    }
}

/*
 * OSPPAuthIndHasRole() - Does AuthIndication have role set?
 */
OSPTBOOL OSPPAuthIndHasRole(
    OSPT_AUTH_IND *ospvAuthInd)
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        has = ospvAuthInd->HasRole;
    }

    return has;
}

/*
 * OSPPAuthIndSetRole() - sets value for role in AuthIndication
 */
void OSPPAuthIndSetRole(
    OSPT_AUTH_IND *ospvAuthInd,
    OSPE_ROLE ospvRole)
{
    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAuthInd->Role = ospvRole;
        ospvAuthInd->HasRole = OSPC_TRUE;
    }
}

/*
 * OSPPAuthIndGetRole() - returns role for an AuthIndication
 */
OSPE_ROLE OSPPAuthIndGetRole(
    OSPT_AUTH_IND *ospvAuthInd)
{
    OSPE_ROLE ospvRole = OSPC_ROLE_UNDEFINED;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvRole = (OSPE_ROLE)ospvAuthInd->Role;
    }

    return ospvRole;
}

/*
 * OSPPAuthIndHasCallId() - does an authorization indication have a Call ID?
 */
OSPTBOOL OSPPAuthIndHasCallId(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        has = (ospvAuthInd->CallId != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthIndGetCallId() - gets the call ID for an authorization indication
 */
OSPT_CALL_ID *OSPPAuthIndGetCallId(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        callid = ospvAuthInd->CallId;
    }

    return callid;
}

/*
 * OSPPAuthIndGetCallIdSize() - gets the call ID size for an authorization indication
 */
unsigned OSPPAuthIndGetCallIdSize(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    unsigned size = 0;

    if (ospvAuthInd != OSPC_OSNULL) {
        size = OSPPCallIdGetSize(ospvAuthInd->CallId);
    }

    return size;
}

/*
 * OSPPAuthIndGetCallIdValue() - gets the call ID value for an authorization indication
 */
unsigned char *OSPPAuthIndGetCallIdValue(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    unsigned char *value = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        value = OSPPCallIdGetValue(ospvAuthInd->CallId);
    }

    return value;
}

/*
 * OSPPAuthIndHasSourceNumber() - does the authorization indication have a source number?
 */
OSPTBOOL OSPPAuthIndHasSourceNumber(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        has = (ospvAuthInd->SourceNumber[0] != '\0');
    }

    return has;
}

/*
 * OSPPAuthIndSetSourceNumber() - set the source number for an authorization indication
 */
void OSPPAuthIndSetSourceNumber(
    OSPT_AUTH_IND *ospvAuthInd, /* authorization indication  to set */
    const char *ospvNum)        /* source number (as string) */
{
    if (ospvAuthInd != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthInd->SourceNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthIndGetSourceNumber() - returns the source number for an  authorization indication
 */
const char *OSPPAuthIndGetSourceNumber(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    const char *ospvSourceNumber = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvSourceNumber = ospvAuthInd->SourceNumber;
    }

    return ospvSourceNumber;
}

/*
 * OSPPAuthIndHasDestNumber() - does the authorization indication include a dest number?
 */
OSPTBOOL OSPPAuthIndHasDestNumber(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        has = (ospvAuthInd->DestinationNumber[0] != '\0');
    }

    return has;
}

/*
 * OSPPAuthIndSetDestNumber() - set the destination number for an authorization indication
 */
void OSPPAuthIndSetDestNumber(
    OSPT_AUTH_IND *ospvAuthInd, /* authorization indication to set */
    const char *ospvNum)        /* destination number (as string) */
{
    if (ospvAuthInd != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthInd->DestinationNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthIndGetDestNumber() - returns the destination number for an authorization indication
 */
const char *OSPPAuthIndGetDestNumber(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    const char *ospvDestNumber = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvDestNumber = ospvAuthInd->DestinationNumber;
    }

    return ospvDestNumber;
}

/*
 * OSPPAuthIndHasSourceAlt() - does an authorization indication have a Source Alternate?
 */
OSPTBOOL OSPPAuthIndHasSourceAlt(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        has = (OSPPAuthIndFirstSourceAlt(ospvAuthInd) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthIndFirstSourceAlt() - gets the First Source alternate for an  authorization indication
 */
OSPT_ALTINFO *OSPPAuthIndFirstSourceAlt(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthInd->SourceAlternate));

    }

    return altinfo;
}

/*
 * OSPPAuthIndNextSourceAlt() - gets the next source alternate for an authorization indication
 */
OSPT_ALTINFO *OSPPAuthIndNextSourceAlt(
    OSPT_AUTH_IND *ospvAuthInd, /* authorization indication */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthInd->SourceAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPAuthIndGetSourceAltValue() - gets the source alternate value for an authorization indication
 */
const char *OSPPAuthIndGetSourceAltValue(
    OSPT_ALTINFO *ospvAltInfo)  /* altinfo ptr */
{
    const char *value = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        value = OSPPAltInfoGetValue(ospvAltInfo);
    }

    return value;
}

/*
 * OSPPAuthIndHasDestinationAlt() - does an authorization indication have a Destination Alternate?
 */
OSPTBOOL OSPPAuthIndHasDestinationAlt(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        has = (OSPPAuthIndFirstDestinationAlt(ospvAuthInd) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthIndFirstDestinationAlt() - gets the First Destination alternate for an authorization indication
 */
OSPT_ALTINFO *OSPPAuthIndFirstDestinationAlt(
    OSPT_AUTH_IND *ospvAuthInd) /* authorization indication */
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvAuthInd->DestinationAlternate));
    }

    return altinfo;
}

/*
 * OSPPAuthIndNextDestinationAlt() - gets the next Destination alternate for an authorization indication
 */
OSPT_ALTINFO *OSPPAuthIndNextDestinationAlt(
    OSPT_AUTH_IND *ospvAuthInd, /* authorization indication */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvAuthInd->DestinationAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPAuthIndGetDestinationAltValue() - gets the Destination alternate value for
 * an authorization indication
 */
const char *OSPPAuthIndGetDestinationAltValue(
    OSPT_ALTINFO *ospvAltInfo)  /* altinfo ptr */
{
    const char *ospvAltInfoValue = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvAltInfoValue = OSPPAltInfoGetValue(ospvAltInfo);
    }

    return ospvAltInfoValue;
}

/*
 * OSPPAuthIndSetDest() - sets the destination for an authorization
 */
void OSPPAuthIndSetDest(
    OSPT_AUTH_IND *ospvAuthInd,
    OSPT_DEST *ospvDest)
{
    if (ospvAuthInd != OSPC_OSNULL) {
        if (ospvDest != OSPC_OSNULL) {
            if (OSPPAuthIndHasDest(ospvAuthInd) == OSPC_TRUE) {
                OSPPDestDelete(&(ospvAuthInd->Destination));
            }
            ospvAuthInd->Destination = ospvDest;
        }
    }
}

/*
 * OSPPAuthIndHasDest() - does an authorization indication have a Destination?
 */
OSPTBOOL OSPPAuthIndHasDest(
    OSPT_AUTH_IND *ospvAuthInd)
{
    if (ospvAuthInd->Destination != OSPC_OSNULL) {
        return OSPC_TRUE;
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthIndSetTimeLimit() - sets the timelimit for an authorization indication
 */
void OSPPAuthIndSetTimeLimit(
    OSPT_AUTH_IND *ospvAuthInd,
    unsigned ospvTimeLimit)
{
    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAuthInd->TimeLimit = ospvTimeLimit;
        ospvAuthInd->HasTimeLimit = OSPC_TRUE;
    }
}

/*
 * OSPPAuthIndNew() - creates a new (empty) authorization indication
 */
OSPT_AUTH_IND *OSPPAuthIndNew(void)
{
    OSPT_AUTH_IND *ospvAuthInd = OSPC_OSNULL;

    OSPM_MALLOC(ospvAuthInd, OSPT_AUTH_IND, sizeof(OSPT_AUTH_IND));
    if (ospvAuthInd != OSPC_OSNULL) {
        OSPM_MEMSET(ospvAuthInd, 0, sizeof(OSPT_AUTH_IND));

        ospvAuthInd->Timestamp = OSPC_TIMEMIN;
        ospvAuthInd->Role = 0;
        ospvAuthInd->HasRole = OSPC_FALSE;
        ospvAuthInd->CallId = OSPC_OSNULL;
        ospvAuthInd->SourceNumber[0] = '\0';
        ospvAuthInd->DestinationNumber[0] = '\0';
        OSPPListNew(&ospvAuthInd->Tokens);
        OSPPListNew(&(ospvAuthInd->SourceAlternate));
        OSPPListNew(&(ospvAuthInd->DestinationAlternate));
        OSPPListNew(&(ospvAuthInd->DeviceInfo));
        ospvAuthInd->HasDestNetworkIdInToken = OSPC_FALSE;
    }

    return ospvAuthInd;
}

/*
 * OSPPAuthIndDelete() - destroy specified AuthInd object
 */
void OSPPAuthIndDelete(
    OSPT_AUTH_IND **ospvAuthInd)
{
    OSPT_TOKEN *token = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (*ospvAuthInd != OSPC_OSNULL) {
        if (OSPPAuthIndHasCallId(*ospvAuthInd)) {
            OSPPCallIdDelete(&((*ospvAuthInd)->CallId));
        }

        while (!OSPPListEmpty(&((*ospvAuthInd)->Tokens))) {
            token = (OSPT_TOKEN *)OSPPListRemove(&((*ospvAuthInd)->Tokens));

            if (token != OSPC_OSNULL) {
                OSPPTokenDelete(&token);
            }
        }

        OSPPListDelete(&((*ospvAuthInd)->Tokens));

        while (!OSPPListEmpty(&((*ospvAuthInd)->SourceAlternate))) {
            altinfo =
                (OSPT_ALTINFO *)
                OSPPListRemove(&((*ospvAuthInd)->SourceAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthInd)->SourceAlternate));

        while (!OSPPListEmpty(&((*ospvAuthInd)->DestinationAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthInd)->DestinationAlternate));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthInd)->DestinationAlternate));

        while (!OSPPListEmpty(&((*ospvAuthInd)->DeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthInd)->DeviceInfo));
            if (altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }

        OSPPListDelete(&((*ospvAuthInd)->DeviceInfo));
        OSPPDestDelete(&((*ospvAuthInd)->Destination));

        OSPM_FREE(*ospvAuthInd);
        *ospvAuthInd = OSPC_OSNULL;
    }
}

/*
 * OSPPAuthIndSetCallId() - sets the call ID for an authorization
 */
void OSPPAuthIndSetCallId(
    OSPT_AUTH_IND *ospvAuthInd, /* authorization indication */
    OSPT_CALL_ID *ospvCallId)   /* call ID */
{
    if (ospvAuthInd != OSPC_OSNULL) {
        if (ospvCallId != OSPC_OSNULL) {
            if (ospvAuthInd->CallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvAuthInd->CallId));
            }
            ospvAuthInd->CallId = OSPPCallIdNew(ospvCallId->Length, ospvCallId->Value);
        }
    }
}
