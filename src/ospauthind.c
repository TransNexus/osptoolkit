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
 * ospauthind.c - OSP authorisation indication functions
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
#include "osp/ospauthind.h"
#include "osp/osputils.h"

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndSetTimestamp() - sets the timestamp for an authorisation indication
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndSetTimestamp(
    OSPTAUTHIND* ospvAuthInd,
    OSPTTIME ospvTime)
{
    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAuthInd->ospmAuthIndTimestamp = ospvTime;
    }
}

/*
 * OSPPAuthIndHasRole() - Does AuthIndication have role set?
 */
OSPTBOOL OSPPAuthIndHasRole(
    OSPTAUTHIND* ospvAuthInd)
{
    OSPTBOOL ospvHasRole = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvHasRole = ospvAuthInd->ospmAuthIndHasRole;
    }
    
    return ospvHasRole;
}

/*^L*/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndSetRole() - sets value for role in AuthIndication
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndSetRole(
    OSPTAUTHIND* ospvAuthInd,
    OSPE_ROLE_TYPE ospvRole)
{
    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAuthInd->ospmAuthIndRole = ospvRole;
        ospvAuthInd->ospmAuthIndHasRole = OSPC_TRUE;
    }
}

/*^L*/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndGetRole() - returns role for an AuthIndication
 *-----------------------------------------------------------------------*
 */
OSPE_ROLE_TYPE OSPPAuthIndGetRole(
    OSPTAUTHIND* ospvAuthInd)
{
    OSPE_ROLE_TYPE ospvRole = OSPC_RTYPE_UNDEFINED;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvRole = (OSPE_ROLE_TYPE)ospvAuthInd->ospmAuthIndRole;
    }
    return ospvRole;
}

/*
 * OSPPAuthIndHasCallId() - does an authorisation indication have a Call ID?
 */
OSPTBOOL OSPPAuthIndHasCallId(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    OSPTBOOL ospvHasCallId = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvHasCallId = (ospvAuthInd->ospmAuthIndCallId != OSPC_OSNULL);
    }
    
    return ospvHasCallId;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndGetCallId() - gets the call ID for an authorisation indication
 *-----------------------------------------------------------------------*
 */
OSPTCALLID* OSPPAuthIndGetCallId(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    OSPTCALLID* ospvCallId = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvCallId = ospvAuthInd->ospmAuthIndCallId;
    }
    return ospvCallId;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndGetCallIdSize() - gets the call ID size for an authorisation indication
 *-----------------------------------------------------------------------*
 */
unsigned OSPPAuthIndGetCallIdSize(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    unsigned ospvCallIdSize = 0;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvCallIdSize = OSPPCallIdGetSize(ospvAuthInd->ospmAuthIndCallId);
    }
    return ospvCallIdSize;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndGetCallIdValue() - gets the call ID value for an authorisation indication
 *-----------------------------------------------------------------------*
 */
unsigned char* OSPPAuthIndGetCallIdValue(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    unsigned char* ospvCallIdValue = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvCallIdValue = OSPPCallIdGetValue(ospvAuthInd->ospmAuthIndCallId);
    }
    return ospvCallIdValue;
}

/*
 * OSPPAuthIndHasSourceNumber() - does the authorisation indication have a source number?
 */
OSPTBOOL OSPPAuthIndHasSourceNumber(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication effected */
{
    OSPTBOOL ospvHasSourceNumber = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvHasSourceNumber = (ospvAuthInd->ospmAuthIndSourceNumber[0] != '\0');
    }
    
    return ospvHasSourceNumber;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndSetSourceNumber() - set the source number for an
 * authorisation indication
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndSetSourceNumber(
    OSPTAUTHIND* ospvAuthInd,       /* authorisation indication  to set */
    const char* ospvNum)   /* source number (as string) */
{
    if (ospvAuthInd != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthInd->ospmAuthIndSourceNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthIndGetSourceNumber() - returns the source number for an 
 * authorisation indication
 */
const char* OSPPAuthIndGetSourceNumber(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    const char* ospvSourceNumber = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvSourceNumber = ospvAuthInd->ospmAuthIndSourceNumber;
    }
    
    return ospvSourceNumber;
}

/*
 * OSPPAuthIndHasDestNumber() - does the authorisation indication include a dest number?
 */
OSPTBOOL OSPPAuthIndHasDestNumber(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication effected */
{
    OSPTBOOL ospvHasDestNumber = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvHasDestNumber = (ospvAuthInd->ospmAuthIndDestNumber[0] != '\0');
    }
    
    return ospvHasDestNumber;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndSetDestNumber() - set the destination number for an
 * authorisation indication
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndSetDestNumber(
    OSPTAUTHIND* ospvAuthInd,       /* authorisation indication to set */
    const char* ospvNum)   /* destination number (as string) */
{
    if (ospvAuthInd != OSPC_OSNULL) {
        if (ospvNum != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvAuthInd->ospmAuthIndDestNumber, ospvNum, tr_min(OSPM_STRLEN(ospvNum) + 1, OSPC_SIZE_E164NUM - 1));
        }
    }
}

/*
 * OSPPAuthIndGetDestNumber() - returns the destination number for an authorisation indication
 */
const char *OSPPAuthIndGetDestNumber(
    OSPTAUTHIND *ospvAuthInd)   /* authorisation indication */
{
    const char *ospvDestNumber = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvDestNumber = ospvAuthInd->ospmAuthIndDestNumber;
    }
    
    return ospvDestNumber;
}

/*
 * OSPPAuthIndHasSourceAlt() - does an authorisation indication have a Source Alternate?
 */
OSPTBOOL OSPPAuthIndHasSourceAlt(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    OSPTBOOL ospvHasSourceAlt = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvHasSourceAlt = (OSPPAuthIndFirstSourceAlt(ospvAuthInd) != OSPC_OSNULL);
    }
    
    return ospvHasSourceAlt;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndFirstSourceAlt() - gets the First Source alternate for an 
 * authorisation indication
 *-----------------------------------------------------------------------*
 */
OSPT_ALTINFO* OSPPAuthIndFirstSourceAlt(
    OSPTAUTHIND* ospvAuthInd)              /* authorisation indication */
{
    OSPT_ALTINFO* ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO*)OSPPListFirst(&(ospvAuthInd->ospmAuthIndSourceAlternate));
    }
    return ospvAltInfo;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndNextSourceAlt() - gets the next source alternate for an 
 * authorisation indication
 *-----------------------------------------------------------------------*
 */
