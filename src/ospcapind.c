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
 * osptcapind.c
 */
#include "osp/osp.h"
#include "osp/osptrans.h"
#include "osp/ospprovider.h"
#include "osp/ospsecurity.h"
#include "osp/ospmime.h"
#include "osp/osputils.h"
#include "osp/ospmsgque.h"
#include "osp/ospmsginfo.h"
#include "osp/ospmsgattr.h"
#include "osp/ospxml.h"
#include "osp/ospxmlattr.h"
#include "osp/ospmsg.h"
#include "osp/ospcomm.h"
#include "osp/ospcapind.h"
#include "osp/ospfail.h"
#include "osp/ospaltinfo.h"


/*
 * release space and set the pointer to NULL
 */
void
OSPPCapIndDelete(
    OSPTCAPIND **ospvCapInd /* In - Release the space  */
)
{
    OSPTCAPIND  *pTmp    = OSPC_OSNULL;
    OSPTALTINFO *altinfo = OSPC_OSNULL;

    if (OSPC_OSNULL != *ospvCapInd)
    {
        pTmp = (*ospvCapInd);

        /* Release Message Id */
        if (OSPC_OSNULL != pTmp->ospmMessageId)
        {
            OSPM_FREE(pTmp->ospmMessageId);
        }
        
        /* Release Component Id */
        if (OSPC_OSNULL != pTmp->ospmComponentId)
        {
            OSPM_FREE(pTmp->ospmComponentId);
        }
            
        /* Release Source Alterntaes */
        if (OSPC_OSNULL != pTmp->ospmDeviceInfo)
        {
            while(!OSPPListEmpty(&(pTmp->ospmDeviceInfo)))
            {
                altinfo = (OSPTALTINFO *)OSPPListRemove(&(pTmp->ospmDeviceInfo));
                if(altinfo != OSPC_OSNULL)
                {
                    OSPPAltInfoDelete(&altinfo);
                }
            }  

            OSPPListDelete(&(pTmp->ospmDeviceInfo));
        }        
        
        /* Release Source Alterntaes */
        if (OSPC_OSNULL != pTmp->ospmSrcAlternate)
        {
            while(!OSPPListEmpty(&(pTmp->ospmSrcAlternate)))
            {
                altinfo = (OSPTALTINFO *)OSPPListRemove(&(pTmp->ospmSrcAlternate));
                if(altinfo != OSPC_OSNULL)
                {
                    OSPPAltInfoDelete(&altinfo);
                }
            }  

            OSPPListDelete(&(pTmp->ospmSrcAlternate));
        }        

        /* Release the structure itself */
        OSPM_FREE(pTmp);
            
        /* Reset the pointer to NULL */
        *ospvCapInd = OSPC_OSNULL;
    }
}




