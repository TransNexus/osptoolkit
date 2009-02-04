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
    OSPTUSAGECNF **ospvUsageCnf)
{
    if (*ospvUsageCnf != OSPC_OSNULL) {
        if (OSPPUsageCnfHasStatus(*ospvUsageCnf)) {
            OSPPStatusDelete(&((*ospvUsageCnf)->ospmUsageCnfStatus));
        }

        if ((*ospvUsageCnf)->ospmUsageCnfTNAudit != OSPC_OSNULL) {
            OSPPTNAuditDelete(&((*ospvUsageCnf)->ospmUsageCnfTNAudit));
        }

        if ((*ospvUsageCnf)->ospmUsageCnfCSAudit != OSPC_OSNULL) {
            OSPPCSAuditDelete(&((*ospvUsageCnf)->ospmUsageCnfCSAudit));
        }

        if ((*ospvUsageCnf)->ospmUsageCnfComponentId != OSPC_OSNULL) {
            OSPM_FREE((*ospvUsageCnf)->ospmUsageCnfComponentId);
        }

        if ((*ospvUsageCnf)->ospmUsageCnfMessageId != OSPC_OSNULL) {
            OSPM_FREE((*ospvUsageCnf)->ospmUsageCnfMessageId);
        }

        OSPM_FREE(*ospvUsageCnf);
        *ospvUsageCnf = OSPC_OSNULL;
    }
}

/*
 * OSPPUsageCnfSetTimestamp() - set the timestamp
 */
void OSPPUsageCnfSetTimestamp(      /* nothing returned */
    OSPTUSAGECNF *ospvUsageCnf,     /* usage cnfication to set */
    OSPTTIME ospvTimestamp)         /* timestamp to set to */
{
    if (ospvUsageCnf != OSPC_OSNULL) {
        if (ospvTimestamp != 0) {
            ospvUsageCnf->ospmUsageCnfTimestamp = ospvTimestamp;
        }
    }
}

/*
 * OSPPUsageCnfHasStatus() - does the usage confirmation have a status?
 */
OSPTBOOL OSPPUsageCnfHasStatus( /* returns non-zero if number exists */
    OSPTUSAGECNF *ospvUsageCnf) /* usage confirmation effected */
{
    OSPTBOOL ospvHas = OSPC_FALSE;

    if (ospvUsageCnf != OSPC_OSNULL) {
        ospvHas = (ospvUsageCnf->ospmUsageCnfStatus != OSPC_OSNULL);
    }
    
    return ospvHas;
}

/*
 * OSPPUsageCnfGetStatus() - returns the status for an usage confirmation
 */
OSPTSTATUS *OSPPUsageCnfGetStatus(  /* returns pointer to dest */
    OSPTUSAGECNF *ospvUsageCnf)     /* usage confirmation */
{
    OSPTSTATUS *ospvStatus = OSPC_OSNULL;

    if (ospvUsageCnf != OSPC_OSNULL) {
        ospvStatus = ospvUsageCnf->ospmUsageCnfStatus;
    }

    return ospvStatus;
}

/*
 * OSPPUsageCnfHasComponentId() - is the component id set ?
 */
OSPTBOOL OSPPUsageCnfHasComponentId(    /* returns non-zero if component id is set */
    OSPTUSAGECNF *ospvUsageCnf)
{
    return(ospvUsageCnf->ospmUsageCnfComponentId != OSPC_OSNULL);
}

/*
 * OSPPUsageCnfGetComponentId() - returns a new copy of the component id.
 */
const char *OSPPUsageCnfGetComponentId(
    OSPTUSAGECNF *ospvUsageCnf)
{
    const char *componentstring = OSPC_OSNULL;
    int len = 0;

    if (OSPPUsageCnfHasComponentId(ospvUsageCnf)) {
        len = OSPM_STRLEN(ospvUsageCnf->ospmUsageCnfComponentId);
        OSPM_MALLOC(componentstring, char, len + 1);
        OSPM_MEMSET(componentstring, 0, len + 1);
        OSPM_MEMCPY(componentstring, ospvUsageCnf->ospmUsageCnfComponentId, len);
    }

    return componentstring;
}

