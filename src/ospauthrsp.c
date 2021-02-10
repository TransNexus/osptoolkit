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

/* ospauthrsp.c - OSP authorization response functions */

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

/*
 * OSPPAuthRspSetTimestamp() - sets the timestamp for an AuthResponse
 */
void OSPPAuthRspSetTimestamp(
    OSPT_AUTH_RSP *ospvAuthRsp,
    OSPTTIME ospvTime)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->Timestamp = ospvTime;
    }
}

/*
 * OSPPAuthRspHasStatus() - does the AuthResponse have a status?
 */
OSPTBOOL OSPPAuthRspHasStatus(
    OSPT_AUTH_RSP *ospvAuthRsp) /* authorization response effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        has = (ospvAuthRsp->Status != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthRspGetStatus() - returns the status for an AuthResponse
 */
OSPT_STATUS* OSPPAuthRspGetStatus(
    OSPT_AUTH_RSP *ospvAuthRsp) /* authorization response */
{
    OSPT_STATUS *status = OSPC_OSNULL;

    if (ospvAuthRsp != OSPC_OSNULL) {
        status = ospvAuthRsp->Status;
    }

    return status;
}

/*
 * OSPPAuthRspSetTrxId() - sets the transaction ID for an authorization
 */
void OSPPAuthRspSetTrxId(
    OSPT_AUTH_RSP *ospvAuthRsp, /* authorization response */
    OSPTTRXID ospvTrxId)        /* transaction ID to set */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->TrxId = ospvTrxId;
    }
}

/*
 * OSPPAuthRspHasDest() - does the AuthResponse have a destination?
 */
OSPTBOOL OSPPAuthRspHasDest(
    OSPT_AUTH_RSP *ospvAuthRsp) /* authorization response effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        has = (ospvAuthRsp->Destination != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPAuthRspFirstDest() - returns the first destination for an AuthResponse
 */
OSPT_DEST* OSPPAuthRspFirstDest(
    OSPT_AUTH_RSP *ospvAuthRsp) /* authorization response */
{
    OSPT_DEST* dest = OSPC_OSNULL;

    if (ospvAuthRsp != OSPC_OSNULL) {
        dest = (OSPT_DEST*)OSPPListFirst(&(ospvAuthRsp->Destination));
    }

    return dest;
}

/*
 * OSPPAuthRspNextDest() - returns the next destination for an AuthResponse
 */
OSPT_DEST* OSPPAuthRspNextDest(
    OSPT_AUTH_RSP *ospvAuthRsp, /* authorization response */
    OSPT_DEST *ospvDest)
{
    OSPT_DEST* dest = OSPC_OSNULL;

    if (ospvAuthRsp != OSPC_OSNULL) {
        dest = (OSPT_DEST*)OSPPListNext(&(ospvAuthRsp->Destination), ospvDest);
    }

    return dest;
}

/*
 * OSPPAuthRspHasDelayLimit() - does an AuthResponse have delay limit?
 */
OSPTBOOL OSPPAuthRspHasDelayLimit(
    OSPT_AUTH_RSP *ospvAuthRsp) /* authorization response */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        has = (ospvAuthRsp->DelayLimit > 0);
    }

    return has;
}

/*
 * OSPPAuthRspSetDelayLimit() - sets the delay limit for an authorization
 */
void OSPPAuthRspSetDelayLimit(
    OSPT_AUTH_RSP *ospvAuthRsp, /* authorization response */
    unsigned ospvDelayLimit)    /* delay limit to set */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->DelayLimit = ospvDelayLimit;
    }
}

/*
 * OSPPAuthRspGetDelayLimit() - gets the delay limit for an AuthResponse
 */
unsigned OSPPAuthRspGetDelayLimit(
    OSPT_AUTH_RSP *ospvAuthRsp) /* authorization response */
{
    unsigned limit = 0;

    if (ospvAuthRsp != OSPC_OSNULL) {
        limit = ospvAuthRsp->DelayLimit;
    }

    return limit;
}

/*
 * OSPPAuthRspHasDelayPref() - does an AuthResponse have delay pref?
 */
