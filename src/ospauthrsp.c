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
 * ospauthrsp.c - OSP authorisation response functions
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
#include "osp/ospauthrsp.h"
#include "osp/ospstatus.h"
#include "osp/osputils.h"

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetTimestamp() - sets the timestamp for an AuthResponse
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetTimestamp(
    OSPTAUTHRSP* ospvAuthRsp,
    OSPTTIME ospvTime)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->ospmAuthRspTimestamp = ospvTime;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasStatus() - does the AuthResponse have a status?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasStatus(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response effected */
{
    int ospvHasStatus = OSPC_FALSE;

    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvHasStatus = (ospvAuthRsp->ospmAuthRspStatus != OSPC_OSNULL);
    }
    return ospvHasStatus;
}

/**/
/*
 *----------------------------------------------------------------------*
 * OSPPAuthRspGetStatus() - returns the status for an AuthResponse 
 *-----------------------------------------------------------------------*
 */
OSPTSTATUS* OSPPAuthRspGetStatus(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    OSPTSTATUS* ospvStatus = OSPC_OSNULL;

    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvStatus = ospvAuthRsp->ospmAuthRspStatus;
    }
    return ospvStatus;
}

/**/
/*
 *----------------------------------------------------------------------*
 * OSPPAuthRspSetTrxId() - sets the transaction ID for an authorisation
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetTrxId(
    OSPTAUTHRSP* ospvAuthRsp,   /* authorisation response */
    OSPTTRXID ospvTrxId)        /* transaction ID to set */
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        (ospvAuthRsp)->ospmAuthRspTrxId = ospvTrxId;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasDest() - does the AuthResponse have a destination?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasDest(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response effected */
{
    int ospvHasDest = OSPC_FALSE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvHasDest = (ospvAuthRsp->ospmAuthRspDest != OSPC_OSNULL);
    }
    return ospvHasDest;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspFirstDest() - returns the first destination for an AuthResponse 
 *-----------------------------------------------------------------------*
 */
OSPTDEST* OSPPAuthRspFirstDest(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    OSPTDEST* dest = OSPC_OSNULL;

    if (ospvAuthRsp != OSPC_OSNULL) {
        dest = (OSPTDEST*)OSPPListFirst(&(ospvAuthRsp->ospmAuthRspDest));
    }
    return dest;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspNextDest() - returns the next destination for an AuthResponse
 *-----------------------------------------------------------------------*
 */
OSPTDEST* OSPPAuthRspNextDest(
    OSPTAUTHRSP* ospvAuthRsp,   /* authorisation response */
    OSPTDEST* ospvDest)
{
    OSPTDEST* dest = (OSPTDEST*)OSPC_OSNULL;

    if (ospvAuthRsp != (OSPTAUTHRSP*)OSPC_OSNULL) {
        dest = (OSPTDEST*)OSPPListNext(&(ospvAuthRsp->ospmAuthRspDest), ospvDest);
    }
    return dest;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasTNDelayLimit() - does an AuthResponse have delay limit?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasTNDelayLimit(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    int ospvHasTNDelayLimit = OSPC_FALSE;

    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvHasTNDelayLimit = (ospvAuthRsp->ospmAuthRspTNDelayLimit > 0);
    }
    return ospvHasTNDelayLimit;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetTNDelayLimit() - sets the delay limit for an authorisation
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetTNDelayLimit(
    OSPTAUTHRSP* ospvAuthRsp,   /* authorisation response */
    unsigned ospvTNDelayLimit)  /* delay limit to set */
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->ospmAuthRspTNDelayLimit = ospvTNDelayLimit;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspGetTNDelayLimit() - gets the delay limit for an AuthResponse
 *-----------------------------------------------------------------------*
 */
unsigned OSPPAuthRspGetTNDelayLimit(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    unsigned ospvTNDelayLimit = 0;

    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvTNDelayLimit = (ospvAuthRsp->ospmAuthRspTNDelayLimit);
    }
    return ospvTNDelayLimit;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasTNDelayPref() - does an AuthResponse have delay pref?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasTNDelayPref(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    int ospvHasTNDelayPref = OSPC_FALSE;

    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvHasTNDelayPref = (ospvAuthRsp->ospmAuthRspTNDelayPref > 0);
    }
    return ospvHasTNDelayPref;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetTNDelayPref() - sets the delay pref for an authorisation
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetTNDelayPref(
    OSPTAUTHRSP* ospvAuthRsp,   /* authorisation response */
    unsigned ospvTNDelayPref)   /* delay pref to set */
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->ospmAuthRspTNDelayPref = ospvTNDelayPref;
    }
}

unsigned OSPPAuthRspHasNumDests(
    OSPTAUTHRSP* ospvAuthRsp)
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        return ospvAuthRsp->ospmNumDests;
    } else {
        return 0;
    }
}

