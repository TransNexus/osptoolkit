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
 * ospusageind.c - OSP usage indication functions
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
#include "osp/ospusage.h"
#include "osp/ospusageind.h"
#include "osp/ospstatistics.h"
#include "osp/osputils.h"
#include "osp/osptrans.h"

/*
 *  OSPPUsageIndHasTimestamp() - is the timestamp set ?
 */
unsigned OSPPUsageIndHasTimestamp(  /* returns non-zero if number exists */
    OSPTUSAGEIND *ospvUsageInd)     /* Usage Indication effected */
{
    unsigned ospvHasTimestamp = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasTimestamp = ospvUsageInd->ospmUsageIndTimestamp;
    }

    return ospvHasTimestamp;
}

/*
 * OSPPUsageIndSetTimestamp() - set the timestamp
 */
void OSPPUsageIndSetTimestamp(      /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTTIME ospvTimestamp)         /* timestamp to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvTimestamp != 0) {
            ospvUsageInd->ospmUsageIndTimestamp = ospvTimestamp;
        }
    }
}

/*
 * OSPPUsageIndGetTimestamp() - returns the timestamp for a usage ind
 */
OSPTTIME OSPPUsageIndGetTimestamp(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    OSPTTIME ospvTimestamp = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvTimestamp = ospvUsageInd->ospmUsageIndTimestamp;
    }

    return ospvTimestamp;
}

/*
 * OSPPUsageIndHasRole() - Does usage indication have role set?
 */
unsigned OSPPUsageIndHasRole(       /* returns non-zero if time */
    OSPTUSAGEIND *ospvUsageInd)     /* usage indication in question */
{
    unsigned ospvHasRole = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasRole = (ospvUsageInd->ospmUsageIndHasRole != OSPC_FALSE);
    }

    return ospvHasRole;
}

/*
 * OSPPUsageIndGetRole() - returns role for an usage indication
 */
OSPE_MSG_ROLE OSPPUsageIndGetRole(  /* returns the role (OGW/TGW) */
    OSPTUSAGEIND *ospvUsageInd)     /* usage indication */
{
    OSPE_MSG_ROLE ospvRole = OSPC_MROLE_UNDEFINED;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvRole = (OSPE_MSG_ROLE)ospvUsageInd->ospmUsageIndRole;
    }

    return ospvRole;
}

/*
 * OSPPUsageIndSetRole() - sets the role for an usage indication
 */
void OSPPUsageIndSetRole(   /* nothing returned */
    OSPTUSAGEIND *
    ospvUsageInd,
    unsigned ospvRole)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndRole = (ospvRole);
        ospvUsageInd->ospmUsageIndHasRole = OSPC_TRUE;
    }
}

/*
 * OSPPUsageIndHasComponentId() - is the component id set ?
 */
unsigned OSPPUsageIndHasComponentId( /* returns non-zero if component id is set */
    OSPTUSAGEIND *ospvUsageInd)
{
    return (ospvUsageInd->ospmUsageIndComponentId != OSPC_OSNULL);
}

/*
 * OSPPUsageIndGetComponentId() - returns a new copy of the component id.
 */
unsigned char *OSPPUsageIndGetComponentId(
    OSPTUSAGEIND *ospvUsageInd)
{
    unsigned char *componentstring = OSPC_OSNULL;
    int len = 0;

    if (OSPPUsageIndHasComponentId(ospvUsageInd)) {
        len = OSPM_STRLEN((const char *) ospvUsageInd->ospmUsageIndComponentId);
        OSPM_MALLOC(componentstring, unsigned char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvUsageInd->ospmUsageIndComponentId, len);
    }

    return componentstring;
}

/*
 * OSPPUsageIndHasTransactionId() - is the transaction id set ?
 */
unsigned OSPPUsageIndHasTransactionId(  /* returns non-zero if number exists */
    OSPTUSAGEIND *ospvUsageInd)         /* Usage Indication effected */
{
    unsigned ospvHasTransactionId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasTransactionId = (ospvUsageInd->ospmUsageIndTransactionId != 0);
    }

    return ospvHasTransactionId;
}

/*
 * OSPPUsageIndSetTransactionId() - set the transaction id
 */
void OSPPUsageIndSetTransactionId(  /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTTRXID ospvTransactionId)    /* transaction id to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvTransactionId != 0) {
            ospvUsageInd->ospmUsageIndTransactionId = ospvTransactionId;
        }
    }
}

/*
 * OSPPUsageIndGetTransactionId() - returns the trans id for a usage ind
 */
OSPTTRXID OSPPUsageIndGetTransactionId(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    OSPTTRXID ospvTransactionId = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvTransactionId = ospvUsageInd->ospmUsageIndTransactionId;
    }

    return ospvTransactionId;
}

/*
 * OSPPUsageIndHasCallId() - does an usage indication have a Call ID?
 */
unsigned OSPPUsageIndHasCallId(     /* returns non-zero if exists */
    OSPTUSAGEIND *ospvUsageInd)     /* usage indication */
{
    unsigned ospvHasCallId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasCallId = (ospvUsageInd->ospmUsageIndCallId != OSPC_OSNULL);
    }

    return ospvHasCallId;
}

/*
 * OSPPUsageIndGetCallId() - gets the call ID for an usage indication
 */
OSPTCALLID *OSPPUsageIndGetCallId(  /* returns call ID pointer */
    OSPTUSAGEIND *ospvUsageInd)     /* usage indication */
{
    OSPTCALLID *ospvCallId = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvCallId = ospvUsageInd->ospmUsageIndCallId;
    }

    return ospvCallId;
}

/*
 * OSPPUsageIndSetSourceNumber() - set the source number
 */
