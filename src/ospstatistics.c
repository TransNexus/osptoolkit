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

/* ospstatistics.c - OSP Statistics elements functions */

#include "osp/osp.h"
#include "osp/ospstatistics.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsg.h"

/* Create new Statistics Structure */
OSPT_STATS *OSPPStatsNew(void)
{
    OSPE_STATS_METRIC metric;
    OSPE_STATS_DIR dir;
    OSPT_STATS *ospvStats = OSPC_OSNULL;

    OSPM_MALLOC(ospvStats, OSPT_STATS, sizeof(OSPT_STATS));
    if (ospvStats != OSPC_OSNULL) {
        OSPM_MEMSET(ospvStats, 0, sizeof(OSPT_STATS));

        for (metric = OSPC_SMETRIC_START; metric < OSPC_SMETRIC_NUMBER; metric++) {
            for (dir = OSPC_SDIR_START; dir < OSPC_SDIR_NUMBER; dir++) {
                ospvStats->Octets[metric][dir] = -1;
                ospvStats->Packets[metric][dir] = -1;
            }
        }

        for (dir = OSPC_SDIR_START; dir < OSPC_SDIR_NUMBER; dir++) {
            ospvStats->RFactor[dir] = -1.0;
            ospvStats->MOSCQ[dir] = -1.0;
            ospvStats->MOSLQ[dir] = -1.0;
            ospvStats->ICPIF[dir] = -1;
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
    int errcode = OSPC_ERR_NO_ERROR;
    OSPE_STATS type;
    OSPE_STATS_METRIC metric;
    OSPE_STATS_DIR dir;
    OSPE_SESSION_LEG leg;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvStats == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NO_STATS;
    }

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the Statistics element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_STATISTICS), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
        }
    }

    /* Now add the children */
    /* First LossSent */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasLossSent(ospvStats, OSPC_SVALUE_PACKET)) {
            errcode = OSPPStatsLossSentToElement(ospvStats, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* LossReceived */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasLossReceived(ospvStats, OSPC_SVALUE_PACKET)) {
            errcode = OSPPStatsLossReceivedToElement(ospvStats, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* OneWay */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasOneWay(ospvStats, OSPC_SVALUE_METRICS)) {
            errcode = OSPPStatsOneWayToElement(ospvStats, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Lost/Jitter/Delay/RoundTripDelay/TotalOctets/TotalPackets */
    for (metric = OSPC_SMETRIC_START; (metric < OSPC_SMETRIC_NUMBER) && (errcode == OSPC_ERR_NO_ERROR); metric++) {
        for (type = OSPC_STATS_START; (type <= OSPC_STATS_NUMBER) && (errcode == OSPC_ERR_NO_ERROR); type++) {
            switch (type) {
            case OSPC_STATS_LOST:
            case OSPC_STATS_JITTER:
            case OSPC_STATS_DELAY:
            case OSPC_STATS_OCTETS:
            case OSPC_STATS_PACKETS:
                leg = OSPC_SLEG_UNDEFINED;
                for (dir = OSPC_SDIR_START; (dir < OSPC_SDIR_NUMBER) && (errcode == OSPC_ERR_NO_ERROR); dir++) {
                    errcode = OSPPStatsValueToElement(ospvStats, type, metric, dir, leg, &elem);
                }
                break;
            case OSPC_STATS_RTDELAY:
                dir = OSPC_SDIR_UNDEFINED;
                for (leg = OSPC_SLEG_START; (leg < OSPC_SLEG_NUMBER) && (errcode == OSPC_ERR_NO_ERROR); leg++) {
                    errcode = OSPPStatsValueToElement(ospvStats, type, metric, dir, leg, &elem);
                }
                break;
            case OSPC_STATS_RFACTOR:
            case OSPC_STATS_MOSCQ:
            case OSPC_STATS_MOSLQ:
            case OSPC_STATS_ICPIF:
            default:
                break;
            }
        }
        if ((errcode == OSPC_ERR_NO_ERROR) && (elem != OSPC_OSNULL)) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* RFactor/MOSCQ/MOSLQ/ICPIF */
    for (type = OSPC_STATS_START; (type <= OSPC_STATS_NUMBER) && (errcode == OSPC_ERR_NO_ERROR); type++) {
        switch (type) {
        case OSPC_STATS_RFACTOR:
        case OSPC_STATS_MOSCQ:
        case OSPC_STATS_MOSLQ:
        case OSPC_STATS_ICPIF:
            for (dir = OSPC_SDIR_START; (dir < OSPC_SDIR_NUMBER) && (errcode == OSPC_ERR_NO_ERROR); dir++) {
                errcode = OSPPStatsValueToElement(ospvStats, type, OSPC_SMETRIC_UNDEFINED, dir, OSPC_SLEG_UNDEFINED, &elem);
                if ((errcode == OSPC_ERR_NO_ERROR) && (elem != OSPC_OSNULL)) {
                    OSPPXMLElemAddChild(*ospvElem, elem);
                    elem = OSPC_OSNULL;
                }
            }
            break;
        case OSPC_STATS_LOST:
        case OSPC_STATS_JITTER:
        case OSPC_STATS_DELAY:
        case OSPC_STATS_OCTETS:
        case OSPC_STATS_PACKETS:
        case OSPC_STATS_RTDELAY:
        default:
            break;
        }
    }

    if (errcode != OSPC_ERR_NO_ERROR) {
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

    return errcode;
}

/* Check for existence of OneWay statistics */
OSPTBOOL OSPPStatsHasOneWay(
    OSPT_STATS *ospvStats,
    unsigned ospvValueFlags)
{
    if (ospvStats != OSPC_OSNULL) {
        return ((ospvStats->OneWay.hasvalue & ospvValueFlags & OSPC_SVALUE_METRICS) != 0);
    } else {
        return OSPC_FALSE;
    }
}

/* Check for existence of TwoWay statistics */
OSPTBOOL OSPPStatsHasTwoWay(
    OSPT_STATS *ospvStats,
    unsigned ospvValueFlags)
{
    if (ospvStats != OSPC_OSNULL) {
        return ((ospvStats->TwoWay.hasvalue & ospvValueFlags & OSPC_SVALUE_METRICS) != 0);
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
        return ((ospvStats->LossSent.hasvalue & ospvValueFlags & OSPC_SVALUE_PACKET) != 0);
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
        return ((ospvStats->LossReceived.hasvalue & ospvValueFlags & OSPC_SVALUE_PACKET) != 0);
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
            ospvStats->LossSent.hasvalue |= OSPC_SVALUE_PACKETS;
            ospvStats->LossSent.packets = ospvLossPacketsSent;
        }
        if (ospvLossFractionSent >= 0) {
            ospvStats->LossSent.hasvalue |= OSPC_SVALUE_FRACTION;
            ospvStats->LossSent.fraction = ospvLossFractionSent;
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
            ospvStats->LossReceived.hasvalue |= OSPC_SVALUE_PACKETS;
            ospvStats->LossReceived.packets = ospvLossPacketsReceived;
        }
        if (ospvLossFractionReceived >= 0) {
            ospvStats->LossReceived.hasvalue |= OSPC_SVALUE_FRACTION;
            ospvStats->LossReceived.fraction = ospvLossFractionReceived;
        }
    }
}

/* Get value for OneWay.Samples */
unsigned OSPPStatsGetOneWaySamples(
    OSPT_STATS *ospvStats)
{
    unsigned samp = 0;

    if (ospvStats != OSPC_OSNULL) {
        samp = ospvStats->OneWay.samples;
    }

    return samp;
}

/* Get value for OneWay.Minimum */
unsigned OSPPStatsGetOneWayMinimum(
    OSPT_STATS *ospvStats)
{
    unsigned min = 0;

    if (ospvStats != OSPC_OSNULL) {
        min = ospvStats->OneWay.minimum;
    }

    return min;
}

/* Get value for OneWay.Maximum */
unsigned OSPPStatsGetOneWayMaximum(
    OSPT_STATS *ospvStats)
{
    unsigned max = 0;

    if (ospvStats != OSPC_OSNULL) {
        max = ospvStats->OneWay.maximum;
    }

    return max;
}

/* Get value for OneWay.Mean */
unsigned OSPPStatsGetOneWayMean(
    OSPT_STATS *ospvStats)
{
    unsigned mean = 0;

    if (ospvStats != OSPC_OSNULL) {
        mean = ospvStats->OneWay.mean;
    }

    return mean;
}

/* Get value for OneWay.Variance */
float OSPPStatsGetOneWayVariance(
    OSPT_STATS *ospvStats)
{
    float var = 0;

    if (ospvStats != OSPC_OSNULL) {
        var = ospvStats->OneWay.variance;
    }

    return var;
}

/* Get value for TwoWay.Samples */
unsigned OSPPStatsGetTwoWaySamples(
    OSPT_STATS *ospvStats)
{
    unsigned samp = 0;

    if (ospvStats != OSPC_OSNULL) {
        samp = ospvStats->TwoWay.samples;
    }

    return samp;
}

/* Get value for TwoWay.Minimum */
unsigned OSPPStatsGetTwoWayMinimum(
    OSPT_STATS *ospvStats)
{
    unsigned min = 0;

    if (ospvStats != OSPC_OSNULL) {
        min = ospvStats->TwoWay.minimum;
    }

    return min;
}

/* Get value for TwoWay.Maximum */
unsigned OSPPStatsGetTwoWayMaximum(
    OSPT_STATS *ospvStats)
{
    unsigned max = 0;

    if (ospvStats != OSPC_OSNULL) {
        max = ospvStats->TwoWay.maximum;
    }

    return max;
}

/* Get value for TwoWay.Mean */
unsigned OSPPStatsGetTwoWayMean(
    OSPT_STATS *ospvStats)
{
    unsigned mean = 0;

    if (ospvStats != OSPC_OSNULL) {
        mean = ospvStats->TwoWay.mean;
    }

    return mean;
}

/* Get value for TwoWay.Variance */
float OSPPStatsGetTwoWayVariance(
    OSPT_STATS *ospvStats)
{
    float var = 0;

    if (ospvStats != OSPC_OSNULL) {
        var = ospvStats->TwoWay.variance;
    }

    return var;
}

/* Get value for packetsreceived */
unsigned OSPPStatsGetPktReceived(
    OSPT_STATS *ospvStats)
{
    unsigned pktsrecvd = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->LossReceived.hasvalue & OSPC_SVALUE_PACKETS) != 0)) {
        pktsrecvd = ospvStats->LossReceived.packets;
    }

    return pktsrecvd;
}

/* Get value for fractionreceived */
unsigned OSPPStatsGetFracReceived(
    OSPT_STATS *ospvStats)
{
    unsigned fracrecvd = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->LossReceived.hasvalue & OSPC_SVALUE_FRACTION) != 0)) {
        fracrecvd = ospvStats->LossReceived.fraction;
    }

    return fracrecvd;
}

