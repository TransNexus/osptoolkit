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

/* ospusage.c - OSP usage detail functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospusage.h"
#include "osp/ospmsg.h"

/*
 * OSPPUsageFromElement() - get usage detail from an XML element
 */
unsigned OSPPUsageFromElement(  /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    unsigned *ospvUsage)        /* where to put usage value */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    unsigned gotAmount = OSPC_FALSE;
    unsigned gotIncrement = OSPC_FALSE;
    unsigned gotUnit = OSPC_FALSE;
    unsigned long amount = 0;
    unsigned long increment = 0;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvUsage == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_NO_USAGE;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR);
            elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_AMOUNT:
                gotAmount = OSPC_TRUE;
                errcode = OSPPMsgNumFromElement(elem, &amount);
                break;
            case OSPC_MELEM_INCREMENT:
                errcode = OSPPMsgNumFromElement(elem, &increment);
                gotIncrement = OSPC_TRUE;
                break;
            case OSPC_MELEM_SERVICE:
// TODO: Added ServiceType
                break;
            case OSPC_MELEM_UNIT:
                gotUnit = OSPC_TRUE;
                /*
                 * The only unit we support is seconds. If this is
                 * different, then we declare an error.
                 */
                if (OSPM_STRCMP(OSPPXMLElemGetValue(elem), "s") != 0) {
                    errcode = OSPC_ERR_BAD_SERVICE;
                }
                break;
            case OSPC_MELEM_TERMCAUSE:
// TODO: Handle TCCode
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
        if (gotAmount && gotIncrement && gotUnit) {
            *ospvUsage = (unsigned)increment * (unsigned)amount;
        } else {
            errcode = OSPC_ERR_XML_BAD_ELEMENT;
        }
    }

    return errcode;
}

/*
 * OSPPAddPricingInfoToUsageElement() - adds pricing info to an xml element
 */
