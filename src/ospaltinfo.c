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

const OSPT_MSG_DESC OSPC_ATYPE_DESCS[] = {
    { OSPC_ATYPE_UNKNOWN,       "" },
    { OSPC_ATYPE_E164,          "e164" },
    { OSPC_ATYPE_H323,          "h323" },
    { OSPC_ATYPE_URL,           "url" },
    { OSPC_ATYPE_EMAIL,         "email" },
    { OSPC_ATYPE_TRANSPORT,     "transport" },
    { OSPC_ATYPE_INTERNATIONAL, "international" },
    { OSPC_ATYPE_NATIONAL,      "national" },
    { OSPC_ATYPE_NETWORK,       "network" },
    { OSPC_ATYPE_SUBSCRIBER,    "subscriber" },
    { OSPC_ATYPE_ABBREVIATED,   "abbreviated" },
    { OSPC_ATYPE_E164PREFIX,    "e164prefix" },
    { OSPC_ATYPE_SIP,           "sip" },
    { OSPC_ATYPE_DEVICEID,      "deviceId" }
};

/*
 * OSPPAltInfoNew() - create a new altinfo object
 */
OSPTALTINFO *OSPPAltInfoNew(        /* returns ptr to altinfo or null */
    unsigned ospvLen,               /* size of altinfo */
    const unsigned char *ospvValue, /* altinfo value */
    OSPE_ALTINFO_TYPE ospvType)
{
    OSPTALTINFO *ospvAltInfo = OSPC_OSNULL;
    unsigned char *valptr;

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
            OSPM_MALLOC(ospvAltInfo, OSPTALTINFO, sizeof(OSPTALTINFO) + ospvLen + 1);

            /* make sure the allocation succeeded before proceeding */
            if (ospvAltInfo != OSPC_OSNULL) {
                /* calculate where the "hidden" values will go */
                valptr = ((unsigned char *)ospvAltInfo) + sizeof(OSPTALTINFO);

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

    return (ospvAltInfo);
}

/*
 * OSPPAltInfoDelete() - destroy a altinfo object
 */
void OSPPAltInfoDelete(
    OSPTALTINFO **ospvAltInfo)  /* AltInfo to destroy */
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
    OSPTALTINFO *ospvAltInfo)
{
    unsigned ospvSize = 0;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvSize = ospvAltInfo->ospmAltInfoLen;
    }

    return (ospvSize);
}

/*
 * OSPPAltInfoGetType() - returns altinfo type
 */
OSPE_ALTINFO_TYPE OSPPAltInfoGetType(
    OSPTALTINFO *ospvAltInfo)
{
    OSPE_ALTINFO_TYPE ospvType = OSPC_ATYPE_UNKNOWN;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvType = (OSPE_ALTINFO_TYPE)ospvAltInfo->ospmAltInfoType;
    }

    return (ospvType);
}

/*
 * OSPPAltInfoGetValue() - returns pointer to altinfo value
 */
const unsigned char *OSPPAltInfoGetValue(
    OSPTALTINFO *ospvAltInfo)
{
    const unsigned char *ospvVal = OSPC_OSNULL;

    if (ospvAltInfo != OSPC_OSNULL) {
        ospvVal = ospvAltInfo->ospmAltInfoVal;
    }

    return (ospvVal);
}

/*
 * OSPPAltInfoToElement() - create an XML element from a altinfo
 */
unsigned OSPPAltInfoToElement(  /* returns error code */
    OSPTALTINFO * ospvAltInfo,  /* In - altinfo */
    OSPTXMLELEM ** ospvElem,    /* Out - XML element pointer */
    OSPE_MSG_ELEM ospvPart)     /* In -source or dest alternate */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTXMLATTR *attr = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvAltInfo == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NO_ALTINFO;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        *ospvElem = OSPPXMLElemNew(OSPPMsgElemGetName(ospvPart), (const char *)OSPPAltInfoGetValue(ospvAltInfo));

        if (ospvElem != OSPC_OSNULL) {
            attr = OSPPXMLAttrNew((const unsigned char *)OSPPMsgAttrGetName(OSPC_MATTR_TYPE), 
                       (const unsigned char *)OSPPAltInfoTypeGetName(ospvAltInfo->ospmAltInfoType));

            if (attr == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_XML_NO_ATTR;
            }

            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPXMLElemAddAttr(*ospvElem, attr);
            }
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPAltInfoTypeGetName() - get type name from an altinfo
 */
const char *OSPPAltInfoTypeGetName( /* Returns a pointer to the name */
    OSPE_ALTINFO_TYPE ospvPart)
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvPart != OSPC_ATYPE_UNKNOWN) {
        ospvName = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPC_ATYPE_DESCS, OSPC_ATYPE_NUMBER);
    }

    return (ospvName);
}
