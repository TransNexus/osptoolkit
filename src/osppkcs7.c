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
 * osppkcs7.c - PKCS #7 Cryptographic message object processing functions.
 */

#include "osp.h"
#include "ospasn1.h"
#include "osppkcs1.h"
#include "ospx509.h"
#include "ospx500.h"
#include "osppkcs1.h"
#include "osppkcs7.h"
#include "ospcrypto.h"
#include "osptnlog.h"
#define DUMPOBJECTDATA(a, b)    { unsigned char *_data; unsigned int _dataLength; OSPTASN1ELEMENTINFO *_eInfo; OSPPASN1ObjectGetElementInfo(a, &_eInfo); OSPPASN1ElementGetElementData(_eInfo, &_data, &_dataLength); OSPTNLOGDUMP(_data, _dataLength, b); }


#define OSPC_ERR_PKCS7                              (25500)
#define OSPC_ERR_PKCS7_MALLOC_FAILED    (OSPC_ERR_PKCS7 + 1000)


/* FUNCTION PROTOTYPES */

/* ---------------------------------------------------------*/
/* Member functions                                         */
/* ---------------------------------------------------------*/

int
OSPPPKCS7SignatureCreate(
    unsigned char   *ospvContent,           /* In - data to signed */
    unsigned        ospvContentLength,      /* In - length of data to sign */
    OSPTASN1OBJECT  *ospvDigestAlgorithm,   /* In - ptr to digest Algorithm */
    OSPTASN1OBJECT  *ospvSignerCertInfo,    /* In - ptr to signer cert info */
    OSPTASN1OBJECT  *ospvSignerPrivateKey,  /* In - ptr to private key struct */
    int             ospvSignatureOnly,      /* In - TRUE - no content in sig
                                            FALSE- content included in sig */
    unsigned char   **ospvSignature,        /* Out - ptr to ptr to sig store */
    unsigned        *ospvSignatureLength)   /* Out - length of signature */
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1OBJECT *signedData = OSPC_OSNULL;
    OSPTASN1OBJECT *contentInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *dataContent = OSPC_OSNULL;
    OSPTASN1OBJECT *signerInfos = OSPC_OSNULL;
    OSPTASN1OBJECT *signerInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *contentObject = OSPC_OSNULL;
    OSPTASN1OBJECT *certificates = OSPC_OSNULL;
    OSPTASN1OBJECT *digestAlgorithms = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    /* Generate an ASN1 Encoded PKCS7 Signed Data signature from the
        parameters given.

        The signed data message is a Content info with a content type of 
        SIGNED DATA, and content, containing a Signed Data message which in
        turn contains a ContentInfo of type DATA.
    */
    /* Encode the content as and OctetString. */
    errorcode = OSPPASN1OctetStringEncode(&dataContent, ospvContent, 
        ospvContentLength, OSPEDRID_CNTINF_CONTENT);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Create the DATA content info structure for the content being
        signed.  Signature Only flag determines if the content is included
        or if an empty string is encoded in the ContentInfo. */

        errorcode = OSPPPKCS7ContentInfoCreate(&contentObject, dataContent, 
            OSPEID_DATA, ospvSignatureOnly);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Generate the Set of Certificates */
        errorcode = OSPPPKCS7CertificatesCreate(&certificates,
            ospvSignerCertInfo);
    }   

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Generate the set of DigestAlgorithms */
        errorcode = OSPPPKCS7DigestAlgorithmsCreate(&digestAlgorithms,
            ospvDigestAlgorithm);
    }   


    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Generate the Signer Info structure.  Generate a signature based
        on the content's octet string (content octets only, no tag or length */
        errorcode = OSPPPKCS7SignerInfoCreate(&signerInfo, dataContent,
            ospvDigestAlgorithm,
            ospvSignerCertInfo,
            ospvSignerPrivateKey);
    }   

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Generate the Set of Signer Info structures. */ 
        errorcode = OSPPPKCS7SignerInfosCreate(&signerInfos,
            signerInfo);
    }   

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* First, create the signed data object using the function parameters.*/
        errorcode = OSPPPKCS7SignedDataCreate(&signedData, contentObject,
            digestAlgorithms, certificates,
            signerInfos);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Next, create a ContentInfo with type SignedData. */
        errorcode = OSPPPKCS7ContentInfoCreate(&contentInfo,signedData,
            OSPEID_SIGNEDDATA, 0 );
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPASN1ObjectGetElementByDataRef(contentInfo, &eInfo, 
            OSPEDRID_CONTENTINFO);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPASN1ElementCopyElementData(eInfo,
                ospvSignature, ospvSignatureLength);
        }
    }

    OSPPASN1ObjectDelete(&certificates);
    OSPPASN1ObjectDelete(&digestAlgorithms);
    OSPPASN1ObjectDelete(&signerInfo);
    OSPPASN1ObjectDelete(&dataContent);
    OSPPASN1ObjectDelete(&signerInfos);
    OSPPASN1ObjectDelete(&signedData);

    OSPPASN1ObjectDelete(&contentInfo);
    OSPPASN1ElementDelete(&eInfo, 0);
    OSPPASN1ObjectDelete(&contentObject);

    return errorcode;
}


