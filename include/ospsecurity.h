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
 * ospsecurity.h - Prototypes for security object.
 */
#ifndef _OSPSECURITY_H
#define _OSPSECURITY_H

#include "osp.h"
#include "osptrans.h"
#include "ospasn1.h"
#include "osppkcs1.h"
#include "ospx509.h"
#include "ospsslsess.h"

#include <openssl/x509.h>

/* defines for Local or Remote validation */
#define OSPC_LOCAL  0x05
#define OSPC_REMOTE 0x06

#define OSPC_SEC_MAX_AUTH_CERTS 5 

#define OSPC_SEC_SIGNATURE_AND_CONTENT 0
#define OSPC_SEC_SIGNATURE_ONLY 1


/*-------------------------------------------*/
/* security typedef                          */
/*-------------------------------------------*/ 
typedef struct _OSPTSEC
{
    unsigned            NumberOfAuthorityCertificates;
    OSPTASN1OBJECT      *AuthorityCertInfo[OSPC_SEC_MAX_AUTH_CERTS];
		X509_STORE					*AuthorityCertStore;
    OSPTASN1OBJECT      *LocalCertInfo;
    OSPTASN1OBJECT      *PrivateKeyInfo;
    OSPTASN1OBJECT      *DigestAlgorithm;
        unsigned            LocalValidation;
    OSPTMUTEX           SecurityMutex;
    OSPTSSLSESSIONID    *SSLSessionIdList;    
    unsigned            SSLLifetime;
    OSPTBOOL            SSLSessionHasMutex;
    OSPTMUTEX           SSLSessionMutex;
    void                *ContextRef;
} OSPTSEC;



#ifdef __cplusplus
extern "C" 
{
#endif

    /********************************/
    /* PROTOTYPES FOR ospsecurity.c */
    /********************************/

    int
    OSPPSecCopyPrivateKey(
        OSPTSEC *ospvSecurity,                  
        OSPTPRIVATEKEY *ospvPrivateKey);

    int  
    OSPPSecGetPrivateKeyData(
        OSPTSEC *ospvSecurity, 
        unsigned char **ospvPrivateKeyData,
        unsigned int  *ospvPrivateKeyDataLength);

    int
    OSPPSecNew(
        OSPTSEC **ospvSecurity);

    int
    OSPPSecGetLocalValidation(
        OSPTSEC *ospvSecurity,
        unsigned *ospvLocalValidation);

    int OSPPSecSetPrivateKey(
        OSPTSEC *ospvSecurity,
        OSPTPRIVATEKEY *ospvPrivateKey);

    int
    OSPPSecSetLocalCertificate(
        OSPTSEC         *ospvSecurity,
        unsigned char   *ospvLocalCertificate,
        unsigned long   *ospvCustomerId,
        unsigned long   *ospvDeviceId);

    int
    OSPPSecSetLocalValidation(
        OSPTSEC     *ospvSecurity, 
        unsigned    ospvLocalValidation);

    int
    OSPPSecGetNumberOfAuthorityCertificates(
        OSPTSEC     *ospvSecurity,
        unsigned    *ospvNumberOfAuthorityCertificates);

    int
    OSPPSecCopyAuthorityCertificates(
        OSPTSEC *ospvSecurity,
        unsigned ospvMaxCertificateSize,
        unsigned char *ospvAuthorityCertificates[],
        unsigned *ospvCertificateCount);

    int
    OSPPSecDelete(
        OSPTSEC     **ospvSecurity);


    int
    OSPPSecVerifyRootAuthorityCertificate(
        OSPTSEC *ospvSecurity,
        void    *ospvRootCACert,
        int      ospvRootCACertLen);

    int
    OSPPSecSetAuthorityCertificates(
        OSPTSEC         *ospvSecurity,
        unsigned        ospvNumberOfAuthorityCertificates,
        const OSPTCERT  *ospvAuthorityCertificates[]);

    int 
    OSPPSecSignatureCreate(
        OSPTSEC *ospvSecurity, 
        unsigned char *ospvContent,
        unsigned ospvContentLength, 
        unsigned char **ospvSignature, 
        unsigned *ospvSignatureLength,
        int ospvSignatureOnly);

    int  
    OSPPSecSignatureVerify(
        OSPTSEC *ospvSecurity, 
        unsigned char **ospvContent, 
        unsigned *ospvContentLength, 
        unsigned char *ospvSignature,
        unsigned ospvSignatureLength,
        int ospvSignatureOnly);

    int 
    OSPPSecCopyLocalCertificate(
        OSPTSEC *ospvSecurity, 
        unsigned *ospvLocalCertificateSize,
        unsigned char *ospvLocalCertificate);

    int
    OSPPSecGetSignerCertSubjectName(
        OSPTSEC         *ospvSecurity,
        unsigned char   *ospvToken,
        unsigned        ospvTokenLen,
        unsigned char   **ospvSubjectName,
        unsigned        *ospvSubjectNameLen);
        

    /******************************/
    /* PROTOTYPES FOR ospsecssl.c */
    /******************************/

    OSPTBOOL
        OSPPSecSSLLifetimeHasExpired(
        OSPTSEC          *ospvSecurity,
        OSPTSSLSESSIONID *ospvSessionId);


    unsigned
    OSPPSecGetSSLLifetime(
        OSPTSEC *ospvSecurity);

    int
    OSPPSecSSLSessionIdNew(
        OSPTSEC        *ospvSecurity,
        unsigned long   ospvLength, 
        unsigned char  *ospvData,
        char           *ospvHostName,
        short           ospvPort);

    int
    OSPPSecSSLSessionIdGet(
        OSPTSEC        *ospvSecurity,
        unsigned long  *ospvLength, 
        unsigned char **ospvData,
        char           *ospvHostName,
        short           ospvPort);

    void
    OSPPSecSSLSessionIdDelete(
        OSPTSEC           *ospvSecurity,
        OSPTSSLSESSIONID **ospvSSLSessionId,
        OSPTBOOL           ospvUseMutex);

    int
    OSPPSecSSLSessionIdInitDB(
        OSPTSEC *ospvSecurity);

    void
    OSPPSecSSLSessionIdDeleteDB(
        OSPTSEC *ospvSecurity);

    void
    OSPPSecSetSSLLifetime(
        OSPTSEC *ospvSecurity,
        unsigned ospvSSLLifetime);

    int
    OSPPSecValidCertChain(
        OSPTSEC *ospvSecurity,
        int     *ospvCAIndex,
        unsigned char *ospvCertificate,
        unsigned int  *ospvCertificateLength);

#ifdef __cplusplus
}
#endif

#endif
