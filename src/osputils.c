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

/* osputils.h - Utility functions. */

#include "osp/osp.h"
#include "osp/osputils.h"
#include "osp/osplist.h"
#include "openssl/bio.h"
#include "openssl/pem.h"
#include "openssl/evp.h"

/* Build a string given a uint64 and an int.
 */
int OSPPUtilBuildString(
    OSPTUINT64 ospv64,
    int ospvInt,
    char **ospvNumberString)
{
    char returnstring[50];
    char *strptr = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MEMSET(returnstring, 0, sizeof(returnstring));

    /*
     * So we don't have to worry about the size of unsigned longs on
     * the system, we work backwards.
     */

    /* First the integer */
    strptr = &returnstring[sizeof(returnstring) - 2];
    do {
        *strptr-- = (char) ('0' + (ospvInt % 10));
        ospvInt = ospvInt / 10;
    } while ((ospvInt != 0) && (strptr >= &returnstring[0]));

    if (ospvInt != 0) {
        /* error - we ran out of space before completing the number */
        errorcode = OSPC_ERR_DATA_BAD_NUMBER;
    }

    /* Now the uint64 */
    do {
        *strptr-- = (char) ('0' + (ospv64 % 10));
        ospv64 = ospv64 / 10;
    } while ((ospv64 != 0) && (strptr >= &returnstring[0]));

    if (ospv64 != 0) {
        /* error - we ran out of space before completing the number */
        errorcode = OSPC_ERR_DATA_BAD_NUMBER;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        strptr++;   /*get back to beginning of string */
        OSPM_MALLOC(*ospvNumberString, char, OSPM_STRLEN(strptr) + 1);
        OSPM_MEMSET(*ospvNumberString, 0, OSPM_STRLEN(strptr) + 1);
        OSPM_MEMCPY(*ospvNumberString, strptr, OSPM_STRLEN(strptr));
    }

    return errorcode;
}

/* Get the proper errorcode based on the status code returned from the server.
 */
int OSPPUtilGetErrorFromStatus(
    unsigned ospvStatusCode)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvStatusCode != OSPC_ERR_NO_ERROR) {
        switch (ospvStatusCode) {
        case 400:
            errorcode = OSPC_ERR_TRAN_BAD_REQUEST;
            break;
        case 401:
            errorcode = OSPC_ERR_TRAN_UNAUTHORIZED;
            break;
        case 403:
            errorcode = OSPC_ERR_TRAN_ROUTE_BLOCKED;
            break;
        case 404:
            errorcode = OSPC_ERR_TRAN_ROUTE_NOT_FOUND;
            break;
        case 405:
            errorcode = OSPC_ERR_TRAN_MAY_NOT_ORIGINATE;
            break;
        case 410:
            errorcode = OSPC_ERR_TRAN_CHAR_ENC_NOT_SUPD;
            break;
        case 411:
            errorcode = OSPC_ERR_TRAN_PARS_UNSUCCESSFUL;
            break;
        case 412:
            errorcode = OSPC_ERR_TRAN_UNSUPD_CRIT_ELEM;
            break;
        case 420:
            errorcode = OSPC_ERR_TRAN_SECURITY_PROBLEM;
            break;
        case 421:
            errorcode = OSPC_ERR_TRAN_SIG_INVALID;
            break;
        case 422:
            errorcode = OSPC_ERR_TRAN_CRYPTO_ALG_NOT_SUPD;
            break;
        case 423:
            errorcode = OSPC_ERR_TRAN_CERT_INVALID;
            break;
        case 424:
            errorcode = OSPC_ERR_TRAN_CERT_REVOKED;
            break;
        case 425:
            errorcode = OSPC_ERR_TRAN_ENCRYPTION_REQD;
            break;
        case 428:
            errorcode = OSPC_ERR_TRAN_CALLING_INVALID;
            break;
        case 441:
            errorcode = OSPC_ERR_TRAN_CALL_RATE_EXCEEDED;
            break;
        case 484:
            errorcode = OSPC_ERR_TRAN_CALLED_FILTERING;
            break;
        case 489:
            errorcode = OSPC_ERR_TRAN_DESTINATION_INFO_INVALID;
            break;
        case 500:
            errorcode = OSPC_ERR_TRAN_INTERNAL_SRVR_ERR;
            break;
        case 501:
            errorcode = OSPC_ERR_TRAN_UNIMPLEMENTED;
            break;
        case 503:
            errorcode = OSPC_ERR_TRAN_SERVICE_NOT_AVAIL;
            break;
        case 510:
            errorcode = OSPC_ERR_TRAN_TRANSIENT_SRVR_PROB;
            break;
        case 520:
            errorcode = OSPC_ERR_TRAN_LONG_TERM_SRVR_PROB;
            break;
        case 530:
            errorcode = OSPC_ERR_TRAN_TIME_PROB;
            break;
        case 531:
            errorcode = OSPC_ERR_TRAN_VALID_TIME_TOO_SOON;
            break;
        case 532:
            errorcode = OSPC_ERR_TRAN_TIME_INTERVAL_TOO_SMALL;
            break;
        case 603:
            errorcode = OSPC_ERR_TRAN_DECLINE;
            break;
        case 9603:
            errorcode = OSPC_ERR_TRAN_NETWORK_BLOCKED;
            break;
        case 999:
            errorcode = OSPC_ERR_TRAN_GENERIC_FAILURE;
            break;
        default:
            if ((ospvStatusCode >= 400) && (ospvStatusCode < 500)) {
                errorcode = OSPC_ERR_TRAN_CLIENT_ERROR;
            } else if ((ospvStatusCode >= 500) && (ospvStatusCode < 600)) {
                errorcode = OSPC_ERR_TRAN_SERVER_ERROR;
            } else if ((ospvStatusCode >= 600) && (ospvStatusCode < 700)) {
                errorcode = OSPC_ERR_TRAN_GLOBAL_ERROR;
            } else {
                errorcode = OSPC_ERR_TRAN_GENERIC_FAILURE;
            }
            break;
        }
    }

    return errorcode;
}

