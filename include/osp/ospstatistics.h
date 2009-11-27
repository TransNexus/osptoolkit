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

#define OSPC_SVALUE_VALUE       ((unsigned)(1 << 0))    /* Also reserved for OSPTBOOL */
#define OSPC_SVALUE_PACKETS     ((unsigned)(1 << 1))
#define OSPC_SVALUE_FRACTION    ((unsigned)(1 << 2))
#define OSPC_SVALUE_SAMPLES     ((unsigned)(1 << 3))
#define OSPC_SVALUE_MINIMUM     ((unsigned)(1 << 4))
#define OSPC_SVALUE_MAXIMUM     ((unsigned)(1 << 5))
#define OSPC_SVALUE_MEAN        ((unsigned)(1 << 6))
#define OSPC_SVALUE_VARIANCE    ((unsigned)(1 << 7))
#define OSPC_SVALUE_SQUARES     ((unsigned)(1 << 8))
#define OSPC_SVALUE_PACKET      ((unsigned)(OSPC_SVALUE_PACKETS | OSPC_SVALUE_FRACTION))
#define OSPC_SVALUE_METRICS     ((unsigned)(OSPC_SVALUE_SAMPLES | OSPC_SVALUE_MINIMUM | OSPC_SVALUE_MAXIMUM | OSPC_SVALUE_MEAN | OSPC_SVALUE_VARIANCE | OSPC_SVALUE_SQUARES))

/*
 * Statistcs, metric types
 */
typedef enum {
    OSPC_SMETRIC_UNDEFINED = -1,
    OSPC_SMETRIC_RTP = 0,
    OSPC_SMETRIC_RTCP,
    /* Number of metric types */
    OSPC_SMETRIC_NUMBER
} OSPE_STATS_METRIC;

/*
 * Statistcs, flow types
 */
typedef enum {
    OSPC_SFLOW_UNDEFINED = -1,
    OSPC_SFLOW_DOWNSTREAM = 0,
    OSPC_SFLOW_UPSTREAM,
    /* Number of flow types */
    OSPC_SFLOW_NUMBER
} OSPE_STATS_FLOW;

/*
 * Statistics, value types
 */
typedef enum {
    OSPC_STATS_LOST = 0,
    OSPC_STATS_JITTER,
    OSPC_STATS_DELAY,
    OSPC_STATS_OCTETS,
    OSPC_STATS_PACKETS,
    OSPC_STATS_RFACTOR,
    OSPC_STATS_MOSCQ,
    OSPC_STATS_MOSLQ,
    OSPC_STATS_ICPIF,
    /* Number of value types */
    OSPC_STATS_NUMBER
} OSPE_STATS;

/*
 * Statistics value structure for lost packets
 */
typedef struct {
    OSPTBOOL hasvalue;
    unsigned packets;
    unsigned fraction;
} OSPT_STATS_PACKET;

/*
 * Statistics value structure for Jitter, delay,
 */
typedef struct {
    OSPTBOOL hasvalue;
    unsigned samples;
    unsigned minimum;
    unsigned maximum;
    unsigned mean;
    float variance;
    double squaressum;
} OSPT_STATS_METRICS;

/*
 * Statistics structure types
 */
typedef enum {
    OSPC_SSTRUCT_PACKET = 0,
    OSPC_SSTRUCT_METRICS,
    OSPC_SSTRUCT_INTEGER,
    OSPC_SSTRUCT_FLOAT,
    /* Number of statistics structure types */
    OSPC_SSTRUCT_NUMBER
} OSPE_STATS_STRUCT;

/* Basic structure for statistics */
typedef struct {
    OSPT_STATS_PACKET ospmLossSent;
    OSPT_STATS_PACKET ospmLossReceived;
    OSPT_STATS_METRICS ospmOneWay;
    OSPT_STATS_METRICS ospmRoundTrip;
    OSPT_STATS_PACKET ospmLost[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    OSPT_STATS_METRICS ospmJitter[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    OSPT_STATS_METRICS ospmDelay[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    int ospmOctets[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    int ospmPackets[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    float ospmRFactor[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    float ospmMOSCQ[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    float ospmMOSLQ[OSPC_SMETRIC_NUMBER][OSPC_SFLOW_NUMBER];
    int ospmICPIF[OSPC_SFLOW_NUMBER];
} OSPT_STATS;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPT_STATS *OSPPStatsNew(void);
    void OSPPStatsDelete(OSPT_STATS **);
    int OSPPStatsToElement(OSPT_STATS *, OSPT_XML_ELEM **);

    OSPTBOOL OSPPStatsHasOneWay(OSPT_STATS *, unsigned);
    OSPTBOOL OSPPStatsHasRoundTrip(OSPT_STATS *, unsigned);
    OSPTBOOL OSPPStatsHasLossSent(OSPT_STATS *, unsigned);
    OSPTBOOL OSPPStatsHasLossReceived(OSPT_STATS *, unsigned);

    int OSPPStatsReportUsage(OSPT_STATS **, int, int, int, int);
    void OSPPStatsSetSentStatistics(OSPT_STATS *, int, int);
    void OSPPStatsSetReceivedStatistics(OSPT_STATS *, int, int);

    unsigned OSPPStatsGetOneWaySamples(OSPT_STATS *);
    unsigned OSPPStatsGetOneWayMinimum(OSPT_STATS *);
    unsigned OSPPStatsGetOneWayMaximum(OSPT_STATS *);
    unsigned OSPPStatsGetOneWayMean(OSPT_STATS *);
    float OSPPStatsGetOneWayVariance(OSPT_STATS *);

    unsigned OSPPStatsGetRoundTripSamples(OSPT_STATS *);
    unsigned OSPPStatsGetRoundTripMinimum(OSPT_STATS *);
    unsigned OSPPStatsGetRoundTripMaximum(OSPT_STATS *);
    unsigned OSPPStatsGetRoundTripMean(OSPT_STATS *);
    float OSPPStatsGetRoundTripVariance(OSPT_STATS *);

    unsigned OSPPStatsGetPktSent(OSPT_STATS *);
    unsigned OSPPStatsGetFracSent(OSPT_STATS *);
    unsigned OSPPStatsGetPktReceived(OSPT_STATS *);
    unsigned OSPPStatsGetFracReceived(OSPT_STATS *);

    int OSPPStatsOneWayToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    int OSPPStatsRoundTripToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    int OSPPStatsLossSentToElement(OSPT_STATS *, OSPT_XML_ELEM **);
    int OSPPStatsLossReceivedToElement(OSPT_STATS *, OSPT_XML_ELEM **);

    OSPTBOOL OSPPStatsHasValue(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, unsigned);
    void OSPPStatsSetPacket(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, int, int);
    void OSPPStatsSetMetrics(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, int, int, int, int, float);
    void OSPPStatsSetInteger(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, int);
    void OSPPStatsSetFloat(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, float);
    void OSPPStatsGetPacket(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, int *, int *);
    void OSPPStatsGetMetrics(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, int *, int *, int *, int *, float *);
    int OSPPStatsGetInteger(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW);
    float OSPPStatsGetFloat(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW);
    int OSPPStatsValueToElement(OSPT_STATS *, OSPE_STATS, OSPE_STATS_METRIC, OSPE_STATS_FLOW, OSPT_XML_ELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTATISTICS_H */
