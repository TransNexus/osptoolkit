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

/* ospstatus.c - OSP status functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/osplist.h"
#include "osp/ospxmldoc.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"
#include "osp/ospstatus.h"

/*
 * OSPPStatusNew() - create a new status object
 */
OSPT_STATUS *OSPPStatusNew(void)    /* returns ptr to status or null */
{
    OSPT_STATUS *status = OSPC_OSNULL;

    /* try to allocate the memory for the entire object */
    OSPM_MALLOC(status, OSPT_STATUS, sizeof(*status));
    if (status != OSPC_OSNULL) {
        /* !!!PS Initialize it */
        OSPM_MEMSET(status, 0, sizeof(*status));
        status->Description = OSPC_OSNULL;
    }

    return status;
}

/*
 * OSPPStatusDelete() - destroy a status object
 */
void OSPPStatusDelete(          /* no return */
    OSPT_STATUS **ospvStatus)   /* Status to destroy */
{
    if (*ospvStatus != OSPC_OSNULL) {
        if ((*ospvStatus)->Description != OSPC_OSNULL) {
            OSPM_FREE((*ospvStatus)->Description);
            (*ospvStatus)->Description = OSPC_OSNULL;
        }
        OSPM_FREE(*ospvStatus);
        *ospvStatus = OSPC_OSNULL;
    }
}

/*
 * OSPPStatusSetDesc() - set the status description
 */
void OSPPStatusSetDesc(     /* no return */
    OSPT_STATUS *ospvStatus,/* Status to set */
    const char *ospvDesc)
{
    if (ospvStatus != OSPC_OSNULL) {
        if (ospvStatus->Description != OSPC_OSNULL) {
            OSPM_FREE(ospvStatus->Description);
            ospvStatus->Description = OSPC_OSNULL;
        }
        OSPM_MALLOC(ospvStatus->Description, char, OSPM_STRLEN(ospvDesc) + 1);
        if (ospvStatus->Description != OSPC_OSNULL) {
            OSPM_MEMCPY(ospvStatus->Description, ospvDesc, OSPM_STRLEN(ospvDesc) + 1);
            ospvStatus->HasDescription = OSPC_TRUE;
        }
    }
}

/*
 * OSPPStatusHasCode() - is the status code populated
 */
OSPTBOOL OSPPStatusHasCode( /* return 1 or 0 */
    OSPT_STATUS *ospvStatus)/* Status to check */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvStatus != OSPC_OSNULL) {
        has = ospvStatus->HasCode;
    }

    return has;
}

/*
 * OSPPStatusGetCode() - get the status code
 */
unsigned OSPPStatusGetCode( /* return code */
    OSPT_STATUS *ospvStatus)/* Status to get */
{
    unsigned statuscode = 0;

    if ((ospvStatus != OSPC_OSNULL) && (ospvStatus->HasCode)) {
        statuscode = ospvStatus->Code;
    }

    return statuscode;
}

/*
 * OSPPStatusFromElement() - create a status from an XML element
 */
unsigned OSPPStatusFromElement( /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    OSPT_STATUS **ospvStatus)   /* where to put status pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    unsigned long temp = 0;
    OSPT_STATUS *status = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
#if 0
    assert(OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_STATUS);
#endif
        if (ospvStatus == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_NO_STATUS;
        }
    }

    /* create the Status structure */
    if (errcode == OSPC_ERR_NO_ERROR) {
        status = OSPPStatusNew();
        if (status == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_NO_STATUS;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /*
         * The Status element should consist of several child
         * elements. We'll run through what's there and pick out
         * the information we need. First, though, let's zero out
         * the values that we expect to find.
         */
        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR);
            elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_CODE:
                errcode = OSPPMsgCodeFromElement(elem, &temp);
                OSPPStatusSetCode(status, temp);
                break;
            case OSPC_MELEM_DESC:
                OSPPStatusSetDesc(status, OSPPXMLElemGetValue(elem));
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
        *ospvStatus = status;
    } else {
        if (status != OSPC_OSNULL) {
            OSPPStatusDelete(&status);
        }
    }

    return errcode;
}

/*
 * OSPPStatusSetCode() - set the status code
 */
void OSPPStatusSetCode(     /* no return */
    OSPT_STATUS *ospvStatus,/* Status to set */
    unsigned ospvCode)
{
    if (ospvStatus != OSPC_OSNULL) {
        ospvStatus->Code = ospvCode;
        ospvStatus->HasCode = 1;
    }
}
