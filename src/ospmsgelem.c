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
 * ospxmlmsgelem.c - XML element processing specific to OSP.
 */

#include "osp/osp.h"
#include "osp/ospmsgdesc.h"
#include "osp/ospmsgelem.h"


/**/
/*-----------------------------------------------------------------------*
 * Static global variables
 *-----------------------------------------------------------------------*/

/* array that associates elements and names */

const OSPTMSGDESC OSPVElemDescs[] =
{
    { ospeElemUnknown,          "" },
    { ospeElemAlmostOutOfResources, "AlmostOutOfResources"},
    { ospeElemAmount,           "Amount" },
    { ospeElemAuthCnf,          "AuthorizationConfirmation" },
    { ospeElemAuthInd,          "AuthorizationIndication" },
    { ospeElemAuthReq,          "AuthorizationRequest" },
    { ospeElemAuthRsp,          "AuthorisationResponse" },
    { ospeElemAuthRzp,          "AuthorizationResponse" },
    { ospeElemAuthURL ,         "AuthorityURL" },
    { ospeElemCallId ,          "CallId" },
    { ospeElemCapInd,           "CapabilitiesIndication"},
    { ospeElemCode,             "Code" },
    { ospeElemCurrency,         "Currency" },
    { ospeElemDesc,             "Description" },
    { ospeElemDest,             "Destination" },
    { ospeElemDestAlt,          "DestinationAlternate" },
    { ospeElemDestInfo,         "DestinationInfo" },
    { ospeElemDestSigAddr,      "DestinationSignalAddress" },
    { ospeElemDeviceInfo,       "DeviceInfo" },
    { ospeElemEndPoint,         "EndPoint" },
    { ospeElemIncrement,        "Increment" },
    { ospeElemMaxDest,          "MaximumDestinations" },
    { ospeElemMessage,          "Message" },
    { ospeElemPricingCnf,       "PricingConfirmation" },
    { ospeElemPricingInd,       "PricingIndication" },
    { ospeElemReauthReq,        "ReauthorizationRequest" },
    { ospeElemReauthRsp,        "ReauthorizationResponse" },
    { ospeElemResources,        "Resources" },
    { ospeElemRole,             "Role" },
    { ospeElemService,          "Service" },
    { ospeElemSrcAlt,           "SourceAlternate" },
    { ospeElemSrcInfo,          "SourceInfo" },
    { ospeElemSrcSigAddr,       "SourceSignalAddress" },
    { ospeElemStartTime,        "StartTime" },
    { ospeElemStatus,           "Status" },
    { ospeElemTimestamp,        "Timestamp" },
    { ospeElemToken,            "Token" },
    { ospeElemTokenInfo,        "TokenInfo" },
    { ospeElemTransId,          "TransactionId" },
    { ospeElemUnit,             "Unit" },
    { ospeElemUsageCnf,         "UsageConfirmation" },
    { ospeElemUsageDetail,      "UsageDetail" },
    { ospeElemUsageInd,         "UsageIndication" },
    { ospeElemValidAfter,       "ValidAfter" },
    { ospeElemValidUntil ,      "ValidUntil" },
    { ospeElemTNCustId,         "transnexus.com:CustomerId" },
    { ospeElemTNDelayLimit,     "transnexus.com:DelayLimit" },
    { ospeElemTNDelayPref,      "transnexus.com:DelayPreference" },
    { ospeElemTNDeviceId,       "transnexus.com:DeviceId" },
    { ospeElemTNFailReason,     "transnexus.com:FailureReason" },
    { ospeElemTNFraction,       "transnexus.com:Fraction" },
    { ospeElemTNLossRcvd,       "transnexus.com:LossReceived" },
    { ospeElemTNLossSent,       "transnexus.com:LossSent" },
    { ospeElemTNMean,           "transnexus.com:Mean" },
    { ospeElemTNMinimum,        "transnexus.com:Minimum" },
    { ospeElemTNOneWayDelay,    "transnexus.com:OneWayDelay" },
    { ospeElemTNPackets,        "transnexus.com:Packets" },
    { ospeElemTNRoundTripDelay, "transnexus.com:RoundTripDelay" },
    { ospeElemTNSamples,        "transnexus.com:Samples" },
    { ospeElemTNStatistics,     "transnexus.com:Statistics" },
    { ospeElemTNVariance,       "transnexus.com:Variance" },
    { ospeElemTNAudit,          "transnexus.com:Audit" },
    { ospeElemTNAuditURL,       "transnexus.com:AuditURL" },
    { ospeElemTNAuditState,       "transnexus.com:AuditState" },
    { ospeElemTNAuditTimeLimit,   "transnexus.com:AuditTimeLimit" },
    { ospeElemTNAuditMaxMessages, "transnexus.com:AuditMaxMessages" },
    { ospeElemCSAuditTrigger,     "cisco.com:AuditSignal" },
    { ospeElemDestProtocol,     "DestinationProtocol" },
    { ospeElemDestOSPVersion,     "OSPVersion" },
    { ospeElemEndTime,        "EndTime" },
    { ospeElemAlertTime,        "AlertTime" },
    { ospeElemPostDialDelay,        "PostDialDelay" }
};


const unsigned OSPVNumElemDesc = sizeof(OSPVElemDescs)/sizeof(OSPTMSGDESC);

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgGetElemName() - get an element name from a part value
 *-----------------------------------------------------------------------*/
const char *                              /* returns pointer to the name */
OSPPMsgGetElemName(
    OSPTMSGELEMPART ospvPart
)
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvPart!= OSPC_MSGPARTUNKNOWN) 
    {

        ospvName = OSPPMsgDescGetName((OSPTMSGPART)ospvPart, OSPVElemDescs,
            OSPVNumElemDesc);
    }
    return(ospvName);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgGetElemPart() - get an elemement part ID from its name
 *-----------------------------------------------------------------------*/
OSPTMSGELEMPART                           /* returns part */
    OSPPMsgGetElemPart(
    const char *ospvName
    )
{
    OSPTMSGELEMPART ospvPart = (OSPTMSGELEMPART)OSPC_MSGPARTUNKNOWN;

    if (ospvName != OSPC_OSNULL) 
    {

        ospvPart = (OSPTMSGELEMPART) OSPPMsgDescGetPart(ospvName,
            OSPVElemDescs, OSPVNumElemDesc);
    }
    return(ospvPart);
}