unsigned OSPPAddPricingInfoToUsageElement(
    OSPT_PRICING_INFO PricingInfo,
    OSPT_XML_ELEM **ospvElem)       /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_PRICINGIND), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgFloatToElement(PricingInfo.amount, OSPPMsgElemGetName(OSPC_MELEM_AMOUNT), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* now add the increment */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgNumToElement(PricingInfo.increment, OSPPMsgElemGetName(OSPC_MELEM_INCREMENT), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* now we need to add units */
    if (errcode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_UNIT), (const char *)PricingInfo.unit);
        if (elem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* add currency */
    if (errcode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CURRENCY), (const char *)PricingInfo.currency);
        if (elem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPAddConfIdToUsageElement() - Add conf ID to usage
 */
unsigned OSPPAddConfIdToUsageElement(
    const char *ospvConferenceId,
    OSPT_XML_ELEM **ospvElem)           /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_GROUP), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvConferenceId && ospvConferenceId[0] != '\0') {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_GROUPID), (const char *)ospvConferenceId);
            if (elem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPUsageToElement() - create an XML element from usage detail
 */
unsigned OSPPUsageToElement(        /* returns error code */
    unsigned ospvUsage,             /* usage value */
    OSPTTIME ospvStartTime,         /* optional, if not 0, call start time */
    OSPTTIME ospvEndTime,           /* optional, if not 0, call end time */
    OSPTTIME ospvAlertTime,         /* optional, if not 0, call alert time */
    OSPTTIME ospvConnectTime,       /* optional, if not 0, call connect time */
    OSPTBOOL ospvHasPDDInfo,        /* Is PDD info present variable */
    unsigned ospvPostDialDelay,     /* PDD value, in milliseconds */
    OSPE_RELEASE ospvReleaseSource, /* Release source value */
    OSPT_XML_ELEM *ospvElem)        /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    /* now add the amount (which is the usage) */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgNumToElement(ospvUsage, OSPPMsgElemGetName(OSPC_MELEM_AMOUNT), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* now add the increment - for us always 1 */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgNumToElement(1, OSPPMsgElemGetName(OSPC_MELEM_INCREMENT), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* the units are seconds */
    if (errcode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_UNIT), "s");
        if (elem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call start time */
    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvStartTime != 0)) {
        errcode = OSPPMsgTimeToElement(ospvStartTime, OSPPMsgElemGetName(OSPC_MELEM_STARTTIME), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call end time */
    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvEndTime != 0)) {
        errcode = OSPPMsgTimeToElement(ospvEndTime, OSPPMsgElemGetName(OSPC_MELEM_ENDTIME), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call alert time */
    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvAlertTime != 0)) {
        errcode = OSPPMsgTimeToElement(ospvAlertTime, OSPPMsgElemGetName(OSPC_MELEM_ALERTTIME), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call connect time */
    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvConnectTime != 0)) {
        errcode = OSPPMsgTimeToElement(ospvConnectTime, OSPPMsgElemGetName(OSPC_MELEM_CONNECTTIME), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* now add the PDD */
    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvHasPDDInfo)) {
        errcode = OSPPMsgFloatToElement((float)ospvPostDialDelay / 1000, OSPPMsgElemGetName(OSPC_MELEM_POSTDIALDELAY), &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_SOURCE));
            if (attr == OSPC_OSNULL) {
                OSPPXMLElemDelete(&elem);
            } else {
               OSPPXMLElemAddAttr(elem, attr);
               attr = OSPC_OSNULL;
               OSPPXMLElemAddChild(ospvElem, elem);
               elem = OSPC_OSNULL;
            }
        }
    }

    /* now add the Rel src */
    if ((errcode == OSPC_ERR_NO_ERROR) &&
        ((ospvReleaseSource == OSPC_RELEASE_UNKNOWN) || ((ospvReleaseSource >= OSPC_RELEASE_START) && (ospvReleaseSource < OSPC_RELEASE_NUMBER))))
    {
        errcode = OSPPStringToElement(OSPC_MELEM_RELEASE, OSPPReleaseGetName(ospvReleaseSource), 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (elem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&elem);
        }
        if (ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPCallPartyNumToElement() - adds call party number to an xml element
 */
unsigned OSPPCallPartyNumToElement(
    OSPE_MSG_ELEM ElemType,             /* Source/destination info */
    const char *CallPartyNum,           /* Called/calling number */
    OSPE_NUMBER_FORMAT CallPartyFormat, /* Number format */
    OSPT_XML_ELEM **ospvElem)           /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (ElemType) {
        case OSPC_MELEM_ASSERTEDID:
        case OSPC_MELEM_CHARGEINFO:
        case OSPC_MELEM_CONTACT:
        case OSPC_MELEM_DESTINFO:
        case OSPC_MELEM_DIVSRCINFO:
        case OSPC_MELEM_FROM:
        case OSPC_MELEM_IDBYPASS:
        case OSPC_MELEM_IDENTITY:
        case OSPC_MELEM_NETTRANSCALLED:
        case OSPC_MELEM_RPID:
        case OSPC_MELEM_SRCINFO:
        case OSPC_MELEM_TO:
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(ElemType), CallPartyNum);
            if (*ospvElem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                switch (CallPartyFormat) {
                case OSPC_NFORMAT_CNAM:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_CNAM));
                    break;
                case OSPC_NFORMAT_DISPLAYNAME:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_DISPLAYNAME));
                    break;
                case OSPC_NFORMAT_E164:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_E164));
                    break;
                case OSPC_NFORMAT_SIP:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_SIP));
                    break;
                case OSPC_NFORMAT_TRANSPORT:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRANSPORT));
                    break;
                case OSPC_NFORMAT_URL:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_URL));
                    break;
                default:
                    errcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
                    break;
                }
                if (errcode == OSPC_ERR_NO_ERROR) {
                    if (attr == OSPC_OSNULL) {
                        OSPPXMLElemDelete(ospvElem);
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(*ospvElem, attr);
                    }
                }
            }
            break;
        default:
            errcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
            break;
        }
    }

    return errcode;
}

/*
 * OSPPTermCauseToElement() - adds termiantion cause to an xml element
 */
