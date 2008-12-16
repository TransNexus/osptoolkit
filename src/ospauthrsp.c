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
 * OSPPAuthRspHasDelayLimit() - does an AuthResponse have delay limit?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasDelayLimit(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    int ospvHasDelayLimit = OSPC_FALSE;

    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvHasDelayLimit = (ospvAuthRsp->ospmAuthRspDelayLimit > 0);
    }
    return ospvHasDelayLimit;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetDelayLimit() - sets the delay limit for an authorisation
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetDelayLimit(
    OSPTAUTHRSP* ospvAuthRsp,   /* authorisation response */
    unsigned ospvDelayLimit)  /* delay limit to set */
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->ospmAuthRspDelayLimit = ospvDelayLimit;
    }
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspGetDelayLimit() - gets the delay limit for an AuthResponse
 *-----------------------------------------------------------------------*
 */
unsigned OSPPAuthRspGetDelayLimit(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    unsigned ospvDelayLimit = 0;

    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvDelayLimit = (ospvAuthRsp->ospmAuthRspDelayLimit);
    }
    return ospvDelayLimit;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspHasDelayPref() - does an AuthResponse have delay pref?
 *-----------------------------------------------------------------------*
 */
int OSPPAuthRspHasDelayPref(
    OSPTAUTHRSP* ospvAuthRsp)   /* authorisation response */
{
    int ospvHasDelayPref = OSPC_FALSE;

    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvHasDelayPref = (ospvAuthRsp->ospmAuthRspDelayPref > 0);
    }
    return ospvHasDelayPref;
}

/**/
/*
 *-----------------------------------------------------------------------*
 * OSPPAuthRspSetDelayPref() - sets the delay pref for an authorisation
 *-----------------------------------------------------------------------*
 */
