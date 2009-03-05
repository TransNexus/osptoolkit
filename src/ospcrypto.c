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
 * ospcrypto.c - Member functions to map OSP crypto calls to crypto 
                    library calls. (BSAFE).  This file is included in 
                    ospcryptowrap.c if OSPC_CRYPTO_BSAFE is defined in
                    ospcrypto.h
*/

#include "osp/osp.h"
#include "osp/osptnlog.h"
#include "osp/osppkcs8.h"
#include "osp/ospcrypto.h"
#include "osp/ospcryptowrap.h"

/*
 * Member functions
 */

int OSPPCryptoEncrypt(
    OSPTASN1OBJECT **ospvEncryptedData,
    OSPTASN1OBJECT *ospvAlgorithmId, 
    OSPTASN1OBJECT *ospvData, 
    OSPTASN1OBJECT *ospvPrivateKeyInfo)
{
    unsigned char *encryptedData = OSPC_OSNULL;
    unsigned int encryptedDataLength = 0;
    unsigned char *algorithmBER = OSPC_OSNULL;
    unsigned int algorithmBERLength = 0;
    unsigned char *data = OSPC_OSNULL;
    unsigned int dataLength = 0;
    unsigned char *signerBERKey = OSPC_OSNULL;
    unsigned int signerBERKeyLength = 0;

    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    int errorcode = OSPC_ERR_NO_ERROR;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the data from the Data object. */
        if (ospvData != OSPC_OSNULL) {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvData, &eInfo);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ElementGetElementData(eInfo, &data, &dataLength);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the algorithm BER values */
        if (ospvAlgorithmId != OSPC_OSNULL) {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvAlgorithmId, &eInfo);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ElementGetElementData(eInfo, &algorithmBER, &algorithmBERLength);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the private key data */
        errorcode = OSPPKCS8KeyInfoGetEncodedKeyInfo(ospvPrivateKeyInfo, &signerBERKey, &signerBERKeyLength);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Encrypt the data using vendor specific code */
        encryptedDataLength = OSPC_CRYPTO_ENCRYPT_BUFFER_MAXLENGTH;
        OSPM_MALLOC(encryptedData, unsigned char, encryptedDataLength);

        if (encryptedData == OSPC_OSNULL) {
            errorcode = OSPC_ERR_CRYPTO_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for encrypted data");
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPCryptoWrapEncrypt(encryptedData, &encryptedDataLength,
                algorithmBER, algorithmBERLength,
                data, dataLength, signerBERKey, signerBERKeyLength, OSPC_CRYPTO_FLAG_USE_BER_ALGORITHM);
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Encode the encrypted data buffer */
        errorcode = OSPPASN1OctetStringEncode(ospvEncryptedData, encryptedData, encryptedDataLength, OSPEDRID_NOTDEFINED);
    }

    if (encryptedData != OSPC_OSNULL) {
        OSPM_FREE(encryptedData);
    }

    return errorcode;
}

int OSPPCryptoVerify(
    OSPTASN1OBJECT *ospvSignature, 
    OSPTASN1OBJECT *ospvReaderKey, 
    unsigned char *ospvData, 
    unsigned ospvDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *signature = OSPC_OSNULL;
    unsigned int signatureLength = 0;
    unsigned char *readerKey = OSPC_OSNULL;
    unsigned int readerKeyLength = 0;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the data from the Signature */
        if (ospvSignature != OSPC_OSNULL) {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvSignature, &eInfo);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ElementGetContentData(eInfo, &signature, &signatureLength);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* NOTE:  Skip the "number of unused bits in last
                       octet" octet in the signature.  (signature is an ASN1
                       Bit String. Since a signature will always be an even
                       multiple of 8 bits, the first byte should be a zero */
                    if (*signature == 0) {
                        signature++;
                        signatureLength--;
                    } else {
                        errorcode = OSPC_ERR_CRYPTO_INVALID_PARAMETERS;
                        OSPM_DBGERRORLOG(errorcode, "Expected 0 for " "empty-bits-count in signature bitstring");
                    }
                }
            }
        } else {
            errorcode = OSPC_ERR_CRYPTO_INVALID_PARAMETERS;
            OSPM_DBGERRORLOG(errorcode, "NULL signature specified");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the Key */
        if (ospvReaderKey != OSPC_OSNULL) {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvReaderKey, &eInfo);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ElementGetElementData(eInfo, &readerKey, &readerKeyLength);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Verify the signature */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPCryptoWrapVerify(ospvData, ospvDataLength,
                                             signature, signatureLength, readerKey, readerKeyLength, OSPC_CRYPTO_FLAG_NORMAL);
        }
    }

    return errorcode;
}