unsigned OSPPTermCauseToElement(
    OSPE_TERM_CAUSE TCType,     /* Termination cause type */
    unsigned TCCode,            /* Termination cause */
    const char *TCDesc,         /* Termination cause description */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_TERMCAUSE), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (TCType) {
        case OSPC_TCAUSE_H323:
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_H323));
            break;
        case OSPC_TCAUSE_Q850:
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_Q850));
            break;
        case OSPC_TCAUSE_SIP:
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_SIP));
            break;
        case OSPC_TCAUSE_XMPP:
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_XMPP));
            break;
        default:
            errcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
            break;
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            if (attr == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgNumToElement(TCCode, OSPPMsgElemGetName(OSPC_MELEM_TCCODE), &elem);
        if (elem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((TCDesc != OSPC_OSNULL) && (TCDesc[0] != '\0')) {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESC), TCDesc);
            if (elem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPStatusToElement() - adds status to an xml element
 */
unsigned OSPPStatusToElement(
    int Code,                   /* Status code */
    const char *Desc,           /* Status description */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_STATUS), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (Code != -1) {
            errcode = OSPPMsgNumToElement(Code, OSPPMsgElemGetName(OSPC_MELEM_CODE), &elem);
            if (elem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((Desc != OSPC_OSNULL) && (Desc[0] != '\0')) {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESC), Desc);
            if (elem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPStringToElement() - adds normal string to an xml element
 */
unsigned OSPPStringToElement(
    OSPE_MSG_ELEM ElemType,         /* Element type */
    const char *ElemValue,          /* Element value */
    unsigned AttrNum,               /* Number of attributes */
    OSPE_MSG_ATTR AttrType[],       /* Attribute type */
    OSPE_ALTINFO AttrValue[],       /* Attribute value */
    OSPT_XML_ELEM **ospvElem)       /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    unsigned cnt;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((ElemType < OSPC_MELEM_START) || (ElemType >= OSPC_MELEM_NUMBER)) {
            errcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(ElemType), ElemValue);
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        for (cnt = 0; cnt < AttrNum; cnt++) {
            if (((AttrType[cnt] < OSPC_MATTR_START) || (AttrType[cnt] >= OSPC_MATTR_NUMBER)) ||
                ((AttrValue[cnt] < OSPC_ALTINFO_START) || (AttrValue[cnt] >= OSPC_ALTINFO_NUMBER)))
            {
                OSPPXMLElemDelete(ospvElem);
                errcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
                break;
            } else {
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(AttrType[cnt]), OSPPAltInfoTypeGetName(AttrValue[cnt]));
                if (attr == OSPC_OSNULL) {
                    OSPPXMLElemDelete(ospvElem);
                    errcode = OSPC_ERR_XML_NO_ATTR;
                    break;
                } else {
                   OSPPXMLElemAddAttr(*ospvElem, attr);
                }
            }
        }
    }

    return errcode;
}

/*
 * OSPPCustomInfoToElement() - adds user-defined info to an xml element
 */
unsigned OSPPCustomInfoToElement(
    unsigned Index,             /* Index */
    const char *CustomInfo,     /* User-defined info */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char buffer[OSPC_SIZE_NORSTR];
    OSPTBOOL isbase64 = OSPC_TRUE;
    OSPE_MSG_ATTR attrtype = OSPC_MATTR_INDEX;
    OSPE_ALTINFO attrvalue = OSPC_ALTINFO_CAPTCHAINDEX;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((Index > OSPC_MAX_INDEX) || (CustomInfo == OSPC_OSNULL) || (CustomInfo[0] == '\0')) {
            errcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (Index == OSPC_CAPTCHA_INDEX) {
            /* For CAPTCHA flag */
            errcode = OSPPStringToElement(OSPC_MELEM_CUSTINFO, CustomInfo,
                1, &attrtype, &attrvalue,
                ospvElem);
        } else {
            snprintf(buffer, sizeof(buffer), "%d", Index + 1);
            errcode = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_CUSTINFO),
                OSPM_STRLEN(CustomInfo), (unsigned char *)CustomInfo,
                OSPPMsgAttrGetName(OSPC_MATTR_INDEX), buffer, isbase64,
                ospvElem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPStiAttestToElement() - adds STIR attestation to a xml element
 */
unsigned OSPPStiAttestToElement(
    char StiAttest,             /* STI attest */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        const char attest[2] = { toupper(StiAttest), '\0' }; 
        switch (StiAttest) {
        case 'A':
        case 'a':
        case 'B':
        case 'b':
        case 'C':
        case 'c':
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_ATTEST), attest);
            if (*ospvElem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            }
            break;
        case '\0':
        default:
            errcode = OSPC_ERR_XML_BAD_ELEMENT;
            break;
        }
    }

    return errcode;
}

/*
 * OSPPCertInfoToElement() - adds STIR certificate information to an xml element
 */