void OSPPAuthRspSetDelayPref(
    OSPTAUTHRSP* ospvAuthRsp,   /* authorisation response */
    unsigned ospvDelayPref)   /* delay pref to set */
{
    if(ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->ospmAuthRspDelayPref = ospvDelayPref;
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
OSPTBOOL OSPPAuthRspHasComponentId(
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
    const char* ospvComponentId) /* In - pointer to component id string */
{
    int len = OSPM_STRLEN(ospvComponentId);

    if(ospvAuthRsp != OSPC_OSNULL) {
        if(ospvAuthRsp->ospmAuthRspComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvAuthRsp->ospmAuthRspComponentId);    
        }

        OSPM_MALLOC(ospvAuthRsp->ospmAuthRspComponentId, char, len + 1);
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
    OSPT_XML_ELEM* ospvElem,      /* input is XML element */
    OSPTAUTHRSP** ospvAuthRsp)  /* where to put authorisation response pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM* elem = OSPC_OSNULL;
    OSPTAUTHRSP* authrsp = OSPC_OSNULL;
    OSPTDEST* dest = OSPC_OSNULL;
    OSPTTIME t = 0L;
    OSPTTRXID transid = 0L;
    int len = 0;
    unsigned long delaylimit = 0L;
    unsigned long delaypref = 0L;
    const char* messageId = OSPC_OSNULL;
    const char* compid = OSPC_OSNULL;
    OSPT_XML_ELEM* ospvParent = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the authorisation response object */
        authrsp = OSPPAuthRspNew();

        if (authrsp == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_DATA_NO_AUTHRSP;
        } else {

            if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem))==OSPC_MELEM_MESSAGE) {
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
                ospvElem = (OSPT_XML_ELEM*)OSPPXMLElemFirstChild(ospvParent);
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

        for (elem = (OSPT_XML_ELEM*)OSPPXMLElemFirstChild(ospvElem);
            (elem != (OSPT_XML_ELEM*)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPT_XML_ELEM*)OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
                case OSPC_MELEM_MESSAGE:
                    OSPPAuthRspMessageIdFromElement(elem, &messageId);
                    if(messageId != OSPC_OSNULL) {
                        OSPPAuthRspSetMessageId(authrsp, messageId);
                    }
                break;

                /*
                ** OSPC_MELEM_AUTHRZP -- OSP Stds spell Authorization not Authorisation
                */
                case OSPC_MELEM_AUTHRZP:
                case OSPC_MELEM_AUTHRSP:
                    OSPPAuthRspComponentIdFromElement(elem, &compid);
                    if(compid != OSPC_OSNULL) {
                        OSPPAuthRspSetComponentId(authrsp, compid);
                    }
                break;
                case OSPC_MELEM_TIMESTAMP:
                    ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        OSPPAuthRspSetTimestamp(authrsp,t);
                    }
                    break;
                case OSPC_MELEM_STATUS:
                    if (authrsp->ospmAuthRspStatus != OSPC_OSNULL) {
                        OSPPStatusDelete(&(authrsp->ospmAuthRspStatus));
                    }
                    ospvErrCode = OSPPStatusFromElement(elem, &(authrsp->ospmAuthRspStatus));
                    break;
                case OSPC_MELEM_TRANSID:
                    len = sizeof(OSPTTRXID);
                    ospvErrCode = OSPPMsgTXFromElement(elem, &transid);
                    OSPPAuthRspSetTrxId(authrsp, transid);
                    break;

                case OSPC_MELEM_DELAYLIMIT:
                    ospvErrCode = OSPPMsgNumFromElement(elem, &delaylimit);
                    OSPPAuthRspSetDelayLimit(authrsp, (unsigned)delaylimit);
                break;

                case OSPC_MELEM_AUDIT:
                    ospvErrCode = OSPPTNAuditFromElement(elem, &(authrsp->ospmAuthRspTNAudit));
                    break;

                case OSPC_MELEM_CSAUDITTRIGGER:
                    ospvErrCode = OSPPCSAuditFromElement(elem, &(authrsp->ospmAuthRspCSAudit));
                    break;

                case OSPC_MELEM_DELAYPREF:
                    ospvErrCode = OSPPMsgNumFromElement(elem, &delaypref);
                    OSPPAuthRspSetDelayPref(authrsp, (unsigned)delaypref);
                    break;

                case OSPC_MELEM_DEST:
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
OSPTBOOL OSPPAuthRspHasMessageId(
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
    const char* ospvMessageId)   /* In - pointer to message id string */
{
    unsigned len = OSPM_STRLEN(ospvMessageId);

    if(ospvAuthRsp != OSPC_OSNULL) {
        if(ospvAuthRsp->ospmAuthRspMessageId != OSPC_OSNULL) {
            OSPM_FREE(ospvAuthRsp->ospmAuthRspMessageId);    
        }

        OSPM_MALLOC(ospvAuthRsp->ospmAuthRspMessageId, char, len + 1);
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
    OSPT_XML_ELEM* ospvElemIn, 
    const char** ospvMessageId)
{
    OSPT_XML_ATTR* attr = (OSPT_XML_ATTR*)OSPC_OSNULL;

    /* look for the message id attribute */
    for (attr = (OSPT_XML_ATTR*)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != (OSPT_XML_ATTR*)OSPC_OSNULL);
        attr = (OSPT_XML_ATTR*)OSPPXMLElemNextAttr(ospvElemIn, attr))
    {
        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_MESSAGEID) {
            /* we found the message attribute. Get the value */
            *ospvMessageId = OSPPXMLAttrGetValue(attr);
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
    OSPT_XML_ELEM* ospvElemIn, 
    const char** ospvComponentId)
{
    OSPT_XML_ATTR* attr = (OSPT_XML_ATTR*)OSPC_OSNULL;

    /* look for the component id attribute */
    for (attr = (OSPT_XML_ATTR*)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != (OSPT_XML_ATTR*)OSPC_OSNULL);
        attr = (OSPT_XML_ATTR*)OSPPXMLElemNextAttr(ospvElemIn, attr))
    {

        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_COMPONENTID) {
            /* we found the component attribute. Get the value */
            *ospvComponentId = OSPPXMLAttrGetValue(attr);
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
    OSPE_MSG_ROLE ospvRole)
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
OSPE_MSG_ROLE OSPPAuthRspGetRole(
    OSPTAUTHRSP* ospvAuthRsp)
{
    OSPE_MSG_ROLE ospvRole = OSPC_MROLE_UNDEFINED;

    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvRole = (OSPE_MSG_ROLE)ospvAuthRsp->ospmAuthRspRole;
    }
    return ospvRole;
}

