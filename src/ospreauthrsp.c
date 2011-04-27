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

/* ospreauthrsp.c - OSP reauthorization response functions */

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

/*
 * OSPPReauthRspSetTimestamp() - sets the timestamp for an reauthorization response
 */
void OSPPReauthRspSetTimestamp( /* nothing returned */
    OSPT_REAUTH_RSP *ospvReauthRsp,
    OSPTTIME ospvTime)
{
    if (ospvReauthRsp != OSPC_OSNULL) {
        ospvReauthRsp->Timestamp = ospvTime;
    }
}

/*
 * OSPPReauthRspHasStatus() - does the reauthorization response have a status?
 */
OSPTBOOL OSPPReauthRspHasStatus(    /* returns non-zero if number exists */
    OSPT_REAUTH_RSP *ospvReauthRsp) /* reauthorization response effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthRsp != OSPC_OSNULL) {
        has = (ospvReauthRsp->Status != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPReauthRspGetStatus() - returns the status for an
 * reauthorization response
 */
OSPT_STATUS *OSPPReauthRspGetStatus(/* returns pointer to dest */
    OSPT_REAUTH_RSP *ospvReauthRsp  /* reauthorization response */
    )
{
    OSPT_STATUS *status = OSPC_OSNULL;

    if (ospvReauthRsp != OSPC_OSNULL) {
        status = ospvReauthRsp->Status;
    }

    return status;
}

/*
 * OSPPReauthRspSetTrxId() - sets the transaction ID for an reauthorization
 */
void OSPPReauthRspSetTrxId(         /* nothing returned */
    OSPT_REAUTH_RSP *ospvReauthRsp, /* reauthorization response */
    OSPTTRXID ospvTrxId)            /* transaction ID to set */
{
    if (ospvReauthRsp != OSPC_OSNULL) {
        ospvReauthRsp->TrxId = ospvTrxId;
    }
}

/*
 * OSPPReauthRspHasDest() - does the reauthorization response have a destination?
 */
OSPTBOOL OSPPReauthRspHasDest(      /* returns non-zero if number exists */
    OSPT_REAUTH_RSP *ospvReauthRsp) /* reauthorization response effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvReauthRsp != OSPC_OSNULL) {
        has = (ospvReauthRsp->Destination != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPReauthRspSetDest() - set the destination for an
 * reauthorization response
 */
void OSPPReauthRspSetDest(          /* nothing returned */
    OSPT_REAUTH_RSP *ospvReauthRsp, /* reauthorization response  to set */
    OSPT_DEST *ospvDest)            /* destination */
{
    if (ospvReauthRsp != OSPC_OSNULL) {
        if (ospvDest != OSPC_OSNULL) {
            ospvReauthRsp->Destination = ospvDest;
        }
    }
}