void OSPPUsageIndSetSourceNumber(       /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,         /* usage indication to set */
    unsigned char *ospvSourceNumber)    /* source number to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvSourceNumber != OSPC_OSNULL) {
            OSPM_STRNCPY((char *)(ospvUsageInd->ospmUsageIndSourceNumber),
                (const char *)ospvSourceNumber,
                tr_min(OSPC_SIZE_E164NUM - 1,
                OSPM_STRLEN((const char *)ospvSourceNumber) + 1));
        }
    }
}

/*
 * OSPPUsageIndGetSourceNumber() - returns the source number for usage ind
 */
unsigned char *OSPPUsageIndGetSourceNumber(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    unsigned char *ospvSourceNumber = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvSourceNumber = ospvUsageInd->ospmUsageIndSourceNumber;
    }

    return ospvSourceNumber;
}

/*
 * OSPPUsageIndSetConferenceId() - set the conference id
 */
void OSPPUsageIndSetConferenceId(       /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,         /* usage indication to set */
    unsigned char *ospvConferenceId)    /* conference id to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvConferenceId != OSPC_OSNULL) {
            OSPM_STRCPY((char *)(ospvUsageInd->ospmUsageIndConferenceId), (const char *)ospvConferenceId);
        }
    }
}

/*
 * OSPPUsageIndGetIsConfIdPresent() - Checks if conf id is present.
 */
int OSPPUsageIndGetIsConfIdPresent(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    unsigned ospvIsConfIdPresent = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvUsageInd->ospmUsageIndConferenceId[0] == '\0') {
            ospvIsConfIdPresent = OSPC_FALSE;
        } else {
            ospvIsConfIdPresent = OSPC_TRUE;
        }
    }

    return ospvIsConfIdPresent;
}

/*
 * OSPPUsageIndGetConferenceId() - returns the conf id
 */
unsigned char *OSPPUsageIndGetConferenceId(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    unsigned char *ospvConferenceId = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvConferenceId = ospvUsageInd->ospmUsageIndConferenceId;
    }

    return ospvConferenceId;
}

/*
 * OSPPUsageIndSetDestNumber() - set the destination number
 */
void OSPPUsageIndSetDestNumber(     /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    unsigned char *ospvDestNumber)  /* destination number to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvDestNumber != OSPC_OSNULL) {
            OSPM_STRNCPY((char *)(ospvUsageInd->ospmUsageIndDestNumber),
                (const char *)ospvDestNumber,
                tr_min(OSPC_SIZE_E164NUM - 1,
                OSPM_STRLEN((const char *) ospvDestNumber) + 1));
        }
    }
}

/*
 * OSPPUsageIndGetDestNumber() - returns the destination number for a usage ind
 */
unsigned char *OSPPUsageIndGetDestNumber(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    unsigned char *ospvDestNumber = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvDestNumber = ospvUsageInd->ospmUsageIndDestNumber;
    }

    return ospvDestNumber;
}

/*
 * OSPPUsageIndHasDuration() - is the duration set ?
 */
unsigned OSPPUsageIndHasDuration(   /* returns non-zero if number exists */
    OSPTUSAGEIND *ospvUsageInd)     /* Usage Indication effected */
{
    unsigned ospvHasDuration = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasDuration = (ospvUsageInd->ospmUsageIndDuration >= 0);
    }

    return ospvHasDuration;
}

/*
 * OSPPUsageIndSetDuration() - set the duration
 */
void OSPPUsageIndSetDuration(   /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd, /* usage indication to set */
    int ospvDuration)           /* duration to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvDuration >= 0) {
            ospvUsageInd->ospmUsageIndDuration = ospvDuration;
        }
    }
}

/*
 * OSPPUsageIndGetDuration() - returns the duration for a usage ind
 */
int OSPPUsageIndGetDuration(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    int ospvDuration = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvDuration = ospvUsageInd->ospmUsageIndDuration;
    }

    return ospvDuration;
}

/*
 * OSPPUsageIndSetIsPDDInfoPresent() - set the IsPDDInfoPResent variable
 */
void OSPPUsageIndSetIsPDDInfoPresent(   /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,         /* usage indication to set */
    unsigned IsPDDInfoPresent)          /* duration to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospvUsageIndIsPDDInfoPresent = IsPDDInfoPresent;
    }
}

/*
 * OSPPUsageIndGetIsPDDInfoPresent() - gets the IsPDDInfoPResent variable.
 */
int OSPPUsageIndGetIsPDDInfoPresent(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    unsigned ospvIsPDDInfoPresent = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvIsPDDInfoPresent = ospvUsageInd->ospvUsageIndIsPDDInfoPresent;
    }

    return ospvIsPDDInfoPresent;
}

/*
 * OSPPUsageIndSetReleaseSource() - set the Rel Src
 */
void OSPPUsageIndSetReleaseSource(  /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    unsigned ospvReleaseSource)     /* Rel Src to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvReleaseSource >= 0) {
            ospvUsageInd->ospmUsageIndReleaseSource = ospvReleaseSource;
        }
    }
}

/*
 * OSPPUsageIndGetReleaseSource() - returns the Rel Src for a usage ind
 */
unsigned OSPPUsageIndGetReleaseSource(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    int ospvReleaseSource = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvReleaseSource = ospvUsageInd->ospmUsageIndReleaseSource;
    }

    return ospvReleaseSource;
}

/*
 * OSPPUsageIndSetPostDialDelay() - set the PDD
 */
void OSPPUsageIndSetPostDialDelay(  /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    int ospvPostDialDelay)          /* PDD to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvPostDialDelay >= 0) {
            ospvUsageInd->ospmUsageIndPostDialDelay = ospvPostDialDelay;
        }
    }
}

/*
 * OSPPUsageIndGetPostDialDelay() - returns the PDD for a usage ind
 */