int 
OSPPPKCS7DigestInfoCreate(
    OSPTASN1OBJECT  **ospvDigestInfo,
    OSPTASN1OBJECT  *ospvDigestAlgorithm,
                                    unsigned char   *ospvContent,
    unsigned int    ospvContentLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1OBJECT *digestInfo = OSPC_OSNULL;
    OSPEASN1DATAREFID  dataRefId = OSPEDRID_NOTDEFINED;

    int i = 0;


    errorcode = OSPPASN1ObjectNew(&digestInfo, OSPEDRID_DIGESTINFO);

    for (i = 0 ;errorcode == OSPC_ERR_NO_ERROR ; i++)
    {
        switch(i)
        {
            case 0: /* Add Digest Algorithm */
            dataRefId = OSPEDRID_DIGINF_DIGESTALGORITHM;
            errorcode = OSPPASN1ObjectCopy(&newObject,ospvDigestAlgorithm);
            if(newObject)   /* !!! PS */
            {
                OSPM_FREE(newObject->ElementInfo->Element);
                OSPM_FREE(newObject->ElementInfo);
            }
            break;

            case 1: /* Add Digest */
            dataRefId = OSPEDRID_DIGINF_DIGEST;
            if(newObject)   /* !!! PS */
            {
                OSPM_FREE(newObject->ElementInfo->Element);
                OSPM_FREE(newObject->ElementInfo);
            }
            errorcode = OSPPCryptoDigest( &newObject, ospvDigestAlgorithm,
                ospvContent, ospvContentLength);

            break;

            case 2:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

            default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unknown case encountered encoding PKCS7 DigestInfoCreate");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL)
            {
                errorcode = OSPPASN1ObjectAddChild( digestInfo, newObject,
                    dataRefId );
                OSPM_FREE(newObject);                                                       
                newObject = OSPC_OSNULL;
            }
        }
    }
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE)
    {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Complete the encoding for this object. Update results, elements, 
            etc. */
        errorcode = OSPPASN1ObjectDeparse(digestInfo, OSPEPTID_DIGESTINFO,
            OSPEDRID_DIGESTINFO);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        *ospvDigestInfo = digestInfo;
    }
    else
    {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&digestInfo);
    }

    return errorcode;

}


