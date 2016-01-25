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

/* ospusagecnf.c - OSP usage confirmation functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospusage.h"
#include "osp/ospusagecnf.h"
#include "osp/ospstatus.h"
#include "osp/osputils.h"
#include "osp/ospciscoext.h"

/*
 * OSPPUsageCnfDelete() - destroy specified UsageCnf object
 */
void OSPPUsageCnfDelete(
    OSPT_USAGE_CNF **ospvUsageCnf)
{
    if (*ospvUsageCnf != OSPC_OSNULL) {
        if (OSPPUsageCnfHasStatus(*ospvUsageCnf)) {
            OSPPStatusDelete(&((*ospvUsageCnf)->Status));
        }

        if ((*ospvUsageCnf)->TNAudit != OSPC_OSNULL) {
            OSPPTNAuditDelete(&((*ospvUsageCnf)->TNAudit));
        }

        if ((*ospvUsageCnf)->CSAudit != OSPC_OSNULL) {
            OSPPCSAuditDelete(&((*ospvUsageCnf)->CSAudit));
        }

        if ((*ospvUsageCnf)->ComponentId != OSPC_OSNULL) {
            OSPM_FREE((*ospvUsageCnf)->ComponentId);
        }

        if ((*ospvUsageCnf)->MessageId != OSPC_OSNULL) {
            OSPM_FREE((*ospvUsageCnf)->MessageId);
        }

        OSPM_FREE(*ospvUsageCnf);
        *ospvUsageCnf = OSPC_OSNULL;
    }
}

/*
 * OSPPUsageCnfSetTimestamp() - set the timestamp
 */
void OSPPUsageCnfSetTimestamp(      /* nothing returned */
    OSPT_USAGE_CNF *ospvUsageCnf,   /* usage cnfication to set */
    OSPTTIME ospvTimestamp)         /* timestamp to set to */
{
    if (ospvUsageCnf != OSPC_OSNULL) {
        if (ospvTimestamp != 0) {
            ospvUsageCnf->Timestamp = ospvTimestamp;
        }
    }
}

/*
 * OSPPUsageCnfHasStatus() - does the usage confirmation have a status?
 */
OSPTBOOL OSPPUsageCnfHasStatus(     /* returns non-zero if number exists */
    OSPT_USAGE_CNF *ospvUsageCnf)   /* usage confirmation effected */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvUsageCnf != OSPC_OSNULL) {
        has = (ospvUsageCnf->Status != OSPC_OSNULL);
    }

    return has;
}

/*
 * OSPPUsageCnfGetStatus() - returns the status for an usage confirmation
 */
OSPT_STATUS *OSPPUsageCnfGetStatus( /* returns pointer to dest */
    OSPT_USAGE_CNF *ospvUsageCnf)   /* usage confirmation */
{
    OSPT_STATUS *status = OSPC_OSNULL;

    if (ospvUsageCnf != OSPC_OSNULL) {
        status = ospvUsageCnf->Status;
    }

    return status;
}

/*
 * OSPPUsageCnfHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPUsageCnfHasComponentId(    /* returns non-zero if component id is set */
    OSPT_USAGE_CNF *ospvUsageCnf)
{
    return(ospvUsageCnf->ComponentId != OSPC_OSNULL);
}

/*
 * OSPPUsageCnfGetComponentId() - returns a new copy of the component id.
 */
const char *OSPPUsageCnfGetComponentId(
    OSPT_USAGE_CNF *ospvUsageCnf)
{
    const char *componentstring = OSPC_OSNULL;
    int len = 0;

    if (OSPPUsageCnfHasComponentId(ospvUsageCnf)) {
        len = OSPM_STRLEN(ospvUsageCnf->ComponentId);
        OSPM_MALLOC(componentstring, char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvUsageCnf->ComponentId, len);
    }

    return componentstring;
}

/*
 * OSPPUsageCnfSetComponentId() - creates space and copies in the string.
 */
void OSPPUsageCnfSetComponentId(
    OSPT_USAGE_CNF *ospvUsageCnf,   /* In - pointer to Usage Confirm struct */
    const char *ospvComponentId)    /* In - pointer to component id string */
{
    int len = OSPM_STRLEN(ospvComponentId);

    if (ospvUsageCnf != OSPC_OSNULL) {
        if (ospvUsageCnf->ComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvUsageCnf->ComponentId);
        }

        OSPM_MALLOC(ospvUsageCnf->ComponentId, char, len + 1);
        OSPM_MEMSET(ospvUsageCnf->ComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageCnf->ComponentId, ospvComponentId, len);
    }
}