/*
 * create space and init capabiliti indication information.
 *
 * Check and store incoming data
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */
unsigned
OSPPCapIndNew(
    OSPTCAPIND **ospvCapInd,                /* Out - New CapInd object  */
    OSPTTRANS   *ospvTrans,                 /* In - Transaction Pointer */
    const char  *ospvSource,                /* In - Source of call      */
    const char  *ospvSourceDevice,          /* In - SourceDevice of call*/
    const char  *ospvSourceNetworkId,       /* In - NetworkId of call. Could be trunk grp*/
    unsigned     ospvAlmostOutOfResources)  /* In - Boolean indicator of availability */
{
    int         errorcode       = OSPC_ERR_NO_ERROR;
    OSPTALTINFO *altinfo        = OSPC_OSNULL;
    OSPTCAPIND  *capInd         = OSPC_OSNULL;

    /*
     * create the authorisation request object
     */
    OSPM_MALLOC(*ospvCapInd,OSPTCAPIND,sizeof(OSPTCAPIND));

    if (OSPC_OSNULL != *ospvCapInd)
    {
        capInd = (*ospvCapInd);
        OSPM_MEMSET(capInd,0,sizeof(OSPTCAPIND));
    }
    else
    {
        errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errorcode, "CapInd struct not created.");
    }
  
    /*
     * Set Time Stamp
     */
    if (OSPC_ERR_NO_ERROR == errorcode)
    {
        capInd->ospmTimestamp = time(OSPC_OSNULL);
    }

    /*
     * Set Component Id
     */
    if (OSPC_ERR_NO_ERROR == errorcode)
    {
        errorcode = OSPPGenerateUniqueId(ospvTrans,&(capInd->ospmComponentId));
    }

    /*
     * Set Message Id - same as component Id
     */
    if (OSPC_ERR_NO_ERROR == errorcode)
    {
        errorcode = OSPPGenerateUniqueId(ospvTrans,&(capInd->ospmMessageId));
    }
    
    /*
     * Initialize Source Alternates
     */
    if (OSPC_ERR_NO_ERROR == errorcode)
    {
        /* Add Source if it is present */
         if (OSPC_OSNULL != ospvSource && strlen(ospvSource) > 0)
        {
            /* Initialize the list */
            OSPPListNew(&(capInd->ospmSrcAlternate));

            altinfo = OSPPAltInfoNew(strlen(ospvSource), 
                                     (const unsigned char *)ospvSource,
                                      ospeTransport);

            if(OSPC_OSNULL != altinfo)
            {
                OSPPListAppend((OSPTLIST *)&(capInd->ospmSrcAlternate),
                                altinfo);
            }
        }
        
        /* Add SourceDevice if it is present */
         if (OSPC_OSNULL != ospvSourceDevice && strlen(ospvSourceDevice) > 0)
        {
            OSPPListNew(&(capInd->ospmDeviceInfo));
            altinfo = OSPPAltInfoNew(strlen(ospvSourceDevice), 
                                     (const unsigned char *)ospvSourceDevice,
                                      ospeTransport);

            if(OSPC_OSNULL != altinfo)
            {
                OSPPListAppend((OSPTLIST *)&(capInd->ospmDeviceInfo),
                                altinfo);
            }
        }

        /* Add Network info if it is present */
         if (OSPC_OSNULL != ospvSourceNetworkId && strlen(ospvSourceNetworkId) > 0)
        {
            /* Initialize the list only if the list has not been initialized above*/
            if (strlen(ospvSource)==0)
            {
                OSPPListNew(&(capInd->ospmSrcAlternate));
            }

            altinfo = OSPPAltInfoNew(strlen(ospvSourceNetworkId), 
                                     (const unsigned char *)ospvSourceNetworkId,
                                      ospeNetwork);

            if(OSPC_OSNULL != altinfo)
            {
                OSPPListAppend((OSPTLIST *)&(capInd->ospmSrcAlternate),
                                altinfo);
            }
        }
    }
    
    
    /*
     * Initialize the resource indicator
     */
    if (OSPC_ERR_NO_ERROR == errorcode)
    {
        capInd->ospmAlmostOutOfResources = ospvAlmostOutOfResources;
    }
    
    
    /*
     * On error, release any allocated space
     */
    if (OSPC_ERR_NO_ERROR != errorcode && OSPC_OSNULL != capInd)
    {
        OSPPCapIndDelete(ospvCapInd);
    }
    
    return errorcode;
}



/*
 * Generates a unique id without using a transaction id as a base.
 */
unsigned
OSPPGenerateUniqueId(
    OSPTTRANS      *ospvTrans,        /* In  - Transaction Pointer */
    unsigned char **ospvIdBuffer      /* Out - Buffer for storing the unique id */
)
{
    int  errorcode       = OSPC_ERR_NO_ERROR;
    int  numbytesrandom  = 0;
    int  numbytescounter = 0;
    char random[OSPC_MAX_RANDOM];
    char counter[OSPC_MAX_RANDOM];
    
    /*
     * Init the buffers to 0s
     */
    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);
    OSPM_MEMSET(counter,0, OSPC_MAX_RANDOM);
    
    /*
     * Generate random Number
     */
    numbytesrandom = OSPPUtilGetRandom(random, 0);
    
    /*
     * Get Transaction Counter
     */
    numbytescounter = OSPM_SPRINTF(counter, "%d", OSPPTransactionGetCounter(ospvTrans));
    
    /*
     * Allocate enough space for both random and counter
     */
    OSPM_MALLOC((*ospvIdBuffer), 
                unsigned char, 
                numbytesrandom + numbytescounter +1);
                
    if (OSPC_OSNULL != *ospvIdBuffer)
    { 
        /*
         * Init the buffer to 0s
         */
        OSPM_MEMSET((*ospvIdBuffer),
                    0, 
                    numbytesrandom + numbytescounter +1);
                    
        /*
         * Copy the randome number first
         */
        OSPM_MEMCPY((*ospvIdBuffer),
                    random, 
                    numbytesrandom);
        
        /*
         * Append the counter
         */
        OSPM_MEMCPY(((*ospvIdBuffer) + numbytesrandom), 
                    counter, 
                    numbytescounter);
        
        /*
         *  Update the Unique counter 
         */
        OSPPTransactionUpdateCounter(ospvTrans);
    }
    else
    {
        errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errorcode, "UniqueId buffer not created.");
    }
    
    return errorcode;
}




