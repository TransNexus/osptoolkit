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
 * ospproviderapi.cpp - API functions for provider.
 */
#include "osp/osp.h"
#include "osp/ospossys.h"
#include "osp/ospproviderapi.h"
#include "osp/ospprovider.h"
#include "osp/ospcomm.h"
#include "osp/ospsecurity.h"
#include "osp/osptrans.h"
#include "osp/ospmsgque.h"
#include "osp/ospssl.h"
#include "osp/ospconfig.h"

/* 
 * OSPPProviderDelete()
 *
 * Delete the Provider Object.
 * 
 * The OSPPProviderDelete function tells the SDK library to delete a
 * provider object. This function immediately prevents the creation of
 * new transactions for the indicated provider. (Attempts to create new
 * transaction objects will be refused with an appropriate error code.).
 * The function also blocks until all pending transactions for the
 * provider have completed or the time limit has been exceeded.
 * The ospvTimeLimit parameter specifies the maximum number of seconds to
 * wait for pending transactions to complete. A negative value for this 
 * parameter instructs the library to wait indefinitely, and a value of zero
 * indicates that the deletion should occur immediately without waiting. If 
 * pending transactions are not complete within the time limit, those 
 * transactions will be terminated abruptly and information, including 
 * information necessary for billing, may be lost.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderDelete(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider object     */
    int ospvTimeLimit)              /* In - Maximum delete time */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {

        OSPPProviderSetNewTransactionAllowed(provider, OSPC_FALSE);

        if (errorcode == OSPC_ERR_NO_ERROR) {

            OSPPAuditDelete(&(provider->Audit));

            /*
             * shutdown the communication thread and destroy all
             * communication resources and message queues
             */
            OSPPCommSetShutdown(&(provider->Comm), ospvTimeLimit);
            OSPPCommSetShutdown(&(provider->CommForCapabilities), ospvTimeLimit);
            OSPPSSLSessionCleanup((void *)provider->Security);

            OSPPSecDelete(&provider->Security);

            OSPPProviderTransactionCollectionDelete(&provider->TransCollection);

            OSPPTransIDTreeDelete(provider);

            errorcode = OSPPProviderInitializeStorage(provider);
        }
    }

    return errorcode;
}

/* 
 * OSPPProviderGetAuthorityCertificates()
 *
 * Get trusted certificate authority public keys.
 *
 * The OSPPProviderGetAuthorityCertificates function returns the 
 * certificate authority public keys that are currently trusted by
 * ospvProvider. These keys are returned in the form of X.509 formatted
 * certificates, and they are returned to the ospvAuthorityCertificates
 * array. The ospvSizeOfCertificate parameter indicates the maximum size
 * of any individual certificate. If any certificate exceeds that value 
 * then no certificates are returned and an error is returned. The parameter 
 * ospvNumberOfAuthorityCertificates points to the maximum number of 
 * certificates to return. That variable is updated with the actual number 
 * supplied when the function returns. If more certificates are available,
 * then only a partial list is returned.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetAuthorityCertificates(
    OSPTPROVHANDLE ospvProvider,                    /* In     - Provider handle          */
    unsigned ospvSizeOfCertificate,                 /* In     - Max cert size            */
    unsigned *ospvNumberOfAuthorityCertificates,    /* In/Out - max to return/actual cnt */
    void *ospvAuthorityCertificates[])              /* Out    - Ptr to auth cert bufs    */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecCopyAuthorityCertificates(provider->Security,
            ospvSizeOfCertificate,
            (unsigned char **)ospvAuthorityCertificates, 
            ospvNumberOfAuthorityCertificates);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetHTTPMaxConnections()
 *
 * Get maximum number of simultaneous HTTP connections for provider.
 *
 * The OSPPProviderGetHTTPMaxConnections function returns the maximum
 * number of simultaneous HTTP connections that may be established with
 * ospvProvider. That number is returned in the variable pointed to by 
 * ospvHTTPMaxConnections.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetHTTPMaxConnections(
    OSPTPROVHANDLE ospvProvider,        /* In - Provider handle      */
    unsigned *ospvHTTPMaxConnections)   /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;

    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*
         * get the maximum number of connections from
         * the Communication Manager module.
         */
        errorcode = OSPPCommGetMaxConnections(provider->Comm, ospvHTTPMaxConnections);
    }

    return errorcode;
}