int OSPPCryptoDigest(
    OSPTASN1OBJECT **ospvDigest, 
    OSPTASN1OBJECT *ospvAlgorithmId, 
    unsigned char *ospvData, 
    unsigned int ospvDataLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *algorithmBER = OSPC_OSNULL;
    unsigned int algorithmBERLength = 0;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    unsigned char *digest = OSPC_OSNULL;
    unsigned int digestLength = 0;

    errorcode = OSPPASN1ObjectGetElementInfo(ospvAlgorithmId, &eInfo);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementGetElementData(eInfo, &algorithmBER, &algorithmBERLength);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {

        /* Digest the data using vendor specific code */
        digestLength = OSPC_CRYPTO_DIGEST_BUFFER_MAXLENGTH;
        OSPM_MALLOC(digest, unsigned char, digestLength);
        if (digest == OSPC_OSNULL) {
            errorcode = OSPC_ERR_CRYPTO_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for digested data");
        } else {
            errorcode = OSPPCryptoWrapDigest(digest, &digestLength,
                algorithmBER, algorithmBERLength, ospvData, ospvDataLength, OSPC_CRYPTO_FLAG_USE_BER_ALGORITHM);
        }
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1OctetStringEncode(ospvDigest, digest, digestLength, OSPEDRID_NOTDEFINED);
    }
    if (OSPC_OSNULL != digest) {
        OSPM_FREE(digest);
    }

    return errorcode;
}   /* OSPPCryptoDigest */

int OSPPCryptoDecrypt(
    OSPTASN1OBJECT **ospvDecryptedData,
    OSPTASN1OBJECT *ospvAlgorithmId, 
    OSPTASN1OBJECT *ospvEncryptedData, 
    OSPTASN1OBJECT *ospvReaderKey)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    unsigned char *decryptedData = OSPC_OSNULL;
    unsigned int decryptedDataLength = 0;
    unsigned char *encryptedData = OSPC_OSNULL;
    unsigned int encryptedDataLength = 0;
    unsigned char *algorithmBER = OSPC_OSNULL;
    unsigned int algorithmBERLength = 0;
    unsigned char *readerKey = OSPC_OSNULL;
    unsigned int readerKeyLength = 0;

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the encryptedData from the encryptedData object. */
        if (ospvEncryptedData != OSPC_OSNULL) {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvEncryptedData, &eInfo);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ElementGetContentData(eInfo, &encryptedData, &encryptedDataLength);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the algorithm BER values */
        if (ospvAlgorithmId != OSPC_OSNULL) {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvAlgorithmId, &eInfo);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ElementGetElementData(eInfo, &algorithmBER, &algorithmBERLength);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Get the key */
        if (ospvReaderKey != OSPC_OSNULL) {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvReaderKey, &eInfo);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPASN1ElementGetElementData(eInfo, &readerKey, &readerKeyLength);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* decrypt the data using vendor specific code */
        decryptedDataLength = OSPC_CRYPTO_DECRYPT_BUFFER_MAXLENGTH;
        OSPM_MALLOC(decryptedData, unsigned char, decryptedDataLength);

        if (decryptedData == OSPC_OSNULL) {
            errorcode = OSPC_ERR_CRYPTO_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for decrypted data");
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPCryptoWrapDecrypt(decryptedData, &decryptedDataLength,
                algorithmBER, algorithmBERLength,
                encryptedData, encryptedDataLength, readerKey, readerKeyLength, OSPC_CRYPTO_FLAG_USE_BER_ALGORITHM);
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /* Encode the encrypted data buffer */
        errorcode = OSPPASN1OctetStringEncode(ospvDecryptedData, decryptedData, decryptedDataLength, OSPEDRID_NOTDEFINED);
    }
    if (OSPC_OSNULL != decryptedData) {
        OSPM_FREE(decryptedData);
    }

    return errorcode;
}   /* OSPPCryptoDecrypt */
