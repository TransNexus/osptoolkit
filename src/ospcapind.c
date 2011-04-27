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

/* osptcapind.c */

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
void OSPPCapIndDelete(
    OSPT_CAP_IND **ospvCapInd)  /* In - Release the space  */
{
    OSPT_CAP_IND *pTmp = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (*ospvCapInd != OSPC_OSNULL) {
        pTmp = (*ospvCapInd);

        /* Release Message Id */
        if (pTmp->MessageId != OSPC_OSNULL) {
            OSPM_FREE(pTmp->MessageId);
        }

        /* Release Component Id */
        if (pTmp->ComponentId != OSPC_OSNULL) {
            OSPM_FREE(pTmp->ComponentId);
        }

        /* Release Source Alterntaes */
        if (pTmp->DeviceInfo != OSPC_OSNULL) {
            while (!OSPPListEmpty(&(pTmp->DeviceInfo))) {
                altinfo = (OSPT_ALTINFO *)OSPPListRemove(&(pTmp->DeviceInfo));
                if (altinfo != OSPC_OSNULL) {
                    OSPPAltInfoDelete(&altinfo);
                }
            }

            OSPPListDelete(&(pTmp->DeviceInfo));
        }

        /* Release Source Alterntaes */
        if (pTmp->SourceAlternate != OSPC_OSNULL) {
            while (!OSPPListEmpty(&(pTmp->SourceAlternate))) {
                altinfo = (OSPT_ALTINFO *)OSPPListRemove(&(pTmp->SourceAlternate));
                if (altinfo != OSPC_OSNULL) {
                    OSPPAltInfoDelete(&altinfo);
                }
            }

            OSPPListDelete(&(pTmp->SourceAlternate));
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
unsigned OSPPCapIndNew(
    OSPT_CAP_IND **ospvCapInd,          /* Out - New CapInd object  */
    OSPTTRANS *ospvTrans,               /* In - Transaction Pointer */
    const char *ospvSource,             /* In - Source of call      */
    const char *ospvSourceDevice,       /* In - SourceDevice of call */
    const char *ospvSourceNetworkId,    /* In - NetworkId of call. Could be trunk grp */
    unsigned ospvAlmostOutOfResources)  /* In - Boolean indicator of availability */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPT_CAP_IND *capInd = OSPC_OSNULL;

    /*
     * create the authorization request object
     */
    OSPM_MALLOC(*ospvCapInd, OSPT_CAP_IND, sizeof(OSPT_CAP_IND));

    if (*ospvCapInd != OSPC_OSNULL) {
        capInd = (*ospvCapInd);
        OSPM_MEMSET(capInd, 0, sizeof(OSPT_CAP_IND));
    } else {
        errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errorcode, "CapInd struct not created.");
    }

    /*
     * Set Time Stamp
     */
    if (OSPC_ERR_NO_ERROR == errorcode) {
        capInd->Timestamp = time(OSPC_OSNULL);
    }

    /*
     * Set Component Id
     */
    if (OSPC_ERR_NO_ERROR == errorcode) {
        errorcode = OSPPGenerateUniqueId(ospvTrans, &(capInd->ComponentId));
    }

    /*
     * Set Message Id - same as component Id
     */
    if (OSPC_ERR_NO_ERROR == errorcode) {
        errorcode = OSPPGenerateUniqueId(ospvTrans, &(capInd->MessageId));
    }

    /*
     * Initialize Source Alternates
     */
    if (OSPC_ERR_NO_ERROR == errorcode) {
        /* Add Source if it is present */
        if (ospvSource != OSPC_OSNULL && OSPM_STRLEN(ospvSource) > 0) {
            /* Initialize the list */
            OSPPListNew(&(capInd->SourceAlternate));
            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSource), ospvSource, OSPC_ALTINFO_TRANSPORT);
            if (altinfo != OSPC_OSNULL) {
                OSPPListAppend((OSPTLIST *)&(capInd->SourceAlternate), altinfo);
            }
        }

        /* Add SourceDevice if it is present */
        if (ospvSourceDevice != OSPC_OSNULL && OSPM_STRLEN(ospvSourceDevice) > 0) {
            OSPPListNew(&(capInd->DeviceInfo));
            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSourceDevice), ospvSourceDevice, OSPC_ALTINFO_TRANSPORT);
            if (altinfo != OSPC_OSNULL) {
                OSPPListAppend((OSPTLIST *)&(capInd->DeviceInfo), altinfo);
            }
        }

        /* Add Network info if it is present */
        if (ospvSourceNetworkId != OSPC_OSNULL && OSPM_STRLEN(ospvSourceNetworkId) > 0) {
            /* Initialize the list only if the list has not been initialized above */
            if (OSPM_STRLEN(ospvSource) == 0) {
                OSPPListNew(&(capInd->SourceAlternate));
            }
            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSourceNetworkId), ospvSourceNetworkId, OSPC_ALTINFO_NETWORK);
            if (altinfo != OSPC_OSNULL) {
                OSPPListAppend((OSPTLIST *)&(capInd->SourceAlternate), altinfo);
            }
        }
    }

    /*
     * Initialize the resource indicator
     */
    if (OSPC_ERR_NO_ERROR == errorcode) {
        capInd->AlmostOutOfResources = ospvAlmostOutOfResources;
    }

    /*
     * On error, release any allocated space
     */
    if (OSPC_ERR_NO_ERROR != errorcode && capInd != OSPC_OSNULL) {
        OSPPCapIndDelete(ospvCapInd);
    }

    return errorcode;
}