/*
 * OSPPProviderGetHTTPPersistence()
 *
 * Get the persistence, in seconds, of connections established with 
 * provider.
 *
 * The OSPPProviderGetHTTPPersistence function returns the persistence
 * of HTTP connections established with ospvProvider. That value, 
 * returned in the location pointed to by ospvHTTPPersistence, is 
 * measured in seconds.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetHTTPPersistence(
    OSPTPROVHANDLE ospvProvider,    /* In  - Provider handle     */
    unsigned *ospvHTTPPersistence)  /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommGetPersistence(provider->Comm, ospvHTTPPersistence);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetHTTPRetryDelay()
 *
 * Get delay, in seconds, between retries for connection with provider.
 *
 * The OSPPProviderGetHTTPRetryDelay function returns the delay between 
 * retries for HTTP connection attempts with ospvProvider. That value, 
 * returned in the location pointed to by ospvHTTPRetryDelay, is measured 
 * in seconds.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetHTTPRetryDelay(
    OSPTPROVHANDLE ospvProvider,    /* In  - Provider handle     */
    unsigned *ospvHTTPRetryDelay)   /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommGetRetryDelay(provider->Comm, ospvHTTPRetryDelay);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetHTTPRetryLimit()
 *
 * Get maximum number of retries for HTTP connection with provider.
 *
 * The OSPPProviderGetHTTPRetryLimit function returns the maximum 
 * number of 
 * retries for HTTP connection attempts with ospvProvider. That value is returned 
 * in the location pointed to by ospvHTTPRetryLimit.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetHTTPRetryLimit(
    OSPTPROVHANDLE ospvProvider,    /* In  - Provider handle     */
    unsigned *ospvHTTPRetryLimit)   /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommGetRetryLimit(provider->Comm, ospvHTTPRetryLimit);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetHTTPTimeout()
 *
 * Get time, in milliseconds, to wait for a response from a provider
 * HTTP connection.
 *
 * The OSPPProviderGetHTTPTimeout function returns the timeout value
 * that specifies how long to wait for responses from HTTP connections
 * with ospvProvider. The value, returned in the location pointed to by
 * ospvHTTPTimeout, is measured in milliseconds.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetHTTPTimeout(
    OSPTPROVHANDLE ospvProvider,    /* In  - Provider handle     */
    unsigned *ospvHTTPTimeout)      /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommGetTimeout(provider->Comm, ospvHTTPTimeout);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetLocalKeys()
 *
 * Get public and private key information being used by provider.
 *
 * The OSPPProviderGetLocalKeys function returns the public and 
 * private key information currently in use by ospvProvider for 
 * signing requests and indications. The RSA private key is returned 
 * in the location pointed to by ospvLocalPrivateKey, and the X.509 
 * formatted public key certificate is stored in ospvLocalCertificate. 
 * The ospvSizeOfCertificate parameter indicates the maximum size of
 * the ospvLocalCertificate array. If the certificate does not fit 
 * within that limit, the function returns an appropriate error code.  
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetLocalKeys(
    OSPTPROVHANDLE ospvProvider,            /* In  - Provider handle      */
    OSPTPRIVATEKEY *ospvLocalPrivateKey,    /* Out - Ptr to storage       */
    unsigned ospvSizeOfCertificate,         /* In  - Length of cert space */
    void *ospvLocalCertificate)             /* Out - Ptr to cert store    */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*
         * Copy the private key from the Security module
         */
        errorcode = OSPPSecCopyPrivateKey(provider->Security, (OSPTPRIVATEKEY *)ospvLocalPrivateKey);
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*
         * Copy the local certificate from the Security module 
         */
        errorcode = OSPPSecCopyLocalCertificate(provider->Security, &ospvSizeOfCertificate, (unsigned char *)ospvLocalCertificate);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetLocalValidation()
 *
 * Indicates whether to validate tokens locally or via a protocol exchange.
 *
 * The OSPPProviderGetLocalValidation function returns an indication of 
 * whether or not ospvProvider is currently set to validate authorisation 
 * tokens locally (i.e. by verifying their digital signature) or via a 
 * protocol exchange. The return value is stored in the location pointed 
 * to by ospvLocalValidation.
 *
 *  returns OSPC_ERR_NO_ERROR if successful, OSPC_ERR_XXX otherwise.
 */
int OSPPProviderGetLocalValidation(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle             */
    unsigned *ospvLocalValidation)  /* Out - Local validation indicator */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR)

        errorcode = OSPPSecGetLocalValidation(provider->Security, ospvLocalValidation);

    return errorcode;
}

