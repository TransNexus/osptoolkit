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
 * ospstatistics.c - OSP Statistics elements functions
 */

#include "osp/osp.h"
#include "osp/ospstatistics.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsg.h"

/* Create new Statistics Structure */
OSPT_STATS *OSPPStatsNew(void)
{
    OSPE_STATS_RANGE range;
    OSPE_STATS_FLOW flow;
    OSPT_STATS *ospvStats = OSPC_OSNULL;

    OSPM_MALLOC(ospvStats, OSPT_STATS, sizeof(OSPT_STATS));
    if (ospvStats != OSPC_OSNULL) {
        OSPM_MEMSET(ospvStats, 0, sizeof(OSPT_STATS));

        for (range = OSPC_SRANGE_PEERPEER; range < OSPC_SRANGE_NUMBER; range++) {
            for (flow = OSPC_SFLOW_DOWNSTREAM; flow < OSPC_SFLOW_NUMBER; flow++) {
                ospvStats->ospmOctets[range][flow] = -1;
                ospvStats->ospmPackets[range][flow] = -1;
                ospvStats->ospmRFactor[range][flow] = -1;
                ospvStats->ospmMOS[range][flow] = -1;
            }
        }
    }

    return ospvStats;
}

/* Delete Statistics structure */
void OSPPStatsDelete(
    OSPT_STATS **ospvStats)
{
    if (*ospvStats != OSPC_OSNULL) {
        OSPM_FREE(*ospvStats);
        *ospvStats = OSPC_OSNULL;
    }
}