OSPT_ALTINFO*  OSPPAuthIndNextSourceAlt(
    OSPTAUTHIND* ospvAuthInd,   /* authorisation indication */
    OSPT_ALTINFO* ospvAltInfo)
{
    OSPT_ALTINFO* altinfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO*)OSPPListNext(&(ospvAuthInd->ospmAuthIndSourceAlternate), 
            ospvAltInfo);
    }
    return altinfo;
}

/*
 * OSPPAuthIndGetSourceAltValue() - gets the source alternate value for 
 * an authorisation indication
 */
const char* OSPPAuthIndGetSourceAltValue(
    OSPT_ALTINFO* ospvAltInfo)      /* Alt info ptr */
{
    const char* ospvAltInfoValue = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvAltInfoValue = OSPPAltInfoGetValue(ospvAltInfo);
    }
    
    return ospvAltInfoValue;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndHasDestinationAlt() - does an authorisation indication have a 
 * Destination Alternate?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthIndHasDestinationAlt(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    int ospvHasDestinationAlt = OSPC_FALSE;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvHasDestinationAlt = (OSPPAuthIndFirstDestinationAlt(ospvAuthInd) != OSPC_OSNULL);
    }
    return ospvHasDestinationAlt;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndFirstDestinationAlt() - gets the First Destination alternate for an 
 * authorisation indication
 *-----------------------------------------------------------------------*
 */
OSPT_ALTINFO* OSPPAuthIndFirstDestinationAlt(
    OSPTAUTHIND* ospvAuthInd)   /* authorisation indication */
{
    OSPT_ALTINFO* ospvAltInfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO*)OSPPListFirst(&(ospvAuthInd->ospmAuthIndDestinationAlternate)); 
    }
    return ospvAltInfo;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndNextDestinationAlt() - gets the next Destination alternate for an 
 * authorisation indication
 *-----------------------------------------------------------------------*
 */
OSPT_ALTINFO* OSPPAuthIndNextDestinationAlt(
    OSPTAUTHIND* ospvAuthInd,   /* authorisation indication */
    OSPT_ALTINFO* ospvAltInfo)
{
    OSPT_ALTINFO* altinfo = OSPC_OSNULL;

    if (ospvAuthInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO*)OSPPListNext(&(ospvAuthInd->ospmAuthIndDestinationAlternate),  
            ospvAltInfo);
    }
    return altinfo;
}

/*
 * OSPPAuthIndGetDestinationAltValue() - gets the Destination alternate value for 
 * an authorisation indication
 */
const char* OSPPAuthIndGetDestinationAltValue(
    OSPT_ALTINFO* ospvAltInfo)      /* Alt info ptr */
{
    const char* ospvAltInfoValue = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvAltInfoValue = OSPPAltInfoGetValue(ospvAltInfo);
    }
    
    return ospvAltInfoValue;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndSetDest() - sets the destination for an authorisation
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndSetDest(
    OSPTAUTHIND* ospvAuthInd,
    OSPTDEST* ospvDest)
{
    if(ospvAuthInd != OSPC_OSNULL) {
        if(ospvDest != OSPC_OSNULL) {
            if(OSPPAuthIndHasDest(ospvAuthInd) == OSPC_TRUE) {
                OSPPDestDelete(&(ospvAuthInd->ospmAuthIndDest));
            }
            ospvAuthInd->ospmAuthIndDest = ospvDest;
        }
    }
}

/*
 * OSPPAuthIndHasDest() - does an authorisation indication have a Destination?
 */