/* 
 * OSPPProviderGetNumberOfAuthorityCertificates()
 *
 * Get the number of trusted certificate authority certificates.
 *
 * The OSPPProviderGetNumberOfAuthorityCertificates function returns 
 * the number of certificate authority public keys currently trusted 
 * by ospvProvider.That value is stored in the location pointed to by
 * ospvNumberOfAuthorityCertificates.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetNumberOfAuthorityCertificates(
    OSPTPROVHANDLE ospvProvider,                    /* In  - Provider handle     */
    unsigned *ospvNumberOfAuthorityCertificates)    /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecGetNumberOfAuthorityCertificates(provider->Security, ospvNumberOfAuthorityCertificates);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetNumberOfServicePoints()
 *
 * Get number of service points defined for provider.
 *
 * The OSPPProviderGetNumberOfServicePoints interface provides the number of 
 * service points currently defined for ospvProvider. The result is returned 
 * in the location pointed to by ospvNumberOfServicePoints.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetNumberOfServicePoints(
    OSPTPROVHANDLE ospvProvider,            /* In  - Provider handle     */
    unsigned *ospvNumberOfServicePoints)    /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommGetNumberOfServicePoints(provider->Comm, ospvNumberOfServicePoints);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetServicePoints()
 *
 * Get list of currently defined service points for provider.
 *
 * The OSPPProviderGetServicePoints function gives the caller the
 * list of service points currently defined for ospvProvider. The 
 * ospvNumberOfServicePoints parameter indicates the maximum number
 * of service points to include, and the ospvSizeOfServicePoint 
 * parameter indicates the maximum length of the character string 
 * (including the terminating '\0') in which service points are placed.
 * The service points themselves are stored in the character strings 
 * indicated by the ospvServicePoints array. If the number of service
 * points is less than ospvNumberOfServicePoints, then excess entries 
 * in the ospvServicePoints array are set to empty strings. If the actual
 * number is more than the parameter, then only the first 
 * ospvNumberOfServicePoints are supplied. If the string length of any 
 * particular service point is greater than ospvSizeOfServicePoint, then no
 * service points are supplied (all pointers in the ospvServicePoints array
 * are set to empty strings) and an error is returned.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetServicePoints(
    OSPTPROVHANDLE ospvProvider,        /* In  - Provider handle       */
    unsigned ospvNumberOfServicePoints, /* In  - Number of items       */
    unsigned ospvSizeOfServicePoint,    /* In  - Max size of each item */
    char *ospvServicePoints[])          /* Out - Ptr to result store   */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*
         * get the service points from the Communication Manager module.
         */
        errorcode = OSPPCommGetServicePoints(provider->Comm, ospvNumberOfServicePoints, ospvSizeOfServicePoint, ospvServicePoints);
    }

    return errorcode;
}

/* 
 * OSPPProviderGetSSLLifetime()
 *
 * Get maximum lifetime, in seconds, of SSL session keys for provider.
 *
 * The OSPPProviderGetSSLLifetime function returns the maximum lifetime
 * of SSL session keys established with ospvProvider. That lifetime, 
 * expressed in seconds, is returned in the location pointed to by 
 * ospvSSLLifetime.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderGetSSLLifetime(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle      */
    unsigned *ospvSSLLifetime)      /* Out - Ptr to result store */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvSSLLifetime = OSPPSecGetSSLLifetime(provider->Security);
    }

    return errorcode;
}

