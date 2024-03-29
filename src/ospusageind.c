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

/* ospusageind.c - OSP usage indication functions */

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
OSPTBOOL OSPPUsageIndHasTimestamp(  /* returns non-zero if number exists */
    OSPT_USAGE_IND *ospvUsageInd)   /* Usage Indication effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (ospvUsageInd->Timestamp != 0);
    }

    return has;
}

/*
 * OSPPUsageIndSetTimestamp() - set the timestamp
 */
void OSPPUsageIndSetTimestamp(      /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPTTIME ospvTimestamp)         /* timestamp to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvTimestamp != 0) {
            ospvUsageInd->Timestamp = ospvTimestamp;
        }
    }
}

/*
 * OSPPUsageIndGetTimestamp() - returns the timestamp for a usage ind
 */
OSPTTIME OSPPUsageIndGetTimestamp(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    OSPTTIME time = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        time = ospvUsageInd->Timestamp;
    }

    return time;
}

/*
 * OSPPUsageIndHasRole() - Does usage indication have role set?
 */
OSPTBOOL OSPPUsageIndHasRole(       /* returns non-zero if time */
    OSPT_USAGE_IND *ospvUsageInd)   /* usage indication in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = ospvUsageInd->HasRole;
    }

    return has;
}

/*
 * OSPPUsageIndGetRole() - returns role for an usage indication
 */
OSPE_ROLE OSPPUsageIndGetRole(      /* returns the role (OGW/TGW) */
    OSPT_USAGE_IND *ospvUsageInd)   /* usage indication */
{
    OSPE_ROLE role = OSPC_ROLE_UNDEFINED;

    if (ospvUsageInd != OSPC_OSNULL) {
        role = ospvUsageInd->Role;
    }

    return role;
}

/*
 * OSPPUsageIndSetRole() - sets the role for an usage indication
 */
void OSPPUsageIndSetRole(   /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_ROLE ospvRole)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->Role = ospvRole;
        ospvUsageInd->HasRole = OSPC_TRUE;
    }
}

/*
 * OSPPUsageIndHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPUsageIndHasComponentId(    /* returns non-zero if component id is set */
    OSPT_USAGE_IND *ospvUsageInd)
{
    return(ospvUsageInd->ComponentId != OSPC_OSNULL);
}

/*
 * OSPPUsageIndGetComponentId() - returns a new copy of the component id.
 */
const char *OSPPUsageIndGetComponentId(
    OSPT_USAGE_IND *ospvUsageInd)
{
    const char *componentstring = OSPC_OSNULL;
    int len = 0;

    if (OSPPUsageIndHasComponentId(ospvUsageInd)) {
        len = OSPM_STRLEN(ospvUsageInd->ComponentId);
        OSPM_MALLOC(componentstring, char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvUsageInd->ComponentId, len);
    }

    return componentstring;
}

/*
 * OSPPUsageIndHasTransactionId() - is the transaction id set ?
 */
OSPTBOOL OSPPUsageIndHasTransactionId(  /* returns non-zero if number exists */
    OSPT_USAGE_IND *ospvUsageInd)       /* Usage Indication effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (ospvUsageInd->TransactionId != 0);
    }

    return has;
}

/*
 * OSPPUsageIndSetTransactionId() - set the transaction id
 */
void OSPPUsageIndSetTransactionId(  /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPTTRXID ospvTransactionId)    /* transaction id to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvTransactionId != 0) {
            ospvUsageInd->TransactionId = ospvTransactionId;
        }
    }
}

/*
 * OSPPUsageIndGetTransactionId() - returns the trans id for a usage ind
 */
OSPTTRXID OSPPUsageIndGetTransactionId(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    OSPTTRXID trxid = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        trxid = ospvUsageInd->TransactionId;
    }

    return trxid;
}

/*
 * OSPPUsageIndHasCallId() - does an usage indication have a Call ID?
 */
OSPTBOOL OSPPUsageIndHasCallId(     /* returns non-zero if exists */
    OSPT_USAGE_IND *ospvUsageInd)   /* usage indication */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (ospvUsageInd->CallId != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPUsageIndGetCallId() - gets the call ID for an usage indication
 */
OSPT_CALL_ID *OSPPUsageIndGetCallId(    /* returns call ID pointer */
    OSPT_USAGE_IND *ospvUsageInd)       /* usage indication */
{
    OSPT_CALL_ID *callid = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        callid = ospvUsageInd->CallId;
    }

    return callid;
}

/*
 * OSPPUsageIndSetSourceNumber() - set the source number
 */
void OSPPUsageIndSetSourceNumber(   /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    const char *ospvSourceNumber)   /* source number to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvSourceNumber != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvUsageInd->SourceNumber,
                ospvSourceNumber,
                OSPM_MIN(OSPM_STRLEN(ospvSourceNumber) + 1, OSPC_SIZE_E164NUM));
        }
    }
}

/*
 * OSPPUsageIndGetSourceNumber() - returns the source number for usage ind
 */
const char *OSPPUsageIndGetSourceNumber(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    const char *num = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        num = ospvUsageInd->SourceNumber;
    }

    return num;
}

/*
 * OSPPUsageIndSetConferenceId() - set the conference id
 */
void OSPPUsageIndSetConferenceId(   /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    const char *ospvConferenceId)   /* conference id to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvConferenceId != OSPC_OSNULL) {
            OSPM_STRCPY(ospvUsageInd->ConferenceId, ospvConferenceId);
        }
    }
}

/*
 * OSPPUsageIndGetHasConfId() - Checks if conf id is present.
 */
int OSPPUsageIndGetHasConfId(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvUsageInd->ConferenceId[0] != '\0') {
            has = OSPC_TRUE;
        }
    }

    return has;
}

/*
 * OSPPUsageIndGetConferenceId() - returns the conf id
 */
const char *OSPPUsageIndGetConferenceId(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    const char *conferenceid = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        conferenceid = ospvUsageInd->ConferenceId;
    }

    return conferenceid;
}

/*
 * OSPPUsageIndSetDestNumber() - set the destination number
 */
void OSPPUsageIndSetDestNumber(     /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    const char *ospvDestNumber)     /* destination number to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvDestNumber != OSPC_OSNULL) {
            OSPM_STRNCPY(ospvUsageInd->DestinationNumber,
                ospvDestNumber,
                OSPM_MIN(OSPM_STRLEN(ospvDestNumber) + 1, OSPC_SIZE_E164NUM));
        }
    }
}

/*
 * OSPPUsageIndGetDestNumber() - returns the destination number for a usage ind
 */
const char *OSPPUsageIndGetDestNumber(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    const char *num = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        num = ospvUsageInd->DestinationNumber;
    }

    return num;
}

/*
 * OSPPUsageIndHasDuration() - is the duration set ?
 */
OSPTBOOL OSPPUsageIndHasDuration(   /* returns non-zero if number exists */
    OSPT_USAGE_IND *ospvUsageInd)   /* Usage Indication effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (ospvUsageInd->Duration >= 0);
    }

    return has;
}

/*
 * OSPPUsageIndSetDuration() - set the duration
 */
void OSPPUsageIndSetDuration(       /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    int ospvDuration)               /* duration to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvDuration >= 0) {
            ospvUsageInd->Duration = ospvDuration;
        }
    }
}

/*
 * OSPPUsageIndGetDuration() - returns the duration for a usage ind
 */
int OSPPUsageIndGetDuration(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    int duration = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        duration = ospvUsageInd->Duration;
    }

    return duration;
}

/*
 * OSPPUsageIndSetHasPDDInfo() - set the IsPDDInfoPResent variable
 */
void OSPPUsageIndSetHasPDDInfo(     /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPTBOOL HasPDDInfo)            /* duration to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->HasPDD = HasPDDInfo;
    }
}

/*
 * OSPPUsageIndHasPDD() - gets the HasPDD variable.
 */
OSPTBOOL OSPPUsageIndHasPDD(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = ospvUsageInd->HasPDD;
    }

    return has;
}

/*
 * OSPPUsageIndSetReleaseSource() - set the Rel Src
 */
void OSPPUsageIndSetReleaseSource(  /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPE_RELEASE ospvReleaseSource) /* Rel Src to set to */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if ((ospvReleaseSource == OSPC_RELEASE_UNKNOWN) || ((ospvReleaseSource >= OSPC_RELEASE_START) && (ospvReleaseSource < OSPC_RELEASE_NUMBER))) {
            ospvUsageInd->ReleaseSource = ospvReleaseSource;
        } else {
            ospvUsageInd->ReleaseSource = OSPC_RELEASE_UNDEFINED;
        }
    }
}

