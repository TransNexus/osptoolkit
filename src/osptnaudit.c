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

/* osptnaudit.c - Functions for tnaudit element. */

#include "osp/osptnaudit.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"
#include "osp/ospmsgattr.h"

void OSPPTNAuditDelete(
    OSPT_TN_AUDIT **ospvTNAudit)
{
    if (*ospvTNAudit != OSPC_OSNULL) {
        if ((*ospvTNAudit)->URL != OSPC_OSNULL) {
            OSPM_FREE((*ospvTNAudit)->URL);
            (*ospvTNAudit)->URL = OSPC_OSNULL;
        }
        OSPM_FREE(*ospvTNAudit);
        *ospvTNAudit = OSPC_OSNULL;
    }
}

int OSPPTNAuditFromElement(
    OSPT_XML_ELEM *ospvElem,
    OSPT_TN_AUDIT **ospvTNAudit)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    unsigned long temp = 0;

    if (ospvElem == OSPC_OSNULL) {
        errorcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvTNAudit == OSPC_OSNULL) {
            errorcode = OSPC_ERR_DATA_NO_STATUS;
        }
    }

    /* create the TNAudit structure */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvTNAudit = OSPPTNAuditNew();
        if (*ospvTNAudit == OSPC_OSNULL) {
            errorcode = OSPC_ERR_AUDIT_MALLOC_FAILED;
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* If the elem has no children, we are probably dealing with
         * an AuditConfirm. Make sure it gets processed properly.
         */
        if ((elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem)) == OSPC_OSNULL) {
            if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_AUDITSTATE) {
                errorcode = OSPPMsgNumFromElement(ospvElem, &temp);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    OSPPTNAuditSetState(*ospvTNAudit, (OSPE_AUDIT_STATE) temp);
                    temp = 0;
                }
            }
        }

        /*
         * The TNAudit element should consist of several child
         * elements. We'll run through what's there and pick out
         * the information we need.
         */
        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
             (elem != OSPC_OSNULL) && (errorcode == OSPC_ERR_NO_ERROR);
             elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_AUDIT:
                break;
            case OSPC_MELEM_AUDITSTATE:
                errorcode = OSPPMsgNumFromElement(elem, &temp);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    OSPPTNAuditSetState(*ospvTNAudit, (OSPE_AUDIT_STATE)temp);
                    temp = 0;
                }
                break;
            case OSPC_MELEM_AUDITURL:
                OSPPTNAuditSetURL(*ospvTNAudit, OSPPXMLElemGetValue(elem));
                break;
            case OSPC_MELEM_AUDITTIMELIMIT:
                errorcode = OSPPMsgNumFromElement(elem, &temp);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    OSPPTNAuditSetTimeLimit(*ospvTNAudit, temp);
                    temp = 0;
                }
                break;
            case OSPC_MELEM_AUDITMAXMESSAGES:
                errorcode = OSPPMsgNumFromElement(elem, &temp);
                if (errorcode == OSPC_ERR_NO_ERROR) {
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
                if (OSPPMsgElemIsCritical(elem)) {
                    errorcode = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            }
        }
    }

    return errorcode;
}

unsigned OSPPTNAuditGetMaxMessages(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return ospvTNAudit->MaxMessages;
    } else {
        return 0;
    }
}

OSPE_AUDIT_STATE OSPPTNAuditGetState(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return (OSPE_AUDIT_STATE)ospvTNAudit->State;
    } else {
        return (OSPE_AUDIT_STATE)0;
    }
}

unsigned OSPPTNAuditGetTimeLimit(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return ospvTNAudit->TimeLimit;
    } else {
        return 0;
    }
}

const char *OSPPTNAuditGetURL(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return ospvTNAudit->URL;
    } else {
        return OSPC_OSNULL;
    }
}

OSPTBOOL OSPPTNAuditHasMaxMessages(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return(ospvTNAudit->MaxMessages > 0);
    } else {
        return OSPC_FALSE;
    }
}

OSPTBOOL OSPPTNAuditHasState(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return(ospvTNAudit->State > 0);
    } else {
        return OSPC_FALSE;
    }
}

OSPTBOOL OSPPTNAuditHasTimeLimit(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return(ospvTNAudit->TimeLimit > 0);
    } else {
        return OSPC_FALSE;
    }
}

OSPTBOOL OSPPTNAuditHasURL(
    OSPT_TN_AUDIT *ospvTNAudit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        return(ospvTNAudit->URL != OSPC_OSNULL);
    } else {
        return OSPC_FALSE;
    }
}

OSPT_TN_AUDIT *OSPPTNAuditNew(void)
{
    OSPT_TN_AUDIT *tnaudit = OSPC_OSNULL;

    /* try to allocate the memory for the entire object */
    OSPM_MALLOC(tnaudit, OSPT_TN_AUDIT, sizeof(OSPT_TN_AUDIT));
    if (tnaudit != OSPC_OSNULL) {
        /* Initialize it */
        OSPM_MEMSET(tnaudit, 0, sizeof(OSPT_TN_AUDIT));
        tnaudit->URL = OSPC_OSNULL;
    }

    return tnaudit;
}

void OSPPTNAuditSetMaxMessages(
    OSPT_TN_AUDIT *ospvTNAudit,
    unsigned ospvMaxMessages)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        ospvTNAudit->MaxMessages = ospvMaxMessages;
    }
}

void OSPPTNAuditSetState(
    OSPT_TN_AUDIT *ospvTNAudit,
    OSPE_AUDIT_STATE ospvAuditState)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        ospvTNAudit->State = ospvAuditState;
    }
}

void OSPPTNAuditSetTimeLimit(
    OSPT_TN_AUDIT *ospvTNAudit,
    unsigned ospvTimeLimit)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        ospvTNAudit->TimeLimit = ospvTimeLimit;
    }
}

void OSPPTNAuditSetURL(
    OSPT_TN_AUDIT *ospvTNAudit,
    const char *ospvAuditURL)
{
    if (ospvTNAudit != OSPC_OSNULL) {
        if (ospvAuditURL != OSPC_OSNULL) {
            if (ospvTNAudit->URL != OSPC_OSNULL) {
                OSPM_FREE(ospvTNAudit->URL);
                ospvTNAudit->URL = OSPC_OSNULL;
            }

            OSPM_MALLOC(ospvTNAudit->URL, char, OSPM_STRLEN(ospvAuditURL) + 1);
            if (ospvTNAudit->URL != OSPC_OSNULL) {
                OSPM_MEMCPY(ospvTNAudit->URL, ospvAuditURL, OSPM_STRLEN(ospvAuditURL) + 1);
            }
        }
    }
}