OSPTBOOL OSPPAuthRspHasDelayPref(
    OSPT_AUTH_RSP *ospvAuthRsp) /* authorization response */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        has = (ospvAuthRsp->DelayPref > 0);
    }

    return has;
}

/*
 * OSPPAuthRspSetDelayPref() - sets the delay pref for an authorization
 */
void OSPPAuthRspSetDelayPref(
    OSPT_AUTH_RSP *ospvAuthRsp, /* authorization response */
    unsigned ospvDelayPref)     /* delay pref to set */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->DelayPref = ospvDelayPref;
    }
}

OSPTBOOL OSPPAuthRspHasNumDests(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return ospvAuthRsp->NumDestination;
    } else {
        return OSPC_FALSE;
    }
}

void OSPPAuthRspIncNumDests(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->NumDestination++;
    }
}

unsigned OSPPAuthRspGetNumDests(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return ospvAuthRsp->NumDestination;
    } else {
        return 0;
    }
}

/*
 * OSPPAuthRspHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPAuthRspHasComponentId(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return(ospvAuthRsp->ComponentId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthRspSetComponentId() - creates space and copies in the string.
 */
void OSPPAuthRspSetComponentId(
    OSPT_AUTH_RSP *ospvAuthRsp,     /* In - pointer to Usage Indication struct */
    const char *ospvComponentId)    /* In - pointer to component id string */
{
    int len = OSPM_STRLEN(ospvComponentId);

    if (ospvAuthRsp != OSPC_OSNULL) {
        if (ospvAuthRsp->ComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvAuthRsp->ComponentId);
        }

        OSPM_MALLOC(ospvAuthRsp->ComponentId, char, len + 1);
        OSPM_MEMSET(ospvAuthRsp->ComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvAuthRsp->ComponentId, ospvComponentId, len);
    }
}

OSPT_TN_AUDIT* OSPPAuthRspGetTNAudit(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return ospvAuthRsp->TNAudit;
    } else {
        return OSPC_OSNULL;
    }
}

/*
 * OSPPAuthRspNew() - creates a new (empty) authorization response
 */
OSPT_AUTH_RSP* OSPPAuthRspNew(void)
{
    OSPT_AUTH_RSP *authrsp;

    OSPM_MALLOC(authrsp, OSPT_AUTH_RSP,sizeof(OSPT_AUTH_RSP));
    if (authrsp != OSPC_OSNULL) {
        OSPM_MEMSET(authrsp, 0, sizeof(OSPT_AUTH_RSP));

        authrsp->Timestamp = OSPC_TIMEMIN;
        authrsp->Status = OSPC_OSNULL;
        authrsp->CSAudit = OSPC_OSNULL;
        OSPPListNew(&(authrsp->Destination));
        authrsp->Verstat = OSPC_VSTATUS_UNKNOWN;
    }

    return authrsp;
}

/*
 * OSPPAuthRspDelete() - deletes an authorization response structure
 */
