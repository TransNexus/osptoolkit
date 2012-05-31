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

/* ospaltinfo.c - OSP alt info functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospmsgdesc.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"
#include "osp/ospmsgattr.h"
#include "osp/ospaltinfo.h"

const OSPT_MSG_DESC OSPV_ATYPE_DESCS[OSPC_ALTINFO_NUMBER] = {
    /* For Alt/Info attributes */
    { OSPC_ALTINFO_E164,            "e164" },
    { OSPC_ALTINFO_H323,            "h323" },
    { OSPC_ALTINFO_URL,             "url" },
    { OSPC_ALTINFO_EMAIL,           "email" },
    { OSPC_ALTINFO_TRANSPORT,       "transport" },
    { OSPC_ALTINFO_INTERNATIONAL,   "international" },
    { OSPC_ALTINFO_NATIONAL,        "national" },
    { OSPC_ALTINFO_NETWORK,         "network" },
    { OSPC_ALTINFO_SUBSCRIBER,      "subscriber" },
    { OSPC_ALTINFO_ABBREVIATED,     "abbreviated" },
    { OSPC_ALTINFO_E164PREFIX,      "e164prefix" },
    { OSPC_ALTINFO_SIP,             "sip" },
    { OSPC_ALTINFO_XMPP,            "xmpp" },
    { OSPC_ALTINFO_Q850,            "q850" },
    { OSPC_ALTINFO_DEVICEID,        "deviceId" },
    { OSPC_ALTINFO_NPRN,            "routingnumber" },
    { OSPC_ALTINFO_NPCIC,           "cic" },
    { OSPC_ALTINFO_NPDI,            "npdi" },
    { OSPC_ALTINFO_SPID,            "spid" },
    { OSPC_ALTINFO_OCN,             "ocn" },
    { OSPC_ALTINFO_SPN,             "spn" },
    { OSPC_ALTINFO_ALTSPN,          "altspn" },
    { OSPC_ALTINFO_MCC,             "mcc" },
    { OSPC_ALTINFO_MNC,             "mnc" },
    { OSPC_ALTINFO_SMS,             "sms" },
    { OSPC_ALTINFO_MMS,             "mms" },
    { OSPC_ALTINFO_DISPLAYNAME,     "displayname" },
    /* For other attributes */
    { OSPC_ALTINFO_TRUE,            "true" },
    { OSPC_ALTINFO_FALSE,           "false" },
    { OSPC_ALTINFO_BASE64,          "base64" },
    { OSPC_ALTINFO_CDATA,           "cdata" },
    { OSPC_ALTINFO_SOURCE,          "source" },
    { OSPC_ALTINFO_DESTINATION,     "destination" },
    { OSPC_ALTINFO_CORRELATION,     "correlation" },
    { OSPC_ALTINFO_LOCAL,           "local" },
    { OSPC_ALTINFO_REMOTE,          "remote" },
    { OSPC_ALTINFO_NA,              "na" },
    { OSPC_ALTINFO_RTP,             "rtp" },
    { OSPC_ALTINFO_RTCP,            "rtcp" },
    { OSPC_ALTINFO_SRCREP,          "source-to-reporter" },
    { OSPC_ALTINFO_REPSRC,          "reporter-to-source" },
    { OSPC_ALTINFO_REPDEST,         "reporter-to-destination" },
    { OSPC_ALTINFO_DESTREP,         "destination-to-reporter" },
    { OSPC_ALTINFO_SRCDEST,         "source-to-destination" },
    { OSPC_ALTINFO_DESTSRC,         "destination-to-source" },
    { OSPC_ALTINFO_CALLING,         "calling" },
    { OSPC_ALTINFO_CALLED,          "called" },
    { OSPC_ALTINFO_PROXY,           "proxy" }
};

/*
 * OSPPAltInfoNew() - create a new altinfo object
 */