int OSPPUsageIndGetPostDialDelay(
    OSPTUSAGEIND *ospvUsageInd)     /* usage ind */
{
    int ospvPostDialDelay = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvPostDialDelay = ospvUsageInd->ospmUsageIndPostDialDelay;
    }

    return ospvPostDialDelay;
}

/*
 * OSPPUsageIndHasTNCustId() - Does usage have a TransNexus Customer Id?
 */
unsigned OSPPUsageIndHasTNCustId(   /* returns non-zero if time */
    OSPTUSAGEIND *ospvUsageInd)     /* usage in question */
{
    unsigned ospvHasTNCustId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasTNCustId = (ospvUsageInd->ospmUsageIndTNCustId != 0L);
    }

    return ospvHasTNCustId;
}

/*
 * OSPPUsageIndSetTNCustId() - Set TransNexus Customer Id
 */
void OSPPUsageIndSetTNCustId(   /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    unsigned long ospvTNCustId)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndTNCustId = ospvTNCustId;
    }
}

/*
 * OSPPUsageIndGetTNCustId() - returns TN Customer Id for an usage request
 */
unsigned long OSPPUsageIndGetTNCustId(  /* returns the time value */
    OSPTUSAGEIND *ospvUsageInd)         /* usage request */
{
    unsigned long ospvTNCustId = 0L;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvTNCustId = ospvUsageInd->ospmUsageIndTNCustId;
    }

    return ospvTNCustId;
}

/*
 * OSPPUsageIndHasTNDeviceId() - Does request have a TransNexus Device Id?
 */
unsigned OSPPUsageIndHasTNDeviceId( /* returns non-zero if time */
    OSPTUSAGEIND *ospvUsageInd)     /* usage request in question */
{
    unsigned ospvHasTNDeviceId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasTNDeviceId = (ospvUsageInd->ospmUsageIndTNDeviceId != 0L);
    }

    return ospvHasTNDeviceId;
}

/*
 * OSPPUsageIndSetTNDeviceId() - Set TransNexus Device Id
 */
void OSPPUsageIndSetTNDeviceId(/* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    unsigned long ospvTNDeviceId)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndTNDeviceId = ospvTNDeviceId;
    }
}

/*
 * OSPPUsageIndGetTNDeviceId() - returns TN Device Id for an usage request
 */
unsigned long OSPPUsageIndGetTNDeviceId(    /* returns the time value */
    OSPTUSAGEIND *ospvUsageInd)             /* usage request */
{
    unsigned long ospvTNDeviceId = 0L;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvTNDeviceId = ospvUsageInd->ospmUsageIndTNDeviceId;
    }

    return ospvTNDeviceId;
}

/*
 * OSPPUsageIndHasTNFailReason() - Does request have a TransNexus Fail Rsn
 */
unsigned OSPPUsageIndHasTNFailReason(   /* returns non-zero if time */
    OSPTUSAGEIND *ospvUsageInd)         /* usage request in question */
{
    unsigned ospvHasTNFailReason = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasTNFailReason = ospvUsageInd->ospmUsageIndTNFailReasonInd;
    }

    return ospvHasTNFailReason;
}

/*
 * OSPPUsageIndSetTNFailReason() - Set TransNexus Fail Reason
 */
void OSPPUsageIndSetTNFailReason(   /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    unsigned ospvTNFailReason)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndTNFailReason = ospvTNFailReason;
        ospvUsageInd->ospmUsageIndTNFailReasonInd = 1;
    }
}

/*
 * OSPPUsageIndGetTNFailReason() - returns TN Fail Rsn for an usage request
 */
unsigned OSPPUsageIndGetTNFailReason(   /* returns the fail reason value */
    OSPTUSAGEIND *ospvUsageInd)         /* usage request */
{
    unsigned ospvTNFailReason = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvTNFailReason = ospvUsageInd->ospmUsageIndTNFailReason;
    }
    return ospvTNFailReason;
}

/*
 * OSPPUsageIndHasSourceAlt() - does an usage indication have a 
 * Source Alternate?
 */
unsigned OSPPUsageIndHasSourceAlt(  /* returns non-zero if exists */
    OSPTUSAGEIND *ospvUsageInd)     /* usage indication */
{
    unsigned ospvHasSourceAlt = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasSourceAlt = (OSPPUsageIndFirstSourceAlt(ospvUsageInd) != OSPC_OSNULL);
    }

    return ospvHasSourceAlt;
}

/*
 * OSPPUsageIndFirstSourceAlt() - gets the First Source alternate for an 
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndFirstSourceAlt(   /* returns alt info pointer */
    OSPTUSAGEIND *ospvUsageInd)             /* usage indication */
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvUsageInd->ospmUsageIndSourceAlternate));
    }

    return ospvAltInfo;
}

/*
 * OSPPUsageIndNextSourceAlt() - gets the next source alternate for an 
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndNextSourceAlt(    /* returns alt info pointer */
    OSPTUSAGEIND *ospvUsageInd,             /* usage indication */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvUsageInd->ospmUsageIndSourceAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPUsageIndHasDestinationAlt() - does an usage indication have a 
 * Destination Alternate?
 */
unsigned OSPPUsageIndHasDestinationAlt( /* returns non-zero if exists */
    OSPTUSAGEIND *ospvUsageInd)         /* usage indication */
{
    unsigned ospvHasDestinationAlt = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvHasDestinationAlt = (OSPPUsageIndFirstDestinationAlt(ospvUsageInd) != OSPC_OSNULL);
    }

    return ospvHasDestinationAlt;
}