void OSPPAuthRspDelete(
    OSPT_AUTH_RSP **ospvAuthRsp)
{
    OSPT_DEST* dest = OSPC_OSNULL;

    if (*ospvAuthRsp != OSPC_OSNULL) {
        /* first remove the status */
        if ((*ospvAuthRsp)->Status != OSPC_OSNULL) {
            OSPPStatusDelete(&((*ospvAuthRsp)->Status));
        }

        /* remove tnaudit */
        if ((*ospvAuthRsp)->TNAudit != OSPC_OSNULL) {
            OSPPTNAuditDelete(&((*ospvAuthRsp)->TNAudit));
        }

        /* remove csaudit */
        if ((*ospvAuthRsp)->CSAudit != OSPC_OSNULL) {
            OSPPCSAuditDelete(&((*ospvAuthRsp)->CSAudit));
        }

        /* remove messageId */
        if (OSPPAuthRspHasMessageId(*ospvAuthRsp)) {
            OSPM_FREE((*ospvAuthRsp)->MessageId);
            (*ospvAuthRsp)->MessageId = OSPC_OSNULL;
        }

        /* remove componentId */
        if (OSPPAuthRspHasComponentId(*ospvAuthRsp)) {
            OSPM_FREE((*ospvAuthRsp)->ComponentId);
            (*ospvAuthRsp)->ComponentId = OSPC_OSNULL;
        }

        /* now remove any attached dests -- this means following the chain */
        while (!OSPPListEmpty(&((*ospvAuthRsp)->Destination))) {
            dest = (OSPT_DEST*)OSPPListRemove(&((*ospvAuthRsp)->Destination));
            if (dest != OSPC_OSNULL) {
                OSPPDestDelete(&dest);
                dest = OSPC_OSNULL;
            }
        }

        /* remove Identity */
        if (OSPPAuthRspHasIdentity(*ospvAuthRsp)) {
            OSPM_FREE((*ospvAuthRsp)->Identity);
            (*ospvAuthRsp)->Identity = OSPC_OSNULL;
        }

        /* now delete the authrsp itself */
        OSPM_FREE(*ospvAuthRsp);
        *ospvAuthRsp = OSPC_OSNULL;
    }
}

/*
 * OSPPAuthRspAddDest() - add a destination to authrsp. If ospvDest is NULL,
 * adds a new, empty destination.
 * Returns: pointer to added dest for manipulation
 */
OSPT_DEST* OSPPAuthRspAddDest(
    OSPT_AUTH_RSP *ospvAuthRsp,
    OSPT_DEST *ospvDest)
{

    OSPT_DEST *ospvAddedDest = OSPC_OSNULL;

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
             OSPM_MEMCPY(ospvAddedDest, ospvDest, sizeof(OSPT_DEST));
             ospvAddedDest->ospmDestCallId =
             OSPPCallIdNew(
                 OSPPCallIdGetSize(ospvDest->ospmDestCallId),
                 OSPPCallIdGetValue(ospvDest->ospmDestCallId));
            */

            OSPPListAppend(&(ospvAuthRsp->Destination), ospvAddedDest);
        }
    }

    return ospvAddedDest;
}

/*
 * OSPPAuthRspFromElement() - get authorization response from an XML element
 */