/* Get value for packetssent */
unsigned OSPPStatsGetPktSent(
    OSPT_STATS *ospvStats)
{
    unsigned pktssent = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->LossSent.hasvalue & OSPC_SVALUE_PACKETS) != 0)) {
        pktssent = ospvStats->LossSent.packets;
    }

    return pktssent;
}

/* Get value for fractionsent */
unsigned OSPPStatsGetFracSent(
    OSPT_STATS *ospvStats)
{
    unsigned fracsent = 0;

    if ((ospvStats != OSPC_OSNULL) && ((ospvStats->LossSent.hasvalue & OSPC_SVALUE_FRACTION) != 0)) {
        fracsent = ospvStats->LossSent.fraction;
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
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
        }
    }

    /* Minimum */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasOneWay(ospvStats, OSPC_SVALUE_MINIMUM))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetOneWayMinimum(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MINIMUM),
            &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Maximum */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasOneWay(ospvStats, OSPC_SVALUE_MAXIMUM))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetOneWayMaximum(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MAXIMUM),
            &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Mean */
    if ((errorcode == OSPC_ERR_NO_ERROR)  && (OSPPStatsHasOneWay(ospvStats, OSPC_SVALUE_MEAN))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetOneWayMean(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MEAN), &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Variance */
    if ((errorcode == OSPC_ERR_NO_ERROR)  && (OSPPStatsHasOneWay(ospvStats, OSPC_SVALUE_VARIANCE))) {
        errorcode = OSPPMsgFloatToElement(OSPPStatsGetOneWayVariance(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_VARIANCE),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Samples */
    if ((errorcode == OSPC_ERR_NO_ERROR)  && (OSPPStatsHasOneWay(ospvStats, OSPC_SVALUE_SAMPLES))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetOneWaySamples(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_SAMPLES),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
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

/* TwoWay to Element */
int OSPPStatsTwoWayToElement(
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
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_TWOWAYDELAY), "");
        if (*ospvElem == OSPC_OSNULL) {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
        }
    }

    /* Minimum */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasTwoWay(ospvStats, OSPC_SVALUE_MINIMUM))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetTwoWayMinimum(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MINIMUM),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Maximum */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasTwoWay(ospvStats, OSPC_SVALUE_MAXIMUM))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetTwoWayMaximum(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MAXIMUM),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Mean */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasTwoWay(ospvStats, OSPC_SVALUE_MEAN))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetTwoWayMean(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_MEAN), &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Variance */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasTwoWay(ospvStats, OSPC_SVALUE_VARIANCE))) {
        errorcode = OSPPMsgFloatToElement(OSPPStatsGetTwoWayVariance(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_VARIANCE),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Samples */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (OSPPStatsHasTwoWay(ospvStats, OSPC_SVALUE_SAMPLES))) {
        errorcode = OSPPMsgNumToElement(OSPPStatsGetTwoWaySamples(ospvStats),
            OSPPMsgElemGetName(OSPC_MELEM_SAMPLES),
            &elem);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
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
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
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
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
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
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
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
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
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
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
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
            if (attr == OSPC_OSNULL) {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
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
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir,
    OSPE_SESSION_LEG ospvLeg,
    unsigned ospvValueFlags)
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvStats != OSPC_OSNULL) {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            if (((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
                ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
            {
                has = ((ospvStats->Lost[ospvMetric][ospvDir].hasvalue & ospvValueFlags & OSPC_SVALUE_PACKET) != 0);
            }
            break;
        case OSPC_STATS_JITTER:
            if (((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
                ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
            {
                has = ((ospvStats->Jitter[ospvMetric][ospvDir].hasvalue & ospvValueFlags & OSPC_SVALUE_METRICS) != 0);
            }
            break;
        case OSPC_STATS_DELAY:
            if (((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
                ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
            {
                has = ((ospvStats->Delay[ospvMetric][ospvDir].hasvalue & ospvValueFlags & OSPC_SVALUE_METRICS) != 0);
            }
            break;
        case OSPC_STATS_RTDELAY:
            if (((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
                ((ospvLeg >= OSPC_SLEG_START) && (ospvLeg < OSPC_SLEG_NUMBER)))
            {
               has = ((ospvStats->RTDelay[ospvMetric][ospvLeg].hasvalue & ospvValueFlags & OSPC_SVALUE_METRICS) != 0);
           }
           break;
        case OSPC_STATS_OCTETS:
            if (((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
                ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
            {
                has = (ospvStats->Octets[ospvMetric][ospvDir] >= 0);
            }
            break;
        case OSPC_STATS_PACKETS:
            if (((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
                ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
            {
                has = (ospvStats->Packets[ospvMetric][ospvDir] >= 0);
            }
            break;
        case OSPC_STATS_RFACTOR:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                has = (ospvStats->RFactor[ospvDir] >= 0);
            }
            break;
        case OSPC_STATS_MOSCQ:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                has = (ospvStats->MOSCQ[ospvDir] >= 0);
            }
            break;
        case OSPC_STATS_MOSLQ:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                has = (ospvStats->MOSLQ[ospvDir] >= 0);
            }
            break;
        case OSPC_STATS_ICPIF:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                has = (ospvStats->ICPIF[ospvDir] >= 0);
            }
            break;
        default:
            break;
        }
    }

    return has;
}

void OSPPStatsSetPacket(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir,
    int ospvPackets,
    int ospvFraction)
{
    OSPT_STATS_PACKET *ospvPacket;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
        ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            ospvPacket = &ospvStats->Lost[ospvMetric][ospvDir];
            break;
        default:
            return;
        }
        if (ospvPackets >= 0) {
            ospvPacket->hasvalue |= OSPC_SVALUE_PACKETS;
            ospvPacket->packets = ospvPackets;
        }
        if (ospvFraction >= 0) {
            ospvPacket->hasvalue |= OSPC_SVALUE_FRACTION;
            ospvPacket->fraction = ospvFraction;
        }
    }
}

void OSPPStatsSetMetrics(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir,
    OSPE_SESSION_LEG ospvLeg,
    int ospvSamples,
    int ospvMinimum,
    int ospvMaximum,
    int ospvMean,
    float ospvVariance)
{
    OSPT_STATS_METRICS *metrics = OSPC_OSNULL;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_JITTER:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                metrics = &ospvStats->Jitter[ospvMetric][ospvDir];
            }
            break;
        case OSPC_STATS_DELAY:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                metrics = &ospvStats->Delay[ospvMetric][ospvDir];
            }
            break;
        case OSPC_STATS_RTDELAY:
            if ((ospvLeg >= OSPC_SLEG_START) && (ospvLeg < OSPC_SLEG_NUMBER)) {
                metrics = &ospvStats->RTDelay[ospvMetric][ospvLeg];
            }
            break;
        default:
            break;
        }
    }

    if (metrics != OSPC_OSNULL) {
        if (ospvSamples >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_SAMPLES;
            metrics->samples = ospvSamples;
        }
        if (ospvMinimum >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_MINIMUM;
            metrics->minimum = ospvMinimum;
        }
        if (ospvMaximum >= 0) {
            metrics->hasvalue |= OSPC_SVALUE_MAXIMUM;
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
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir,
    int ospvValue)
{
    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_OCTETS:
            if ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) {
                ospvStats->Octets[ospvMetric][ospvDir] = ospvValue;
            }
            break;
        case OSPC_STATS_PACKETS:
            if ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) {
                ospvStats->Packets[ospvMetric][ospvDir] = ospvValue;
            }
            break;
        case OSPC_STATS_ICPIF:
            if (ospvMetric == OSPC_SMETRIC_UNDEFINED) {
                ospvStats->ICPIF[ospvDir] = ospvValue;
            }
            break;
        default:
            break;
        }
    }
}

void OSPPStatsSetFloat(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_DIR ospvDir,
    float ospvValue)
{
    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_RFACTOR:
            ospvStats->RFactor[ospvDir] = ospvValue;
            break;
        case OSPC_STATS_MOSCQ:
            ospvStats->MOSCQ[ospvDir] = ospvValue;
            break;
        case OSPC_STATS_MOSLQ:
            ospvStats->MOSLQ[ospvDir] = ospvValue;
            break;
        default:
            break;
        }
    }
}

void OSPPStatsGetPacket(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir,
    int *ospvPackets,
    int *ospvFraction)
{
    OSPT_STATS_PACKET *ospvPacket;

    *ospvPackets = -1;
    *ospvFraction = -1;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) &&
        ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            ospvPacket = &ospvStats->Lost[ospvMetric][ospvDir];
            break;
        default:
            return;
        }
        if ((ospvPacket->hasvalue & OSPC_SVALUE_PACKETS) != 0) {
            *ospvPackets = ospvPacket->packets;
        }
        if ((ospvPacket->hasvalue & OSPC_SVALUE_FRACTION) != 0) {
            *ospvFraction = ospvPacket->fraction;
        }
    }
}

