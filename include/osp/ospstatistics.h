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
 * ospstatistics.h - Statistics elements.
 */

#ifndef _OSPSTATISTICS_H
#define _OSPSTATISTICS_H

#include "osp/osp.h"

/* Basic structure for TN statistics */
typedef struct {
    OSPTBOOL ospmHasSentStats;
    unsigned ospmLossPacketsSent;
    unsigned ospmLossFractionSent;
    OSPTBOOL ospmHasReceivedStats;
    unsigned ospmLossPacketsReceived;
    unsigned ospmLossFractionReceived;
    OSPTBOOL ospmHasOneWay;
    OSPTDELAY ospmOneWay;
    OSPTBOOL ospmHasRoundTrip;
    OSPTDELAY ospmRoundTrip;
} OSPTSTATISTICS;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    void OSPPStatisticsDelete(OSPTSTATISTICS **);
    signed OSPPStatisticsGetFracReceived(OSPTSTATISTICS *);
    signed OSPPStatisticsGetFracSent(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetOneWayMinimum(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetOneWayMean(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetOneWaySamples(OSPTSTATISTICS *);
    float OSPPStatisticsGetOneWayVariance(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetPktReceived(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetPktSent(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetRoundTripMinimum(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetRoundTripMean(OSPTSTATISTICS *);
    unsigned OSPPStatisticsGetRoundTripSamples(OSPTSTATISTICS *);
    float OSPPStatisticsGetRoundTripVariance(OSPTSTATISTICS *);
    OSPTBOOL OSPPStatisticsHasLossReceived(OSPTSTATISTICS *);
    OSPTBOOL OSPPStatisticsHasLossSent(OSPTSTATISTICS *);
    OSPTBOOL OSPPStatisticsHasOneWay(OSPTSTATISTICS *);
    OSPTBOOL OSPPStatisticsHasRoundTrip(OSPTSTATISTICS *);
    int OSPPStatisticsLossReceivedToElement(OSPTSTATISTICS *, OSPTXMLELEM **);
    int OSPPStatisticsLossSentToElement(OSPTSTATISTICS *, OSPTXMLELEM **);
    OSPTSTATISTICS *OSPPStatisticsNew(void);
    int OSPPStatisticsOneWayToElement(OSPTSTATISTICS *, OSPTXMLELEM **);
    int OSPPStatisticsRoundTripToElement(OSPTSTATISTICS *, OSPTXMLELEM **);
    int OSPPStatisticsReportUsage(OSPTSTATISTICS **, unsigned, signed, unsigned, signed);
    void OSPPStatisticsSetReceivedStatistics(OSPTSTATISTICS *, unsigned, signed);
    void OSPPStatisticsSetSentStatistics(OSPTSTATISTICS *, unsigned, signed);
    int OSPPStatisticsToElement(OSPTSTATISTICS *, OSPTXMLELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTATISTICS_H */
