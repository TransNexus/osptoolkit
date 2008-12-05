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
 * ospreauthrsp.c - OSP reauthorisation response functions
 */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospdest.h"
#include "osp/ospreauthrsp.h"
#include "osp/ospstatus.h"
#include "osp/osputils.h"

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspSetTimestamp() - sets the timestamp for an reauthorisation response
 *-----------------------------------------------------------------------*/

void                                       /* nothing returned */
OSPPReauthRspSetTimestamp(
    OSPTREAUTHRSP *ospvReauthRsp,
    OSPTTIME  ospvTime
)
{
    if (ospvReauthRsp != OSPC_OSNULL)
    {
        ospvReauthRsp->ospmReauthRspTimestamp = ospvTime;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspHasStatus() - does the reauthorisation response have
 * a status?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPReauthRspHasStatus(
    OSPTREAUTHRSP *ospvReauthRsp        /* reauthorisation response effected */
)
{
    unsigned ospvHasStatus = OSPC_FALSE;

    if (ospvReauthRsp != OSPC_OSNULL)
    {
        ospvHasStatus = (ospvReauthRsp->ospmReauthRspStatus != OSPC_OSNULL);
    }
    return(ospvHasStatus);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspGetStatus() - returns the status for an 
 * reauthorisation response
 *-----------------------------------------------------------------------*/
OSPTSTATUS *                                 /* returns pointer to dest */
    OSPPReauthRspGetStatus(
    OSPTREAUTHRSP *ospvReauthRsp               /* reauthorisation response */
    )
{
    OSPTSTATUS *ospvStatus = OSPC_OSNULL;

    if (ospvReauthRsp != OSPC_OSNULL)
    {
        ospvStatus = ospvReauthRsp->ospmReauthRspStatus;
    }
    return(ospvStatus);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspSetTrxId() - sets the transaction ID for an reauthorisation
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthRspSetTrxId(
    OSPTREAUTHRSP   *ospvReauthRsp,            /* reauthorisation response */
    OSPTTRXID ospvTrxId                 /* transaction ID to set */
)
{
    if (ospvReauthRsp   != OSPC_OSNULL)
    {
        ospvReauthRsp->ospmReauthRspTrxId = ospvTrxId;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspHasDest() - does the reauthorisation response have
 * a destination?
 *-----------------------------------------------------------------------*/
unsigned                            /* returns non-zero if number exists */
OSPPReauthRspHasDest(
    OSPTREAUTHRSP *ospvReauthRsp        /* reauthorisation response effected */
)
{
    unsigned ospvHasDest = OSPC_FALSE;

    if (ospvReauthRsp != OSPC_OSNULL)
    {
        ospvHasDest = (ospvReauthRsp->ospmReauthRspDest != OSPC_OSNULL);
    }
    return(ospvHasDest);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspSetDest() - set the destination for an
 * reauthorisation response
 *-----------------------------------------------------------------------*/
void                                       /* nothing returned */
OSPPReauthRspSetDest(
    OSPTREAUTHRSP       *ospvReauthRsp,    /* reauthorisation response  to set */
    OSPTDEST            *ospvDest          /* destination */
)
{
    if (ospvReauthRsp != OSPC_OSNULL)
    {
        if(ospvDest  != OSPC_OSNULL)
        {
            ospvReauthRsp->ospmReauthRspDest = ospvDest;
        }
    }
    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspHasComponentId() - is the component id set ?
 *-----------------------------------------------------------------------*/
unsigned                    /* returns non-zero if component id is set */
OSPPReauthRspHasComponentId(
    OSPTREAUTHRSP *ospvReauthRsp
)
{
  return (ospvReauthRsp->ospmReauthRspComponentId != OSPC_OSNULL);
}


OSPTTNAUDIT *
OSPPReauthRspGetTNAudit(
    OSPTREAUTHRSP *ospvReauthRsp
)
{
    if(ospvReauthRsp != OSPC_OSNULL)
    {
        return ospvReauthRsp->ospmReauthRspTNAudit;
    }
    else
    {
        return OSPC_OSNULL;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspNew() - creates a new (empty) reauthorisation response
 *-----------------------------------------------------------------------*/

OSPTREAUTHRSP *                              /* returns pointer or NULL */
    OSPPReauthRspNew()
{
    OSPTREAUTHRSP *ospvReauthRsp;

    OSPM_MALLOC(ospvReauthRsp, OSPTREAUTHRSP,sizeof(OSPTREAUTHRSP));
    OSPM_MEMSET(ospvReauthRsp, 0, sizeof(ospvReauthRsp));

    if (ospvReauthRsp != OSPC_OSNULL)
    {
        ospvReauthRsp->ospmReauthRspTimestamp = OSPC_TIMEMIN;
        ospvReauthRsp->ospmReauthRspStatus = OSPC_OSNULL;
        ospvReauthRsp->ospmReauthRspTrxId = 0;
        ospvReauthRsp->ospmReauthRspDest = OSPC_OSNULL;
        ospvReauthRsp->ospmReauthRspTNAudit = OSPC_OSNULL;
        ospvReauthRsp->ospmReauthRspComponentId = OSPC_OSNULL;
        ospvReauthRsp->ospmReauthRspMessageId = OSPC_OSNULL;
    }

    return(ospvReauthRsp);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspFromElement() - get reauthorisation response from an XML element
 *-----------------------------------------------------------------------*/
int                                   /* returns error code */
OSPPReauthRspFromElement(
    OSPTXMLELEM     *ospvElem,        /* input is XML element */
    OSPTREAUTHRSP   **ospvReauthRsp   /* where to put reauthorisation response pointer */
)
{
    unsigned        ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM     *elem       = OSPC_OSNULL;
    OSPTREAUTHRSP   *reauthrsp  = OSPC_OSNULL;
    OSPTDEST        *dest       = OSPC_OSNULL;
    OSPTTIME        t           = 0L;
    OSPTTRXID       transid     = 0L;
    OSPTXMLELEM     *ospvParent	= OSPC_OSNULL;
    unsigned char   *messageId  = OSPC_OSNULL;
    unsigned char   *compid     = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvReauthRsp == OSPC_OSNULL)
    {
        ospvErrCode = OSPC_ERR_DATA_NO_REAUTHRSP;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {

        if(OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem))==OSPC_MELEM_MESSAGE)
        {
            OSPPReauthRspMessageIdFromElement(ospvElem, &messageId);
            OSPPReauthRspSetMessageId(reauthrsp, messageId);

            /* ospvElem is pointing to the Message element. 
             * The first child contains the Component element. 
             * The following two lines of code change ospvElem from 
             * pointing to the Message element to the Component element.
             */
            ospvParent = ospvElem;
            ospvElem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvParent);
        }

        /* create the reauthorisation response object */
        reauthrsp = OSPPReauthRspNew();

        if (reauthrsp == OSPC_OSNULL)
        {
            
            ospvErrCode = OSPC_ERR_DATA_NO_REAUTHRSP;
        }
        else
        {
        /* Have to get ComponentId here */
            if(OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_REAUTHREQ)
            {
                OSPPReauthRspComponentIdFromElement(ospvElem, &compid);
                if(compid != OSPC_OSNULL)
                {
                    OSPPReauthRspSetComponentId(reauthrsp, compid);
                }                    
            }
        }
    }
    /*
     * The Reauthorisation Response element should consist of several
     * child elements. We'll run through what's there and pick out
     * the information we need.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR)
    {
        for ( elem = (OSPTXMLELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != (OSPTXMLELEM *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPTXMLELEM *)OSPPXMLElemNextChild(ospvElem, elem) )
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem)))
            {
                case OSPC_MELEM_MESSAGE:
                if(messageId != OSPC_OSNULL)
                {
                    OSPPReauthRspSetMessageId(reauthrsp, messageId);
                }
                break;

                case OSPC_MELEM_REAUTHREQ:
                if(compid != OSPC_OSNULL)
                {
                    OSPPReauthRspSetComponentId(reauthrsp, compid);
                }
                break;

                case OSPC_MELEM_TIMESTAMP:
                ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPReauthRspSetTimestamp(reauthrsp,t);
                }
                break;

                case OSPC_MELEM_STATUS:
                if(reauthrsp->ospmReauthRspStatus == OSPC_OSNULL)
                {
                    ospvErrCode = OSPPStatusFromElement(elem, &(reauthrsp->ospmReauthRspStatus));
                }
                break;

                case OSPC_MELEM_TRANSID:
                ospvErrCode = OSPPMsgTXFromElement(elem, &transid);
                OSPPReauthRspSetTrxId(reauthrsp, transid);
                break;

                case OSPC_MELEM_TNAUDIT:
                ospvErrCode = OSPPTNAuditFromElement(elem, &(reauthrsp->ospmReauthRspTNAudit));
                break;

                case OSPC_MELEM_DEST:
                ospvErrCode = OSPPDestFromElement(elem, &dest);
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    OSPPReauthRspSetDest(reauthrsp,dest);
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

        if(ospvParent != OSPC_OSNULL)
        {
            ospvElem = ospvParent;
        }

    }

    if (ospvErrCode != OSPC_ERR_DATA_NO_REAUTHRSP)
    {
        *ospvReauthRsp = reauthrsp;
    }

    return(ospvErrCode);
}

/*-----------------------------------------------------------------------*
 * OSPPReauthRspDelete() - deletes a reauthorisation response structure
 *-----------------------------------------------------------------------*/
void OSPPReauthRspDelete(
    OSPTREAUTHRSP **ospvReauthRsp
)
{
    if (*ospvReauthRsp != OSPC_OSNULL)
    {
        /* first remove the status */
        if ((*ospvReauthRsp)->ospmReauthRspStatus != OSPC_OSNULL)
        {
            OSPPStatusDelete(&((*ospvReauthRsp)->ospmReauthRspStatus));
        }

        /* now remove dest */
        if(OSPPReauthRspHasDest(*ospvReauthRsp))
        {
            OSPPDestDelete(&((*ospvReauthRsp)->ospmReauthRspDest));
        }

        /* delete ComponentId and MessageId */
        if(OSPPReauthRspHasComponentId(*ospvReauthRsp))
        {
            OSPM_FREE((*ospvReauthRsp)->ospmReauthRspComponentId);
            (*ospvReauthRsp)->ospmReauthRspComponentId = OSPC_OSNULL;
        }

        if(OSPPReauthRspHasMessageId(*ospvReauthRsp))
        {
            OSPM_FREE((*ospvReauthRsp)->ospmReauthRspMessageId);
            (*ospvReauthRsp)->ospmReauthRspMessageId = OSPC_OSNULL;
        }

        /* now delete the authrsp itself */
        OSPM_FREE(*ospvReauthRsp);
        *ospvReauthRsp = OSPC_OSNULL;
    }
}


/*-----------------------------------------------------------------------*
 * OSPPReauthRspHasMessageId() - is the message id set ?
 *-----------------------------------------------------------------------*/
unsigned                   /* returns non-zero if message id is set */
OSPPReauthRspHasMessageId(
    OSPTREAUTHRSP *ospvReauthRsp
)
{
  return (ospvReauthRsp->ospmReauthRspMessageId != OSPC_OSNULL);
}

/*-----------------------------------------------------------------------*
 * OSPPReauthRspSetMessageId() - creates space and copies in the string.
 *-----------------------------------------------------------------------*/
void      
OSPPReauthRspSetMessageId(
    OSPTREAUTHRSP  *ospvReauthRsp,    /* In - pointer to Usage Indication struct */
    unsigned char  *ospvMessageId     /* In - pointer to message id string */
    )
{
    int len = OSPM_STRLEN((const char *)ospvMessageId);

    if(ospvReauthRsp != OSPC_OSNULL)
    {
        if(ospvReauthRsp->ospmReauthRspMessageId != OSPC_OSNULL)
        {
            OSPM_FREE(ospvReauthRsp->ospmReauthRspMessageId);    
        }

        OSPM_MALLOC(ospvReauthRsp->ospmReauthRspMessageId, unsigned char, len + 1);
        OSPM_MEMSET(ospvReauthRsp->ospmReauthRspMessageId, 0, len + 1);
        OSPM_MEMCPY(ospvReauthRsp->ospmReauthRspMessageId, ospvMessageId, len);
    }

    return;
}

/* -----------------------------------------------------------------------------*
 * OSPPReauthRspMessageIdFromElement() - Get message id attribute from element.
 * -----------------------------------------------------------------------------*/
void   
OSPPReauthRspMessageIdFromElement(
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

        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_MESSAGEID)
        {
            /* we found the message attribute. Get the value */
            *ospvMessageId = (unsigned char *)OSPPXMLAttrGetValue(attr);
            break;

        }
    }
}

/* -----------------------------------------------------------------------------*
 * OSPPReauthRspComponentIdFromElement() - Get component id attribute from element.
 * -----------------------------------------------------------------------------*/
void   
OSPPReauthRspComponentIdFromElement(
    OSPTXMLELEM *ospvElemIn, 
    unsigned char **ospvComponentId
)
{
    OSPTXMLATTR *attr = (OSPTXMLATTR *)OSPC_OSNULL;

    /* look for the component id attribute */
    for (attr = (OSPTXMLATTR *)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != (OSPTXMLATTR *)OSPC_OSNULL);
        attr = (OSPTXMLATTR *)OSPPXMLElemNextAttr(ospvElemIn, attr))
    {

        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_COMPONENTID)
        {
            /* we found the component attribute. Get the value */
            *ospvComponentId = (unsigned char *)OSPPXMLAttrGetValue(attr);
            break;

        }
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPReauthRspSetComponentId() - creates space and copies in the string.
 *-----------------------------------------------------------------------*/
void      
OSPPReauthRspSetComponentId(
    OSPTREAUTHRSP  *ospvReauthRsp,    /* In - pointer to Usage Indication struct */
    unsigned char *ospvComponentId  /* In - pointer to component id string */
    )
{
    int len = OSPM_STRLEN((const char *)ospvComponentId);

    if(ospvReauthRsp != OSPC_OSNULL)
    {
        if(ospvReauthRsp->ospmReauthRspComponentId != OSPC_OSNULL)
        {
            OSPM_FREE(ospvReauthRsp->ospmReauthRspComponentId);    
        }

        OSPM_MALLOC(ospvReauthRsp->ospmReauthRspComponentId, unsigned char, len + 1);
        OSPM_MEMSET(ospvReauthRsp->ospmReauthRspComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvReauthRsp->ospmReauthRspComponentId, ospvComponentId, len);
    }

    return;
}
