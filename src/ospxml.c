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
 * ospxml.cpp - XML functions
 */

#include "osp/osp.h"
#include "osp/osperrno.h"
#include "osp/ospxml.h"
#include "osp/ospmsg.h"
#include "osp/ospbfr.h"
#include "osp/ospxmldoc.h"
#include "osp/osptokeninfo.h"
#include "osp/ospauthreq.h"
#include "osp/ospauthrsp.h"
#include "osp/ospusagecnf.h"
#include "osp/ospusageind.h"
#include "osp/ospreauthreq.h"
#include "osp/ospreauthrsp.h"
#include "osp/ospcapind.h"

int OSPPXMLElementProcess(
    OSPT_XML_ELEM *ospvElem,        /* In - xmlelement to be processed */
    unsigned char **ospvMessage,    /* Out - xml doc as unsigned char */
    unsigned *ospvSizeOfMessage)    /* Out - size of xml doc */
{                               
    OSPTBFR *outbuffer = NULL;
    unsigned transferlen = 0;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == (OSPT_XML_ELEM *)NULL) {
        errorcode = OSPC_ERR_XML_INVALID_ARGS;
        OSPM_DBGERRORLOG(errorcode, "ospvElem == NULL");
    }

    /* 
     * now that we have everything stored into the message element, 
     * we can
     * convert it into an XML.
     */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* 
         * Possible Optimization - 
         * Instead of allocating buffer for the below mentioned size,
         * allocate for OSPC_XMLDOC_DECLLEN byte's.
         * This will cut down on some subsequent malloc calls.
         */

        if ((outbuffer = OSPPBfrNew(0)) != NULL) {
            errorcode = OSPPXMLDocCreate(ospvElem, &outbuffer);
        } else {
            errorcode = OSPC_ERR_XML_BUFR_NEW_FAILED;
            OSPM_DBGERRORLOG(errorcode, "bufrnew failed");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if ((transferlen = OSPPBfrSize(outbuffer)) > 0) {

            OSPM_MALLOC(*ospvMessage, unsigned char, transferlen + 1);
        } else {
            errorcode = OSPC_ERR_XML_BFR_SZ_FAIL;
            OSPM_DBGERRORLOG(errorcode, "bufrsize failed");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (*ospvMessage != (unsigned char *)NULL) {
            OSPM_MEMSET(*ospvMessage, 0, transferlen + 1);
            /* Read data from OSPTBFR to output message */
            if ((*ospvSizeOfMessage = OSPPBfrReadBlock(&outbuffer, *ospvMessage, transferlen)) != transferlen) {
                errorcode = OSPC_ERR_XML_BFR_READ_BLOCK_FAIL;
                OSPM_DBGERRORLOG(errorcode, "bufrsize failed");
            }
        } else {
            errorcode = OSPC_ERR_XML_MALLOC_FAILED;
            OSPM_DBGERRORLOG(errorcode, "malloc for ospvMessage failed");
        }
    }

    OSPPBfrDelete(&outbuffer);

    if (errorcode != OSPC_ERR_NO_ERROR) {
        /* get rid of any malloced space */
        if (*ospvMessage != (unsigned char *)NULL) {
            OSPM_FREE(*ospvMessage);
            *ospvMessage = NULL;
        }
    }

    return errorcode;
}