/*
 * OSPPUsageIndGetReleaseSource() - returns the Rel Src for a usage ind
 */
OSPE_RELEASE OSPPUsageIndGetReleaseSource(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    OSPE_RELEASE ospvReleaseSource = OSPC_RELEASE_UNDEFINED;

    if (ospvUsageInd != OSPC_OSNULL) {
        if ((ospvUsageInd->ReleaseSource == OSPC_RELEASE_UNKNOWN) ||
            ((ospvUsageInd->ReleaseSource >= OSPC_RELEASE_START) && (ospvUsageInd->ReleaseSource < OSPC_RELEASE_NUMBER)))
        {
            ospvReleaseSource = ospvUsageInd->ReleaseSource;
        }
    }

    return ospvReleaseSource;
}

/*
 * OSPPUsageIndSetPostDialDelay() - set the PDD
 */
void OSPPUsageIndSetPostDialDelay(  /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    unsigned ospvPostDialDelay)     /* PDD in milliseconds */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->PostDialDelay = ospvPostDialDelay;
        ospvUsageInd->HasPDD = OSPC_TRUE;
    }
}

/*
 * OSPPUsageIndGetPostDialDelay() - returns the PDD for a usage ind in milliseconds
 */
unsigned OSPPUsageIndGetPostDialDelay(
    OSPT_USAGE_IND *ospvUsageInd)   /* usage ind */
{
    int pdd = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        pdd = ospvUsageInd->PostDialDelay;
    }

    return pdd;
}

/*
 * OSPPUsageIndHasCustId() - Does usage have a Customer Id?
 */
OSPTBOOL OSPPUsageIndHasCustId(     /* returns non-zero if time */
    OSPT_USAGE_IND *ospvUsageInd)   /* usage in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (ospvUsageInd->CustomerId != 0L);
    }

    return has;
}

/*
 * OSPPUsageIndSetCustId() - Set Customer Id
 */
void OSPPUsageIndSetCustId(     /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    unsigned long ospvCustId)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->CustomerId = ospvCustId;
    }
}

/*
 * OSPPUsageIndGetCustId() - returns Customer Id for an usage request
 */
unsigned long OSPPUsageIndGetCustId(    /* returns the time value */
    OSPT_USAGE_IND *ospvUsageInd)       /* usage request */
{
    unsigned long ospvCustId = 0L;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvCustId = ospvUsageInd->CustomerId;
    }

    return ospvCustId;
}

/*
 * OSPPUsageIndHasDeviceId() - Does request have a Device Id?
 */
OSPTBOOL OSPPUsageIndHasDeviceId(   /* returns non-zero if time */
    OSPT_USAGE_IND *ospvUsageInd)   /* usage request in question */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (ospvUsageInd->DeviceId != 0L);
    }

    return has;
}

/*
 * OSPPUsageIndSetDeviceId() - Set Device Id
 */
void OSPPUsageIndSetDeviceId(   /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    unsigned long ospvDeviceId)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->DeviceId = ospvDeviceId;
    }
}

/*
 * OSPPUsageIndGetDeviceId() - returns Device Id for an usage request
 */
unsigned long OSPPUsageIndGetDeviceId(  /* returns the time value */
    OSPT_USAGE_IND *ospvUsageInd)       /* usage request */
{
    unsigned long deviceid = 0L;

    if (ospvUsageInd != OSPC_OSNULL) {
        deviceid = ospvUsageInd->DeviceId;
    }

    return deviceid;
}

/*
 * OSPPUsageIndHasTermCause() - Does request have a Fail Reason
 */
OSPTBOOL OSPPUsageIndHasTermCause(  /* returns non-zero if time */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage request in question */
    OSPE_TERM_CAUSE ospvType)       /* termination cause type */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = OSPPHasTermCause(&ospvUsageInd->TermCause, ospvType);
    }

    return has;
}

/*
 * OSPPUsageIndCopyTermCause() - Copy Fail Reason
 */
void OSPPUsageIndCopyTermCause(     /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    OSPT_TERM_CAUSE *ospvTermCause)
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvTermCause != OSPC_OSNULL)) {
        OSPM_MEMCPY(&ospvUsageInd->TermCause, ospvTermCause, sizeof(OSPT_TERM_CAUSE));
    }
}

/*
 * OSPPUsageIndSetTermCause() - Set Fail Reason
 */
void OSPPUsageIndSetTermCause(      /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_TERM_CAUSE ospvType,
    unsigned ospvTCCode,
    const char *ospvTCDesc)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        OSPPSetTermCause(&ospvUsageInd->TermCause, ospvType, ospvTCCode, ospvTCDesc);
    }
}

/*
 * OSPPUsageIndGetTCCode() - returns Fail Reason value for an usage request
 */
unsigned OSPPUsageIndGetTCCode(
    OSPT_USAGE_IND *ospvUsageInd,   /* usage request */
    OSPE_TERM_CAUSE ospvType)       /* fail reason type */
{
    unsigned tccode = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        tccode = OSPPGetTCCode(&ospvUsageInd->TermCause, ospvType);
    }

    return tccode;
}

/*
 * OSPPUsageIndGetTCDesc() - returns Fail Reason description for an usage request
 */
const char *OSPPUsageIndGetTCDesc(
    OSPT_USAGE_IND *ospvUsageInd,   /* usage request */
    OSPE_TERM_CAUSE ospvType)       /* fail reasion type */
{
    const char *tcdesc = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        tcdesc = OSPPGetTCDesc(&ospvUsageInd->TermCause, ospvType);
    }

    return tcdesc;
}

/*
 * OSPPUsageIndHasSourceAlt() - does an usage indication have a
 * Source Alternate?
 */
OSPTBOOL OSPPUsageIndHasSourceAlt(  /* returns non-zero if exists */
    OSPT_USAGE_IND *ospvUsageInd)   /* usage indication */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (OSPPUsageIndFirstSourceAlt(ospvUsageInd) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPUsageIndFirstSourceAlt() - gets the First Source alternate for an
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndFirstSourceAlt(   /* returns alt info pointer */
    OSPT_USAGE_IND *ospvUsageInd)           /* usage indication */
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvUsageInd->SourceAlternate));
    }

    return altinfo;
}

/*
 * OSPPUsageIndNextSourceAlt() - gets the next source alternate for an
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndNextSourceAlt(    /* returns alt info pointer */
    OSPT_USAGE_IND *ospvUsageInd,           /* usage indication */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvUsageInd->SourceAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPUsageIndHasDestinationAlt() - does an usage indication have a
 * Destination Alternate?
 */
OSPTBOOL OSPPUsageIndHasDestinationAlt( /* returns non-zero if exists */
    OSPT_USAGE_IND *ospvUsageInd)       /* usage indication */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        has = (OSPPUsageIndFirstDestinationAlt(ospvUsageInd) != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPUsageIndFirstDestinationAlt() - gets the First Destination alternate for an
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndFirstDestinationAlt(  /* returns altinfo pointer */
    OSPT_USAGE_IND *ospvUsageInd)               /* usage indication */
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvAltInfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvUsageInd->DestinationAlternate));
    }

    return ospvAltInfo;
}

/*
 * OSPPUsageIndNextDestinationAlt() - gets the next Destination alternate for an
 * usage indication
 */
OSPT_ALTINFO *OSPPUsageIndNextDestinationAlt(   /* returns altinfo pointer */
    OSPT_USAGE_IND *ospvUsageInd,               /* usage indication */
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL) {
        altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvUsageInd->DestinationAlternate), ospvAltInfo);
    }

    return altinfo;
}

/*
 * OSPPUsageIndGetDestinationAltSize() - gets the Destination alternate size
 * for an altinfo
 */
unsigned OSPPUsageIndGetDestinationAltSize( /* returns altinfo size */
    OSPT_ALTINFO *ospvAltInfo)              /* altinfo ptr */
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
    OSPT_USAGE_IND *ospvUsageInd,
    OSPTTIME ospvStartTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->StartTime = ospvStartTime;
    }
}

/*
 * OSPPUsageGetGetStartTime() - Set Call Start Time
 */
OSPTTIME  OSPPUsageIndGetStartTime( /* call start time */
    OSPT_USAGE_IND *ospvUsageInd)
{
    OSPTTIME ospvStartTime = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        ospvStartTime = ospvUsageInd->StartTime;
    }

    return ospvStartTime;
}