/*
 * OSPPUsageIndFirstDestinationAlt() - gets the First Destination alternate for an 
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndFirstDestinationAlt(  /* returns alt info pointer */
    OSPTUSAGEIND *ospvUsageInd)                 /* usage indication */
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvUsageInd->ospmUsageIndDestinationAlternate));
    }

    return ospvAltInfo;
}

/*
 * OSPPUsageIndNextDestinationAlt() - gets the next Destination alternate for an 
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndNextDestinationAlt(   /* returns alt info pointer */
    OSPTUSAGEIND *ospvUsageInd,                 /* usage indication */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvUsageInd->ospmUsageIndDestinationAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPUsageIndGetDestinationAltSize() - gets the Destination alternate size 
 * for an altinfo
 */
unsigned OSPPUsageIndGetDestinationAltSize( /* returns alt info size */
    OSPT_ALTINFO *ospvAltInfo)              /* Alt info ptr */
{
    unsigned ospvAltInfoSize = 0;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvAltInfoSize = OSPPAltInfoGetSize(ospvAltInfo);
    }

    return ospvAltInfoSize;
}

/*
 * OSPPUsageIndSetStartTime() - Set Call Start Time
 */
void OSPPUsageIndSetStartTime(  /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    OSPTTIME ospvStartTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndStartTime = ospvStartTime;
    }
}

/*
 * OSPPUsageGetGetStartTime() - Set Call Start Time
 */
OSPTTIME  OSPPUsageIndGetStartTime( /* call start time */
    OSPTUSAGEIND *ospvUsageInd)
{
    OSPTTIME ospvStartTime = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvStartTime = ospvUsageInd->ospmUsageIndStartTime;
    }

    return ospvStartTime;
}

/*
 * OSPPUsageIndSetEndTime() - Set Call End Time
 */
void OSPPUsageIndSetEndTime(    /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    OSPTTIME ospvEndTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndEndTime = ospvEndTime;
    }
}

/*
 * OSPPUsageGetGetEndTime() - Get Call End Time
 */
OSPTTIME OSPPUsageIndGetEndTime(    /* call end time */
    OSPTUSAGEIND *ospvUsageInd)
{
    OSPTTIME ospvEndTime = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvEndTime = ospvUsageInd->ospmUsageIndEndTime;
    }

    return ospvEndTime;
}

/*
 * OSPPUsageIndSetConnectTime() - Set Call Connect Time
 */
void OSPPUsageIndSetConnectTime(    /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    OSPTTIME ospvConnectTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndConnectTime = ospvConnectTime;
    }
}

/*
 * OSPPUsageIndGetConnectTime() - Get Call Connect Time
 */
OSPTTIME OSPPUsageIndGetConnectTime(    /* call connect time */
    OSPTUSAGEIND *ospvUsageInd)
{
    OSPTTIME ospvConnectTime = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvConnectTime = ospvUsageInd->ospmUsageIndConnectTime;
    }

    return ospvConnectTime;
}

/*
 * OSPPUsageIndSetAlertTime() - Set Call Alert Time
 */
void OSPPUsageIndSetAlertTime(  /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    OSPTTIME ospvAlertTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ospmUsageIndAlertTime = ospvAlertTime;
    }
}

/*
 * OSPPUsageGetGetAlertTime() - Get Call Alert Time
 */
OSPTTIME OSPPUsageIndGetAlertTime(  /* call alert time */
    OSPTUSAGEIND *ospvUsageInd)
{
    OSPTTIME ospvAlertTime = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvAlertTime = ospvUsageInd->ospmUsageIndAlertTime;
    }

    return ospvAlertTime;
}

/*
 * OSPPUsageIndSetComponentId() - creates space and copies in the string.
 */
void OSPPUsageIndSetComponentId(
    OSPTUSAGEIND *ospvUsageInd,     /* In - pointer to Usage Indication struct */
    unsigned char *ospvComponentId) /* In - pointer to component id string */
{
    int len = OSPM_STRLEN((const char *)ospvComponentId);

    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvUsageInd->ospmUsageIndComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvUsageInd->ospmUsageIndComponentId);
        }

        OSPM_MALLOC(ospvUsageInd->ospmUsageIndComponentId, unsigned char, len + 1);
        OSPM_MEMSET(ospvUsageInd->ospmUsageIndComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageInd->ospmUsageIndComponentId, ospvComponentId, len);
    }
}

/*
 * OSPPUsageIndMoveDeviceinfo() - move the device info list from list
 */
void OSPPUsageIndMoveDeviceInfo(    /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST *ospvList)             /* list to move */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->ospmUsageIndDeviceInfo));
        OSPPListMove(&(ospvUsageInd->ospmUsageIndDeviceInfo), ospvList);
    }
}

/*
 * OSPPUsageIndMoveSourceAlt() - move the source alt list from list
 */
void OSPPUsageIndMoveSourceAlt( /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd, /* usage indication to set */
    OSPTLIST *ospvList          /* list to move */
    )
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->ospmUsageIndSourceAlternate));
        OSPPListMove(&(ospvUsageInd->ospmUsageIndSourceAlternate), ospvList);
    }
}

/*
 * OSPPUsageIndCopyDeviceInfo() - Copy the device info list
 */
void OSPPUsageIndCopyDeviceInfo(    /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST *ospvList)             /* list to move */
{
    OSPT_ALTINFO *altinfo1 = OSPC_OSNULL, *altinfo2 = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->ospmUsageIndDeviceInfo));

        for (altinfo1 = (OSPT_ALTINFO *)OSPPListFirst(ospvList);
             altinfo1 != OSPC_OSNULL;
             altinfo1 = (OSPT_ALTINFO *)OSPPListNext(ospvList, altinfo1))
        {
            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetType(altinfo1));
            if (altinfo2 != OSPC_OSNULL) {
                OSPPListAppend(&ospvUsageInd->ospmUsageIndDeviceInfo, altinfo2);
            }
            altinfo2 = OSPC_OSNULL;
        }
    }

    if (altinfo2 != OSPC_OSNULL) {
        OSPPAltInfoDelete(&altinfo2);
    }
}

