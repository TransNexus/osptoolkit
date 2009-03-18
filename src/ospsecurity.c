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
 * ospsecurity.cpp - Functions for security object.
 */
#include "osp/osp.h"
#include "osp/ospdatatypes.h"
#include "osp/ospasn1.h"
#include "osp/osppkcs1.h"
#include "osp/osppkcs7.h"
#include "osp/osppkcs8.h"
#include "osp/ospx509.h"
#include "osp/ospsecurity.h"
#include "osp/osptnlog.h"

#include <openssl/pkcs7.h>
#include <openssl/bio.h>
#include <openssl/err.h>

/* PROTOTYPES FOR LOCAL FUNCTIONS */
int OSPPSecTestContext(OSPTSEC *ospvSecurity);
int OSPPSecLock(OSPTSEC *ospvSecurity, OSPE_LOCK_MODE ospvLockMode);
int OSPPSecUnlock(OSPTSEC *ospvSecurity);
int OSPPSecDeleteAuthorityCertificates(OSPTSEC *ospvSecurity);
int OSPPSecSetDigestAlgorithm(OSPTSEC *ospvSecurity, OSPTASN1OBJECT *ospvDigestAlgorithmId);
int OSPPSecGetLocalCertInfo(OSPTSEC *ospvSecurity, OSPTASN1OBJECT **ospvLocalCertInfo);
void OpenSSLErrorLog(int errorcode);

/**************************************/
/* IMPLEMENTATION OF MEMBER FUNCTIONS */
/**************************************/

/* Create and initialize new security object 
    Create the object and zero it out.
    Elements will be added separately by the caller.
    Set default digest algorithm and local validation.
*/
int OSPPSecNew(
    OSPTSEC **ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *digestAlgorithm;

    if ((ospvSecurity == OSPC_OSNULL) || (*ospvSecurity != OSPC_OSNULL)) {
        errorcode = OSPC_ERR_SEC_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "Invalid pointer to new security object");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*
         * create a new security object
         */

        OSPM_MALLOC(*ospvSecurity, OSPTSEC, sizeof(OSPTSEC));
        if (*ospvSecurity == OSPC_OSNULL) {
            errorcode = OSPC_ERR_SEC_NO_MEMORY;
            OSPM_DBGERRORLOG(errorcode, "ospvSec malloc failed");
        }
        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Initialize the security object */
            OSPM_MEMSET(*ospvSecurity, 0, sizeof(OSPTSEC));
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_MUTEX_INIT((*ospvSecurity)->SecurityMutex, NULL, errorcode);
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the Default Digest Algorithm */
        errorcode = OSPPASN1AlgorithmIdEncode(&digestAlgorithm, OSPEID_MD5, OSPEDRID_NOTDEFINED);

        /* Set the Digest Algorithm Id */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPSecSetDigestAlgorithm(*ospvSecurity, digestAlgorithm);
        }

        if (errorcode != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(errorcode, "OSPPSecSetDigestAlgorithm Failed");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecSetLocalValidation(*ospvSecurity, 1);
        if (errorcode != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(errorcode, "OSPPSecSetLocalValidation Failed");
        }
    }
#ifdef OSP_SDK
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecSSLSessionIdInitDB(*ospvSecurity);
        if (errorcode != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(errorcode, "OSPPSecSSLSessionIdInitDB Failed");
        }
    }
#endif

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPPSecDelete(ospvSecurity);
    }

    return errorcode;
}

int OSPPSecSetPrivateKey(
    OSPTSEC *ospvSecurity, 
    OSPTPRIVATEKEY *ospvPrivateKey)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *privateKeyInfo = OSPC_OSNULL;

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_WRITE);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get rid of existing private key */
        if (ospvSecurity->PrivateKeyInfo != OSPC_OSNULL) {
            OSPPPKCS8KeyInfoDelete(&(ospvSecurity->PrivateKeyInfo));
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Add new private key if one is provided */
        if (ospvPrivateKey != OSPC_OSNULL) {
            errorcode = OSPPPKCS8KeyInfoCreate(&privateKeyInfo, ospvPrivateKey);
        }
    }

    /* Update privatekey field in security with value of privateKeyInfo */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        ospvSecurity->PrivateKeyInfo = privateKeyInfo;
    } else {
        OSPPPKCS8KeyInfoDelete(&privateKeyInfo);
    }

    OSPPSecUnlock(ospvSecurity);

    return errorcode;
}


