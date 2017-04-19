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

/* ospstir.c - OSP STIR functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/osplist.h"
#include "osp/ospusage.h"
#include "osp/ospstir.h"

/*
 * OSPPFingerprintNew() - create a new SDP finger print object
 */
OSPT_SDP_FINGERPRINT *OSPPFingerprintNew(   /* returns ptr to SDP finger print or null */
    const char *ospvValue)                  /* SDP finger print value */
{
    OSPT_SDP_FINGERPRINT *fingerprint = OSPC_OSNULL;

    if (ospvValue != OSPC_OSNULL) {
        OSPM_MALLOC(fingerprint, OSPT_SDP_FINGERPRINT, sizeof(OSPT_SDP_FINGERPRINT));
        if (fingerprint != OSPC_OSNULL) {
            OSPPListLinkNew(&(fingerprint->Link));
            OSPM_STRNCPY(fingerprint->Value, ospvValue, sizeof(fingerprint->Value));
        }
    }

    return fingerprint;
}

/*
 * OSPPFingerprintToElement() - create an XML element from a SDP finger print item
 */
int OSPPFingerprintToElement(               /* returns error code */
    OSPT_SDP_FINGERPRINT *ospvFingerprint,  /* SDP finger print */
    OSPT_XML_ELEM **ospvElem)               /* where to put XML element pointer */
{
    int errcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvFingerprint == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_INVALID_ARGS;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPStringToElement(OSPC_MELEM_FINGERPRINT, ospvFingerprint->Value, 0, OSPC_OSNULL, OSPC_OSNULL, ospvElem);
    }

    return errcode;
}

