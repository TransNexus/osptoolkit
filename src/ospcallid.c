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

/* ospcallid.c - OSP call identifier functions */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/osplist.h"
#include "osp/ospxmldoc.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospmsgattr.h"
#include "osp/ospmsgelem.h"
#include "osp/ospmsg.h"

/*
 * OSPPCallIdNew() - create a new call identifier object
 */
OSPT_CALL_ID *OSPPCallIdNew(        /* returns ptr to call ID or null */
    unsigned ospvLen,               /* size of call ID */
    const unsigned char *ospvValue) /* call ID value */
{
    OSPT_CALL_ID *ospvCallId = OSPC_OSNULL;
    unsigned char *valptr;

    if (ospvLen > 0) {
        if (ospvValue != OSPC_OSNULL) {

            /*
             * CallId objects are actually two parts -- the first is the CallId
             * structure and the second is the call Id value. The two parts are
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
             * We do it this way to conveniently manage variable size call IDs
             * If we put the value in a visible part of the structure,
             * then we'd have to define that field, and that would require
             * defining a size for it. Since we can't predict the size of
             * callId values , picking the size for the structure would be pretty
             * difficult.
             *
             * Note that this technique does use dynamic memory allocation.
             * If memory fragmentation is a concern, one possible optimization
             * would be to define a pool of CallId objects in which the name and
             * value sizes were fixed to some reasonable size. Then you could
             * allocate from the pool in most cases, and fall back to this
             * approach only if the name/value was too big for the pool objects
             * (or, perhaps, if the pool was empty).
             */

            /* NOTE: The following code has a very similar structure to OSPPAltInfoNew()
             * in ospaltinfo.c and OSPPTokenNew() in osptoken.c. One might think that
             * the malloc'd size needs to be one byte bigger to allow for a null
             * character at the end of ospvValue; this is not so. Even though
             * ospvValue is an unsigned char, callid values are not always strings.
             * For this reason a null character should NOT be placed at the end of a
             * callid value.
             */

            /* try to allocate the memory for the entire object */
            OSPM_MALLOC(ospvCallId, OSPT_CALL_ID, sizeof(OSPT_CALL_ID) + ospvLen);

            /* make sure the allocation succeeded before proceeding */
            if (ospvCallId != OSPC_OSNULL) {
                /* calculate where the "hidden" values will go */
                valptr = ((unsigned char *)ospvCallId) + sizeof(OSPT_CALL_ID);

                /* copy the values into their hidden location */
                OSPM_MEMCPY(valptr, ospvValue, ospvLen);

                /* fill in the structure fields */
                OSPPListLinkNew(&ospvCallId->Link);
                ospvCallId->Length = ospvLen;
                ospvCallId->Value = valptr;
            }
        }
    }

    return ospvCallId;
}

/*
 * OSPPCallIdFromASCIIElement() - create a call id from an ASCII element
 */
unsigned OSPPCallIdFromASCIIElement(    /* returns error code */
    unsigned char *ospvElem,            /* input is ASCII element */
    OSPT_CALL_ID **ospvCallId)          /* where to put CallID pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char *callIdValue = OSPC_OSNULL;
    unsigned callIdLen = 0;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_ASCII_NO_ELEMENT;
    }
    if (ospvCallId == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NOCALLID;
    }

    /* start by assuming we will fail */
    *ospvCallId = OSPC_OSNULL;
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgBinFromASCIIElement(ospvElem, &callIdLen, &callIdValue);

        /* create the CallId structure */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            *ospvCallId = OSPPCallIdNew(callIdLen, callIdValue);
            if (*ospvCallId == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_DATA_NOCALLID;
            }
        }
    }

    if (callIdValue != OSPC_OSNULL) {
        OSPM_FREE(callIdValue);
    }

    return ospvErrCode;
}

/*
 * OSPPCallIdFromElement() - create a call id from an XML element
 */