int 
OSPPPKCS7SignerInfoCreate(
OSPTASN1OBJECT      **ospvSignerInfo,
OSPTASN1OBJECT      *ospvDataContent,
OSPTASN1OBJECT      *ospvDigestAlgorithm,
OSPTASN1OBJECT      *ospvSignerCertInfo,
OSPTASN1OBJECT      *ospvSignerPrivateKey)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;
    OSPTASN1OBJECT  *signerInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1OBJECT *digestEncryptionAlgorithm = OSPC_OSNULL;
    OSPTASN1OBJECT *digestInfo = OSPC_OSNULL;

    unsigned char *contentData = OSPC_OSNULL;
    unsigned int  contentDataLength = 0;

    int i = 0;

    /*The Signature is a ContentInfo structure with a datatype of signedData
        */

    errorcode = OSPPASN1ObjectNew(&signerInfo, OSPEDRID_SIGNERINFO);

    /* The elements are a contentType OID, and a content element.  */
    /* Add the content type Element to the result list */
    for (i = 0 ;errorcode == OSPC_ERR_NO_ERROR ; i++)
    {
        switch(i)
        {
            case 0: /* Add VERSION Element */
            dataRefId = OSPEDRID_SGNINF_VERSION;
            errorcode = OSPPASN1SmallIntegerEncode(&newObject, 
                OSPC_ASN1_SIGNERINFO_VERSION,
                dataRefId);
            break;

            case 1: /* Add Issuer */
            dataRefId = OSPEDRID_SGNINF_ISSUER;
            errorcode = OSPPASN1ObjectCopyElementObject(
                &newObject,
                ospvSignerCertInfo,
                OSPEDRID_CERT_ISSUER);
            if(newObject != OSPC_OSNULL)
            {
                if(newObject->ElementInfo != OSPC_OSNULL)
                {
                    OSPPASN1ElementDelete(&(newObject->ElementInfo) , 0);
                }
            }
            break;

            case 2: /* Add SerialNumber */
            dataRefId = OSPEDRID_SGNINF_SERIALNUMBER;
            errorcode = OSPPASN1ObjectCopyElementObject(&newObject, 
                ospvSignerCertInfo, 
                OSPEDRID_CERT_SERIALNUMBER);
            if(newObject != OSPC_OSNULL)
            {
                if(newObject->ElementInfo != OSPC_OSNULL)
                {
                    OSPPASN1ElementDelete(&(newObject->ElementInfo) , 0);
                }
            }
            break;

            case 3: /* Add DigestAlgorithm */
            dataRefId = OSPEDRID_SGNINF_DIGESTALGORITHM;
            errorcode = OSPPASN1ObjectCopy(&newObject, 
                ospvDigestAlgorithm);
            if(newObject != OSPC_OSNULL)
            {
                if(newObject->ElementInfo != OSPC_OSNULL)
                {
                    OSPPASN1ElementDelete(&(newObject->ElementInfo) , 0);
                }
            }
            break;

            case 4: /* Add AuthenticatedAttributes */
            dataRefId = OSPEDRID_SGNINF_AUTHATTRIBUTES;
            break;

            case 5: /* Add DigestEncryptionAlgorithm */
            dataRefId = OSPEDRID_SGNINF_DIGENCRYPTALG;
            errorcode = OSPPASN1ObjectCopyElementObject(&newObject, 
                ospvSignerCertInfo, 
                OSPEDRID_CERT_PUBLICKEYALG);
            if(newObject != OSPC_OSNULL)
            {
                if(newObject->ElementInfo != OSPC_OSNULL)
                {
                    OSPPASN1ElementDelete(&(newObject->ElementInfo) , 0);
                }
            }
            break;

            case 6: /* Add EncryptedDigest */
            dataRefId = OSPEDRID_SGNINF_ENCRYPTEDDIGEST;
            errorcode = OSPPASN1ObjectGetElementInfo(ospvDataContent,
                &eInfo);


            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPASN1ElementGetContentData(eInfo,
                    &contentData, &contentDataLength);
            }


            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPPKCS7DigestInfoCreate(&digestInfo,
                    ospvDigestAlgorithm,
                    contentData, contentDataLength);
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPASN1ObjectCopyElementObject(
                    &digestEncryptionAlgorithm, 
                    ospvSignerCertInfo, 
                    OSPEDRID_CERT_PUBLICKEYALG);
            }
/*
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                unsigned char *pubkeybuf = OSPC_OSNULL;
                unsigned int pubkeybuflen = 0;
                OSPTASN1ELEMENTINFO *tmpSignerSubjPubKeyInfo = OSPC_OSNULL;

                errorcode = OSPPASN1ObjectGetElementByDataRef(
                    ospvSignerCertInfo, &tmpSignerSubjPubKeyInfo,
                    OSPEDRID_CERT_SUBJPUBKEYINFO);

                errorcode = OSPPASN1ElementGetElementData(
                    tmpSignerSubjPubKeyInfo, &pubkeybuf, &pubkeybuflen);

                OSPTNLOGDUMP(pubkeybuf, pubkeybuflen,"signer PUB KEY INFO");
            }
*/
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPCryptoEncrypt(&newObject,
                    digestEncryptionAlgorithm,
                    digestInfo,
                    ospvSignerPrivateKey);
            }
            if(digestInfo != OSPC_OSNULL)
            {
                OSPPASN1ObjectDelete(&digestInfo);
            }

            if(digestEncryptionAlgorithm != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(digestEncryptionAlgorithm->ParseResults->ElementInfo), 0);
                OSPPASN1ObjectDelete(&digestEncryptionAlgorithm);
            }
            break;

            case 7: /* Add UnAuthenticatedAttributes */
            dataRefId = OSPEDRID_SGNINF_UNAUTHATTRIBUTES;
            newObject = OSPC_OSNULL;        /* Add nothing */
            break;

            case 8:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

            default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unknown case encountered encoding PKCS7 SignerInfo");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL)
            {
                errorcode = OSPPASN1ObjectAddChild(signerInfo, newObject,
                    dataRefId);
                if(newObject != OSPC_OSNULL)
                {
                    OSPM_FREE(newObject);
                }
            }
        }
    }
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE)
    {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Complete the encoding for this object. Update results, elements, 
            etc. */
        errorcode = OSPPASN1ObjectDeparse(signerInfo, 
            OSPEPTID_SIGNERINFO,
            OSPEDRID_SIGNERINFO);
    }


    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvSignerInfo = signerInfo;
    }
    else
    {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&signerInfo);
    }

    if(digestInfo != OSPC_OSNULL)
    {
        OSPM_FREE(digestInfo);
    }

    return errorcode;
}