int OSPPSecCopyPrivateKey(
    OSPTSEC *ospvSecurity,              /*In - security structure */
    OSPTPRIVATEKEY *ospvPrivateKey)     /*In - allocated private key store */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *privateKeyData = OSPC_OSNULL;
    unsigned int privateKeyDataLength = 0;

    /* Check context and lock the security module for reading */
    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get pointers to private key data */
        errorcode = OSPPSecGetPrivateKeyData(ospvSecurity, &privateKeyData, &privateKeyDataLength);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Test data size */
        if ((ospvPrivateKey == OSPC_OSNULL) ||
            (ospvPrivateKey->PrivateKeyData == OSPC_OSNULL) || (privateKeyDataLength > ospvPrivateKey->PrivateKeyLength)) 
        {
            errorcode = OSPC_ERR_SEC_NOT_ENOUGH_SPACE_FOR_COPY;
            OSPM_DBGERRORLOG(errorcode, "Not enough space provided for key\n"
                             "Must allocate OSPTPRIVATEKEY, data element,\n" "and set data length to max size");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Copy the private key */
        OSPM_MEMCPY(ospvPrivateKey->PrivateKeyData, privateKeyData, privateKeyDataLength);
        ospvPrivateKey->PrivateKeyLength = privateKeyDataLength;
    }

    /* Unlock the security module */
    OSPPSecUnlock(ospvSecurity);

    return errorcode;
}

int OSPPSecGetLocalValidation(
    OSPTSEC *ospvSecurity, 
    unsigned *ospvLocalValidation)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvLocalValidation = ospvSecurity->LocalValidation;
    }

    OSPPSecUnlock(ospvSecurity);

    return errorcode;
}

int OSPPSecSetLocalCertificate(
    OSPTSEC *ospvSecurity, 
    unsigned char *ospvLocalCertificate, 
    unsigned long *ospvCustomerId, 
    unsigned long *ospvDeviceId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *newCertInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *oldCertInfo = OSPC_OSNULL;

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_WRITE);

    /* Get the existing local certificate */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecGetLocalCertInfo(ospvSecurity, &oldCertInfo);
    }

    /* If new certificate is NOT NULL, then create the local cert info */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvLocalCertificate != OSPC_OSNULL) {
            /* Create/initialize a new certInfo for new local certificate */
            errorcode = OSPPX509CertCreate(ospvLocalCertificate, &newCertInfo);
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPX509CertGetCustDeviceId(newCertInfo, ospvCustomerId, ospvDeviceId);
    }

    /* Store new cert info or OSPC_NULL in LocalCertInfo field */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        ospvSecurity->LocalCertInfo = newCertInfo;

        /* Clean up old certificate */
        if (oldCertInfo != OSPC_OSNULL) {
            errorcode = OSPPX509CertDelete(&oldCertInfo);
        }
    } else {
        /* Clean up the newly build certificate (if any) and leave things
           the way they were */
        OSPPX509CertDelete(&newCertInfo);
    }

    return errorcode;
}

int OSPPSecSetLocalValidation(
    OSPTSEC *ospvSecurity, 
    unsigned ospvLocalValidation)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_WRITE);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        ospvSecurity->LocalValidation = ospvLocalValidation;
    }

    OSPPSecUnlock(ospvSecurity);

    return errorcode;
}

/* Return the list of authority certificates stored in the security object.  
    Storage for ospvCertificateCount certificates, each less than or equal to
    ospvMaxCertificateSize bytes must be provided by the caller.
*/
int OSPPSecCopyAuthorityCertificates(
    OSPTSEC *ospvSecurity,                      /* In - security context */
    unsigned ospvMaxCertificateSize,            /* In - Max cert size */
    unsigned char *ospvAuthorityCertificates[], 
    unsigned *ospvCertificateCount)             /* In/Out - Max allowed/# returned */
{
    unsigned i;
    unsigned certCount = 0;
    unsigned certSize = 0;
    unsigned char *certificate = OSPC_OSNULL;
    OSPTASN1OBJECT *certInfo;
    OSPTASN1OBJECT **certInfoList;

    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        certCount = ospvSecurity->NumberOfAuthorityCertificates;

        if (certCount > *ospvCertificateCount) {
            errorcode = OSPC_ERR_SEC_TOO_MANY_CERTIFICATES;
            OSPM_DBGERRORLOG(errorcode, "To many certificates to return");
        }

        certInfoList = ospvSecurity->AuthorityCertInfo;

        for (i = 0; (errorcode == OSPC_ERR_NO_ERROR) && (i < certCount); i++) {
            certInfo = certInfoList[i];

            errorcode = OSPPX509CertGetCertificate(certInfo, &certificate, &certSize);
            if (certSize > ospvMaxCertificateSize) {
                errorcode = OSPC_ERR_SEC_CERTIFICATE_TOO_BIG;
                OSPM_DBGERRORLOG(errorcode, "Not enough space for certificate");
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPM_MEMCPY(ospvAuthorityCertificates[i], certificate, certSize);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvCertificateCount = certCount;
    } else {
        *ospvCertificateCount = 0;
    }

    return errorcode;
}

int OSPPSecDelete(
    OSPTSEC **ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecTestContext(*ospvSecurity);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Delete the authority certificates
         */
        errorcode = OSPPSecDeleteAuthorityCertificates(*ospvSecurity);

        /* Delete the local certificate
         */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            if ((*ospvSecurity)->LocalCertInfo != OSPC_OSNULL) {
                errorcode = OSPPX509CertDelete(&((*ospvSecurity)->LocalCertInfo));
            }
        }

        /* Delete the PrivateKey
         */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Get rid of existing private key */
            if ((*ospvSecurity)->PrivateKeyInfo != OSPC_OSNULL) {
                OSPPPKCS8KeyInfoDelete(&((*ospvSecurity)->PrivateKeyInfo));
            }
        }

        /* Delete the digest algorithm
         */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            if ((*ospvSecurity)->DigestAlgorithm != OSPC_OSNULL) {
                OSPPASN1ObjectDelete(&((*ospvSecurity)->DigestAlgorithm));
            }
        }
