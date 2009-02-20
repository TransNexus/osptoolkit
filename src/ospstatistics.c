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

/* Delete Statistics structure */
void OSPPStatsDelete(
    OSPT_STATS **ospvStats)
{
    if (*ospvStats != OSPC_OSNULL) {
        OSPM_FREE(*ospvStats);
        *ospvStats = OSPC_OSNULL;
    }
}

/* Get value for fractionreceived */
signed OSPPStatsGetFracReceived(
    OSPT_STATS *ospvStats)
{
    signed fracrecvd = 0;

    if (ospvStats != OSPC_OSNULL) {
        fracrecvd = ospvStats->ospmLossFractionReceived;
    }

    return fracrecvd;
}

/* Get value for OneWay.Minimum */
unsigned OSPPStatsGetOneWayMinimum(
    OSPT_STATS *ospvStats)
{
    unsigned min = 0;

    if (ospvStats != OSPC_OSNULL) {
        min = ospvStats->ospmOneWay.Minimum;
    }

    return min;
}

/* Get value for OneWay.Mean */
unsigned OSPPStatsGetOneWayMean(
    OSPT_STATS *ospvStats)
{
    unsigned mean = 0;

    if (ospvStats != OSPC_OSNULL) {
        mean = ospvStats->ospmOneWay.Mean;
    }

    return mean;
}

/* Get value for OneWay.Variance */
float OSPPStatsGetOneWayVariance(
    OSPT_STATS *ospvStats)
{
    float var = 0;

    if (ospvStats != OSPC_OSNULL) {
        var = ospvStats->ospmOneWay.Variance;
    }

    return var;
}

/* Get value for OneWay.Samples */
unsigned OSPPStatsGetOneWaySamples(
    OSPT_STATS *ospvStats)
{
    unsigned samp = 0;

    if (ospvStats != OSPC_OSNULL) {
        samp = ospvStats->ospmOneWay.Samples;
    }

    return samp;
}

/* Get value for packetsreceived */
unsigned OSPPStatsGetPktReceived(
    OSPT_STATS *ospvStats)
{
    unsigned pktsrecvd = 0;

    if (ospvStats != OSPC_OSNULL) {
        pktsrecvd = ospvStats->ospmLossPacketsReceived;
    }

    return pktsrecvd;
}

/* Get value for fractionsent */
signed OSPPStatsGetFracSent(
    OSPT_STATS *ospvStats)
{
    signed fracsent = 0;

    if (ospvStats != OSPC_OSNULL) {
        fracsent = ospvStats->ospmLossFractionSent;
    }

    return fracsent;
}

/* Get value for packetssent */
unsigned OSPPStatsGetPktSent(
    OSPT_STATS *ospvStats)
{
    unsigned pktssent = 0;

    if (ospvStats != OSPC_OSNULL) {
        pktssent = ospvStats->ospmLossPacketsSent;
    }

    return pktssent;
}

/* Get value for RoundTrip.Minimum */
unsigned OSPPStatsGetRoundTripMinimum(
    OSPT_STATS *ospvStats)
{
    unsigned min = 0;

    if (ospvStats != OSPC_OSNULL) {
        min = ospvStats->ospmRoundTrip.Minimum;
    }

    return min;
}

/* Get value for RoundTrip.Mean */
unsigned OSPPStatsGetRoundTripMean(
    OSPT_STATS *ospvStats)
{
    unsigned mean = 0;

    if (ospvStats != OSPC_OSNULL) {
        mean = ospvStats->ospmRoundTrip.Mean;
    }

    return mean;
}

/* Get value for RoundTrip.Variance */
float OSPPStatsGetRoundTripVariance(
    OSPT_STATS *ospvStats)
{
    float var = 0;
    
    if (ospvStats != OSPC_OSNULL) {
        var = ospvStats->ospmRoundTrip.Variance;
    }

    return var;
}

/* Get value for RoundTrip.Samples */
unsigned OSPPStatsGetRoundTripSamples(
    OSPT_STATS *ospvStats)
{
    unsigned samp = 0;

    if (ospvStats != OSPC_OSNULL) {
        samp = ospvStats->ospmRoundTrip.Samples;
    }

    return samp;
}

/* Check for existence of LossReceived statistics */
OSPTBOOL OSPPStatsHasLossReceived(
    OSPT_STATS *ospvStats)
{
    if (ospvStats != OSPC_OSNULL) {
        return ospvStats->ospmHasReceivedStats;
    } else {
        return OSPC_FALSE;
    }
}