/*
 * OSPPUsageIndMergeSourceAlt() - Merges the source alt list
 * The list (ospmAuthReqSourceAlternate) could contain - NetworkId, SrcAddr,
 * or the Subscriber Info. We need to copy everything from list1, except
 * the SrcAddr. The 2nd list contains the Updated SrcAddr that we just 
 * append to the list.
 */
void OSPPUsageIndMergeSourceAlt(    /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,
    OSPTLIST *ospvList1,
    OSPTLIST *ospvList2)
   
{
    OSPT_ALTINFO *altinfo1 = OSPC_OSNULL, *altinfo2 = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvList1 != OSPC_OSNULL) || (ospvList2 != OSPC_OSNULL))) {
        OSPPListNew(&(ospvUsageInd->ospmUsageIndSourceAlternate));
    }

    /*
     * Copy the node from List 2.
     */
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList2 != OSPC_OSNULL)) {
        for (altinfo1 = (OSPT_ALTINFO *) OSPPListFirst(ospvList2);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPT_ALTINFO *) OSPPListNext(ospvList2, altinfo1)) 
        {
            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetType(altinfo1));
            if (altinfo2 != OSPC_OSNULL) {
                OSPPListAppend(&ospvUsageInd->ospmUsageIndSourceAlternate, altinfo2);
            }
            altinfo2 = OSPC_OSNULL;
        }
    }

    if (altinfo2 != OSPC_OSNULL) {
        OSPPAltInfoDelete(&altinfo2);
    }

    /*
     * Now copy the nodes for Network/Subscriber Info
     */
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList1 != OSPC_OSNULL)) {
        for (altinfo1 = (OSPT_ALTINFO *) OSPPListFirst(ospvList1);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPT_ALTINFO *) OSPPListNext(ospvList1, altinfo1)) 
        {
            if (OSPPAltInfoGetType(altinfo1) != OSPC_ATYPE_TRANSPORT) {
                altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetType(altinfo1));
                if (altinfo2 != OSPC_OSNULL) {
                    OSPPListAppend(&ospvUsageInd-> ospmUsageIndSourceAlternate, altinfo2);
                }
                altinfo2 = OSPC_OSNULL;
            }
        }
    }

    if (altinfo2 != OSPC_OSNULL) {
        OSPPAltInfoDelete(&altinfo2);
    }
}

/*
 * OSPPUsageIndCopySourceAlt() - Copy the source alt list
 */
void OSPPUsageIndCopySourceAlt( /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd, /* usage indication to set */
    OSPTLIST *ospvList)         /* list to move */
{
    OSPT_ALTINFO *altinfo1 = OSPC_OSNULL, *altinfo2 = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->ospmUsageIndSourceAlternate));

        for (altinfo1 = (OSPT_ALTINFO *) OSPPListFirst(ospvList);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPT_ALTINFO *) OSPPListNext(ospvList, altinfo1)) 
        {
            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetType(altinfo1));
            if (altinfo2 != OSPC_OSNULL) {
                OSPPListAppend(&ospvUsageInd->ospmUsageIndSourceAlternate, altinfo2);
            }
            altinfo2 = OSPC_OSNULL;
        }
    }

    if (altinfo2 != OSPC_OSNULL) {
        OSPPAltInfoDelete(&altinfo2);
    }
}

/*
 * OSPPUsageIndMoveDestinationAlt() - move the destination alt list from 
 * a list
 */
void OSPPUsageIndMoveDestinationAlt(/* nothing returned */
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST *ospvList)             /* list to move */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->ospmUsageIndDestinationAlternate));
        OSPPListMove(&(ospvUsageInd->ospmUsageIndDestinationAlternate), ospvList);
    }
}

/*
 * OSPPUsageIndNew() - creates a new (empty) usage indication
 */
OSPTUSAGEIND *OSPPUsageIndNew(void) /* returns pointer or NULL */
{
    OSPTUSAGEIND *ospvUsageInd = OSPC_OSNULL;

    OSPM_MALLOC(ospvUsageInd, OSPTUSAGEIND, sizeof(OSPTUSAGEIND));
    if (ospvUsageInd != OSPC_OSNULL) {
        OSPM_MEMSET(ospvUsageInd, 0, sizeof(OSPTUSAGEIND));

        OSPPListLinkNew(&(ospvUsageInd->ospmUsageIndLink));
        ospvUsageInd->ospmUsageIndTimestamp = (OSPTTIME) 0;
        ospvUsageInd->ospmUsageIndStartTime = (OSPTTIME) 0;
        ospvUsageInd->ospmUsageIndAlertTime = (OSPTTIME) 0;
        ospvUsageInd->ospmUsageIndEndTime = (OSPTTIME) 0;
        ospvUsageInd->ospmUsageIndConnectTime = (OSPTTIME) 0;
        ospvUsageInd->ospvUsageIndIsPDDInfoPresent = 0;
        ospvUsageInd->ospmUsageIndPostDialDelay = 0;
        ospvUsageInd->ospmUsageIndReleaseSource = 0;
        ospvUsageInd->ospmUsageIndConferenceId[0] = '\0';
        ospvUsageInd->ospmUsageIndTransactionId = 0;
        ospvUsageInd->ospmUsageIndCallId = (OSPTCALLID *) OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndSourceNumber[0] = '\0';
        ospvUsageInd->ospmUsageIndDestNumber[0] = '\0';
        ospvUsageInd->ospmUsageIndDuration = -1;
        ospvUsageInd->ospmUsageIndTNCustId = 0L;
        ospvUsageInd->ospmUsageIndTNDeviceId = 0L;
        ospvUsageInd->ospmUsageIndTNFailReasonInd = 0;
        OSPPListNew(&(ospvUsageInd->ospmUsageIndSourceAlternate));
        OSPPListNew(&(ospvUsageInd->ospmUsageIndDestinationAlternate));
        OSPPListNew(&(ospvUsageInd->ospmUsageIndDeviceInfo));
        ospvUsageInd->ospmUsageIndTNStats = OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndComponentId = OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndMessageId = OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndIsPricingInfoPresent = OSPC_FALSE;
        ospvUsageInd->osmpUsageIndIsServiceInfoPresent = OSPC_FALSE;
        ospvUsageInd->ospmUsageIndDestinationCount = (OSPT_ALTINFO *)OSPC_OSNULL;
    }

    return ospvUsageInd;
}

