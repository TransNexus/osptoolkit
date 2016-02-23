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
 * OSPPFingerPrintNew() - create a new SDP finger print object
 */
OSPT_SDP_FINGERPRINT *OSPPFingerPrintNew(   /* returns ptr to SDP finger print or null */
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
 * OSPPFingerPrintToElement() - create an XML element from a SDP finger print item
 */
int OSPPFingerPrintToElement(               /* returns error code */
    OSPT_SDP_FINGERPRINT *ospvFingerPrint,  /* SDP finger print */
    OSPT_XML_ELEM **ospvElem)               /* where to put XML element pointer */
{
    int errcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvFingerPrint == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_INVALID_ARGS;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPStringToElement(OSPC_MELEM_FINGERPRINT, ospvFingerPrint->Value, 0, OSPC_OSNULL, OSPC_OSNULL, ospvElem);
    }

    return errcode;
}

int OSPPIdentityToElement(
    OSPT_IDENTITY *ospvIdentity,    /* Identity */
    OSPT_XML_ELEM **ospvElem)       /* Where to put XML element pointer */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_IDENTITY), "");
        if (*ospvElem == OSPC_OSNULL) {
            error = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvIdentity->SignSize != 0) {
            error = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_IDSIGN),
                ospvIdentity->SignSize, ospvIdentity->IdSign,
                OSPC_OSNULL, OSPC_OSNULL, OSPC_TRUE, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
         }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvIdentity->IdAlg[0] != '\0') {
            error = OSPPStringToElement(OSPC_MELEM_IDALG, ospvIdentity->IdAlg, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvIdentity->IdInfo[0] != '\0') {
            error = OSPPStringToElement(OSPC_MELEM_IDINFO, ospvIdentity->IdInfo, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvIdentity->IdType[0] != '\0') {
            error = OSPPStringToElement(OSPC_MELEM_IDTYPE, ospvIdentity->IdType, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvIdentity->CanonSize != 0) {
            error = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_IDCANON),
                ospvIdentity->CanonSize, ospvIdentity->IdCanon,
                OSPC_OSNULL, OSPC_OSNULL, OSPC_TRUE, &elem);
            if (error == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
         }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((error != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return error;
}

int OSPPIdentityFromElement(
    OSPT_XML_ELEM *ospvElem,
    OSPT_IDENTITY *ospvIdentity)
{
    unsigned char *value = OSPC_OSNULL;
    unsigned size = 0;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    int error = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        error = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (ospvIdentity == OSPC_OSNULL) {
            error = OSPC_ERR_XML_INVALID_ARGS;;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        for (elem = OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (error == OSPC_ERR_NO_ERROR);
            elem = OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_IDSIGN:
                size = sizeof(ospvIdentity->IdSign);
                error = OSPPMsgBinFromElement(elem, &size, &value);
                if (error == OSPC_ERR_NO_ERROR) {
                    ospvIdentity->SignSize = size;
                    OSPM_MEMCPY(ospvIdentity->IdSign, value, size);
                    OSPM_FREE(value);
                } else {
                    ospvIdentity->SignSize = 0;
                    error = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            case OSPC_MELEM_IDALG:
                OSPM_STRNCPY(ospvIdentity->IdAlg, OSPPXMLElemGetValue(elem), sizeof(ospvIdentity->IdAlg));
                break;
            case OSPC_MELEM_IDINFO:
                OSPM_STRNCPY(ospvIdentity->IdInfo, OSPPXMLElemGetValue(elem), sizeof(ospvIdentity->IdInfo));
                break;
            case OSPC_MELEM_IDTYPE:
                OSPM_STRNCPY(ospvIdentity->IdType, OSPPXMLElemGetValue(elem), sizeof(ospvIdentity->IdType));
                break;
            case OSPC_MELEM_IDCANON:
                size = sizeof(ospvIdentity->IdCanon);
                error = OSPPMsgBinFromElement(elem, &size, &value);
                if (error == OSPC_ERR_NO_ERROR) {
                    ospvIdentity->CanonSize = size;
                    OSPM_MEMCPY(ospvIdentity->IdCanon, value, size);
                    OSPM_FREE(value);
                } else {
                    ospvIdentity->CanonSize = 0;
                    error = OSPC_ERR_XML_BAD_ELEMENT;
                }
                break;
            default:
                /*
                 * This is an element we don't understand. If it's
                 * critical, then we have to report an error.
                 * Otherwise we can ignore it.
                 */
//                if (OSPPMsgElemIsCritical(elem)) {
//                    error = OSPC_ERR_XML_BAD_ELEMENT;
//                }
                break;
            }
        }
    }

    return error;
}