/* Check for existence of LossSent statistics */
OSPTBOOL OSPPStatsHasLossSent(
    OSPT_STATS *ospvStats)
{
    if (ospvStats != OSPC_OSNULL) {
        return ospvStats->ospmHasSentStats;
    } else {
        return OSPC_FALSE;
    }
}

/* Check for existence of OneWay statistics */
OSPTBOOL OSPPStatsHasOneWay(
    OSPT_STATS *ospvStats)
{
    if (ospvStats != OSPC_OSNULL) {
        return ospvStats->ospmHasOneWay;
    } else {
        return OSPC_FALSE;
    }
}

/* Check for existence of RoundTrip statistics */
OSPTBOOL OSPPStatsHasRoundTrip(
    OSPT_STATS *ospvStats)
{
    if (ospvStats != OSPC_OSNULL) {
        return ospvStats->ospmHasRoundTrip;
    } else {
        return OSPC_FALSE;
    }
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
    if (errorcode == OSPC_ERR_NO_ERROR) {
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
    if (errorcode == OSPC_ERR_NO_ERROR) {
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
    if (errorcode == OSPC_ERR_NO_ERROR) {
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
    if (errorcode == OSPC_ERR_NO_ERROR) {
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

/* Create new Statistics Structure */
OSPT_STATS *OSPPStatsNew(void)
{
    OSPT_STATS *ospvStats = OSPC_OSNULL;

    OSPM_MALLOC(ospvStats, OSPT_STATS, sizeof(OSPT_STATS));

    if (ospvStats != OSPC_OSNULL) {
        OSPM_MEMSET(ospvStats, 0, sizeof(OSPT_STATS));
    }

    return ospvStats;
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

/* Report statistics for this transaction. */
int OSPPStatsReportUsage(
    OSPT_STATS **ospvStats,             /* In - pter to place for stats struct */
    unsigned ospvLossPacketsSent,       /* In  */
    signed ospvLossFractionSent,        /* In  */
    unsigned ospvLossPacketsReceived,   /* In  */
    signed ospvLossFractionReceived)    /* In  */
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /* 
     * If LossFractionSent and received vars are negative, no statistics are 
     * reported. 
     * Just return with no error
     */
    if (ospvLossFractionSent >= 0) {
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
        if (ospvLossFractionReceived >= 0) {
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

/* Set Received statistics */
void OSPPStatsSetReceivedStatistics(
    OSPT_STATS *ospvStats,
    unsigned ospvLossPacketsReceived,
    signed ospvLossFractionReceived)
{
    if (ospvStats != OSPC_OSNULL) {
        ospvStats->ospmHasReceivedStats = OSPC_TRUE;
        ospvStats->ospmLossPacketsReceived = ospvLossPacketsReceived;
        ospvStats->ospmLossFractionReceived = ospvLossFractionReceived;
    }
}

/* Set Sent statistics */
void OSPPStatsSetSentStatistics(
    OSPT_STATS *ospvStats,
    unsigned ospvLossPacketsSent,
    signed ospvLossFractionSent)
{
    if (ospvStats != OSPC_OSNULL) {
        ospvStats->ospmHasSentStats = OSPC_TRUE;
        ospvStats->ospmLossPacketsSent = ospvLossPacketsSent;
        ospvStats->ospmLossFractionSent = ospvLossFractionSent;
    }
}

/* Create xml element from statistics structure */
int OSPPStatsToElement(
    OSPT_STATS *ospvStats,
    OSPT_XML_ELEM **ospvElem)
{
    int errorcode = OSPC_ERR_NO_ERROR;
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
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    /* Now add the children */
    /* First LossSent */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasLossSent(ospvStats)) {
            errorcode = OSPPStatsLossSentToElement(ospvStats, &elem);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* LossReceived */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (OSPPStatsHasLossReceived(ospvStats)) {
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
    
    /* Inbound Delay */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPStatsValueToElement(ospvStats, OSPC_STATS_DELAY, OSPC_DIR_INBOUND, &elem);     
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* Outbound Delay */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPStatsValueToElement(ospvStats, OSPC_STATS_DELAY, OSPC_DIR_OUTBOUND, &elem);     
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* Inbound Jitter */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPStatsValueToElement(ospvStats, OSPC_STATS_JITTER, OSPC_DIR_INBOUND, &elem);    
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }
                            
    /* Outbound Jitter */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPStatsValueToElement(ospvStats, OSPC_STATS_JITTER, OSPC_DIR_OUTBOUND, &elem);    
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }
             
    /* Inbound PackLoss */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPStatsValueToElement(ospvStats, OSPC_STATS_PACKLOSS, OSPC_DIR_INBOUND, &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* Outbound PackLoss */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPStatsValueToElement(ospvStats, OSPC_STATS_PACKLOSS, OSPC_DIR_OUTBOUND, &elem);
        if (errorcode == OSPC_ERR_NO_ERROR) {
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

OSPTBOOL OSPPStatsHasValue(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType, 
    OSPE_DIRECTION ospvDirection,
    unsigned ospvValueFlags)
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvHas = ((ospvStats->ospmDelay[ospvDirection].HasValue & ospvValueFlags & OSPC_SVALUE_ALL) != 0);
            break;
        case OSPC_STATS_JITTER:
            ospvHas = ((ospvStats->ospmJitter[ospvDirection].HasValue & ospvValueFlags & OSPC_SVALUE_ALL) != 0);
            break;
        case OSPC_STATS_PACKLOSS:
            ospvHas = ((ospvStats->ospmPackLoss[ospvDirection].HasValue & ospvValueFlags & OSPC_SVALUE_ALL) != 0);
            break;
        default:
            break;
        }
    }
    
    return ospvHas;
}

unsigned OSPPStatsGetSamples(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection)
{
    unsigned ospvSamples = 0;
    
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvSamples = ospvStats->ospmDelay[ospvDirection].Samples;
            break;
        case OSPC_STATS_JITTER:
            ospvSamples = ospvStats->ospmJitter[ospvDirection].Samples;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvSamples = ospvStats->ospmPackLoss[ospvDirection].Samples;
            break;
        default:
            break;
        }
    }
    
    return ospvSamples;
}

unsigned OSPPStatsGetMin(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection)
{
    unsigned ospvMin = 0;
    
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvMin = ospvStats->ospmDelay[ospvDirection].Minimum;
            break;
        case OSPC_STATS_JITTER:
            ospvMin = ospvStats->ospmJitter[ospvDirection].Minimum;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvMin = ospvStats->ospmPackLoss[ospvDirection].Minimum;
            break;
        default:
            break;
        }
    }
    
    return ospvMin;
}

unsigned OSPPStatsGetMax(
    OSPT_STATS * ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection)
{
    unsigned ospvMax = 0;
    
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvMax = ospvStats->ospmDelay[ospvDirection].Maximum;
            break;
        case OSPC_STATS_JITTER:
            ospvMax = ospvStats->ospmJitter[ospvDirection].Maximum;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvMax = ospvStats->ospmPackLoss[ospvDirection].Maximum;
            break;
        default:
            break;
        }
    }
    
    return ospvMax;
}

unsigned OSPPStatsGetMean(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection)
{
    unsigned ospvMean = 0;
    
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvMean = ospvStats->ospmDelay[ospvDirection].Mean;
            break;
        case OSPC_STATS_JITTER:
            ospvMean = ospvStats->ospmJitter[ospvDirection].Mean;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvMean = ospvStats->ospmPackLoss[ospvDirection].Mean;
            break;
        default:
            break;
        }
    }
    
    return ospvMean;
}