/*
 * OSPPUsageCnfNew() - creates a new (empty) usage confirmation
 */
OSPT_USAGE_CNF *OSPPUsageCnfNew(void)   /* returns pointer or NULL */
{
    OSPT_USAGE_CNF *usagecnf;

    OSPM_MALLOC(usagecnf, OSPT_USAGE_CNF, sizeof(OSPT_USAGE_CNF));
    if (usagecnf != OSPC_OSNULL) {
        OSPM_MEMSET(usagecnf, 0, sizeof(OSPT_USAGE_CNF));
        OSPPListLinkNew(&(usagecnf->Link));
        usagecnf->Timestamp = (OSPTTIME) 0;
        usagecnf->Status = OSPC_OSNULL;
        usagecnf->CSAudit = OSPC_OSNULL;
        usagecnf->TNAudit = OSPC_OSNULL;
        usagecnf->ComponentId = OSPC_OSNULL;
        usagecnf->MessageId = OSPC_OSNULL;
    }

    return usagecnf;
}

OSPTCSAUDIT *OSPPUsageCnfGetCSAudit(
    OSPT_USAGE_CNF *ospvUsageCnf)
{
    if (ospvUsageCnf != OSPC_OSNULL) {
        return ospvUsageCnf->CSAudit;
    } else {
        return OSPC_OSNULL;
    }
}

OSPT_TN_AUDIT *OSPPUsageCnfGetTNAudit(
    OSPT_USAGE_CNF *ospvUsageCnf)
{
    if (ospvUsageCnf != OSPC_OSNULL) {
        return ospvUsageCnf->TNAudit;
    } else {
        return OSPC_OSNULL;
    }
}

/*
 * OSPPUsageCnfComponentIdFromElement() - Get component id attribute from element.
 */
void OSPPUsageCnfComponentIdFromElement(
    OSPT_XML_ELEM *ospvElem,
    const char **ospvComponentId)
{
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    /* look for the component id attribute */
    for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElem);
        (attr != OSPC_OSNULL);
        attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElem, attr))
    {
        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_COMPONENTID) {
            /* we found the component attribute. Get the value */
            *ospvComponentId = OSPPXMLAttrGetValue(attr);
        }
    }
}

/*
 * OSPPUsageCnfFromElement() - get usage confirmation from an XML element
 */
