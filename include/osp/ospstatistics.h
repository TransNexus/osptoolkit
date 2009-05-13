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

#define OSPC_SVALUE_UNUSED      ((unsigned)(1 << 0))    /* Reserve for OSPTBOOL */
#define OSPC_SVALUE_VALUE       ((unsigned)(1 << 1))
#define OSPC_SVALUE_SAMPLES     ((unsigned)(1 << 2))
#define OSPC_SVALUE_MIN         ((unsigned)(1 << 3))
#define OSPC_SVALUE_MAX         ((unsigned)(1 << 4))
#define OSPC_SVALUE_MEAN        ((unsigned)(1 << 5))
#define OSPC_SVALUE_VARIANCE    ((unsigned)(1 << 6))
#define OSPC_SVALUE_SQUARES     ((unsigned)(1 << 7))
#define OSPC_SVALUE_FULL        ((unsigned)(OSPC_SVALUE_SAMPLES | OSPC_SVALUE_MIN | OSPC_SVALUE_MAX | OSPC_SVALUE_MEAN | OSPC_SVALUE_VARIANCE | OSPC_SVALUE_SQUARES))

/*
 * Statistics value types
 */
typedef enum {
    OSPC_STATS_DELAY = 0,
    OSPC_STATS_JITTER,
    OSPC_STATS_PACKLOSS,
    OSPC_STATS_RFACTOR,
    OSPC_STATS_MOS,
    /* Number of statistics types */
    OSPC_STATS_NUMBER
} OSPE_STATS;

/*
 * Statistics value structure
 */
typedef struct {
    OSPTBOOL HasValue;
    unsigned Value;
    unsigned Samples;
    unsigned Minimum;
    unsigned Maximum;
    unsigned Mean;
    float Variance;
    double SquaresSum;
} OSPT_STATS_VALUE;

/* Basic structure for statistics */
typedef struct {
    OSPTBOOL ospmHasSentStats;
    unsigned ospmLossPacketsSent;
    unsigned ospmLossFractionSent;
    OSPTBOOL ospmHasReceivedStats;
    unsigned ospmLossPacketsReceived;
    unsigned ospmLossFractionReceived;
    OSPT_STATS_VALUE ospmOneWay;
    OSPT_STATS_VALUE ospmRoundTrip;
    OSPT_STATS_VALUE ospmDelay[OSPC_DIR_NUMBER];
    OSPT_STATS_VALUE ospmJitter[OSPC_DIR_NUMBER];
    OSPT_STATS_VALUE ospmPackLoss[OSPC_DIR_NUMBER];
    OSPT_STATS_VALUE ospmRFactor[OSPC_DIR_NUMBER];
    OSPT_STATS_VALUE ospmMOS[OSPC_DIR_NUMBER];
} OSPT_STATS;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    void OSPPStatsDelete(OSPT_STATS **);
    signed OSPPStatsGetFracReceived(OSPT_STATS *);
    signed OSPPStatsGetFracSent(OSPT_STATS *);
    unsigned OSPPStatsGetOneWayMinimum(OSPT_STATS *);
    unsigned OSPPStatsGetOneWayMean(OSPT_STATS *);
    unsigned OSPPStatsGetOneWaySamples(OSPT_STATS *);
    float OSPPStatsGetOneWayVariance(OSPT_STATS *);
    unsigned OSPPStatsGetPktReceived(OSPT_STATS *);
    unsigned OSPPStatsGetPktSent(OSPT_STATS *);
    unsigned OSPPStatsGetRoundTripMinimum(OSPT_STATS *);
    unsigned OSPPStatsGetRoundTripMean(OSPT_STATS *);
    unsigned OSPPStatsGetRoundTripSamples(OSPT_STATS *);
    float OSPPStatsGetRoundTripVariance(OSPT_STATS *);
    OSPTBOOL OSPPStatsHasLossReceived(OSPT_STATS *);
    OSPTBOOL OSPPStatsHasLossSent(OSPT_STATS *);
    OSPTBOOL OSPPStatsHasOneWay(OSPT_STATS *);
    OSPTBOOL OSPPStatsHasRoundTrip(OSPT_STATS *);
    int OSPPStatsLossReceivedToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    int OSPPStatsLossSentToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    OSPT_STATS *OSPPStatsNew(void);
    int OSPPStatsOneWayToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    int OSPPStatsRoundTripToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    int OSPPStatsReportUsage(OSPT_STATS **, unsigned, signed, unsigned, signed);
    void OSPPStatsSetReceivedStatistics(OSPT_STATS *, unsigned, signed);
    void OSPPStatsSetSentStatistics(OSPT_STATS *, unsigned, signed);
    int OSPPStatsToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    OSPTBOOL OSPPStatsHasValue(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, unsigned);
    unsigned OSPPStatsGetValue(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION);
    unsigned OSPPStatsGetSamples(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION);
    unsigned OSPPStatsGetMin(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION);
    unsigned OSPPStatsGetMax(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION);
    unsigned OSPPStatsGetMean(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION);
    float OSPPStatsGetVariance(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION);
    double OSPPStatsGetSquaresSum(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION);
    void OSPPStatsSetSamples(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, unsigned);
    void OSPPStatsSetMin(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, unsigned);
    void OSPPStatsSetMax(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, unsigned);
    void OSPPStatsSetMean(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, unsigned);
    void OSPPStatsSetVariance(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, float);
    void OSPPStatsSetSquaresSum(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, double);
    void OSPPStatsSetRFactor(OSPT_STATS *, OSPE_DIRECTION, unsigned);
    void OSPPStatsSetMOS(OSPT_STATS *, OSPE_DIRECTION, unsigned);
    int OSPPStatsValueToElement(OSPT_STATS *, OSPE_STATS, OSPE_DIRECTION, OSPT_XML_ELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTATISTICS_H */
