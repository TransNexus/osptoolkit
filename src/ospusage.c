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
 * ospusage.c - OSP usage detail functions
 */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospusage.h"
#include "osp/ospmsg.h"

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageFromElement() - get usage detail from an XML element
 *-----------------------------------------------------------------------*/

unsigned                          /* returns error code */
OSPPUsageFromElement(
    OSPTXMLELEM *ospvElem,        /* input is XML element */
    unsigned    *ospvUsage        /* where to put usage value */
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM  *elem = OSPC_OSNULL;
    unsigned      gotAmount = OSPC_FALSE;
    unsigned      gotIncrement = OSPC_FALSE;
    unsigned      gotUnit = OSPC_FALSE;
    unsigned long amount=0;
    unsigned long increment=0;

    if (ospvElem  == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvUsage == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_USAGE;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        for ( elem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != (OSPTXMLELEM *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPTXMLELEM *)OSPPXMLElemNextChild(ospvElem, elem) )
        {
            switch (OSPPMsgGetElemPart(OSPPXMLElemGetName(elem)))
            {
                case ospeElemAmount:
                gotAmount = OSPC_TRUE;
                ospvErrCode = OSPPMsgNumFromElement(elem, &amount);
                break;
                case ospeElemIncrement:
                ospvErrCode = OSPPMsgNumFromElement(elem, &increment);
                gotIncrement = OSPC_TRUE;
                break;
                case ospeElemService:
                /* we don't do anything with service at this point */
                break;
                case ospeElemUnit:
                gotUnit = OSPC_TRUE;
                /*
                 * The only unit we support is seconds. If this is
                 * different, then we declare an error.
                 */
                if (OSPM_STRCMP(OSPPXMLElemGetValue(elem), "s") != 0)
                {
                    ospvErrCode = OSPC_ERR_BAD_SERVICE;
                }
                break;
                default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem))
                {
                    ospvErrCode = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        if (gotAmount && gotIncrement && gotUnit)
        {
            *ospvUsage = (unsigned)increment * (unsigned)amount;
        }
        else
        {
            ospvErrCode = OSPC_ERR_XML_BAD_ELEMENT;
        }
    }

    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageToElement() - create an XML element from usage detail
 *-----------------------------------------------------------------------*/

unsigned                           /* returns error code */
OSPPUsageToElement(
    unsigned      ospvUsage,       /* usage value */
		OSPTTIME		  ospvStartTime,	 /* optional, if not 0, call start time */
    OSPTXMLELEM **ospvElem         /* where to put XML element pointer */
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM  *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemUsageDetail), "");
        if (*ospvElem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    /* now add the children - start with the service */
    /* the service is blank for basic service */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        elem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemService), "");
        if (elem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPXMLElemAddChild(*ospvElem, elem);
    }

    /* now add the amount (which is the usage) */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        ospvErrCode = OSPPMsgNumToElement(ospvUsage,
            (const unsigned char *)OSPPMsgGetElemName(ospeElemAmount), &elem);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPXMLElemAddChild(*ospvElem, elem);
    }

    /* now add the increment - for us always 1 */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        ospvErrCode = OSPPMsgNumToElement(1,
            (const unsigned char *)OSPPMsgGetElemName(ospeElemIncrement), &elem);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPXMLElemAddChild(*ospvElem, elem);
    }

    /* the units are seconds */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        elem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemUnit), "s");
        if (elem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPXMLElemAddChild(*ospvElem, elem);
    }

    /* optional (if not null) call start time */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvStartTime != 0))
    {
        ospvErrCode = OSPPMsgTimeToElement(ospvStartTime,
                                          (const unsigned char *)OSPPMsgGetElemName(ospeElemStartTime), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL))
    {
        OSPPXMLElemDelete(ospvElem);
    }

    return(ospvErrCode);
}