OSPTBOOL OSPPUtilIsDottedNumericIP(
    const char *szStr)
{

    const char *pTmp = szStr;

    while (*pTmp) {
        if (*pTmp != '.' && (*pTmp < '0' || *pTmp > '9'))
            return OSPC_FALSE;
        pTmp++;
    }
    return OSPC_TRUE;
}

/* OSPPUtilGetRandom - gets a decimal number string for the random attribute.
 * ****NOTE****
 * Need to get a better way of obtaining a cryptographically strong random number
 */
int OSPPUtilGetRandom(
    char *ospvRandom,
    int ospvAddValue)
{
    int randnum = 0;
    int numchars = 0;
    unsigned int seed = time(NULL) + ospvAddValue;

#ifdef _WIN32

    srand(seed);
    randnum = rand();

#else

    randnum = rand_r(&seed);

#endif

    if (randnum > 0) {
        numchars = sprintf(ospvRandom, "%u", (unsigned) randnum);
    }

    return numchars;
}

/*
 * Converts a string to all lowercase characters
 */
void OSPPUtilStringToLowercase(
    char **ospvString)
{
    int ospvIndex = 0;

    if (*ospvString != OSPC_OSNULL) {
        for (ospvIndex = 0; ((unsigned int)ospvIndex) < OSPM_STRLEN((const char *)*ospvString); ospvIndex++) {
            *(*ospvString + ospvIndex) = (char)OSPM_TOLOWER((unsigned int) *(*ospvString + ospvIndex));
        }
    }
}

/*
 * Loads the Private Key
 */
int OSPPUtilLoadPEMPrivateKey(
    unsigned char *FileName,
    OSPTPRIVATEKEY *key)
{
    BIO *bioIn = NULL;
    RSA *rsaKey = NULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    /* The i2d function will allocate memory */
    key->PrivateKeyData = NULL;

    bioIn = BIO_new_file((const char *)FileName, "r");

    if (bioIn == NULL) {
        errorcode = OSPC_ERR_CRYPTO_FILE_OPEN_ERROR;
        OSPM_DBGERRORLOG(errorcode, "Failed to open PEM key file");
    } else {
        rsaKey = PEM_read_bio_RSAPrivateKey(bioIn, NULL, NULL, NULL);
        if (rsaKey == NULL) {
            errorcode = OSPC_ERR_CRYPTO_FILE_LOAD_ERROR;
            OSPM_DBGERRORLOG(errorcode, "Failed to load PEM key from file");
        } else {
            key->PrivateKeyLength = i2d_RSAPrivateKey(rsaKey, &(key->PrivateKeyData));

            if (key->PrivateKeyLength == 0) {
                errorcode = OSPC_ERR_CRYPTO_FILE_PARSE;
                OSPM_DBGERRORLOG(errorcode, "Failed to parse PEM key from file");
            }
        }
    }
    if (bioIn != NULL) {
        BIO_free(bioIn);
    }

    if (rsaKey != NULL) {
        RSA_free(rsaKey);
    }

    return errorcode;
}

/*
 * Loads the Certificate
 */
int OSPPUtilLoadPEMCert(
    unsigned char *FileName,
    OSPT_CERT *cert)
{
    BIO *bioIn = NULL;
    X509 *x509cert = NULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    /* The i2d function will allocate memory */
    cert->CertData = NULL;

    bioIn = BIO_new_file((const char *)FileName, "r");

    if (bioIn == NULL) {
        errorcode = OSPC_ERR_CRYPTO_FILE_OPEN_ERROR;
    } else {
        x509cert = PEM_read_bio_X509(bioIn, NULL, NULL, NULL);
        if (x509cert == NULL) {
            errorcode = OSPC_ERR_CRYPTO_FILE_LOAD_ERROR;
            OSPM_DBGERRORLOG(errorcode, "Failed to load PEM Cert from file");
        } else {
            cert->CertDataLength = i2d_X509(x509cert, &(cert->CertData));

            if (cert->CertDataLength == 0) {
                errorcode = OSPC_ERR_CRYPTO_FILE_PARSE;
                OSPM_DBGERRORLOG(errorcode, "Failed to parse PEM Certificate from file");
            }
        }
    }

    if (bioIn != NULL) {
        BIO_free(bioIn);
    }

    if (x509cert != NULL) {
        X509_free(x509cert);
    }

    return errorcode;
}