void OSPPStatsGetMetrics(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir,
    OSPE_SESSION_LEG ospvLeg,
    int *ospvSamples,
    int *ospvMinimum,
    int *ospvMaximum,
    int *ospvMean,
    float *ospvVariance)
{
    OSPT_STATS_METRICS *metrics = OSPC_OSNULL;

    *ospvSamples = -1;
    *ospvMinimum = -1;
    *ospvMaximum = -1;
    *ospvMean = -1;
    *ospvVariance = -1.0;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_JITTER:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                metrics = &ospvStats->Jitter[ospvMetric][ospvDir];
            }
            break;
        case OSPC_STATS_DELAY:
            if ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)) {
                metrics = &ospvStats->Delay[ospvMetric][ospvDir];
            }
            break;
        case OSPC_STATS_RTDELAY:
            if ((ospvLeg >= OSPC_SLEG_START) && (ospvLeg < OSPC_SLEG_NUMBER)) {
                metrics = &ospvStats->RTDelay[ospvMetric][ospvLeg];
            }
            break;
        default:
            break;;
        }
    }

    if (metrics != OSPC_OSNULL) {
        if ((metrics->hasvalue & OSPC_SVALUE_SAMPLES) != 0) {
            *ospvSamples = metrics->samples;
        }
        if ((metrics->hasvalue & OSPC_SVALUE_MINIMUM) != 0) {
            *ospvMinimum = metrics->minimum;
        }
        if ((metrics->hasvalue & OSPC_SVALUE_MAXIMUM) != 0) {
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
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir)
{
    int ospvValue = -1;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_OCTETS:
            if ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) {
                ospvValue = ospvStats->Octets[ospvMetric][ospvDir];
            }
            break;
        case OSPC_STATS_PACKETS:
            if ((ospvMetric >= OSPC_SMETRIC_START) && (ospvMetric < OSPC_SMETRIC_NUMBER)) {
                ospvValue = ospvStats->Packets[ospvMetric][ospvDir];
            }
            break;
        case OSPC_STATS_ICPIF:
            if (ospvMetric == OSPC_SMETRIC_UNDEFINED) {
                ospvValue = ospvStats->ICPIF[ospvDir];
            }
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
    OSPE_STATS_DIR ospvDir)
{
    float ospvValue = -1.0;

    if ((ospvStats != OSPC_OSNULL) &&
        ((ospvDir >= OSPC_SDIR_START) && (ospvDir < OSPC_SDIR_NUMBER)))
    {
        switch (ospvType) {
        case OSPC_STATS_RFACTOR:
            ospvValue = ospvStats->RFactor[ospvDir];
            break;
        case OSPC_STATS_MOSCQ:
            ospvValue = ospvStats->MOSCQ[ospvDir];
            break;
        case OSPC_STATS_MOSLQ:
            ospvValue = ospvStats->MOSLQ[ospvDir];
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
    OSPE_STATS_METRIC ospvMetric,
    OSPE_STATS_DIR ospvDir,
    OSPE_SESSION_LEG ospvLeg,
    OSPT_XML_ELEM **ospvElem)
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPE_STATS_STRUCT stype = -1;
    OSPE_MSG_ELEM stats = OSPC_MELEM_UNKNOWN;
    OSPE_ALTINFO metric = OSPC_ALTINFO_UNKNOWN;
    OSPE_ALTINFO dir = OSPC_ALTINFO_UNKNOWN;
    OSPE_ALTINFO leg = OSPC_ALTINFO_UNKNOWN;
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
        errcode = OSPC_ERR_DATA_NO_STATS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (ospvMetric) {
        case OSPC_SMETRIC_UNDEFINED:
            metric = OSPC_ALTINFO_UNDEFINED;
            break;
        case OSPC_SMETRIC_RTP:
            metric = OSPC_ALTINFO_RTP;
            break;
        case OSPC_SMETRIC_RTCP:
            metric = OSPC_ALTINFO_RTCP;
            break;
        default:
            errcode = OSPC_ERR_DATA_INVALID_TYPE;
            break;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (ospvDir) {
        case OSPC_SDIR_UNDEFINED:
            dir = OSPC_ALTINFO_UNDEFINED;
            break;
        case OSPC_SDIR_SRCREP:
            dir = OSPC_ALTINFO_SRCREP;
            break;
        case OSPC_SDIR_REPSRC:
            dir = OSPC_ALTINFO_REPSRC;
            break;
        case OSPC_SDIR_REPDEST:
            dir = OSPC_ALTINFO_REPDEST;
            break;
        case OSPC_SDIR_DESTREP:
            dir = OSPC_ALTINFO_DESTREP;
            break;
        case OSPC_SDIR_SRCDEST:
            dir = OSPC_ALTINFO_SRCDEST;
            break;
        case OSPC_SDIR_DESTSRC:
            dir = OSPC_ALTINFO_DESTSRC;
            break;
        default:
            errcode = OSPC_ERR_DATA_INVALID_TYPE;
            break;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (ospvLeg) {
        case OSPC_SLEG_UNDEFINED:
            leg = OSPC_ALTINFO_UNKNOWN;
            break;
        case OSPC_SLEG_SOURCE:
            leg = OSPC_ALTINFO_SOURCE;
            break;
        case OSPC_SLEG_DESTINATION:
            leg = OSPC_ALTINFO_DESTINATION;
            break;
        default:
            errcode = OSPC_ERR_DATA_INVALID_TYPE;
            break;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (ospvType) {
        case OSPC_STATS_LOST:
            if ((metric == OSPC_ALTINFO_UNDEFINED) || (dir == OSPC_ALTINFO_UNDEFINED)) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_PACKET;
                stats = OSPC_MELEM_LOST;
            }
            break;
        case OSPC_STATS_JITTER:
            if ((metric == OSPC_ALTINFO_UNDEFINED) || (dir == OSPC_ALTINFO_UNDEFINED)) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_METRICS;
                stats = OSPC_MELEM_JITTER;
            }
            break;
        case OSPC_STATS_DELAY:
            if ((metric == OSPC_ALTINFO_UNDEFINED) || (dir == OSPC_ALTINFO_UNDEFINED)) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_METRICS;
                stats = OSPC_MELEM_DELAY;
            }
            break;
        case OSPC_STATS_RTDELAY:
            if ((metric ==OSPC_ALTINFO_UNDEFINED) || (leg == OSPC_ALTINFO_UNDEFINED)) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_METRICS;
                stats = OSPC_MELEM_RTDELAY;
            }
            break;
        case OSPC_STATS_OCTETS:
            if ((metric == OSPC_ALTINFO_UNDEFINED) || (dir == OSPC_ALTINFO_UNDEFINED)) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_INTEGER;
                stats = OSPC_MELEM_TOTALOCTETS;
            }
            break;
        case OSPC_STATS_PACKETS:
            if ((metric == OSPC_ALTINFO_UNDEFINED) || (dir == OSPC_ALTINFO_UNDEFINED)) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_INTEGER;
                stats = OSPC_MELEM_TOTALPACKETS;
            }
            break;
        case OSPC_STATS_RFACTOR:
            if (dir == OSPC_ALTINFO_UNDEFINED) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_FLOAT;
                stats = OSPC_MELEM_RFACTOR;
            }
            break;
        case OSPC_STATS_MOSCQ:
            if (dir == OSPC_ALTINFO_UNDEFINED) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_FLOAT;
                stats = OSPC_MELEM_MOSCQ;
            }
            break;
        case OSPC_STATS_MOSLQ:
            if (dir == OSPC_ALTINFO_UNDEFINED) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_FLOAT;
                stats = OSPC_MELEM_MOSLQ;
            }
            break;
        case OSPC_STATS_ICPIF:
            if (dir == OSPC_ALTINFO_UNDEFINED) {
                errcode = OSPC_ERR_DATA_INVALID_TYPE;
            } else {
                stype = OSPC_SSTRUCT_INTEGER;
                stats = OSPC_MELEM_ICPIF;
            }
            break;
        default:
            errcode = OSPC_ERR_DATA_INVALID_TYPE;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (stype) {
        case OSPC_SSTRUCT_PACKET:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_PACKET)) {
                OSPPStatsGetPacket(ospvStats, ospvType, ospvMetric, ospvDir, &packets, &fraction);

                statselem = OSPPXMLElemNew(OSPPMsgElemGetName(stats), "");
                if (statselem == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_DIR), OSPPAltInfoTypeGetName(dir));
                    if (attr == OSPC_OSNULL) {
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    }
                }

                /* Packets */
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_PACKETS)) {
                    errcode = OSPPMsgNumToElement(packets, OSPPMsgElemGetName(OSPC_MELEM_PACKETS), &valueelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Fraction */
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_FRACTION)) {
                    errcode = OSPPMsgNumToElement(fraction, OSPPMsgElemGetName(OSPC_MELEM_FRACTION), &valueelem);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }
            }
            break;
        case OSPC_SSTRUCT_METRICS:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_METRICS)) {
                OSPPStatsGetMetrics(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, &samples, &min, &max, &mean, &variance);

                statselem = OSPPXMLElemNew(OSPPMsgElemGetName(stats), "");
                if (statselem == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    switch (ospvType) {
                    case OSPC_STATS_JITTER:
                    case OSPC_STATS_DELAY:
                        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_DIR), OSPPAltInfoTypeGetName(dir));
                        break;
                    case OSPC_STATS_RTDELAY:
                        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(leg));
                        break;
                    default:
                        errcode = OSPC_ERR_DATA_INVALID_TYPE;
                        break;
                    }
                    if ((errcode == OSPC_ERR_NO_ERROR) && (attr != OSPC_OSNULL)) {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    }
                }

                /* Samples */
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_SAMPLES)) {
                    errcode = OSPPMsgNumToElement(samples, OSPPMsgElemGetName(OSPC_MELEM_SAMPLES), &valueelem);
                    if ((errcode == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Minimum */
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_MINIMUM)) {
                    errcode = OSPPMsgNumToElement(min, OSPPMsgElemGetName(OSPC_MELEM_MINIMUM), &valueelem);
                    if ((errcode == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Maximum */
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_MAXIMUM)) {
                    errcode = OSPPMsgNumToElement(max, OSPPMsgElemGetName(OSPC_MELEM_MAXIMUM), &valueelem);
                    if ((errcode == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Mean */
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_MEAN)) {
                    errcode = OSPPMsgNumToElement(mean, OSPPMsgElemGetName(OSPC_MELEM_MEAN), &valueelem);
                    if ((errcode == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }

                /* Variance */
                if ((errcode == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_VARIANCE)) {
                    errcode = OSPPMsgFloatToElement(variance, OSPPMsgElemGetName(OSPC_MELEM_VARIANCE), &valueelem);
                    if ((errcode == OSPC_ERR_NO_ERROR) && (valueelem != OSPC_OSNULL)) {
                        OSPPXMLElemAddChild(statselem, valueelem);
                        valueelem = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_XML_NO_ELEMENT;
                    }
                }
            }
            break;
        case OSPC_SSTRUCT_INTEGER:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_VALUE)) {
                ivalue = OSPPStatsGetInteger(ospvStats, ospvType, ospvMetric, ospvDir);

                errcode = OSPPMsgNumToElement(ivalue, OSPPMsgElemGetName(stats), &statselem);
                if ((errcode != OSPC_ERR_NO_ERROR) || (statselem == OSPC_OSNULL)) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_DIR), OSPPAltInfoTypeGetName(dir));
                    if (attr == OSPC_OSNULL) {
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    }
                }
            }
            break;
        case OSPC_SSTRUCT_FLOAT:
            if (OSPPStatsHasValue(ospvStats, ospvType, ospvMetric, ospvDir, ospvLeg, OSPC_SVALUE_VALUE)) {
                fvalue = OSPPStatsGetFloat(ospvStats, ospvType, ospvDir);

                errcode = OSPPMsgFloatToElement(fvalue, OSPPMsgElemGetName(stats), &statselem);
                if ((errcode != OSPC_ERR_NO_ERROR) || (statselem == OSPC_OSNULL)) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_DIR), OSPPAltInfoTypeGetName(dir));
                    if (attr == OSPC_OSNULL) {
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(statselem, attr);
                        attr = OSPC_OSNULL;
                    }
                }
            }
            break;
        default:
            errcode = OSPC_ERR_DATA_INVALID_TYPE;
            break;
        }
    }

    if ((errcode == OSPC_ERR_NO_ERROR) && (statselem != OSPC_OSNULL)) {
        if (metric == OSPC_ALTINFO_UNDEFINED) {
            if (*ospvElem != OSPC_OSNULL) {
                OSPPXMLElemDelete(ospvElem);
            }
            *ospvElem = statselem;
            statselem = OSPC_OSNULL;
        } else {
            if (*ospvElem == OSPC_OSNULL) {
                *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_METRICS), "");
                if (*ospvElem == OSPC_OSNULL) {
                    errcode = OSPC_ERR_XML_NO_ELEMENT;
                } else {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(metric));
                    if (attr == OSPC_OSNULL) {
                        OSPPXMLElemDelete(ospvElem);
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(*ospvElem, attr);
                        attr = OSPC_OSNULL;
                    }
                }
            }
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, statselem);
                statselem = OSPC_OSNULL;
            }
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

    return errcode;
}