#ifdef OSP_SDK
        /* Delete the SSL session database
         */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPSecSSLSessionIdDeleteDB(*ospvSecurity);
        }
#endif

        /* Get rid of the security and session mutexen */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_MUTEX_DESTROY((*ospvSecurity)->SecurityMutex, errorcode);
        }

        if ((errorcode == OSPC_ERR_NO_ERROR) && ((*ospvSecurity)->SSLSessionHasMutex == OSPC_TRUE)) {
            OSPM_MUTEX_DESTROY((*ospvSecurity)->SSLSessionMutex, errorcode);
        }

        /* Delete the OSPTSEC storage
         */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_FREE(*ospvSecurity);
            *ospvSecurity = NULL;
        }
    }

    return errorcode;
}

int OSPPSecVerifyRootAuthorityCertificate(
    OSPTSEC *ospvSecurity, 
    void *ospvRootCACert, 
    int ospvRootCACertLen)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned certNum = 0;                   /* Number of certificates */
    OSPTASN1OBJECT **cert = OSPC_OSNULL;    /* Certificate info pointers */
    unsigned count = 0;                     /* Loop var. */
    unsigned char *certData = NULL;         /* Certificate itself */
    unsigned certSize = 0;                  /* Certificate size */
    int certMatch = OSPC_FALSE;             /* Do they match ? */

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);

    OSPTNLOGDUMP(ospvRootCACert, ospvRootCACertLen, "ROOT CA Cert");

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvSecurity == OSPC_OSNULL) {
            errorcode = OSPC_ERR_SEC_INVALID_CONTEXT;
        } else if (ospvRootCACert == OSPC_OSNULL) {
            errorcode = OSPC_ERR_SEC_NULL_CERTIFICATE;
        } else if (ospvRootCACertLen < 1) {
            errorcode = OSPC_ERR_SEC_ZERO_LENGTH_CERTIFICATE;
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get authority certificates and count */
        certNum = ospvSecurity->NumberOfAuthorityCertificates;

        if (certNum == 0) {
            errorcode = OSPC_ERR_SEC_NO_AUTHORITY_CERTIFICATES;
            OSPM_DBGERRORLOG(errorcode, "No authority certificates to test");
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Get pointer to array of authority certificates */
            cert = ospvSecurity->AuthorityCertInfo;

            if (cert == OSPC_OSNULL) {
                errorcode = OSPC_ERR_SEC_NULL_CERTIFICATE;
                OSPM_DBGERRORLOG(errorcode, "Count > 0, but no authority certificates");
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Verify certificate against trusted certificates */
        for (count = 0; (count < certNum) && (errorcode == OSPC_ERR_NO_ERROR); count++) {
            if (cert[count] == OSPC_OSNULL) {
                errorcode = OSPC_ERR_SEC_NULL_CERTIFICATE;
                OSPM_DBGERRORLOG(errorcode, "Missing pointer to cert");
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /* Get an authority certificate to test against */
                certData = NULL;
                errorcode = OSPPX509CertGetCertificate(cert[count], &certData, &certSize);
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                if (certData == OSPC_OSNULL) {
                    errorcode = OSPC_ERR_SEC_NULL_CERTIFICATE;
                    OSPM_DBGERRORLOG(errorcode, "Missing certificate data");
                }

                /* authority cert must be same size as test cert for a match */
                if (ospvRootCACertLen == (int) certSize) {
                    /* Certs are same size, compare them */
                    if (OSPM_MEMCMP(ospvRootCACert, certData, ospvRootCACertLen) == 0) {
                        certMatch = OSPC_TRUE;
                        break;
                    }
                }
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (certMatch != OSPC_TRUE) {
            errorcode = OSPC_ERR_SEC_CA_CERTIFICATES_DONT_MATCH;
        }
    }

    return errorcode;

}   /* OSPPSecVerifyRootAuthorityCertificate */

int OSPPSecSetAuthorityCertificates(
    OSPTSEC *ospvSecurity, 
    unsigned ospvNumberOfAuthorityCertificates, 
    const OSPT_CERT *ospvAuthorityCertificates[])
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *newCertInfo = OSPC_OSNULL;
    unsigned i = 0;
    unsigned certCount;
    X509 *newX509Cert = OSPC_OSNULL;
    unsigned char *p = OSPC_OSNULL;

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_WRITE);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvNumberOfAuthorityCertificates > OSPC_SEC_MAX_AUTH_CERTS) {
            errorcode = OSPC_ERR_SEC_TOO_MANY_CERTIFICATES;
            OSPM_DBGERRORLOG(errorcode, "Too many authority certificates");
        }
    }

    /* Delete the existing certificate list */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        certCount = ospvSecurity->NumberOfAuthorityCertificates;

        if (certCount > 0) {
            OSPPSecDeleteAuthorityCertificates(ospvSecurity);
            certCount = 0;
        }
    }

    /* Add certificates to certificate list */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Create X509_STORE */
        ospvSecurity->AuthorityCertStore = X509_STORE_new();
        ospvSecurity->AuthorityCertStack = sk_X509_new_null();

        /* Create new certificate list */
        for (i = 0; i < ospvNumberOfAuthorityCertificates; i++) {
            if (ospvAuthorityCertificates[i] == OSPC_OSNULL) {
                errorcode = OSPC_ERR_SEC_NULL_CERTIFICATE;
                OSPM_DBGERRORLOG(errorcode, "No certificate provided");
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /* Create/initialize a new certInfo for new local certificate */
                errorcode = OSPPX509CertCreate(ospvAuthorityCertificates[i]->CertData, &newCertInfo);

                /* Convert DER encoded cert to X509 object */

                /* p is a temporary pointer, it will be updated by the functions */
                p = ospvAuthorityCertificates[i]->CertData;
                newX509Cert = d2i_X509(OSPC_OSNULL, (const unsigned char **)(&p), ospvAuthorityCertificates[i]->CertDataLength);

                if (newX509Cert != OSPC_OSNULL) {
                    sk_X509_push(ospvSecurity->AuthorityCertStack, newX509Cert);
                    if (1 == X509_STORE_add_cert(ospvSecurity->AuthorityCertStore, newX509Cert)) {
                        /* Success */
                        errorcode = OSPC_ERR_NO_ERROR;
                    } else {
                        errorcode = OSPC_ERR_X509_STORE_ERROR;
                        OSPM_DBGERRORLOG(errorcode, "Failed to decode X509 cert");
                        OpenSSLErrorLog(errorcode);
                    }

                    /*
                     ** At this time, there are two references to the X509 object - 
                     ** this pointer and the X509_STORE.
                     ** This call will decrement the reference counter, but
                     ** will NOT release the object.
                     */
                    X509_free(newX509Cert);
                } else {
                    errorcode = OSPC_ERR_X509_DECODING_ERROR;
                    OSPM_DBGERRORLOG(errorcode, "d2i_X509 failed");
                    OpenSSLErrorLog(errorcode);
                }
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                ospvSecurity->AuthorityCertInfo[i] = newCertInfo;
                ospvSecurity->NumberOfAuthorityCertificates++;
            }

            if (errorcode != OSPC_ERR_NO_ERROR) {
                break;
            }
        }
    }

    OSPPSecUnlock(ospvSecurity);
    return errorcode;
}

