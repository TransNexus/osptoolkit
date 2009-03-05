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
 * osppkcs8.c - PKCS #8 Private Key Information Standard
 */

#include "osp/osp.h"
#include "osp/ospdatatypes.h"
#include "osp/ospasn1.h"
#include "osp/osppkcs8.h"
#include "osp/ospcrypto.h"

/* FUNCTION PROTOTYPES */

/*
 * Member functions
 */
/*
 * Non-Member functions
 */

/* This routine assumes the private key passed by the caller is an
RSAEncryption PKCS1 Private Key. It is converted to a OSPPASN1OBJECT and
wrapped to become a PKCS8 PrivateKeyInfo Encoded object */
int OSPPPKCS8KeyInfoCreate(
    OSPTASN1OBJECT **ospvPrivateKeyInfo, 
    OSPTPRIVATEKEY *ospvPrivateKey)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;
    OSPTASN1OBJECT *privateKeyInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1OBJECT *tmpObject = OSPC_OSNULL;

    unsigned char *rsaPrivateKey = OSPC_OSNULL;
    unsigned rsaPrivateKeyLength = 0;

    int i = 0;

    if (ospvPrivateKey == OSPC_OSNULL) {
        errorcode = OSPC_ERR_PKCS8_INVALID_PRIVATE_KEY_POINTER;
        OSPM_DBGERRORLOG(errorcode, "Private Key is NULL");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectNew(&privateKeyInfo, OSPEDRID_PRIVATEKEYINFO);
    }

    /* The elements are a contentType OID, and a content element.  */
    /* Add the content type Element to the result list */
    for (i = 0; errorcode == OSPC_ERR_NO_ERROR; i++) {
        switch (i) {
        case 0:                /* Add VERSION Element */
            dataRefId = OSPEDRID_PVTKEYINF_VERSION;
            errorcode = OSPPASN1SmallIntegerEncode(&newObject, OSPC_ASN1_PRIVATEKEYINFO_VERSION, dataRefId);
            break;

        case 1:                /* Add PrivateKey Algorithm */
            dataRefId = OSPEDRID_PVTKEYINF_ALGORITHM;
            errorcode = OSPPASN1AlgorithmIdEncode(&tmpObject, OSPEID_RSAENCRYPTION, dataRefId);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ObjectCopy(&newObject, tmpObject);
            }

            OSPM_FREE(newObject->ElementInfo->Element);    /* !!! PS */
            OSPM_FREE(newObject->ElementInfo);    /* !!! PS */
            OSPPASN1ObjectDelete(&tmpObject);
            break;

        case 2:                /* Add PrivateKey OctetString */
            dataRefId = OSPEDRID_PVTKEYINF_PRIVATEKEY;

            rsaPrivateKey = ospvPrivateKey->PrivateKeyData;
            rsaPrivateKeyLength = ospvPrivateKey->PrivateKeyLength;

            errorcode = OSPPASN1OctetStringEncode(&newObject, rsaPrivateKey, rsaPrivateKeyLength, dataRefId);
            break;

        case 3:                /* Add PrivateKey Attributes */
            dataRefId = OSPEDRID_PVTKEYINF_ATTRIBUTES;
            newObject = OSPC_OSNULL;    /* None required for rsaPrivateKey */
            break;

        case 4:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

        default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, "Unknown case encountered encoding PKCS7 SignerInfo");
            break;
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL) {
                errorcode = OSPPASN1ObjectAddChild(privateKeyInfo, newObject, dataRefId);
                OSPM_FREE(newObject);    /* !!! PS */
                newObject = OSPC_OSNULL;
            }
        }
    }
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE) {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Complete the encoding for this object. Update results, elements, 
           etc. */
        errorcode = OSPPASN1ObjectDeparse(privateKeyInfo, OSPEPTID_PRIVATEKEYINFO, OSPEDRID_PRIVATEKEYINFO);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvPrivateKeyInfo = privateKeyInfo;
    } else {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&privateKeyInfo);
    }

    return errorcode;
}

int OSPPPKCS8KeyInfoTestContext(
    OSPTASN1OBJECT *ospvPrivateKeyInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvPrivateKeyInfo == OSPC_OSNULL) {
        errorcode = OSPC_ERR_PKCS8_INVALID_CONTEXT;
        OSPM_DBGERRORLOG(errorcode, "PKCS#1 Private Key context is null pointer");
    }

    return errorcode;
}

void OSPPPKCS8KeyInfoDelete(
    OSPTASN1OBJECT **ospvPrivateKeyInfo)
{
    OSPPASN1ObjectDelete(ospvPrivateKeyInfo);
}

int OSPPKCS8KeyInfoGetEncodedKeyInfo(
    OSPTASN1OBJECT *ospvPrivateKeyInfo, 
    unsigned char **ospvBERPrivateKeyInfo, 
    unsigned *ospvBERPrivateKeyInfoLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    errorcode = OSPPPKCS8KeyInfoTestContext(ospvPrivateKeyInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPPKCS8KeyInfoGetPrivateKeyElement(ospvPrivateKeyInfo, OSPEDRID_PRIVATEKEYINFO, &eInfo);

    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementGetElementData(eInfo, ospvBERPrivateKeyInfo, ospvBERPrivateKeyInfoLength);
    }

    return errorcode;
}

int OSPPPKCS8KeyInfoGetPrivateKey(
    OSPTASN1OBJECT *ospvPrivateKeyInfo, 
    unsigned char **ospvPrivateKey, 
    unsigned *ospvPrivateKeyLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    errorcode = OSPPPKCS8KeyInfoTestContext(ospvPrivateKeyInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPPKCS8KeyInfoGetPrivateKeyElement(ospvPrivateKeyInfo, OSPEDRID_PVTKEYINF_PRIVATEKEY, &eInfo);

    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementGetContentData(eInfo, ospvPrivateKey, ospvPrivateKeyLength);
    }

    return errorcode;
}

int OSPPPKCS8KeyInfoGetPrivateKeyElement(
    OSPTASN1OBJECT *ospvPrivateKeyInfo, 
    OSPEASN1DATAREFID ospvDataRefId, 
    OSPTASN1ELEMENTINFO **ospvElementInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *foundElement = OSPC_OSNULL;

    errorcode = OSPPPKCS8KeyInfoTestContext(ospvPrivateKeyInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementGet(ospvDataRefId, ospvPrivateKeyInfo->ParseResults, &foundElement);
    }

    *ospvElementInfo = foundElement;

    return errorcode;
}