/* 
 * OSPPProviderNew()
 *
 * Create and initialize a provider object.
 *
 * The OSPPProviderNew function creates and initializes a provider object.
 * This function must be called and return without errors before any other
 * interaction with the SDK library can take place.
 *
 * The parameters passed to this function provide the initial configuration
 * information for the provider. That information consists of the following 
 * items:
 *
 * ospvNumberOfServicePoints: the number of service points included in the 
 * list referenced by the ospvServicePoints parameter.
 *
 * ospvServicePoints: a list of character strings indicating where the library
 * should send requests and indications. Each service point in the list takes
 * the form of a standard URL,and may consist of up to four components:
 *
 *  An optional indication of the protocol to be used for communicating with
 *  the service point. This release of the SDK supports both HTTP and HTTP 
 *  secured with SSL; they are indicated by "http://" and "https://" 
 *  respectively. If the protocol is not explicitly indicated, the SDK defaults
 *  to HTTP secured with SSL.
 *
 *  The Internet domain name for the service point. Raw IP addresses may also
 *  be used, provided they are enclosed in square brackets such as "[172.16.1.1]".
 *
 *  An optional TCP port number for communicating with the service point. If the
 *  port number is omitted, the SDK defaults to port 80 (for HTTP) or port 443
 *  (for HTTP secured with SSL).
 *
 *  The uniform resource identifier for requests to the service point. This 
 *  component is not optional and must be included.
 *
 * The service points are ordered in the list by decreasing preference. The SDK
 * library, therefore, attempts to contact the first service point first. Only 
 * if that attempt fails will it fall back to the second service point.
 *
 *  Examples of valid service points include:
 *      "https://service.transnexus.com/scripts/voice/osp.cmd"
 *      "service.uk.transnexus.co.uk/scripts/fax/osp.cmd"
 *      "http://[172.16.1.2]:443/scripts/video/osp.cmd"
 *
 * ospvLocalPrivateKey: the RSA private key to be used for signing messages sent
 * to the settlement service.
 *
 * ospvLocalCertificate: a X.509 formatted certificate containing the RSA public
 * key corresponding to the local private key.
 *
 * ospvNumberOfAuthorityCertificates: the number of certificate authority 
 * certificates passed in the next parameter.
 *
 * ospvAuthorityCertificates: an array of X.509 formatted certificates 
 * containing certificate authority public keys. These public keys are used to 
 * authenticate the settlement provider server during the initial SSL exchange.
 *
 * ospvLocalValidation: a Boolean value to indicate whether or not the SDK 
 * should validation authorisation tokens locally (i.e. by verifying digital 
 * signatures) or via a protocol exchange.
 *
 * ospvSSLLifetime: the lifetime, in seconds, of a single SSL session key. 
 * Once this time limit is exceeded, the SDK library will negotiate a new 
 * session key. Communication exchanges in progress will not be interrupted 
 * when this time limit expires.
 *
 * ospvHTTPMaxConnections: the maximum number of simultaneous connections 
 * to be used for communication to the settlement provider.
 *
 * ospvHTTPPersistence: the time, in seconds, that an HTTP connection should
 * be maintained after the completion of a communication exchange. The library
 * will maintain the connection for this time period in anticipation of future
 * communication exchanges to the same server.
 *
 * ospvHTTPRetryDelay: the time, in seconds, between retrying connection 
 * attempts to the provider. After exhausting all service points for the 
 * provider, the library will delay for this amount of time before resuming 
 * connection attempts.
 *
 * ospvHTTPRetryLimit: the maximum number of retries for connection attempts 
 * to the provider.If no connection is established after this many retry 
 * attempts to all service points, then the library will cease connection 
 * attempts and return appropriate error codes. 
 * This number does not count the initial connection attempt, so that an 
 * ospvHTTPRetryLimit of 1 will result in a total of two connection attempts 
 * to every service point.
 *
 * ospvHTTPTimeout: the maximum time, in milliseconds, to wait for a response
 * from a server. If no response is received within this time, the current 
 * connection is aborted and the  library attempts to contact the next service
 * point.
 * 
 * ospvProvider: pointer to variable in which to store a handle for the newly
 * created provider object. That handle must be used for all subsequent 
 * interactions with the provider.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderNew(
    unsigned ospvNumberOfServicePoints,             /* In  - Svc ptr count           */
    const char *ospvServicePoints[],                /* In  - Svc pts strings         */
    unsigned long ospvMessageCount[],               /* Array of Integers that tell the 
                                                       toolkit about the maximum messages
                                                       that can be sent to each SP on a connection */
    const char *ospvAuditURL,                       /* In  - Audit URL string         */
    const OSPTPRIVATEKEY *ospvLocalPrivateKey,      /* In  - Private key obj ptr     */
    const OSPT_CERT *ospvLocalCertificate,          /* In  - Local cert ptr          */
    unsigned ospvNumberOfAuthorityCertificates,     /* In  - Auth cert nt            */
    const OSPT_CERT *ospvAuthorityCertificates[],   /* In  - Auth certs              */
    unsigned ospvLocalValidation,                   /* In  - Local validation or not */
    unsigned ospvSSLLifetime,                       /* In  - SSL lifetime            */
    unsigned ospvHTTPMaxConnections,                /* In  - Max connections         */
    unsigned ospvHTTPPersistence,                   /* In  - Connection persistence  */
    unsigned ospvHTTPRetryDelay,                    /* In  - Retry delay             */
    unsigned ospvHTTPRetryLimit,                    /* In  - Retry limit             */
    unsigned ospvHTTPTimeout,                       /* In  - Timeout                 */
    const char *ospvCustomerId,                     /* In - Customer Id              */
    const char *ospvDeviceId,                       /* In - Device Id                */
    OSPTPROVHANDLE *ospvProvider)                   /* Out - Handle to new provider  */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned long custid = 0L, deviceid = 0L;
    unsigned i;

    if (ospvMessageCount != NULL) {
        for (i = 0; i < ospvNumberOfServicePoints; i++) {
            if (ospvMessageCount[i] < 0) {
                errorcode = OSPC_ERR_PROV_INVALID_VALUE;
                OSPM_DBGERRORLOG(errorcode, "Invalid input value for Message count");
                break;
            }
        }
    }

    /* 
     * check incoming values and reset to defaults if necessary
     */

    if ((errorcode == OSPC_ERR_NO_ERROR) && 
        ((ospvNumberOfServicePoints <= 0) || (ospvServicePoints == OSPC_OSNULL) ||
        (OSPPCommValidateSvcPts(ospvNumberOfServicePoints, ospvServicePoints) != OSPC_ERR_NO_ERROR) ||
        (ospvLocalPrivateKey == OSPC_OSNULL) || (ospvLocalCertificate == OSPC_OSNULL) ||
        (ospvNumberOfAuthorityCertificates <= 0) || (ospvAuthorityCertificates == OSPC_OSNULL) ||
        (ospvSSLLifetime < 0) || (ospvHTTPMaxConnections < 0) || (ospvHTTPPersistence < 0) ||
        (ospvHTTPRetryDelay < 0) || (ospvAuditURL == OSPC_OSNULL) || (ospvHTTPRetryLimit < 0) || (ospvHTTPTimeout < 0))) 
    {
        errorcode = OSPC_ERR_PROV_INVALID_VALUE;
        OSPM_DBGERRORLOG(errorcode, "Invalid input value");
    } else {
        /* if proper values are not set, set them to defaults */
        if (ospvSSLLifetime == 0) {
            ospvSSLLifetime = OSPC_DEFAULT_SSLLIFETIME;
        }

        if (ospvHTTPMaxConnections == 0) {
            ospvHTTPMaxConnections = OSPC_DEFAULT_HTTPMAXCONNECTIONS;
        }

        if (ospvHTTPPersistence == 0) {
            ospvHTTPPersistence = OSPC_DEFAULT_HTTPPERSISTENCE;
        }

        if (ospvHTTPRetryDelay == 0) {
            ospvHTTPRetryDelay = OSPC_DEFAULT_HTTPRETRYDELAY;
        }

        if (ospvHTTPTimeout < 1000) {
            ospvHTTPTimeout = OSPC_DEFAULT_HTTPTIMEOUT;
        }

        /*
         * get a new Provider handle
         */
        errorcode = OSPPProviderGetNewCollectionItem(ospvProvider);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /*
             * get the new Provider handle context
             */
            provider = OSPPProviderGetContext(*ospvProvider, &errorcode);

            if (errorcode == OSPC_ERR_NO_ERROR) {

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * initialize the area which holds the transactions 
                     */
                    errorcode = OSPPProviderTransactionCollectionNew(&(provider->TransCollection));
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * initialize security area
                     */
                    errorcode = OSPPSecNew(&(provider->Security));
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * initialize the Comm object
                     */
                    errorcode = OSPPCommNew(&(provider->Comm));
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * initialize the CommForCapabilities object
                     */
                    errorcode = OSPPCommNew(&(provider->CommForCapabilities));
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * set security for the Comm object
                     */
                    OSPPCommSetSecurity(provider->Comm, provider->Security);
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * set security for the Comm object
                     */
                    OSPPCommSetSecurity(provider->CommForCapabilities, provider->Security);
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * set the connection SSL Session Lifetime
                     */
                    errorcode = OSPPProviderSetSSLLifetime(*ospvProvider, ospvSSLLifetime);
                }

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * Perform any global SSL initialisation routines
                     */
                    errorcode = OSPPSSLSessionInit((void *)provider->Security);
                }
            }

            if (errorcode == OSPC_ERR_NO_ERROR)

                /*
                 * allow new transactions to be processed
                 */
                OSPPProviderSetNewTransactionAllowed(provider, OSPC_TRUE);

            /*
             * set the service points configured
             */
            errorcode = OSPPProviderSetServicePoints(*ospvProvider, ospvNumberOfServicePoints, ospvMessageCount, ospvServicePoints);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /*
                 * set the capabilities URLs configured
                 * use the same set of service points as for the main manager
                 */
                errorcode = OSPPProviderSetCapabilitiesURLs(*ospvProvider, ospvNumberOfServicePoints, ospvMessageCount, ospvServicePoints);
            }

            if (errorcode == OSPC_ERR_NO_ERROR)

                /*
                 * set the CA certs
                 */
                errorcode = OSPPProviderSetAuthorityCertificates(*ospvProvider, ospvNumberOfAuthorityCertificates, ospvAuthorityCertificates);

            if (errorcode == OSPC_ERR_NO_ERROR)

                /*
                 * set the connection HTTP Persistence
                 */
                errorcode = OSPPProviderSetHTTPPersistence(*ospvProvider, ospvHTTPPersistence);

            if (errorcode == OSPC_ERR_NO_ERROR)

                /*
                 * set the maximum number of HTTP connections 
                 */
                errorcode = OSPPProviderSetHTTPMaxConnections(*ospvProvider, ospvHTTPMaxConnections);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /* set Customer Id */
                custid = atol(ospvCustomerId);
                OSPPProviderSetCustId(provider, custid);
                /* set Device Id */
                deviceid = atol(ospvDeviceId);
                OSPPProviderSetDeviceId(provider, deviceid);
                /*
                 * set the private key. 
                 */
                errorcode = OSPPProviderSetLocalKeys(*ospvProvider, ospvLocalPrivateKey, ospvLocalCertificate->CertData);
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
                /*
                 * set the HTTP retry delay
                 */
                errorcode = OSPPProviderSetHTTPRetryDelay(*ospvProvider, ospvHTTPRetryDelay);

            if (errorcode == OSPC_ERR_NO_ERROR)
                /*
                 * set the HTTP retry limit
                 */
                errorcode = OSPPProviderSetHTTPRetryLimit(*ospvProvider, ospvHTTPRetryLimit);

            if (errorcode == OSPC_ERR_NO_ERROR)
                /*
                 * set the HTTP timeout
                 */
                errorcode = OSPPProviderSetHTTPTimeout(*ospvProvider, ospvHTTPTimeout);

            if (errorcode == OSPC_ERR_NO_ERROR)
                /*
                 * set local validation
                 */
                if (ospvLocalValidation)
                    errorcode = errorcode;  /* not implemented */

            /* 
             * set audit info 
             */
            if (errorcode == OSPC_ERR_NO_ERROR) {
                provider->Audit = OSPPAuditNew(ospvAuditURL);
                if (provider->Audit != OSPC_OSNULL) {
                    OSPPAuditSetSecurity(provider->Audit, provider->Security);
                    OSPPAuditSetComm(provider->Audit, provider->Comm);
                    OSPPCommSetAuditURL(provider->Comm, ospvAuditURL);
                }

                /* setup transactionid tree */
                OSPPTransIdInit(provider);
            }
        } else {
            /*
             ** Failed to create a new provider handle
             */
        }
    }   /* end of valid values */
    
    return errorcode;
}