/*
 * OSPPUsageIndDelete() - deletes a usage indication
 */
void OSPPUsageIndDelete(
    OSPTUSAGEIND **ospvUsageInd)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (*ospvUsageInd) {
        if (OSPPUsageIndHasCallId(*ospvUsageInd)) {
            OSPPCallIdDelete(&((*ospvUsageInd)->ospmUsageIndCallId));
        }

        if (OSPPUsageIndHasComponentId(*ospvUsageInd)) {
            OSPM_FREE((*ospvUsageInd)->ospmUsageIndComponentId);
        }

        while (!OSPPListEmpty(&((*ospvUsageInd)->ospmUsageIndSourceAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvUsageInd)->ospmUsageIndSourceAlternate));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }

        OSPPListDelete(&((*ospvUsageInd)->ospmUsageIndSourceAlternate));

        while (!OSPPListEmpty(&((*ospvUsageInd)->ospmUsageIndDeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvUsageInd)->ospmUsageIndDeviceInfo));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }

        OSPPListDelete(&((*ospvUsageInd)->ospmUsageIndDeviceInfo));

        while (!OSPPListEmpty(&((*ospvUsageInd)->ospmUsageIndDestinationAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvUsageInd)->ospmUsageIndDestinationAlternate));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }

        if (OSPPUsageIndHasTNStatistics(*ospvUsageInd)) {
            OSPPStatisticsDelete(&((*ospvUsageInd)->ospmUsageIndTNStats));
        }

        if (OSPPUsageIndHasComponentId(*ospvUsageInd)) {
            OSPM_FREE((*ospvUsageInd)->ospmUsageIndComponentId);
        }

        if (OSPPUsageIndHasMessageId(*ospvUsageInd)) {
            OSPM_FREE((*ospvUsageInd)->ospmUsageIndMessageId);
        }

        if (OSPPUsageIndGetDestinationCount(*ospvUsageInd) != (OSPT_ALTINFO *)OSPC_OSNULL) {
            OSPM_FREE((*ospvUsageInd)->ospmUsageIndDestinationCount);
        }

        OSPPListDelete(&((*ospvUsageInd)->ospmUsageIndDestinationAlternate));
        OSPM_FREE(*ospvUsageInd);

        *ospvUsageInd = OSPC_OSNULL;
    }
}

/*
 * OSPPUsageIndSetCallId() - sets the call ID for an usage
 */
void OSPPUsageIndSetCallId(     /* nothing returned */
    OSPTUSAGEIND *ospvUsageInd, /* usage indication */
    OSPTCALLID *ospvCallId      /* call ID */
    )
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if ((ospvCallId) != OSPC_OSNULL) {
            if (ospvUsageInd->ospmUsageIndCallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvUsageInd->ospmUsageIndCallId));
            }
            ospvUsageInd->ospmUsageIndCallId = OSPPCallIdNew(ospvCallId->ospmCallIdLen, ospvCallId->ospmCallIdVal);
        }
    }
}

/*
 * OSPPUsageIndHasTNStatistics() - does usageind have statistics?
 */
OSPTBOOL OSPPUsageIndHasTNStatistics(
    OSPTUSAGEIND *ospvUsageInd)
{
    OSPTBOOL hasstats = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvUsageInd->ospmUsageIndTNStats != OSPC_OSNULL) {
            hasstats = OSPC_TRUE;
        }
    }

    return hasstats;
}

/*
 * OSPPUsageIndSetTNStatistics() - set values for statistics in usageind
 */
void OSPPUsageIndSetTNStatistics(
    OSPTUSAGEIND *ospvUsageInd,     /* In - ptr to usage ind */
    OSPTSTATISTICS *ospvStatistics) /* In - ptr to completed stats struct */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvStatistics != OSPC_OSNULL)) {
        ospvUsageInd->ospmUsageIndTNStats = OSPPStatisticsNew();
        OSPM_MEMCPY(ospvUsageInd->ospmUsageIndTNStats, ospvStatistics, sizeof(OSPTSTATISTICS));
    }
}

/*
 * OSPPUsageIndToElement() - create an XML element from a usage ind
 */
