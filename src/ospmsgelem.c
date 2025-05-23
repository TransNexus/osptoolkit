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
    { OSPC_MELEM_ATTEST,               "Attestation" },
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
    { OSPC_MELEM_CACHED,               "Cached" },
    { OSPC_MELEM_CALLCATEGORY,         "CallCategory" },
    { OSPC_MELEM_CALLEDPARTYINFO,      "CalledPartyInfo" },
    { OSPC_MELEM_CALLID,               "CallId" },
    { OSPC_MELEM_CALLIDPRIVACY,        "CallIdPrivacy" },
    { OSPC_MELEM_CALLINGPARTYINFO,     "CallingPartyInfo" },
    { OSPC_MELEM_CALLREASON,           "CallReason" },
    { OSPC_MELEM_CALLTYPE,             "CallType" },
    { OSPC_MELEM_CAPIND,               "CapabilitiesIndication" },
    { OSPC_MELEM_CDRPROXY,             "CDRProxy" },
    { OSPC_MELEM_CDRPROXYFOLDER,       "FolderName" },
    { OSPC_MELEM_CDRPROXYHOST,         "Host" },
    { OSPC_MELEM_CDRPROXYSUBFOLDER,    "SubfolderName" },
    { OSPC_MELEM_CERTINFO,             "CertificateInfo" },
    { OSPC_MELEM_CHARGEINFO,           "ChargeInfoSourceInfo" },
    { OSPC_MELEM_CHARGINGVECTOR,       "ChargingVector" },
    { OSPC_MELEM_CODE,                 "Code" },
    { OSPC_MELEM_CODEC,                "Codec" },
    { OSPC_MELEM_CONNECTTIME,          "ConnectTime" },
    { OSPC_MELEM_CONTACT,              "ContactSourceInfo" },
    { OSPC_MELEM_CPSLATENCY,           "CpsLatency" },
    { OSPC_MELEM_CPSRSPCODE,           "CpsResponseCode" },
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
    { OSPC_MELEM_DESTAUDIOADDR,        "DestinationAudioAddress" },
    { OSPC_MELEM_DESTINFO,             "DestinationInfo" },
    { OSPC_MELEM_DESTOSPVERSION,       "OSPVersion" },
    { OSPC_MELEM_DESTREALM,            "DestinationRealm" },
    { OSPC_MELEM_DESTSIGADDR,          "DestinationSignalAddress" },
    { OSPC_MELEM_DESTVIDEOADDR,        "DestinationVideoAddress" },
    { OSPC_MELEM_DEVICEID,             "DeviceId" },
    { OSPC_MELEM_DEVICEINFO,           "DeviceInfo" },
    { OSPC_MELEM_DIVDEVINFO,           "DiversionDeviceInfo" },
    { OSPC_MELEM_DISPLAYNAME,          "DisplayName" },
    { OSPC_MELEM_DIVSRCINFO,           "DiversionSourceInfo" },
    { OSPC_MELEM_ENDPOINT,             "EndPoint" },
    { OSPC_MELEM_ENDTIME,              "EndTime" },
    { OSPC_MELEM_FINGERPRINT,          "SdpFingerprint" },
    { OSPC_MELEM_FRACTION,             "Fraction" },
    { OSPC_MELEM_FROM,                 "FromSourceInfo" },
    { OSPC_MELEM_GROUP,                "Group" },
    { OSPC_MELEM_GROUPID,              "GroupId" },
    { OSPC_MELEM_HEADERNAME,           "HeaderName" },
    { OSPC_MELEM_HEADERVALUE,          "HeaderValue" },
    { OSPC_MELEM_ICONDIGEST,           "IconDigest" },
    { OSPC_MELEM_ICONURL,              "IconUrl" },
    { OSPC_MELEM_ICPIF,                "ICPIF" },
    { OSPC_MELEM_IDBYPASS,             "IdBypassSourceInfo" },
    { OSPC_MELEM_IDENTITY,             "IdentitySourceInfo" },
    { OSPC_MELEM_INCREMENT,            "Increment" },
    { OSPC_MELEM_LATENCY,              "Latency" },
    { OSPC_MELEM_REQUESTDATE,          "SipRequestDate" },
    { OSPC_MELEM_JIP,                  "JIP" },
    { OSPC_MELEM_JITTER,               "Jitter" },
    { OSPC_MELEM_JURISDICTIONTYPE,     "JurisdictionType" },
    { OSPC_MELEM_LASTREDIRECTBEHALF,   "LastRedirectOnBehalfOf" },
    { OSPC_MELEM_LASTREDIRECTREASON,   "LastRedirectReason" },
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
    { OSPC_MELEM_NETTRANSCALLED,       "NetworkTranslatedCalledNumber" },
    { OSPC_MELEM_NETWORKTYPE,          "NetworkType" },
    { OSPC_MELEM_ONEWAYDELAY,          "OneWayDelay" },
    { OSPC_MELEM_OOB,                  "OutOfBand" },
    { OSPC_MELEM_ORIGID,               "OriginationId" },
    { OSPC_MELEM_ORIGREDIRECTBEHALF,   "OriginalRedirectOnBehalfOf" },
    { OSPC_MELEM_ORIGREDIRECTREASON,   "OriginalRedirectReason" },
    { OSPC_MELEM_PACKETS,              "Packets" },
    { OSPC_MELEM_POSTDIALDELAY,        "PostDialDelay" },
    { OSPC_MELEM_PRICINGCNF,           "PricingConfirmation" },
    { OSPC_MELEM_PRICINGIND,           "PricingIndication" },
    { OSPC_MELEM_PROTOCOL,             "SignalingProtocol" },
    { OSPC_MELEM_RCD,                  "Rcd" },
    { OSPC_MELEM_RCDINTEGRITY,         "RcdIntegrity" },
    { OSPC_MELEM_REAUTHREQ,            "ReauthorizationRequest" },
    { OSPC_MELEM_REAUTHRSP,            "ReauthorizationResponse" },
    { OSPC_MELEM_RELATEDREASON,        "RelatedCallIdReason" },
    { OSPC_MELEM_RELEASE,              "ReleaseSource" },
    { OSPC_MELEM_REQUESTURI,           "RequestDestinationInfo" },
    { OSPC_MELEM_RESOURCES,            "Resources" },
    { OSPC_MELEM_RFACTOR,              "RFactor" },
    { OSPC_MELEM_RICHCALLDATA,         "RichCallData" },
    { OSPC_MELEM_ROLE,                 "Role" },
    { OSPC_MELEM_ROLEFORMAT,           "Format" },
    { OSPC_MELEM_ROLEINFO,             "RoleInfo" },
    { OSPC_MELEM_ROLESTATE,            "State" },
    { OSPC_MELEM_ROLEVENDOR,           "VendorInfo" },
    { OSPC_MELEM_RPID,                 "RemotePartyIdSourceInfo" },
    { OSPC_MELEM_RTDELAY,              "RoundTripDelay" },
    { OSPC_MELEM_SAMPLES,              "Samples" },
    { OSPC_MELEM_SERVICE,              "Service" },
    { OSPC_MELEM_SERVICEPROVIDER,      "ServiceProviderId" },
    { OSPC_MELEM_SERVICETYPE,          "ServiceType" },
    { OSPC_MELEM_SESSIONID,            "SessionId" },
    { OSPC_MELEM_SETUPATTEMPT,         "SetupAttempt" },
    { OSPC_MELEM_SIPMESSAGE,           "SipMessage" },
    { OSPC_MELEM_SRCALT,               "SourceAlternate" },
    { OSPC_MELEM_SRCAUDIOADDR,         "SourceAudioAddress" },
    { OSPC_MELEM_SRCINFO,              "SourceInfo" },
    { OSPC_MELEM_SRCREALM,             "SourceRealm" },
    { OSPC_MELEM_SRCSIGADDR,           "SourceSignalAddress" },
    { OSPC_MELEM_SRCVIDEOADDR,         "SourceVideoAddress" },
    { OSPC_MELEM_STARTTIME,            "StartTime" },
    { OSPC_MELEM_STATISTICS,           "Statistics" },
    { OSPC_MELEM_STATUS,               "Status" },
    { OSPC_MELEM_STATUSHEADER,         "SipStatusHeader" },
    { OSPC_MELEM_STIRINFO,             "StirInfo" },
    { OSPC_MELEM_SYSTEMID,             "SystemId" },
    { OSPC_MELEM_TCCODE,               "TCCode" },
    { OSPC_MELEM_TERMCAUSE,            "TerminationCause" },
    { OSPC_MELEM_TIMESTAMP,            "Timestamp" },
    { OSPC_MELEM_TO,                   "ToDestinationInfo" },
    { OSPC_MELEM_TOKEN,                "Token" },
    { OSPC_MELEM_TOKENINFO,            "TokenInfo" },
    { OSPC_MELEM_TOTALOCTETS,          "TotalOctets" },
    { OSPC_MELEM_TOTALPACKETS,         "TotalPackets" },
    { OSPC_MELEM_TOTALSETUPATTEMPTS,   "TotalSetupAttempts" },
    { OSPC_MELEM_TRANSID,              "TransactionId" },
    { OSPC_MELEM_TRANSFERID,           "TransferId" },
    { OSPC_MELEM_TRANSFERSTATUS,       "TransferStatus" },
    { OSPC_MELEM_TWOWAYDELAY,          "TwoWayDelay" },
    { OSPC_MELEM_UNIT,                 "Unit" },
    { OSPC_MELEM_URL,                  "URL" },
    { OSPC_MELEM_USAGECNF,             "UsageConfirmation" },
    { OSPC_MELEM_USAGEDETAIL,          "UsageDetail" },
    { OSPC_MELEM_USAGEIND,             "UsageIndication" },
    { OSPC_MELEM_USERAGENT,            "UserAgent" },
    { OSPC_MELEM_USERGROUP,            "UserGroup" },
    { OSPC_MELEM_USERID,               "UserId" },
    { OSPC_MELEM_USERNAME,             "UserName" },
    { OSPC_MELEM_USERRATEPLAN,         "SubscriberRatePlan" },
    { OSPC_MELEM_VALIDAFTER,           "ValidAfter" },
    { OSPC_MELEM_VALIDUNTIL,           "ValidUntil" },
    { OSPC_MELEM_VARIANCE,             "Variance" },
    { OSPC_MELEM_VERSTAT,              "VerificationStatus" },
};

const OSPE_MSG_ELEM OSPV_MELEM_SIPHEADER[OSPC_SIPHEADER_NUMBER] = {
    OSPC_MELEM_CONTACT,
    OSPC_MELEM_DIVSRCINFO,
    OSPC_MELEM_FROM,
    OSPC_MELEM_IDBYPASS,
    OSPC_MELEM_IDENTITY,
    OSPC_MELEM_ASSERTEDID,
    OSPC_MELEM_CHARGEINFO,
    OSPC_MELEM_RPID,
    OSPC_MELEM_REQUESTURI,
    OSPC_MELEM_TO
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