/* 
 * OSPPProviderSetAuthorityCertificates()
 *
 * Set the list of authority certificates trusted by the provider.
 *
 * The OSPPProviderSetAuthorityCertificates function indicates the 
 * certificate authority public keys that should be trusted for 
 * ospvProvider. Those public keys are conveyed in the form of X.509 
 * formatted certificates. The parameter ospvNumberOfAuthorityCertificates
 * indicates how many of such certificates are conveyed in the 
 * ospvAuthorityCertificates array. Communication exchanges already in 
 * progress are not interrupted by this function, but subsequent exchanges 
 * will use the new values.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderSetAuthorityCertificates(
    OSPTPROVHANDLE ospvProvider,                    /* In - Provider handle       */
    unsigned ospvNumberOfAuthorityCertificates,     /* In - Number of auth certs  */
    const OSPT_CERT *ospvAuthorityCertificates[])   /* In - Ptr to auth cert bufs */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecSetAuthorityCertificates(provider->Security, ospvNumberOfAuthorityCertificates, ospvAuthorityCertificates);
    }

    return errorcode;
}

/* 
 * OSPPProviderSetHTTPMaxConnections()
 *
 * Set the maximum number of simultaneous HTTP connections 
 * with the provider.
 *
 * The OSPPProviderSetHTTPMaxConnections function indicates the 
 * maximum number of simultaneous HTTP connections that should be 
 * established with ospvProvider. The number is passed in the 
 * ospvHTTPMaxConnections parameter. Changes to this value  do not 
 * effect active communication exchanges but otherwise take place 
 * immediately. In particular, HTTP connections being kept alive 
 * strictly because of persistence are terminated immediately if 
 * the number of open connections must be reduced.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderSetHTTPMaxConnections(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle       */
    unsigned ospvHTTPMaxConnections)/* In - New HTTP Max Connects */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetMaxConnections(provider->Comm, ospvHTTPMaxConnections);
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetMaxConnections(provider->CommForCapabilities, ospvHTTPMaxConnections);
    }

    return errorcode;
}