float OSPPStatsGetVariance(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection)
{
    float ospvVariance = 0;
    
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvVariance = ospvStats->ospmDelay[ospvDirection].Variance;
            break;
        case OSPC_STATS_JITTER:
            ospvVariance = ospvStats->ospmJitter[ospvDirection].Variance;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvVariance = ospvStats->ospmPackLoss[ospvDirection].Variance;
            break;
        default:
            break;
        }
    }
    
    return ospvVariance;
}

double OSPPStatsGetSquaresSum(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection)
{
    float ospvSquaresSum = 0;
    
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvSquaresSum = ospvStats->ospmDelay[ospvDirection].SquaresSum;
            break;
        case OSPC_STATS_JITTER:
            ospvSquaresSum = ospvStats->ospmJitter[ospvDirection].SquaresSum;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvSquaresSum = ospvStats->ospmPackLoss[ospvDirection].SquaresSum;
            break;
        default:
            break;
        }
    }
    
    return ospvSquaresSum;
}

void OSPPStatsSetSamples(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection,
    unsigned ospvSamples)
{
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvStats->ospmDelay[ospvDirection].Samples = ospvSamples;
            ospvStats->ospmDelay[ospvDirection].HasValue |= OSPC_SVALUE_SAMPLES;
            break;
        case OSPC_STATS_JITTER:
            ospvStats->ospmJitter[ospvDirection].Samples = ospvSamples;
            ospvStats->ospmJitter[ospvDirection].HasValue |= OSPC_SVALUE_SAMPLES;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvStats->ospmPackLoss[ospvDirection].Samples = ospvSamples;
            ospvStats->ospmPackLoss[ospvDirection].HasValue |= OSPC_SVALUE_SAMPLES;
            break;
        default:
            break;
        }
    }
}