void OSPPAuthRspIncNumDests(
    OSPTAUTHRSP* ospvAuthRsp)
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->ospmNumDests++;
    }
}

unsigned OSPPAuthRspGetNumDests(
    OSPTAUTHRSP* ospvAuthRsp)
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        return ospvAuthRsp->ospmNumDests;
    } else {
        return 0;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasComponentId() - is the component id set ?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasComponentId(
    OSPTAUTHRSP* ospvAuthRsp)
{
  return (ospvAuthRsp->ospmAuthRspComponentId != OSPC_OSNULL);
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetComponentId() - creates space and copies in the string.
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetComponentId(
    OSPTAUTHRSP* ospvAuthRsp,       /* In - pointer to Usage Indication struct */
    unsigned char* ospvComponentId) /* In - pointer to component id string */
{
    int len = OSPM_STRLEN((const char*)ospvComponentId);

    if(ospvAuthRsp != OSPC_OSNULL) {
        if(ospvAuthRsp->ospmAuthRspComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvAuthRsp->ospmAuthRspComponentId);    
        }

        OSPM_MALLOC(ospvAuthRsp->ospmAuthRspComponentId, unsigned char, len + 1);
        OSPM_MEMSET(ospvAuthRsp->ospmAuthRspComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvAuthRsp->ospmAuthRspComponentId, ospvComponentId, len);
    }
}

OSPTTNAUDIT* OSPPAuthRspGetTNAudit(
    OSPTAUTHRSP* ospvAuthRsp)
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        return ospvAuthRsp->ospmAuthRspTNAudit;
    } else {
        return OSPC_OSNULL;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspNew() - creates a new (empty) authorisation response
 *-----------------------------------------------------------------------*
 */
OSPTAUTHRSP* OSPPAuthRspNew(void)
{
    OSPTAUTHRSP* ospvAuthRsp;

    OSPM_MALLOC(ospvAuthRsp, OSPTAUTHRSP,sizeof(OSPTAUTHRSP));
    if (ospvAuthRsp != OSPC_OSNULL) {
        OSPM_MEMSET(ospvAuthRsp, 0, sizeof(OSPTAUTHRSP));

        ospvAuthRsp->ospmAuthRspTimestamp = OSPC_TIMEMIN;
        ospvAuthRsp->ospmAuthRspStatus = OSPC_OSNULL;
        ospvAuthRsp->ospmAuthRspTrxId = 0;
        ospvAuthRsp->ospmAuthRspCSAudit = (OSPTCSAUDIT*)OSPC_OSNULL;
        OSPPListNew(&(ospvAuthRsp->ospmAuthRspDest));
    }

    return ospvAuthRsp;
}

/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspDelete() - deletes an authorisation response structure
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspDelete(
    OSPTAUTHRSP** ospvAuthRsp)
{
    OSPTDEST* dest = OSPC_OSNULL;

    if (*ospvAuthRsp != OSPC_OSNULL) {
        /* first remove the status */
        if ((*ospvAuthRsp)->ospmAuthRspStatus != OSPC_OSNULL) {
            OSPPStatusDelete(&((*ospvAuthRsp)->ospmAuthRspStatus));
        }

        /* remove tnaudit */
        if ((*ospvAuthRsp)->ospmAuthRspTNAudit != OSPC_OSNULL) {
            OSPPTNAuditDelete(&((*ospvAuthRsp)->ospmAuthRspTNAudit));
        }

        /* remove csaudit */
        if ((*ospvAuthRsp)->ospmAuthRspCSAudit != OSPC_OSNULL) {
            OSPPCSAuditDelete(&((*ospvAuthRsp)->ospmAuthRspCSAudit));
        }

        /* remove messageId */
        if(OSPPAuthRspHasMessageId(*ospvAuthRsp)) {
            OSPM_FREE((*ospvAuthRsp)->ospmAuthRspMessageId);
            (*ospvAuthRsp)->ospmAuthRspMessageId = OSPC_OSNULL;
        }

        /* remove componentId */
        if(OSPPAuthRspHasComponentId(*ospvAuthRsp)) {
            OSPM_FREE((*ospvAuthRsp)->ospmAuthRspComponentId);
            (*ospvAuthRsp)->ospmAuthRspComponentId = OSPC_OSNULL;
        }

        /* now remove any attached dests -- this means following the chain */
        while (!OSPPListEmpty(&((*ospvAuthRsp)->ospmAuthRspDest))) {
            dest = (OSPTDEST*)OSPPListRemove(&((*ospvAuthRsp)->ospmAuthRspDest));
            if (dest != OSPC_OSNULL) {
                OSPPDestDelete(&dest);
                dest = OSPC_OSNULL;
            }
        }

        /* now delete the authrsp itself */
        OSPM_FREE(*ospvAuthRsp);
        *ospvAuthRsp = OSPC_OSNULL;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspAddDest() - add a destination to authrsp. If ospvDest is NULL,
 * adds a new, empty destination.
 * Returns: pointer to added dest for manipulation
 *-----------------------------------------------------------------------*
 */
OSPTDEST* OSPPAuthRspAddDest(
    OSPTAUTHRSP* ospvAuthRsp,
    OSPTDEST* ospvDest)
{

    OSPTDEST* ospvAddedDest = OSPC_OSNULL;

    if (ospvAuthRsp != OSPC_OSNULL) {
        /* if this is called with a null destination, create a new
         * empty destination and add it to the list.
         */
        if (ospvDest == OSPC_OSNULL) {
            ospvAddedDest = OSPPDestNew();
        } else {
            ospvAddedDest = ospvDest;
        }

        if (ospvAddedDest != OSPC_OSNULL) {
            /*
             OSPM_MEMCPY(ospvAddedDest, ospvDest, sizeof(OSPTDEST));
             ospvAddedDest->ospmDestCallId = 
             OSPPCallIdNew(
                 OSPPCallIdGetSize(ospvDest->ospmDestCallId),
                 OSPPCallIdGetValue(ospvDest->ospmDestCallId));
            */

            OSPPListAppend(&(ospvAuthRsp->ospmAuthRspDest), ospvAddedDest);
        }
    }

    return ospvAddedDest;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspFromElement() - get authorisation response from an XML element
 *-----------------------------------------------------------------------*
 */
unsigned OSPPAuthRspFromElement(
    OSPTXMLELEM* ospvElem,      /* input is XML element */
    OSPTAUTHRSP** ospvAuthRsp)  /* where to put authorisation response pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLELEM* elem = OSPC_OSNULL;
    OSPTAUTHRSP* authrsp = OSPC_OSNULL;
    OSPTDEST* dest = OSPC_OSNULL;
    OSPTTIME t = 0L;
    OSPTTRXID transid = 0L;
    int len = 0;
    unsigned long delaylimit = 0L;
    unsigned long delaypref = 0L;
    unsigned char* compid = OSPC_OSNULL;
    OSPTXMLELEM* ospvParent = OSPC_OSNULL;
    unsigned char* messageId = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the authorisation response object */
        authrsp = OSPPAuthRspNew();

        if (authrsp == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_DATA_NO_AUTHRSP;
        } else {

            if (OSPPMsgGetElemPart(OSPPXMLElemGetName(ospvElem))==ospeElemMessage) {
                OSPPAuthRspMessageIdFromElement(ospvElem, &messageId);
                if(messageId != OSPC_OSNULL) {
                    OSPPAuthRspSetMessageId(authrsp, messageId);
                }

                /* ospvElem is pointing to the Message element. 
                 * The first child contains the Component element. 
                 * The following two lines of code change ospvElem from 
                 * pointing to the Message element to the Component element.
                 */
                ospvParent = ospvElem;
                ospvElem = (OSPTXMLELEM*)OSPPXMLElemFirstChild(ospvParent);
            }

            OSPPAuthRspComponentIdFromElement(ospvElem, &compid);
            if(compid != OSPC_OSNULL) {
                OSPPAuthRspSetComponentId(authrsp, compid);
            }
        }

        /*
         * The Authorisation Rsp element should consist of several
         * child elements. We'll run through what's there and pick out
         * the information we need.
         */

        for (elem = (OSPTXMLELEM*)OSPPXMLElemFirstChild(ospvElem);
            (elem != (OSPTXMLELEM*)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPTXMLELEM*)OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgGetElemPart(OSPPXMLElemGetName(elem))) {
                case ospeElemMessage:
                    OSPPAuthRspMessageIdFromElement(elem, &messageId);
                    if(messageId != OSPC_OSNULL) {
                        OSPPAuthRspSetMessageId(authrsp, messageId);
                    }
                break;

                /*
                ** ospeElemAuthRzp -- OSP Stds spell Authorization not Authorisation
                */
                case ospeElemAuthRzp:
                case ospeElemAuthRsp:
                    OSPPAuthRspComponentIdFromElement(elem, &compid);
                    if(compid != OSPC_OSNULL) {
                        OSPPAuthRspSetComponentId(authrsp, compid);
                    }
                break;
                case ospeElemTimestamp:
                    ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPAuthRspSetTimestamp(authrsp,t);
                    }
                    break;
                case ospeElemStatus:
                    if (authrsp->ospmAuthRspStatus != OSPC_OSNULL) {
                        OSPPStatusDelete(&(authrsp->ospmAuthRspStatus));
                    }
                    ospvErrCode = OSPPStatusFromElement(elem, &(authrsp->ospmAuthRspStatus));
                    break;
                case ospeElemTransId:
                    len = sizeof(OSPTTRXID);
                    ospvErrCode = OSPPMsgTXFromElement(elem, &transid);
                    OSPPAuthRspSetTrxId(authrsp, transid);
                    break;

                case ospeElemTNDelayLimit:
                    ospvErrCode = OSPPMsgNumFromElement(elem, &delaylimit);
                    OSPPAuthRspSetTNDelayLimit(authrsp, (unsigned)delaylimit);
                break;

                case ospeElemTNAudit:
                    ospvErrCode = OSPPTNAuditFromElement(elem, &(authrsp->ospmAuthRspTNAudit));
                    break;

                case ospeElemCSAuditTrigger:
                    ospvErrCode = OSPPCSAuditFromElement(elem, &(authrsp->ospmAuthRspCSAudit));
                    break;

                case ospeElemTNDelayPref:
                    ospvErrCode = OSPPMsgNumFromElement(elem, &delaypref);
                    OSPPAuthRspSetTNDelayPref(authrsp, (unsigned)delaypref);
                    break;

                case ospeElemDest:
                    ospvErrCode = OSPPDestFromElement(elem, &dest);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        if(dest != OSPC_OSNULL) {
                            OSPPAuthRspAddDest(authrsp,dest);
                            OSPPAuthRspIncNumDests(authrsp);
                            dest = OSPC_OSNULL;
                        }
                    }
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

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            *ospvAuthRsp = authrsp;
        }
    }

    return ospvErrCode;
}