/*
 * OSPPUsageIndSetEndTime() - Set Call End Time
 */
void OSPPUsageIndSetEndTime(    /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    OSPTTIME ospvEndTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->EndTime = ospvEndTime;
    }
}

/*
 * OSPPUsageGetGetEndTime() - Get Call End Time
 */
OSPTTIME OSPPUsageIndGetEndTime(    /* call end time */
    OSPT_USAGE_IND *ospvUsageInd)
{
    OSPTTIME time = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        time = ospvUsageInd->EndTime;
    }

    return time;
}

/*
 * OSPPUsageIndSetConnectTime() - Set Call Connect Time
 */
void OSPPUsageIndSetConnectTime(    /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    OSPTTIME ospvConnectTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->ConnectTime = ospvConnectTime;
    }
}

/*
 * OSPPUsageIndGetConnectTime() - Get Call Connect Time
 */
OSPTTIME OSPPUsageIndGetConnectTime(    /* call connect time */
    OSPT_USAGE_IND *ospvUsageInd)
{
    OSPTTIME time = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        time = ospvUsageInd->ConnectTime;
    }

    return time;
}

/*
 * OSPPUsageIndSetAlertTime() - Set Call Alert Time
 */
void OSPPUsageIndSetAlertTime(  /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,
    OSPTTIME ospvAlertTime)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        ospvUsageInd->AlertTime = ospvAlertTime;
    }
}

/*
 * OSPPUsageGetGetAlertTime() - Get Call Alert Time
 */
OSPTTIME OSPPUsageIndGetAlertTime(  /* call alert time */
    OSPT_USAGE_IND *ospvUsageInd)
{
    OSPTTIME time = 0;

    if (ospvUsageInd != OSPC_OSNULL) {
        time = ospvUsageInd->AlertTime;
    }

    return time;
}

/*
 * OSPPUsageIndSetComponentId() - creates space and copies in the string.
 */
void OSPPUsageIndSetComponentId(
    OSPT_USAGE_IND *ospvUsageInd,   /* In - pointer to Usage Indication struct */
    const char *ospvComponentId)    /* In - pointer to component id string */
{
    int len = OSPM_STRLEN(ospvComponentId);

    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvUsageInd->ComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvUsageInd->ComponentId);
        }

        OSPM_MALLOC(ospvUsageInd->ComponentId, char, len + 1);
        OSPM_MEMSET(ospvUsageInd->ComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageInd->ComponentId, ospvComponentId, len);
    }
}

/*
 * OSPPUsageIndMoveDeviceinfo() - move the device info list from list
 */
void OSPPUsageIndMoveDeviceInfo(    /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPTLIST *ospvList)             /* list to move */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->DeviceInfo));
        OSPPListMove(&(ospvUsageInd->DeviceInfo), ospvList);
    }
}

/*
 * OSPPUsageIndMoveSourceAlt() - move the source alt list from list
 */
void OSPPUsageIndMoveSourceAlt(     /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPTLIST *ospvList              /* list to move */
    )
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->SourceAlternate));
        OSPPListMove(&(ospvUsageInd->SourceAlternate), ospvList);
    }
}

/*
 * OSPPUsageIndCopyDeviceInfo() - Copy the device info list
 */
void OSPPUsageIndCopyDeviceInfo(    /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPTLIST *ospvList)             /* list to move */
{
    OSPT_ALTINFO *altinfo1 = OSPC_OSNULL, *altinfo2 = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->DeviceInfo));

        for (altinfo1 = (OSPT_ALTINFO *)OSPPListFirst(ospvList);
             altinfo1 != OSPC_OSNULL;
             altinfo1 = (OSPT_ALTINFO *)OSPPListNext(ospvList, altinfo1))
        {
            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetPart(altinfo1));
            if (altinfo2 != OSPC_OSNULL) {
                OSPPListAppend(&ospvUsageInd->DeviceInfo, altinfo2);
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
    OSPT_USAGE_IND *ospvUsageInd,
    OSPTLIST *ospvList1,
    OSPTLIST *ospvList2)

{
    OSPT_ALTINFO *altinfo1 = OSPC_OSNULL, *altinfo2 = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvList1 != OSPC_OSNULL) || (ospvList2 != OSPC_OSNULL))) {
        OSPPListNew(&(ospvUsageInd->SourceAlternate));
    }

    /*
     * Copy the node from List 2.
     */
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList2 != OSPC_OSNULL)) {
        for (altinfo1 = (OSPT_ALTINFO *)OSPPListFirst(ospvList2);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPT_ALTINFO *)OSPPListNext(ospvList2, altinfo1))
        {
            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetPart(altinfo1));
            if (altinfo2 != OSPC_OSNULL) {
                OSPPListAppend(&ospvUsageInd->SourceAlternate, altinfo2);
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
        for (altinfo1 = (OSPT_ALTINFO *)OSPPListFirst(ospvList1);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPT_ALTINFO *)OSPPListNext(ospvList1, altinfo1))
        {
            if (OSPPAltInfoGetPart(altinfo1) != OSPC_ALTINFO_TRANSPORT) {
                altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetPart(altinfo1));
                if (altinfo2 != OSPC_OSNULL) {
                    OSPPListAppend(&ospvUsageInd->SourceAlternate, altinfo2);
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
void OSPPUsageIndCopySourceAlt(     /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication to set */
    OSPTLIST *ospvList)             /* list to move */
{
    OSPT_ALTINFO *altinfo1 = OSPC_OSNULL, *altinfo2 = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->SourceAlternate));

        for (altinfo1 = (OSPT_ALTINFO *)OSPPListFirst(ospvList);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPT_ALTINFO *)OSPPListNext(ospvList, altinfo1))
        {
            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1), OSPPAltInfoGetValue(altinfo1), OSPPAltInfoGetPart(altinfo1));
            if (altinfo2 != OSPC_OSNULL) {
                OSPPListAppend(&ospvUsageInd->SourceAlternate, altinfo2);
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
void OSPPUsageIndMoveDestinationAlt(    /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,       /* usage indication to set */
    OSPTLIST *ospvList)                 /* list to move */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvList != OSPC_OSNULL)) {
        OSPPListNew(&(ospvUsageInd->DestinationAlternate));
        OSPPListMove(&(ospvUsageInd->DestinationAlternate), ospvList);
    }
}

/*
 * OSPPUsageIndNew() - creates a new (empty) usage indication
 */
OSPT_USAGE_IND *OSPPUsageIndNew(void)    /* returns pointer or NULL */
{
    OSPT_USAGE_IND *usageind = OSPC_OSNULL;
    OSPE_TERM_CAUSE cause;
    OSPE_PROTOCOL_TYPE prot;
    OSPE_SERVICE svc;
    OSPE_CODEC_TYPE codec;
    OSPE_SESSION_ID sess;

    OSPM_MALLOC(usageind, OSPT_USAGE_IND, sizeof(OSPT_USAGE_IND));
    if (usageind != OSPC_OSNULL) {
        OSPM_MEMSET(usageind, 0, sizeof(OSPT_USAGE_IND));

        OSPPListLinkNew(&(usageind->Link));
        usageind->Timestamp = (OSPTTIME) 0;
        usageind->StartTime = (OSPTTIME) 0;
        usageind->AlertTime = (OSPTTIME) 0;
        usageind->EndTime = (OSPTTIME) 0;
        usageind->ConnectTime = (OSPTTIME) 0;
        usageind->HasPDD = OSPC_FALSE;
        usageind->PostDialDelay = 0;
        usageind->ProviderPDD = -1;
        usageind->ReleaseSource = OSPC_RELEASE_UNDEFINED;
        usageind->ConferenceId[0] = '\0';
        usageind->Role = OSPC_ROLE_UNDEFINED;
        usageind->HasRole = OSPC_FALSE;
        usageind->TransactionId = 0;
        usageind->CallId = OSPC_OSNULL;
        usageind->SourceNumber[0] = '\0';
        usageind->DestinationNumber[0] = '\0';
        usageind->Duration = -1;
        usageind->CustomerId = 0L;
        usageind->DeviceId = 0L;
        for (cause = OSPC_TCAUSE_START; cause < OSPC_TCAUSE_NUMBER; cause++) {
            usageind->TermCause.hastermcause[cause] = OSPC_FALSE;
        }
        OSPPListNew(&(usageind->SourceAlternate));
        OSPPListNew(&(usageind->DestinationAlternate));
        OSPPListNew(&(usageind->DeviceInfo));
        usageind->Stats = OSPC_OSNULL;
        usageind->ComponentId = OSPC_OSNULL;
        usageind->MessageId = OSPC_OSNULL;
        usageind->HasPricingInfo = OSPC_FALSE;
        usageind->HasServiceInfo = OSPC_FALSE;
        usageind->DestinationCount = OSPC_OSNULL;
        usageind->SetupAttempt = -1;
        for (prot = OSPC_PROTTYPE_START; prot < OSPC_PROTTYPE_NUMBER; prot++) {
            usageind->Protocol[prot] = OSPC_PROTNAME_UNKNOWN;
        }
        for (svc = OSPC_SERVICE_START; svc < OSPC_SERVICE_NUMBER; svc++) {
            for (codec = OSPC_CODEC_START; codec < OSPC_CODEC_NUMBER; codec++) {
                usageind->Codec[svc][codec][0] = '\0';
            }
        }
        for (sess = OSPC_SESSIONID_START; sess < OSPC_SESSIONID_NUMBER; sess++) {
            usageind->SessionId[sess] = OSPC_OSNULL;
        }
        usageind->RoleState = OSPC_RSTATE_UNKNOWN;
        usageind->RoleFormat = OSPC_RFORMAT_UNKNOWN;
        usageind->RoleVendor = OSPC_RVENDOR_UNKNOWN;
        usageind->TransferId[0] = '\0';
        usageind->TransferStatus = OSPC_TSTATUS_UNKNOWN;
    }

    return usageind;
}