/*
 * OSPPReauthRspHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPReauthRspHasComponentId(   /* returns non-zero if component id is set */
    OSPT_REAUTH_RSP *ospvReauthRsp)
{
    if (ospvReauthRsp != OSPC_OSNULL) {
        return (ospvReauthRsp->ComponentId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

OSPT_TN_AUDIT *OSPPReauthRspGetTNAudit(
    OSPT_REAUTH_RSP *ospvReauthRsp)
{
    if (ospvReauthRsp != OSPC_OSNULL) {
        return ospvReauthRsp->TNAudit;
    } else {
        return OSPC_OSNULL;
    }
}

/*
 * OSPPReauthRspNew() - creates a new (empty) reauthorization response
 */
OSPT_REAUTH_RSP *OSPPReauthRspNew(void) /* returns pointer or NULL */
{
    OSPT_REAUTH_RSP *ospvReauthRsp;

    OSPM_MALLOC(ospvReauthRsp, OSPT_REAUTH_RSP, sizeof(OSPT_REAUTH_RSP));
    OSPM_MEMSET(ospvReauthRsp, 0, sizeof(ospvReauthRsp));

    if (ospvReauthRsp != OSPC_OSNULL) {
        ospvReauthRsp->Timestamp = OSPC_TIMEMIN;
        ospvReauthRsp->Status = OSPC_OSNULL;
        ospvReauthRsp->TrxId = 0;
        ospvReauthRsp->Destination = OSPC_OSNULL;
        ospvReauthRsp->TNAudit = OSPC_OSNULL;
        ospvReauthRsp->ComponentId = OSPC_OSNULL;
        ospvReauthRsp->MessageId = OSPC_OSNULL;
    }

    return ospvReauthRsp;
}

/*
 * OSPPReauthRspFromElement() - get reauthorization response from an XML element
 */
int OSPPReauthRspFromElement(       /* returns error code */
    OSPT_XML_ELEM *ospvElem,        /* input is XML element */
    OSPT_REAUTH_RSP **ospvReauthRsp)/* where to put reauthorization response pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_REAUTH_RSP *reauthrsp = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPTTIME t = 0L;
    OSPTTRXID transid = 0L;
    OSPT_XML_ELEM *parent = OSPC_OSNULL;
    const char *messageId = OSPC_OSNULL;
    const char *compid = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvReauthRsp == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NO_REAUTHRSP;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {

        if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_MESSAGE) {
            OSPPReauthRspMessageIdFromElement(ospvElem, &messageId);
            OSPPReauthRspSetMessageId(reauthrsp, messageId);

            /* ospvElem is pointing to the Message element.
             * The first child contains the Component element.
             * The following two lines of code change ospvElem from
             * pointing to the Message element to the Component element.
             */
            parent = ospvElem;
            ospvElem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(parent);
        }

        /* create the reauthorization response object */
        reauthrsp = OSPPReauthRspNew();

        if (reauthrsp == OSPC_OSNULL) {

            errcode = OSPC_ERR_DATA_NO_REAUTHRSP;
        } else {
            /* Have to get ComponentId here */
            if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_REAUTHREQ) {
                OSPPReauthRspComponentIdFromElement(ospvElem, &compid);
                if (compid != OSPC_OSNULL) {
                    OSPPReauthRspSetComponentId(reauthrsp, compid);
                }
            }
        }
    }
    /*
     * The Reauthorization Response element should consist of several
     * child elements. We'll run through what's there and pick out
     * the information we need.
     */
    if (errcode == OSPC_ERR_NO_ERROR) {
        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
             (elem != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR); elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, elem)) {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_MESSAGE:
                if (messageId != OSPC_OSNULL) {
                    OSPPReauthRspSetMessageId(reauthrsp, messageId);
                }
                break;

            case OSPC_MELEM_REAUTHREQ:
                if (compid != OSPC_OSNULL) {
                    OSPPReauthRspSetComponentId(reauthrsp, compid);
                }
                break;

            case OSPC_MELEM_TIMESTAMP:
                errcode = OSPPMsgTimeFromElement(elem, &t);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPReauthRspSetTimestamp(reauthrsp, t);
                }
                break;

            case OSPC_MELEM_STATUS:
                if (reauthrsp->Status == OSPC_OSNULL) {
                    errcode = OSPPStatusFromElement(elem, &(reauthrsp->Status));
                }
                break;

            case OSPC_MELEM_TRANSID:
                errcode = OSPPMsgTXFromElement(elem, &transid);
                OSPPReauthRspSetTrxId(reauthrsp, transid);
                break;

            case OSPC_MELEM_AUDIT:
                errcode = OSPPTNAuditFromElement(elem, &(reauthrsp->TNAudit));
                break;

            case OSPC_MELEM_DEST:
                errcode = OSPPDestFromElement(elem, &dest);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPReauthRspSetDest(reauthrsp, dest);
                }
                break;

            default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
                if (OSPPMsgElemIsCritical(elem)) {
                    errcode = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }

        if (parent != OSPC_OSNULL) {
            ospvElem = parent;
        }
    }

    if (errcode != OSPC_ERR_DATA_NO_REAUTHRSP) {
        *ospvReauthRsp = reauthrsp;
    }

    return errcode;
}

/*
 * OSPPReauthRspDelete() - deletes a reauthorization response structure
 */