int OSPPXMLMessageCreate(
    OSPE_MESSAGE ospvDataType,      /* In - what data type (AREQ, ARESP...) */
    unsigned char **ospvMessage,    /* Out - actual xml message */
    unsigned *ospvSizeOfMessage,    /* Out - size of xml message */
    void *ospvInfo,                 /* In - structure holding data */
    OSPTTRANS *trans)               /* In - transaction handle */
{                              
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *xmlelem = NULL;

    /* verify input */
    if ((ospvDataType > OSPC_MSG_NUMBER) || (ospvDataType < OSPC_MSG_UNKNOWN) || (ospvInfo == NULL)) {
        errorcode = OSPC_ERR_XML_INVALID_ARGS;
        OSPM_DBGERRORLOG(errorcode, "invalid arg in osppxmlmessagecreate");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        switch (ospvDataType) {
        case OSPC_MSG_AREQ:
            errorcode = OSPPAuthReqToElement((OSPT_AUTH_REQ *)ospvInfo, &xmlelem, trans);
            break;
        case OSPC_MSG_UIND:
            errorcode = OSPPUsageIndToElement((OSPTLIST *)ospvInfo, &xmlelem, trans);
            break;
        case OSPC_MSG_REAREQ:
            errorcode = OSPPReauthReqToElement((OSPTREAUTHREQ *)ospvInfo, &xmlelem, trans);
            break;
        case OSPC_MSG_CAPIND:
            errorcode = OSPPCapIndToElement((OSPT_CAP_IND *)ospvInfo, &xmlelem);
            break;
#ifndef OSP_SDK
        case OSPC_MSG_REARESP:
            errorcode = OSPPReauthRspToElement((OSPTREAUTHRSP *)ospvInfo, &xmlelem);
            break;
        case OSPC_MSG_ARESP:
        case OSPC_MSG_AREZP:
            errorcode = OSPPAuthRspToElement((OSPT_AUTH_RSP *)ospvInfo, &xmlelem, ospvDataType);
            break;
        case OSPC_MSG_UCNF:
            errorcode = OSPPUsageCnfToElement((OSPTLIST *)ospvInfo, &xmlelem);
            break;
#endif
        default:
            errorcode = OSPC_ERR_XML_INVALID_TYPE;
            break;
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPXMLElementProcess(xmlelem, ospvMessage, ospvSizeOfMessage);
    }

    /* Delete Element */
    OSPPXMLElemDelete(&xmlelem);

    return errorcode;
}

int OSPPXMLGetDataType(
    OSPT_XML_ELEM *ospvXMLElem,     /* In - xml element */
    OSPE_MESSAGE *ospvDataType)     /* Out - datatype for this element */
{                               
    OSPT_XML_ELEM *parent = OSPC_OSNULL;
    char *name = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvXMLElem == OSPC_OSNULL) {
        errorcode = OSPC_ERR_XML_INVALID_ARGS;
        OSPM_DBGERRORLOG(errorcode, "ospvXMLElem is NULL");
    } else {
        if (OSPPMsgElemGetPart(OSPPXMLElemGetName(ospvXMLElem)) == OSPC_MELEM_MESSAGE) {
            parent = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvXMLElem);
        } else {
            parent = ospvXMLElem;
        }
    }

    /* make sure we have a parent */

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (parent != OSPC_OSNULL) {
            /* get first element which should hold component name */
            name = (char *)OSPPXMLElemGetName(parent);
        } else {
            errorcode = OSPC_ERR_XML_PARENT_NOT_FOUND;
            OSPM_DBGERRORLOG(errorcode, "parent OSPTXMLELEM is NULL");
        }
    }

    /* now figure out which one we have */
    if ((errorcode == OSPC_ERR_NO_ERROR) && (name != NULL)) {
        /*
         * OSP Stds document spelling Authori(z|s)ation
         * C. Bokath
         */

        if (OSPM_STRSTR(name, "AuthorisationRequest") != NULL) {
            *ospvDataType = OSPC_MSG_AREQ;
        } else if (OSPM_STRSTR(name, "AuthorizationRequest") != NULL) {
            *ospvDataType = OSPC_MSG_AREQ;
        } else if (OSPM_STRSTR(name, "AuthorisationResponse") != NULL) {
            *ospvDataType = OSPC_MSG_ARESP;
        } else if (OSPM_STRSTR(name, "AuthorizationResponse") != NULL) {
            *ospvDataType = OSPC_MSG_ARESP;
        } else if (OSPM_STRSTR(name, "AuthorizationIndication") != NULL) {
            *ospvDataType = OSPC_MSG_AIND;
        } else if (OSPM_STRSTR(name, "AuthorizationConfirmation") != NULL) {
            *ospvDataType = OSPC_MSG_ACNF;
        } else if (OSPM_STRSTR(name, "UsageIndication") != NULL) {
            *ospvDataType = OSPC_MSG_UIND;
        } else if (OSPM_STRSTR(name, "UsageConfirmation") != NULL) {
            *ospvDataType = OSPC_MSG_UCNF;
        } else if (OSPM_STRSTR(name, "TokenInfo") != NULL) {
            *ospvDataType = OSPC_MSG_TOKINFO;
        } else if (OSPM_STRSTR(name, "ReauthorizationRequest") != NULL) {
            *ospvDataType = OSPC_MSG_REAREQ;
        } else if (OSPM_STRSTR(name, "ReauthorizationResponse") != NULL) {
            *ospvDataType = OSPC_MSG_REARESP;
        } else if (OSPM_STRSTR(name, "CapabilitiesConfirmation") != NULL) {
            *ospvDataType = OSPC_MSG_CAPCNF;
        } else {
            errorcode = OSPC_ERR_XML_DATA_TYPE_NOT_FOUND;
            OSPM_DBGERRORLOG(errorcode, name);
        }
    }

    return errorcode;
}

