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

#ifndef _OSPCRYPTOWRAP_H
#define _OSPCRYPTOWRAP_H

#define OSPC_CRYPTO_DECRYPT_BUFFER_MAXLENGTH        1024
#define OSPC_CRYPTO_ENCRYPT_BUFFER_MAXLENGTH        1024
#define OSPC_CRYPTO_DIGEST_BUFFER_MAXLENGTH         1024

#define OSPC_CRYPTO_FLAG_NORMAL                     0x00
#define OSPC_CRYPTO_FLAG_USE_BER_ALGORITHM          0x01
#define OSPC_CRYPTO_FLAG_USE_PUBLIC_KEY             0x02
#define OSPC_CRYPTO_FLAG_RAW_MODE                   0x04
#define OSPC_CRYPTO_FLAG_USE_SHA1_DIGEST            0x08
#define OSPC_CRYPTO_FLAG_ENCRYPTED_WITH_PUBLIC_KEY  0x10

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif
    int OSPPCryptoWrapDigest(unsigned char *ospvDigest, unsigned *ospvDigestLength, unsigned char *ospvBERAlgorithm, unsigned ospvBERAlgorithmLength,
            unsigned char *ospvData, unsigned ospvDataLength, unsigned char ospvFlags);
    int OSPPCryptoWrapEncrypt(unsigned char *ospvEncryptedData, unsigned *ospvEncryptedDataLength, unsigned char *ospvBERAlgorithm,
            unsigned ospvBERAlgorithmLength, unsigned char *ospvData, unsigned ospvDataLength, unsigned char *ospvSignerKey,
            unsigned ospvSignerKeyLength, unsigned char ospvFlags);
    int OSPPCryptoWrapDecrypt(unsigned char *ospvDecryptedData, unsigned *ospvDecryptedDataLength, unsigned char *ospvBERAlgorithm,
            unsigned ospvBERAlgorithmLength, unsigned char *ospvEncryptedData, unsigned ospvEncryptedDataLength, unsigned char *ospvBERReaderKey,
            unsigned ospvBERReaderKeyLength, unsigned char ospvFlags);
    int OSPPCryptoWrapVerify(unsigned char *ospvData, unsigned ospvDataLength, unsigned char *ospvSignature, unsigned ospvSignatureLength,
            unsigned char *ospvReaderKey, unsigned ospvReaderKeyLength, unsigned char ospvFlags);

#ifdef __cplusplus
}
#endif

#endif /* _OSPCRYPTOWRAP_H */