OSPTCSAUDIT* OSPPAuthRspGetCSAudit(
    OSPTAUTHRSP* ospvAuthRsp)
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        if(ospvAuthRsp->ospmAuthRspCSAudit != OSPC_OSNULL) {
            return ospvAuthRsp->ospmAuthRspCSAudit;
        }
    }
    return (OSPTCSAUDIT*)OSPC_OSNULL;
}

int OSPPAuthRspHasCSAudit(
    OSPTAUTHRSP* ospvAuthRsp)
{
    return ospvAuthRsp->ospmAuthRspCSAudit != (OSPTCSAUDIT*)OSPC_OSNULL;
}

/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasMessageId() - is the message id set ?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasMessageId(
    OSPTAUTHRSP* ospvAuthRsp)
{
    return (ospvAuthRsp->ospmAuthRspMessageId != OSPC_OSNULL);
}

/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetMessageId() - creates space and copies in the string.
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetMessageId(     
    OSPTAUTHRSP* ospvAuthRsp,       /* In - pointer to Usage Indication struct */
    unsigned char* ospvMessageId)   /* In - pointer to message id string */
{
    int len = OSPM_STRLEN((const char*)ospvMessageId);

    if(ospvAuthRsp != OSPC_OSNULL) {
        if(ospvAuthRsp->ospmAuthRspMessageId != OSPC_OSNULL) {
            OSPM_FREE(ospvAuthRsp->ospmAuthRspMessageId);    
        }

        OSPM_MALLOC(ospvAuthRsp->ospmAuthRspMessageId, unsigned char, len + 1);
        OSPM_MEMSET(ospvAuthRsp->ospmAuthRspMessageId, 0, len + 1);
        OSPM_MEMCPY(ospvAuthRsp->ospmAuthRspMessageId, ospvMessageId, len);
    }
}