/* Create xml element from statistics structure */
int OSPPStatsToElement(
    OSPT_STATS *ospvStats,
    OSPT_XML_ELEM **ospvElem)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPE_STATS_REPORTER reporter;
    OSPE_STATS type;
    OSPE_STATS_RANGE range;
    OSPE_STATS_FLOW flow;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvStats == OSPC_OSNULL) {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }

    if (ospvElem == OSPC_OSNULL) {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* create the Statistics element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_STATISTICS), "");
        if (*ospvElem == OSPC_OSNULL) {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            switch (OSPPStatsGetReporter(ospvStats)) {
            case OSPC_SREPORTER_CALLING:
                reporter = OSPC_ALTINFO_CALLING;
                break;
            case OSPC_SREPORTER_CALLED:
                reporter = OSPC_ALTINFO_CALLED;
                break;
            case OSPC_SREPORTER_PROXY:
            default:
                reporter = OSPC_ALTINFO_PROXY;
                break;
            }
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_REPORTER), OSPPAltInfoTypeGetName(reporter));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;

                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
                if (attr != OSPC_OSNULL) {
                    OSPPXMLElemAddAttr(*ospvElem, attr);
                    attr = OSPC_OSNULL;
                } else {
                    errorcode = OSPC_ERR_XML_NO_ATTR;
                }
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    /* Now add the children */
    /* First LossSent */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasLossSent(ospvStats, OSPC_SVALUE_PACK)) {
            errorcode = OSPPStatsLossSentToElement(ospvStats, &elem);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* LossReceived */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasLossReceived(ospvStats, OSPC_SVALUE_PACK)) {
            errorcode = OSPPStatsLossReceivedToElement(ospvStats, &elem);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* OneWay */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasOneWay(ospvStats)) {
            errorcode = OSPPStatsOneWayToElement(ospvStats, &elem);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* RoundTrip */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasRoundTrip(ospvStats)) {
            errorcode = OSPPStatsRoundTripToElement(ospvStats, &elem);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Lost/Jitter/Delay/Octets/Packets/RFactor/MOS */
    for (range = OSPC_SRANGE_PEERPEER; (range < OSPC_SRANGE_NUMBER) && (errorcode == OSPC_ERR_NO_ERROR); range++) {
           for (type = OSPC_STATS_LOST; (type < OSPC_STATS_NUMBER) && (errorcode == OSPC_ERR_NO_ERROR); type++) {
            for (flow = OSPC_SFLOW_DOWNSTREAM; (flow < OSPC_SFLOW_NUMBER) && (errorcode == OSPC_ERR_NO_ERROR); flow++) {
                errorcode = OSPPStatsValueToElement(ospvStats, type, range, flow, &elem);
            }
        }
        if ((errorcode == OSPC_ERR_NO_ERROR) && (elem != OSPC_OSNULL)) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    if (elem != OSPC_OSNULL) {
        OSPPXMLElemDelete(&elem);
    }

    if (attr != OSPC_OSNULL) {
        OSPPXMLAttrDelete(&attr);
    }

    return errorcode;
}

/* Check for existence of OneWay statistics */
OSPTBOOL OSPPStatsHasOneWay(
    OSPT_STATS *ospvStats)
{
    if (ospvStats != OSPC_OSNULL) {
        return ospvStats->ospmOneWay.hasvalue;
    } else {
        return OSPC_FALSE;
    }
}

/* Check for existence of RoundTrip statistics */
OSPTBOOL OSPPStatsHasRoundTrip(
    OSPT_STATS *ospvStats)
{
    if (ospvStats != OSPC_OSNULL) {
        return ospvStats->ospmRoundTrip.hasvalue;
    } else {
        return OSPC_FALSE;
    }
}

/* Check for existence of LossSent statistics */
OSPTBOOL OSPPStatsHasLossSent(
    OSPT_STATS *ospvStats,
    unsigned ospvValueFlags)
{
    if (ospvStats != OSPC_OSNULL) {
        return ((ospvStats->ospmLossSent.hasvalue & ospvValueFlags & OSPC_SVALUE_PACK) != 0);
    } else {
        return OSPC_FALSE;
    }
}

/* Check for existence of LossReceived statistics */
OSPTBOOL OSPPStatsHasLossReceived(
    OSPT_STATS *ospvStats,
    unsigned ospvValueFlags)
{
    if (ospvStats != OSPC_OSNULL) {
        return ((ospvStats->ospmLossReceived.hasvalue & ospvValueFlags & OSPC_SVALUE_PACK) != 0);
    } else {
        return OSPC_FALSE;
    }
}

/* Report statistics for this transaction. */
int OSPPStatsReportUsage(
    OSPT_STATS **ospvStats,         /* In - pter to place for stats struct */
    int ospvLossPacketsSent,        /* In  */
    int ospvLossFractionSent,       /* In  */
    int ospvLossPacketsReceived,    /* In  */
    int ospvLossFractionReceived)   /* In  */
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * If variables are negative, no statistics are reported.
     * Just return with no error
     */
    if ((ospvLossPacketsSent >= 0) || (ospvLossFractionSent >= 0)) {
        if (*ospvStats == OSPC_OSNULL) {
            *ospvStats = OSPPStatsNew();
            if (*ospvStats == OSPC_OSNULL) {
                errorcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetSentStatistics(*ospvStats, ospvLossPacketsSent, ospvLossFractionSent);
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if ((ospvLossPacketsReceived >= 0) || (ospvLossFractionReceived >= 0)) {
            if (*ospvStats == OSPC_OSNULL) {
                *ospvStats = OSPPStatsNew();
                if (*ospvStats == OSPC_OSNULL) {
                    errorcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
                }
            }
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPStatsSetReceivedStatistics(*ospvStats, ospvLossPacketsReceived, ospvLossFractionReceived);
            }
        }
    }

    if ((errorcode != OSPC_ERR_NO_ERROR) && (*ospvStats != OSPC_OSNULL)) {
        OSPPStatsDelete(ospvStats);
    }

    return errorcode;
}

/* Set Sent statistics */
void OSPPStatsSetSentStatistics(
    OSPT_STATS *ospvStats,
    int ospvLossPacketsSent,
    int ospvLossFractionSent)
{
    if (ospvStats != OSPC_OSNULL) {
        if (ospvLossPacketsSent >= 0) {
            ospvStats->ospmLossSent.hasvalue |= OSPC_SVALUE_PACKETS;
            ospvStats->ospmLossSent.packets = ospvLossPacketsSent;
        }
        if (ospvLossFractionSent >= 0) {
            ospvStats->ospmLossSent.hasvalue |= OSPC_SVALUE_FRACTION;
            ospvStats->ospmLossSent.fraction = ospvLossFractionSent;
        }
    }
}

/* Set Received statistics */
void OSPPStatsSetReceivedStatistics(
    OSPT_STATS *ospvStats,
    int ospvLossPacketsReceived,
    int ospvLossFractionReceived)
{
    if (ospvStats != OSPC_OSNULL) {
        if (ospvLossPacketsReceived >= 0) {
            ospvStats->ospmLossReceived.hasvalue |= OSPC_SVALUE_PACKETS;
            ospvStats->ospmLossReceived.packets = ospvLossPacketsReceived;
        }
        if (ospvLossFractionReceived >= 0) {
            ospvStats->ospmLossReceived.hasvalue |= OSPC_SVALUE_FRACTION;
            ospvStats->ospmLossReceived.fraction = ospvLossFractionReceived;
        }
    }
}

/* Get value for OneWay.Samples */
unsigned OSPPStatsGetOneWaySamples(
    OSPT_STATS *ospvStats)
{
    unsigned samp = 0;

    if (ospvStats != OSPC_OSNULL) {
        samp = ospvStats->ospmOneWay.samples;
    }

    return samp;
}

/* Get value for OneWay.Minimum */
unsigned OSPPStatsGetOneWayMinimum(
    OSPT_STATS *ospvStats)
{
    unsigned min = 0;

    if (ospvStats != OSPC_OSNULL) {
        min = ospvStats->ospmOneWay.minimum;
    }

    return min;
}

/* Get value for OneWay.Mean */
unsigned OSPPStatsGetOneWayMean(
    OSPT_STATS *ospvStats)
{
    unsigned mean = 0;

    if (ospvStats != OSPC_OSNULL) {
        mean = ospvStats->ospmOneWay.mean;
    }

    return mean;
}

/* Get value for OneWay.Variance */
float OSPPStatsGetOneWayVariance(
    OSPT_STATS *ospvStats)
{
    float var = 0;

    if (ospvStats != OSPC_OSNULL) {
        var = ospvStats->ospmOneWay.variance;
    }

    return var;
}

/* Get value for RoundTrip.Samples */
unsigned OSPPStatsGetRoundTripSamples(
    OSPT_STATS *ospvStats)
{
    unsigned samp = 0;

    if (ospvStats != OSPC_OSNULL) {
        samp = ospvStats->ospmRoundTrip.samples;
    }

    return samp;
}

/* Get value for RoundTrip.Minimum */
unsigned OSPPStatsGetRoundTripMinimum(
    OSPT_STATS *ospvStats)
{
    unsigned min = 0;

    if (ospvStats != OSPC_OSNULL) {
        min = ospvStats->ospmRoundTrip.minimum;
    }

    return min;
}

/* Get value for RoundTrip.Mean */
unsigned OSPPStatsGetRoundTripMean(
    OSPT_STATS *ospvStats)
{
    unsigned mean = 0;

    if (ospvStats != OSPC_OSNULL) {
        mean = ospvStats->ospmRoundTrip.mean;
    }

    return mean;
}

/* Get value for RoundTrip.Variance */
float OSPPStatsGetRoundTripVariance(
    OSPT_STATS *ospvStats)
{
    float var = 0;

    if (ospvStats != OSPC_OSNULL) {
        var = ospvStats->ospmRoundTrip.variance;
    }

    return var;
}

/* Get value for packetsreceived */
unsigned OSPPStatsGetPktReceived(
    OSPT_STATS *ospvStats)
{
    unsigned pktsrecvd = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->ospmLossReceived.hasvalue & OSPC_SVALUE_PACKETS) != 0)) {
        pktsrecvd = ospvStats->ospmLossReceived.packets;
    }

    return pktsrecvd;
}