int OSPPUsageIndToElement(      /* returns error code */
    OSPTLIST *ospvUsageInd,     /* usage ind list */
    OSPT_XML_ELEM **ospvElem,   /* where to put XML element pointer */
    void *ospvtrans)
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *usgindelem = OSPC_OSNULL, *subelem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    int len = 0;
    OSPTTRXID trxid = 0;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPTUSAGEIND *usage = OSPC_OSNULL;
    char random[OSPC_MAX_RANDOM];
    OSPTBOOL isbase64 = OSPC_TRUE;
    OSPTTRANS *trans = (OSPTTRANS *)ospvtrans;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvUsageInd == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_USAGEIND;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the "Message" element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");

        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            /* Add the Message attribute */
            usage = (OSPTUSAGEIND *) OSPPListFirst(ospvUsageInd);
            attr = OSPPXMLAttrNew((const unsigned char *)
                OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID),
                (OSPPUsageIndHasMessageId(usage)) ? (const unsigned char *)(usage-> ospmUsageIndMessageId) : (const unsigned char *)"NULL");

            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }

            /* random */
            if ((OSPPUtilGetRandom(random, 0) > 0) &&
                (ospvErrCode == OSPC_ERR_NO_ERROR)) {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const unsigned char *)random);

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

        /* Build multiple usage ind if there are more than one */
        for (usage = (OSPTUSAGEIND *) OSPPListFirst(ospvUsageInd);
            (ospvErrCode == OSPC_ERR_NO_ERROR) &&
            (usage != OSPC_OSNULL);
            usage = (OSPTUSAGEIND *) OSPPListNext(ospvUsageInd, usage)) 
        {
            /* create the usage element */
            usgindelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_USAGEIND), "");
            if (usgindelem == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }

            /* 
             * now add the attributes to the parent -- in this case the component id
             */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID),
                    (OSPPUsageIndHasComponentId(usage)) ? (const unsigned char *)(usage->ospmUsageIndComponentId) : (const unsigned char *) "NULL");
                if (attr == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddAttr(usgindelem, attr);
                attr = OSPC_OSNULL;
            }

            /* now add the children - start with timestamp */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                OSPPUsageIndHasTimestamp(usage)) {
                ospvErrCode = OSPPMsgTimeToElement(OSPPUsageIndGetTimestamp(usage),
                    (const unsigned char *)OSPPMsgElemGetName
                    (OSPC_MELEM_TIMESTAMP), 
                    &subelem);
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(usgindelem, subelem);
                subelem = OSPC_OSNULL;
            }

            /* add role  */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                OSPPUsageIndHasRole(usage)) {
                ospvErrCode = OSPPMsgRoleToElement(OSPPUsageIndGetRole(usage),
                    (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_ROLE),
                    &subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the transaction ID */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasTransactionId(usage)) {
                len = sizeof(OSPTTRXID);
                trxid = OSPPUsageIndGetTransactionId(usage);
                ospvErrCode = OSPPMsgTXToElement(trxid,
                    (const unsigned char *)OSPPMsgElemGetName
                    (OSPC_MELEM_TRANSID),&subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the call ID */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                OSPPUsageIndHasCallId(usage)) {
                ospvErrCode = OSPPCallIdToElement(OSPPUsageIndGetCallId(usage), &subelem, isbase64);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the source number */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                subelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_SRCINFO), (const char *)OSPPUsageIndGetSourceNumber(usage));
                if (subelem == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                if (trans->CallingNumberFormat == OSPC_E164) {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                        (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_E164));
                } else if (trans->CallingNumberFormat == OSPC_SIP) {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                        (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_SIP));
                } else if (trans->CallingNumberFormat == OSPC_URL) {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                        (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_URL));
                }

                if (attr == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddAttr(subelem, attr);
                attr = OSPC_OSNULL;
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(usgindelem, subelem);
                subelem = OSPC_OSNULL;
            }

            /* add the device info */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                (usage->ospmUsageIndDeviceInfo != NULL)) {
                for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(usage->ospmUsageIndDeviceInfo));
                    altinfo != (OSPT_ALTINFO *) OSPC_OSNULL;
                    altinfo = (OSPT_ALTINFO *)OSPPListNext(&(usage->ospmUsageIndDeviceInfo), altinfo)) 
                {
                    ospvErrCode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_DEVICEINFO);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add the source alternates */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                OSPPUsageIndHasSourceAlt(usage)) {
                for (altinfo = (OSPT_ALTINFO *) OSPPUsageIndFirstSourceAlt(usage);
                    ((altinfo != (OSPT_ALTINFO *) OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                     altinfo = (OSPT_ALTINFO *) OSPPUsageIndNextSourceAlt(usage, altinfo)) 
                {
                    ospvErrCode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_SRCALT);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add the destination number */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                subelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTINFO), (const char *)OSPPUsageIndGetDestNumber(usage));
                if (subelem == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                if (trans->CalledNumberFormat == OSPC_E164) {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                        (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_E164));
                } else if (trans->CalledNumberFormat == OSPC_SIP) {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                        (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_SIP));
                } else if (trans->CalledNumberFormat == OSPC_URL) {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE),
                        (const unsigned char *)OSPPAltInfoTypeGetName(OSPC_ATYPE_URL));
                }

                if (attr == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddAttr(subelem, attr);
                attr = OSPC_OSNULL;
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(usgindelem, subelem);
                subelem = OSPC_OSNULL;
            }

            /* add the destination alternates */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                OSPPUsageIndHasDestinationAlt(usage)) {
                for (altinfo = (OSPT_ALTINFO *) OSPPUsageIndFirstDestinationAlt(usage);
                    ((altinfo != (OSPT_ALTINFO *) OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                    altinfo = (OSPT_ALTINFO *) OSPPUsageIndNextDestinationAlt(usage, altinfo))
                {
                    ospvErrCode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_DESTALT);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add destination count */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (OSPPUsageIndGetDestinationCount(usage) != (OSPT_ALTINFO *) OSPC_OSNULL)) {
                altinfo = OSPPUsageIndGetDestinationCount(usage);

                ospvErrCode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_DESTALT);

                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /*
             * Add Pricing Info 
             */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (usage->ospmUsageIndIsPricingInfoPresent == OSPC_TRUE)) {
                ospvErrCode = OSPPAddPricingInfoToUsageElement(usage-> osmpUsageIndPricingInfo, &subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /*
             * Add Service Info
             */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (usage->osmpUsageIndIsServiceInfoPresent == OSPC_TRUE)) {
                ospvErrCode = OSPPAddServiceTypeToUsageElement(usage-> osmpUsageIndServiceType, &subelem);
                                                    
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /*
             * Add conference id if present 
             */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (OSPPUsageIndGetIsConfIdPresent(usage))) {
                ospvErrCode = OSPPAddConfIdToUsageElement(OSPPUsageIndGetConferenceId (usage), &subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add usage detail (if appropriate) */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                OSPPUsageIndHasDuration(usage)) {
                ospvErrCode = OSPPUsageToElement((unsigned)
                    OSPPUsageIndGetDuration(usage),
                    OSPPUsageIndGetStartTime(usage),
                    OSPPUsageIndGetEndTime(usage),
                    OSPPUsageIndGetAlertTime(usage),
                    OSPPUsageIndGetConnectTime(usage),
                    OSPPUsageIndGetIsPDDInfoPresent(usage),
                    OSPPUsageIndGetPostDialDelay(usage),
                    OSPPUsageIndGetReleaseSource(usage),
                    &subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* now add the transnexus extensions (if available) */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                if (OSPPUsageIndHasTNCustId(usage)) {
                    ospvErrCode = OSPPMsgNumToElement(OSPPUsageIndGetTNCustId(usage),
                        (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_CUSTID), &subelem);
                    /*add attribute critical = "False" since not all servers understand */
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), (const unsigned char *) "False");
                        if (attr != OSPC_OSNULL) {
                            OSPPXMLElemAddAttr(subelem, attr);
                            attr = OSPC_OSNULL;
                        } else {
                            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                        }

                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                    if (OSPPUsageIndHasTNDeviceId(usage)) {
                        ospvErrCode = OSPPMsgNumToElement(OSPPUsageIndGetTNDeviceId(usage),
                            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_DEVICEID), &subelem);

                        /*add attribute critical = "False" since not all servers understand */
                        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), (const unsigned char *)"False");
                            if (attr != OSPC_OSNULL) {
                                OSPPXMLElemAddAttr(subelem, attr);
                                attr = OSPC_OSNULL;
                            } else {
                                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                            }
                            OSPPXMLElemAddChild(usgindelem, subelem);
                            subelem = OSPC_OSNULL;
                        }
                    }

                }
            }

            /* Failure reason */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                if (OSPPUsageIndHasTNFailReason(usage)) {
                    ospvErrCode = OSPPMsgNumToElement(OSPPUsageIndGetTNFailReason(usage),
                        (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_FAILREASON), &subelem);

                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* Statistics */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {

                if (OSPPUsageIndHasTNStatistics(usage)) {
                    ospvErrCode = OSPPStatisticsToElement(usage->ospmUsageIndTNStats, &subelem);

                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, usgindelem);
                usgindelem = OSPC_OSNULL;
            }
        }                        /* end for */
    }

    /* if for any reason we found an error - destroy any elements created */
    if (ospvErrCode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    if (usgindelem != OSPC_OSNULL) {
        OSPPXMLElemDelete(&usgindelem);
    }

    if (subelem != OSPC_OSNULL) {
        OSPPXMLElemDelete(&subelem);
    }

    if (attr != OSPC_OSNULL) {
        OSPPXMLAttrDelete(&attr);
    }

    return (ospvErrCode);
}