/* 
 * OSPPProviderSetHTTPPersistence()
 *
 * Set the keep-alive time, in seconds, for the connection
 * with the provider.
 *
 * The OSPPProviderSetHTTPPersistence function configures the persistence
 * of HTTP connections established with ospvProvider. That lifetime, 
 * expressed in seconds, is indicated by the ospvHTTPPersistence parameter.
 * The SDK library keeps a HTTP connection alive for this number of seconds 
 * after each communication exchange, anticipating that a subsequent exchange
 * may reuse the existing connection. Changes to this parameter take place 
 * immediately.
 * 
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderSetHTTPPersistence(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle      */
    unsigned ospvHTTPPersistence)   /* In - New HTTP Persistence */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetPersistence(provider->Comm, ospvHTTPPersistence);
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetPersistence(provider->CommForCapabilities, ospvHTTPPersistence);
    }

    return errorcode;
}

/* 
 * OSPPProviderSetHTTPRetryDelay()
 *
 * Set delay between retries for connection attempts with provider.
 *
 * The OSPPProviderSetHTTPRetryDelay function configures the delay 
 * between retries for connections attempts with ospvProvider. That 
 * delay, expressed in seconds, is indicated by the ospvHTTPRetryDelay
 * parameter. After exhausting all service points for the provider, the
 * library will delay for this amount of time before resuming 
 * connection attempts. Changes to this parameter take place immediately.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderSetHTTPRetryDelay(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle      */
    unsigned ospvHTTPRetryDelay)    /* In - New HTTP retry delay */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetRetryDelay(provider->Comm, ospvHTTPRetryDelay);
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetRetryDelay(provider->CommForCapabilities, ospvHTTPRetryDelay);
    }

    return errorcode;
}

/* 
 * OSPPProviderSetHTTPRetryLimit()
 *
 * Set maximum number of retries for connection attempts to provider.
 *
 * The OSPPProviderSetHTTPRetryLimit function configures the maximum 
 * number of retries for connections attempts with ospvProvider. If no 
 * connection is established after this many retry attempts to all service
 * points, then the library will cease connection attempts and return 
 * appropriate error codes. This number does not count the initial 
 * connection attempt, so that an ospvHTTPRetryLimit of 1 will result 
 * in a total of two connection attempts to every service point.
 * 
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderSetHTTPRetryLimit(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle      */
    unsigned ospvHTTPRetryLimit)    /* In - New HTTP retry limit */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetRetryLimit(provider->Comm, ospvHTTPRetryLimit);
    }
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetRetryLimit(provider->CommForCapabilities, ospvHTTPRetryLimit);
    }

    return errorcode;
}

