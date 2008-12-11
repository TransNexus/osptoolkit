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
void
OSPPStatisticsDelete(
    OSPT_STATISTICS  **ospvStatistics
)
{
    if(*ospvStatistics != OSPC_OSNULL)
    {
        OSPM_FREE(*ospvStatistics);
        *ospvStatistics = OSPC_OSNULL;
    }

    return;
}

/* Get value for fractionreceived */
signed
OSPPStatisticsGetFracReceived(
    OSPT_STATISTICS  *ospvStatistics
)
{
    signed fracrecvd = 0;

    if(ospvStatistics != OSPC_OSNULL)
    {
        fracrecvd = ospvStatistics->ospmLossFractionReceived;
    }

    return fracrecvd;
}

/* Get value for OneWay.Minimum */
unsigned
OSPPStatisticsGetOneWayMinimum(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned min = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        min = ospvStatistics->ospmOneWay.Minimum;
    }

    return min;
}

/* Get value for OneWay.Mean */
unsigned
OSPPStatisticsGetOneWayMean(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned mean = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        mean = ospvStatistics->ospmOneWay.Mean;
    }

    return mean;
}

/* Get value for OneWay.Variance */
float
OSPPStatisticsGetOneWayVariance(
    OSPT_STATISTICS  *ospvStatistics
)
{
    float var = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        var = ospvStatistics->ospmOneWay.Variance;
    }

    return var;
}

/* Get value for OneWay.Samples */
unsigned
OSPPStatisticsGetOneWaySamples(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned samp = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        samp = ospvStatistics->ospmOneWay.NumberOfSamples;
    }

    return samp;
}

/* Get value for packetsreceived */
unsigned
OSPPStatisticsGetPktReceived(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned pktsrecvd = 0;

    if(ospvStatistics != OSPC_OSNULL)
    {
        pktsrecvd = ospvStatistics->ospmLossPacketsReceived;
    }

    return pktsrecvd;
}

/* Get value for fractionsent */
signed
OSPPStatisticsGetFracSent(
    OSPT_STATISTICS  *ospvStatistics
)
{
    signed fracsent = 0;

    if(ospvStatistics != OSPC_OSNULL)
    {
        fracsent = ospvStatistics->ospmLossFractionSent;
    }

    return fracsent;
}

/* Get value for packetssent */
unsigned
OSPPStatisticsGetPktSent(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned pktssent = 0;

    if(ospvStatistics != OSPC_OSNULL)
    {
        pktssent = ospvStatistics->ospmLossPacketsSent;
    }

    return pktssent;
}

/* Get value for RoundTrip.Minimum */
unsigned
OSPPStatisticsGetRoundTripMinimum(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned min = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        min = ospvStatistics->ospmRoundTrip.Minimum;
    }

    return min;
}

/* Get value for RoundTrip.Mean */
unsigned
OSPPStatisticsGetRoundTripMean(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned mean = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        mean = ospvStatistics->ospmRoundTrip.Mean;
    }

    return mean;
}

/* Get value for RoundTrip.Variance */
float
OSPPStatisticsGetRoundTripVariance(
    OSPT_STATISTICS  *ospvStatistics
)
{
    float var = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        var = ospvStatistics->ospmRoundTrip.Variance;
    }

    return var;
}

/* Get value for RoundTrip.Samples */
unsigned
OSPPStatisticsGetRoundTripSamples(
    OSPT_STATISTICS  *ospvStatistics
)
{
    unsigned samp = 0;
    if(ospvStatistics != OSPC_OSNULL)
    {
        samp = ospvStatistics->ospmRoundTrip.NumberOfSamples;
    }

    return samp;
}

/* Check for existence of LossReceived statistics */
OSPTBOOL
    OSPPStatisticsHasLossReceived(
    OSPT_STATISTICS *ospvStatistics
    )
{
    if(ospvStatistics != OSPC_OSNULL)
    {
        return ospvStatistics->ospmHasReceivedStats;
    }
    else
    {
        return OSPC_FALSE;
    }
}

/* Check for existence of LossSent statistics */
OSPTBOOL
    OSPPStatisticsHasLossSent(
    OSPT_STATISTICS *ospvStatistics
    )
{
    if(ospvStatistics != OSPC_OSNULL)
    {
        return ospvStatistics->ospmHasSentStats;
    }
    else
    {
        return OSPC_FALSE;
    }
}

/* Check for existence of OneWay statistics */
OSPTBOOL
    OSPPStatisticsHasOneWay(
    OSPT_STATISTICS *ospvStatistics
    )
{
    if(ospvStatistics != OSPC_OSNULL)
    {
        return ospvStatistics->ospmHasOneWay;
    }
    else
    {
        return OSPC_FALSE;
    }
}