int
OSPPPKCS7DigestAlgorithmsCreate(
OSPTASN1OBJECT      **ospvDigestAlgorithms,
OSPTASN1OBJECT      *ospvDigestAlgorithm)
{
    int errorcode=OSPC_ERR_NO_ERROR;

    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;
    OSPTASN1OBJECT  *digestAlgorithms = OSPC_OSNULL;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;

    int i = 0;

    errorcode = OSPPASN1ObjectNew(&digestAlgorithms, 
        OSPEDRID_SGNDAT_DIGESTALGORITHMS);

    /* The elements are a contentType OID, and a content element.  */
    /* Add the content type Element to the result list */
    for (i = 0 ;errorcode == OSPC_ERR_NO_ERROR ; i++)
    {
        switch(i)
        {
            case 0: /* Add DigestAlgorithm */
            dataRefId = OSPEDRID_SGNDAT_DIGESTALGORITHM;
            errorcode = OSPPASN1ObjectCopy(&newObject, ospvDigestAlgorithm);
            if(newObject != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);
            }
            break;

            case 1:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

            default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unknown case encountered encoding PKCS7 DigestAlgorithms");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL)
            {
                errorcode = OSPPASN1ObjectAddChild(digestAlgorithms, newObject,
                    dataRefId);
                OSPM_FREE(newObject);
            }
        }
    }
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE)
    {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Complete the encoding for this object. Update results, elements, 
            etc. */
        errorcode = OSPPASN1ObjectDeparse(digestAlgorithms, 
            OSPEPTID_DIGESTALGORITHMS,
            OSPEDRID_SGNDAT_DIGESTALGORITHMS);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvDigestAlgorithms = digestAlgorithms;
    }
    else
    {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&digestAlgorithms);
    }

    return errorcode;
}


int
OSPPPKCS7CertificatesCreate(
    OSPTASN1OBJECT      **ospvCertificates,
    OSPTASN1OBJECT      *ospvCertificate)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;
    OSPTASN1OBJECT  *certificates = OSPC_OSNULL;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;

    int i = 0;

    errorcode = OSPPASN1ObjectNew(&certificates, 
        OSPEDRID_SGNDAT_CERTIFICATES);

    /* The elements are a contentType OID, and a content element.  */
    /* Add the content type Element to the result list */
    for (i = 0 ;errorcode == OSPC_ERR_NO_ERROR ; i++)
    {
        switch(i)
        {
            case 0: /* Add Certificate */
            dataRefId = OSPEDRID_SGNDAT_CERTIFICATE;
            errorcode = OSPPASN1ObjectCopy(&newObject, ospvCertificate);
            if(newObject->ElementInfo->Element != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);
            }
            break;

            case 1:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

            default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unknown case encountered encoding PKCS7 Certificates");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL)
            {
                errorcode = OSPPASN1ObjectAddChild(certificates, newObject,
                    dataRefId);
                OSPM_FREE(newObject);
            }
        }
    }
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE)
    {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Complete the encoding for this object. Update results, elements, 
            etc. */
        errorcode = OSPPASN1ObjectDeparse(certificates, 
            OSPEPTID_CERTIFICATES,
            OSPEDRID_SGNDAT_CERTIFICATES);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvCertificates = certificates;
    }
    else
    {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&certificates);
    }

    return errorcode;
}