/* Get value for fractionreceived */
unsigned OSPPStatsGetFracReceived(
    OSPT_STATS *ospvStats)
{
    unsigned fracrecvd = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->ospmLossReceived.hasvalue & OSPC_SVALUE_FRACTION) != 0)) {
        fracrecvd = ospvStats->ospmLossReceived.fraction;
    }

    return fracrecvd;
}

/* Get value for packetssent */
unsigned OSPPStatsGetPktSent(
    OSPT_STATS *ospvStats)
{
    unsigned pktssent = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->ospmLossSent.hasvalue & OSPC_SVALUE_PACKETS) != 0)) {
        pktssent = ospvStats->ospmLossSent.packets;
    }

    return pktssent;
}

/* Get value for fractionsent */
unsigned OSPPStatsGetFracSent(
    OSPT_STATS *ospvStats)
{
    unsigned fracsent = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->ospmLossSent.hasvalue & OSPC_SVALUE_FRACTION) != 0)) {
        fracsent = ospvStats->ospmLossSent.fraction;
    }

    return fracsent;
}

/* OneWay to Element */
int OSPPStatsOneWayToElement(
    OSPT_STATS *ospvStats,
    OSPT_XML_ELEM **ospvElem)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvStats == OSPC_OSNULL) {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {

        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_ONEWAYDELAY), "");
        if (*ospvElem == OSPC_OSNULL) {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    /* Minimum */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetOneWayMinimum(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MINIMUM),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Mean */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetOneWayMean(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MEAN), &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Variance */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgFloatToElement(OSPPStatsGetOneWayVariance(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_VARIANCE),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Samples */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetOneWaySamples(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_SAMPLES),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if (attr != OSPC_OSNULL) {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if (elem != OSPC_OSNULL) {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

/* RoundTrip to Element */
int OSPPStatsRoundTripToElement(
    OSPT_STATS *ospvStats,
    OSPT_XML_ELEM **ospvElem)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvStats == OSPC_OSNULL) {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_ROUNDTRIPDELAY), "");
        if (*ospvElem == OSPC_OSNULL) {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    /* Minimum */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetRoundTripMinimum(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MINIMUM),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Mean */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetRoundTripMean(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MEAN), &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Variance */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgFloatToElement(OSPPStatsGetRoundTripVariance(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_VARIANCE),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Samples */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetRoundTripSamples(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_SAMPLES),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if (attr != OSPC_OSNULL) {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if (elem != OSPC_OSNULL) {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

/* Create loss sent element */
int OSPPStatsLossSentToElement(
    OSPT_STATS *ospvStats,
    OSPT_XML_ELEM **ospvElem)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvStats == OSPC_OSNULL) {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_LOSSSENT), "");
        if (*ospvElem == OSPC_OSNULL) {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    /* Loss Packets Sent */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasLossSent(ospvStats, OSPC_SVALUE_PACKETS))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetPktSent(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_PACKETS),
            &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Loss Fraction Sent */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasLossSent(ospvStats, OSPC_SVALUE_FRACTION))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetFracSent(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_FRACTION),
            &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if (attr != OSPC_OSNULL) {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if (elem != OSPC_OSNULL) {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

/* Create loss received element */
int OSPPStatsLossReceivedToElement(
    OSPT_STATS *ospvStats,
    OSPT_XML_ELEM **ospvElem)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvStats == OSPC_OSNULL) {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_LOSSRCVD), "");
        if (*ospvElem == OSPC_OSNULL) {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    /* Loss Packets Received */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasLossReceived(ospvStats, OSPC_SVALUE_PACKETS))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetPktReceived(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_PACKETS),
            &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Loss Fraction Received */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasLossReceived(ospvStats, OSPC_SVALUE_FRACTION))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetFracReceived(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_FRACTION),
            &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if (attr != OSPC_OSNULL) {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if (elem != OSPC_OSNULL) {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

OSPTBOOL OSPPStatsHasValue(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    unsigned ospvValueFlags)
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            ospvHas = ((ospvStats->ospmLost[ospvRange][ospvFlow].hasvalue & ospvValueFlags & OSPC_SVALUE_PACK) != 0);
            break;
        case OSPC_STATS_JITTER:
            ospvHas = ((ospvStats->ospmJitter[ospvRange][ospvFlow].hasvalue & ospvValueFlags & OSPC_SVALUE_METRICS) != 0);
            break;
        case OSPC_STATS_DELAY:
            ospvHas = ((ospvStats->ospmDelay[ospvRange][ospvFlow].hasvalue & ospvValueFlags & OSPC_SVALUE_METRICS) != 0);
            break;
        case OSPC_STATS_OCTETS:
            ospvHas = (ospvStats->ospmOctets[ospvRange][ospvFlow] >= 0);
            break;
        case OSPC_STATS_PACKETS:
            ospvHas = (ospvStats->ospmPackets[ospvRange][ospvFlow] >= 0);
            break;
        case OSPC_STATS_RFACTOR:
            ospvHas = (ospvStats->ospmRFactor[ospvRange][ospvFlow] >= 0);
            break;
        case OSPC_STATS_MOS:
            ospvHas = (ospvStats->ospmMOS[ospvRange][ospvFlow] >= 0);
            break;
        default:
            break;
        }
    }

    return ospvHas;
}

void OSPPStatsSetReporter(
    OSPT_STATS *ospvStats,
    OSPE_STATS_REPORTER ospvReporter)
{
    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvReporter == OSPC_SREPORTER_PROXY) || (ospvReporter == OSPC_SREPORTER_CALLING) || (ospvReporter == OSPC_SREPORTER_CALLED)))
    {
        ospvStats->ospmReporter = ospvReporter;
    }
}