/*
 * OSPPUsageIndDelete() - deletes a usage indication
 */
void OSPPUsageIndDelete(
    OSPT_USAGE_IND **ospvUsageInd)
{
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    unsigned cnt;

    if (*ospvUsageInd) {
        if (OSPPUsageIndHasCallId(*ospvUsageInd)) {
            OSPPCallIdDelete(&((*ospvUsageInd)->CallId));
        }

        if (OSPPUsageIndHasComponentId(*ospvUsageInd)) {
            OSPM_FREE((*ospvUsageInd)->ComponentId);
        }

        while (!OSPPListEmpty(&((*ospvUsageInd)->SourceAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvUsageInd)->SourceAlternate));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }

        OSPPListDelete(&((*ospvUsageInd)->SourceAlternate));

        while (!OSPPListEmpty(&((*ospvUsageInd)->DeviceInfo))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvUsageInd)->DeviceInfo));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }

        OSPPListDelete(&((*ospvUsageInd)->DeviceInfo));

        while (!OSPPListEmpty(&((*ospvUsageInd)->DestinationAlternate))) {
            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvUsageInd)->DestinationAlternate));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }

        if (OSPPUsageIndHasStatistics(*ospvUsageInd)) {
            OSPPStatsDelete(&((*ospvUsageInd)->Stats));
        }

        if (OSPPUsageIndHasComponentId(*ospvUsageInd)) {
            OSPM_FREE((*ospvUsageInd)->ComponentId);
        }

        if (OSPPUsageIndHasMessageId(*ospvUsageInd)) {
            OSPM_FREE((*ospvUsageInd)->MessageId);
        }

        if (OSPPUsageIndGetDestinationCount(*ospvUsageInd) != OSPC_OSNULL) {
            OSPM_FREE((*ospvUsageInd)->DestinationCount);
        }

        OSPPListDelete(&((*ospvUsageInd)->DestinationAlternate));

        for (cnt = OSPC_SESSIONID_START; cnt < OSPC_SESSIONID_NUMBER; cnt++) {
            if (OSPPUsageIndHasSessionId(*ospvUsageInd, cnt)) {
                OSPPCallIdDelete(&((*ospvUsageInd)->SessionId[cnt]));
            }
        }

        OSPM_FREE(*ospvUsageInd);
        *ospvUsageInd = OSPC_OSNULL;
    }
}

/*
 * OSPPUsageIndSetCallId() - sets the call ID for an usage
 */
void OSPPUsageIndSetCallId(         /* nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* usage indication */
    OSPT_CALL_ID *ospvCallId)       /* call ID */
{
    if (ospvUsageInd != OSPC_OSNULL) {
        if ((ospvCallId) != OSPC_OSNULL) {
            if (ospvUsageInd->CallId != OSPC_OSNULL) {
                OSPPCallIdDelete(&(ospvUsageInd->CallId));
            }
            ospvUsageInd->CallId = OSPPCallIdNew(ospvCallId->Length, ospvCallId->Value);
        }
    }
}

/*
 * OSPPUsageIndHasStatistics() - does usageind have statistics?
 */
OSPTBOOL OSPPUsageIndHasStatistics(
    OSPT_USAGE_IND *ospvUsageInd)
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        if (ospvUsageInd->Stats != OSPC_OSNULL) {
            has = OSPC_TRUE;
        }
    }

    return has;
}

/*
 * OSPPUsageIndSetStatistics() - set values for statistics in usageind
 */