void OSPPReauthRspDelete(
    OSPT_REAUTH_RSP **ospvReauthRsp)
{
    if (*ospvReauthRsp != OSPC_OSNULL) {
        /* first remove the status */
        if ((*ospvReauthRsp)->Status != OSPC_OSNULL) {
            OSPPStatusDelete(&((*ospvReauthRsp)->Status));
        }

        /* now remove dest */
        if (OSPPReauthRspHasDest(*ospvReauthRsp)) {
            OSPPDestDelete(&((*ospvReauthRsp)->Destination));
        }

        /* delete ComponentId and MessageId */
        if (OSPPReauthRspHasComponentId(*ospvReauthRsp)) {
            OSPM_FREE((*ospvReauthRsp)->ComponentId);
            (*ospvReauthRsp)->ComponentId = OSPC_OSNULL;
        }

        if (OSPPReauthRspHasMessageId(*ospvReauthRsp)) {
            OSPM_FREE((*ospvReauthRsp)->MessageId);
            (*ospvReauthRsp)->MessageId = OSPC_OSNULL;
        }

        /* now delete the authrsp itself */
        OSPM_FREE(*ospvReauthRsp);
        *ospvReauthRsp = OSPC_OSNULL;
    }
}

/*
 * OSPPReauthRspHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPReauthRspHasMessageId(     /* returns non-zero if message id is set */
    OSPT_REAUTH_RSP *ospvReauthRsp)
{
    if (ospvReauthRsp != OSPC_OSNULL) {
        return (ospvReauthRsp->MessageId != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

/*
 * OSPPReauthRspSetMessageId() - creates space and copies in the string.
 */
void OSPPReauthRspSetMessageId(
    OSPT_REAUTH_RSP *ospvReauthRsp, /* In - pointer to Usage Indication struct */
    const char *ospvMessageId)      /* In - pointer to message id string */
{
    int len = OSPM_STRLEN(ospvMessageId);

    if (ospvReauthRsp != OSPC_OSNULL) {
        if (ospvReauthRsp->MessageId != OSPC_OSNULL) {
            OSPM_FREE(ospvReauthRsp->MessageId);
        }

        OSPM_MALLOC(ospvReauthRsp->MessageId, char, len + 1);
        OSPM_MEMSET(ospvReauthRsp->MessageId, 0, len + 1);
        OSPM_MEMCPY(ospvReauthRsp->MessageId, ospvMessageId, len);
    }
}

/*
 * OSPPReauthRspMessageIdFromElement() - Get message id attribute from element.
 */
void OSPPReauthRspMessageIdFromElement(
    OSPT_XML_ELEM *ospvElemIn,
    const char **ospvMessageId)
{
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    /* look for the message id attribute */
    for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElemIn);
         (attr != OSPC_OSNULL); attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElemIn, attr)) {

        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_MESSAGEID) {
            /* we found the message attribute. Get the value */
            *ospvMessageId = OSPPXMLAttrGetValue(attr);
            break;
        }
    }
}

/*
 * OSPPReauthRspComponentIdFromElement() - Get component id attribute from element.
 */
void OSPPReauthRspComponentIdFromElement(
    OSPT_XML_ELEM *ospvElemIn,
    const char **ospvComponentId)
{
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    /* look for the component id attribute */
    for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElemIn);
         (attr != OSPC_OSNULL); attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElemIn, attr)) {

        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_COMPONENTID) {
            /* we found the component attribute. Get the value */
            *ospvComponentId = OSPPXMLAttrGetValue(attr);
            break;
        }
    }
}

/*
 * OSPPReauthRspSetComponentId() - creates space and copies in the string.
 */
void OSPPReauthRspSetComponentId(
    OSPT_REAUTH_RSP *ospvReauthRsp, /* In - pointer to Usage Indication struct */
    const char *ospvComponentId)    /* In - pointer to component id string */
{
    int len = OSPM_STRLEN(ospvComponentId);

    if (ospvReauthRsp != OSPC_OSNULL) {
        if (ospvReauthRsp->ComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvReauthRsp->ComponentId);
        }

        OSPM_MALLOC(ospvReauthRsp->ComponentId, char, len + 1);
        OSPM_MEMSET(ospvReauthRsp->ComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvReauthRsp->ComponentId, ospvComponentId, len);
    }
}