unsigned OSPPAuthRspFromElement(
    OSPT_XML_ELEM *ospvElem,        /* input is XML element */
    OSPT_AUTH_RSP **ospvAuthRsp)    /* where to put authorization response pointer */
{
    unsigned error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM* elem = OSPC_OSNULL;
    OSPT_AUTH_RSP* authrsp = OSPC_OSNULL;
    OSPT_DEST* dest = OSPC_OSNULL;
    OSPTTIME t = 0L;
    OSPTTRXID transid = 0L;
    unsigned long delaylimit = 0L;
    unsigned long delaypref = 0L;
    const char* messageId = OSPC_OSNULL;
    const char* compid = OSPC_OSNULL;
    OSPT_XML_ELEM *ospvParent = OSPC_OSNULL;
    const char* identity = OSPC_OSNULL;
    const char* verstat = OSPC_OSNULL;
    const char* attest = OSPC_OSNULL;
    const char* origid = OSPC_OSNULL;
    const char* jdtype = OSPC_OSNULL;
    unsigned headerIndex = 0;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvAuthRsp == OSPC_OSNULL) {
            error = OSPC_ERR_DATA_NO_AUTHRSP;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* create the authorization response object */
        authrsp = OSPPAuthRspNew();
        if (authrsp == OSPC_OSNULL) {
            error = OSPC_ERR_DATA_NO_AUTHRSP;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem))==OSPC_MELEM_MESSAGE) {
            OSPPAuthRspMessageIdFromElement(ospvElem, &messageId);
            if (messageId != OSPC_OSNULL) {
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
        if (compid != OSPC_OSNULL) {
            OSPPAuthRspSetComponentId(authrsp, compid);
        }

        /*
         * The Authorization Rsp element should consist of several
         * child elements. We'll run through what's there and pick out
         * the information we need.
         */
        for (elem = (OSPT_XML_ELEM*)OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR);
            elem = (OSPT_XML_ELEM*)OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_MESSAGE:
                OSPPAuthRspMessageIdFromElement(elem, &messageId);
                if (messageId != OSPC_OSNULL) {
                    OSPPAuthRspSetMessageId(authrsp, messageId);
                }
                break;

            /* OSPC_MELEM_AUTHRZP -- OSP Stds spell Authorization not Authorization */
            case OSPC_MELEM_AUTHRZP:
            case OSPC_MELEM_AUTHRSP:
                OSPPAuthRspComponentIdFromElement(elem, &compid);
                if (compid != OSPC_OSNULL) {
                    OSPPAuthRspSetComponentId(authrsp, compid);
                }
                break;
            case OSPC_MELEM_TIMESTAMP:
                if ((error = OSPPMsgTimeFromElement(elem, &t)) == OSPC_ERR_NO_ERROR) {
                    OSPPAuthRspSetTimestamp(authrsp,t);
                }
                break;
            case OSPC_MELEM_STATUS:
                if (authrsp->Status != OSPC_OSNULL) {
                    OSPPStatusDelete(&(authrsp->Status));
                }
                error = OSPPStatusFromElement(elem, &(authrsp->Status));
                break;
            case OSPC_MELEM_TRANSID:
                if ((error = OSPPMsgTXFromElement(elem, &transid)) == OSPC_ERR_NO_ERROR) {
                    OSPPAuthRspSetTrxId(authrsp, transid);
                }
                break;
            case OSPC_MELEM_DELAYLIMIT:
                if ((error = OSPPMsgNumFromElement(elem, &delaylimit)) == OSPC_ERR_NO_ERROR) {
                    OSPPAuthRspSetDelayLimit(authrsp, (unsigned)delaylimit);
                }
                break;
            case OSPC_MELEM_AUDIT:
                error = OSPPTNAuditFromElement(elem, &(authrsp->TNAudit));
                break;
            case OSPC_MELEM_CSAUDITTRIGGER:
                error = OSPPCSAuditFromElement(elem, &(authrsp->CSAudit));
                break;
            case OSPC_MELEM_DELAYPREF:
                if ((error = OSPPMsgNumFromElement(elem, &delaypref)) == OSPC_ERR_NO_ERROR) {
                    OSPPAuthRspSetDelayPref(authrsp, (unsigned)delaypref);
                }
                break;
            case OSPC_MELEM_DEST:
                if ((error = OSPPDestFromElement(elem, &dest)) == OSPC_ERR_NO_ERROR) {
                    if (dest != OSPC_OSNULL) {
                        OSPPAuthRspAddDest(authrsp, dest);
                        OSPPAuthRspIncNumDests(authrsp);
                        dest = OSPC_OSNULL;
                    }
                }
                break;
            case OSPC_MELEM_IDENTITY:
                if ((identity = OSPPXMLElemGetValue(elem)) != OSPC_OSNULL) {
                    OSPPAuthRspSetIdentity(authrsp, identity);
                }
                break;
            case OSPC_MELEM_TERMCAUSE:
                error = OSPPTermCauseFromElement(elem, &(authrsp->TermCause));
                break;
            case OSPC_MELEM_STATUSHEADER:
                error = OSPPStatusHeadersFromElement(elem, &(authrsp->StatusHeaders), headerIndex);
                if (error == OSPC_ERR_NO_ERROR) {
                    headerIndex++;
                }
                break;
            case OSPC_MELEM_VERSTAT:
                if ((verstat = OSPPXMLElemGetValue(elem)) != OSPC_OSNULL) {
                    OSPPAuthRspSetVerstat(authrsp, OSPPVerstatGetPart(verstat));
                }
                break;
            case OSPC_MELEM_ATTEST:
                if ((attest = OSPPXMLElemGetValue(elem)) != OSPC_OSNULL) {
                    OSPPAuthRspSetAttest(authrsp, attest);
                }
                break;
            case OSPC_MELEM_ORIGID:
                if ((origid = OSPPXMLElemGetValue(elem)) != OSPC_OSNULL) {
                    OSPPAuthRspSetOrigId(authrsp, origid);
                }
                break;
            case OSPC_MELEM_JURISDICTIONTYPE:
                if ((jdtype = OSPPXMLElemGetValue(elem)) != OSPC_OSNULL) {
                    OSPPAuthRspSetJurisdictionType(authrsp, jdtype);
                }
                break;
            default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem)) {
                    error = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        *ospvAuthRsp = authrsp;
    } else {
        if (authrsp != OSPC_OSNULL) {
            OSPPAuthRspDelete(&authrsp);
        }
    }

    return error;
}

