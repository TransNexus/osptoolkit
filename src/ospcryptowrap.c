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







#include <stdlib.h>
#include <stdio.h>

#include "osp/osp.h"
#include "osp/ospcryptowrap.h"
#include "osp/osptnlog.h"
#include "osp/osperrno.h"



/*
**	OpenSSL implementation
*/

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>




int 
OSPPCryptoWrapDigest(
        unsigned char   *ospvDigest,
        unsigned        *ospvDigestLength,
        unsigned char   *ospvBERAlgorithm,
        unsigned        ospvBERAlgorithmLength,
        unsigned char   *ospvData,
        unsigned        ospvDataLength,
        unsigned char   ospvFlags)
{
    int errorcode = OSPC_ERR_CRYPTO_IMPLEMENTATION_SPECIFIC_ERROR;
    unsigned char digestedData[OSPC_CRYPTO_DIGEST_BUFFER_MAXLENGTH];    
    unsigned int digestedDataLength = 0;

		EVP_MD_CTX	ctx;
		EVP_MD*			type = OSPC_OSNULL;

    OSPM_ARGUSED(ospvFlags);
    OSPM_ARGUSED(ospvBERAlgorithm);
    OSPM_ARGUSED(ospvBERAlgorithmLength);

    OSPTNLOGDUMP(ospvBERAlgorithm, ospvBERAlgorithmLength, "DIGEST: ospvBERAlgorithm");
    OSPTNLOGDUMP(ospvData, ospvDataLength, "DIGEST: ospvData");


		/* Set digest algorithm */
		type = (EVP_MD *)EVP_md5();

		if (type)
		{
			/* Calcualte digest */
			EVP_DigestInit(&ctx, type);
			EVP_DigestUpdate(&ctx,ospvData,ospvDataLength);
			EVP_DigestFinal(&ctx,digestedData,&digestedDataLength);
			errorcode = OSPC_ERR_NO_ERROR;
		}
		else
		{
			OSPM_DBGERRORLOG(errorcode, "Error setting digest type");
		}


		/* Copy results to OUT params */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {

        if (ospvDigest == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_CRYPTO_INVALID_POINTER;
            OSPM_DBGERRORLOG(errorcode, 
                "Error allocating space for message digest");
        }

        if (*ospvDigestLength < digestedDataLength)
        {
            errorcode = OSPC_ERR_CRYPTO_NOT_ENOUGH_SPACE;
            OSPM_DBGERRORLOG(errorcode,
                "Not enough space for digest");
        }

				if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPM_MEMCPY(ospvDigest, 
                				digestedData, 
												digestedDataLength);

            *ospvDigestLength = digestedDataLength;
            OSPTNLOGDUMP(ospvDigest, *ospvDigestLength, "DIGEST: ospvDigest");
        }
    }

    return errorcode;
}


/* This routine is not yet implemented, it is only used for SMIME */
int 
OSPPCryptoWrapEncrypt(
        unsigned char   *ospvEncryptedData,
        unsigned        *ospvEncryptedDataLength,
        unsigned char   *ospvBERAlgorithm,
        unsigned        ospvBERAlgorithmLength,
        unsigned char   *ospvData,
        unsigned        ospvDataLength,
        unsigned char   *ospvBERSignerKey,
        unsigned        ospvBERSignerKeyLength,
        unsigned char   ospvFlags)
{
    int errorcode = OSPC_ERR_CRYPTO_IMPLEMENTATION_SPECIFIC_ERROR;

    OSPM_DBGERRORLOG(errorcode, "This function is not yet implemented in Open-SSL");

    return errorcode;
}