void OSPPUsageIndSetStatistics(
    OSPT_USAGE_IND *ospvUsageInd,   /* In - ptr to usage ind */
    OSPT_STATS *ospvStats)          /* In - ptr to completed stats struct */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvStats != OSPC_OSNULL)) {
        ospvUsageInd->Stats = OSPPStatsNew();
        OSPM_MEMCPY(ospvUsageInd->Stats, ospvStats, sizeof(OSPT_STATS));
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
    int errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *usageindelem = OSPC_OSNULL;
    OSPT_XML_ELEM *usagedetailelem = OSPC_OSNULL;
    OSPT_XML_ELEM *roleinfoelem = OSPC_OSNULL;
    OSPT_XML_ELEM *callpartyelem = OSPC_OSNULL;
    OSPT_XML_ELEM *cdrproxyelem = OSPC_OSNULL;
    OSPT_XML_ELEM *subelem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    OSPTTRXID trxid = 0;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPT_USAGE_IND *usage = OSPC_OSNULL;
    char random[OSPC_MAX_RANDOM];
    OSPTBOOL isbase64 = OSPC_TRUE;
    OSPTTRANS *trans = (OSPTTRANS *)ospvtrans;
    OSPE_MSG_ELEM elemtype;
    OSPE_MSG_ATTR attrtype[OSPC_MAX_ATTR];
    OSPE_ALTINFO attrvalue[OSPC_MAX_ATTR];
    OSPE_TERM_CAUSE tctype;
    int index;
    OSPE_SERVICE svc;
    OSPE_CALL_PARTY party;
    OSPE_PROTOCOL_TYPE prot;
    OSPE_SIP_HEADER header;
    OSPE_NUMBER_FORMAT format;
    OSPE_CODEC_TYPE codec;
    OSPE_SESSION_ID sess;
    OSPT_SDP_FINGERPRINT *fingerprint = OSPC_OSNULL;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvUsageInd == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_NO_USAGEIND;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the "Message" element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* Add the Message attribute */
        usage = (OSPT_USAGE_IND *)OSPPListFirst(ospvUsageInd);
        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID),
            OSPPUsageIndHasMessageId(usage) ? (const char *)(usage->MessageId) : "NULL");
        if (attr == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ATTR;
        } else {
            OSPPXMLElemAddAttr(*ospvElem, attr);
            attr = OSPC_OSNULL;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* random */
        if (OSPPUtilGetRandom(random, 0) > 0) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *)random);
            if (attr == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* Build multiple usage ind if there are more than one */
        for (usage = (OSPT_USAGE_IND *)OSPPListFirst(ospvUsageInd);
            (usage != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR);
            usage = (OSPT_USAGE_IND *)OSPPListNext(ospvUsageInd, usage))
        {
            /* create the usage element */
            usageindelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_USAGEIND), "");
            if (usageindelem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            }

            /* now add the attributes to the parent -- in this case the component id */
            if (errcode == OSPC_ERR_NO_ERROR) {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID),
                    OSPPUsageIndHasComponentId(usage) ? (const char *)(usage->ComponentId) : "NULL");
                if (attr == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ATTR;
                } else {
                    OSPPXMLElemAddAttr(usageindelem, attr);
                    attr = OSPC_OSNULL;
                }
            }

            /* now add the children - start with timestamp */
            if ((errcode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasTimestamp(usage)) {
                errcode = OSPPMsgTimeToElement(OSPPUsageIndGetTimestamp(usage), OSPPMsgElemGetName(OSPC_MELEM_TIMESTAMP), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add role  */
            if ((errcode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasRole(usage)) {
                errcode = OSPPStringToElement(OSPC_MELEM_ROLE, OSPPRoleGetName(OSPPUsageIndGetRole(usage)), 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add role additional info */
            if ((errcode == OSPC_ERR_NO_ERROR) &&
                (((usage->RoleState >= OSPC_RSTATE_START) && (usage->RoleState < OSPC_RSTATE_NUMBER)) ||
                ((usage->RoleFormat >= OSPC_RFORMAT_START) && (usage->RoleFormat < OSPC_RFORMAT_NUMBER)) ||
                ((usage->RoleVendor >= OSPC_RVENDOR_START) && (usage->RoleVendor < OSPC_RVENDOR_NUMBER))))
            {
                roleinfoelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_ROLEINFO), "");
                if (roleinfoelem == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }

            if ((errcode == OSPC_ERR_NO_ERROR) &&
                ((usage->RoleState >= OSPC_RSTATE_START) && (usage->RoleState < OSPC_RSTATE_NUMBER)))
            {
                errcode = OSPPStringToElement(OSPC_MELEM_ROLESTATE, OSPPRoleStateGetName(usage->RoleState), 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(roleinfoelem, subelem);
                    subelem = OSPC_OSNULL;
                } else {
                    OSPPXMLElemDelete(&roleinfoelem);
                    roleinfoelem = OSPC_OSNULL;
                }
            }

            if ((errcode == OSPC_ERR_NO_ERROR) &&
                ((usage->RoleFormat >= OSPC_RFORMAT_START) && (usage->RoleFormat < OSPC_RFORMAT_NUMBER)))
            {
                errcode = OSPPStringToElement(OSPC_MELEM_ROLEFORMAT, OSPPRoleFormatGetName(usage->RoleFormat), 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(roleinfoelem, subelem);
                    subelem = OSPC_OSNULL;
                } else {
                    OSPPXMLElemDelete(&roleinfoelem);
                    roleinfoelem = OSPC_OSNULL;
                }
            }

            if ((errcode == OSPC_ERR_NO_ERROR) &&
                ((usage->RoleVendor >= OSPC_RVENDOR_START) && (usage->RoleVendor < OSPC_RVENDOR_NUMBER)))
            {
                errcode = OSPPStringToElement(OSPC_MELEM_ROLEVENDOR, OSPPRoleVendorGetName(usage->RoleVendor), 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(roleinfoelem, subelem);
                    subelem = OSPC_OSNULL;
                } else {
                    OSPPXMLElemDelete(&roleinfoelem);
                    roleinfoelem = OSPC_OSNULL;
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(usageindelem, roleinfoelem);
                roleinfoelem = OSPC_OSNULL;
            }

            /* add the transaction ID */
            if ((errcode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasTransactionId(usage)) {
                trxid = OSPPUsageIndGetTransactionId(usage);
                errcode = OSPPMsgTXToElement(trxid, OSPPMsgElemGetName(OSPC_MELEM_TRANSID), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the call ID */
            if ((errcode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasCallId(usage)) {
                errcode = OSPPCallIdToElement(OSPPUsageIndGetCallId(usage), &subelem, isbase64);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the source number */
            if (errcode == OSPC_ERR_NO_ERROR) {
                errcode = OSPPCallPartyNumToElement(OSPC_MELEM_SRCINFO,
                    OSPPUsageIndGetSourceNumber(usage),
                    trans->CallingNumberFormat,
                    &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the device info */
            if ((errcode == OSPC_ERR_NO_ERROR) && (usage->DeviceInfo != NULL)) {
                for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(usage->DeviceInfo));
                    altinfo != OSPC_OSNULL;
                    altinfo = (OSPT_ALTINFO *)OSPPListNext(&(usage->DeviceInfo), altinfo))
                {
                    errcode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_DEVICEINFO);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usageindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add the source alternates */
            if ((errcode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasSourceAlt(usage)) {
                for (altinfo = (OSPT_ALTINFO *)OSPPUsageIndFirstSourceAlt(usage);
                    ((altinfo != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR));
                     altinfo = (OSPT_ALTINFO *)OSPPUsageIndNextSourceAlt(usage, altinfo))
                {
                    errcode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_SRCALT);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usageindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add the destination number */
            if (errcode == OSPC_ERR_NO_ERROR) {
                errcode = OSPPCallPartyNumToElement(OSPC_MELEM_DESTINFO,
                    OSPPUsageIndGetDestNumber(usage),
                    trans->CalledNumberFormat,
                    &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the routing number */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->NPRn[0] != '\0')) {
                errcode = OSPPNPRnToElement(trans->NPRn, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the carrier identification code */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->NPCic[0] != '\0')) {
                errcode = OSPPNPCicToElement(trans->NPCic, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the npdi flag */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->NPNpdi == OSPC_TRUE)) {
                errcode = OSPPNPNpdiToElement(OSPC_TRUE, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the operator names */
             for (index = 0; index < OSPC_OPNAME_NUMBER; index++) {
                 if ((errcode == OSPC_ERR_NO_ERROR) && trans->OpName[index][0] != '\0') {
                     errcode = OSPPOperatorNameToElement(index, trans->OpName[index], &subelem);
                     if (errcode == OSPC_ERR_NO_ERROR) {
                         OSPPXMLElemAddChild(usageindelem, subelem);
                         subelem = OSPC_OSNULL;
                     }
                 }
             }

            /* add the destination alternates */
            if ((errcode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasDestinationAlt(usage)) {
                for (altinfo = (OSPT_ALTINFO *)OSPPUsageIndFirstDestinationAlt(usage);
                    ((altinfo != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR));
                    altinfo = (OSPT_ALTINFO *)OSPPUsageIndNextDestinationAlt(usage, altinfo))
                {
                    errcode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_DESTALT);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usageindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add destination count */
            if ((errcode == OSPC_ERR_NO_ERROR) && (OSPPUsageIndGetDestinationCount(usage) != OSPC_OSNULL)) {
                altinfo = OSPPUsageIndGetDestinationCount(usage);
                errcode = OSPPAltInfoToElement(altinfo, &subelem, OSPC_MELEM_DESTALT);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (usage->SetupAttempt >= 0)) {
                OSPPMsgNumToElement(usage->SetupAttempt, OSPPMsgElemGetName(OSPC_MELEM_SETUPATTEMPT), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            // add total setup attempts
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->TotalSetupAttempts >= 0)) {
                OSPPMsgNumToElement(trans->TotalSetupAttempts, OSPPMsgElemGetName(OSPC_MELEM_TOTALSETUPATTEMPTS), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add Pricing Info */
            if ((errcode == OSPC_ERR_NO_ERROR) && (usage->HasPricingInfo == OSPC_TRUE)) {
                errcode = OSPPPricingInfoToElement(&(usage->PricingInfo), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add Service Info */
            if ((errcode == OSPC_ERR_NO_ERROR) && (usage->HasServiceInfo == OSPC_TRUE)) {
                errcode = OSPPServiceTypeToElement(usage->ServiceType, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add conference id if present */
            if ((errcode == OSPC_ERR_NO_ERROR) && (OSPPUsageIndGetHasConfId(usage))) {
                errcode = OSPPAddConfIdToUsageElement(OSPPUsageIndGetConferenceId(usage), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add user-defined info */
            for (index = 0; index < OSPC_MAX_INDEX; index++) {
                if ((errcode == OSPC_ERR_NO_ERROR) && (trans->CustomInfo[index] != OSPC_OSNULL) && (trans->CustomInfo[index][0] != '\0')) {
                    errcode = OSPPCustomInfoToElement(index, trans->CustomInfo[index], &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usageindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* Add call party info */
            for (party = OSPC_CPARTY_START; party < OSPC_CPARTY_NUMBER; party++) {
                OSPT_CALL_PARTY *cparty = &(usage->CallParty[party]);
                if ((errcode == OSPC_ERR_NO_ERROR) &&
                    ((cparty->UserName[0] != '\0') || (cparty->UserId[0] != '\0') || (cparty->UserGroup[0] != '\0')))
                {
                    if (party == OSPC_CPARTY_SOURCE) {
                        elemtype = OSPC_MELEM_CALLINGPARTYINFO;
                    } else if (party == OSPC_CPARTY_DESTINATION) {
                        elemtype = OSPC_MELEM_CALLEDPARTYINFO;
                    } else {
                        continue;
                    }
                    callpartyelem = OSPPXMLElemNew(OSPPMsgElemGetName(elemtype), "");
                    if (callpartyelem == OSPC_OSNULL) {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }
                if ((errcode == OSPC_ERR_NO_ERROR) && (cparty->UserName[0] != '\0')) {
                    errcode = OSPPStringToElement(OSPC_MELEM_USERNAME, cparty->UserName, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(callpartyelem, subelem);
                        subelem = OSPC_OSNULL;
                    } else {
                        OSPPXMLElemDelete(&callpartyelem);
                        callpartyelem = OSPC_OSNULL;
                    }
                }
                if ((errcode == OSPC_ERR_NO_ERROR) && (cparty->UserId[0] != '\0')) {
                    errcode = OSPPStringToElement(OSPC_MELEM_USERID, cparty->UserId, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(callpartyelem, subelem);
                        subelem = OSPC_OSNULL;
                    } else {
                        OSPPXMLElemDelete(&callpartyelem);
                        callpartyelem = OSPC_OSNULL;
                    }
                }
                if ((errcode == OSPC_ERR_NO_ERROR) && (cparty->UserGroup[0] != '\0')) {
                    errcode = OSPPStringToElement(OSPC_MELEM_USERGROUP, cparty->UserGroup, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(callpartyelem, subelem);
                        subelem = OSPC_OSNULL;
                    } else {
                        OSPPXMLElemDelete(&callpartyelem);
                        callpartyelem = OSPC_OSNULL;
                    }
                }
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, callpartyelem);
                    callpartyelem = OSPC_OSNULL;
                }
            }

            /* Transfer ID */
            if ((errcode == OSPC_ERR_NO_ERROR) && (usage->TransferId[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_TRANSFERID, usage->TransferId, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Transfer Status */
            if ((errcode == OSPC_ERR_NO_ERROR) &&
                ((usage->TransferStatus >= OSPC_TSTATUS_START) && (usage->TransferStatus < OSPC_TSTATUS_NUMBER)))
            {
                errcode = OSPPStringToElement(OSPC_MELEM_TRANSFERSTATUS, OSPPTransferStatusGetName(usage->TransferStatus), 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Create usage detail element */
            if (errcode == OSPC_ERR_NO_ERROR) {
                usagedetailelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_USAGEDETAIL), "");
                if (usagedetailelem == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }

            /* Failure reason */
            if (errcode == OSPC_ERR_NO_ERROR) {
                for (tctype = OSPC_TCAUSE_START; tctype < OSPC_TCAUSE_NUMBER; tctype++) {
                    if (OSPPUsageIndHasTermCause(usage, tctype)) {
                        errcode = OSPPTermCauseToElement(tctype,
                            OSPPUsageIndGetTCCode(usage, tctype),
                            OSPPUsageIndGetTCDesc(usage, tctype),
                            &subelem);
                        if (errcode == OSPC_ERR_NO_ERROR) {
                            OSPPXMLElemAddChild(usagedetailelem, subelem);
                            subelem = OSPC_OSNULL;
                        } else {
                            break;
                        }
                    }
                }
            }

            /* Protocol */
            for (prot = OSPC_PROTTYPE_START; prot < OSPC_PROTTYPE_NUMBER; prot++) {
                if ((errcode == OSPC_ERR_NO_ERROR) && (OSPPUsageIndHasProtocol(usage, prot))) {
                    attrtype[0] = OSPC_MATTR_TYPE;
                    switch (prot) {
                    case OSPC_PROTTYPE_SOURCE:
                        attrvalue[0] = OSPC_ALTINFO_SOURCE;
                        break;
                    case OSPC_PROTTYPE_DESTINATION:
                        attrvalue[0] = OSPC_ALTINFO_DESTINATION;
                        break;
                    case OSPC_PROTTYPE_NA:
                        attrvalue[0] = OSPC_ALTINFO_NA;
                        break;
                    default:
                        continue;
                    }
                    errcode = OSPPStringToElement(OSPC_MELEM_PROTOCOL, OSPPDestProtocolGetName(OSPPUsageIndGetProtocol(usage, prot)), 1, attrtype, attrvalue, &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usagedetailelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* Codecs */
            for (svc = OSPC_SERVICE_START; svc < OSPC_SERVICE_NUMBER; svc++) {
                attrtype[0] = OSPC_MATTR_SERVICE;
                switch (svc) {
                case OSPC_SERVICE_VOICE:
                    attrvalue[0] = OSPC_ALTINFO_VOICE;
                    break;
                case OSPC_SERVICE_VIDEO:
                    attrvalue[0] = OSPC_ALTINFO_VIDEO;
                    break;
                default:
                    continue;
                }
                for (codec = OSPC_CODEC_START; codec < OSPC_CODEC_NUMBER; codec++) {
                    attrtype[1] = OSPC_MATTR_TYPE;
                    switch (codec) {
                    case OSPC_CODEC_SOURCE:
                        attrvalue[1] = OSPC_ALTINFO_SOURCE;
                        break;
                    case OSPC_CODEC_DESTINATION:
                        attrvalue[1] = OSPC_ALTINFO_DESTINATION;
                        break;
                    default:
                        continue;
                    }
                    if ((errcode == OSPC_ERR_NO_ERROR) && (OSPPUsageIndHasCodec(usage, svc, codec))) {
                        errcode = OSPPStringToElement(OSPC_MELEM_CODEC, OSPPUsageIndGetCodec(usage, svc, codec), 2, attrtype, attrvalue, &subelem);
                        if (errcode == OSPC_ERR_NO_ERROR) {
                            OSPPXMLElemAddChild(usagedetailelem, subelem);
                            subelem = OSPC_OSNULL;
                        }
                    }
                }
            }

            /* Add Session IDs */
            for (sess = OSPC_SESSIONID_START; sess < OSPC_SESSIONID_NUMBER; sess ++) {
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasSessionId(usage, sess)) {
                    errcode = OSPPSessionIdToElement(OSPPUsageIndGetSessionId(usage, sess), sess, isbase64, &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usagedetailelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* Add Provider Post Dial Delay */
            if ((errcode == OSPC_ERR_NO_ERROR) && (usage->ProviderPDD >= 0)) {
                errcode = OSPPMsgFloatToElement((float)(usage->ProviderPDD) / 1000, OSPPMsgElemGetName(OSPC_MELEM_POSTDIALDELAY), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_DESTINATION));
                    if (attr == OSPC_OSNULL) {
                        OSPPXMLElemDelete(&subelem);
                    } else {
                       OSPPXMLElemAddAttr(subelem, attr);
                       attr = OSPC_OSNULL;
                       OSPPXMLElemAddChild(usagedetailelem, subelem);
                       subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add usage detail (if appropriate) */
            if (errcode == OSPC_ERR_NO_ERROR) {
                if (OSPPUsageIndHasDuration(usage)) {
                    errcode = OSPPUsageToElement(
                        (unsigned)OSPPUsageIndGetDuration(usage),
                        OSPPUsageIndGetStartTime(usage),
                        OSPPUsageIndGetEndTime(usage),
                        OSPPUsageIndGetAlertTime(usage),
                        OSPPUsageIndGetConnectTime(usage),
                        OSPPUsageIndHasPDD(usage),
                        OSPPUsageIndGetPostDialDelay(usage),
                        OSPPUsageIndGetReleaseSource(usage),
                        usagedetailelem);
                }
            }

            /* Statistics */
            if (errcode == OSPC_ERR_NO_ERROR) {
                if (OSPPUsageIndHasStatistics(usage)) {
                    errcode = OSPPStatsToElement(usage->Stats, &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usagedetailelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* Add source audio address */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->SrcAudioAddr[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_SRCAUDIOADDR, trans->SrcAudioAddr, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usagedetailelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add source video address */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->SrcVideoAddr[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_SRCVIDEOADDR, trans->SrcVideoAddr, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usagedetailelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add destination audio address */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (usage->DestAudioAddr[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_DESTAUDIOADDR, usage->DestAudioAddr, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usagedetailelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add destination video address */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (usage->DestVideoAddr[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_DESTVIDEOADDR, usage->DestVideoAddr, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usagedetailelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(usageindelem, usagedetailelem);
                usagedetailelem = OSPC_OSNULL;
            }

            /* now add the transnexus extensions (if available) */
            if (errcode == OSPC_ERR_NO_ERROR) {
                if (OSPPUsageIndHasCustId(usage)) {
                    errcode = OSPPMsgNumToElement(OSPPUsageIndGetCustId(usage),
                        OSPPMsgElemGetName(OSPC_MELEM_CUSTID), &subelem);
                    /*add attribute critical = "False" since not all servers understand */
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
                        if (attr == OSPC_OSNULL) {
                            errcode = OSPC_ERR_XML_NO_ATTR;
                        } else {
                            OSPPXMLElemAddAttr(subelem, attr);
                            attr = OSPC_OSNULL;
                            OSPPXMLElemAddChild(usageindelem, subelem);
                            subelem = OSPC_OSNULL;
                        }
                    }
                    if (OSPPUsageIndHasDeviceId(usage)) {
                        errcode = OSPPMsgNumToElement(OSPPUsageIndGetDeviceId(usage),
                            OSPPMsgElemGetName(OSPC_MELEM_DEVICEID), &subelem);

                        /*add attribute critical = "False" since not all servers understand */
                        if (errcode == OSPC_ERR_NO_ERROR) {
                            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
                            if (attr == OSPC_OSNULL) {
                                errcode = OSPC_ERR_XML_NO_ATTR;
                            } else {
                                OSPPXMLElemAddAttr(subelem, attr);
                                attr = OSPC_OSNULL;
                                OSPPXMLElemAddChild(usageindelem, subelem);
                                subelem = OSPC_OSNULL;
                            }
                        }
                    }
                }
            }

            /* Add source realm info */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->SrcRealm[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_SRCREALM, trans->SrcRealm, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add destination realm info */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->DestRealm[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_DESTREALM, trans->DestRealm, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add SIP headers */
            for (header = OSPC_SIPHEADER_START; header < OSPC_SIPHEADER_NUMBER; header++) {
                for (format = OSPC_NFORMAT_START; format < OSPC_NFORMAT_NUMBER; format++) {
                    if ((errcode == OSPC_ERR_NO_ERROR) && (trans->SipHeader[header][format][0] != '\0')) {
                        errcode = OSPPCallPartyNumToElement(OSPV_MELEM_SIPHEADER[header], trans->SipHeader[header][format], format, &subelem);
                        if (errcode == OSPC_ERR_NO_ERROR) {
                            OSPPXMLElemAddChild(usageindelem, subelem);
                            subelem = OSPC_OSNULL;
                        }
                    }
                }
            }

            /* add diversion device info */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->DivDevInfo[0] != '\0')) {
                attrtype[0] = OSPC_MATTR_TYPE;
                attrvalue[0] = OSPC_ALTINFO_TRANSPORT;
                errcode = OSPPStringToElement(OSPC_MELEM_DIVDEVINFO, trans->DivDevInfo, 1, attrtype, attrvalue, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add application ID */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->ApplicationId[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_APPLID, trans->ApplicationId, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add network translated called number */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->NetworkTranslatedCalled[0] != '\0')) {
                errcode = OSPPCallPartyNumToElement(OSPC_MELEM_NETTRANSCALLED, trans->NetworkTranslatedCalled, trans->NetworkTranslatedCalledFormat, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add source service provider */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->SrcServiceProvider[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_SERVICEPROVIDER, trans->SrcServiceProvider, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add destination service provider */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (usage->DestServiceProvider[0] != '\0')) {
                attrtype[0] = OSPC_MATTR_TYPE;
                attrvalue[0] = OSPC_ALTINFO_DESTINATION;
                errcode = OSPPStringToElement(OSPC_MELEM_SERVICEPROVIDER, usage->DestServiceProvider, 1, attrtype, attrvalue, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add system ID */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->SystemId[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_SYSTEMID, trans->SystemId, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add related reason */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->RelatedReason[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_RELATEDREASON, trans->RelatedReason, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add CDR proxy */
            if ((errcode == OSPC_ERR_NO_ERROR) &&
                ((trans->CDRProxyHost[0] != '\0') || (trans->CDRProxyFolder[0] != '\0') || (trans->CDRProxySubfolder[0] != '\0')))
            {
                cdrproxyelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CDRPROXY), "");
                if (cdrproxyelem == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->CDRProxyHost[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_CDRPROXYHOST, trans->CDRProxyHost, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(cdrproxyelem, subelem);
                    subelem = OSPC_OSNULL;
                } else {
                    OSPPXMLElemDelete(&cdrproxyelem);
                    roleinfoelem = OSPC_OSNULL;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->CDRProxyFolder[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_CDRPROXYFOLDER, trans->CDRProxyFolder, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(cdrproxyelem, subelem);
                    subelem = OSPC_OSNULL;
                } else {
                    OSPPXMLElemDelete(&cdrproxyelem);
                    roleinfoelem = OSPC_OSNULL;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->CDRProxySubfolder[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_CDRPROXYSUBFOLDER, trans->CDRProxySubfolder, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(cdrproxyelem, subelem);
                    subelem = OSPC_OSNULL;
                } else {
                    OSPPXMLElemDelete(&cdrproxyelem);
                    roleinfoelem = OSPC_OSNULL;
                }
            }
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(usageindelem, cdrproxyelem);
                cdrproxyelem = OSPC_OSNULL;
            }

            /* Add Jurisdiction Information Parameter */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->JIP[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_JIP, trans->JIP, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add call type */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (usage->CallType[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_CALLTYPE, usage->CallType, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add call category */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (usage->CallCategory[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_CALLCATEGORY, usage->CallCategory, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add network type */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (usage->NetworkType[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_NETWORKTYPE, usage->NetworkType, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add SIP request Date */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->RequestDate != OSPC_TIMEMIN)) {
                errcode = OSPPMsgTimeToElement(trans->RequestDate, OSPPMsgElemGetName(OSPC_MELEM_REQUESTDATE), &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                /* Add SDP finger print */
                if (OSPPHasFingerprint(trans->SDPFingerprint)) {
                    for (fingerprint = OSPPFirstFingerprint(trans->SDPFingerprint);
                        ((fingerprint != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR));
                        fingerprint = OSPPNextFingerprint(trans->SDPFingerprint, fingerprint))
                    {
                        errcode = OSPPFingerprintToElement(fingerprint, &subelem);
                        if (errcode == OSPC_ERR_NO_ERROR) {
                            OSPPXMLElemAddChild(usageindelem, subelem);
                            subelem = OSPC_OSNULL;
                        }
                    }
                }
            }

            /* Add charging vector */
            if ((errcode == OSPC_ERR_NO_ERROR) &&  (trans->PCVICID[0] != '\0')) {
                attrtype[0] = OSPC_MATTR_TYPE;
                attrvalue[0] = OSPC_ALTINFO_ICID;
                errcode = OSPPStringToElement(OSPC_MELEM_CHARGINGVECTOR, trans->PCVICID, 1, attrtype, attrvalue, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add user rate plan */
            for (index = 0; index < OSPC_CPARTY_NUMBER; index++) {
                attrtype[0] = OSPC_MATTR_TYPE;
                if (trans->UserRatePlan[index][0] != '\0') {
                    if (index == OSPC_CPARTY_SOURCE) {
                        attrvalue[0] = OSPC_ALTINFO_SOURCE;
                    } else {
                        attrvalue[0] = OSPC_ALTINFO_DESTINATION;
                    }
                    errcode = OSPPStringToElement(OSPC_MELEM_USERRATEPLAN, trans->UserRatePlan[index], 1, attrtype, attrvalue, &subelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(usageindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* Add User-Agnet */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->UserAgent[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_USERAGENT, trans->UserAgent, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add STIR info */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->StiAsStatus[0] != '\0')) {
                errcode = OSPPStiAsToElement(trans->StiAsStatus, trans->StiAsAttest, trans->StiAsOrigId, trans->StiAsCpsLatency, trans->StiAsCpsRspCode, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->StiVsStatus[0] != '\0')) {
                errcode = OSPPStiVsToElement(trans->StiVsStatus, trans->StiVsAttest, trans->StiVsOrigId, trans->StiVsCertCached, trans->StiVsCertLatency, trans->StiVsCertUrl, trans->StiVsCpsLatency, trans->StiVsCpsRspCode, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* Add redirect info */
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->OrigRedirectBehalf[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_ORIGREDIRECTBEHALF, trans->OrigRedirectBehalf, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->LastRedirectBehalf[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_LASTREDIRECTBEHALF, trans->LastRedirectBehalf, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->OrigRedirectReason[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_ORIGREDIRECTREASON, trans->OrigRedirectReason, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }
            if ((errcode == OSPC_ERR_NO_ERROR) && (trans->LastRedirectBehalf[0] != '\0')) {
                errcode = OSPPStringToElement(OSPC_MELEM_LASTREDIRECTREASON, trans->LastRedirectReason, 0, OSPC_OSNULL, OSPC_OSNULL, &subelem);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPXMLElemAddChild(usageindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, usageindelem);
                usageindelem = OSPC_OSNULL;
            }
        }   /* end for */
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
        if (usageindelem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&usageindelem);
        }
        if (usagedetailelem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&usagedetailelem);
        }
        if (subelem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&subelem);
        }
        if (attr != OSPC_OSNULL) {
            OSPPXMLAttrDelete(&attr);
        }
    }

    return errcode;
}

/*
 * OSPPUsageIndAddSourceAlt() - add a source alt to list a list
 */
void OSPPUsageIndAddSourceAlt(
    OSPT_USAGE_IND *ospvUsageInd,   /* authorization indication */
    OSPT_ALTINFO *ospvAltInfo)      /* altinfo to add */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvAltInfo != OSPC_OSNULL)) {
        OSPPListAppend(&(ospvUsageInd->SourceAlternate), ospvAltInfo);
    }
}

/*
 * OSPPUsageIndAddDestinationAlt() - add a destination alt to list a list
 */
void OSPPUsageIndAddDestinationAlt(
    OSPT_USAGE_IND *ospvUsageInd,   /* authorization indication */
    OSPT_ALTINFO *ospvAltInfo)      /* altinfo to add */
{
    if ((ospvUsageInd != OSPC_OSNULL) && (ospvAltInfo != OSPC_OSNULL)) {
        OSPPListAppend(&(ospvUsageInd->DestinationAlternate), ospvAltInfo);
    }
}

/*
 * OSPPUsageIndHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPUsageIndHasMessageId(  /* returns non-zero if message id is set */
    OSPT_USAGE_IND *ospvUsageInd)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        return(ospvUsageInd->MessageId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPUsageIndGetMessageId() - returns a new copy of the message id.
 */
const char *OSPPUsageIndGetMessageId(
    OSPT_USAGE_IND *ospvUsageInd)
{
    const char *messagestring = OSPC_OSNULL;
    int len = 0;

    if (OSPPUsageIndHasMessageId(ospvUsageInd)) {
        len = OSPM_STRLEN(ospvUsageInd->MessageId);
        OSPM_MALLOC(messagestring, char, len + 1);
        OSPM_MEMSET(messagestring, 0, len + 1);
        OSPM_MEMCPY(messagestring, ospvUsageInd->MessageId, len);
    }

    return messagestring;
}

void OSPPUsageIndSetDestinationCount(
    OSPT_USAGE_IND *ospvUsageInd,
    unsigned ospvDestinationCount)
{
    char buf[64];

    if (ospvDestinationCount >= 0) {
        sprintf(buf, "%d", ospvDestinationCount);
        ospvUsageInd->DestinationCount = OSPPAltInfoNew(OSPM_STRLEN(buf), buf, OSPC_ALTINFO_DEVICEID);
        ospvUsageInd->SetupAttempt = ospvDestinationCount;
    }
}

OSPT_ALTINFO *OSPPUsageIndGetDestinationCount(
    OSPT_USAGE_IND *ospvUsageInd)
{
    if (ospvUsageInd != OSPC_OSNULL) {
        return ospvUsageInd->DestinationCount;
    } else {
        return OSPC_OSNULL;
    }
}

OSPTBOOL OSPPUsageIndHasProtocol(
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_PROTOCOL_TYPE ospvType)
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) {
        if (OSPM_VALIDATE_PROTTYPE(ospvType) &&
            ((ospvUsageInd->Protocol[ospvType] >= OSPC_PROTNAME_START) && (ospvUsageInd->Protocol[ospvType] < OSPC_PROTNAME_NUMBER)))
        {
            has = OSPC_TRUE;
        }
    }

    return has;
}

OSPE_PROTOCOL_NAME OSPPUsageIndGetProtocol(
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_PROTOCOL_TYPE ospvType)
{
    OSPE_PROTOCOL_NAME protocol = OSPC_PROTNAME_UNKNOWN;

    if ((ospvUsageInd != OSPC_OSNULL) && OSPM_VALIDATE_PROTTYPE(ospvType)) {
        protocol = ospvUsageInd->Protocol[ospvType];
    }

    return protocol;
}

void OSPPUsageIndSetProtocol(
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_PROTOCOL_TYPE ospvType,
    OSPE_PROTOCOL_NAME ospvName)
{
    if ((ospvUsageInd != OSPC_OSNULL) && OSPM_VALIDATE_PROTTYPE(ospvType) && OSPM_VALIDATE_PROTNAME(ospvName)) {
        ospvUsageInd->Protocol[ospvType] = ospvName;
    }
}

OSPTBOOL OSPPUsageIndHasCodec(
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_SERVICE ospvService,
    OSPE_CODEC_TYPE ospvType)
{
    OSPTBOOL has = OSPC_FALSE;

    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvService >= OSPC_SERVICE_START) && (ospvService < OSPC_SERVICE_NUMBER)) &&
        ((ospvType >= OSPC_CODEC_START) && (ospvType < OSPC_CODEC_NUMBER)))
    {
        has = (ospvUsageInd->Codec[ospvService][ospvType][0] != '\0');
    }

    return has;
}

const char *OSPPUsageIndGetCodec(
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_SERVICE ospvService,
    OSPE_CODEC_TYPE ospvType)
{
    const char *ospvCodec = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvService >= OSPC_SERVICE_START) && (ospvService < OSPC_SERVICE_NUMBER)) &&
        ((ospvType >= OSPC_CODEC_START) && (ospvType < OSPC_CODEC_NUMBER)))
    {
        ospvCodec = ospvUsageInd->Codec[ospvService][ospvType];
    }

    return ospvCodec;
}

void OSPPUsageIndSetCodec(
    OSPT_USAGE_IND *ospvUsageInd,
    OSPE_SERVICE ospvService,
    OSPE_CODEC_TYPE ospvType,
    const char *ospvCodec)
{
    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvService >= OSPC_SERVICE_START) && (ospvService < OSPC_SERVICE_NUMBER)) &&
        ((ospvType >= OSPC_CODEC_START) && (ospvType < OSPC_CODEC_NUMBER)) &&
        (ospvCodec != OSPC_OSNULL))
    {
        OSPM_STRNCPY(ospvUsageInd->Codec[ospvService][ospvType], ospvCodec, sizeof(ospvUsageInd->Codec[ospvService][ospvType]));
    }
}

/*
 * OSPPUsageIndHasSessionId() - does an usage indication have session ID?
 */
OSPTBOOL OSPPUsageIndHasSessionId(  /* Returns non-zero if exists */
    OSPT_USAGE_IND *ospvUsageInd,   /* Usage indication */
    OSPE_SESSION_ID ospvType)       /* Session id type */
{
    OSPTBOOL has = OSPC_FALSE;

    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvType >= OSPC_SESSIONID_START) && (ospvType < OSPC_SESSIONID_NUMBER)))
    {
        has = (ospvUsageInd->SessionId[ospvType] != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPUsageIndGetSessionId() - gets session ID for an usage indication
 */
OSPT_CALL_ID *OSPPUsageIndGetSessionId( /* Returns session ID pointer */
    OSPT_USAGE_IND *ospvUsageInd,       /* Usage indication */
    OSPE_SESSION_ID ospvType)           /* Session id type */
{
    OSPT_CALL_ID *ospvSessionId = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvType >= OSPC_SESSIONID_START) && (ospvType < OSPC_SESSIONID_NUMBER)))
    {
        ospvSessionId = ospvUsageInd->SessionId[ospvType];
    }

    return ospvSessionId;
}

/*
 * OSPPUsageIndSetSessionId() - sets session ID for an usage
 */
void OSPPUsageIndSetSessionId(      /* Nothing returned */
    OSPT_USAGE_IND *ospvUsageInd,   /* Usage indication */
    OSPE_SESSION_ID ospvType,       /* Session ID type */
    OSPT_CALL_ID *ospvSessionId)    /* Session ID */
{
    if ((ospvUsageInd != OSPC_OSNULL) &&
        ((ospvType >= OSPC_SESSIONID_START) && (ospvType < OSPC_SESSIONID_NUMBER)) &&
        (ospvSessionId != OSPC_OSNULL))
    {
        if (ospvUsageInd->SessionId[ospvType] != OSPC_OSNULL) {
            OSPPCallIdDelete(&(ospvUsageInd->SessionId[ospvType]));
        }
        ospvUsageInd->SessionId[ospvType] = OSPPCallIdNew(ospvSessionId->Length, ospvSessionId->Value);
    }
}