OSPT_ALTINFO *OSPPAltInfoNew(   /* returns ptr to altinfo or null */
    unsigned ospvLen,           /* size of altinfo */
    const char *ospvValue,      /* altinfo value */
    OSPE_ALTINFO ospvType)
{
    OSPT_ALTINFO *ospvAltInfo = OSPC_OSNULL;
    char *valptr;

    /*
     * AltInfo objects are actually two parts -- the first is the AltInfo
     * structure and the second is the altinfo value. The two parts are
     * contiguous in memory, and are created (and destroyed) at the same
     * time. Here's a rough picture:
     *
     *    Memory:
     *      +--------+
     *      |        |  This part is the structure itself. It's actually
     *      |        |  visible to the functions (and macros).
     *      +--------+
     *      |        |
     *      |        |  This part contains the actual value. It's not directly
     *      |        |  accessible, but pointers to the value within
     *      |        |  the structure are set to point to it.
     *      +--------+
     *
     * We do it this way to conveniently manage variable size altinfos
     * If we put the value in a visible part of the structure,
     * then we'd have to define that field, and that would require
     * defining a size for it. Since we can't predict the size of
     * altinfo values , picking the size for the structure would be pretty
     * difficult.
     *
     * Note that this technique does use dynamic memory allocation.
     * If memory fragmentation is a concern, one possible optimization
     * would be to define a pool of AltInfo objects in which the name and
     * value sizes were fixed to some reasonable size. Then you could
     * allocate from the pool in most cases, and fall back to this
     * approach only if the name/value was too big for the pool objects
     * (or, perhaps, if the pool was empty).
     */

    if (ospvLen > 0) {
        if (ospvValue != OSPC_OSNULL) {
            /* try to allocate the memory for the entire object */
            OSPM_MALLOC(ospvAltInfo, OSPT_ALTINFO, sizeof(OSPT_ALTINFO) + ospvLen + 1);
            /* make sure the allocation succeeded before proceeding */
            if (ospvAltInfo != OSPC_OSNULL) {
                /* calculate where the "hidden" values will go */
                valptr = (char *)ospvAltInfo + sizeof(OSPT_ALTINFO);

                /* copy the values into their hidden location */
                OSPM_MEMCPY(valptr, ospvValue, ospvLen);

                /* fill in the structure fields */
                OSPPListLinkNew(&ospvAltInfo->Link);
                ospvAltInfo->Length = ospvLen;
                ospvAltInfo->Type = ospvType;
                ospvAltInfo->Value = valptr;
                ospvAltInfo->Value[ospvLen] = '\0';
            }
        }
    }

    return ospvAltInfo;
}

/*
 * OSPPAltInfoDelete() - destroy a altinfo object
 */
void OSPPAltInfoDelete(
    OSPT_ALTINFO **ospvAltInfo)     /* AltInfo to destroy */
{
    if (*ospvAltInfo != OSPC_OSNULL) {
        OSPM_FREE(*ospvAltInfo);
        *ospvAltInfo = OSPC_OSNULL;
    }
}

/*
 * OSPPAltInfoGetSize() - returns size of altinfo value
 */
unsigned OSPPAltInfoGetSize(
    OSPT_ALTINFO *ospvAltInfo)
{
    unsigned size = 0;

    if (ospvAltInfo != OSPC_OSNULL) {
        size = ospvAltInfo->Length;
    }

    return size;
}

/*
 * OSPPAltInfoGetPart() - returns altinfo type
 */
OSPE_ALTINFO OSPPAltInfoGetPart(
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPE_ALTINFO type = OSPC_ALTINFO_UNKNOWN;

    if (ospvAltInfo != OSPC_OSNULL) {
        type = (OSPE_ALTINFO)ospvAltInfo->Type;
    }

    return type;
}

/*
 * OSPPAltInfoGetValue() - returns pointer to altinfo value
 */
const char *OSPPAltInfoGetValue(
    OSPT_ALTINFO *ospvAltInfo)
{
    const char *value = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        value = ospvAltInfo->Value;
    }

    return value;
}

/*
 * OSPPAltInfoToElement() - create an XML element from a altinfo
 */
unsigned OSPPAltInfoToElement(      /* returns error code */
    OSPT_ALTINFO *ospvAltInfo,      /* In - altinfo */
    OSPT_XML_ELEM **ospvElem,       /* Out - XML element pointer */
    OSPE_MSG_ELEM ospvPart)         /* In -source or dest alternate */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvAltInfo == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NO_ALTINFO;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(ospvPart), OSPPAltInfoGetValue(ospvAltInfo));
        if (ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        } else {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(ospvAltInfo->Type));
            if (attr == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ATTR;
            } else {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }
    }

    return errcode;
}

/*
 * OSPPAltInfoTypeGetPart() - get type from an altinfo name
 */
OSPE_ALTINFO OSPPAltInfoTypeGetPart(
    const char *ospvName)
{
    OSPE_ALTINFO ospvPart = OSPC_ALTINFO_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        ospvPart = (OSPE_ALTINFO)OSPPMsgDescGetPart(ospvName, OSPV_ATYPE_DESCS, OSPC_ALTINFO_NUMBER);
    }

    return ospvPart;
}

/*
 * OSPPAltInfoTypeGetName() - get type name from an altinfo type
 */
const char *OSPPAltInfoTypeGetName( /* Returns a pointer to the name */
    OSPE_ALTINFO ospvPart)
{
    const char *ospvName = OSPC_OSNULL;

    if ((ospvPart >= OSPC_ALTINFO_START) && (ospvPart < OSPC_ALTINFO_NUMBER)) {
        ospvName = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_ATYPE_DESCS, OSPC_ALTINFO_NUMBER);
    }

    return ospvName;
}

