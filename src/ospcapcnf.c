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
 * osptcapncf.cpp 
 */
#include "osp.h"
#include "osptrans.h"
#include "ospprovider.h"
#include "ospsecurity.h"
#include "ospmime.h"
#include "osputils.h"
#include "ospmsgque.h"
#include "ospmsginfo.h"
#include "ospmsgattr.h"
#include "ospxml.h"
#include "ospxmlattr.h"
#include "ospmsg.h"
#include "ospcomm.h"
#include "ospcapind.h"
#include "ospfail.h"
#include "ospaltinfo.h"



unsigned
OSPPCapCnfNew(OSPTCAPCNF **ospvCapCnf)
{
    unsigned errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC(*ospvCapCnf, OSPTCAPCNF,sizeof(OSPTCAPCNF));

    if (OSPC_OSNULL != *ospvCapCnf)
    {
        OSPM_MEMSET(*ospvCapCnf,0,sizeof(OSPTCAPCNF));
    }
    else
    {
        errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errorcode, "CapCnf struct not created.");
    }

    return errorcode;
}





void
OSPPCapCnfDelete(OSPTCAPCNF **ospvCapCnf)
{
    if (OSPC_OSNULL != ospvCapCnf && OSPC_OSNULL != *ospvCapCnf)
    {
        OSPPStatusDelete( &((*ospvCapCnf)->ospmStatus) );

        OSPM_FREE( *ospvCapCnf );

        *ospvCapCnf = OSPC_OSNULL;
    }
}


unsigned                          /* returns error code */
OSPPCapCnfFromElement(
    OSPTXMLELEM *ospvParent,      /* input is XML element */
    OSPTCAPCNF **ospvCapCnf       /* where to put capability confirmation pointer */
)
{
    unsigned      ospvErrCode   = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM  *elem          = OSPC_OSNULL;
    OSPTXMLELEM  *capCnfElem    = OSPC_OSNULL;
    OSPTCAPCNF   *capcnf        = OSPC_OSNULL;
    OSPTTIME      t             = 0L;
    int           len           = 0;
    unsigned char *compid       = OSPC_OSNULL;
    unsigned char *messageId    = OSPC_OSNULL;

    /*
     * Check input data
     */
    if (OSPC_OSNULL == ospvParent)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    /*
     * Create new confirmation object
     */
    if (OSPC_ERR_NO_ERROR == ospvErrCode)
    {
        ospvErrCode = OSPPCapCnfNew(&capcnf);
    }



    if (OSPC_ERR_NO_ERROR == ospvErrCode)
    {
        /*
         * The Parent points to Message, its 1st child is 
         * CapabilityConfirmation
         */
        capCnfElem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvParent);

        /*
         * The Confirmation element should consist of several
         * child elements. We'll run through what's there and pick out
         * the information we need.
         */
        for (elem  = (OSPTXMLELEM *)OSPPXMLElemFirstChild(capCnfElem);
            (elem != (OSPTXMLELEM *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
             elem = (OSPTXMLELEM *)OSPPXMLElemNextChild(capCnfElem,elem))
        {
            switch (OSPPMsgGetElemPart(OSPPXMLElemGetName(elem)))
            {
                case ospeElemTimestamp:
                case ospeElemOSPVersion:
                    /* For now, we aren't interested in the value */
                    break;

                case ospeElemStatus:
                    ospvErrCode = OSPPStatusFromElement(elem, &(capcnf->ospmStatus));
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
        } /* For each child element of CapabilitiesConfirmation */
    }




    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        *ospvCapCnf = capcnf;
    }
    
    return(ospvErrCode);
}