/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspMessageIdFromElement() - Get message id attribute from element.
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspMessageIdFromElement(
    OSPTXMLELEM* ospvElemIn, 
    unsigned char** ospvMessageId)
{
    OSPTXMLATTR* attr = (OSPTXMLATTR*)OSPC_OSNULL;

    /* look for the message id attribute */
    for (attr = (OSPTXMLATTR*)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != (OSPTXMLATTR*)OSPC_OSNULL);
        attr = (OSPTXMLATTR*)OSPPXMLElemNextAttr(ospvElemIn, attr))
    {
        if (OSPPMsgGetAttrPart(OSPPXMLAttrGetName(attr)) == ospeAttrMessageId) {
            /* we found the message attribute. Get the value */
            *ospvMessageId = (unsigned char*)OSPPXMLAttrGetValue(attr);
            break;
        }
    }
}

/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspComponentIdFromElement() - Get component id attribute from element.
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspComponentIdFromElement(
    OSPTXMLELEM* ospvElemIn, 
    unsigned char** ospvComponentId)
{
    OSPTXMLATTR* attr = (OSPTXMLATTR*)OSPC_OSNULL;

    /* look for the component id attribute */
    for (attr = (OSPTXMLATTR*)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != (OSPTXMLATTR*)OSPC_OSNULL);
        attr = (OSPTXMLATTR*)OSPPXMLElemNextAttr(ospvElemIn, attr))
    {

        if (OSPPMsgGetAttrPart(OSPPXMLAttrGetName(attr)) == ospeAttrComponentId) {
            /* we found the component attribute. Get the value */
            *ospvComponentId = (unsigned char*)OSPPXMLAttrGetValue(attr);
            break;
        }
    }
}

/*^L*/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasRole() - Does AuthResponse have role set?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasRole(
    OSPTAUTHRSP* ospvAuthRsp)
{
    int ospvHasRole = OSPC_FALSE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvHasRole = (ospvAuthRsp->ospmAuthRspHasRole != OSPC_FALSE);
    }
    return ospvHasRole;
}

/*^L*/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetRole() - sets value for role in AuthResponse
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetRole(
    OSPTAUTHRSP* ospvAuthRsp,
    OSPE_MSG_ROLETYPES ospvRole)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->ospmAuthRspRole = ospvRole;
        ospvAuthRsp->ospmAuthRspHasRole = OSPC_TRUE;
    }
}

/*^L*/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspGetRole() - returns role for an AuthResponse
 *-----------------------------------------------------------------------*
 */
OSPE_MSG_ROLETYPES OSPPAuthRspGetRole(
    OSPTAUTHRSP* ospvAuthRsp)
{
    OSPE_MSG_ROLETYPES ospvRole = OSPC_UNDEFINED_ROLE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvRole = (OSPE_MSG_ROLETYPES)ospvAuthRsp->ospmAuthRspRole;
    }
    return ospvRole;
}