OSPTCSAUDIT* OSPPAuthRspGetCSAudit(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        if (ospvAuthRsp->CSAudit != OSPC_OSNULL) {
            return ospvAuthRsp->CSAudit;
        }
    }
    return OSPC_OSNULL;
}

OSPTBOOL OSPPAuthRspHasCSAudit(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return ospvAuthRsp->CSAudit != OSPC_OSNULL;
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthRspHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPAuthRspHasMessageId(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return(ospvAuthRsp->MessageId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthRspSetMessageId() - creates space and copies in the string.
 */
void OSPPAuthRspSetMessageId(
    OSPT_AUTH_RSP *ospvAuthRsp, /* In - pointer to Usage Indication struct */
    const char *ospvMessageId)  /* In - pointer to message id string */
{
    unsigned len = OSPM_STRLEN(ospvMessageId);

    if (ospvAuthRsp != OSPC_OSNULL) {
        if (ospvAuthRsp->MessageId != OSPC_OSNULL) {
            OSPM_FREE(ospvAuthRsp->MessageId);
        }

        OSPM_MALLOC(ospvAuthRsp->MessageId, char, len + 1);
        OSPM_MEMSET(ospvAuthRsp->MessageId, 0, len + 1);
        OSPM_MEMCPY(ospvAuthRsp->MessageId, ospvMessageId, len);
    }
}

/*
 * OSPPAuthRspMessageIdFromElement() - Get message id attribute from element.
 */
void OSPPAuthRspMessageIdFromElement(
    OSPT_XML_ELEM *ospvElemIn,
    const char **ospvMessageId)
{
    OSPT_XML_ATTR* attr = OSPC_OSNULL;

    /* look for the message id attribute */
    for (attr = (OSPT_XML_ATTR*)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != OSPC_OSNULL);
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
 * OSPPAuthRspComponentIdFromElement() - Get component id attribute from element.
 */
void OSPPAuthRspComponentIdFromElement(
    OSPT_XML_ELEM *ospvElemIn,
    const char **ospvComponentId)
{
    OSPT_XML_ATTR* attr = OSPC_OSNULL;

    /* look for the component id attribute */
    for (attr = (OSPT_XML_ATTR*)OSPPXMLElemFirstAttr(ospvElemIn);
        (attr != OSPC_OSNULL);
        attr = (OSPT_XML_ATTR*)OSPPXMLElemNextAttr(ospvElemIn, attr))
    {

        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_COMPONENTID) {
            /* we found the component attribute. Get the value */
            *ospvComponentId = OSPPXMLAttrGetValue(attr);
            break;
        }
    }
}

/*
 * OSPPAuthRspHasRole() - Does AuthResponse have role set?
 */
OSPTBOOL OSPPAuthRspHasRole(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvAuthRsp != OSPC_OSNULL) {
        has = (ospvAuthRsp->Role != OSPC_FALSE);
    }

    return has;
}

/*
 * OSPPAuthRspSetRole() - sets value for role in AuthResponse
 */
void OSPPAuthRspSetRole(
    OSPT_AUTH_RSP *ospvAuthRsp,
    OSPE_ROLE ospvRole)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->Role = ospvRole;
        ospvAuthRsp->HasRole = OSPC_TRUE;
    }
}

/*
 * OSPPAuthRspGetRole() - returns role for an AuthResponse
 */
OSPE_ROLE OSPPAuthRspGetRole(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    OSPE_ROLE role = OSPC_ROLE_UNDEFINED;

    if (ospvAuthRsp != OSPC_OSNULL) {
        role = ospvAuthRsp->Role;
    }

    return role;
}

