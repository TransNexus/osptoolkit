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
 * ospcrypto.h      Constants and prototypes for OSP crypto library
 *
 */

#ifndef _OSP_CRYPTO_H
#define _OSP_CRYPTO_H

#ifdef __cplusplus
extern "C" 
{
#endif

    /*
     * Member functions
     */

    int
    OSPPCryptoVerify(
        OSPTASN1OBJECT  *ospvSignature,
        OSPTASN1OBJECT  *ospvReaderKey,
        unsigned char   *ospvData,
        unsigned        ospvDataLength);

    int
    OSPPCryptoDigest( 
        OSPTASN1OBJECT  **ospvDigest, 
        OSPTASN1OBJECT  *ospvAlgorithmId,
        unsigned char   *ospvData, 
        unsigned int    ospvDataLength);

    int 
    OSPPCryptoEncrypt(
        OSPTASN1OBJECT  **ospvEncryptedData,
        OSPTASN1OBJECT  *ospvAlgorithmId,
        OSPTASN1OBJECT  *ospvData,
        OSPTASN1OBJECT  *ospvPrivateKeyInfo);

    int 
    OSPPCryptoDecrypt(
        OSPTASN1OBJECT  **ospvDecryptedData,
        OSPTASN1OBJECT  *ospvAlgorithmId,
        OSPTASN1OBJECT  *ospvEncryptedData,
        OSPTASN1OBJECT  *ospvBERReaderKey);
    /*
     * Non-Member functions
     */

#ifdef __cplusplus
}
#endif

#endif