/*-----------------------------------------------------------------------*
 * OSPPCapIndToElement() - create an XML element from an authorisation request
 *-----------------------------------------------------------------------*/
int                                /* returns error code */
OSPPCapIndToElement(
    OSPTCAPIND   *ospvCapInd,      /* capability indication value */
    OSPTXMLELEM **ospvElem         /* where to put XML element pointer */
)
{
    int           ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM  *elem        = OSPC_OSNULL,
                 *subelem     = OSPC_OSNULL,
                 *capindelem  = OSPC_OSNULL;
    OSPTXMLATTR  *attr        = OSPC_OSNULL;
    OSPTALTINFO  *altinfo     = OSPC_OSNULL;
    char         random[OSPC_MAX_RANDOM];

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);


    /*
     * 
     *  Create the "Message" element as the parent 
     * 
     */
    *ospvElem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemMessage), "");

    /*
     * Create/Add messageId attribute
     */
    attr = OSPPXMLAttrNew(
                      (const unsigned char *)OSPPMsgGetAttrName(ospeAttrMessageId), 
                      ospvCapInd->ospmMessageId);
    OSPPXMLElemAddAttr(*ospvElem, attr);
    
    
    /*
     * Create/Add random attribute
     */
    OSPPUtilGetRandom(random, 0);
    attr = OSPPXMLAttrNew(
                      (const unsigned char *)OSPPMsgGetAttrName(ospeAttrRandom),
                      (const unsigned char *)random);
    OSPPXMLElemAddAttr(*ospvElem, attr);
    
    
    
    
    
    
    /*
     * 
     * Create/Add the CapabilitiesIndication element
     * 
     */
    capindelem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemCapInd), "");
    
    /*
     * Create/Add componentId attribute
     */
    attr = OSPPXMLAttrNew(
                      (const unsigned char *)OSPPMsgGetAttrName(ospeAttrComponentId), 
                      ospvCapInd->ospmComponentId);
    OSPPXMLElemAddAttr(capindelem, attr);
    
    /*
     * Add capabilities indication to the parent message
     */
    OSPPXMLElemAddChild(*ospvElem, capindelem);
    
    
    
    
     /*
     * 
     * Create/Add DeviceInfo elements
     * 
     */
    for(altinfo = (OSPTALTINFO *)OSPPListFirst( &(ospvCapInd->ospmDeviceInfo) );
        altinfo!= (OSPTALTINFO *)OSPC_OSNULL;
        altinfo = (OSPTALTINFO *)OSPPListNext( &(ospvCapInd->ospmDeviceInfo),altinfo))
    {
        OSPPAltInfoToElement(altinfo, &elem, ospeElemDeviceInfo);
        OSPPXMLElemAddChild(capindelem, elem);
    }
    
     /*
     * 
     * Create/Add SrcAltTransport elements
     * 
     */
    for(altinfo = (OSPTALTINFO *)OSPPListFirst( &(ospvCapInd->ospmSrcAlternate) );
        altinfo!= (OSPTALTINFO *)OSPC_OSNULL;
        altinfo = (OSPTALTINFO *)OSPPListNext( &(ospvCapInd->ospmSrcAlternate),altinfo))
    {
        OSPPAltInfoToElement(altinfo, &elem, ospeElemSrcAlt);
        OSPPXMLElemAddChild(capindelem, elem);
    }
    
    /*
     * 
     * Create/Add OSPVersion
     * 
     */
     elem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemDestOSPVersion),"2.1.1");
     OSPPXMLElemAddChild(capindelem, elem);
    
    
    
    /*
     * 
     * Create/Add Resources element
     * 
     */
     elem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemResources),"");
     OSPPXMLElemAddChild(capindelem, elem);

    /*
     * 
     * Create/Add AlmostOutOfResources sub-element
     * 
     */
     subelem = OSPPXMLElemNew(OSPPMsgGetElemName(ospeElemAlmostOutOfResources),
                              ospvCapInd->ospmAlmostOutOfResources==0?"false":"true");
     OSPPXMLElemAddChild(elem,subelem);



    return(ospvErrCode);
}