int OSPPSecSignatureVerify(
    OSPTSEC *ospvSecurity,
    unsigned char **ospvContent,
    unsigned *ospvContentLength, 
    unsigned char *ospvSignature, 
    unsigned ospvSignatureLength, 
    int ospvSignatureOnly)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    PKCS7 *signedData = OSPC_OSNULL;
    unsigned char *p = ospvSignature;

    BIO *mem_bio_out = OSPC_OSNULL;
    char *tokenBuf = OSPC_OSNULL;

    /* decode signed data */
    signedData = d2i_PKCS7(OSPC_OSNULL, (const unsigned char **)(&p), ospvSignatureLength);

    if (signedData != OSPC_OSNULL) {
        /* it will be used for storing decoded xml token */
        mem_bio_out = BIO_new(BIO_s_mem());

        /* verify message and extract xml token */
        if (1 == PKCS7_verify(signedData,
                              ospvSecurity->AuthorityCertStack, ospvSecurity->AuthorityCertStore, OSPC_OSNULL, mem_bio_out, PKCS7_NOCHAIN)) {
            /* Success, copy extracted message to output parameters */
            *ospvContentLength = BIO_get_mem_data(mem_bio_out, &tokenBuf);

            OSPM_MALLOC(*ospvContent, unsigned char, *ospvContentLength);
            OSPM_MEMCPY(*ospvContent, tokenBuf, *ospvContentLength);
        } else {
            errorcode = OSPC_ERR_PKCS7_INVALID_SIGNATURE;
            OpenSSLErrorLog(errorcode);
            OSPM_DBGERRORLOG(errorcode, "Failed to verify signature");
        }

        /* release resources */
        BIO_free(mem_bio_out);
        PKCS7_free(signedData);
    } else {
        errorcode = OSPC_ERR_PKCS7_INVALID_SIGNATURE;
        OpenSSLErrorLog(errorcode);
        OSPM_DBGERRORLOG(errorcode, "Failed to decode signature");
    }

