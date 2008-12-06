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

/*
 * Static global variables
 */

/* Array that associates elements and names */
const OSPT_MSG_DESC OSPV_MELEM_DESCS[OSPC_MELEM_NUMBER] = { 
    { OSPC_MELEM_UNKNOWN,              "" },
    { OSPC_MELEM_ALMOSTOUTOFRESOURCES, "AlmostOutOfResources" },
    { OSPC_MELEM_AMOUNT,               "Amount" },
    { OSPC_MELEM_AUTHCNF,              "AuthorizationConfirmation" },
    { OSPC_MELEM_AUTHIND,              "AuthorizationIndication" },
    { OSPC_MELEM_AUTHREQ,              "AuthorizationRequest" },
    { OSPC_MELEM_AUTHRSP,              "AuthorisationResponse" },
    { OSPC_MELEM_AUTHRZP,              "AuthorizationResponse" },
    { OSPC_MELEM_AUTHURL,              "AuthorityURL" },
    { OSPC_MELEM_CALLID,               "CallId" },
    { OSPC_MELEM_CAPIND,               "CapabilitiesIndication" },
    { OSPC_MELEM_CODE,                 "Code" },
    { OSPC_MELEM_CURRENCY,             "Currency" },
    { OSPC_MELEM_DESC,                 "Description" },
    { OSPC_MELEM_DEST,                 "Destination" },
    { OSPC_MELEM_DESTALT,              "DestinationAlternate" },
    { OSPC_MELEM_DESTINFO,             "DestinationInfo" },
    { OSPC_MELEM_DESTSIGADDR,          "DestinationSignalAddress" },
    { OSPC_MELEM_DEVICEINFO,           "DeviceInfo" },
    { OSPC_MELEM_ENDPOINT,             "EndPoint" },
    { OSPC_MELEM_INCREMENT,            "Increment" },
    { OSPC_MELEM_MAXDEST,              "MaximumDestinations" },
    { OSPC_MELEM_MESSAGE,              "Message" },
    { OSPC_MELEM_PRICINGCNF,           "PricingConfirmation" },
    { OSPC_MELEM_PRICINGIND,           "PricingIndication" },
    { OSPC_MELEM_REAUTHREQ,            "ReauthorizationRequest" },
    { OSPC_MELEM_REAUTHRSP,            "ReauthorizationResponse" },
    { OSPC_MELEM_RESOURCES,            "Resources" },
    { OSPC_MELEM_ROLE,                 "Role" },
    { OSPC_MELEM_SERVICE,              "Service" },
    { OSPC_MELEM_SRCALT,               "SourceAlternate" },
    { OSPC_MELEM_SRCINFO,              "SourceInfo" },
    { OSPC_MELEM_SRCSIGADDR,           "SourceSignalAddress" },
    { OSPC_MELEM_STARTTIME,            "StartTime" },
    { OSPC_MELEM_STATUS,               "Status" },
    { OSPC_MELEM_TIMESTAMP,            "Timestamp" },
    { OSPC_MELEM_TOKEN,                "Token" },
    { OSPC_MELEM_TOKENINFO,            "TokenInfo" },
    { OSPC_MELEM_TRANSID,              "TransactionId" },
    { OSPC_MELEM_UNIT,                 "Unit" },
    { OSPC_MELEM_USAGECNF,             "UsageConfirmation" },
    { OSPC_MELEM_USAGEDETAIL,          "UsageDetail" },
    { OSPC_MELEM_USAGEIND,             "UsageIndication" },
    { OSPC_MELEM_VALIDAFTER,           "ValidAfter" },
    { OSPC_MELEM_VALIDUNTIL,           "ValidUntil" },
    { OSPC_MELEM_TNCUSTID,             "transnexus.com:CustomerId" },
    { OSPC_MELEM_TNDELAYLIMIT,         "transnexus.com:DelayLimit" },
    { OSPC_MELEM_TNDELAYPREF,          "transnexus.com:DelayPreference" },
    { OSPC_MELEM_TNDEVICEID,           "transnexus.com:DeviceId" },
    { OSPC_MELEM_TNFAILREASON,         "transnexus.com:FailureReason" },
    { OSPC_MELEM_TNFRACTION,           "transnexus.com:Fraction" },
    { OSPC_MELEM_TNLOSSRCVD,           "transnexus.com:LossReceived" },
    { OSPC_MELEM_TNLOSSSENT,           "transnexus.com:LossSent" },
    { OSPC_MELEM_TNMEAN,               "transnexus.com:Mean" },
    { OSPC_MELEM_TNMINIMUM,            "transnexus.com:Minimum" },
    { OSPC_MELEM_TNONEWAYDELAY,        "transnexus.com:OneWayDelay" },
    { OSPC_MELEM_TNPACKETS,            "transnexus.com:Packets" },
    { OSPC_MELEM_TNROUNDTRIPDELAY,     "transnexus.com:RoundTripDelay" },
    { OSPC_MELEM_TNSAMPLES,            "transnexus.com:Samples" },
    { OSPC_MELEM_TNSTATISTICS,         "transnexus.com:Statistics" },
    { OSPC_MELEM_TNVARIANCE,           "transnexus.com:Variance" },
    { OSPC_MELEM_TNAUDIT,              "transnexus.com:Audit" },
    { OSPC_MELEM_TNAUDITURL,           "transnexus.com:AuditURL" },
    { OSPC_MELEM_TNAUDITSTATE,         "transnexus.com:AuditState" },
    { OSPC_MELEM_TNAUDITTIMELIMIT,     "transnexus.com:AuditTimeLimit" },
    { OSPC_MELEM_TNAUDITMAXMESSAGES,   "transnexus.com:AuditMaxMessages" },
    { OSPC_MELEM_CSAUDITTRIGGER,       "cisco.com:AuditSignal" },
    { OSPC_MELEM_DESTPROTOCOL,         "DestinationProtocol" },
    { OSPC_MELEM_DESTOSPVERSION,       "OSPVersion" },
    { OSPC_MELEM_ENDTIME,              "EndTime" },
    { OSPC_MELEM_ALERTTIME,            "AlertTime" },
    { OSPC_MELEM_POSTDIALDELAY,        "PostDialDelay" },
    { OSPC_MELEM_CONNECTTIME,          "ConnectTime" },
    { OSPC_MELEM_RELEASESOURCE,        "ReleaseSource" },
    { OSPC_MELEM_SERVICETYPE,          "ServiceType" },
    { OSPC_MELEM_GROUP,                "Group" },
    { OSPC_MELEM_GROUPID,              "GroupId" }
};

/*
 * OSPPMsgElemGetName() - get an element name from a part value
 */
const char *OSPPMsgElemGetName(     /* returns pointer to the name */
    OSPE_MSG_ELEM ospvPart)
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvPart != OSPC_MELEM_UNKNOWN) {
        ospvName = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_MELEM_DESCS, OSPC_MELEM_NUMBER);
    }

    return (ospvName);
}

/*
 * OSPPMsgElemGetPart() - get an elemement part ID from its name
 */
OSPE_MSG_ELEM OSPPMsgElemGetPart(   /* returns part */
    const char *ospvName)
{
    OSPE_MSG_ELEM ospvPart = OSPC_MELEM_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        ospvPart = (OSPE_MSG_ELEM)OSPPMsgDescGetPart(ospvName, OSPV_MELEM_DESCS, OSPC_MELEM_NUMBER);
    }

    return (ospvPart);
}