int OSPPXMLMessageParse(
    unsigned char *ospvXMLMessage,  /* In - xml message */
    unsigned ospvSizeOfMessage,     /* In - size of message */
    void **ospvData,                /* Out - pointer to struct w/data from message */
    OSPE_MESSAGE *ospvDataType)     /* Out - what type struct void pointer is pointing to */
{                               
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *xmlelem = OSPC_OSNULL;
    OSPTBFR *xmlbufr = OSPC_OSNULL;
    unsigned numbyteswritten = 0;
    OSPT_XML_ELEM *elem1 = OSPC_OSNULL;
    OSPT_XML_ELEM *tempxmlelem = OSPC_OSNULL;

    /* check input */
    if ((ospvXMLMessage == (unsigned char *)NULL) ||
        (ospvSizeOfMessage == 0)) {
        errorcode = OSPC_ERR_XML_INVALID_ARGS;
        OSPM_DBGERRORLOG(errorcode, "invalid arg in osppxmlmessageparse");
    }

    /* get a new xml buffer */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        xmlbufr = OSPPBfrNew(ospvSizeOfMessage);

        /* add the message to it */
        if (xmlbufr != NULL) {
            numbyteswritten = OSPPBfrWriteBlock(&xmlbufr, ospvXMLMessage, ospvSizeOfMessage);
            if (numbyteswritten != ospvSizeOfMessage) {
                errorcode = OSPC_ERR_XML_BFR_WRITE_BLOCK_FAILED;
                OSPM_DBGERRORLOG(errorcode, "bufrWriteblock failed for xmlmessageparse");
            }
        } else {
            errorcode = OSPC_ERR_XML_BUFR_NEW_FAILED;
            OSPM_DBGERRORLOG(errorcode, "bufrNew failed for xmlmessageparse");
        }

        /* send it to the parser */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPXMLDocParse(&xmlbufr, &xmlelem);
        }

        /* Get data type */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPXMLGetDataType(xmlelem, ospvDataType);
        }

        /* Process the message depending on the data type */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPXMLMessageProcess(xmlelem, ospvData, *ospvDataType);
        }

        /* delete buffer */
        if (xmlbufr != OSPC_OSNULL) {
            OSPPBfrDelete(&xmlbufr);
            xmlbufr = OSPC_OSNULL;
        }

        if (xmlelem != OSPC_OSNULL) {
            if ((*ospvDataType == OSPC_MSG_UIND) || (*ospvDataType == OSPC_MSG_UCNF)) {
                if (OSPPMsgElemGetPart(OSPPXMLElemGetName(xmlelem)) ==
                    OSPC_MELEM_MESSAGE) {
                    tempxmlelem = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(xmlelem);
                    xmlelem->ospmXMLElemChild = OSPC_OSNULL;
                    OSPPXMLElemDelete(&xmlelem);
                    xmlelem = tempxmlelem;
                }

                while ((elem1 = (OSPT_XML_ELEM *)OSPPListRemove((OSPTLIST *)&xmlelem)) != OSPC_OSNULL) {
                    OSPPXMLElemDelete(&elem1);
                    elem1 = OSPC_OSNULL;
                }
                OSPPXMLElemDelete(&xmlelem);
                xmlelem = OSPC_OSNULL;
            } else {
                OSPPXMLElemDelete(&xmlelem);
                xmlelem = OSPC_OSNULL;
            }
        }
    }

    return errorcode;
}

int OSPPXMLMessageProcess(
    OSPT_XML_ELEM *ospvElem,    /* In - xml element for this datatype */
    void **ospvStruct,          /* Out- pointer to struct to be filled in */
    OSPE_MESSAGE ospvDataType)  /* In - datatype for this struct */
{                         
    int errorcode = OSPC_ERR_NO_ERROR;

    switch (ospvDataType) {
    case OSPC_MSG_ARESP:
        errorcode = OSPPAuthRspFromElement(ospvElem, (OSPT_AUTH_RSP **)ospvStruct);
        break;
    case OSPC_MSG_UCNF:
        errorcode = OSPPUsageCnfFromElement(ospvElem, (OSPTLIST *)ospvStruct);
        break;
    case OSPC_MSG_TOKINFO:
        errorcode = OSPPTokenInfoFromElement(ospvElem, (OSPTTOKENINFO **)ospvStruct);
        break;
    case OSPC_MSG_REARESP:
        errorcode = OSPPReauthRspFromElement(ospvElem, (OSPTREAUTHRSP **)ospvStruct);
        break;
    case OSPC_MSG_CAPCNF:
        errorcode = OSPPCapCnfFromElement(ospvElem, (OSPT_CAP_CNF **)ospvStruct);
        break;
#ifndef OSP_SDK
    case OSPC_MSG_AREQ:
        errorcode = OSPPAuthReqFromElement(ospvElem, (OSPT_AUTH_REQ **)ospvStruct);
        break;
    case OSPC_MSG_UIND:
        errorcode = OSPPUsageIndFromElement(ospvElem, (OSPTLIST *)ospvStruct);
        break;
    case OSPC_MSG_REAREQ:
        errorcode = OSPPReauthReqFromElement(ospvElem, (OSPTREAUTHREQ **)ospvStruct);
        break;
#endif
    default:
        errorcode = OSPC_ERR_DATA_INVALID_TYPE;
    }

    return errorcode;
}
