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
 * ospusagecnf.c - OSP usage confirmation functions
 */

#include "osp.h"
#include "osperrno.h"
#include "ospbfr.h"
#include "osplist.h"
#include "ospxmlattr.h"
#include "ospxmlelem.h"
#include "ospmsgattr.h"
#include "ospmsgelem.h"
#include "ospusage.h"
#include "ospusagecnf.h"
#include "ospstatus.h"
#include "osputils.h"
#include "ospciscoext.h"

/*-----------------------------------------------------------------------*
 * OSPPUsageCnfDelete() - destroy specified UsageCnf object
 *-----------------------------------------------------------------------*/

void
OSPPUsageCnfDelete(OSPTUSAGECNF **ospvUsageCnf)
{

    if (*ospvUsageCnf != OSPC_OSNULL)
    {
        if (OSPPUsageCnfHasStatus(*ospvUsageCnf))
        {
            OSPPStatusDelete(&((*ospvUsageCnf)->ospmUsageCnfStatus));
        }

        if((*ospvUsageCnf)->ospmUsageCnfTNAudit != OSPC_OSNULL)
        {      
            OSPPTNAuditDelete(&((*ospvUsageCnf)->ospmUsageCnfTNAudit));
        }

        if((*ospvUsageCnf)->ospmUsageCnfCSAudit != OSPC_OSNULL)
        {      
            OSPPCSAuditDelete(&((*ospvUsageCnf)->ospmUsageCnfCSAudit));
        }

        if((*ospvUsageCnf)->ospmUsageCnfComponentId != OSPC_OSNULL)
        {      
            OSPM_FREE((*ospvUsageCnf)->ospmUsageCnfComponentId);
        }

        if((*ospvUsageCnf)->ospmUsageCnfMessageId != OSPC_OSNULL)
        {
            OSPM_FREE((*ospvUsageCnf)->ospmUsageCnfMessageId);
        }

        OSPM_FREE(*ospvUsageCnf);
        *ospvUsageCnf = OSPC_OSNULL;
    }
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfSetTimestamp() - set the timestamp
 *-----------------------------------------------------------------------*/
void                                /* nothing returned */
OSPPUsageCnfSetTimestamp(
    OSPTUSAGECNF *ospvUsageCnf,     /* usage cnfication to set */
    OSPTTIME ospvTimestamp          /* timestamp to set to */
)
{
    if (ospvUsageCnf != OSPC_OSNULL)
    {
        if(ospvTimestamp  != 0)
        {
            ospvUsageCnf->ospmUsageCnfTimestamp = ospvTimestamp;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfHasStatus() - does the usage confirmation have
 * a status?
 *-----------------------------------------------------------------------*/

unsigned                            /* returns non-zero if number exists */
OSPPUsageCnfHasStatus(
    OSPTUSAGECNF *ospvUsageCnf        /* usage confirmation effected */
)
{
    unsigned ospvHasStatus = OSPC_FALSE;

    if(ospvUsageCnf != OSPC_OSNULL)
    {
        ospvHasStatus = ((ospvUsageCnf)->ospmUsageCnfStatus != OSPC_OSNULL);
    }
    return(ospvHasStatus);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfGetStatus() - returns the status for an 
 * usage confirmation
 *-----------------------------------------------------------------------*/

OSPTSTATUS *                                 /* returns pointer to dest */
    OSPPUsageCnfGetStatus(
    OSPTUSAGECNF *ospvUsageCnf               /* usage confirmation */
    )
{
    OSPTSTATUS *ospvStatus = OSPC_OSNULL;

    if (ospvUsageCnf != OSPC_OSNULL)
    {
        ospvStatus = (ospvUsageCnf)->ospmUsageCnfStatus;
    }
    return(ospvStatus);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfHasComponentId() - is the component id set ?
 *-----------------------------------------------------------------------*/
unsigned                    /* returns non-zero if component id is set */
OSPPUsageCnfHasComponentId(
    OSPTUSAGECNF *ospvUsageCnf
)
{
  return (ospvUsageCnf->ospmUsageCnfComponentId != OSPC_OSNULL);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfGetComponentId() - returns a new copy of the component id.
 *-----------------------------------------------------------------------*/
unsigned char  *
OSPPUsageCnfGetComponentId(
    OSPTUSAGECNF *ospvUsageCnf
)
{
    unsigned char   *componentstring   = OSPC_OSNULL;
    int             len                = 0;

    if (OSPPUsageCnfHasComponentId(ospvUsageCnf))
    {
        len = OSPM_STRLEN((const char *)(ospvUsageCnf->ospmUsageCnfComponentId));
        OSPM_MALLOC(componentstring, unsigned char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvUsageCnf->ospmUsageCnfComponentId, len);
    }

    return componentstring;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfSetComponentId() - creates space and copies in the string.
 *-----------------------------------------------------------------------*/
void      
OSPPUsageCnfSetComponentId(
    OSPTUSAGECNF  *ospvUsageCnf,    /* In - pointer to Usage Confirm struct */
    unsigned char *ospvComponentId  /* In - pointer to component id string */
    )
{
    int len = OSPM_STRLEN((const char *)ospvComponentId);

    if(ospvUsageCnf != OSPC_OSNULL)
    {
        if(ospvUsageCnf->ospmUsageCnfComponentId != OSPC_OSNULL)
        {
            OSPM_FREE(ospvUsageCnf->ospmUsageCnfComponentId);    
        }

        OSPM_MALLOC(ospvUsageCnf->ospmUsageCnfComponentId, unsigned char, len + 1);
        OSPM_MEMSET(ospvUsageCnf->ospmUsageCnfComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageCnf->ospmUsageCnfComponentId, ospvComponentId, len);
    }

    return;
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfNew() - creates a new (empty) usage confirmation
 *-----------------------------------------------------------------------*/
OSPTUSAGECNF *                                 /* returns pointer or NULL */
    OSPPUsageCnfNew(void)
{
    OSPTUSAGECNF *usagecnf;

    OSPM_MALLOC(usagecnf, OSPTUSAGECNF,sizeof(OSPTUSAGECNF));
    if (usagecnf != (OSPTUSAGECNF *)OSPC_OSNULL)
    {
        OSPM_MEMSET(usagecnf, 0, sizeof(OSPTUSAGECNF));

        OSPPListLinkNew (&(usagecnf->ospmUsageCnfLink));

        usagecnf->ospmUsageCnfTimestamp = (OSPTTIME)0;
        usagecnf->ospmUsageCnfStatus = OSPC_OSNULL;
        usagecnf->ospmUsageCnfCSAudit = OSPC_OSNULL;
        usagecnf->ospmUsageCnfTNAudit = OSPC_OSNULL;
        usagecnf->ospmUsageCnfComponentId = OSPC_OSNULL;
        usagecnf->ospmUsageCnfMessageId = OSPC_OSNULL;
    }

    return usagecnf;
}

OSPTCSAUDIT *
OSPPUsageCnfGetCSAudit(
    OSPTUSAGECNF *ospvUsageCnf
)
{
    if(ospvUsageCnf != OSPC_OSNULL)
    {
        return ospvUsageCnf->ospmUsageCnfCSAudit;
    }
    else
    {
        return OSPC_OSNULL;
    }
}

OSPTTNAUDIT *
OSPPUsageCnfGetTNAudit(
    OSPTUSAGECNF *ospvUsageCnf
)
{
    if(ospvUsageCnf != OSPC_OSNULL)
    {
        return ospvUsageCnf->ospmUsageCnfTNAudit;
    }
    else
    {
        return OSPC_OSNULL;
    }
}

/*
 * OSPPUsageCnfComponentIdFromElement() - Get component id attribute from element.
 */
void   
OSPPUsageCnfComponentIdFromElement(
    OSPTXMLELEM *ospvElem, 
    const unsigned char **ospvComponentId
)
{
    OSPTXMLATTR *attr = (OSPTXMLATTR *)OSPC_OSNULL;

    /* look for the component id attribute */
    for (attr = (OSPTXMLATTR *)OSPPXMLElemFirstAttr(ospvElem);
        (attr != (OSPTXMLATTR *)OSPC_OSNULL);
        attr = (OSPTXMLATTR *)OSPPXMLElemNextAttr(ospvElem, attr))
    {

        if (OSPPMsgGetAttrPart(OSPPXMLAttrGetName(attr)) == ospeAttrComponentId)
        {
            /* we found the component attribute. Get the value */
            *ospvComponentId = (const unsigned char *)OSPPXMLAttrGetValue(attr);
        }
    }
    
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPUsageCnfFromElement() - get usage confirmation from an XML element
 *-----------------------------------------------------------------------*/

unsigned                          /* returns error code */
OSPPUsageCnfFromElement(
    OSPTXMLELEM *ospvElem,       /* input is XML element */
    OSPTLIST    *ospvUsageCnf   /* where to put usage cnf list */
)
{
    unsigned        ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM     *elem       = OSPC_OSNULL;
    OSPTXMLELEM     *elem1      = OSPC_OSNULL;
    OSPTUSAGECNF    *usagecnf   = OSPC_OSNULL;
    OSPTTIME        t           = 0;
    unsigned char   *compid     = OSPC_OSNULL;
    unsigned char   *messageId  = OSPC_OSNULL;
    OSPTXMLELEM     *ospvParent = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvUsageCnf == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_USAGECNF;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        OSPPListNew(ospvUsageCnf);

        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            if(OSPPMsgGetElemPart(OSPPXMLElemGetName(ospvElem))==ospeElemMessage)
            {
                OSPPUsageCnfMessageIdFromElement(ospvElem, &messageId);

                /* ospvElem is pointing to the Message element. 
                 * The first child contains the Component element. 
                 * The following two lines of code change ospvElem from 
                 * pointing to the Message element to the Component element.
                 */
                ospvParent = ospvElem;
                ospvElem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvParent);
            }

            for(elem1 = (OSPTXMLELEM *)OSPPListFirst((OSPTLIST *)ospvElem);
                (elem1 != OSPC_OSNULL) &&
                (ospvErrCode == OSPC_ERR_NO_ERROR);
                elem1 = (OSPTXMLELEM *)OSPPListNext((OSPTLIST *)ospvElem, elem1))
            {
                /* create the usage confirmation object */
                usagecnf = OSPPUsageCnfNew();

                if (usagecnf == OSPC_OSNULL)
                {
                    ospvErrCode = OSPC_ERR_DATA_NO_USAGECNF;
                }
                else
                {
                    /*
                     * The Usage Confirmation element should consist of several child
                     * elements. We'll run through what's there and pick out
                     * the information we need.
                     */

                    if(messageId != OSPC_OSNULL)
                    {
                        OSPPUsageCnfSetMessageId(usagecnf, messageId);
                    }

                    if(OSPPMsgGetElemPart(OSPPXMLElemGetName(elem1)) == ospeElemUsageCnf)
                    {
                        /* Get ComponentId */
                    if(elem1 != OSPC_OSNULL)
                        {
                            OSPPUsageCnfComponentIdFromElement(elem1,(const unsigned char**) &compid);
                            if(compid != OSPC_OSNULL)
                            {
                                OSPPUsageCnfSetComponentId(usagecnf, compid);
                            }
                        }

                        for (elem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(elem1);
                            (elem != (OSPTXMLELEM *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
                            elem = (OSPTXMLELEM *)OSPPXMLElemNextChild(elem1, elem) )
                        {
                            switch (OSPPMsgGetElemPart(OSPPXMLElemGetName(elem)))
                            {
                                case ospeElemMessage:
                                if(OSPPMsgGetElemPart(OSPPXMLElemGetName(elem))==ospeElemMessage)
                                {
                                    OSPPUsageCnfMessageIdFromElement(elem, &messageId);
                                    if(messageId != OSPC_OSNULL)
                                    {
                                        OSPPUsageCnfSetMessageId(usagecnf, messageId);
                                    }
                                }
                                break;

                                case ospeElemUsageCnf:
                                OSPPUsageCnfComponentIdFromElement(elem,(const unsigned char **)  &compid);
                                if(compid != OSPC_OSNULL)
                                {
                                    OSPPUsageCnfSetComponentId(usagecnf, compid);
                                }
                                break;

                                case ospeElemTimestamp:
                                ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                                {
                                    OSPPUsageCnfSetTimestamp(usagecnf, t);
                                }
                                break;
    
                                case ospeElemStatus:
                                if(usagecnf->ospmUsageCnfStatus == OSPC_OSNULL)
                                {
                                    /* usagecnf->ospmUsageCnfStatus = OSPPStatusNew(); */
                                    ospvErrCode = OSPPStatusFromElement(elem, &(usagecnf->ospmUsageCnfStatus));
                                }
                                break;

                                case ospeElemTNAudit:
                                ospvErrCode = OSPPTNAuditFromElement(elem, &(usagecnf->ospmUsageCnfTNAudit));
                                break;
    
                                case ospeElemCSAuditTrigger:
                                ospvErrCode = OSPPCSAuditFromElement(elem, &(usagecnf->ospmUsageCnfCSAudit));
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
                        if(usagecnf != OSPC_OSNULL)
                        {
                            OSPPListAppend(ospvUsageCnf, usagecnf);
                            usagecnf = OSPC_OSNULL;
                        }
                    }
                }

            }
        }
    }


   /* clean up */
    if(ospvErrCode != OSPC_ERR_NO_ERROR)
    {
        if(ospvUsageCnf != OSPC_OSNULL)
        {
            while(!OSPPListEmpty(ospvUsageCnf))
            {

                usagecnf = (OSPTUSAGECNF *)OSPPListRemove(ospvUsageCnf);

                if(usagecnf != OSPC_OSNULL)
                {

                    OSPPUsageCnfDelete(&(usagecnf));
                }
            }  

            OSPPListDelete(ospvUsageCnf);
        }
    }

    if(usagecnf != OSPC_OSNULL)
    {
        OSPPUsageCnfDelete(&usagecnf);
    }
    return ospvErrCode;
}

/*-----------------------------------------------------------------------*
 * OSPPUsageCnfHasMessageId() - is the message id set ?
 *-----------------------------------------------------------------------*/
unsigned                   /* returns non-zero if message id is set */
OSPPUsageCnfHasMessageId(
    OSPTUSAGECNF *ospvUsageCnf
)
{
  return (ospvUsageCnf->ospmUsageCnfMessageId != OSPC_OSNULL);
}

/*-----------------------------------------------------------------------*
 * OSPPUsageCnfSetMessageId() - creates space and copies in the string.
 *-----------------------------------------------------------------------*/
void      
OSPPUsageCnfSetMessageId(
    OSPTUSAGECNF  *ospvUsageCnf,  /* In - pointer to Usage Indication struct */
    unsigned char *ospvMessageId  /* In - pointer to message id string */
    )
{
    int len = OSPM_STRLEN((const char *)ospvMessageId);

    if(ospvUsageCnf != OSPC_OSNULL)
    {
        if(ospvUsageCnf->ospmUsageCnfMessageId != OSPC_OSNULL)
        {
            OSPM_FREE(ospvUsageCnf->ospmUsageCnfMessageId);    
        }

        OSPM_MALLOC(ospvUsageCnf->ospmUsageCnfMessageId, unsigned char, len + 1);
        OSPM_MEMSET(ospvUsageCnf->ospmUsageCnfMessageId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageCnf->ospmUsageCnfMessageId, ospvMessageId, len);
    }

    return;
}

/* -----------------------------------------------------------------------------*
 * OSPPUsageCnfMessageIdFromElement() - Get message id attribute from element.
 * -----------------------------------------------------------------------------*/
void   
OSPPUsageCnfMessageIdFromElement(
    OSPTXMLELEM *ospvElemIn, 
    unsigned char **ospvMessageId
)
{
    OSPTXMLATTR *attr = (OSPTXMLATTR *)OSPC_OSNULL;

    /* look for the message id attribute */
    for (attr = (OSPTXMLATTR *)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != (OSPTXMLATTR *)OSPC_OSNULL);
        attr = (OSPTXMLATTR *)OSPPXMLElemNextAttr(ospvElemIn, attr))
    {

        if (OSPPMsgGetAttrPart(OSPPXMLAttrGetName(attr)) == ospeAttrMessageId)
        {
            /* we found the message attribute. Get the value */
            *ospvMessageId = (unsigned char *)OSPPXMLAttrGetValue(attr);
            break;

        }
    }
}