void OSPPStatsSetMin(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection,
    unsigned ospvMin)
{
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvStats->ospmDelay[ospvDirection].Minimum = ospvMin;
            ospvStats->ospmDelay[ospvDirection].HasValue |= OSPC_SVALUE_MIN;
            break;
        case OSPC_STATS_JITTER:
            ospvStats->ospmJitter[ospvDirection].Minimum = ospvMin;
            ospvStats->ospmJitter[ospvDirection].HasValue |= OSPC_SVALUE_MIN;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvStats->ospmPackLoss[ospvDirection].Minimum = ospvMin;
            ospvStats->ospmPackLoss[ospvDirection].HasValue |= OSPC_SVALUE_MIN;
            break;
        default:
            break;
        }
    }
}

void OSPPStatsSetMax(
    OSPT_STATS * ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection,
    unsigned ospvMax)
{
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvStats->ospmDelay[ospvDirection].Maximum = ospvMax;
            ospvStats->ospmDelay[ospvDirection].HasValue |= OSPC_SVALUE_MAX;
            break;
        case OSPC_STATS_JITTER:
            ospvStats->ospmJitter[ospvDirection].Maximum = ospvMax;
            ospvStats->ospmJitter[ospvDirection].HasValue |= OSPC_SVALUE_MAX;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvStats->ospmPackLoss[ospvDirection].Maximum = ospvMax;
            ospvStats->ospmPackLoss[ospvDirection].HasValue |= OSPC_SVALUE_MAX;
            break;
        default:
            break;
        }
    }
}

void OSPPStatsSetMean(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection,
    unsigned ospvMean)
{
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvStats->ospmDelay[ospvDirection].Mean = ospvMean;
            ospvStats->ospmDelay[ospvDirection].HasValue |= OSPC_SVALUE_MEAN;
            break;
        case OSPC_STATS_JITTER:
            ospvStats->ospmJitter[ospvDirection].Mean = ospvMean;
            ospvStats->ospmJitter[ospvDirection].HasValue |= OSPC_SVALUE_MEAN;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvStats->ospmPackLoss[ospvDirection].Mean = ospvMean;
            ospvStats->ospmPackLoss[ospvDirection].HasValue |= OSPC_SVALUE_MEAN;
            break;
        default:
            break;
        }
    }
}

void OSPPStatsSetVariance(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection,
    float ospvVariance)
{
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvStats->ospmDelay[ospvDirection].Variance = ospvVariance;
            ospvStats->ospmDelay[ospvDirection].HasValue |= OSPC_SVALUE_VARIANCE;
            break;
        case OSPC_STATS_JITTER:
            ospvStats->ospmJitter[ospvDirection].Variance = ospvVariance;
            ospvStats->ospmJitter[ospvDirection].HasValue |= OSPC_SVALUE_VARIANCE;
            break;
        case OSPC_STATS_PACKLOSS:
            ospvStats->ospmPackLoss[ospvDirection].Variance = ospvVariance;
            ospvStats->ospmPackLoss[ospvDirection].HasValue |= OSPC_SVALUE_VARIANCE;
            break;
        default:
            break;
        }
    }
}