#if 0
    OSPTASN1OBJECT *signatureObject = OSPC_OSNULL;
    OSPTASN1OBJECT **authorityCertInfos = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *el = OSPC_OSNULL;
    unsigned int authorityCertInfoCount = 0;
    unsigned char *content = OSPC_OSNULL;
    unsigned contentLength = 0;

    /* THE SIGNATURE
       The signature is assumed to be an ASN1 encoded  PKCS7 CONTENT INFO
       whose content type is SIGNED DATA.  The SIGNED DATA content
       contains a CONTENT INFO whose content type is DATA.  The DATA
       content may or may not contain the data that was digested to create
       the SIGNED DATA Signature element.  

       If ospvContent is a null pointer, then expect the content that was
       digested to be included in the signature.  Verify the signature
       using the data and then extract the data into dynamically allocated
       storage.  

       If ospvContent is non-null, then assume the signature does not
       contain the data that was digested to create the signature.  Digest
       the data specified by ospvContent/ospvContentLength and use the
       digest to verify the signature.  Do not attempt to extract content
       from the signature.
     */

    /* Decode the signature, extract content if necessary, verify signature
       against signer certificate contained in signature.  Return decoded signer
       signer certificate so it can be validated agains certificate
       authorities. */
    errorcode = OSPPPKCS7SignatureParse(&signatureObject, ospvSignature, ospvSignatureLength);


    if (ospvSignatureOnly) {
        content = *ospvContent;
        contentLength = *ospvContentLength;
    } else {
        errorcode = OSPPPKCS7SignatureGetContent(signatureObject, &content, &contentLength, &el);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the ca cetificates */
        authorityCertInfos = ospvSecurity->AuthorityCertInfo;

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Get the number of CA's */
            authorityCertInfoCount = ospvSecurity->NumberOfAuthorityCertificates;
        }
    }


    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPPKCS7SignatureVerify(signatureObject, content, contentLength, authorityCertInfos, authorityCertInfoCount);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*** OSPM_DBGERRORLOG(0, "SIGNATURE VERIFIED"); ***/

        if (!ospvSignatureOnly) {
            OSPM_MALLOC(*ospvContent, unsigned char, contentLength);
            if ((*ospvContent) == OSPC_OSNULL) {
                errorcode = OSPC_ERR_SEC_UNABLE_TO_ALLOCATE_SPACE;
                OSPM_DBGERRORLOG(errorcode, "Couldn't allocate space for content");
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPM_MEMCPY(*ospvContent, content, contentLength);
                *ospvContentLength = contentLength;
            }
        }
    } else if (errorcode == OSPC_ERR_PKCS7_INVALID_SIGNATURE) {
        OSPM_DBGERRORLOG(errorcode, "SIGNATURE NOT VERIFIED");
    } else {
        OSPM_DBGERRORLOG(errorcode, "SIGNATURE - ERROR");
    }
    if (el != OSPC_OSNULL) {
        OSPPASN1ElementDelete(&el, 0);
    }
    OSPPASN1ObjectDelete(&signatureObject);

