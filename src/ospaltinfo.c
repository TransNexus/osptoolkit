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
 * ospaltinfo.c - OSP alt info functions
 */

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
    { OSPC_ALTINFO_DEVICEID,        "deviceid" },
    { OSPC_ALTINFO_ASSERTEDID,      "assertedid" },
    { OSPC_ALTINFO_ROUTINGNUM,      "routingnumber" },
    /* For other attributes */
    { OSPC_ALTINFO_FALSE,           "flase" },
    { OSPC_ALTINFO_BASE64,          "base64" },
    { OSPC_ALTINFO_CDATA,           "cdata" },
    { OSPC_ALTINFO_FORWARD,         "forward" },
    { OSPC_ALTINFO_REVERSE,         "reverse" },
    { OSPC_ALTINFO_GENERAL,         "general" },
    { OSPC_ALTINFO_INBOUND,         "inbound" },
    { OSPC_ALTINFO_OUTBOUND,        "outbound" }
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
                OSPPListLinkNew(&ospvAltInfo->ospmAltInfoLink);
                ospvAltInfo->ospmAltInfoLen = ospvLen;
                ospvAltInfo->ospmAltInfoType = ospvType;
                ospvAltInfo->ospmAltInfoVal = valptr;
                ospvAltInfo->ospmAltInfoVal[ospvLen] = '\0';
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
    unsigned ospvSize = 0;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvSize = ospvAltInfo->ospmAltInfoLen;
    }

    return ospvSize;
}

/*
 * OSPPAltInfoTypeGetPart() - returns altinfo type
 */
OSPE_ALTINFO OSPPAltInfoTypeGetPart(
    OSPT_ALTINFO *ospvAltInfo)
{
    OSPE_ALTINFO ospvType = OSPC_ALTINFO_UNKNOWN;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvType = (OSPE_ALTINFO)ospvAltInfo->ospmAltInfoType;
    }

    return ospvType;
}

/*
 * OSPPAltInfoGetValue() - returns pointer to altinfo value
 */
const char *OSPPAltInfoGetValue(
    OSPT_ALTINFO *ospvAltInfo)
{
    const char *ospvVal = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvVal = ospvAltInfo->ospmAltInfoVal;
    }

    return ospvVal;
}

/*
 * OSPPAltInfoToElement() - create an XML element from a altinfo
 */
unsigned OSPPAltInfoToElement(      /* returns error code */
    OSPT_ALTINFO *ospvAltInfo,      /* In - altinfo */
    OSPT_XML_ELEM **ospvElem,       /* Out - XML element pointer */
    OSPE_MSG_ELEM ospvPart)         /* In -source or dest alternate */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvAltInfo == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_ALTINFO;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(ospvPart), OSPPAltInfoGetValue(ospvAltInfo));
        if (ospvElem != OSPC_OSNULL) {
            attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(ospvAltInfo->ospmAltInfoType));       
            if (attr == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }

            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPAltInfoTypeGetName() - get type name from an altinfo
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