/*
 * OSPPAuthRspHasIdentity() - is the Identity set ?
 */
OSPTBOOL OSPPAuthRspHasIdentity(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return(ospvAuthRsp->Identity != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPAuthRspSetIdentity() - creates space and copies in the string.
 */
void OSPPAuthRspSetIdentity(
    OSPT_AUTH_RSP *ospvAuthRsp, /* In - pointer to AuthRsp struct */
    const char *ospvIdentity)   /* In - pointer to Identity string */
{
    unsigned len = OSPM_STRLEN(ospvIdentity);

    if (ospvAuthRsp != OSPC_OSNULL) {
        if (ospvAuthRsp->Identity != OSPC_OSNULL) {
            OSPM_FREE(ospvAuthRsp->Identity);
        }

        OSPM_MALLOC(ospvAuthRsp->Identity, char, len + 1);
        OSPM_MEMSET(ospvAuthRsp->Identity, 0, len + 1);
        OSPM_MEMCPY(ospvAuthRsp->Identity, ospvIdentity, len);
    }
}

void OSPPAuthRspSetTermCause(
    OSPT_AUTH_RSP *ospvAuthRsp, /* In - pointer to AuthRsp struct */
    OSPE_TERM_CAUSE ospvType,   /* In - TC type */
    unsigned ospvTCCode,        /* In - TC code */
    const char *ospvTCDesc)     /* In - TC description */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        OSPPSetTermCause(&ospvAuthRsp->TermCause, ospvType, ospvTCCode, ospvTCDesc);
    }
}

/*
 * OSPPAuthRspHasVerstat() - is the verification status set ?
 */
OSPTBOOL OSPPAuthRspHasVerstat(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return((ospvAuthRsp->Verstat >= OSPC_VSTATUS_START) && (ospvAuthRsp->Verstat < OSPC_VSTATUS_NUMBER));
    } else {
        return OSPC_FALSE;
    }
}

void OSPPAuthRspSetVerstat(
    OSPT_AUTH_RSP *ospvAuthRsp,             /* In - pointer to AuthRsp struct */
    OSPE_VERIFICATION_STATUS ospvStatus)    /* In - verification status */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->Verstat = ospvStatus;
    }
}

OSPTBOOL OSPPAuthRspHasAttest(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return(ospvAuthRsp->Attest != '\0');
    } else {
        return OSPC_FALSE;
    }
}

void OSPPAuthRspSetAttest(
    OSPT_AUTH_RSP *ospvAuthRsp, /* In - pointer to AuthRsp struct */
    const char *ospvAttest)     /* In - Attestation */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        ospvAuthRsp->Attest = toupper(ospvAttest[0]);
    }
}

OSPTBOOL OSPPAuthRspHasOrigId(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return(ospvAuthRsp->OrigId[0] != '\0');
    } else {
        return OSPC_FALSE;
    }
}

void OSPPAuthRspSetOrigId(
    OSPT_AUTH_RSP *ospvAuthRsp, /* In - pointer to AuthRsp struct */
    const char *ospvOrigId)     /* In - Origination ID */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        OSPM_STRNCPY(ospvAuthRsp->OrigId, ospvOrigId, sizeof(ospvAuthRsp->OrigId));
    }
}

OSPTBOOL OSPPAuthRspHasJurisdictionType(
    OSPT_AUTH_RSP *ospvAuthRsp)
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        return(ospvAuthRsp->JurisdictionType[0] != '\0');
    } else {
        return OSPC_FALSE;
    }
}

void OSPPAuthRspSetJurisdictionType(
    OSPT_AUTH_RSP *ospvAuthRsp,         /* In - pointer to AuthRsp struct */
    const char *ospvJurisdictionType)   /* In - Jurisdiction type */
{
    if (ospvAuthRsp != OSPC_OSNULL) {
        OSPM_STRNCPY(ospvAuthRsp->JurisdictionType, ospvJurisdictionType, sizeof(ospvAuthRsp->JurisdictionType));
    }
}

