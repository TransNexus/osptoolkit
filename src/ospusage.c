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
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    unsigned gotAmount = OSPC_FALSE;
    unsigned gotIncrement = OSPC_FALSE;
    unsigned gotUnit = OSPC_FALSE;
    unsigned long amount = 0;
    unsigned long increment = 0;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvUsage == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_USAGE;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        for (elem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
            (elem != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            elem = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, elem))
        {
            switch (OSPPMsgElemGetPart(OSPPXMLElemGetName(elem))) {
            case OSPC_MELEM_AMOUNT:
                gotAmount = OSPC_TRUE;
                ospvErrCode = OSPPMsgNumFromElement(elem, &amount);
                break;
            case OSPC_MELEM_INCREMENT:
                ospvErrCode = OSPPMsgNumFromElement(elem, &increment);
                gotIncrement = OSPC_TRUE;
                break;
            case OSPC_MELEM_SERVICE:
                /* we don't do anything with service at this point */
                break;
            case OSPC_MELEM_UNIT:
                gotUnit = OSPC_TRUE;
                /*
                 * The only unit we support is seconds. If this is
                 * different, then we declare an error.
                 */
                if (OSPM_STRCMP(OSPPXMLElemGetValue(elem), "s") != 0) {
                    ospvErrCode = OSPC_ERR_BAD_SERVICE;
                }
                break;
            case OSPC_MELEM_TERMCAUSE:
// SDS TODO
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
        if (gotAmount && gotIncrement && gotUnit) {
            *ospvUsage = (unsigned)increment * (unsigned)amount;
        } else {
            ospvErrCode = OSPC_ERR_XML_BAD_ELEMENT;
        }
    }

    return ospvErrCode;
}

/*
 * OSPPAddPricingInfoToUsageElement() - adds pricing info to an xml element
 */
unsigned OSPPAddPricingInfoToUsageElement(
    OSPT_PRICING_INFO PricingInfo,
    OSPT_XML_ELEM **ospvElem)       /* where to put XML element pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_PRICINGIND), "");
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgFloatToElement(PricingInfo.amount, OSPPMsgElemGetName(OSPC_MELEM_AMOUNT), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* now add the increment */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgNumToElement(PricingInfo.increment, OSPPMsgElemGetName(OSPC_MELEM_INCREMENT), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* now we need to add units */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_UNIT), (const char *)PricingInfo.unit);
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* add currency */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_CURRENCY), (const char *)PricingInfo.currency);
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return ospvErrCode;
}

/*
 * OSPPAddConfIdToUsageElement() - Add conf ID to usage
 */