#endif
    return errorcode;
}

/* 
 * Copy the DER encoded local certificate from the security object  to the storage supplied by the caller 
 */
int OSPPSecCopyLocalCertificate(
    OSPTSEC *ospvSecurity,                  /* In - security context */
    unsigned *ospvLocalCertificateSize,     /* In/Out - max size/actual size */
    unsigned char *ospvLocalCertificate)    /* Out - Storage space for cert */
{
    OSPTASN1OBJECT *certInfo = OSPC_OSNULL;
    unsigned certSize = 0;
    unsigned char *certData = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    /* Lock the security module for reading */
    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);

    /* Check to see if there is a local certificate, and that enough space has
       been provided to save the certificate. */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if (ospvLocalCertificate == OSPC_OSNULL) {
            errorcode = OSPC_ERR_SEC_NO_SPACE_FOR_CERTIFICATE;
            OSPM_DBGERRORLOG(errorcode, "No space provided for certificate");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the certinfo structure */
        errorcode = OSPPSecGetLocalCertInfo(ospvSecurity, &certInfo);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the certificate size and test for enough space */
        errorcode = OSPPX509CertGetCertificate(certInfo, &certData, &certSize);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            if (certSize > *ospvLocalCertificateSize) {
                errorcode = OSPC_ERR_SEC_CERTIFICATE_TOO_BIG;
                OSPM_DBGERRORLOG(errorcode, "Local certificate is too big");
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Retrieve the certificate into the supplied storage area */
        if (certSize > 0) {
            OSPM_MEMCPY(ospvLocalCertificate, certData, certSize);
        }
        *ospvLocalCertificateSize = certSize;
    }

    /* Unlock the security module */
    OSPPSecUnlock(ospvSecurity);

    return errorcode;
}

/* Return the number of authority certificates stored in the security object */
int OSPPSecGetNumberOfAuthorityCertificates(
    OSPTSEC *ospvSecurity,  /* In - Security context */
    unsigned *ospvNumberOfAuthorityCertificates)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvNumberOfAuthorityCertificates = ospvSecurity->NumberOfAuthorityCertificates;
    }

    return errorcode;
}

/*
 * IMPLEMENTATION OF LOCAL FUNCTIONS
 */
int OSPPSecTestContext(
    OSPTSEC *ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvSecurity == OSPC_OSNULL) {
        errorcode = OSPC_ERR_SEC_INVALID_CONTEXT;
        OSPM_DBGERRORLOG(errorcode, "Security context is null pointer");
    }

    return errorcode;
}

int OSPPSecLock(OSPTSEC *ospvSecurity, OSPE_LOCK_MODE ospvLockMode)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecTestContext(ospvSecurity);

#ifdef ENABLESECLOCK
#ifndef ENABLETESTVERIFY
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MUTEX_LOCK(ospvSecurity->SecurityMutex, errorcode);
        OSPM_ARGUSED(ospvLockMode);
    }
#endif

#endif
    OSPM_ARGUSED(ospvLockMode);

    return errorcode;
}

int OSPPSecUnlock(OSPTSEC *ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecTestContext(ospvSecurity);

#ifdef ENABLESECLOCK
#ifndef ENABLETESTVERIFY
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MUTEX_UNLOCK(ospvSecurity->SecurityMutex, errorcode);
    }
#endif
#endif

    return errorcode;
}

int OSPPSecDeleteAuthorityCertificates(
    OSPTSEC *ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *certInfo = OSPC_OSNULL;
    unsigned i = 0;

    /* Context must exist */
    errorcode = OSPPSecTestContext(ospvSecurity);

    if (errorcode == OSPC_ERR_NO_ERROR) {

        /* Free X509 Store */
        X509_STORE_free(ospvSecurity->AuthorityCertStore);
        sk_X509_free(ospvSecurity->AuthorityCertStack);

        /* Delete certificates from the list */
        for (i = 0; ospvSecurity->NumberOfAuthorityCertificates; i++, ospvSecurity->NumberOfAuthorityCertificates--) {
            /* Get the next certificate from list */
            certInfo = ospvSecurity->AuthorityCertInfo[i];

            /* Make sure there is something to delete */
            if (certInfo == OSPC_OSNULL) {
                errorcode = OSPC_ERR_SEC_NULL_CERTIFICATE;
                OSPM_DBGERRORLOG(errorcode, "Found unexpected NULL certificate");
            }

            /* Delete the certificate storage */
            if (errorcode == OSPC_ERR_NO_ERROR) {
                /* Free the Cert Info Structure */
                errorcode = OSPPX509CertDelete(&certInfo);
            }

            /* Quit if error occurs */
            if (errorcode != OSPC_ERR_NO_ERROR) {
                break;
            }
        }
    }

    return errorcode;
}