void OSPPStatsSetSquaresSum(
    OSPT_STATS *ospvStats, 
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection,
    double ospvSquaresSum)
{
    if ((ospvStats != OSPC_OSNULL) && ((ospvDirection == OSPC_DIR_INBOUND) || (ospvDirection == OSPC_DIR_OUTBOUND))) {
        switch (ospvType) {
        case OSPC_STATS_DELAY:
            ospvStats->ospmDelay[ospvDirection].SquaresSum = ospvSquaresSum;
            ospvStats->ospmDelay[ospvDirection].HasValue = OSPC_SVALUE_SQUARES; 
            break;
        case OSPC_STATS_JITTER:
            ospvStats->ospmJitter[ospvDirection].SquaresSum = ospvSquaresSum;
            ospvStats->ospmJitter[ospvDirection].HasValue = OSPC_SVALUE_SQUARES; 
            break;
        case OSPC_STATS_PACKLOSS:
            ospvStats->ospmPackLoss[ospvDirection].SquaresSum = ospvSquaresSum;
            ospvStats->ospmPackLoss[ospvDirection].HasValue = OSPC_SVALUE_SQUARES; 
            break;
        default:
            break;
        }
    }
}

int OSPPStatsValueToElement(
    OSPT_STATS *ospvStats,
    OSPE_STATS ospvType,
    OSPE_DIRECTION ospvDirection,
    OSPT_XML_ELEM **ospvElem)
{
    int error = OSPC_ERR_NO_ERROR;
    OSPE_MSG_ELEM etype = OSPC_MELEM_UNKNOWN;
    OSPE_ALTINFO atype = OSPC_ALTINFO_UNKNOWN;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvStats == OSPC_OSNULL) {
        error = OSPC_ERR_DATA_NO_STATS;
    } 
    
    if (error == OSPC_ERR_NO_ERROR) { 
        if (ospvType == OSPC_STATS_DELAY) {
            etype = OSPC_MELEM_DELAY;
        } else if (ospvType == OSPC_STATS_JITTER) {
            etype = OSPC_MELEM_JITTER;
        } else if (ospvType == OSPC_STATS_PACKLOSS) {
            etype = OSPC_MELEM_PACKLOSS;
        } else {
            error = OSPC_ERR_DATA_INVALID_TYPE;
        }
    }
    
    if (error == OSPC_ERR_NO_ERROR) { 
        if (ospvDirection == OSPC_DIR_INBOUND) {
            atype = OSPC_ALTINFO_INBOUND;
        } else if (ospvDirection == OSPC_DIR_OUTBOUND) {
            atype = OSPC_ALTINFO_OUTBOUND;
        } else {
            error = OSPC_ERR_DATA_INVALID_TYPE;
        }
    }

    if ((error == OSPC_ERR_NO_ERROR) && (ospvElem == OSPC_OSNULL)) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvDirection, OSPC_SVALUE_ALL)) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(etype), "");
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(atype));
            if (attr != OSPC_OSNULL) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            } else {
                error = OSPC_ERR_XML_NO_ATTR;
            }
        }

        /* Samples */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvDirection, OSPC_SVALUE_SAMPLES)) {
            error = OSPPMsgNumToElement(OSPPStatsGetSamples(ospvStats, ospvType, ospvDirection), 
                OSPPMsgElemGetName(OSPC_MELEM_SAMPLES), &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            } else {
                error = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    
        /* Minimum */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvDirection, OSPC_SVALUE_MIN)) {
            error = OSPPMsgNumToElement(OSPPStatsGetMin(ospvStats, ospvType, ospvDirection), 
                OSPPMsgElemGetName(OSPC_MELEM_MINIMUM), &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            } else {
                error = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    
        /* Maximum */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvDirection, OSPC_SVALUE_MAX)) {
            error = OSPPMsgNumToElement(OSPPStatsGetMax(ospvStats, ospvType, ospvDirection), 
                OSPPMsgElemGetName(OSPC_MELEM_MAXIMUM), &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            } else {
                error = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    
        /* Mean */
        if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvDirection, OSPC_SVALUE_MEAN)) {
            error = OSPPMsgNumToElement(OSPPStatsGetMean(ospvStats, ospvType, ospvDirection), 
                OSPPMsgElemGetName(OSPC_MELEM_MEAN), &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            } else {
                error = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    
        if ((error == OSPC_ERR_NO_ERROR) && OSPPStatsHasValue(ospvStats, ospvType, ospvDirection, OSPC_SVALUE_VARIANCE)) {
            error = OSPPMsgFloatToElement(OSPPStatsGetVariance(ospvStats, ospvType, ospvDirection), 
                OSPPMsgElemGetName(OSPC_MELEM_VARIANCE), &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            } else {
                error = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    
        if (error != OSPC_ERR_NO_ERROR) {
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
    }
    
    return error;
}

