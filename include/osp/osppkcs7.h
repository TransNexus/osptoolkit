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
 * osppkcs7.h - Constants and prototypes for PKCS#7 cryptographic messages
 */

#ifndef _OSP_PKCS7_H
#define _OSP_PKCS7_H

#include "osp.h"
#include "ospasn1.h"
#include "ospx500.h"
#include "ospx509.h"
#include "osppkcs1.h"

/*-------------------------------------------*/
/* SIGNED DATA typedef                                           */
/*-------------------------------------------*/ 

#define OSPC_SIGNERINFO_MAX_ATTRIBUTES  10
#define OSPC_PKCS7_SIGNERINFO_VERSION   1
#define OSPC_PKCS7_SIGNEDDATA_VERSION   1

#ifdef __cplusplus
extern "C" 
{
#endif

    /*
     * Member functions
     */

    int 
    OSPPPKCS7SignerInfosCreate(
    OSPTASN1OBJECT      **ospvSignerInfos,
    OSPTASN1OBJECT      *ospvSignerInfo);

    int
    OSPPPKCS7SignerInfoCreate(
    OSPTASN1OBJECT      **ospvSignerInfo,
    OSPTASN1OBJECT      *ospvDataContent,
    OSPTASN1OBJECT      *ospvDigestAlgorithm,
    OSPTASN1OBJECT      *ospvSignerCertInfo,
    OSPTASN1OBJECT      *ospvSignerPrivateKey);

    int
    OSPPPKCS7CertificatesCreate(
    OSPTASN1OBJECT      **ospvCertificates,
    OSPTASN1OBJECT      * ospvCertificate);

    int
    OSPPPKCS7DigestAlgorithmsCreate(
    OSPTASN1OBJECT      **ospvDigestAlgorithms,
    OSPTASN1OBJECT      *ospvDigestAlgorithm);

    int
    OSPPPKCS7SignedDataCreate(
    OSPTASN1OBJECT  **ospvSignedData,
    OSPTASN1OBJECT  *ospvContentInfo,   
    OSPTASN1OBJECT  *ospvSignerCertificates, 
    OSPTASN1OBJECT  *ospvSignerInfos,       
    OSPTASN1OBJECT  *ospvDigestAlgorithms);

    int
    OSPPPKCS7ContentInfoCreate( 
    OSPTASN1OBJECT **ospvContentInfo,
    OSPTASN1OBJECT *ospvContent,
    OSPEASN1ID ospvContentTypeId, 
    int ospvSignatureOnly);


    int
    OSPPPKCS7SignatureCreate(
    unsigned char   *ospvContent,           /* In - data to signed */
    unsigned        ospvContentLength,      /* In - length of data to sign */
    OSPTASN1OBJECT  *ospvDigestAlgorithm,   /* In - ptr to digest Algorithm */
    OSPTASN1OBJECT  *ospvSignerCertInfo,    /* In - ptr to signer cert info */
    OSPTASN1OBJECT  *ospvSignerPrivateKeyInfo,
                                            /* In - ptr to private key struct */
    int             ospvSignatureOnly,      /* In - TRUE - no content in sig
                                            FALSE- content included in sig */
    unsigned char   **ospvSignature,        /* Out - ptr to ptr to sig store */
    unsigned        *ospvSignatureLength);  /* Out - length of signature */

    int 
    OSPPPKCS7SignatureParse(
    OSPTASN1OBJECT **ospvSignatureObject,
    unsigned char *ospvSignature,
    unsigned int  ospvSignatureLength);

    int
    OSPPPKCS7SignatureGetContent(
    OSPTASN1OBJECT *ospvSignatureObject,
    unsigned char **ospvContent, 
    unsigned int  *ospvContentLength,
    OSPTASN1ELEMENTINFO **);

    int 
    OSPPPKCS7SignatureVerify(
    OSPTASN1OBJECT *ospvSignatureObject,
    unsigned char  *ospvContent, 
    unsigned int   ospvContentLength,
    OSPTASN1OBJECT **ospvAuthorityCertificates,
    unsigned int   ospvNumberOfAuthorityCertificates);

    int
    OSPPPKCS7DigestInfoCreate(
    OSPTASN1OBJECT  **ospvDigestInfo,
    OSPTASN1OBJECT  *ospvDigestAlgorithm,
        unsigned char *ospvContent, 
        unsigned int ospvContentLength);


    /*
     * Non-Member functions
     */

#ifdef __cplusplus
}
#endif

#endif
