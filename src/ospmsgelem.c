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

/* ospxmlmsgelem.c - XML element processing specific to OSP. */

#include "osp/osp.h"
#include "osp/ospmsgdesc.h"
#include "osp/ospmsgelem.h"

/* Static global variables */

/* Array that associates elements and names */
const OSPT_MSG_DESC OSPV_MELEM_DESCS[OSPC_MELEM_NUMBER] = {
    { OSPC_MELEM_ALMOSTOUTOFRESOURCES, "AlmostOutOfResources" },
    { OSPC_MELEM_ALERTTIME,            "AlertTime" },
    { OSPC_MELEM_AMOUNT,               "Amount" },
    { OSPC_MELEM_APPLID,               "ApplicationId" },
    { OSPC_MELEM_ASSERTEDID,           "AssertedIdSourceInfo" },
    { OSPC_MELEM_AUDIT,                "Audit" },
    { OSPC_MELEM_AUDITMAXMESSAGES,     "AuditMaxMessages" },
    { OSPC_MELEM_AUDITSTATE,           "AuditState" },
    { OSPC_MELEM_AUDITTIMELIMIT,       "AuditTimeLimit" },
    { OSPC_MELEM_AUDITURL,             "AuditURL" },
    { OSPC_MELEM_AUTHCNF,              "AuthorizationConfirmation" },
    { OSPC_MELEM_AUTHIND,              "AuthorizationIndication" },
    { OSPC_MELEM_AUTHREQ,              "AuthorizationRequest" },
    { OSPC_MELEM_AUTHRSP,              "AuthorisationResponse" },
    { OSPC_MELEM_AUTHRZP,              "AuthorizationResponse" },
    { OSPC_MELEM_AUTHURL,              "AuthorityURL" },
    { OSPC_MELEM_CALLID,               "CallId" },
    { OSPC_MELEM_CAPIND,               "CapabilitiesIndication" },
    { OSPC_MELEM_CODE,                 "Code" },
    { OSPC_MELEM_CODEC,                "Codec" },
    { OSPC_MELEM_CONNECTTIME,          "ConnectTime" },
    { OSPC_MELEM_CSAUDITTRIGGER,       "cisco.com:AuditSignal" },
    { OSPC_MELEM_CURRENCY,             "Currency" },
    { OSPC_MELEM_CUSTID,               "CustomerId" },
    { OSPC_MELEM_CUSTINFO,             "CustomInfo" },
    { OSPC_MELEM_DELAY,                "Delay" },
    { OSPC_MELEM_DELAYLIMIT,           "DelayLimit" },
    { OSPC_MELEM_DELAYPREF,            "DelayPreference" },
    { OSPC_MELEM_DESC,                 "Description" },
    { OSPC_MELEM_DEST,                 "Destination" },
    { OSPC_MELEM_DESTALT,              "DestinationAlternate" },
    { OSPC_MELEM_DESTINFO,             "DestinationInfo" },
    { OSPC_MELEM_DESTOSPVERSION,       "OSPVersion" },
    { OSPC_MELEM_DESTREALM,            "DestinationRealm" },
    { OSPC_MELEM_DESTSIGADDR,          "DestinationSignalAddress" },
    { OSPC_MELEM_DEVICEID,             "DeviceId" },
    { OSPC_MELEM_DEVICEINFO,           "DeviceInfo" },
    { OSPC_MELEM_DIVDEVINFO,           "DiversionDeviceInfo" },
    { OSPC_MELEM_DIVSRCINFO,           "DiversionSourceInfo" },
    { OSPC_MELEM_ENDPOINT,             "EndPoint" },
    { OSPC_MELEM_ENDTIME,              "EndTime" },
    { OSPC_MELEM_FRACTION,             "Fraction" },
    { OSPC_MELEM_GROUP,                "Group" },
    { OSPC_MELEM_GROUPID,              "GroupId" },
    { OSPC_MELEM_ICPIF,                "ICPIF" },
    { OSPC_MELEM_INCREMENT,            "Increment" },
    { OSPC_MELEM_JITTER,               "Jitter" },
    { OSPC_MELEM_LOSSRCVD,             "LossReceived" },
    { OSPC_MELEM_LOSSSENT,             "LossSent" },
    { OSPC_MELEM_LOST,                 "Lost" },
    { OSPC_MELEM_MAXDEST,              "MaximumDestinations" },
    { OSPC_MELEM_MAXIMUM,              "Maximum" },
    { OSPC_MELEM_MEAN,                 "Mean" },
    { OSPC_MELEM_MESSAGE,              "Message" },
    { OSPC_MELEM_METRICS,              "Metrics" },
    { OSPC_MELEM_MINIMUM,              "Minimum" },
    { OSPC_MELEM_MOSCQ,                "MOSCQ" },
    { OSPC_MELEM_MOSLQ,                "MOSLQ" },
    { OSPC_MELEM_ONEWAYDELAY,          "OneWayDelay" },
    { OSPC_MELEM_PACKETS,              "Packets" },
    { OSPC_MELEM_POSTDIALDELAY,        "PostDialDelay" },
    { OSPC_MELEM_PRICINGCNF,           "PricingConfirmation" },
    { OSPC_MELEM_PRICINGIND,           "PricingIndication" },
    { OSPC_MELEM_PROTOCOL,             "SignalingProtocol" },
    { OSPC_MELEM_REAUTHREQ,            "ReauthorizationRequest" },
    { OSPC_MELEM_REAUTHRSP,            "ReauthorizationResponse" },
    { OSPC_MELEM_RELEASESOURCE,        "ReleaseSource" },
    { OSPC_MELEM_RESOURCES,            "Resources" },
    { OSPC_MELEM_RFACTOR,              "RFactor" },
    { OSPC_MELEM_ROLE,                 "Role" },
    { OSPC_MELEM_ROLEFORMAT,           "Format" },
    { OSPC_MELEM_ROLEINFO,             "RoleInfo" },
    { OSPC_MELEM_ROLESTATE,            "State" },
    { OSPC_MELEM_ROLEVENDOR,           "VendorInfo" },
    { OSPC_MELEM_RPID,                 "RemotePartyIdSourceInfo" },
    { OSPC_MELEM_RTDELAY,              "RoundTripDelay" },
    { OSPC_MELEM_SAMPLES,              "Samples" },
    { OSPC_MELEM_SERVICE,              "Service" },
    { OSPC_MELEM_SERVICETYPE,          "ServiceType" },
    { OSPC_MELEM_SESSIONID,            "SessionId" },
    { OSPC_MELEM_SETUPATTEMPT,         "SetupAttempt" },
    { OSPC_MELEM_SRCALT,               "SourceAlternate" },
    { OSPC_MELEM_SRCINFO,              "SourceInfo" },
    { OSPC_MELEM_SRCREALM,             "SourceRealm" },
    { OSPC_MELEM_SRCSIGADDR,           "SourceSignalAddress" },
    { OSPC_MELEM_STARTTIME,            "StartTime" },
    { OSPC_MELEM_STATISTICS,           "Statistics" },
    { OSPC_MELEM_STATUS,               "Status" },
    { OSPC_MELEM_TCCODE,               "TCCode" },
    { OSPC_MELEM_TERMCAUSE,            "TerminationCause" },
    { OSPC_MELEM_TIMESTAMP,            "Timestamp" },
    { OSPC_MELEM_TOKEN,                "Token" },
    { OSPC_MELEM_TOKENINFO,            "TokenInfo" },
    { OSPC_MELEM_TOTALOCTETS,          "TotalOctets" },
    { OSPC_MELEM_TOTALPACKETS,         "TotalPackets" },
    { OSPC_MELEM_TRANSID,              "TransactionId" },
    { OSPC_MELEM_TWOWAYDELAY,          "TwoWayDelay" },
    { OSPC_MELEM_UNIT,                 "Unit" },
    { OSPC_MELEM_USAGECNF,             "UsageConfirmation" },
    { OSPC_MELEM_USAGEDETAIL,          "UsageDetail" },
    { OSPC_MELEM_USAGEIND,             "UsageIndication" },
    { OSPC_MELEM_VALIDAFTER,           "ValidAfter" },
    { OSPC_MELEM_VALIDUNTIL,           "ValidUntil" },
    { OSPC_MELEM_VARIANCE,             "Variance" },
};

/*
 * OSPPMsgElemGetName() - get an element name from a part value
 */
const char *OSPPMsgElemGetName(     /* returns pointer to the name */
    OSPE_MSG_ELEM ospvPart)
{
    const char *ospvName = OSPC_OSNULL;

    if ((ospvPart >= OSPC_MELEM_START) && (ospvPart < OSPC_MELEM_NUMBER)) {
        ospvName = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_MELEM_DESCS, OSPC_MELEM_NUMBER);
    }

    return ospvName;
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

    return ospvPart;
}