void OSPPStatsSetPack(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    int ospvPackets,
    int ospvFraction)
{
    OSPT_STATS_PACK *ospvPack;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            ospvPack = &ospvStats->ospmLost[ospvRange][ospvFlow];
            break;
        default:
            return;
        }
        if (ospvPackets >= 0) {
            ospvPack->hasvalue |= OSPC_SVALUE_PACKETS;
            ospvPack->packets = ospvPackets;
        }
        if (ospvFraction >= 0) {
            ospvPack->hasvalue |= OSPC_SVALUE_FRACTION;
            ospvPack->fraction = ospvFraction;
        }
    }
}

void OSPPStatsSetMetrics(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    int ospvSamples,
    int ospvMinimum,
    int ospvMaximum,
    int ospvMean,
    float ospvVariance)
{
    OSPT_STATS_METRICS *metrics;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_JITTER:
            metrics = &ospvStats->ospmJitter[ospvRange][ospvFlow];
            break;
        case OSPC_STATS_DELAY:
            metrics = &ospvStats->ospmDelay[ospvRange][ospvFlow];
            break;
        default:
            return;
        }
        if (ospvSamples >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_SAMPLES;
            metrics->samples = ospvSamples;
        }
        if (ospvMinimum >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_MIN;
            metrics->minimum = ospvMinimum;
        }
        if (ospvMaximum >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_MAX;
            metrics->maximum = ospvMaximum;
        }
        if (ospvMean >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_MEAN;
            metrics->mean = ospvMean;
        }
        if (ospvVariance >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_VARIANCE;
            metrics->variance = ospvVariance;
        }
    }
}