int OSPPSecSetDigestAlgorithm(
    OSPTSEC *ospvSecurity, 
    OSPTASN1OBJECT *ospvDigestAlgorithmId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecTestContext(ospvSecurity);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        ospvSecurity->DigestAlgorithm = ospvDigestAlgorithmId;
    }

    return errorcode;
}

int OSPPSecSignatureCreate(OSPTSEC *ospvSecurity,
    unsigned char *ospvContent,
    unsigned ospvContentLength, 
    unsigned char **ospvSignature, 
    unsigned *ospvSignatureLength, 
    int ospvSignatureOnly)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /* Lock Sec for reading */
    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Create Signature */
        errorcode = OSPPPKCS7SignatureCreate(ospvContent, ospvContentLength,
            ospvSecurity->DigestAlgorithm,
            ospvSecurity->LocalCertInfo,
            ospvSecurity->PrivateKeyInfo, ospvSignatureOnly, ospvSignature, ospvSignatureLength);
    }
#ifdef ENABLETESTVERIFY
    if (errorcode == OSPC_ERR_NO_ERROR) {
        unsigned char *content = OSPC_OSNULL;
        unsigned contentLength = 0;
        OSPTASN1OBJECT *signatureObject = OSPC_OSNULL;

        OSPM_DBGERRORLOG(0, "TEST VERIFYING CREATED SIGNATURE....");

        if (ospvSignatureOnly) {
            content = ospvContent;
            contentLength = ospvContentLength;
        }

        errorcode = OSPPPKCS7SignatureParse(&signatureObject, *ospvSignature, *ospvSignatureLength);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPSecSignatureVerify(ospvSecurity, &content, &contentLength, *ospvSignature, *ospvSignatureLength, ospvSignatureOnly);
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(0, "SIGNATURE VERIFIED");
        } else if (errorcode == OSPC_ERR_PKCS7_INVALID_SIGNATURE) {
            OSPM_DBGERRORLOG(errorcode, "SIGNATURE NOT VERIFIED");
        } else {
            OSPM_DBGERRORLOG(errorcode, "SIGNATURE - ERROR");
        }

        OSPPASN1ObjectDelete(&signatureObject);

    }
#endif

    errorcode = OSPPSecUnlock(ospvSecurity);

    return errorcode;
}

int OSPPSecGetPrivateKeyData(
    OSPTSEC *ospvSecurity, 
    unsigned char **ospvPrivateKeyData, 
    unsigned int *ospvPrivateKeyDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecTestContext(ospvSecurity);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPPKCS8KeyInfoGetPrivateKey(ospvSecurity->PrivateKeyInfo, ospvPrivateKeyData, ospvPrivateKeyDataLength);
    }

    return errorcode;
}


int OSPPSecGetLocalCertInfo(
    OSPTSEC *ospvSecurity,              /* In - security context */
    OSPTASN1OBJECT **ospvLocalCertInfo) /* In - Ptr to ptr to cert */
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPSecTestContext(ospvSecurity);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvLocalCertInfo = ospvSecurity->LocalCertInfo;
    }

    return errorcode;
}