unsigned OSPPUsageCnfFromElement(   /* returns error code */
    OSPT_XML_ELEM *ospvElem,        /* input is XML element */
    OSPTLIST *ospvUsageCnf)         /* where to put usage cnf list */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ELEM *elem1 = OSPC_OSNULL;
    OSPT_USAGE_CNF *usagecnf = OSPC_OSNULL;
    OSPTTIME t = 0;
    const char *compid = OSPC_OSNULL;
    const char *messageId = OSPC_OSNULL;
    OSPT_XML_ELEM *parent = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvUsageCnf == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_NO_USAGECNF;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPListNew(ospvUsageCnf);

        if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) ==
            OSPC_MELEM_MESSAGE)
        {
            OSPPUsageCnfMessageIdFromElement(ospvElem, &messageId);

            /* ospvElem is pointing to the Message element.
             * The first child contains the Component element.
             * The following two lines of code change ospvElem from
             * pointing to the Message element to the Component element.
             */
            parent = ospvElem;
            ospvElem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(parent);
        }

        for (elem1 = (OSPT_XML_ELEM *)OSPPListFirst((OSPTLIST *)ospvElem);
             (elem1 != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR);
             elem1 = (OSPT_XML_ELEM *)OSPPListNext((OSPTLIST *)ospvElem, elem1))
        {
            /* create the usage confirmation object */
            usagecnf = OSPPUsageCnfNew();
            if (usagecnf == OSPC_OSNULL) {
                errcode = OSPC_ERR_DATA_NO_USAGECNF;
            } else {
                /*
                 * The Usage Confirmation element should consist of several child
                 * elements. We'll run through what's there and pick out
                 * the information we need.
                 */
                if (messageId != OSPC_OSNULL) {
                    OSPPUsageCnfSetMessageId(usagecnf, messageId);
                }

                if (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem1)) == OSPC_MELEM_USAGECNF) {
                    /* Get ComponentId */
                    if (elem1 != OSPC_OSNULL) {
                        OSPPUsageCnfComponentIdFromElement(elem1, &compid);
                        if (compid != OSPC_OSNULL) {
                            OSPPUsageCnfSetComponentId(usagecnf, compid);
                        }
                    }

                    for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(elem1);
                        (elem != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR);
                         elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(elem1, elem))
                    {
                        switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
                        case OSPC_MELEM_MESSAGE:
                            if (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem)) == OSPC_MELEM_MESSAGE) {
                                OSPPUsageCnfMessageIdFromElement(elem, &messageId);
                                if (messageId != OSPC_OSNULL) {
                                    OSPPUsageCnfSetMessageId(usagecnf, messageId);
                                }
                            }
                            break;
                        case OSPC_MELEM_USAGECNF:
                            OSPPUsageCnfComponentIdFromElement(elem, &compid);
                            if (compid != OSPC_OSNULL) {
                                OSPPUsageCnfSetComponentId(usagecnf, compid);
                            }
                            break;
                        case OSPC_MELEM_TIMESTAMP:
                            errcode = OSPPMsgTimeFromElement(elem, &t);
                            if (errcode == OSPC_ERR_NO_ERROR) {
                                OSPPUsageCnfSetTimestamp(usagecnf, t);
                            }
                            break;
                        case OSPC_MELEM_STATUS:
                            if (usagecnf->Status == OSPC_OSNULL) {
                                /* usagecnf->ospmUsageCnfStatus = OSPPStatusNew(); */
                                errcode = OSPPStatusFromElement(elem, &(usagecnf->Status));
                            }
                            break;
                        case OSPC_MELEM_AUDIT:
                            errcode = OSPPTNAuditFromElement(elem, &(usagecnf->TNAudit));
                            break;
                        case OSPC_MELEM_CSAUDITTRIGGER:
                            errcode = OSPPCSAuditFromElement(elem, &(usagecnf->CSAudit));
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
                }
                 if (errcode == OSPC_ERR_NO_ERROR) {
                    if (usagecnf != OSPC_OSNULL) {
                        OSPPListAppend(ospvUsageCnf, usagecnf);
                        usagecnf = OSPC_OSNULL;
                    }
                }
            }
        }
    }

    /* clean up */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (ospvUsageCnf != OSPC_OSNULL) {
            while (!OSPPListEmpty(ospvUsageCnf)) {
                usagecnf = (OSPT_USAGE_CNF *)OSPPListRemove(ospvUsageCnf);
                if (usagecnf != OSPC_OSNULL) {
                    OSPPUsageCnfDelete(&(usagecnf));
                }
            }

            OSPPListDelete(ospvUsageCnf);
        }
    }

    if (usagecnf != OSPC_OSNULL) {
        OSPPUsageCnfDelete(&usagecnf);
    }

    return errcode;
}

/*
 * OSPPUsageCnfHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPUsageCnfHasMessageId(  /* returns non-zero if message id is set */
    OSPT_USAGE_CNF *ospvUsageCnf)
{
    return(ospvUsageCnf->MessageId != OSPC_OSNULL);
}

/*
 * OSPPUsageCnfSetMessageId() - creates space and copies in the string.
 */
void OSPPUsageCnfSetMessageId(
    OSPT_USAGE_CNF *ospvUsageCnf,   /* In - pointer to Usage Indication struct */
    const char *ospvMessageId)      /* In - pointer to message id string */
{
    int len = OSPM_STRLEN(ospvMessageId);

    if (ospvUsageCnf != OSPC_OSNULL) {
        if (ospvUsageCnf->MessageId != OSPC_OSNULL) {
            OSPM_FREE(ospvUsageCnf->MessageId);
        }

        OSPM_MALLOC(ospvUsageCnf->MessageId, char, len + 1);
        OSPM_MEMSET(ospvUsageCnf->MessageId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageCnf->MessageId, ospvMessageId, len);
    }
}

/*
 * OSPPUsageCnfMessageIdFromElement() - Get message id attribute from element.
 */
void OSPPUsageCnfMessageIdFromElement(
    OSPT_XML_ELEM *ospvElemIn,
    const char **ospvMessageId)
{
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    /* look for the message id attribute */
    for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElemIn);
         (attr != OSPC_OSNULL);
         attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElemIn, attr)) {

        if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_MESSAGEID) {
            /* we found the message attribute. Get the value */
            *ospvMessageId = OSPPXMLAttrGetValue(attr);
            break;
        }
    }
}