unsigned OSPPCertInfoToElement(
    int CertCached,             /* STI certificate cached flag */
    int CertLatency,            /* STI certificate latency in ms */
    const char *CertUrl,        /* STI certificate URL */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CERTINFO), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (CertCached != -1) {
            if (CertCached == 0) {
                elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CACHED), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE));
            } else {
                elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CACHED), OSPPAltInfoTypeGetName(OSPC_ALTINFO_TRUE));
            }
            if (elem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (CertLatency != -1) {
            errcode = OSPPMsgFloatToElement((float)CertLatency / 1000, OSPPMsgElemGetName(OSPC_MELEM_LATENCY), &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((CertUrl != OSPC_OSNULL) && (CertUrl[0] != '\0')) {
            errcode = OSPPStringToElement(OSPC_MELEM_URL, CertUrl, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPOobToElement() - adds STIR OOB to a xml element
 */
unsigned OSPPOobToElement(
    int CpsLatency,             /* STI CPS latency in ms */
    int CpsResponseCode,        /* STI CPS URL */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_OOB), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (CpsLatency != -1) {
            errcode = OSPPMsgFloatToElement((float)CpsLatency / 1000, OSPPMsgElemGetName(OSPC_MELEM_CPSLATENCY), &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (CpsResponseCode != -1) {
            errcode = OSPPMsgNumToElement(CpsResponseCode, OSPPMsgElemGetName(OSPC_MELEM_CPSRSPCODE), &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPStiAsToElement() - adds STIR information of authentication to an xml element
 */
unsigned OSPPStiAsToElement(
    const char *StiAsStatus,    /* STI AS status */
    char StiAsAttest,           /* STI AS attest */
    const char *StiAsOrigId,    /* STI AS origid */
    int StiAsCpsLatency,        /* STI AS CPS latency */
    int StiAsCpsRspCode,        /* STI AS CPS response code */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_STIRINFO), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_AUTHENTICATION));
        if (attr == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ATTR;
        } else {
            OSPPXMLElemAddAttr(*ospvElem, attr);
            attr = OSPC_OSNULL;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((StiAsStatus != OSPC_OSNULL) && (StiAsStatus[0] != '\0')) {
            errcode = OSPPStatusToElement(-1, StiAsStatus, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (StiAsAttest != '\0') {
            errcode = OSPPStiAttestToElement(StiAsAttest, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((StiAsOrigId != OSPC_OSNULL) &&(StiAsOrigId[0] != '\0')) {
            errcode = OSPPStringToElement(OSPC_MELEM_ORIGID, StiAsOrigId, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((StiAsCpsLatency != -1) || (StiAsCpsRspCode != -1)) {
            errcode = OSPPOobToElement(StiAsCpsLatency, StiAsCpsRspCode, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

/*
 * OSPPStiVsToElement() - adds STIR information of verification to an xml element
 */
unsigned OSPPStiVsToElement(
    const char *StiVsStatus,    /* STI VS status */
    char StiVsAttest,           /* STI VS attest */
    const char *StiVsOrigId,    /* STI VS origid */
    int StiVsCertCached,        /* STI VS certificate cached flag */
    int StiVsCertLatency,       /* STI VS certificate latency in ms */
    const char *StiVsCertUrl,   /* STI VS certificate URL */
    int StiVsCpsLatency,        /* STI VS CPS latency */
    int StiVsCpsRspCode,        /* STI VS CPS response code */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_STIRINFO), "");
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_VERIFICATION));
        if (attr == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ATTR;
        } else {
            OSPPXMLElemAddAttr(*ospvElem, attr);
            attr = OSPC_OSNULL;
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((StiVsStatus != OSPC_OSNULL) && (StiVsStatus[0] != '\0')) {
            errcode = OSPPStatusToElement(-1, StiVsStatus, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (StiVsAttest != '\0') {
            errcode = OSPPStiAttestToElement(StiVsAttest, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((StiVsOrigId != OSPC_OSNULL) && (StiVsOrigId[0] != '\0')) {
            errcode = OSPPStringToElement(OSPC_MELEM_ORIGID, StiVsOrigId, 0, OSPC_OSNULL, OSPC_OSNULL, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((StiVsCertCached != -1) || (StiVsCertLatency != -1) || ((StiVsCertUrl != OSPC_OSNULL) && (StiVsCertUrl[0] != '\0'))) {
            errcode = OSPPCertInfoToElement(StiVsCertCached, StiVsCertLatency, StiVsCertUrl, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if ((StiVsCpsLatency != -1) || (StiVsCpsRspCode != -1)) {
            errcode = OSPPOobToElement(StiVsCpsLatency, StiVsCpsRspCode, &elem);
            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddChild(*ospvElem, elem);
                elem = OSPC_OSNULL;
            }
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (errcode != OSPC_ERR_NO_ERROR) {
        if (*ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(ospvElem);
        }
    }

    return errcode;
}