unsigned OSPPCallIdFromElement( /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    OSPT_CALL_ID **ospvCallId)  /* where to put CallID pointer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char *callIdValue = OSPC_OSNULL;
    unsigned callIdLen = 0;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvCallId == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_DATA_NOCALLID;
    }
    /* assert(OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvElem)) == OSPC_MELEM_CALLID); */

    /* start by assuming we will fail */
    *ospvCallId = OSPC_OSNULL;
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPMsgBinFromElement(ospvElem, &callIdLen, &callIdValue);

        /* create the CallId structure */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            *ospvCallId = OSPPCallIdNew(callIdLen, callIdValue);
            if (*ospvCallId == OSPC_OSNULL) {
                ospvErrCode = OSPC_ERR_DATA_NOCALLID;
            }
        }
    }

    if (callIdValue != OSPC_OSNULL) {
        OSPM_FREE(callIdValue);
    }

    return ospvErrCode;
}

/*
 * OSPPCallIdDelete() - destroy a call identifier object
 */
void OSPPCallIdDelete(          /* no return */
    OSPT_CALL_ID **ospvCallId)  /* CallId to destroy */
{
    /* We free the whole callid at once because of the way it is constructed. */
    if ((*ospvCallId != OSPC_OSNULL) && (ospvCallId != OSPC_OSNULL)) {
        OSPM_FREE(*ospvCallId);
        *ospvCallId = OSPC_OSNULL;
    }
}

/*
 * OSPPCallIdToElement() - create an XML element from a call id
 */
unsigned OSPPCallIdToElement(   /* returns error code */
    OSPT_CALL_ID *ospvCallId,   /* Call ID */
    OSPT_XML_ELEM **ospvElem,   /* where to put XML element pointer */
    OSPTBOOL ospvIsBase64)      /* indicates base64 or cdata */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvCallId == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NOCALLID;
    }
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_CALLID),
            OSPPCallIdGetSize(ospvCallId), OSPPCallIdGetValue(ospvCallId),
            OSPC_OSNULL, OSPC_OSNULL, ospvIsBase64,
            ospvElem);
    }

    return errcode;
}

/*
 * OSPPCallIdGetSize() - returns size of call ID value
 */
unsigned OSPPCallIdGetSize(
    OSPT_CALL_ID *ospvCallId)
{
    unsigned size = 0;

    if (ospvCallId != OSPC_OSNULL) {
        size = ospvCallId->Length;
    }

    return size;
}

/*
 * OSPPCallIdGetValue() - returns pointer to call ID value
 */
unsigned char *OSPPCallIdGetValue(
    OSPT_CALL_ID *ospvCallId)
{
    unsigned char *value = OSPC_OSNULL;

    if (ospvCallId != OSPC_OSNULL) {
        value = ospvCallId->Value;
    }

    return value;
}

/*
 * OSPPSessionIdToElement() - create an XML element from a call id
 */
unsigned OSPPSessionIdToElement(    /* returns error code */
    OSPT_CALL_ID *ospvSessionId,    /* Session ID */
    OSPE_SESSION_ID ospvType,       /* Session ID type */
    OSPTBOOL ospvIsBase64,          /* indicates base64 or cdata */
    OSPT_XML_ELEM **ospvElem)       /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    } else if (ospvSessionId == OSPC_OSNULL) {
        errcode = OSPC_ERR_DATA_NOCALLID;
    } else {
        switch (ospvType) {
        case OSPC_SESSIONID_SOURCE:
            errcode = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_SESSIONID),
                OSPPCallIdGetSize(ospvSessionId), OSPPCallIdGetValue(ospvSessionId),
                OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_SOURCE), ospvIsBase64,
                ospvElem);
            break;
        case OSPC_SESSIONID_DESTINATION:
            errcode = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_SESSIONID),
                OSPPCallIdGetSize(ospvSessionId), OSPPCallIdGetValue(ospvSessionId),
                OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_DESTINATION), ospvIsBase64,
                ospvElem);
            break;
        case OSPC_SESSIONID_CORRELATION:
            errcode = OSPPMsgBinToElement(OSPPMsgElemGetName(OSPC_MELEM_SESSIONID),
                OSPPCallIdGetSize(ospvSessionId), OSPPCallIdGetValue(ospvSessionId),
                OSPPMsgAttrGetName(OSPC_MATTR_TYPE), OSPPAltInfoTypeGetName(OSPC_ALTINFO_CORRELATION), ospvIsBase64,
                ospvElem);
            break;
        default:
            errcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
           break;
        }
    }

    return errcode;
}