/*
 * OSPPUsageIndAddDestinationAlt() - add a destination alt to list a list
 */
void OSPPUsageIndAddDestinationAlt(
    OSPTUSAGEIND *ospvUsageInd,     /* authorisation indication */
    OSPT_ALTINFO *ospvAltInfo)      /* alt info to add */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvAltInfo != OSPC_OSNULL)) {
        OSPPListAppend(&(ospvUsageInd->ospmUsageIndDestinationAlternate),
                       (ospvAltInfo));
    }
}

/*
 * OSPPUsageIndHasMessageId() - is the message id set ?
 */
unsigned OSPPUsageIndHasMessageId(  /* returns non-zero if message id is set */
    OSPTUSAGEIND *ospvUsageInd)
{
    return (ospvUsageInd->ospmUsageIndMessageId != OSPC_OSNULL);
}

/*
 * OSPPUsageIndGetMessageId() - returns a new copy of the message id.
 */
unsigned char *OSPPUsageIndGetMessageId(
    OSPTUSAGEIND *ospvUsageInd)
{
    unsigned char *messagestring = OSPC_OSNULL;
    int len = 0;

    if (OSPPUsageIndHasMessageId(ospvUsageInd)) {
        len = OSPM_STRLEN((const char *) ospvUsageInd->ospmUsageIndMessageId);
        OSPM_MALLOC(messagestring, unsigned char, len + 1);
        OSPM_MEMSET(messagestring, 0, len + 1);
        OSPM_MEMCPY(messagestring, ospvUsageInd->ospmUsageIndMessageId, len);
    }

    return messagestring;
}


void OSPPUsageIndSetDestinationCount(
    OSPTUSAGEIND *ospvUsageInd,
    unsigned ospvDestinationCount)
{
    char buf[64];

    if (ospvDestinationCount > 0) {
        sprintf(buf, "%d", ospvDestinationCount);
        ospvUsageInd->ospmUsageIndDestinationCount = OSPPAltInfoNew(strlen(buf), (unsigned char *)buf, OSPC_ATYPE_DEVICEID);
    }
}

OSPT_ALTINFO *OSPPUsageIndGetDestinationCount(
    OSPTUSAGEIND *ospvUsageInd)
{
    return ospvUsageInd->ospmUsageIndDestinationCount;
}
