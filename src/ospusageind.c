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


/**/
/*-----------------------------------------------------------------------*
 *  OSPPUsageIndHasTimestamp() - is the timestamp set ?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPUsageIndHasTimestamp(
    OSPTUSAGEIND *ospvUsageInd              /* Usage Indication effected */
)
{
    unsigned ospvHasTimestamp = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) 
    {


        ospvHasTimestamp = ((ospvUsageInd)->ospmUsageIndTimestamp);
    }
    return ospvHasTimestamp;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetTimestamp() - set the timestamp
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndSetTimestamp(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTTIME ospvTimestamp          /* timestamp to set to */
)
{
    if (ospvUsageInd != OSPC_OSNULL) 
    {
        if (ospvTimestamp  != 0) 
        {
            ospvUsageInd->ospmUsageIndTimestamp = ospvTimestamp;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetTimestamp() - returns the timestamp for a usage ind
 *-----------------------------------------------------------------------*/
OSPTTIME
    OSPPUsageIndGetTimestamp(
    OSPTUSAGEIND *ospvUsageInd                     /* usage ind */
    )
{
    OSPTTIME ospvTimestamp = 0;

    if (ospvUsageInd != OSPC_OSNULL) 
    {
        ospvTimestamp = ospvUsageInd->ospmUsageIndTimestamp;
    }
    return ospvTimestamp;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasRole() - Does usage indication have role set?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPUsageIndHasRole(
    OSPTUSAGEIND *ospvUsageInd             /* usage indication in question */
)
{
    unsigned ospvHasRole = OSPC_FALSE;
    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasRole = ((ospvUsageInd)->ospmUsageIndHasRole != OSPC_FALSE);
    }
    return(ospvHasRole);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetRole() - returns role for an usage indication
 *-----------------------------------------------------------------------*/
OSPE_MSG_ROLETYPES                         /* returns the role (OGW/TGW) */
    OSPPUsageIndGetRole(
    OSPTUSAGEIND *ospvUsageInd             /* usage indication */
    )
{
    OSPE_MSG_ROLETYPES ospvRole = OSPC_UNDEFINED_ROLE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvRole = (OSPE_MSG_ROLETYPES)ospvUsageInd->ospmUsageIndRole;
    }
    return(ospvRole);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetRole() - sets the role for an usage indication
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetRole(
    OSPTUSAGEIND    *ospvUsageInd,
    unsigned        ospvRole
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        (ospvUsageInd)->ospmUsageIndRole = (ospvRole);
        (ospvUsageInd)->ospmUsageIndHasRole = OSPC_TRUE;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasComponentId() - is the component id set ?
 *-----------------------------------------------------------------------*/
unsigned                    /* returns non-zero if component id is set */
OSPPUsageIndHasComponentId(
    OSPTUSAGEIND *ospvUsageInd
)
{
  return (ospvUsageInd->ospmUsageIndComponentId != OSPC_OSNULL);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetComponentId() - returns a new copy of the component id.
 *-----------------------------------------------------------------------*/
unsigned char  *
OSPPUsageIndGetComponentId(
    OSPTUSAGEIND *ospvUsageInd
)
{
    unsigned char   *componentstring   = OSPC_OSNULL;
    int             len                = 0;

    if (OSPPUsageIndHasComponentId(ospvUsageInd))
    {
        len = OSPM_STRLEN((const char *)ospvUsageInd->ospmUsageIndComponentId);
        OSPM_MALLOC(componentstring, unsigned char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvUsageInd->ospmUsageIndComponentId, len);
    }

    return componentstring;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasTransactionId() - is the transaction id set ?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPUsageIndHasTransactionId(
    OSPTUSAGEIND *ospvUsageInd              /* Usage Indication effected */
)
{
    unsigned ospvHasTransactionId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL) 
    {
        ospvHasTransactionId = ((ospvUsageInd)->ospmUsageIndTransactionId != 0);
    }
    return ospvHasTransactionId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetTransactionId() - set the transaction id
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndSetTransactionId(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTTRXID ospvTransactionId    /* transaction id to set to */
)
{
    if (ospvUsageInd != OSPC_OSNULL) 
    {
        if (ospvTransactionId  != (OSPTTRXID)OSPC_OSNULL) 
        {
            ospvUsageInd->ospmUsageIndTransactionId = ospvTransactionId;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetTransactionId() - returns the trans id for a usage ind
 *-----------------------------------------------------------------------*/
OSPTTRXID
    OSPPUsageIndGetTransactionId(
    OSPTUSAGEIND *ospvUsageInd                     /* usage ind */
    )
{
    OSPTTRXID ospvTransactionId = 0;
    if (ospvUsageInd != OSPC_OSNULL) 
    {
        ospvTransactionId = ospvUsageInd->ospmUsageIndTransactionId;
    }
    return ospvTransactionId;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasCallId() - does an usage indication have a Call ID?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPUsageIndHasCallId(
    OSPTUSAGEIND *ospvUsageInd                  /* usage indication */
)
{
    unsigned ospvHasCallId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasCallId = ((ospvUsageInd)->ospmUsageIndCallId != OSPC_OSNULL);
    }
    return(ospvHasCallId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetCallId() - gets the call ID for an usage indication
 *-----------------------------------------------------------------------*/
OSPTCALLID *                               /* returns call ID pointer */
    OSPPUsageIndGetCallId(
    OSPTUSAGEIND *ospvUsageInd               /* usage indication */
    )
{
    OSPTCALLID *ospvCallId = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvCallId = ((ospvUsageInd)->ospmUsageIndCallId);
    }
    return(ospvCallId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetSourceNumber() - set the source number
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndSetSourceNumber(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    unsigned char *ospvSourceNumber /* source number to set to */
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        if (ospvSourceNumber  != OSPC_OSNULL)
        {
            OSPM_STRNCPY((char *)(ospvUsageInd)->ospmUsageIndSourceNumber, 
                (const char *)(ospvSourceNumber),
                tr_min(OSPC_E164NUMSIZE-1, OSPM_STRLEN((const char *) ospvSourceNumber)+1));
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetSourceNumber() - returns the source number for usage ind
 *-----------------------------------------------------------------------*/
unsigned char *
OSPPUsageIndGetSourceNumber(
    OSPTUSAGEIND *ospvUsageInd                     /* usage ind */
)
{
    unsigned char *ospvSourceNumber = OSPC_OSNULL;
    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvSourceNumber = ospvUsageInd->ospmUsageIndSourceNumber;
    }
    return ospvSourceNumber;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetDestNumber() - set the destination number
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndSetDestNumber(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    unsigned char *ospvDestNumber /* destination number to set to */
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        if (ospvDestNumber != OSPC_OSNULL)
        {
            OSPM_STRNCPY((char *)(ospvUsageInd)->ospmUsageIndDestNumber, 
                (const char *)(ospvDestNumber), 
                tr_min(OSPC_E164NUMSIZE-1, OSPM_STRLEN((const char *) ospvDestNumber)+1));
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetDestNumber() - returns the destination number for a usage ind
 *-----------------------------------------------------------------------*/
unsigned char *
OSPPUsageIndGetDestNumber(
    OSPTUSAGEIND *ospvUsageInd                     /* usage ind */
)
{
    unsigned char *ospvDestNumber = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvDestNumber = ospvUsageInd->ospmUsageIndDestNumber;
    }
    return ospvDestNumber;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasDuration() - is the duration set ?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPUsageIndHasDuration(
    OSPTUSAGEIND *ospvUsageInd              /* Usage Indication effected */
)
{
    unsigned ospvHasDuration = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasDuration = (ospvUsageInd->ospmUsageIndDuration >= 0);
    }
    return ospvHasDuration;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetDuration() - set the duration
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndSetDuration(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    int ospvDuration /* duration to set to */
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        if (ospvDuration  >= 0)
        {
            ospvUsageInd->ospmUsageIndDuration = ospvDuration;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetDuration() - returns the duration for a usage ind
 *-----------------------------------------------------------------------*/
int
OSPPUsageIndGetDuration(
    OSPTUSAGEIND *ospvUsageInd                     /* usage ind */
)
{
    int ospvDuration = 0;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvDuration = ospvUsageInd->ospmUsageIndDuration;
    }
    return ospvDuration;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetIsPDDInfoPresent() - set the IsPDDInfoPResent variable
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndSetIsPDDInfoPresent(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    unsigned IsPDDInfoPresent /* duration to set to */
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvUsageInd->ospvUsageIndIsPDDInfoPresent = IsPDDInfoPresent;
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetIsPDDInfoPresent() - gets the IsPDDInfoPResent variable.
 *-----------------------------------------------------------------------*/
int
OSPPUsageIndGetIsPDDInfoPresent(
    OSPTUSAGEIND *ospvUsageInd                     /* usage ind */
)
{
    unsigned ospvIsPDDInfoPresent = 0;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvIsPDDInfoPresent = ospvUsageInd->ospvUsageIndIsPDDInfoPresent;
    }
    return ospvIsPDDInfoPresent;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetPostDialDelay() - set the PDD
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndSetPostDialDelay(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    int ospvPostDialDelay /* PDD to set to */
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        if (ospvPostDialDelay  >= 0)
        {
            ospvUsageInd->ospmUsageIndPostDialDelay = ospvPostDialDelay;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetPostDialDelay() - returns the PDD for a usage ind
 *-----------------------------------------------------------------------*/
int
OSPPUsageIndGetPostDialDelay(
    OSPTUSAGEIND *ospvUsageInd                     /* usage ind */
)
{
    int ospvPostDialDelay = 0;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvPostDialDelay = ospvUsageInd->ospmUsageIndPostDialDelay;
    }
    return ospvPostDialDelay;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasTNCustId() - Does usage have a TransNexus Customer Id?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPUsageIndHasTNCustId(
    OSPTUSAGEIND *ospvUsageInd               /* usage in question */
)
{
    unsigned ospvHasTNCustId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasTNCustId = ((ospvUsageInd)->ospmUsageIndTNCustId != 0L);
    }
    return(ospvHasTNCustId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetTNCustId() - Set TransNexus Customer Id
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetTNCustId(
    OSPTUSAGEIND   *ospvUsageInd,
    unsigned long ospvTNCustId
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        (ospvUsageInd)->ospmUsageIndTNCustId = (ospvTNCustId);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetTNCustId() - returns TN Customer Id for an usage request
 *-----------------------------------------------------------------------*/
unsigned long                              /* returns the time value */
    OSPPUsageIndGetTNCustId(
    OSPTUSAGEIND *ospvUsageInd               /* usage request */
    )
{
    unsigned long ospvTNCustId = 0L;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvTNCustId = (ospvUsageInd)->ospmUsageIndTNCustId;
    }
    return(ospvTNCustId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasTNDeviceId() - Does request have a TransNexus Device Id?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPUsageIndHasTNDeviceId(
    OSPTUSAGEIND *ospvUsageInd               /* usage request in question */
)
{
    unsigned ospvHasTNDeviceId = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasTNDeviceId = ((ospvUsageInd)->ospmUsageIndTNDeviceId != 0L);
    }
    return(ospvHasTNDeviceId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetTNDeviceId() - Set TransNexus Device Id
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetTNDeviceId(
    OSPTUSAGEIND    *ospvUsageInd,
    unsigned long  ospvTNDeviceId
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        (ospvUsageInd)->ospmUsageIndTNDeviceId = (ospvTNDeviceId);
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetTNDeviceId() - returns TN Device Id for an usage request
 *-----------------------------------------------------------------------*/
unsigned long                              /* returns the time value */
    OSPPUsageIndGetTNDeviceId(
    OSPTUSAGEIND *ospvUsageInd               /* usage request */
    )
{
    unsigned long ospvTNDeviceId = 0L;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvTNDeviceId = (ospvUsageInd)->ospmUsageIndTNDeviceId;
    }
    return(ospvTNDeviceId);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasTNFailReason() - Does request have a TransNexus Fail Rsn
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if time */
OSPPUsageIndHasTNFailReason(
    OSPTUSAGEIND *ospvUsageInd               /* usage request in question */
)
{
    unsigned ospvHasTNFailReason = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasTNFailReason = ((ospvUsageInd)->ospmUsageIndTNFailReasonInd);
    }
    return(ospvHasTNFailReason);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetTNFailReason() - Set TransNexus Fail Reason
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetTNFailReason(
    OSPTUSAGEIND *ospvUsageInd,
    unsigned     ospvTNFailReason
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        (ospvUsageInd)->ospmUsageIndTNFailReason    = (ospvTNFailReason);
        (ospvUsageInd)->ospmUsageIndTNFailReasonInd = 1;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetTNFailReason() - returns TN Fail Rsn for an usage request
 *-----------------------------------------------------------------------*/
unsigned                                 /* returns the fail reason value */
    OSPPUsageIndGetTNFailReason(
    OSPTUSAGEIND *ospvUsageInd               /* usage request */
    )
{
    unsigned ospvTNFailReason = 0;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvTNFailReason = (ospvUsageInd)->ospmUsageIndTNFailReason;
    }
    return(ospvTNFailReason);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasSourceAlt() - does an usage indication have a 
 * Source Alternate?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPUsageIndHasSourceAlt(
    OSPTUSAGEIND *ospvUsageInd             /* usage indication */
)
{
    unsigned ospvHasSourceAlt = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasSourceAlt = (OSPPUsageIndFirstSourceAlt(ospvUsageInd) != OSPC_OSNULL);
    }
    return(ospvHasSourceAlt);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndFirstSourceAlt() - gets the First Source alternate for an 
 * usage indication
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                              /* returns alt info pointer */
    OSPPUsageIndFirstSourceAlt(
    OSPTUSAGEIND *ospvUsageInd             /* usage indication */
    )
{
    OSPTALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPTALTINFO *)OSPPListFirst(&((ospvUsageInd)->ospmUsageIndSourceAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndNextSourceAlt() - gets the next source alternate for an 
 * usage indication
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                               /* returns alt info pointer */
    OSPPUsageIndNextSourceAlt(
    OSPTUSAGEIND *ospvUsageInd,             /* usage indication */
    OSPTALTINFO  *ospvAltInfo
    )
{
    OSPTALTINFO *altinfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        altinfo = 
            (OSPTALTINFO *)OSPPListNext(&((ospvUsageInd)->ospmUsageIndSourceAlternate), 
            ospvAltInfo);
    }
    return(altinfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasDestinationAlt() - does an usage indication have a 
 * Destination Alternate?
 *-----------------------------------------------------------------------*/
unsigned                                   /* returns non-zero if exists */
OSPPUsageIndHasDestinationAlt(
    OSPTUSAGEIND *ospvUsageInd             /* usage indication */
)
{
    unsigned ospvHasDestinationAlt = OSPC_FALSE;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvHasDestinationAlt = (OSPPUsageIndFirstDestinationAlt(ospvUsageInd) != OSPC_OSNULL);
    }
    return(ospvHasDestinationAlt);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndFirstDestinationAlt() - gets the First Destination alternate for an 
 * usage indication
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                              /* returns alt info pointer */
    OSPPUsageIndFirstDestinationAlt(
    OSPTUSAGEIND *ospvUsageInd             /* usage indication */
    )
{
    OSPTALTINFO *ospvAltInfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvAltInfo = 
            (OSPTALTINFO *)OSPPListFirst(&((ospvUsageInd)->ospmUsageIndDestinationAlternate));
    }
    return(ospvAltInfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndNextDestinationAlt() - gets the next Destination alternate for an 
 * usage indication
 *-----------------------------------------------------------------------*/
OSPTALTINFO *                               /* returns alt info pointer */
    OSPPUsageIndNextDestinationAlt(
    OSPTUSAGEIND *ospvUsageInd,             /* usage indication */
    OSPTALTINFO  *ospvAltInfo
    )
{
    OSPTALTINFO *altinfo = OSPC_OSNULL;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        altinfo = 
            (OSPTALTINFO *)OSPPListNext(&((ospvUsageInd)->ospmUsageIndDestinationAlternate), 
            ospvAltInfo);
    }
    return(altinfo);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetDestinationAltSize() - gets the Destination alternate size 
 * for an altinfo
 *-----------------------------------------------------------------------*/
unsigned                                    /* returns alt info size */
    OSPPUsageIndGetDestinationAltSize(
    OSPTALTINFO *ospvAltInfo                /* Alt info ptr */
    )
{
    unsigned ospvAltInfoSize = 0;

    if (ospvAltInfo != OSPC_OSNULL)
    {
        ospvAltInfoSize = OSPPAltInfoGetSize(ospvAltInfo);
    }
    return(ospvAltInfoSize);
}



/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetStartTime() - Set Call Start Time
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetStartTime(
    OSPTUSAGEIND *ospvUsageInd,
    OSPTTIME      ospvStartTime
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        (ospvUsageInd)->ospmUsageIndStartTime = ospvStartTime;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageGetGetStartTime() - Set Call Start Time
 *-----------------------------------------------------------------------*/
OSPTTIME                                    /* call start time */
OSPPUsageIndGetStartTime(
    OSPTUSAGEIND *ospvUsageInd
)
{
		OSPTTIME ospvStartTime = 0;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvStartTime = (ospvUsageInd)->ospmUsageIndStartTime;
    }

		return(ospvStartTime);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetEndTime() - Set Call End Time
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetEndTime(
    OSPTUSAGEIND *ospvUsageInd,
    OSPTTIME      ospvEndTime
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        (ospvUsageInd)->ospmUsageIndEndTime = ospvEndTime;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageGetGetEndTime() - Get Call End Time
 *-----------------------------------------------------------------------*/
OSPTTIME                                    /* call end time */
OSPPUsageIndGetEndTime(
    OSPTUSAGEIND *ospvUsageInd
)
{
		OSPTTIME ospvEndTime = 0;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvEndTime = (ospvUsageInd)->ospmUsageIndEndTime;
    }

		return(ospvEndTime);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetAlertTime() - Set Call Alert Time
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetAlertTime(
    OSPTUSAGEIND *ospvUsageInd,
    OSPTTIME      ospvAlertTime
)
{
    if (ospvUsageInd != OSPC_OSNULL)
    {
        (ospvUsageInd)->ospmUsageIndAlertTime = ospvAlertTime;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageGetGetAlertTime() - Get Call Alert Time
 *-----------------------------------------------------------------------*/
OSPTTIME                                    /* call alert time */
OSPPUsageIndGetAlertTime(
    OSPTUSAGEIND *ospvUsageInd
)
{
		OSPTTIME ospvAlertTime = 0;

    if (ospvUsageInd != OSPC_OSNULL)
    {
        ospvAlertTime = (ospvUsageInd)->ospmUsageIndAlertTime;
    }

		return(ospvAlertTime);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetComponentId() - creates space and copies in the string.
 *-----------------------------------------------------------------------*/
void      
OSPPUsageIndSetComponentId(
    OSPTUSAGEIND  *ospvUsageInd,    /* In - pointer to Usage Indication struct */
    unsigned char *ospvComponentId  /* In - pointer to component id string */
    )
{
    int len = OSPM_STRLEN((const char *)ospvComponentId);

    if(ospvUsageInd != OSPC_OSNULL)
    {
        if(ospvUsageInd->ospmUsageIndComponentId != OSPC_OSNULL)
        {
            OSPM_FREE(ospvUsageInd->ospmUsageIndComponentId);    
        }

        OSPM_MALLOC(ospvUsageInd->ospmUsageIndComponentId, unsigned char, len + 1);
        OSPM_MEMSET(ospvUsageInd->ospmUsageIndComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageInd->ospmUsageIndComponentId, ospvComponentId, len);
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndMoveDeviceinfo() - move the device info list from list
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndMoveDeviceInfo(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST     *ospvList          /* list to move */
    )
{
    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvList != OSPC_OSNULL))
    {

        OSPPListNew(&(ospvUsageInd->ospmUsageIndDeviceInfo));

        OSPPListMove(&(ospvUsageInd->ospmUsageIndDeviceInfo),
            ospvList);
    }

    return;
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndMoveSourceAlt() - move the source alt list from list
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndMoveSourceAlt(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST     *ospvList          /* list to move */
    )
{
    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvList != OSPC_OSNULL))
    {

        OSPPListNew(&(ospvUsageInd->ospmUsageIndSourceAlternate));

        OSPPListMove(&(ospvUsageInd->ospmUsageIndSourceAlternate),
            ospvList);
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndCopyDeviceInfo() - Copy the device info list
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndCopyDeviceInfo(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST     *ospvList          /* list to move */
    )
{
    OSPTALTINFO *altinfo1    = OSPC_OSNULL,
        *altinfo2    = OSPC_OSNULL;

    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvList != OSPC_OSNULL))
    {

        OSPPListNew(&(ospvUsageInd->ospmUsageIndDeviceInfo));


        for(altinfo1 = (OSPTALTINFO *)OSPPListFirst(ospvList);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPTALTINFO *)OSPPListNext(ospvList, altinfo1))
        {

            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1),
                OSPPAltInfoGetValue(altinfo1),
                OSPPAltInfoGetType(altinfo1));
            if(altinfo2 != OSPC_OSNULL)
            {
                OSPPListAppend(&(ospvUsageInd)->ospmUsageIndDeviceInfo, altinfo2);
            }
            altinfo2 = OSPC_OSNULL;
        }
    }

    if(altinfo2 != OSPC_OSNULL)
    {
        OSPPAltInfoDelete(&altinfo2);
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndMergeSourceAlt() - Merges the source alt list
 * The list (ospmAuthReqSourceAlternate) could contain - NetworkId, SrcAddr,
 * or the Subscriber Info. We need to copy everything from list1, except
 * the SrcAddr. The 2nd list contains the Updated SrcAddr that we just 
 * append to the list.
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndMergeSourceAlt(
    OSPTUSAGEIND *ospvUsageInd,     
    OSPTLIST     *ospvList1,          
    OSPTLIST     *ospvList2          
    )
{
    OSPTALTINFO *altinfo1    = OSPC_OSNULL,
        *altinfo2    = OSPC_OSNULL;

    if ((ospvUsageInd != OSPC_OSNULL) && ((ospvList1 != OSPC_OSNULL) || (ospvList2 != OSPC_OSNULL)))
    {
        OSPPListNew(&(ospvUsageInd->ospmUsageIndSourceAlternate));
    }

    /*
     * Copy the node from List 2.
     */
    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvList2 != OSPC_OSNULL))
    {
        for(altinfo1 = (OSPTALTINFO *)OSPPListFirst(ospvList2);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPTALTINFO *)OSPPListNext(ospvList2, altinfo1))
        {
            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1),
                    OSPPAltInfoGetValue(altinfo1),
                    OSPPAltInfoGetType(altinfo1));
            if(altinfo2 != OSPC_OSNULL)
            {
               OSPPListAppend(&(ospvUsageInd)->ospmUsageIndSourceAlternate, altinfo2);
            }
            altinfo2 = OSPC_OSNULL;
        }
    }

    if(altinfo2 != OSPC_OSNULL)
    {
        OSPPAltInfoDelete(&altinfo2);
    }

    /*
     * Now copy the nodes for Network/Subscriber Info
     */
    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvList1 != OSPC_OSNULL))
    {
        for(altinfo1 = (OSPTALTINFO *)OSPPListFirst(ospvList1);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPTALTINFO *)OSPPListNext(ospvList1, altinfo1))
        {
            if (OSPPAltInfoGetType(altinfo1) != ospeTransport)
            {
                altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1),
                    OSPPAltInfoGetValue(altinfo1),
                    OSPPAltInfoGetType(altinfo1));
                if(altinfo2 != OSPC_OSNULL)
                {
                    OSPPListAppend(&(ospvUsageInd)->ospmUsageIndSourceAlternate, altinfo2);
                }
                altinfo2 = OSPC_OSNULL;
            }
        }
    }

    if(altinfo2 != OSPC_OSNULL)
    {
        OSPPAltInfoDelete(&altinfo2);
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndCopySourceAlt() - Copy the source alt list
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndCopySourceAlt(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST     *ospvList          /* list to move */
    )
{
    OSPTALTINFO *altinfo1    = OSPC_OSNULL,
        *altinfo2    = OSPC_OSNULL;

    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvList != OSPC_OSNULL))
    {

        OSPPListNew(&(ospvUsageInd->ospmUsageIndSourceAlternate));


        for(altinfo1 = (OSPTALTINFO *)OSPPListFirst(ospvList);
            altinfo1 != OSPC_OSNULL;
            altinfo1 = (OSPTALTINFO *)OSPPListNext(ospvList, altinfo1))
        {

            altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo1),
                OSPPAltInfoGetValue(altinfo1),
                OSPPAltInfoGetType(altinfo1));
            if(altinfo2 != OSPC_OSNULL)
            {
                OSPPListAppend(&(ospvUsageInd)->ospmUsageIndSourceAlternate, altinfo2);
            }
            altinfo2 = OSPC_OSNULL;
        }
    }

    if(altinfo2 != OSPC_OSNULL)
    {
        OSPPAltInfoDelete(&altinfo2);
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndMoveDestinationAlt() - move the destination alt list from 
 * a list
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageIndMoveDestinationAlt(
    OSPTUSAGEIND *ospvUsageInd,     /* usage indication to set */
    OSPTLIST     *ospvList          /* list to move */
    )
{
    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvList != OSPC_OSNULL))
    {

        OSPPListNew(&(ospvUsageInd->ospmUsageIndDestinationAlternate));

        OSPPListMove(&(ospvUsageInd->ospmUsageIndDestinationAlternate),
            ospvList);
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndNew() - creates a new (empty) usage indication
 *-----------------------------------------------------------------------*/
OSPTUSAGEIND *                                 /* returns pointer or NULL */
    OSPPUsageIndNew()
{
    OSPTUSAGEIND *ospvUsageInd = OSPC_OSNULL;

    OSPM_MALLOC(ospvUsageInd, OSPTUSAGEIND,sizeof(OSPTUSAGEIND));
    if (ospvUsageInd != OSPC_OSNULL)
    {
        OSPM_MEMSET(ospvUsageInd, 0, sizeof(OSPTUSAGEIND));

        OSPPListLinkNew (&(ospvUsageInd->ospmUsageIndLink));
        ospvUsageInd->ospmUsageIndTimestamp = (OSPTTIME)0;
        ospvUsageInd->ospmUsageIndStartTime = (OSPTTIME)0;
        ospvUsageInd->ospmUsageIndTransactionId = (OSPTTRXID)OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndCallId = (OSPTCALLID *)OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndSourceNumber[0] = '\0';
        ospvUsageInd->ospmUsageIndDestNumber[0] = '\0';
        ospvUsageInd->ospmUsageIndDuration   = -1;
        ospvUsageInd->ospmUsageIndTNCustId   = 0L;
        ospvUsageInd->ospmUsageIndTNDeviceId = 0L;
        ospvUsageInd->ospmUsageIndTNFailReasonInd = 0;
        OSPPListNew (&(ospvUsageInd->ospmUsageIndSourceAlternate));
        OSPPListNew(&(ospvUsageInd->ospmUsageIndDestinationAlternate));
        OSPPListNew(&(ospvUsageInd->ospmUsageIndDeviceInfo));
        ospvUsageInd->ospmUsageIndTNStats = OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndComponentId = OSPC_OSNULL;
        ospvUsageInd->ospmUsageIndMessageId = OSPC_OSNULL;
    }

    return ospvUsageInd;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndDelete() - deletes a usage indication
 *-----------------------------------------------------------------------*/
void
OSPPUsageIndDelete(OSPTUSAGEIND **ospvUsageInd)
{
    OSPTALTINFO     *altinfo    = OSPC_OSNULL;

    if (*ospvUsageInd)
    {
        if (OSPPUsageIndHasCallId(*ospvUsageInd))
        {
            OSPPCallIdDelete(&((*ospvUsageInd)->ospmUsageIndCallId));
        }

        if(OSPPUsageIndHasComponentId(*ospvUsageInd))
        {
            OSPM_FREE((*ospvUsageInd)->ospmUsageIndComponentId);
        }

        while(!OSPPListEmpty(&((*ospvUsageInd)->ospmUsageIndSourceAlternate)))
        {
            altinfo = (OSPTALTINFO *)OSPPListRemove(&((*ospvUsageInd)->ospmUsageIndSourceAlternate));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }  

        OSPPListDelete(&((*ospvUsageInd)->ospmUsageIndSourceAlternate));

        while(!OSPPListEmpty(&((*ospvUsageInd)->ospmUsageIndDeviceInfo)))
        {
            altinfo = (OSPTALTINFO *)OSPPListRemove(&((*ospvUsageInd)->ospmUsageIndDeviceInfo));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }  

        OSPPListDelete(&((*ospvUsageInd)->ospmUsageIndDeviceInfo));

        while(!OSPPListEmpty(&((*ospvUsageInd)->ospmUsageIndDestinationAlternate)))
        {
            altinfo = (OSPTALTINFO *)OSPPListRemove(&((*ospvUsageInd)->ospmUsageIndDestinationAlternate));
            OSPM_FREE(altinfo);
            altinfo = OSPC_OSNULL;
        }

        if(OSPPUsageIndHasTNStatistics(*ospvUsageInd))
        {
            OSPPStatisticsDelete(&((*ospvUsageInd)->ospmUsageIndTNStats));
        }

        if(OSPPUsageIndHasComponentId(*ospvUsageInd))
        {
            OSPM_FREE((*ospvUsageInd)->ospmUsageIndComponentId);
        }

        if(OSPPUsageIndHasMessageId(*ospvUsageInd))
        {
            OSPM_FREE((*ospvUsageInd)->ospmUsageIndMessageId);
        }

        OSPPListDelete(&((*ospvUsageInd)->ospmUsageIndDestinationAlternate));
        OSPM_FREE(*ospvUsageInd);
        *ospvUsageInd = OSPC_OSNULL;
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetCallId() - sets the call ID for an usage
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPUsageIndSetCallId(
    OSPTUSAGEIND   *ospvUsageInd,            /* usage indication */
    OSPTCALLID    *ospvCallId              /* call ID */
)
{
    if (ospvUsageInd   != OSPC_OSNULL) 
    {
        if ((ospvCallId) != OSPC_OSNULL)
        {
            if ((ospvUsageInd)->ospmUsageIndCallId != OSPC_OSNULL)
            {
                OSPPCallIdDelete(&((ospvUsageInd)->ospmUsageIndCallId));
            }
            (ospvUsageInd)->ospmUsageIndCallId = 
                OSPPCallIdNew((ospvCallId)->ospmCallIdLen, (ospvCallId)->ospmCallIdVal); 
        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasTNStatistics() - does usageind have statistics?
 *-----------------------------------------------------------------------*/
OSPTBOOL       
    OSPPUsageIndHasTNStatistics(
    OSPTUSAGEIND *ospvUsageInd
    )
{
    OSPTBOOL hasstats = OSPC_FALSE;

    if(ospvUsageInd != OSPC_OSNULL)
    {
        if(ospvUsageInd->ospmUsageIndTNStats != OSPC_OSNULL)
        {
            hasstats = OSPC_TRUE;
        }
    }

    return hasstats;
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndSetTNStatistics() - set values for statistics in usageind
 *-----------------------------------------------------------------------*/
void
OSPPUsageIndSetTNStatistics(
    OSPTUSAGEIND    *ospvUsageInd,      /* In - ptr to usage ind */
    OSPTSTATISTICS  *ospvStatistics     /* In - ptr to completed stats struct*/
)
{
    if((ospvUsageInd != OSPC_OSNULL) && (ospvStatistics != OSPC_OSNULL))
    {
        ospvUsageInd->ospmUsageIndTNStats = OSPPStatisticsNew();
        OSPM_MEMCPY(ospvUsageInd->ospmUsageIndTNStats, 
            ospvStatistics,
            sizeof(OSPTSTATISTICS));

    }

    return;

}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndToElement() - create an XML element from a usage ind
 *-----------------------------------------------------------------------*/
int                                 /* returns error code */
OSPPUsageIndToElement(
    OSPTLIST         *ospvUsageInd, /* usage ind list */
    OSPTXMLELEM      **ospvElem,     /* where to put XML element pointer */
    void        *ospvtrans
)
{
    int             ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM     *usgindelem   = OSPC_OSNULL,
        *subelem    = OSPC_OSNULL;
    OSPTXMLATTR     *attr       = OSPC_OSNULL;
    int             len         = 0;
    OSPTTRXID       trxid       = 0;
    OSPTALTINFO     *altinfo    = OSPC_OSNULL;
    OSPTUSAGEIND    *usage      = OSPC_OSNULL;
    char            random[OSPC_MAX_RANDOM];
    OSPTBOOL        isbase64    = OSPC_TRUE;
    OSPTTRANS       *trans = (OSPTTRANS *)ospvtrans;

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvUsageInd == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_USAGEIND;
    }

    if(ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* create the "Message" element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemMessage), "");

        if(*ospvElem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
        else
        {
            /* Add the Message attribute */
            usage = (OSPTUSAGEIND *)OSPPListFirst(ospvUsageInd);
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrMessageId), 
                (OSPPUsageIndHasMessageId(usage))?(const unsigned char *)(usage->ospmUsageIndMessageId): (const unsigned char *)"NULL");

            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }

            /* random */
            if((OSPPUtilGetRandom(random, 0) > 0) &&
                (ospvErrCode == OSPC_ERR_NO_ERROR))
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrRandom),
                    (const unsigned char *)random);

                if (attr != OSPC_OSNULL) 
                {
                    OSPPXMLElemAddAttr(*ospvElem, attr);
                    attr = OSPC_OSNULL;
                }
                else
                {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
        }
    }

    if(ospvErrCode == OSPC_ERR_NO_ERROR)
    {

        /* Build multiple usage ind if there are more than one */
        for(usage = (OSPTUSAGEIND *)OSPPListFirst(ospvUsageInd);
            (ospvErrCode == OSPC_ERR_NO_ERROR) &&
            (usage != OSPC_OSNULL);
            usage = (OSPTUSAGEIND *)OSPPListNext(ospvUsageInd, usage))
        {

            /* create the usage element */
            usgindelem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemUsageInd), "");
            if (usgindelem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }

            /* now add the attributes to the parent -- in this case the component id
             */
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrComponentId), 
                    (OSPPUsageIndHasComponentId(usage))?(const unsigned char *)(usage->ospmUsageIndComponentId): (const unsigned char *)"NULL");
                if (attr == OSPC_OSNULL)
                {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddAttr(usgindelem, attr);
                attr = OSPC_OSNULL;
            }

            /* now add the children - start with timestamp */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasTimestamp(usage))
            {
                ospvErrCode = OSPPMsgTimeToElement(OSPPUsageIndGetTimestamp(usage),
                    (const unsigned char *)OSPPMsgGetElemName(ospeElemTimestamp), &subelem);
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(usgindelem, subelem);
                subelem = OSPC_OSNULL;
            }

            /* add role  */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasRole(usage))
            {
                ospvErrCode = OSPPMsgRoleToElement(OSPPUsageIndGetRole(usage),
                    (const unsigned char *) OSPPMsgGetElemName(ospeElemRole), &subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the transaction ID */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasTransactionId(usage))
            {
                len = sizeof(OSPTTRXID);
                trxid = OSPPUsageIndGetTransactionId(usage);
                ospvErrCode = OSPPMsgTXToElement(trxid, 
                    (const unsigned char *)OSPPMsgGetElemName(ospeElemTransId), &subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the call ID */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasCallId(usage))
            {
                ospvErrCode = OSPPCallIdToElement(OSPPUsageIndGetCallId(usage),
                    &subelem, isbase64);

                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* add the source number */
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                subelem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemSrcInfo),
                    (const char *)OSPPUsageIndGetSourceNumber(usage));
                if (subelem == OSPC_OSNULL)
                {
                    ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                if (trans->CallingNumberFormat == OSPC_E164)
                {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType), 
                        (const unsigned char *)"e164");
                }
                else if (trans->CallingNumberFormat == OSPC_SIP)
                {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType), 
                        (const unsigned char *)"sip");
                }
                else if (trans->CallingNumberFormat == OSPC_URL)
                {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType), 
                        (const unsigned char *)"url");
                }

                if (attr == OSPC_OSNULL)
                {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddAttr(subelem, attr);
                attr = OSPC_OSNULL;
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(usgindelem, subelem);
                subelem = OSPC_OSNULL;
            }

            /* add the device info */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                  (usage->ospmUsageIndDeviceInfo != NULL))
            {
                for(altinfo = (OSPTALTINFO *)OSPPListFirst( &(usage->ospmUsageIndDeviceInfo));
                           altinfo!= (OSPTALTINFO *)OSPC_OSNULL;
                           altinfo = (OSPTALTINFO *)OSPPListNext( &(usage->ospmUsageIndDeviceInfo), altinfo))
                {
                     ospvErrCode = OSPPAltInfoToElement(altinfo, &subelem, ospeElemDeviceInfo);
                     if (ospvErrCode == OSPC_ERR_NO_ERROR)
                     {
                         OSPPXMLElemAddChild(usgindelem, subelem);
                         subelem = OSPC_OSNULL;
                     }
                }
            }

            /* add the source alternates */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && 
                OSPPUsageIndHasSourceAlt(usage))
            {
                for (altinfo = (OSPTALTINFO *)OSPPUsageIndFirstSourceAlt(usage);
                    ((altinfo != (OSPTALTINFO *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                    altinfo = (OSPTALTINFO *)OSPPUsageIndNextSourceAlt(usage, altinfo))
                {
                    ospvErrCode = OSPPAltInfoToElement(altinfo, &subelem, ospeElemSrcAlt);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add the destination number */
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                subelem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemDestInfo),
                    (const char *)OSPPUsageIndGetDestNumber(usage));
                if (subelem == OSPC_OSNULL)
                {
                    ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                if (trans->CalledNumberFormat == OSPC_E164)
                {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType), 
                        (const unsigned char *)"e164");
                }
                else if (trans->CalledNumberFormat == OSPC_SIP)
                {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType), 
                        (const unsigned char *)"sip");
                }
                else if (trans->CalledNumberFormat == OSPC_URL)
                {
                    attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrType), 
                        (const unsigned char *)"url");
                }

                if (attr == OSPC_OSNULL)
                {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                }
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddAttr(subelem, attr);
                attr = OSPC_OSNULL;
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(usgindelem, subelem);
                subelem = OSPC_OSNULL;
            }

            /* add the destination alternates */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && 
                OSPPUsageIndHasDestinationAlt(usage))
            {
                for (altinfo = (OSPTALTINFO *)OSPPUsageIndFirstDestinationAlt(usage);
                    ((altinfo != (OSPTALTINFO *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
                    altinfo = (OSPTALTINFO *)OSPPUsageIndNextDestinationAlt(usage, altinfo))
                {
                    ospvErrCode = OSPPAltInfoToElement(altinfo, &subelem, ospeElemDestAlt);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* add usage detail (if appropriate) */
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) && OSPPUsageIndHasDuration(usage))
            {
                ospvErrCode = OSPPUsageToElement(
                    (unsigned)OSPPUsageIndGetDuration(usage),
                    OSPPUsageIndGetStartTime(usage),OSPPUsageIndGetEndTime(usage),
                    OSPPUsageIndGetAlertTime(usage),OSPPUsageIndGetIsPDDInfoPresent(usage),
                    OSPPUsageIndGetPostDialDelay(usage),&subelem);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPXMLElemAddChild(usgindelem, subelem);
                    subelem = OSPC_OSNULL;
                }
            }

            /* now add the transnexus extensions (if available) */
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                if (OSPPUsageIndHasTNCustId(usage))
                {
                    ospvErrCode = OSPPMsgNumToElement( 
                        OSPPUsageIndGetTNCustId(usage),
                        (const unsigned char *)OSPPMsgGetElemName(ospeElemTNCustId),
                        &subelem);
             /*add attribute critical = "False" since not all servers understand */
                    if (ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
			attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrCritical),
                    (const unsigned char *)"False");
			if (attr != OSPC_OSNULL)
			{
			    OSPPXMLElemAddAttr(subelem, attr);
			    attr = OSPC_OSNULL;
			}
			else
			{
			    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
			}
		    
		
			OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                     }		
                   if (OSPPUsageIndHasTNDeviceId(usage))
                   {
                       ospvErrCode = OSPPMsgNumToElement( 
                            OSPPUsageIndGetTNDeviceId(usage),
                            (const unsigned char *)OSPPMsgGetElemName(ospeElemTNDeviceId),
                            &subelem);
               
                            /*add attribute critical = "False" since not all servers understand */
			    if (ospvErrCode == OSPC_ERR_NO_ERROR)
                            {
                                 attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgGetAttrName(ospeAttrCritical),
                    			(const unsigned char *)"False");
                        	if (attr != OSPC_OSNULL)
                        	{
                            		OSPPXMLElemAddAttr(subelem, attr);
                            		attr = OSPC_OSNULL;
                        	}
                        	else
                        	{
                            		ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                       	 	}
				OSPPXMLElemAddChild(usgindelem, subelem);
                                subelem = OSPC_OSNULL;
                            }
                    }
                    
                }
            }

            /* Failure reason */
            if(ospvErrCode == OSPC_ERR_NO_ERROR)
            {

                if(OSPPUsageIndHasTNFailReason(usage))
                {

                    ospvErrCode = OSPPMsgNumToElement( 
                        OSPPUsageIndGetTNFailReason(usage),
                        (const unsigned char *)OSPPMsgGetElemName(ospeElemTNFailReason),
                        &subelem);

                    if(ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            /* Statistics */
            if(ospvErrCode == OSPC_ERR_NO_ERROR)
            {

                if(OSPPUsageIndHasTNStatistics(usage))
                {
                    ospvErrCode = OSPPStatisticsToElement(usage->ospmUsageIndTNStats, &subelem);

                    if(ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPXMLElemAddChild(usgindelem, subelem);
                        subelem = OSPC_OSNULL;
                    }
                }
            }

            if(ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, usgindelem);
                usgindelem = OSPC_OSNULL;
            }
        } /* end for */
    }

    /* if for any reason we found an error - destroy any elements created */
    if (ospvErrCode != OSPC_ERR_NO_ERROR)
    {
        if(*ospvElem != OSPC_OSNULL)
        {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    if(usgindelem != OSPC_OSNULL)
    {
        OSPPXMLElemDelete(&usgindelem);
    }

    if(subelem != OSPC_OSNULL)
    {
        OSPPXMLElemDelete(&subelem);
    }

    if(attr != OSPC_OSNULL)
    {
        OSPPXMLAttrDelete(&attr);
    }


    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageIndAddDestinationAlt() - add a destination alt to list a list
 *-----------------------------------------------------------------------*/
void
OSPPUsageIndAddDestinationAlt(
    OSPTUSAGEIND *ospvUsageInd,   /* authorisation indication */
    OSPTALTINFO *ospvAltInfo      /* alt info to add */
    )
{
    if((ospvUsageInd != OSPC_OSNULL) &&
        (ospvAltInfo != OSPC_OSNULL))
    {
        OSPPListAppend(&(ospvUsageInd->ospmUsageIndDestinationAlternate), (ospvAltInfo));
    }
    return;
}

/*-----------------------------------------------------------------------*
 * OSPPUsageIndHasMessageId() - is the message id set ?
 *-----------------------------------------------------------------------*/
unsigned                   /* returns non-zero if message id is set */
OSPPUsageIndHasMessageId(
    OSPTUSAGEIND *ospvUsageInd
)
{
  return (ospvUsageInd->ospmUsageIndMessageId != OSPC_OSNULL);
}

/*-----------------------------------------------------------------------*
 * OSPPUsageIndGetMessageId() - returns a new copy of the message id.
 *-----------------------------------------------------------------------*/
unsigned char  *
OSPPUsageIndGetMessageId(
    OSPTUSAGEIND *ospvUsageInd
)
{
    unsigned char   *messagestring   = OSPC_OSNULL;
    int             len                = 0;

    if (OSPPUsageIndHasMessageId(ospvUsageInd))
    {
        len = OSPM_STRLEN((const char *)ospvUsageInd->ospmUsageIndMessageId);
        OSPM_MALLOC(messagestring, unsigned char, len + 1);
        OSPM_MEMSET(messagestring, 0, len + 1);
        OSPM_MEMCPY(messagestring, ospvUsageInd->ospmUsageIndMessageId, len);
    }

    return messagestring;
}