OSPTBOOL OSPPAuthIndHasDest(
    OSPTAUTHIND* ospvAuthInd)
{
    if(ospvAuthInd->ospmAuthIndDest != OSPC_OSNULL) {
        return OSPC_TRUE;
    } else {
        return OSPC_FALSE;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndSetTimeLimit() - sets the timelimit for an authorisation 
 * indication
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndSetTimeLimit(
    OSPTAUTHIND* ospvAuthInd,
    unsigned ospvTimeLimit)
{
    if (ospvAuthInd != OSPC_OSNULL) {
        ospvAuthInd->ospmAuthIndTimeLimit = ospvTimeLimit;
        ospvAuthInd->ospmAuthIndHasTimeLimit = OSPC_TRUE;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndNew() - creates a new (empty) authorisation indication
 *-----------------------------------------------------------------------*
 */
OSPTAUTHIND* OSPPAuthIndNew()
{
    OSPTAUTHIND* ospvAuthInd = OSPC_OSNULL;

    OSPM_MALLOC(ospvAuthInd, OSPTAUTHIND,sizeof(OSPTAUTHIND));
    if (ospvAuthInd != OSPC_OSNULL) {
        OSPM_MEMSET(ospvAuthInd, 0, sizeof(OSPTAUTHIND));

        ospvAuthInd->ospmAuthIndTimestamp = OSPC_TIMEMIN;
        ospvAuthInd->ospmAuthIndRole = 0;
        ospvAuthInd->ospmAuthIndHasRole = OSPC_FALSE;
        ospvAuthInd->ospmAuthIndCallId = OSPC_OSNULL;
        ospvAuthInd->ospmAuthIndSourceNumber[0] = '\0';
        ospvAuthInd->ospmAuthIndDestNumber[0] = '\0';
        OSPPListNew(&ospvAuthInd->ospmAuthIndTokens);
        OSPPListNew (&(ospvAuthInd->ospmAuthIndSourceAlternate));
        OSPPListNew(&(ospvAuthInd->ospmAuthIndDestinationAlternate));
        OSPPListNew(&(ospvAuthInd->ospmAuthIndDeviceInfo));
        ospvAuthInd->ospmAuthIndHasDestNetworkIdInToken = OSPC_FALSE;
    }
    return ospvAuthInd;
}

/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndDelete() - destroy specified AuthInd object
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndDelete(OSPTAUTHIND** ospvAuthInd)
{
    OSPTTOKEN* token = OSPC_OSNULL;
    OSPT_ALTINFO* altinfo = OSPC_OSNULL;

    if (*ospvAuthInd != OSPC_OSNULL) {
        if (OSPPAuthIndHasCallId(*ospvAuthInd)) {
            OSPPCallIdDelete(&((*ospvAuthInd)->ospmAuthIndCallId));
        }

        while(!OSPPListEmpty(&((*ospvAuthInd)->ospmAuthIndTokens))) {
            token = (OSPTTOKEN*)OSPPListRemove(&((*ospvAuthInd)->ospmAuthIndTokens));
            if(token != OSPC_OSNULL) {
                OSPPTokenDelete(&token);
            }
        }  

        OSPPListDelete(&((*ospvAuthInd)->ospmAuthIndTokens));

        while(!OSPPListEmpty(&((*ospvAuthInd)->ospmAuthIndSourceAlternate))) {
            altinfo = (OSPT_ALTINFO*)OSPPListRemove(&((*ospvAuthInd)->ospmAuthIndSourceAlternate));
            if(altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvAuthInd)->ospmAuthIndSourceAlternate));

        while(!OSPPListEmpty(&((*ospvAuthInd)->ospmAuthIndDestinationAlternate))) {
            altinfo = (OSPT_ALTINFO*)OSPPListRemove(&((*ospvAuthInd)->ospmAuthIndDestinationAlternate));
            if(altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvAuthInd)->ospmAuthIndDestinationAlternate));

        while(!OSPPListEmpty(&((*ospvAuthInd)->ospmAuthIndDeviceInfo))) {
            altinfo = (OSPT_ALTINFO*)OSPPListRemove(&((*ospvAuthInd)->ospmAuthIndDeviceInfo));
            if(altinfo != OSPC_OSNULL) {
                OSPM_FREE(altinfo);
                altinfo = OSPC_OSNULL;
            }
        }  

        OSPPListDelete(&((*ospvAuthInd)->ospmAuthIndDeviceInfo));
        OSPPDestDelete(&((*ospvAuthInd)->ospmAuthIndDest));

        OSPM_FREE(*ospvAuthInd);
        *ospvAuthInd = OSPC_OSNULL;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthIndSetCallId() - sets the call ID for an authorisation
 *-----------------------------------------------------------------------*
 */
void OSPPAuthIndSetCallId(
    OSPTAUTHIND* ospvAuthInd,   /* authorisation indication */
    OSPTCALLID* ospvCallId)     /* call ID */
{
    if (ospvAuthInd != OSPC_OSNULL) {
        if (ospvCallId != OSPC_OSNULL) {
            if (ospvAuthInd->ospmAuthIndCallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvAuthInd->ospmAuthIndCallId));
            }
            ospvAuthInd->ospmAuthIndCallId = OSPPCallIdNew((ospvCallId)->ospmCallIdLen, ospvCallId->ospmCallIdVal); 
        }
    }
}
