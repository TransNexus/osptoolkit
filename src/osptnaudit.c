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
 * osptnaudit.c - Functions for tnaudit element.
 */
#include "osp/osptnaudit.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"
#include "osp/ospmsgattr.h"

void           
OSPPTNAuditDelete(
    OSPTTNAUDIT **ospvTNAudit
)
{
    if (*ospvTNAudit != OSPC_OSNULL)
    {
        if ((*ospvTNAudit)->ospmAuditURL != OSPC_OSNULL ) 
        {
            OSPM_FREE((*ospvTNAudit)->ospmAuditURL);
            (*ospvTNAudit)->ospmAuditURL = OSPC_OSNULL;
        }
        OSPM_FREE(*ospvTNAudit);
        *ospvTNAudit = OSPC_OSNULL;
    }

    return;
}

int       
OSPPTNAuditFromElement(
    OSPTXMLELEM *ospvElem, 
    OSPTTNAUDIT **ospvTNAudit
)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM     *elem       = OSPC_OSNULL;
    unsigned long   temp        = 0;

    if (ospvElem  == OSPC_OSNULL) 
    {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvTNAudit == OSPC_OSNULL) 
    {
        errorcode = OSPC_ERR_DATA_NO_STATUS;
    }

    /* create the TNAudit structure */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvTNAudit = OSPPTNAuditNew();
        if (*ospvTNAudit == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_AUDIT_MALLOC_FAILED;
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        /* If the elem has no children, we are probably dealing with
         * an AuditConfirm. Make sure it gets processed properly.
         */
        if((elem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvElem)) == OSPC_OSNULL)
        {
            if(OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_TNAUDITSTATE)
            {
                errorcode = OSPPMsgNumFromElement(ospvElem, &temp);
                if(errorcode == OSPC_ERR_NO_ERROR)
                {
                    OSPPTNAuditSetState(*ospvTNAudit, (OSPE_AUDIT_STATE)temp);
                    temp = 0;
                }            
            }
        }

        /*
         * The TNAudit element should consist of several child
         * elements. We'll run through what's there and pick out
         * the information we need. 
         */
        for ( elem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != (OSPTXMLELEM *)OSPC_OSNULL) && (errorcode == OSPC_ERR_NO_ERROR);
            elem = (OSPTXMLELEM *)OSPPXMLElemNextChild(ospvElem, elem) )
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem)))
            {
                case OSPC_MELEM_TNAUDIT:
                break;
                
                case OSPC_MELEM_TNAUDITSTATE:
                    errorcode = OSPPMsgNumFromElement(elem, &temp);
                    if(errorcode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPTNAuditSetState(*ospvTNAudit, (OSPE_AUDIT_STATE)temp);
                        temp = 0;
                    }
                break;

                case OSPC_MELEM_TNAUDITURL:
                    OSPPTNAuditSetURL(*ospvTNAudit, (unsigned char *)OSPPXMLElemGetValue(elem));
                break;

                case OSPC_MELEM_TNAUDITTIMELIMIT:
                    errorcode = OSPPMsgNumFromElement(elem, &temp);
                    if(errorcode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPTNAuditSetTimeLimit(*ospvTNAudit, temp);
                        temp = 0;
                    }
                break;

                case OSPC_MELEM_TNAUDITMAXMESSAGES:
                    errorcode = OSPPMsgNumFromElement(elem, &temp);
                    if(errorcode == OSPC_ERR_NO_ERROR)
                    {
                        OSPPTNAuditSetMaxMessages(*ospvTNAudit, temp);
                        temp = 0;
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
                    errorcode = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }
    return errorcode;
}

unsigned       
OSPPTNAuditGetMaxMessages(
    OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return ospvTNAudit->ospmAuditMaxMessages;
    }
    else
    {
        return 0;
    }
}