int
OSPPPKCS7SignerInfosCreate(
    OSPTASN1OBJECT      **ospvSignerInfos,
    OSPTASN1OBJECT      *ospvSignerInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;
    OSPTASN1OBJECT  *signerInfos = OSPC_OSNULL;
    OSPTASN1OBJECT *newObject = OSPC_OSNULL;

    int i = 0;

    errorcode = OSPPASN1ObjectNew(&signerInfos, OSPEDRID_SGNDAT_SIGNERINFOS);

    /* The elements are a contentType OID, and a content element.  */
    /* Add the content type Element to the result list */
    for (i = 0 ;errorcode == OSPC_ERR_NO_ERROR ; i++)
    {
        switch(i)
        {
            case 0: /* Add SignerInfo */
            dataRefId = OSPEDRID_SGNDAT_SIGNERINFO;
            errorcode = OSPPASN1ObjectCopy(&newObject, ospvSignerInfo);
            if(newObject->ElementInfo->Element != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);
            }
            break;

            case 1:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

            default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unknown case encountered encoding PKCS7 SignerInfos");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL)
            {
                errorcode = OSPPASN1ObjectAddChild(signerInfos, newObject,
                    dataRefId);
                if(newObject != OSPC_OSNULL)
                {
                    OSPM_FREE(newObject);
                }
            }
        }
    }
    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE)
    {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Complete the encoding for this object. Update results, elements, 
            etc. */
        errorcode = OSPPASN1ObjectDeparse(signerInfos, 
            OSPEPTID_SIGNERINFOS,
            OSPEDRID_SGNDAT_SIGNERINFOS);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvSignerInfos = signerInfos;
    }
    else
    {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&signerInfos);
    }

    return errorcode;
}


int
OSPPPKCS7SignedDataCreate(
    OSPTASN1OBJECT  **ospvSignedData,
    OSPTASN1OBJECT  *ospvContentInfo,   
    OSPTASN1OBJECT  *ospvDigestAlgorithms,
    OSPTASN1OBJECT  *ospvSignerCertificates, 
    OSPTASN1OBJECT  *ospvSignerInfos)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned int i = 0;

    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;

    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1OBJECT *signedData = OSPC_OSNULL;

    errorcode = OSPPASN1ObjectNew(&signedData, OSPEDRID_SIGNEDDATA);

    for (i = 0 ;errorcode == OSPC_ERR_NO_ERROR ; i++)
    {
        switch(i)
        {
            case 0:
            dataRefId = OSPEDRID_SGNDAT_VERSION;
            errorcode = OSPPASN1SmallIntegerEncode(&newObject, 
                OSPC_PKCS7_SIGNEDDATA_VERSION,
                dataRefId);
            break;

            case 1:
            dataRefId = OSPEDRID_SGNDAT_DIGESTALGORITHMS;
            errorcode = OSPPASN1ObjectCopy(&newObject, 
                ospvDigestAlgorithms);
            if(newObject != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);
            }
            break;

            case 2:
            dataRefId = OSPEDRID_SGNDAT_CONTENTINFO;
            errorcode = OSPPASN1ObjectCopy(&newObject, 
                ospvContentInfo);
            if(newObject != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);
            }
            break;

            case 3:
            dataRefId = OSPEDRID_SGNDAT_CERTIFICATES;
            errorcode = OSPPASN1ObjectCopy(&newObject, 
                ospvSignerCertificates);
            if(newObject != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);
            }
            break;

            case 4:
            dataRefId = OSPEDRID_SGNDAT_SIGNERINFOS;
            errorcode = OSPPASN1ObjectCopy(&newObject, 
                ospvSignerInfos);
            if(newObject != OSPC_OSNULL)
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);               
            }
            break;

            case 5:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

            default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unknown case encountered encoding PKCS7 SignedData");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL)
            {
                errorcode = OSPPASN1ObjectAddChild(signedData, newObject,
                    dataRefId);
                OSPM_FREE(newObject);
            }
        }
    }

    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE)
    {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPASN1ObjectDeparse(signedData, 
            OSPEPTID_SIGNEDDATA,
            OSPEDRID_SIGNEDDATA);
    }
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvSignedData = signedData;
    }

    else
    {
        /* Clean up from errors */
        OSPPASN1ObjectDelete(&signedData);
    }    
    
    if(newObject != OSPC_OSNULL)
    {
        OSPPASN1ObjectDelete(&newObject);
    }

    return errorcode;
}      