void OSPPStatsSetInteger(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    int ospvValue)
{
    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_OCTETS:
            ospvStats->ospmOctets[ospvRange][ospvFlow] = ospvValue;
            break;
        case OSPC_STATS_PACKETS:
            ospvStats->ospmPackets[ospvRange][ospvFlow] = ospvValue;
            break;
        default:
            break;
        }
    }
}

void OSPPStatsSetFloat(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    float ospvValue)
{
    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_RFACTOR:
            ospvStats->ospmRFactor[ospvRange][ospvFlow] = ospvValue;
            break;
        case OSPC_STATS_MOS:
            ospvStats->ospmMOS[ospvRange][ospvFlow] = ospvValue;
            break;
        default:
            break;
        }
    }
}

OSPE_STATS_REPORTER OSPPStatsGetReporter(
    OSPT_STATS *ospvStats)
{
    OSPE_STATS_REPORTER ospvReporter = OSPC_SREPORTER_PROXY;

    if (ospvStats != OSPC_OSNULL) {
        ospvReporter = ospvStats->ospmReporter;
    }

    return ospvReporter;
}

void OSPPStatsGetPack(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    int *ospvPackets,
    int *ospvFraction)
{
    OSPT_STATS_PACK *ospvPack;

    *ospvPackets = -1;
    *ospvFraction = -1;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            ospvPack = &ospvStats->ospmLost[ospvRange][ospvFlow];
            break;
        default:
            return;
        }
        if ((ospvPack->hasvalue & OSPC_SVALUE_PACKETS) != 0) {
            *ospvPackets = ospvPack->packets;
        }
        if ((ospvPack->hasvalue & OSPC_SVALUE_FRACTION) != 0) {
            *ospvFraction = ospvPack->fraction;
        }
    }
}