OSPE_AUDIT_STATE       
OSPPTNAuditGetState(
    OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return (OSPE_AUDIT_STATE)ospvTNAudit->ospmAuditState;
    }
    else
    {
        return (OSPE_AUDIT_STATE)0;
    }
}

unsigned       
OSPPTNAuditGetTimeLimit(
    OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return ospvTNAudit->ospmAuditTimeLimit;
    }
    else
    {
        return 0;
    }
}

unsigned char  *
OSPPTNAuditGetURL(
   OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return ospvTNAudit->ospmAuditURL;
    }
    else
    {
        return OSPC_OSNULL;
    }
}

unsigned       
OSPPTNAuditHasMaxMessages(
    OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return (ospvTNAudit->ospmAuditMaxMessages  > 0);
    }
    else
    {
        return 0;
    }
}

unsigned       
OSPPTNAuditHasState(
    OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return (ospvTNAudit->ospmAuditState > 0);
    }
    else
    {
        return 0;
    }
}

unsigned       
OSPPTNAuditHasTimeLimit(
    OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return (ospvTNAudit->ospmAuditTimeLimit > 0);
    }
    else
    {
        return 0;
    }

}

unsigned       
OSPPTNAuditHasURL(
    OSPTTNAUDIT *ospvTNAudit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        return(ospvTNAudit->ospmAuditURL != OSPC_OSNULL);
    }
    else
    {
        return 0;
    }
}

OSPTTNAUDIT    *
OSPPTNAuditNew(
    void
)
{
    OSPTTNAUDIT    *ospvTNAudit = OSPC_OSNULL;

    /* try to allocate the memory for the entire object */
    OSPM_MALLOC(ospvTNAudit, OSPTTNAUDIT, sizeof(OSPTTNAUDIT));
    if(ospvTNAudit != OSPC_OSNULL)
    {
        /* Initialize it */
        OSPM_MEMSET(ospvTNAudit, 0, sizeof(OSPTTNAUDIT));
        ospvTNAudit->ospmAuditURL = OSPC_OSNULL;
    }

    return(ospvTNAudit);
}

void           
OSPPTNAuditSetMaxMessages(
    OSPTTNAUDIT *ospvTNAudit, 
    unsigned    ospvMaxMessages
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        ospvTNAudit->ospmAuditMaxMessages = ospvMaxMessages;
    }

    return;
}

void           
OSPPTNAuditSetState(
    OSPTTNAUDIT         *ospvTNAudit, 
    OSPE_AUDIT_STATE    ospvAuditState
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        ospvTNAudit->ospmAuditState = ospvAuditState;
    }

    return;
}

void           
OSPPTNAuditSetTimeLimit(
    OSPTTNAUDIT *ospvTNAudit, 
    unsigned    ospvTimeLimit
)
{
    if(ospvTNAudit != OSPC_OSNULL)
    {
        ospvTNAudit->ospmAuditTimeLimit = ospvTimeLimit;
    }

    return;
}

void           
OSPPTNAuditSetURL(
    OSPTTNAUDIT *ospvTNAudit, 
    unsigned char *ospvAuditURL
)
{
    if (ospvTNAudit != OSPC_OSNULL)
    {
        if(ospvAuditURL != OSPC_OSNULL)
        {
            if (ospvTNAudit->ospmAuditURL != OSPC_OSNULL)
            {
                OSPM_FREE(ospvTNAudit->ospmAuditURL);
                ospvTNAudit->ospmAuditURL = OSPC_OSNULL;
            }

            OSPM_MALLOC(ospvTNAudit->ospmAuditURL, unsigned char, 
                strlen((const char *)ospvAuditURL)+1);

            if (ospvTNAudit->ospmAuditURL != OSPC_OSNULL)
            {

                OSPM_MEMCPY((ospvTNAudit->ospmAuditURL), 
                    (const char *)(ospvAuditURL), strlen((const char *)ospvAuditURL)+1);
            }
        }
    }

    return; 
}