/*
 * OSPPUsageCnfSetComponentId() - creates space and copies in the string.
 */
void OSPPUsageCnfSetComponentId(
    OSPTUSAGECNF *ospvUsageCnf,     /* In - pointer to Usage Confirm struct */
    const char *ospvComponentId)    /* In - pointer to component id string */
{
    int len = OSPM_STRLEN(ospvComponentId);

    if (ospvUsageCnf != OSPC_OSNULL) {
        if (ospvUsageCnf->ospmUsageCnfComponentId != OSPC_OSNULL) {
            OSPM_FREE(ospvUsageCnf->ospmUsageCnfComponentId);
        }

        OSPM_MALLOC(ospvUsageCnf->ospmUsageCnfComponentId, char, len + 1);
        OSPM_MEMSET(ospvUsageCnf->ospmUsageCnfComponentId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageCnf->ospmUsageCnfComponentId, ospvComponentId, len);
    }
}


/*
 * OSPPUsageCnfNew() - creates a new (empty) usage confirmation
 */
OSPTUSAGECNF *OSPPUsageCnfNew(void)     /* returns pointer or NULL */
{
    OSPTUSAGECNF *usagecnf;

    OSPM_MALLOC(usagecnf, OSPTUSAGECNF, sizeof(OSPTUSAGECNF));
    if (usagecnf != OSPC_OSNULL) {
        OSPM_MEMSET(usagecnf, 0, sizeof(OSPTUSAGECNF));
        OSPPListLinkNew(&(usagecnf->ospmUsageCnfLink));
        usagecnf->ospmUsageCnfTimestamp = (OSPTTIME) 0;
        usagecnf->ospmUsageCnfStatus = OSPC_OSNULL;
        usagecnf->ospmUsageCnfCSAudit = OSPC_OSNULL;
        usagecnf->ospmUsageCnfTNAudit = OSPC_OSNULL;
        usagecnf->ospmUsageCnfComponentId = OSPC_OSNULL;
        usagecnf->ospmUsageCnfMessageId = OSPC_OSNULL;
    }

    return usagecnf;
}

OSPTCSAUDIT *OSPPUsageCnfGetCSAudit(
    OSPTUSAGECNF *ospvUsageCnf)
{
    if (ospvUsageCnf != OSPC_OSNULL) {
        return ospvUsageCnf->ospmUsageCnfCSAudit;
    } else {
        return OSPC_OSNULL;
    }
}