int
OSPPPKCS7ContentInfoCreate( 
    OSPTASN1OBJECT **ospvContentInfo,
    OSPTASN1OBJECT *ospvContent,
    OSPEASN1ID ospvContentTypeId, 
    int ospvSignatureOnly)
{
    int errorcode = 0;
    unsigned int           i     = 0;

    OSPEASN1DATAREFID dataRefId = OSPEDRID_NOTDEFINED;
    OSPEASN1PARSETABLEID tableId = OSPEPTID_NOTDEFINED;

    OSPTASN1OBJECT *newObject = OSPC_OSNULL;
    OSPTASN1OBJECT *contentInfo = OSPC_OSNULL;

    /*The Signature is a ContentInfo sturcture with a datatype of signedData
    */

    /* Create a parse result for each of the input parameters.  Add the
    result to the results list in the correct order.  It will be necessary 
    to call subroutines to build the sub element.  The parse results table will
    have DataReference-able elements attached.  Running the parse table
    through the rules to build the element info list and then traversing
    the element info list should build an ASN1 encode object.  */

    /* Signed Data structure will contain element info pointer (null), and
    a parse results table containing populated data elements with data
    reference values. */

    /* Content type is SignedData.  Content may or may not be contained in
    the signed data structure.  */

    /* Create a content info object for the signature */
    errorcode = OSPPASN1ObjectNew(&contentInfo, OSPEDRID_CONTENTINFO);

    /* The elements are a contentType OID, and a content element.  */
    /* Add the content type Element to the result list */
    for (i = 0 ;errorcode == OSPC_ERR_NO_ERROR ; i++)
    {
        switch(i)
        {
            case 0:
            dataRefId = OSPEDRID_CNTINF_CONTENTTYPE;
            errorcode = OSPPASN1ObjectIdentifierEncode(&newObject, 
                ospvContentTypeId,
                dataRefId);
            break;

            case 1:
            dataRefId = OSPEDRID_CNTINF_CONTENT;           
            if ((ospvSignatureOnly) && (newObject != OSPC_OSNULL))
            {
                OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);
            }
            else
            {
                errorcode = OSPPASN1ObjectCopy(&newObject,ospvContent);
                if(newObject != OSPC_OSNULL)
                {
                    OSPPASN1ElementDelete(&(newObject->ElementInfo), 0);            
                }
            }
            break;

            case 2:
            errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            break;

            default:
            errorcode = OSPC_ERR_PKCS7_ENCODING_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unknown case encountered encoding PKCS7 ContentInfo.");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Add new object to this object */
            if (newObject != OSPC_OSNULL)
            {
                errorcode = OSPPASN1ObjectAddChild(contentInfo, newObject,
                    dataRefId);
                OSPM_FREE(newObject);
            }
        }   
    }

    if (errorcode == OSPC_ERR_ASN1_PARSE_COMPLETE)
    {
        errorcode = OSPC_ERR_NO_ERROR;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        if (ospvContentTypeId == OSPEID_DATA)
        {
            tableId = OSPEPTID_CONTENTINFO_DATA; 
        }
        else if (ospvContentTypeId == OSPEID_SIGNEDDATA)
        {
            tableId = OSPEPTID_CONTENTINFO_SIGNEDDATA;
        }
        else
        {
            errorcode = OSPC_ERR_ASN1_PARSE_ERROR;
            OSPM_DBGERRORLOG(errorcode, 
                "Unsupported contentInfo type specified");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPASN1ObjectDeparse(contentInfo, tableId,
                OSPEDRID_CONTENTINFO);
        }
    }   

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvContentInfo = contentInfo;
    }
    else
    {
        OSPPASN1ObjectDelete(&contentInfo);
        /* Do the cleanup stuff here */ 
    }

    if(newObject != OSPC_OSNULL)
    {
        OSPPASN1ObjectDelete(&newObject);
    }

    return errorcode;
}


/* Caller must provide space for content.  ospvContentLength should be set
to the size of the space provided.  An error will occur if the content
contained in the signature is larger than the space provided.

If the signature does not contain the signed content, then the caller will
set content to the data that was signed and set content length to the
length of the data being signed.  

If the signature does contain the signed content, then the call should set
content to zeros and set contentlength to the size of the content buffer.
The content from the signature will be copied to the content buffer and the
length will be changed to the actual length of the content.  An error will
occur if the length of the content is larger than the length of the buffer.
*/

