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

/* osptcapncf.cpp */

#include "osp/osp.h"
#include "osp/osptrans.h"
#include "osp/ospprovider.h"
#include "osp/ospsecurity.h"
#include "osp/ospmime.h"
#include "osp/osputils.h"
#include "osp/ospmsgque.h"
#include "osp/ospmsginfo.h"
#include "osp/ospmsgattr.h"
#include "osp/ospxml.h"
#include "osp/ospxmlattr.h"
#include "osp/ospmsg.h"
#include "osp/ospcomm.h"
#include "osp/ospcapind.h"
#include "osp/ospfail.h"
#include "osp/ospaltinfo.h"

unsigned OSPPCapCnfNew(
    OSPT_CAP_CNF **ospvCapCnf)
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC(*ospvCapCnf, OSPT_CAP_CNF, sizeof(OSPT_CAP_CNF));

    if (*ospvCapCnf != OSPC_OSNULL) {
        OSPM_MEMSET(*ospvCapCnf, 0, sizeof(OSPT_CAP_CNF));
    } else {
        errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errcode, "CapCnf struct not created.");
    }

    return errcode;
}

void OSPPCapCnfDelete(
    OSPT_CAP_CNF **ospvCapCnf)
{
    if (ospvCapCnf != OSPC_OSNULL && *ospvCapCnf != OSPC_OSNULL) {
        OSPPStatusDelete(&((*ospvCapCnf)->ospmStatus));

        OSPM_FREE(*ospvCapCnf);

        *ospvCapCnf = OSPC_OSNULL;
    }
}

unsigned OSPPCapCnfFromElement( /* returns error code */
    OSPT_XML_ELEM *ospvParent,  /* input is XML element */
    OSPT_CAP_CNF **ospvCapCnf)  /* where to put capability confirmation pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ELEM *capCnfElem = OSPC_OSNULL;
    OSPT_CAP_CNF *capcnf = OSPC_OSNULL;

    /*
     * Check input data
     */
    if (OSPC_OSNULL == ospvParent) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    /*
     * Create new confirmation object
     */
    if (OSPC_ERR_NO_ERROR == errcode) {
        errcode = OSPPCapCnfNew(&capcnf);
    }

    if (OSPC_ERR_NO_ERROR == errcode) {
        /*
         * The Parent points to Message, its 1st child is
         * CapabilityConfirmation
         */
        capCnfElem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvParent);

        /*
         * The Confirmation element should consist of several
         * child elements. We'll run through what's there and pick out
         * the information we need.
         */
        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(capCnfElem);
             (elem != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR); elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(capCnfElem, elem)) {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_TIMESTAMP:
            case OSPC_MELEM_DESTOSPVERSION:
                /* For now, we aren't interested in the value */
                break;

            case OSPC_MELEM_STATUS:
                errcode = OSPPStatusFromElement(elem, &(capcnf->ospmStatus));
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
        }   /* For each child element of CapabilitiesConfirmation */
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        *ospvCapCnf = capcnf;
    }

    return errcode;
}