OSPTTNAUDIT *OSPPUsageCnfGetTNAudit(
    OSPTUSAGECNF *ospvUsageCnf)
{
    if (ospvUsageCnf != OSPC_OSNULL) {
        return ospvUsageCnf->ospmUsageCnfTNAudit;
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
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ELEM *elem1 = OSPC_OSNULL;
    OSPTUSAGECNF *usagecnf = OSPC_OSNULL;
    OSPTTIME t = 0;
    const char *compid = OSPC_OSNULL;
    const char *messageId = OSPC_OSNULL;
    OSPT_XML_ELEM *ospvParent = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvUsageCnf == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_USAGECNF;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPListNew(ospvUsageCnf);

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) ==
                OSPC_MELEM_MESSAGE) {
                OSPPUsageCnfMessageIdFromElement(ospvElem, &messageId);

                /* ospvElem is pointing to the Message element. 
                 * The first child contains the Component element. 
                 * The following two lines of code change ospvElem from 
                 * pointing to the Message element to the Component element.
                 */
                ospvParent = ospvElem;
                ospvElem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvParent);
            }

            for (elem1 = (OSPT_XML_ELEM *)OSPPListFirst((OSPTLIST *)ospvElem);
                 (elem1 != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
                 elem1 = (OSPT_XML_ELEM *)OSPPListNext((OSPTLIST *)ospvElem, elem1)) 
            {
                /* create the usage confirmation object */
                usagecnf = OSPPUsageCnfNew();

                if (usagecnf == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_DATA_NO_USAGECNF;
                } else {
                    /*
                     * The Usage Confirmation element should consist of several child
                     * elements. We'll run through what's there and pick out
                     * the information we need.
                     */

                    if (messageId != OSPC_OSNULL) {
                        OSPPUsageCnfSetMessageId(usagecnf, messageId);
                    }

                    if (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem1)) ==
                        OSPC_MELEM_USAGECNF) {
                        /* Get ComponentId */
                        if (elem1 != OSPC_OSNULL) {
                            OSPPUsageCnfComponentIdFromElement(elem1, &compid);
                            if (compid != OSPC_OSNULL) {
                                OSPPUsageCnfSetComponentId(usagecnf, compid);
                            }
                        }

                        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(elem1);
                            (elem != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
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
                                ospvErrCode = OSPPMsgTimeFromElement(elem, &t);
                                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                                    OSPPUsageCnfSetTimestamp(usagecnf, t);
                                }
                                break;
                            case OSPC_MELEM_STATUS:
                                if (usagecnf->ospmUsageCnfStatus == OSPC_OSNULL) {
                                    /* usagecnf->ospmUsageCnfStatus = OSPPStatusNew(); */
                                    ospvErrCode = OSPPStatusFromElement(elem, &(usagecnf->ospmUsageCnfStatus));
                                }
                                break;
                            case OSPC_MELEM_AUDIT:
                                ospvErrCode = OSPPTNAuditFromElement(elem, &(usagecnf->ospmUsageCnfTNAudit));
                                break;
                            case OSPC_MELEM_CSAUDITTRIGGER:
                                ospvErrCode = OSPPCSAuditFromElement(elem, &(usagecnf->ospmUsageCnfCSAudit));
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

                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        if (usagecnf != OSPC_OSNULL) {
                            OSPPListAppend(ospvUsageCnf, usagecnf);
                            usagecnf = OSPC_OSNULL;
                        }
                    }
                }
            }
        }
    }

    /* clean up */
    if (ospvErrCode != OSPC_ERR_NO_ERROR) {
        if (ospvUsageCnf != OSPC_OSNULL) {
            while (!OSPPListEmpty(ospvUsageCnf)) {
                usagecnf = (OSPTUSAGECNF *)OSPPListRemove(ospvUsageCnf);
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

    return ospvErrCode;
}

/*
 * OSPPUsageCnfHasMessageId() - is the message id set ?
 */
OSPTBOOL OSPPUsageCnfHasMessageId(  /* returns non-zero if message id is set */
    OSPTUSAGECNF *ospvUsageCnf)
{
    return(ospvUsageCnf->ospmUsageCnfMessageId != OSPC_OSNULL);
}

/*
 * OSPPUsageCnfSetMessageId() - creates space and copies in the string.
 */
void OSPPUsageCnfSetMessageId(
    OSPTUSAGECNF *ospvUsageCnf,     /* In - pointer to Usage Indication struct */
    const char *ospvMessageId)      /* In - pointer to message id string */
{
    int len = OSPM_STRLEN(ospvMessageId);

    if (ospvUsageCnf != OSPC_OSNULL) {
        if (ospvUsageCnf->ospmUsageCnfMessageId != OSPC_OSNULL) {
            OSPM_FREE(ospvUsageCnf->ospmUsageCnfMessageId);
        }

        OSPM_MALLOC(ospvUsageCnf->ospmUsageCnfMessageId, char, len + 1);
        OSPM_MEMSET(ospvUsageCnf->ospmUsageCnfMessageId, 0, len + 1);
        OSPM_MEMCPY(ospvUsageCnf->ospmUsageCnfMessageId, ospvMessageId, len);
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
