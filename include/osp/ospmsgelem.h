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
 * ospmsgelem.h - Definitions for OSP message elements.
 */

#ifndef _OSPMSGELEM_H
#define _OSPMSGELEM_H

#include "osp/ospmsgpart.h"
#include "osp/ospmsgdesc.h"

/* the elements defined by the Open Settlement Protocol */

typedef enum
{
    ospeElemUnknown = OSPC_MSGPARTUNKNOWN,
    ospeElemAlmostOutOfResources,
    ospeElemAmount,
    ospeElemAuthCnf,
    ospeElemAuthInd,
    ospeElemAuthReq,
    ospeElemAuthRsp,
    ospeElemAuthRzp,
    ospeElemAuthURL,
    ospeElemCallId,
    ospeElemCapInd,
    ospeElemCode,
    ospeElemCurrency,
    ospeElemDesc,
    ospeElemDest,
    ospeElemDestAlt,
    ospeElemDestInfo,
    ospeElemDestSigAddr,
    ospeElemDeviceInfo,
    ospeElemEndPoint,
    ospeElemIncrement,
    ospeElemMaxDest,
    ospeElemMessage,
    ospeElemPricingCnf,
    ospeElemPricingInd,
    ospeElemReauthReq,
    ospeElemReauthRsp,
    ospeElemResources,
    ospeElemRole,
    ospeElemService,
    ospeElemSrcAlt,
    ospeElemSrcInfo,
    ospeElemSrcSigAddr,
    ospeElemStartTime,
    ospeElemStatus,
    ospeElemTimestamp,
    ospeElemToken,
    ospeElemTokenInfo,
    ospeElemTransId,
    ospeElemUnit,
    ospeElemUsageCnf,
    ospeElemUsageDetail,
    ospeElemUsageInd,
    ospeElemValidAfter,
    ospeElemValidUntil,
    ospeElemTNCustId,
    ospeElemTNDelayLimit,
    ospeElemTNDelayPref,
    ospeElemTNDeviceId,
    ospeElemTNFailReason,
    ospeElemTNFraction,
    ospeElemTNLossRcvd,
    ospeElemTNLossSent,
    ospeElemTNMean,
    ospeElemTNMinimum,
    ospeElemTNOneWayDelay,
    ospeElemTNPackets,
    ospeElemTNRoundTripDelay,
    ospeElemTNSamples,
    ospeElemTNStatistics,
    ospeElemTNVariance,
    ospeElemTNAudit,
    ospeElemTNAuditURL,
    ospeElemTNAuditState,
    ospeElemTNAuditTimeLimit,
    ospeElemTNAuditMaxMessages,
    ospeElemCSAuditTrigger,
    ospeElemDestProtocol,
    ospeElemDestOSPVersion,
    ospeElemEndTime,
    ospeElemAlertTime,
    ospeElemPostDialDelay
}
OSPTMSGELEMPART;


/**/
/*-----------------------------------------------------------------------*
 * externally declared global variables
 *-----------------------------------------------------------------------*/

extern const OSPTMSGDESC OSPVElemDescs[];
extern const unsigned    OSPVNumElemDesc;


/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/
const char *OSPPMsgGetElemName(OSPTMSGELEMPART);
OSPTMSGELEMPART OSPPMsgGetElemPart(const char *);

#endif /* _OSPMSGELEM_H */