/* Check for existence of RoundTrip statistics */
OSPTBOOL
    OSPPStatisticsHasRoundTrip(
    OSPT_STATISTICS *ospvStatistics
    )
{
    if(ospvStatistics != OSPC_OSNULL)
    {
        return ospvStatistics->ospmHasRoundTrip;
    }
    else
    {
        return OSPC_FALSE;
    }
}

/* Create loss received element */
int
OSPPStatisticsLossReceivedToElement(
    OSPT_STATISTICS *ospvStatistics, 
    OSPT_XML_ELEM    **ospvElem
)
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem       = OSPC_OSNULL;
    OSPT_XML_ATTR *attr       = OSPC_OSNULL;

    if(ospvElem == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if(ospvStatistics == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_LOSSRCVD), "");
        if(*ospvElem == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");

            if (attr != OSPC_OSNULL)
            {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }        
    }

    /* Loss Packets Received */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetPktReceived(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_PACKETS), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Loss Fraction Received */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetFracReceived(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_FRACTION), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if(attr != OSPC_OSNULL)
    {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if(elem != OSPC_OSNULL)
    {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

/* Create loss sent element */
int
OSPPStatisticsLossSentToElement(
    OSPT_STATISTICS *ospvStatistics, 
    OSPT_XML_ELEM    **ospvElem
)
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem       = OSPC_OSNULL;
    OSPT_XML_ATTR *attr       = OSPC_OSNULL;

    if(ospvElem == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if(ospvStatistics == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_LOSSSENT), "");
        if(*ospvElem == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");

            if (attr != OSPC_OSNULL)
            {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }        
    }

    /* Loss Packets Sent */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetPktSent(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_PACKETS), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Loss Fraction Sent */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetFracSent(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_FRACTION), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if(attr != OSPC_OSNULL)
    {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if(elem != OSPC_OSNULL)
    {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

/* Create new Statistics Structure */
OSPT_STATISTICS *
    OSPPStatisticsNew(void)
{
    OSPT_STATISTICS  *ospvStatistics = OSPC_OSNULL;

    OSPM_MALLOC(ospvStatistics, OSPT_STATISTICS, sizeof(OSPT_STATISTICS));

    if (ospvStatistics != OSPC_OSNULL)
    {
        OSPM_MEMSET(ospvStatistics, 0, sizeof(OSPT_STATISTICS));
    }

    return ospvStatistics;
}

/* OneWay to Element */
int
OSPPStatisticsOneWayToElement(
    OSPT_STATISTICS  *ospvStatistics,
    OSPT_XML_ELEM     **ospvElem
)
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem       = OSPC_OSNULL;
    OSPT_XML_ATTR *attr       = OSPC_OSNULL;

    if(ospvElem == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if(ospvStatistics == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_ONEWAYDELAY), "");
        if(*ospvElem == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");

            if (attr != OSPC_OSNULL)
            {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }        
    }

    /* Minimum */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetOneWayMinimum(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_MINIMUM), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Mean */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetOneWayMean(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_MEAN), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Variance */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgFloatToElement(OSPPStatisticsGetOneWayVariance(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_VARIANCE), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Samples */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetOneWaySamples(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_SAMPLES), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if(attr != OSPC_OSNULL)
    {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if(elem != OSPC_OSNULL)
    {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

/* RoundTrip to Element */
int
OSPPStatisticsRoundTripToElement(
    OSPT_STATISTICS  *ospvStatistics,
    OSPT_XML_ELEM     **ospvElem
)
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem       = OSPC_OSNULL;
    OSPT_XML_ATTR *attr       = OSPC_OSNULL;

    if(ospvElem == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if(ospvStatistics == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_ROUNDTRIPDELAY), ""); \
        if(*ospvElem == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");

            if (attr != OSPC_OSNULL)
            {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }        
    }

    /* Minimum */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetRoundTripMinimum(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_MINIMUM), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Mean */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetRoundTripMean(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_MEAN), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Variance */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgFloatToElement(OSPPStatisticsGetRoundTripVariance(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_VARIANCE), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* Samples */
    if(errorcode== OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgNumToElement(OSPPStatisticsGetRoundTripSamples(ospvStatistics),
            (const unsigned char *)OSPPMsgElemGetName(OSPC_MELEM_SAMPLES), &elem);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(elem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* clean up */
    if(attr != OSPC_OSNULL)
    {
        OSPM_FREE(attr);
        attr = OSPC_OSNULL;
    }

    if(elem != OSPC_OSNULL)
    {
        OSPM_FREE(elem);
        elem = OSPC_OSNULL;
    }

    return errorcode;
}

/* Report statistics for this transaction. */
int
OSPPStatisticsReportUsage(
    OSPT_STATISTICS  **ospvStatistics,           /* In - pter to place for stats
                                                        struct */
    unsigned        ospvLossPacketsSent,        /* In  */
    signed          ospvLossFractionSent,       /* In  */
    unsigned        ospvLossPacketsReceived,    /* In  */
    signed          ospvLossFractionReceived    /* In  */
)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;

    /* if LossFractionSent and received vars are negative, no statistics are 
     * reported. 
     * Just return with no error
     */
    if(ospvLossFractionSent >= 0)
    {

        if(*ospvStatistics == OSPC_OSNULL)
        {
            *ospvStatistics = OSPPStatisticsNew();

            if(*ospvStatistics == OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {

            OSPPStatisticsSetSentStatistics(*ospvStatistics, 
                ospvLossPacketsSent,
                ospvLossFractionSent);
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        if(ospvLossFractionReceived >= 0)
        {

            if(*ospvStatistics == OSPC_OSNULL)
            {
                *ospvStatistics = OSPPStatisticsNew();

                if(*ospvStatistics == OSPC_OSNULL)
                {
                    errorcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                OSPPStatisticsSetReceivedStatistics(*ospvStatistics, 
                    ospvLossPacketsReceived,
                    ospvLossFractionReceived);
            }

        }
    }

    if((errorcode != OSPC_ERR_NO_ERROR) &&
        (*ospvStatistics != OSPC_OSNULL))
    {
        OSPPStatisticsDelete(ospvStatistics);
    }

    return errorcode;
}

/* Set Received statistics */
void
OSPPStatisticsSetReceivedStatistics(
    OSPT_STATISTICS  *ospvStatistics,
    unsigned        ospvLossPacketsReceived,
    signed          ospvLossFractionReceived
)
{
    if(ospvStatistics != OSPC_OSNULL)
    {
        ospvStatistics->ospmHasReceivedStats = OSPC_TRUE;
        ospvStatistics->ospmLossPacketsReceived = ospvLossPacketsReceived;
        ospvStatistics->ospmLossFractionReceived = ospvLossFractionReceived;
    }

    return;
}

/* Set Sent statistics */
void
OSPPStatisticsSetSentStatistics(
    OSPT_STATISTICS  *ospvStatistics,
    unsigned        ospvLossPacketsSent,
    signed          ospvLossFractionSent
)
{
    if(ospvStatistics != OSPC_OSNULL)
    {
        ospvStatistics->ospmHasSentStats = OSPC_TRUE;
        ospvStatistics->ospmLossPacketsSent = ospvLossPacketsSent;
        ospvStatistics->ospmLossFractionSent = ospvLossFractionSent;
    }

    return;
}

/* Create xml element from statistics structure */
int
OSPPStatisticsToElement(
    OSPT_STATISTICS  *ospvStatistics,
    OSPT_XML_ELEM     **ospvElem
)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM     *elem       = OSPC_OSNULL;
    OSPT_XML_ATTR     *attr       = OSPC_OSNULL;

    if(ospvStatistics == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_DATA_NO_STATS;
    }

    if(ospvElem == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        /* create the Statistics element as the parent */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_STATISTICS), "");

        if(*ospvElem == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_XML_NO_ELEMENT;
        }
        else
        {
            attr = OSPPXMLAttrNew(
                (const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_CRITICAL),
                (const unsigned char *)"False");
            if (attr != OSPC_OSNULL) 
            {
                OSPPXMLElemAddAttr(*ospvElem, attr);
                attr = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_XML_NO_ATTR;
            }
        }
    }

    /* Now add the children */
    /* First LossSent */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        if(OSPPStatisticsHasLossSent(ospvStatistics))
        {

            errorcode = OSPPStatisticsLossSentToElement(ospvStatistics, &elem);
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* LossReceived */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        if(OSPPStatisticsHasLossReceived(ospvStatistics))
        {

            errorcode = OSPPStatisticsLossReceivedToElement(ospvStatistics, &elem);
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* OneWay */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        if(OSPPStatisticsHasOneWay(ospvStatistics))
        {

            errorcode = OSPPStatisticsOneWayToElement(ospvStatistics, &elem);
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* RoundTrip */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        if(OSPPStatisticsHasRoundTrip(ospvStatistics))
        {

            errorcode = OSPPStatisticsRoundTripToElement(ospvStatistics, &elem);
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if(errorcode != OSPC_ERR_NO_ERROR)
    {
        if(*ospvElem!= OSPC_OSNULL)
        {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    if(elem != OSPC_OSNULL)
    {
        OSPPXMLElemDelete(&elem);
    }

    if(attr != OSPC_OSNULL)
    {
        OSPPXMLAttrDelete(&attr);
    }

    return errorcode;
}