/*
 * Generates a unique id without using a transaction id as a base.
 */
unsigned OSPPGenerateUniqueId(
    OSPTTRANS *ospvTrans,       /* In  - Transaction Pointer */
    char **ospvIdBuffer)        /* Out - Buffer for storing the unique id */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    int numbytesrandom = 0;
    int numbytescounter = 0;
    char random[OSPC_MAX_RANDOM];
    char counter[OSPC_MAX_RANDOM];

    /*
     * Init the buffers to 0s
     */
    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);
    OSPM_MEMSET(counter, 0, OSPC_MAX_RANDOM);

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
    OSPM_MALLOC(*ospvIdBuffer, char, numbytesrandom + numbytescounter + 1);

    if (*ospvIdBuffer != OSPC_OSNULL) {
        /*
         * Init the buffer to 0s
         */
        OSPM_MEMSET((*ospvIdBuffer), 0, numbytesrandom + numbytescounter + 1);

        /*
         * Copy the randome number first
         */
        OSPM_MEMCPY((*ospvIdBuffer), random, numbytesrandom);

        /*
         * Append the counter
         */
        OSPM_MEMCPY(((*ospvIdBuffer) + numbytesrandom), counter, numbytescounter);

        /*
         *  Update the Unique counter
         */
        OSPPTransactionUpdateCounter(ospvTrans);
    } else {
        errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errorcode, "UniqueId buffer not created.");
    }

    return errorcode;
}

/*
 * OSPPCapIndToElement() - create an XML element from an authorization request
 */
int OSPPCapIndToElement(        /* returns error code */
    OSPT_CAP_IND *ospvCapInd,   /* capability indication value */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL, *subelem = OSPC_OSNULL, *capindelem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    char random[OSPC_MAX_RANDOM];

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    /*
     *  Create the "Message" element as the parent
     */
    *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_MESSAGE), "");

    /*
     * Create/Add messageId attribute
     */
    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_MESSAGEID), (const char *)ospvCapInd->MessageId);
    OSPPXMLElemAddAttr(*ospvElem, attr);

    /*
     * Create/Add random attribute
     */
    OSPPUtilGetRandom(random, 0);
    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_RANDOM), (const char *)random);
    OSPPXMLElemAddAttr(*ospvElem, attr);

    /*
     * Create/Add the CapabilitiesIndication element
     */
    capindelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CAPIND), "");

    /*
     * Create/Add componentId attribute
     */
    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_COMPONENTID), (const char *)ospvCapInd->ComponentId);
    OSPPXMLElemAddAttr(capindelem, attr);

    /*
     * Add capabilities indication to the parent message
     */
    OSPPXMLElemAddChild(*ospvElem, capindelem);

    /*
     * Create/Add DeviceInfo elements
     */
    for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvCapInd->DeviceInfo));
         altinfo != OSPC_OSNULL; altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvCapInd->DeviceInfo), altinfo)) {
        OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_DEVICEINFO);
        OSPPXMLElemAddChild(capindelem, elem);
    }

    /*
     * Create/Add SrcAltTransport elements
     */
    for (altinfo = (OSPT_ALTINFO *)OSPPListFirst(&(ospvCapInd->SourceAlternate));
         altinfo != OSPC_OSNULL; altinfo = (OSPT_ALTINFO *)OSPPListNext(&(ospvCapInd->SourceAlternate), altinfo)) {
        OSPPAltInfoToElement(altinfo, &elem, OSPC_MELEM_SRCALT);
        OSPPXMLElemAddChild(capindelem, elem);
    }

    /*
     * Create/Add OSPVersion
     */
    elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESTOSPVERSION), "2.1.1");
    OSPPXMLElemAddChild(capindelem, elem);

    /*
     * Create/Add Resources element
     */
    elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_RESOURCES), "");
    OSPPXMLElemAddChild(capindelem, elem);

    /*
     * Create/Add AlmostOutOfResources sub-element
     */
    subelem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_ALMOSTOUTOFRESOURCES), ospvCapInd->AlmostOutOfResources == 0 ? OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE) : OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRUE));
    OSPPXMLElemAddChild(elem, subelem);

    return ospvErrCode;
}