void OSPPStatsGetMetrics(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    int *ospvSamples,
    int *ospvMinimum,
    int *ospvMaximum,
    int *ospvMean,
    float *ospvVariance)
{
    OSPT_STATS_METRICS *metrics;

    *ospvSamples = -1;
    *ospvMinimum = -1;
    *ospvMaximum = -1;
    *ospvMean = -1;
    *ospvVariance = -1;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_JITTER:
            metrics = &ospvStats->ospmJitter[ospvRange][ospvFlow];
            break;
        case OSPC_STATS_DELAY:
            metrics = &ospvStats->ospmDelay[ospvRange][ospvFlow];
            break;
        default:
            return;
        }
        if ((metrics->hasvalue & OSPC_SVALUE_SAMPLES) != 0) {
            *ospvSamples = metrics->samples;
        }
        if ((metrics->hasvalue & OSPC_SVALUE_MIN) != 0) {
            *ospvMinimum = metrics->minimum;
        }
        if ((metrics->hasvalue & OSPC_SVALUE_MAX) != 0) {
            *ospvMaximum = metrics->maximum;
        }
        if ((metrics->hasvalue & OSPC_SVALUE_MEAN) != 0) {
            *ospvMean = metrics->mean;
        }
        if ((metrics->hasvalue & OSPC_SVALUE_VARIANCE) != 0) {
            *ospvVariance = metrics->variance;
        }
    }
}

int OSPPStatsGetInteger(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow)
{
    int ospvValue = -1;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_OCTETS:
            ospvValue = ospvStats->ospmOctets[ospvRange][ospvFlow];
            break;
        case OSPC_STATS_PACKETS:
            ospvValue = ospvStats->ospmPackets[ospvRange][ospvFlow];
            break;
        default:
            break;
        }
    }

    return ospvValue;
}

float OSPPStatsGetFloat(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow)
{
    int ospvValue = -1;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvRange == OSPC_SRANGE_PEERPEER) || (ospvRange == OSPC_SRANGE_PEERPROXY) || (ospvRange == OSPC_SRANGE_PROXYPEER)) &&
        ((ospvFlow == OSPC_SFLOW_DOWNSTREAM) || (ospvFlow == OSPC_SFLOW_UPSTREAM)))
    {
        switch (ospvType) {
        case OSPC_STATS_RFACTOR:
            ospvValue = ospvStats->ospmRFactor[ospvRange][ospvFlow];
            break;
        case OSPC_STATS_MOS:
            ospvValue = ospvStats->ospmMOS[ospvRange][ospvFlow];
            break;
        default:
            break;
        }
    }

    return ospvValue;
}