int
OSPPPKCS7SignatureParse(
    OSPTASN1OBJECT **ospvSignatureObject,
    unsigned char *ospvSignature,
    unsigned int  ospvSignatureLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *signatureObject = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;

    if ((ospvSignatureObject == OSPC_OSNULL) ||
        (ospvSignature == OSPC_OSNULL) ||
        (ospvSignatureLength == 0))
    {
        errorcode = OSPC_ERR_PKCS7_INVALID_POINTER;
        OSPM_DBGERRORLOG(errorcode, "Invalid input or output pointers");
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Decode the signature into element and build element list */
        errorcode = OSPPASN1ElementDecode(ospvSignature, &eInfo, 0);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Parse the element list to validate structure and get cross
        reference list elements */
        errorcode = OSPPASN1ElementParse( eInfo, 
            OSPEPTID_CONTENTINFO_SIGNEDDATA, 
            OSPC_OSNULL,
            &parseResults, 
            OSPC_ASN1_DATAREFID_CONTENTINFO);
    }


    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Create the signatureObject object that will contain the 
            decoded/parsed signature */
        errorcode = OSPPASN1ObjectCreate(&signatureObject, eInfo, parseResults);
    }



    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        *ospvSignatureObject = signatureObject;

    }

    return errorcode;
}   


int
OSPPPKCS7SignatureGetContent(
    OSPTASN1OBJECT *ospvSignatureObject,
    unsigned char **ospvContent, 
    unsigned int  *ospvContentLength,
    OSPTASN1ELEMENTINFO **el    )
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *contentElement = OSPC_OSNULL;
    unsigned char *content = OSPC_OSNULL;
    unsigned int  contentLength = 0;

    /* Extract content from signature object */
    errorcode = OSPPASN1ObjectGetElementByDataRef(
        ospvSignatureObject, &contentElement,
        OSPEDRID_SIG_SGNDAT_DATA);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Now we have the data element that should contain the data that
        was signed. */

        /* Get pointer to data and set length */
        errorcode = OSPPASN1ElementGetContentData(contentElement, 
            &content, &contentLength);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            *ospvContent = content;
            *ospvContentLength = contentLength;

        }
        if(OSPC_OSNULL!=el) /* !!! PS */
        {
            *el=contentElement;
        }
    }
    return errorcode;
}