int OSPPSecValidCertChain(
    OSPTSEC *ospvSecurity, 
    int *ospvCAIndex, 
    unsigned char *ospvCertificate, 
    unsigned int *ospvCertificateLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *testCert = OSPC_OSNULL;
    int parentIndex = 0;
    unsigned char *certificate = OSPC_OSNULL;
    unsigned int certificateLength = 0;
    OSPTBOOL localcert = OSPC_FALSE;

    /* 
       This function tests certificates against the certificates stored in the
       CA certificate list. If it finds a parent certificate, it returns the
       index of the parent to the caller.  

       The first time this fuction is called, the caller will set the CAIndex 
       to -1 to indicate that the certificate to be tested is the 
       local signing certificate.  Upon return, CAIndex will be set to the
       index of the issuing certificate.  

       For subsequent calls to this function the caller will leave the CAIndex
       value as is.  The function will use that certificate as the test
       certificate and will find that issuer's certificate and return the
       index to that certificate.

       CAIndex in = -1 means uses localcertificate as the test cert.
       != -1 means test cert is the ca cert at that index.

       Return the parent cert data and length upon each call.  Storge is
       provided by caller.  Caller should set certificate length to size of
       certificate storage.

       For self signed, the index going back will be the index of the
       certificate that was tested.  Rich will check it to see if it changes
       on his end.
     */

    errorcode = OSPPSecLock(ospvSecurity, OSPC_LOCK_READ);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the test certificate */
        if (*ospvCAIndex == (-1)) {
            /* Get local certificate for testing */
            testCert = ospvSecurity->LocalCertInfo;
            localcert = OSPC_TRUE;
        } else {
            if (*ospvCAIndex < (int)ospvSecurity->NumberOfAuthorityCertificates) {
                testCert = ospvSecurity->AuthorityCertInfo[*ospvCAIndex];
            } else {
                errorcode = OSPC_ERR_SEC_CACERT_INDEX_OVERFLOW;
                OSPM_DBGERRORLOG(errorcode, "CA Index is out of range");
            }
        }

        /* Have the test certificate, find issuer in Authority Certificates */
        errorcode = OSPPX509CertValidateCertificate(testCert,
            ospvSecurity->AuthorityCertInfo, ospvSecurity->NumberOfAuthorityCertificates, &parentIndex);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Set the index return value */
            *ospvCAIndex = parentIndex;

            /* Get the authority certificate content, place in provided
               buffer */
            if (localcert) {
                errorcode = OSPPX509CertGetCertificate(ospvSecurity->LocalCertInfo, &certificate, &certificateLength);
            } else {
                errorcode = OSPPX509CertGetCertificate(ospvSecurity->AuthorityCertInfo[parentIndex], &certificate, &certificateLength);
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                if (*ospvCertificateLength < certificateLength) {
                    errorcode = OSPC_ERR_SEC_NOT_ENOUGH_SPACE_FOR_COPY;
                    OSPM_DBGERRORLOG(errorcode, "Not enough space provided for certificate");
                } else {
                    OSPM_MEMCPY(ospvCertificate, certificate, certificateLength);
                    *ospvCertificateLength = certificateLength;
                }
            }
        }
    }

    errorcode = OSPPSecUnlock(ospvSecurity);

    return errorcode;
}

int OSPPSecGetSignerCertSubjectName(
    OSPTSEC *ospvSecurity,
    unsigned char *ospvToken, 
    unsigned ospvTokenLen, 
    unsigned char **ospvSubjectName, 
    unsigned *ospvSubjectNameLen)
{
    OSPTASN1OBJECT *signercert = OSPC_OSNULL;
    OSPTASN1OBJECT *signatureobj = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL, *eInfo2 = OSPC_OSNULL;
    unsigned char *cert = OSPC_OSNULL;
    unsigned certlen = 0;

    errorcode = OSPPSecTestContext(ospvSecurity);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPPKCS7SignatureParse(&signatureobj, ospvToken, ospvTokenLen);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ObjectGetElementByDataRef(signatureobj, &eInfo, OSPEDRID_SIG_SGNDAT_CERTIFICATE);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Reparse the certificate */
        errorcode = OSPPASN1ElementGetElementData(eInfo, &cert, &certlen);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPX509CertCreate(cert, &signercert);
        cert = OSPC_OSNULL;
        certlen = 0;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the signer certificate subject name */
        errorcode = OSPPASN1ObjectGetElementByDataRef(signercert, &eInfo2, OSPEDRID_CERT_SUBJECT);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementGetElementData(eInfo2, &cert, &certlen);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(*ospvSubjectName, unsigned char, certlen + 1);
        if (*ospvSubjectName != OSPC_OSNULL) {
            OSPM_MEMSET(*ospvSubjectName, 0, certlen + 1);
            OSPM_MEMCPY(*ospvSubjectName, cert, certlen);
            *ospvSubjectNameLen = certlen;
        }
    }

    if (eInfo != OSPC_OSNULL) {
        OSPPASN1ElementDelete(&eInfo, 0);
    }

    if (eInfo2 != OSPC_OSNULL) {
        OSPPASN1ElementDelete(&eInfo2, 0);
    }

    if (signercert != OSPC_OSNULL) {
        OSPPASN1ObjectDelete(&signercert);
    }

    if (signatureobj != OSPC_OSNULL) {
        OSPPASN1ObjectDelete(&signatureobj);
    }

    return errorcode;
}

void OpenSSLErrorLog(
    int errorcode)
{
    BIO *mem_bio = OSPC_OSNULL;
    char *p = OSPC_OSNULL;
    long len = 0;

    mem_bio = BIO_new(BIO_s_mem());

    ERR_print_errors(mem_bio);

    len = BIO_get_mem_data(mem_bio, &p);

    p[len] = '\0';

    OSPM_DBGERRORLOG(errorcode, p);

    BIO_free(mem_bio);
}