int OSPPStatsValueToElement(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_RANGE ospvRange,
    OSPE_STATS_FLOW ospvFlow,
    OSPT_XML_ELEM **ospvElem)
{
    int error = OSPC_ERR_NO_ERROR;
    OSPE_STATS_STRUCT stype = -1;
    OSPE_MSG_ELEM stats = OSPC_MELEM_UNKNOWN;
    OSPE_ALTINFO range = OSPC_ALTINFO_UNKNOWN;
    OSPE_ALTINFO flow = OSPC_ALTINFO_UNKNOWN;
    OSPT_XML_ELEM *statselem = OSPC_OSNULL;
    OSPT_XML_ELEM *valueelem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    int packets;
    int fraction;
    int samples;
    int min;
    int max;
    int mean;
    float variance;
    int ivalue;
    float fvalue;

    if (ospvStats == OSPC_OSNULL) {
        error = OSPC_ERR_DATA_NO_STATS;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            stype = OSPC_SSTRUCT_PACK;
            stats = OSPC_MELEM_LOST;
            break;
        case OSPC_STATS_JITTER:
            stype = OSPC_SSTRUCT_METRICS;
            stats = OSPC_MELEM_JITTER;
            break;
        case OSPC_STATS_DELAY:
            stype = OSPC_SSTRUCT_METRICS;
            stats = OSPC_MELEM_DELAY;
            break;
        case OSPC_STATS_OCTETS:
            stype = OSPC_SSTRUCT_INTEGER;
            stats = OSPC_MELEM_OCTETS;
            break;
        case OSPC_STATS_PACKETS:
            stype = OSPC_SSTRUCT_INTEGER;
            stats = OSPC_MELEM_PACKETS;
            break;
        case OSPC_STATS_RFACTOR:
            stype = OSPC_SSTRUCT_FLOAT;
            stats = OSPC_MELEM_RFACTOR;
            break;
        case OSPC_STATS_MOS:
            stype = OSPC_SSTRUCT_FLOAT;
            stats = OSPC_MELEM_MOS;
            break;
        default:
            error = OSPC_ERR_DATA_INVALID_TYPE;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        switch (ospvRange) {
        case OSPC_SRANGE_PEERPEER:
            range = OSPC_ALTINFO_PEERPEER;
            break;
        case OSPC_SRANGE_PEERPROXY:
            range = OSPC_ALTINFO_PEERPROXY;
            break;
        case OSPC_SRANGE_PROXYPEER:
            range = OSPC_ALTINFO_PROXYPEER;
            break;
        default:
            error = OSPC_ERR_DATA_INVALID_TYPE;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        switch (ospvFlow) {
        case OSPC_SFLOW_DOWNSTREAM:
            flow = OSPC_ALTINFO_DOWNSTREAM;
            break;
        case OSPC_SFLOW_UPSTREAM:
            flow = OSPC_ALTINFO_UPSTREAM;
            break;
        default:
            error = OSPC_ERR_DATA_INVALID_TYPE;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        switch (stype) {
        case OSPC_SSTRUCT_PACK:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_PACK)) {
                OSPPStatsGetPack(ospvStats, ospvType, ospvRange, ospvFlow, &packets, &fraction);

                statselem = OSPPXMLElemNew(OSPPMsgElemGetName(stats), "");
                if (statselem == OSPC_OSNULL) {
                    error = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(flow));
                    if (attr != OSPC_OSNULL) {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ATTR;
                    }
                }

                /* Packets */
                if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_PACKETS)) {
                    error = OSPPMsgNumToElement(packets, OSPPMsgElemGetName(OSPC_MELEM_PACKETS), &valueelem);
                    if (error == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Fraction */
                if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_FRACTION)) {
                    error = OSPPMsgNumToElement(fraction, OSPPMsgElemGetName(OSPC_MELEM_FRACTION), &valueelem);
                    if (error == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }
            }
            break;
        case OSPC_SSTRUCT_METRICS:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_METRICS)) {
                OSPPStatsGetMetrics(ospvStats, ospvType, ospvRange, ospvFlow, &samples, &min, &max, &mean, &variance);

                statselem = OSPPXMLElemNew(OSPPMsgElemGetName(stats), "");
                if (statselem == OSPC_OSNULL) {
                    error = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(flow));
                    if (attr != OSPC_OSNULL) {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ATTR;
                    }
                }

                /* Samples */
                if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_SAMPLES)) {
                    error = OSPPMsgNumToElement(samples, OSPPMsgElemGetName(OSPC_MELEM_SAMPLES), &valueelem);
                    if ((error == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Minimum */
                if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_MIN)) {
                    error = OSPPMsgNumToElement(min, OSPPMsgElemGetName(OSPC_MELEM_MINIMUM), &valueelem);
                    if ((error == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Maximum */
                if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_MAX)) {
                    error = OSPPMsgNumToElement(max, OSPPMsgElemGetName(OSPC_MELEM_MAXIMUM), &valueelem);
                    if ((error == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Mean */
                if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_MEAN)) {
                    error = OSPPMsgNumToElement(mean, OSPPMsgElemGetName(OSPC_MELEM_MEAN), &valueelem);
                    if ((error == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Variance */
                if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_VARIANCE)) {
                    error = OSPPMsgFloatToElement(variance, OSPPMsgElemGetName(OSPC_MELEM_VARIANCE), &valueelem);
                    if ((error == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }
            }
            break;
        case OSPC_SSTRUCT_INTEGER:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_VALUE)) {
                ivalue = OSPPStatsGetInteger(ospvStats, ospvType, ospvRange, ospvFlow);

                error = OSPPMsgNumToElement(ivalue, OSPPMsgElemGetName(stats), &statselem);
                if ((error != OSPC_ERR_NO_ERROR) || (statselem == OSPC_OSNULL)) {
                    error = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(flow));
                    if (attr != OSPC_OSNULL) {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ATTR;
                    }
                }
            }
            break;
        case OSPC_SSTRUCT_FLOAT:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvRange, ospvFlow, OSPC_SVALUE_VALUE)) {
                fvalue = OSPPStatsGetFloat(ospvStats, ospvType, ospvRange, ospvFlow);

                error = OSPPMsgFloatToElement(fvalue, OSPPMsgElemGetName(stats), &statselem);
                if ((error != OSPC_ERR_NO_ERROR) || (statselem == OSPC_OSNULL)) {
                    error = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(flow));
                    if (attr != OSPC_OSNULL) {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    } else {
                        error = OSPC_ERR_XML_NO_ATTR;
                    }
                }
            }
            break;
        default:
            error = OSPC_ERR_DATA_INVALID_TYPE;
            break;
        }
    }

    if ((error == OSPC_ERR_NO_ERROR) && (statselem != OSPC_OSNULL)) {
        if (*ospvElem == OSPC_OSNULL) {
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_METRICS), "");
            if (*ospvElem == OSPC_OSNULL) {
                error = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(range));
                if (attr != OSPC_OSNULL) {
                    OSPPXMLElemAddAttr(*ospvElem, attr);
                    attr = OSPC_OSNULL;
                } else {
                    OSPPXMLElemDelete(ospvElem);
                    error = OSPC_ERR_XML_NO_ATTR;
                }
            }
        }
        if (error == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, statselem);
            statselem = OSPC_OSNULL;
        }
    }

    if (statselem != OSPC_OSNULL) {
        OSPPXMLElemDelete(&statselem);
    }

    if (valueelem != OSPC_OSNULL) {
        OSPPXMLElemDelete(&valueelem);
    }

    if (attr != OSPC_OSNULL) {
        OSPPXMLAttrDelete(&attr);
    }

    return error;
}
