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
 * ospciscoext.c - Functions for Cisco audit element.
 */

#include "osp/ospciscoext.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"
#include "osp/ospmsgattr.h"

void           
OSPPCSAuditDelete(
    OSPTCSAUDIT **ospvCSAudit
)
{
    if (*ospvCSAudit != OSPC_OSNULL)
    {
        if ((*ospvCSAudit)->ospmAuditTrigger != OSPC_OSNULL ) 
        {
            OSPM_FREE((*ospvCSAudit)->ospmAuditTrigger);
            (*ospvCSAudit)->ospmAuditTrigger = OSPC_OSNULL;
        }

        OSPM_FREE(*ospvCSAudit);
        *ospvCSAudit = OSPC_OSNULL;
    }

    return;
}

int       
OSPPCSAuditFromElement(
    OSPTXMLELEM *ospvElem, 
    OSPTCSAUDIT **ospvCSAudit
)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;

    if (ospvElem  == OSPC_OSNULL) 
    {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvCSAudit == OSPC_OSNULL) 
    {
        errorcode = OSPC_ERR_DATA_NO_STATUS;
    }

    /* create the CSAudit structure */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvCSAudit = OSPPCSAuditNew();
        if (*ospvCSAudit == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_AUDIT_MALLOC_FAILED;
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        /*
         * The CSAudit element should consist of one element.
         *  
         */
        if(OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_CSAUDITTRIGGER)
        {
            OSPPCSAuditSetTrigger(*ospvCSAudit, (unsigned char *)OSPPXMLElemGetValue(ospvElem));
        }
        else if (OSPPMsgElemIsCritical(ospvElem))
        {
            errorcode = OSPC_ERR_XML_BAD_ELEMENT;
        }
    }

    return errorcode;
}

unsigned char  *
OSPPCSAuditGetTrigger(
   OSPTCSAUDIT *ospvCSAudit
)
{
    if(ospvCSAudit != OSPC_OSNULL)
    {
        return ospvCSAudit->ospmAuditTrigger;
    }
    else
    {
        return OSPC_OSNULL;
    }
}

unsigned       
OSPPCSAuditHasTrigger(
    OSPTCSAUDIT *ospvCSAudit
)
{
    if(ospvCSAudit != OSPC_OSNULL)
    {
        return(ospvCSAudit->ospmAuditTrigger != OSPC_OSNULL);
    }
    else
    {
        return 0;
    }
}

OSPTCSAUDIT    *
OSPPCSAuditNew(
    void
)
{
    OSPTCSAUDIT    *ospvCSAudit = OSPC_OSNULL;

    /* try to allocate the memory for the entire object */
    OSPM_MALLOC(ospvCSAudit, OSPTCSAUDIT, sizeof(OSPTCSAUDIT));
    if(ospvCSAudit != OSPC_OSNULL)
    {
        /* Initialize it */
        OSPM_MEMSET(ospvCSAudit, 0, sizeof(OSPTCSAUDIT));
        ospvCSAudit->ospmAuditTrigger = OSPC_OSNULL;
    }

    return(ospvCSAudit);
}

void           
OSPPCSAuditSetTrigger(
    OSPTCSAUDIT *ospvCSAudit, 
    unsigned char *ospvAuditTrigger
)
{
    if (ospvCSAudit != OSPC_OSNULL)
    {
        if(ospvAuditTrigger != OSPC_OSNULL)
        {
            if (ospvCSAudit->ospmAuditTrigger != OSPC_OSNULL)
            {
                OSPM_FREE(ospvCSAudit->ospmAuditTrigger);
                ospvCSAudit->ospmAuditTrigger = OSPC_OSNULL;
            }

            OSPM_MALLOC(ospvCSAudit->ospmAuditTrigger, unsigned char, 
                strlen((const char *)ospvAuditTrigger)+1);

            if (ospvCSAudit->ospmAuditTrigger != OSPC_OSNULL)
            {

                OSPM_MEMCPY((ospvCSAudit->ospmAuditTrigger), 
                    (const char *)(ospvAuditTrigger), strlen((const char *)ospvAuditTrigger)+1);
            }
        }
    }

    return; 
}