/* 
 * OSPPProviderSetHTTPTimeout()
 *
 * Configure maximum time in ms to wait for HTTP reply from provider.
 *
 * The OSPPProviderSetHTTPTimeout function configures the maximum 
 * amount of time to wait for a reply from ospvProvider. That timeout,
 * expressed in milliseconds, is indicated by the ospvHTTPTimeout 
 * parameter. If no response arrives within this time, the current 
 * connection is aborted and the library attempts to connect with another
 * service point. Changes to this parameter do not affect connection 
 * attempts already in progress, but take affect for all subsequent attempts.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderSetHTTPTimeout(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle  */
    unsigned ospvHTTPTimeout)       /* In - New HTTP Timeout */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetTimeout(provider->Comm, ospvHTTPTimeout);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
        errorcode = OSPPCommSetTimeout(provider->CommForCapabilities, ospvHTTPTimeout);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetConnSelectionTimeout(provider->Comm, ospvHTTPTimeout);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPCommSetConnSelectionTimeout(provider->CommForCapabilities, ospvHTTPTimeout);
    }

    return errorcode;
}

/* 
 * OSPPProviderSetLocalKeys()
 *
 * Set public and private keys provider uses to sign requests 
 * and indications.
 *
 * The OSPPProviderSetLocalKeys function configures the public and 
 * private key pair used by ospvProvider to sign its requests and 
 * indications. The parameter ospvLocalPrivateKey identifies the RSA 
 * private key and the parameter ospvLocalCertificate points to a X.509
 * formatted certificate containing the corresponding RSA public key.
 * Communication exchanges already in progress are not interrupted by 
 * this function, but subsequent exchanges will use the new values.
 * 
 * Returns: OSPC_ERR_NO_ERROR if successful, OSP_ERR_xxx otherwise.
 */
int OSPPProviderSetLocalKeys(
    OSPTPROVHANDLE ospvProvider,                /* In - Provider handle       */
    const OSPTPRIVATEKEY *ospvLocalPrivateKey,  /* In - New Local Private Key */
    const void *ospvLocalCertificate)           /* In - New Local Certificate */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned long custId = 0;
    unsigned long devId = 0;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecSetPrivateKey(provider->Security, (OSPTPRIVATEKEY *)ospvLocalPrivateKey);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            errorcode = OSPPSecSetLocalCertificate(provider->Security, (unsigned char *)ospvLocalCertificate, &custId, &devId);
            if ((errorcode == OSPC_ERR_NO_ERROR) && (custId != 0) && (devId != 0)) {
                /* set Customer Id */
                OSPPProviderSetCustId(provider, custId);

                /* set Device Id */
                OSPPProviderSetDeviceId(provider, devId);
            }
        }
    }

    return errorcode;
}

/*
 * OSPPProviderSetLocalValidation()
 *
 * Set local validation on or off.
 *
 * The OSPPProviderSetLocalValidation funcitonindicates to opsvProvider
 * whether authorisation tokens should be validated locally (i.e. by 
 * verifying their digital signature) or via a protocol exchange. The 
 * parameter ospvLocalValidation is non-zero for local validation or zero 
 * for remote validation.
 *
 * returns OSPC_ERR_NO_ERROR if successful, error code otherwise.
 */
int OSPPProviderSetLocalValidation(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider handle             */
    unsigned ospvLocalValidation)   /* In - Local validation indicator  */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecSetLocalValidation(provider->Security, ospvLocalValidation);
    }

    return errorcode;
}

/*
 * OSPPProviderSetSPMessageCount()
 * Sets the maximum message count for each SP configured
 */
int OSPPProviderSetSPMessageCount(
    void *ospvcomm,                     /* In - Comm Manager handle    */
    unsigned long ospvMessageCount[])   /* In - Message count for each SP */
{
    OSPTSVCPT *svcptlist = OSPC_OSNULL, *svcptitem = OSPC_OSNULL;
    int i = 0, errorcode = OSPC_ERR_NO_ERROR;
    OSPTCOMM *comm = (OSPTCOMM *)ospvcomm;

    /*
     * get a pointer to the service point list
     */
    OSPPCommGetServicePointList(comm, &svcptlist);

    svcptitem = (OSPTSVCPT *)OSPPListFirst((OSPTLIST *)& svcptlist);

    if (ospvMessageCount != NULL) {
        while (svcptitem != OSPC_OSNULL) {
            svcptitem->MaxMsgAllowed = ospvMessageCount[i++];
            svcptitem = (OSPTSVCPT *)OSPPListNext((OSPTLIST *)& svcptlist, svcptitem);
        }
    } else {
        while (svcptitem != OSPC_OSNULL) {
            svcptitem->MaxMsgAllowed = 0;
            svcptitem = (OSPTSVCPT *)OSPPListNext((OSPTLIST *)& svcptlist, svcptitem);
        }
    }

    return errorcode;
}