int 
OSPPCryptoWrapDecrypt(
        unsigned char   *ospvDecryptedData,
        unsigned        *ospvDecryptedDataLength,
        unsigned char   *ospvBERAlgorithm,
        unsigned        ospvBERAlgorithmLength,
        unsigned char   *ospvEncryptedData,
        unsigned        ospvEncryptedDataLength,
        unsigned char   *ospvBERReaderKey,
        unsigned        ospvBERReaderKeyLength,
        unsigned char   ospvFlags)
{

    int errorcode = OSPC_ERR_CRYPTO_IMPLEMENTATION_SPECIFIC_ERROR;
    unsigned char decryptedData[OSPC_CRYPTO_ENCRYPT_BUFFER_MAXLENGTH];      
    unsigned int decryptedDataLength = 0;

		X509_PUBKEY* 		pX509PubKey	= OSPC_OSNULL;
		RSA*         		pRSAPubKey	= OSPC_OSNULL;
		unsigned char*	pData				=	OSPC_OSNULL;
		unsigned int 		len					=	0;


    OSPM_ARGUSED(ospvFlags);
    OSPM_ARGUSED(ospvBERAlgorithm);
    OSPM_ARGUSED(ospvBERAlgorithmLength);

    OSPTNLOGDUMP(ospvEncryptedData, ospvEncryptedDataLength, "DECRYPT: ospvEncryptedData");
    OSPTNLOGDUMP(ospvBERReaderKey, ospvBERReaderKeyLength, "DECRYPT: ospvBERReaderKey");

		pX509PubKey = d2i_X509_PUBKEY(NULL, (const unsigned char**)(&ospvBERReaderKey), ospvBERReaderKeyLength); 

		if (pX509PubKey)
		{
			pData				=	pX509PubKey->public_key->data;
			len					=	pX509PubKey->public_key->length;
			pRSAPubKey	= d2i_RSAPublicKey(NULL,(const unsigned char **) &pData, len);

			if (pRSAPubKey)
			{
				decryptedDataLength = RSA_public_decrypt( ospvEncryptedDataLength,
																									ospvEncryptedData,
																									decryptedData,
																									pRSAPubKey,
																									RSA_PKCS1_PADDING);
				if (decryptedDataLength != -1)
				{
						errorcode = OSPC_ERR_NO_ERROR;
				}
				else
				{
					OSPM_DBGERRORLOG(errorcode, "Failed to decrypt message");
				}

				/* Free up mem */
				RSA_free(pRSAPubKey);
			}
			else
			{
				OSPM_DBGERRORLOG(errorcode, "Failed to init RSA key");
			}

			/* Free up mem */
			X509_PUBKEY_free(pX509PubKey);
		}
		else
		{
			OSPM_DBGERRORLOG(errorcode, "Failed to init X509_PUBKEY");
		}


		/* Copy results to OUT params */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        if (ospvDecryptedData == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_CRYPTO_UNABLE_TO_ALLOCATE_SPACE;
            OSPM_DBGERRORLOG(errorcode, 
                "Error allocating space for decrypted data");
        }

        if (*ospvDecryptedDataLength < decryptedDataLength)
        {
            errorcode = OSPC_ERR_CRYPTO_NOT_ENOUGH_SPACE;
            OSPM_DBGERRORLOG(errorcode,
                "Not enough space for digest");
        }

				if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPM_MEMCPY (ospvDecryptedData, 
												decryptedData, decryptedDataLength);

            *ospvDecryptedDataLength = decryptedDataLength;
            OSPTNLOGDUMP(ospvDecryptedData, *ospvDecryptedDataLength, "DECRYPT: ospvDecryptedData");
        }
    }

    return errorcode;
}




int 
OSPPCryptoWrapVerify(
        unsigned char   *ospvData,
        unsigned        ospvDataLength,
        unsigned char   *ospvSignature,
        unsigned        ospvSignatureLength,
        unsigned char   *ospvBERReaderKey,
        unsigned        ospvBERReaderKeyLength,
        unsigned char   ospvFlags)
{
    int errorcode = OSPC_ERR_CRYPTO_IMPLEMENTATION_SPECIFIC_ERROR;
    unsigned char digestedData[OSPC_CRYPTO_DIGEST_BUFFER_MAXLENGTH];    
    unsigned int digestedDataLength = OSPC_CRYPTO_DIGEST_BUFFER_MAXLENGTH;    
		X509_PUBKEY*		pX509PubKey	= OSPC_OSNULL;
		RSA*						pRSAPubKey	= OSPC_OSNULL;
		unsigned char*	pData				=	OSPC_OSNULL;
		unsigned int		len					=	0;
		int							type				=	NID_md5;

    OSPM_ARGUSED(ospvFlags);

    OSPTNLOGDUMP(ospvData, ospvDataLength, "VERIFY: ospvData");
    OSPTNLOGDUMP(ospvSignature, ospvSignatureLength, "VERIFY: ospvSignature");
    OSPTNLOGDUMP(ospvBERReaderKey, ospvBERReaderKeyLength, "VERIFY: ospvBERReaderKey");


		pX509PubKey = d2i_X509_PUBKEY(NULL, (const unsigned char**)(&ospvBERReaderKey), ospvBERReaderKeyLength);

		if (pX509PubKey)
		{
			pData	=	pX509PubKey->public_key->data;
			len		=	pX509PubKey->public_key->length;
			pRSAPubKey = d2i_RSAPublicKey(NULL, (const unsigned char **)&pData, len);

			if (pRSAPubKey)
			{
					if (OSPC_ERR_NO_ERROR == OSPPCryptoWrapDigest(digestedData,&digestedDataLength,
																												OSPC_OSNULL,0,ospvData,ospvDataLength,0))
					{
						if ( 1 == RSA_verify(type,digestedData,digestedDataLength,
																ospvSignature,ospvSignatureLength,pRSAPubKey))
						{
							errorcode = OSPC_ERR_NO_ERROR;
						}
						else
						{
							OSPM_DBGERRORLOG(errorcode, "Open-SSL error occurred in Verify");
						}
					}
					else
					{
						OSPM_DBGERRORLOG(errorcode, "Failed to calculate digest");
					}

				/* Free up mem */
				RSA_free(pRSAPubKey);
			}
			else
			{
				OSPM_DBGERRORLOG(errorcode, "Failed to init RSA key");
			}

			/* Free up mem */
			X509_PUBKEY_free(pX509PubKey);
		}
		else
		{
			OSPM_DBGERRORLOG(errorcode, "Failed to init X509_PUBKEY");
		}

    return errorcode;
}
