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

/* Basic structure for statistics */
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
} OSPT_STATISTICS;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    void OSPPStatisticsDelete(OSPT_STATISTICS **);
    signed OSPPStatisticsGetFracReceived(OSPT_STATISTICS *);
    signed OSPPStatisticsGetFracSent(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetOneWayMinimum(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetOneWayMean(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetOneWaySamples(OSPT_STATISTICS *);
    float OSPPStatisticsGetOneWayVariance(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetPktReceived(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetPktSent(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetRoundTripMinimum(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetRoundTripMean(OSPT_STATISTICS *);
    unsigned OSPPStatisticsGetRoundTripSamples(OSPT_STATISTICS *);
    float OSPPStatisticsGetRoundTripVariance(OSPT_STATISTICS *);
    OSPTBOOL OSPPStatisticsHasLossReceived(OSPT_STATISTICS *);
    OSPTBOOL OSPPStatisticsHasLossSent(OSPT_STATISTICS *);
    OSPTBOOL OSPPStatisticsHasOneWay(OSPT_STATISTICS *);
    OSPTBOOL OSPPStatisticsHasRoundTrip(OSPT_STATISTICS *);
    int OSPPStatisticsLossReceivedToElement(OSPT_STATISTICS *, OSPT_XML_ELEM **);
    int OSPPStatisticsLossSentToElement(OSPT_STATISTICS *, OSPT_XML_ELEM **);
    OSPT_STATISTICS *OSPPStatisticsNew(void);
    int OSPPStatisticsOneWayToElement(OSPT_STATISTICS *, OSPT_XML_ELEM **);
    int OSPPStatisticsRoundTripToElement(OSPT_STATISTICS *, OSPT_XML_ELEM **);
    int OSPPStatisticsReportUsage(OSPT_STATISTICS **, unsigned, signed, unsigned, signed);
    void OSPPStatisticsSetReceivedStatistics(OSPT_STATISTICS *, unsigned, signed);
    void OSPPStatisticsSetSentStatistics(OSPT_STATISTICS *, unsigned, signed);
    int OSPPStatisticsToElement(OSPT_STATISTICS *, OSPT_XML_ELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTATISTICS_H */
