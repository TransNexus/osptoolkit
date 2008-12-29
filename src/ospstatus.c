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
 * ospstatus.c - OSP status functions
 */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/osplist.h"
#include "osp/ospxmldoc.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"
#include "osp/ospstatus.h"


/**/
/*-----------------------------------------------------------------------*
 * OSPPStatusNew() - create a new status object
 *-----------------------------------------------------------------------*/

OSPTSTATUS *                           /* returns ptr to status or null */
    OSPPStatusNew(void)
{
    OSPTSTATUS     *ospvStatus = OSPC_OSNULL;

    /* try to allocate the memory for the entire object */
    OSPM_MALLOC(ospvStatus, OSPTSTATUS,sizeof(*ospvStatus));
    if(ospvStatus!=OSPC_OSNULL)
    {
        /* !!!PS Initialize it */
        OSPM_MEMSET(ospvStatus, 0, sizeof(*ospvStatus));
        ospvStatus->ospmStatusDesc=OSPC_OSNULL;
    }
    return(ospvStatus);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPStatusDelete() - destroy a status object
 *-----------------------------------------------------------------------*/
void                                  /* no return */
OSPPStatusDelete(
    OSPTSTATUS **ospvStatus            /* Status to destroy */
)
{
    if (*ospvStatus != OSPC_OSNULL)
    {
        if ((*ospvStatus)->ospmStatusDesc != OSPC_OSNULL ) 
        {
            OSPM_FREE((*ospvStatus)->ospmStatusDesc);
            (*ospvStatus)->ospmStatusDesc = OSPC_OSNULL;
        }
        OSPM_FREE(*ospvStatus);
        *ospvStatus = OSPC_OSNULL;
    }
}

/*
 * OSPPStatusSetDesc() - set the status description
 */
void OSPPStatusSetDesc(         /* no return */
    OSPTSTATUS *ospvStatus,     /* Status to set */
    const char *ospvDesc)
{
    if (ospvStatus != OSPC_OSNULL) 
    {
        if (ospvStatus->ospmStatusDesc != OSPC_OSNULL)
        {
            OSPM_FREE(ospvStatus->ospmStatusDesc);
            ospvStatus->ospmStatusDesc = OSPC_OSNULL;
        }
        OSPM_MALLOC(ospvStatus->ospmStatusDesc, char, OSPM_STRLEN(ospvDesc)+1);
        if (ospvStatus->ospmStatusDesc != OSPC_OSNULL)
        {
            OSPM_MEMCPY((ospvStatus->ospmStatusDesc), ospvDesc, OSPM_STRLEN(ospvDesc) + 1);
            ospvStatus->ospmHasDesc = OSPC_TRUE;
        }
    } 
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPStatusHasCode() - is the status code populated
 *-----------------------------------------------------------------------*/
unsigned                              /* return 1 or 0 */
OSPPStatusHasCode(
    OSPTSTATUS *ospvStatus            /* Status to check */
)
{
    unsigned hascode = OSPC_FALSE;

    if (ospvStatus != OSPC_OSNULL) 
    {
        hascode=ospvStatus->ospmHasCode;
    }
    return hascode; 
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPStatusGetCode() - get the status code
 *-----------------------------------------------------------------------*/
unsigned                              /* return code */
OSPPStatusGetCode(
    OSPTSTATUS *ospvStatus            /* Status to get */
)
{
    unsigned statuscode = 0;

    if((ospvStatus != OSPC_OSNULL)&&(ospvStatus->ospmHasCode))
    {
        statuscode = ospvStatus->ospmStatusCode;
    }
    return statuscode; 
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPStatusFromElement() - create a status from an XML element
 *-----------------------------------------------------------------------*/
unsigned                          /* returns error code */
OSPPStatusFromElement(
    OSPT_XML_ELEM *ospvElem,        /* input is XML element */
    OSPTSTATUS  **ospvStatus      /* where to put status pointer */
)
{
    unsigned       ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM   *elem = OSPC_OSNULL;
    unsigned long temp = 0;

    if (ospvElem  == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvStatus == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_DATA_NO_STATUS;
    }

#if 0
    assert(OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_STATUS);
#endif

    /* create the Status structure */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        *ospvStatus = OSPPStatusNew();
        if (*ospvStatus == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_DATA_NO_STATUS;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /*
         * The Status element should consist of several child
         * elements. We'll run through what's there and pick out
         * the information we need. First, though, let's zero out
         * the values that we expect to find.
         */
        for ( elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, elem) )
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_CODE:
                ospvErrCode = OSPPMsgCodeFromElement(elem, &temp);
                OSPPStatusSetCode(*ospvStatus, temp);
                break;
            case OSPC_MELEM_DESC:
                OSPPStatusSetDesc(*ospvStatus, OSPPXMLElemGetValue(elem));
                break;
            default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem)) {
                    ospvErrCode = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }
    
    return ospvErrCode;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPStatusSetCode() - set the status code
 *-----------------------------------------------------------------------*/
void                                  /* no return */
OSPPStatusSetCode(
    OSPTSTATUS *ospvStatus,           /* Status to set */
    unsigned   ospvCode
)
{
    if (ospvStatus != OSPC_OSNULL) 
    {
        ospvStatus->ospmStatusCode = ospvCode;
        ospvStatus->ospmHasCode=1;
    }
    return; 
}