int
OSPPPKCS7SignatureVerify(
    OSPTASN1OBJECT *ospvSignatureObject,
    unsigned char  *ospvContent, 
    unsigned int   ospvContentLength,
    OSPTASN1OBJECT *ospvAuthorityCertificates[],
    unsigned int   ospvNumberOfAuthorityCertificates)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *digestAlgorithmId = OSPC_OSNULL;
    OSPTASN1OBJECT *encryptionAlgorithm = OSPC_OSNULL;
    OSPTASN1OBJECT *encryptedDigest = OSPC_OSNULL;
    OSPTASN1OBJECT *subjPubKeyInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *signerCertificate = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    unsigned char *contentDigest = OSPC_OSNULL;
    unsigned int  contentDigestLength = 0;
    OSPTASN1OBJECT *decryptedDigestObject = OSPC_OSNULL;
    OSPTASN1OBJECT *digestInfo = OSPC_OSNULL;
    unsigned char *decryptedDigest = OSPC_OSNULL;
    unsigned int  decryptedDigestLength = 0;

    if ((ospvContent == OSPC_OSNULL) || (ospvContentLength == 0))
    {
        errorcode = OSPC_ERR_PKCS7_INVALID_POINTER;
        OSPM_DBGERRORLOG(errorcode, 
            "Content buffer pointer is null, or length is 0");
    }


    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Now we have the content that was signed and the length.
        Need to generate a digest of it for comparison against the
        signature */

        if (errorcode == OSPC_ERR_NO_ERROR)
        {

            /* Get the digest algorithm id from the signature object. */
            errorcode = OSPPASN1ObjectCopyElementObject( &digestAlgorithmId,
                ospvSignatureObject, 
                OSPEDRID_SIG_SGNDAT_SGNINF_DIGESTALGORITHM);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Create a local Digest Info object to compare to results of
            decryption. */
            errorcode = OSPPPKCS7DigestInfoCreate(&digestInfo, 
                digestAlgorithmId,
                ospvContent, ospvContentLength);

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPASN1ObjectGetElementInfo(digestInfo, &eInfo);
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                    errorcode = OSPPASN1ElementGetElementData(eInfo,
                        &contentDigest, &contentDigestLength);
                }
            }
        }

    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Ok, now we have a digest of the content, Decrypt the
        signature to get the digest that was encrypted and compare the 
        two digests */ 

        errorcode = OSPPASN1ObjectCopyElementObject(&encryptionAlgorithm,
            ospvSignatureObject, 
            OSPEDRID_SIG_SGNDAT_SGNINF_DIGENCRYPTALG);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get the signature next */
            errorcode = OSPPASN1ObjectCopyElementObject(&encryptedDigest,
                ospvSignatureObject, 
                OSPEDRID_SIG_SGNDAT_SGNINF_ENCRYPTEDDIGEST);

        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get the signer public key from the signers certificate,
                Only have ONE certificate in signatures, so it is NOT
                necessary to search for certificate, In the future this
                might be necessary if certificate chains and/or
                multiple signers are allowed.  This is where the code
                would be enhanced......BKL */

            errorcode = OSPPASN1ObjectCopyElementObject(&subjPubKeyInfo,
                ospvSignatureObject, 
                OSPEDRID_SIG_SGNDAT_CERTIFICATE_SUBJPUBKEYINFO);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Have the public key data */ 
            errorcode = OSPPCryptoDecrypt(&decryptedDigestObject, 
                encryptionAlgorithm, encryptedDigest,
                subjPubKeyInfo);
        }
    }


    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Now we have a decrypted digest in object form and the 
        digest generated from the content - compare them */

        errorcode = OSPPASN1ObjectGetElementInfo(decryptedDigestObject, &eInfo);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPASN1ElementGetContentData(eInfo, 
                &decryptedDigest, &decryptedDigestLength);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPC_ERR_PKCS7_INVALID_SIGNATURE;

            if (decryptedDigestLength == contentDigestLength)
            {
                if (OSPM_MEMCMP(decryptedDigest, 
                    contentDigest, contentDigestLength) == 0)
                {
                    errorcode = OSPC_ERR_NO_ERROR;
                }
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Signature is good.  Now test signer certificate to make sure it
        is valid and properly signed and authorized */

        /* Get the signer certificate */
        errorcode = OSPPASN1ObjectCopyElementObject(&signerCertificate,
            ospvSignatureObject, 
            OSPEDRID_SIG_SGNDAT_CERTIFICATE);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            int ix=0;

            /* Test valid signer certificate against CA certificates */
            errorcode = OSPPX509CertValidateCertificate(signerCertificate, 
                ospvAuthorityCertificates, 
                ospvNumberOfAuthorityCertificates,&ix);
        }
        if(OSPC_OSNULL!=signerCertificate)  /* !!! PS */
        {
            OSPPASN1ElementDelete(&(signerCertificate->ParseResults->ElementInfo),0);
            OSPPASN1ObjectDelete(&signerCertificate);
        }
    }
    if(OSPC_OSNULL!=subjPubKeyInfo) /* !!! PS */
    {
        OSPPASN1ElementDelete(&(subjPubKeyInfo->ParseResults->ElementInfo),0);
        OSPPASN1ObjectDelete(&subjPubKeyInfo);
    }
    if(OSPC_OSNULL!=digestAlgorithmId)  /* !!! PS */
    {
        OSPPASN1ElementDelete(&(digestAlgorithmId->ParseResults->ElementInfo),0);
        OSPPASN1ObjectDelete(&digestAlgorithmId);
    }
    if(OSPC_OSNULL!=encryptionAlgorithm)    /* !!! PS */
    {
        OSPPASN1ElementDelete(&(encryptionAlgorithm->ParseResults->ElementInfo),0);
        OSPPASN1ObjectDelete(&encryptionAlgorithm);
    }
    if(OSPC_OSNULL!=encryptedDigest)    /* !!! PS */
    {
        OSPPASN1ElementDelete(&(encryptedDigest->ParseResults->ElementInfo),0);
        OSPPASN1ObjectDelete(&encryptedDigest);
    }
    if(OSPC_OSNULL!=decryptedDigestObject)  /* !!! PS */
    {
        OSPPASN1ObjectDelete(&decryptedDigestObject);
    }
    if(OSPC_OSNULL!=digestInfo) /* !!! PS */
    {
        OSPPASN1ObjectDelete(&digestInfo);
    }
    return errorcode;
} /* OSPPPKCS7SignatureVerify */


/* EOF */