unsigned OSPPAddConfIdToUsageElement(
    const char *ospvConferenceId,
    OSPT_XML_ELEM **ospvElem)           /* where to put XML element pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_GROUP), "");
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (ospvConferenceId && ospvConferenceId[0] != '\0') {
            elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_GROUPID), (const char *)ospvConferenceId);
            if (elem == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLElemAddChild(*ospvElem, elem);
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return ospvErrCode;
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
    unsigned ospvPostDialDelay,     /* PDD value */
    unsigned ospvReleaseSource,     /* Release source value */
    OSPT_XML_ELEM *ospvElem)        /* where to put XML element pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    /* now add the amount (which is the usage) */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgNumToElement(ospvUsage, OSPPMsgElemGetName(OSPC_MELEM_AMOUNT), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* now add the increment - for us always 1 */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgNumToElement(1, OSPPMsgElemGetName(OSPC_MELEM_INCREMENT), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* the units are seconds */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_UNIT), "s");
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call start time */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvStartTime != 0)) {
        ospvErrCode = OSPPMsgTimeToElement(ospvStartTime, OSPPMsgElemGetName(OSPC_MELEM_STARTTIME), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call end time */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvEndTime != 0)) {
        ospvErrCode = OSPPMsgTimeToElement(ospvEndTime, OSPPMsgElemGetName(OSPC_MELEM_ENDTIME), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call alert time */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvAlertTime != 0)) {
        ospvErrCode = OSPPMsgTimeToElement(ospvAlertTime, OSPPMsgElemGetName(OSPC_MELEM_ALERTTIME), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* optional (if not null) call connect time */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvConnectTime != 0)) {
        ospvErrCode = OSPPMsgTimeToElement(ospvConnectTime, OSPPMsgElemGetName(OSPC_MELEM_CONNECTTIME), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* now add the PDD */
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (ospvHasPDDInfo)) {
        ospvErrCode = OSPPMsgNumToElement(ospvPostDialDelay, OSPPMsgElemGetName(OSPC_MELEM_POSTDIALDELAY), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* now add the Rel src */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgNumToElement(ospvReleaseSource, OSPPMsgElemGetName(OSPC_MELEM_RELEASESOURCE), &elem);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLElemAddChild(ospvElem, elem);
            elem = OSPC_OSNULL;
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if (ospvErrCode != OSPC_ERR_NO_ERROR) {
        if (elem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&elem);
        }
        if (ospvElem != OSPC_OSNULL) {
            OSPPXMLElemDelete(&ospvElem);
        }
    }

    return ospvErrCode;
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
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        switch (ElemType) {
        case OSPC_MELEM_SRCINFO:
        case OSPC_MELEM_DESTINFO:
        case OSPC_MELEM_DIVERSIONSRCINFO:
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(ElemType), CallPartyNum);
            if (*ospvElem == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                switch (CallPartyFormat) {
                case OSPC_NFORMAT_E164:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_E164));
                    break;
                case OSPC_NFORMAT_SIP:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_SIP));
                    break;
                case OSPC_NFORMAT_URL:
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_URL));
                    break;
                default:
                    ospvErrCode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
                    break;
                }
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    if (attr == OSPC_OSNULL) {
                        OSPPXMLElemDelete(ospvElem);
                        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(*ospvElem, attr);
                    }
                }
            }
            break;
        default:
            ospvErrCode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
            break;
        }
    }

    return ospvErrCode;
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
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* create the parent element */
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_TERMCAUSE), "");
        if (*ospvElem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            switch (TCType) {
            case OSPC_TCAUSE_Q850:
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_Q850));
                break;
            case OSPC_TCAUSE_H323:
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_H323));
                break;
            case OSPC_TCAUSE_SIP:
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_SIP));
                break;
            case OSPC_TCAUSE_XMPP:
                attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_XMPP));
                break;
            default:
                ospvErrCode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
                break;
            }
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                if (attr == OSPC_OSNULL) {
                    ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                } else {
                    OSPPXMLElemAddAttr(*ospvElem, attr);
                }
            }
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgNumToElement(TCCode, OSPPMsgElemGetName(OSPC_MELEM_TCCODE), &elem);
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (TCDesc != OSPC_OSNULL) && (TCDesc[0] != '\0')) {
        elem = OSPPXMLElemNew(OSPPMsgElemGetName(OSPC_MELEM_DESC), TCDesc);
        if (elem == OSPC_OSNULL) {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            OSPPXMLElemAddChild(*ospvElem, elem);
        }
    }

    /* if for any reason we found an error - destroy any elements created */
    if ((ospvErrCode != OSPC_ERR_NO_ERROR) && (*ospvElem != OSPC_OSNULL)) {
        OSPPXMLElemDelete(ospvElem);
    }

    return ospvErrCode;
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
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    unsigned cnt;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        if ((ElemType < OSPC_MELEM_START) || (ElemType > OSPC_MELEM_NUMBER)) {
            ospvErrCode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
        } else {
            *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(ElemType), ElemValue);
            if (*ospvElem == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                for (cnt = 0; cnt < AttrNum; cnt++) {
                    if (((AttrType[cnt] < OSPC_MATTR_START) || (AttrType[cnt] > OSPC_MATTR_NUMBER)) ||
                        ((AttrValue[cnt] < OSPC_ALTINFO_START) || (AttrValue[cnt] > OSPC_ALTINFO_NUMBER)))
                    {
                        OSPPXMLElemDelete(ospvElem);
                        ospvErrCode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
                        break;
                    } else {
                        attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(AttrType[cnt]), OSPPAltInfoTypeGetName(AttrValue[cnt]));
                        if (attr == OSPC_OSNULL) {
                            OSPPXMLElemDelete(ospvElem);
                            ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                            break;
                        } else {
                           OSPPXMLElemAddAttr(*ospvElem, attr);
                        }
                    }
                }
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPCustomInfoToElement() - adds user-defined info to an xml element
 */
unsigned OSPPCustomInfoToElement(
    unsigned Index,             /* Index */
    const char *CustomInfo,     /* User-defined info */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    char buffer[OSPC_SIZE_NORSTR];
    OSPTBOOL isbase64 = OSPC_TRUE;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    } else {
        if ((Index > OSPC_MAX_INDEX) || (CustomInfo == OSPC_OSNULL) || (CustomInfo[0] == '\0')) {
            ospvErrCode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
        } else {
            snprintf(buffer, sizeof(buffer), "%d", Index + 1);
            ospvErrCode = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_CUSTINFO),
                OSPM_STRLEN(CustomInfo), (unsigned char *)CustomInfo,
                OSPPMsgAttrGetName(OSPC_MATTR_INDEX), buffer, isbase64,
                ospvElem);
        }
    }

    return ospvErrCode;
}