/* 
 * OSPPProviderSetServicePoints()
 *
 * Set service point configuration for future communications exchanges. 
 *
 * The OSPPProviderSetServicePoints function indicates the service points 
 * the SDK library should use for future communication exchanges with
 * ospvProvider. Communication exchanges already in progress are not
 * interrupted.
 *
 * The format for the ospvServicePoints parameter is the same as in the
 * OSPPProviderNew function call. In particular, it is a list of service
 * points, each in the form of a standard URL. A service point URL may consist
 * of up to four components:
 *
 * An optional indication of the protocol to be used for communicating
 * with the service point. This release of the SDK supports both HTTP and 
 * with the service point. This release of the SDK supports both HTTP and 
 * HTTP secured with SSL; they are indicated by "http://" and "https://" 
 * respectively. If the protocol is not explicitly indicated, the SDK 
 * defaults to HTTP secured with SSL.
 *
 * The Internet domain name for the service point. Raw IP addresses 
 * may also be used, provided they are enclosed in square brackets such as 
 * "[172.16.1.1]".
 *
 * An optional TCP port number for communicating with the service 
 * point. If the port number is omitted, the SDK defaults to port 80 
 * (for HTTP) or port 443 (for HTTP secured with SSL).
 *
 * The uniform resource identifier for requests to the service point.  
 * This component is not optional and must be included.
 *
 * The service points are ordered in the list by decreasing preference. The SDK 
 * library, therefore, attempts to contact the first service point 
 * first.  Only if that attempt fails will it fall back to the second 
 *      service point.
 *
 *      Examples of valid service points include:
 *
 *          "https://service.transnexus.com/scripts/voice/osp.cmd"
 *              "service.uk.transnexus.co.uk/scripts/fax/osp.cmd"
 *              "http://[172.16.1.2]:443/scripts/video/osp.cmd"
 *
 *      Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 *
 */
int OSPPProviderSetServicePoints(
    OSPTPROVHANDLE ospvProvider,        /* In - Provider handle    */
    unsigned ospvNumberOfServicePoints, /* In - New svc pt cnt     */
    unsigned long ospvMessageCount[],   /* In - Message cnt     */
    const char *ospvServicePoints[])    /* In - New svc pt strings */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (OSPC_ERR_NO_ERROR == errorcode) {
        provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    }

    if (OSPC_ERR_NO_ERROR == errorcode) {
        errorcode = OSPPCommUpdateURLs(provider->Comm, ospvNumberOfServicePoints, ospvServicePoints);
    }

    /*
     * set message counts for all SP
     */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPProviderSetSPMessageCount((void *)provider->Comm, ospvMessageCount);
    }

    return errorcode;
}

/*
 * 
 * Save as OSPPProviderSetServicePoints only updates the set of URLs used
 * for exchanging capabilities messages.
 *
 */
int OSPPProviderSetCapabilitiesURLs(
    OSPTPROVHANDLE ospvProvider,        /* In - Provider handle     */
    unsigned ospvNumberOfURLs,          /* In - New svc url cnt     */
    unsigned long ospvMessageCount[],   /* In - Msg count for URL */
    const char *ospvCapabilitiesURLs[]) /* In - New svc url strings */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (OSPC_ERR_NO_ERROR == errorcode) {
        provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    }

    if (OSPC_ERR_NO_ERROR == errorcode) {
        errorcode = OSPPCommUpdateURLs(provider->CommForCapabilities, ospvNumberOfURLs, ospvCapabilitiesURLs);
    }

    /*
     * set message counts for all Comm SP
     */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPProviderSetSPMessageCount((void *)provider->CommForCapabilities, ospvMessageCount);
    }

    return errorcode;
}

/* 
 * OSPPProviderSetSSLLifetime()
 *
 * Set the maximum lifetime of SSL session keys with provider.
 *
 * The OSPPProviderSetSSLLifetime function configures the maximum 
 * lifetime of SSL session keys established with ospvProvider. That 
 * lifetime, expressed in seconds, is indicated by the ospvSSLLifetime
 * parameter. The SDK library attempts to reuse previously established 
 * SSL session keys in order to minimize delay when communicating 
 * with a settlement server. This parameter places a maximum lifetime on 
 * these keys. Changes to this parameter take place immediately. Note, 
 * however, that communication  exchanges already in progress are never 
 * interrupted when a session key lifetime expires.
 *
 * Returns: OSPC_ERR_NO_ERROR if successful, OSPC_ERR_xxx otherwise.
 */
int OSPPProviderSetSSLLifetime(
    OSPTPROVHANDLE ospvProvider,/* In - Provider handle  */
    unsigned ospvSSLLifetime)   /* In - New SSL Lifetime */
{
    OSPTPROVIDER *provider = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPSecSetSSLLifetime(provider->Security, ospvSSLLifetime);
    }

    return errorcode;
}
