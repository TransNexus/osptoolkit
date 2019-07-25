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

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "osp/osp.h"
#include "osp/ospproviderapi.h"
#include "osp/ospprovider.h"
#include "osp/osppkcs1.h"
#include "osp/osptransapi.h"
#include "osp/ospfail.h"
#include "osp/osptnlog.h"
#include "osp/ospasn1.h"
#include "osp/ospsecurity.h"
#include "osp/osppkcs8.h"
#include "osp/ospx509.h"
#include "osp/osptrans.h"
#include "osp/osputils.h"
#include "nonblocking.h"

#ifdef  WIN32
#define _Open       _open
#define _Read       _read
#define _Close      _close
#define _Lseek      _lseek
#define _Strdup     _strdup
#else
#define _Open       open
#define _Read       read
#define _Close      close
#define _Lseek      lseek
#define _Strdup     strdup
#define O_BINARY    (0)
#endif

#define LOCAL_VALIDATION        1
#define DEF_SSL_LIFETIME        300
#define DEF_HTTP_PERSIST        60000
#define DEF_HTTP_RETRYDELAY     0
#define DEF_HTTP_RETRYLIMIT     1
#define DEF_HTTP_TIMEOUT        (60 * 1000)
#define DEF_TIME_LIMIT          4
#define DEF_CUST_ID             1000L
#define DEF_DEVICE_ID           1000L

#define CALL_ID_SZ              256
#define CALLING_NUM_SZ          256
#define CALLED_NUM_SZ           256
#define DESTINATION_SZ          64
#define TOKEN_SZ                2096*8

/*
 * WARNING: BEFORE CHANGING THE VALUE OF NUM_CALL_IDS, BE SURE
 *   TO MODIFY THE FUNCTION testInitializeCallIds SO THAT
 *   THE NUMBER OF ELEMENTS IN THE ARRAYS MATCH THIS VALUE.
 */
#define NUM_CALL_IDS            8

#define TIMESTAMP_SZ            32

#define SVC_PT_SZ               262
#define CERT_SZ                 1024
#define PKEY_SZ                 1024

/*
 * request info globals
 */
static char validafter[TIMESTAMP_SZ], validuntil[TIMESTAMP_SZ];
static unsigned timelimit = 0;
static unsigned callidsize = CALL_ID_SZ;
static void *callid = NULL;
static unsigned char ret_cid[CALL_ID_SZ];
static unsigned char c_id[CALL_ID_SZ + 1] = { "1234567890123456" };
static char callednumber[CALLED_NUM_SZ];
static char callingnumber[CALLING_NUM_SZ];
static char dest[DESTINATION_SZ] = { "" };
static char destdev[DESTINATION_SZ] = { "" };
static unsigned tokensize = TOKEN_SZ;
static char c_token[TOKEN_SZ] = { "" };
static void *token = NULL;
static const char *New_ServicePoint = { "http://osptestserver.transnexus.com:5045/osp" };
static OSPT_CALL_ID *callids[NUM_CALL_IDS];
static token_algo_t tokenalgo = TOKEN_ALGO_SIGNED;
static char *SourceIP = NULL, *SourceDevIP = NULL, *DestIP = NULL, *DestDevIP = NULL;
static char *ModifiedSourceIP = NULL, *ModifiedSourceDevIP = NULL, *ModifiedDestIP = NULL, *ModifiedDestDevIP = NULL;
static unsigned almostOutOfResources = 0;
static unsigned hardwareSupport = 0;
static unsigned TCcode = 0;
static unsigned duration = 30;
static time_t call_start_time = 0;
static time_t call_end_time = 0;
static time_t call_alert_time = 0;
static time_t call_connect_time = 0;
static NBMONITOR *nbMonitor = NULL;
static int WORK_THREAD_NUM = 30;        /* make sure that this number does not exceed DEF_HTTP_MAXCONN */
static OSPE_RELEASE release = OSPC_RELEASE_UNKNOWN;

#define TOKEN_SIZE          2000
#define TEST_ERROR          1
#define MAX_QUEUE_SIZE      10000

static int TEST_NUM = 0;
static OSPE_NUMBER_FORMAT CallingNumFormat, CalledNumFormat;
static unsigned long SPMsgCount[50];
static unsigned long *MsgCount = SPMsgCount;
static int DEF_HTTP_MAXCONN = 30;
static unsigned IS_PDD_INFO_AVAILABLE = 0;
static unsigned long CapSPMsgCount[50];
static unsigned long *CapMsgCount = CapSPMsgCount;

/*
 * service point globals
 */
#define CONFIG_FILENAME     "test.cfg"
#define MAX_SERVICE_POINTS  10

static char *auditurl = NULL;
static char *servicepoints[MAX_SERVICE_POINTS];
static char *capURLs[MAX_SERVICE_POINTS];

static int num_serv_points = 0;
static int num_capURLs = 0;

static long custid = DEF_CUST_ID;
static long devid = DEF_DEVICE_ID;

/*
 * certificate globals
 */
static unsigned NUM_CA_CERTS = 0;

static int quietmode = 0;
static unsigned numdestinations = NUM_CALL_IDS;
static int num_test_calls = 500;

/* Local prototypes */
int testTestCalls(void);
OSPTTHREADRETURN testNonBlockingPerformanceTest(void *);
int testNonBlockingPerformanceTestForCapabilities();

/*
 * accumulate table
 */
typedef struct _ACCUM {
    unsigned Number;
    unsigned Min;
    unsigned Mean;
    float Variance;
} ACCUM;

static ACCUM accumtable10[10] = {
    { 1, 3335, 3335, 0.0f },
    { 1, 822, 822, 0.0f },
    { 1, 268, 268, 0.0f },
    { 1, 524, 524, 0.0f },
    { 1, 97, 97, 0.0f },
    { 1, 874, 874, 0.0f },
    { 1, 171, 171, 0.0f },
    { 1, 595, 595, 0.0f },
    { 1, 1196, 1196, 0.0f },
    { 1, 958, 958, 0.0f }
};

static ACCUM accumtable2[2] = {
    { 10, 6, 718, 485206.00f },
    { 20, 1, 141, 39733.00f }
};

/*
 * global handles
 */
OSPTPROVHANDLE OSPVProviderHandle = 0;
OSPTTRANHANDLE OSPVTransactionHandle = OSPC_TRAN_HANDLE_INVALID;
OSPTTRANHANDLE tranhandle2 = OSPC_TRAN_HANDLE_INVALID;

int testNotImplemented()
{
    printf("not implemented\n");
    return 0;
}

int testOSPPSetServicePoints()
{
    int errcode = 0;
    const char **servpts;

    servpts = &New_ServicePoint;

    errcode = OSPPProviderSetServicePoints(OSPVProviderHandle, 1, MsgCount, servpts);

    return errcode;
}

int testOSPPProviderNew(OSPTPROVHANDLE *ProvHandle)
{
    unsigned int i;
    int errcode = 0;
    const char **servpts;

    char customer_id[64];
    char device_id[64];
    char searchstr[20];
    OSPT_CERT localcert;
    OSPTPRIVATEKEY privatekey;
    OSPT_CERT *authCerts[OSPC_SEC_MAX_AUTH_CERTS];
    OSPT_CERT TheAuthCert[OSPC_SEC_MAX_AUTH_CERTS];

    errcode = OSPPUtilLoadPEMPrivateKey((unsigned char *)"pkey.pem", &privatekey);
    if (errcode != OSPC_ERR_NO_ERROR) {
        return errcode;
    }
    errcode = OSPPUtilLoadPEMCert((unsigned char *)"localcert.pem", &localcert);
    if (errcode != OSPC_ERR_NO_ERROR) {
        return errcode;
    }
    i = 0;
    while (i < OSPC_SEC_MAX_AUTH_CERTS) {
        sprintf(searchstr, "cacert_%d.pem", i);
        errcode = OSPPUtilLoadPEMCert((unsigned char *)searchstr, &(TheAuthCert[i]));
        if (errcode == OSPC_ERR_NO_ERROR) {
            authCerts[i] = &(TheAuthCert[i]);
            i++;
            printf("Loaded %d Authorization Certificate\n", i);
        } else {
            if (errcode == OSPC_ERR_CRYPTO_FILE_OPEN_ERROR) {
                /*
                 * If i!=0 then we have read at least one cacert.
                 * No problem in that case.
                 * Otherwise return an error
                 */
                if (i == 0) {
                    printf("Failed to find the File - %s\n", searchstr);
                    return errcode;
                } else {
                    /*
                     * Break out of thew loop
                     */
                    printf("There are no more cert files\n");
                    break;
                }
            } else {
                return errcode;
            }
        }
    }
    NUM_CA_CERTS = i;

    printf("Number of cert files %d\n", NUM_CA_CERTS);

    sprintf(customer_id, "%ld", custid);
    sprintf(device_id, "%ld", devid);

    servpts = (const char **)servicepoints;
    errcode = OSPPProviderNew(num_serv_points,
        servpts,
        MsgCount,
        auditurl,
        (const OSPTPRIVATEKEY *)&privatekey,
        /* Local certificate */
        &localcert,
        /* CA certificates */
        NUM_CA_CERTS,
        (const OSPT_CERT **)authCerts,
        /**/
        LOCAL_VALIDATION,
        DEF_SSL_LIFETIME,
        DEF_HTTP_MAXCONN,
        DEF_HTTP_PERSIST,
        DEF_HTTP_RETRYDELAY,
        DEF_HTTP_RETRYLIMIT,
        DEF_HTTP_TIMEOUT,
        customer_id,
        device_id,
        ProvHandle);

    /* Free memory allocated while loading crypto information from PEM-encoded files */
    if (privatekey.PrivateKeyData != NULL) {
        free(privatekey.PrivateKeyData);
    }

    if (localcert.CertData != NULL) {
        free(localcert.CertData);
    }

    for (i = 0; i < NUM_CA_CERTS; i++) {
        if (TheAuthCert[i].CertData != NULL) {
            free(TheAuthCert[i].CertData);
        }
    }

    return errcode;
}

int testOSPPProviderDelete()
{
    int errcode = 0;

    errcode = OSPPProviderDelete(OSPVProviderHandle, DEF_TIME_LIMIT);

    NUM_CA_CERTS = 0;

    return errcode;
}

int testOSPPProviderGetAuthorityCertificates()
{
    int errcode = 0;
    void *certs[100];
    unsigned int i;
    unsigned int certCount = 0;
    char msg[100] = "";
    OSPTASN1ELEMENTINFO *eInfo;

    OSPM_MEMSET(&eInfo, 0, sizeof(eInfo));

    errcode = OSPPProviderGetNumberOfAuthorityCertificates(OSPVProviderHandle, &certCount);

    if (certCount != NUM_CA_CERTS) {
        printf("Count Mismatch: Authority Certificates Loaded: %d, Authority Certificates Stored: %d\n", NUM_CA_CERTS, certCount);

    } else {
        printf("Authority Certificates Loaded/Stored: %d\n", certCount);
    }

    for (i = 0; i < certCount; i++) {
        OSPM_MALLOC(certs[i], void, CERT_SZ);
        if (!certs[i])
            return -1;
    }

    errcode = OSPPProviderGetAuthorityCertificates(OSPVProviderHandle, CERT_SZ, &certCount, certs);

    for (i = 0; i < certCount; i++) {
        errcode = OSPPASN1ElementDecode((unsigned char *)certs[i], &eInfo, 0);

        sprintf(msg, "Authority Certificate #%02d of %02d (%d bytes)", i + 1, certCount, eInfo->ElementLength);
        OSPTNLOGDUMP(eInfo->Element, eInfo->ElementLength, msg);
        OSPM_FREE(certs[i]);
        printf("%s dumped to test_app.log\n", msg);
    }

    OSPPASN1ElementDelete(&eInfo, 0);

    return errcode;
}

int testOSPPProviderSetAuthorityCertificates()
{
    int errcode = 0;
    unsigned int i;
    char searchstr[20];
    OSPT_CERT *authCerts[OSPC_SEC_MAX_AUTH_CERTS];
    OSPT_CERT TheAuthCert[OSPC_SEC_MAX_AUTH_CERTS];

    i = 0;
    while (i < OSPC_SEC_MAX_AUTH_CERTS) {
        sprintf(searchstr, "cacert_%d.pem", i);
        errcode = OSPPUtilLoadPEMCert((unsigned char *)searchstr, &(TheAuthCert[i]));
        authCerts[i] = &(TheAuthCert[i]);
        if (errcode == OSPC_ERR_NO_ERROR) {
            i++;
            printf("Read %d Authorization Certificate\n", i);
        } else {
            if (errcode == OSPC_ERR_CRYPTO_FILE_OPEN_ERROR) {
                /*
                 * If i!=0 then we have read at least one cacert.
                 * No problem in that case.
                 * Otherwise return an error
                 */
                if (i == 0) {
                    printf("Failed to find the File - %s\n", searchstr);
                    return errcode;
                } else {
                    /*
                     * Break out of thew loop
                     */
                    break;
                }
            } else {
                return errcode;
            }
        }
    }
    NUM_CA_CERTS = i;

    errcode = OSPPProviderSetAuthorityCertificates(OSPVProviderHandle, NUM_CA_CERTS, (const OSPT_CERT **)authCerts);

    /* Free memory allocated while loading crypto information from PEM-encoded files */
    for (i = 0; i < NUM_CA_CERTS; i++) {
        if (TheAuthCert[i].CertData != NULL) {
            free(TheAuthCert[i].CertData);
        }
    }
    return errcode;
}

int testOSPPProviderGetHTTPMaxConnections()
{
    int errcode = 0;
    unsigned maxconns;

    errcode = OSPPProviderGetHTTPMaxConnections(OSPVProviderHandle, &maxconns);

    printf("max conns = %u\n", maxconns);
    return errcode;
}

int testOSPPProviderSetHTTPMaxConnections()
{
    int errcode = 0;

    errcode = OSPPProviderSetHTTPMaxConnections(OSPVProviderHandle, DEF_HTTP_MAXCONN);

    return errcode;
}

int testOSPPProviderGetHTTPPersistence()
{
    int errcode = 0;
    unsigned persist;

    errcode = OSPPProviderGetHTTPPersistence(OSPVProviderHandle, &persist);

    printf("http persistence = %u\n", persist);

    return errcode;
}

int testOSPPProviderSetHTTPPersistence()
{
    int errcode = 0;

    errcode = OSPPProviderSetHTTPPersistence(OSPVProviderHandle, DEF_HTTP_PERSIST);

    return errcode;
}

int testOSPPProviderGetHTTPRetryDelay()
{
    int errcode = 0;
    unsigned retrydelay;

    errcode = OSPPProviderGetHTTPRetryDelay(OSPVProviderHandle, &retrydelay);

    printf("http retry delay = %u\n", retrydelay);
    return errcode;
}

int testOSPPProviderSetHTTPRetryDelay()
{
    int errcode = 0;

    errcode = OSPPProviderSetHTTPRetryDelay(OSPVProviderHandle, DEF_HTTP_RETRYDELAY);

    return errcode;
}

int testOSPPProviderGetHTTPTimeout()
{
    int errcode = 0;
    unsigned timeout;

    errcode = OSPPProviderGetHTTPTimeout(OSPVProviderHandle, &timeout);

    printf("http timeout = %u\n", timeout);
    return errcode;
}

int testOSPPProviderSetHTTPTimeout()
{
    int errcode = 0;

    errcode = OSPPProviderSetHTTPTimeout(OSPVProviderHandle, DEF_HTTP_TIMEOUT);

    return errcode;
}

int testOSPPProviderGetLocalKeys()
{
    int errcode = 0;
    OSPTPRIVATEKEY privkey;
    void *localcert;
    char msg[100];
    OSPTASN1ELEMENTINFO *eInfo;

    OSPM_MEMSET(&privkey, 0, sizeof(privkey));

    OSPM_MALLOC(localcert, unsigned char, CERT_SZ);
    if (localcert == OSPC_OSNULL) {
        errcode = OSPC_ERR_OS_FAILURE;
        OSPM_DBGERRORLOG(errcode, "Unable to allocate space for local certificate");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(privkey.PrivateKeyData, unsigned char, PKEY_SZ);
        if (privkey.PrivateKeyData == OSPC_OSNULL) {
            errcode = OSPC_ERR_OS_FAILURE;
            OSPM_DBGERRORLOG(errcode, "Unable to allocate space for private key");
        }
        privkey.PrivateKeyLength = 0;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPProviderGetLocalKeys(OSPVProviderHandle, &privkey, CERT_SZ, localcert);
    }

    /* Dump the local certificate */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPASN1ElementDecode((unsigned char *)localcert, &eInfo, 0);

        sprintf(msg, "Local Certificate (%d bytes)", eInfo->ElementLength);
        OSPTNLOGDUMP(eInfo->Element, eInfo->ElementLength, msg);
        printf("%s dumped to test_app.log\n", msg);
    }

    /* Dump the private key */
    if (errcode == OSPC_ERR_NO_ERROR) {
        sprintf(msg, "Private Key (%d bytes)", privkey.PrivateKeyLength);
        OSPTNLOGDUMP(privkey.PrivateKeyData, privkey.PrivateKeyLength, msg);
        printf("%s dumped to test_app.log\n", msg);
    }

    OSPM_FREE(localcert);
    OSPM_FREE(privkey.PrivateKeyData);
    privkey.PrivateKeyLength = 0;

    OSPPASN1ElementDelete(&eInfo, 0);

    return errcode;
}

int testOSPPProviderSetLocalKeys()
{
    int errcode = 0;
    OSPT_CERT localcert;
    OSPTPRIVATEKEY privatekey;

    errcode = OSPPUtilLoadPEMPrivateKey((unsigned char *)"pkey.pem", &privatekey);

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPUtilLoadPEMCert((unsigned char *)"localcert.pem", &localcert);
        if (errcode == OSPC_ERR_NO_ERROR) {
            printf("Read 1 Local Certificate\n");
        } else {
            printf("OSPPUtilLoadPEMCertreturned Error !\n");
        }
    } else {
        printf("OSPPUtilLoadPEMPrivateKey returned Error !\n");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPProviderSetLocalKeys(OSPVProviderHandle, &privatekey, localcert.CertData);
    }

    /* Free memory allocated while loading crypto information from PEM-encoded files */
    if (privatekey.PrivateKeyData != NULL) {
        free(privatekey.PrivateKeyData);
    }

    if (localcert.CertData != NULL) {
        free(localcert.CertData);
    }

    return errcode;
}

int testOSPPProviderGetLocalValidation()
{
    int errcode = 0;
    unsigned localvalidation;
    const char *local = "LOCAL";
    const char *remote = "REMOTE";

    errcode = OSPPProviderGetLocalValidation(OSPVProviderHandle, &localvalidation);

    if (localvalidation)
        printf("Token Validation = %s\n", local);
    else
        printf("Token Validation = %s\n", remote);

    return errcode;
}

int testOSPPProviderSetLocalValidation()
{
    int errcode = 0;

    return errcode;
}

int testOSPPProviderGetServicePoints()
{
    int errcode = 0;
    int i = 0;
    char *svcpts[MAX_SERVICE_POINTS];

    for (i = 0; i < MAX_SERVICE_POINTS; i++)
        OSPM_MALLOC(svcpts[i], char, SVC_PT_SZ);

    errcode = OSPPProviderGetServicePoints(OSPVProviderHandle, MAX_SERVICE_POINTS, SVC_PT_SZ, svcpts);

    for (i = 0; i < MAX_SERVICE_POINTS; i++) {
        printf("svcpt[%d] = %s\n", i, svcpts[i]);
        OSPM_FREE(svcpts[i]);
    }

    return errcode;
}

int testOSPPProviderGetSSLLifetime()
{
    int errcode = 0;
    unsigned ssllifetime;

    errcode = OSPPProviderGetSSLLifetime(OSPVProviderHandle, &ssllifetime);

    printf("ssl lifetime = %u\n", ssllifetime);

    return errcode;
}

int testOSPPProviderSetSSLLifetime()
{
    int errcode = 0;

    errcode = OSPPProviderSetSSLLifetime(OSPVProviderHandle, DEF_SSL_LIFETIME);

    return errcode;
}

int testOSPPProviderGetNumberOfAuthorityCertificates()
{
    int errcode = 0;
    unsigned numofcacerts = 0;

    errcode = OSPPProviderGetNumberOfAuthorityCertificates(OSPVProviderHandle, &numofcacerts);

    printf("num of ca certs = %u\n", numofcacerts);

    return errcode;
}

int testOSPPProviderGetNumberOfServicePoints()
{
    int errcode = 0;
    unsigned numofsvcpts = 0;

    errcode = OSPPProviderGetNumberOfServicePoints(OSPVProviderHandle, &numofsvcpts);

    if (errcode == 0)
        printf("num of svc pts = %u\n", numofsvcpts);

    return errcode;
}

int testOSPPTransactionSetPricingInfo()
{
    int errcode = 0;
    OSPT_PRICING_INFO PricingInfo1;
    OSPT_PRICING_INFO PricingInfo2;
    OSPT_PRICING_INFO *ospvPricingInfo[MAX_PRICING_INFO_ALLOWED];

    PricingInfo1.amount = 0.10f;
    PricingInfo1.increment = 60;
    OSPM_STRCPY(PricingInfo1.unit, "s");
    OSPM_STRCPY(PricingInfo1.currency, "USD");

    PricingInfo2.amount = 0.15f;
    PricingInfo2.increment = 10;
    OSPM_STRCPY(PricingInfo2.unit, "s");
    OSPM_STRCPY(PricingInfo2.currency, "EUR");

    ospvPricingInfo[0] = &PricingInfo1;
    ospvPricingInfo[1] = &PricingInfo2;
    ospvPricingInfo[2] = NULL;

    errcode = OSPPTransactionSetPricingInfo(OSPVTransactionHandle, ospvPricingInfo);

    return errcode;
}

int testOSPPTransactionSetNetworkId()
{
    int errcode = 0, i = 0;
    char SrcNetId[OSPC_SIZE_NORID];
    char DestNetId[OSPC_SIZE_NORID];

    printf("Enter the Source Network Identifier : ");
    fflush(stdin);
    while ((SrcNetId[i] = getchar()) != '\n')
        i++;

    SrcNetId[i] = '\0';

    i = 0;
    printf("Enter the Destination Network Identifier : ");
    fflush(stdin);

    while ((DestNetId[i] = getchar()) != '\n')
        i++;

    DestNetId[i] = '\0';

    errcode = OSPPTransactionSetNetworkIds(OSPVTransactionHandle, SrcNetId, DestNetId);

    return errcode;
}

int testOSPPTransactionNew()
{
    int errcode = 0;

    errcode = OSPPTransactionNew(OSPVProviderHandle, &OSPVTransactionHandle);

    return errcode;
}

int testOSPPTransactionDelete()
{
    int errcode = 0;

    if ((OSPVTransactionHandle == OSPC_TRAN_HANDLE_INVALID) &&
        (tranhandle2 == OSPC_TRAN_HANDLE_INVALID)) {
        printf("No Transaction to Delete\n");
        errcode = OSPC_ERR_TRAN_HANDLE_INVALID;
    }

    if (OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID) {

        errcode = OSPPTransactionDelete(OSPVTransactionHandle);
        if (errcode == OSPC_ERR_NO_ERROR) {
            if (!quietmode)
                printf("OSPVTransactionHandle deleted.\n");

            OSPVTransactionHandle = OSPC_TRAN_HANDLE_INVALID;
        }
    }

    if (tranhandle2 != OSPC_TRAN_HANDLE_INVALID) {
        errcode = OSPPTransactionDelete(tranhandle2);

        if (errcode == OSPC_ERR_NO_ERROR) {
            printf("tranhandle2 deleted.\n");

            tranhandle2 = OSPC_TRAN_HANDLE_INVALID;
        }
    }

    return errcode;
}

int testOSPPTransactionAccumulateOneWayDelay()
{
    int errcode = OSPC_ERR_NO_ERROR, i = 0;

    for (i = 0; i < 10; i++) {
        errcode = OSPPTransactionAccumulateOneWayDelay(OSPVTransactionHandle,
            accumtable10[i].Number,
            accumtable10[i].Min,
            accumtable10[i].Mean,
            accumtable10[i].Variance);

        printf("OSPPTransactionAccumulateOneWayDelay errcode = %d\n", errcode);
    }

    return errcode;
}

int testOSPPTransactionAccumulateTwoWayDelay()
{
    int errcode = OSPC_ERR_NO_ERROR, i = 0;

    for (i = 0; i < 2; i++) {
        errcode = OSPPTransactionAccumulateTwoWayDelay(OSPVTransactionHandle,
            accumtable2[i].Number,
            accumtable2[i].Min,
            accumtable2[i].Mean,
            accumtable2[i].Variance);

        printf("OSPPTransactionAccumulateTwoWayDelay errcode = %d\n", errcode);
    }

    return errcode;
}

int testOSPPTransactionGetDestProtocol()
{
    OSPE_PROTOCOL_NAME protocol;
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionGetDestProtocol(OSPVTransactionHandle, &protocol);
    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (protocol) {
        case OSPC_PROTNAME_UNDEFINED:
            printf("Destination Protocol is Not Configured at Server\n");
            break;
        case OSPC_PROTNAME_SIP:
            printf("Destination Protocol is SIP\n");
            break;
        case OSPC_PROTNAME_Q931:
            printf("Destination Protocol is H.323-Q931\n");
            break;
        case OSPC_PROTNAME_LRQ:
            printf("Destination Protocol is H.323-LRQ\n");
            break;
        case OSPC_PROTNAME_IAX:
            printf("Destination Protocol is IAX\n");
            break;
        case OSPC_PROTNAME_T37:
            printf("Destination Protocol is Fax-T.37\n");
            break;
        case OSPC_PROTNAME_T38:
            printf("Destination Protocol is Fax-T.38\n");
            break;
        case OSPC_PROTNAME_SKYPE:
            printf("Destination Protocol is Skype\n");
            break;
        case OSPC_PROTNAME_SMPP:
            printf("Destination Protocol is SMPP\n");
            break;
        case OSPC_PROTNAME_XMPP:
            printf("Destination Protocol is XMPP\n");
            break;
        case OSPC_PROTNAME_SMS:
            printf("Destination Protocol is SMS\n");
            break;
        case OSPC_PROTNAME_UNKNOWN:
            printf("Destination Protocol is Unknown\n");
            break;
        default:
            printf("Wrong Destination Protocol\n");
            break;
        }
    }

    return errcode;
}

int testOSPPTransactionIsDestOSPEnabled()
{
    OSPE_DEST_OSPENABLED dest_osp_ver;
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionIsDestOSPEnabled(OSPVTransactionHandle, &dest_osp_ver);
    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (dest_osp_ver) {
        case OSPC_DOSP_UNDEFINED:
            printf("Destination OSP Version Not Configured at Server\n");
            break;
        case OSPC_DOSP_TRUE:
            printf("Destination is OSP Enabled\n");
            break;
        case OSPC_DOSP_FALSE:
            printf("Destination is Not OSP Enabled\n");
            break;
        case OSPC_DOSP_UNKNOWN:
            printf("Destination OSP Status is Unknown\n");
            break;
        }
    }

    return errcode;
}

int testOSPPTransactionGetFirstDestination()
{
    int errcode = 0;
    char msg[100];
    token = (void *)c_token;
    tokensize = TOKEN_SZ;

    OSPM_MEMSET(ret_cid, 0, CALL_ID_SZ);
    callid = (void *)ret_cid;
    callidsize = CALL_ID_SZ;

    errcode = OSPPTransactionGetFirstDestination(OSPVTransactionHandle,
        /* TIMESTAMP_SZ, */
        0,
        validafter,
        validuntil,
        &timelimit,
        &callidsize,
        callid,
        CALLED_NUM_SZ,
        callednumber,
        CALLING_NUM_SZ,
        callingnumber,
        DESTINATION_SZ,
        dest,
        DESTINATION_SZ,
        destdev,
        &tokensize, token);

    if (errcode == 0 && !quietmode) {
        printf("callid size = %d value = %.*s\n", callidsize, callidsize, (char *)callid);
        printf("%s\n", dest);
        printf("timelimit = %u\n", timelimit);
        OSPM_SPRINTF(msg, "DEST = %s", dest);
        OSPTNLOGDUMP(token, tokensize, msg);
    }
    return errcode;
}

int testOSPPTransactionGetNextDestination()
{
    int errcode = 0;
    char msg[100];

    token = (void *)c_token;
    tokensize = TOKEN_SZ;
    OSPM_MEMSET(ret_cid, 0, CALL_ID_SZ);
    callid = (void *)ret_cid;
    callidsize = CALL_ID_SZ;

    errcode = OSPPTransactionGetNextDestination(OSPVTransactionHandle,
        (OSPEFAILREASON)TCcode, TIMESTAMP_SZ,
        validafter, validuntil,
        &timelimit, &callidsize,
        callid, CALLED_NUM_SZ,
        callednumber,
        CALLING_NUM_SZ,
        callingnumber,
        DESTINATION_SZ, dest,
        DESTINATION_SZ, destdev,
        &tokensize, token);

    if (errcode == 0 && !quietmode) {
        printf("callid size = %d value = %.*s\n", callidsize, callidsize, (char *)callid);
        printf("%s\n", dest);
        printf("timelimit = %u\n", timelimit);
        OSPM_SPRINTF(msg, "DEST = %s", dest);
        OSPTNLOGDUMP(token, tokensize, msg);
    }
    return errcode;
}

void testFreeCallIds()
{
    register int i = 0;
    for (i = 0; i < NUM_CALL_IDS; i++)
        OSPPCallIdDelete(&(callids[i]));
}

int testInitializeCallIds()
{
    register int i = 0;
    int errcode = 0;

    unsigned char *val[NUM_CALL_IDS] = {
        (unsigned char *)"1",
        (unsigned char *)"2",
        (unsigned char *)"3",
        (unsigned char *)"4",
        (unsigned char *)"5",
        (unsigned char *)"6",
        (unsigned char *)"7",
        (unsigned char *)"8"
    };
    unsigned lens[NUM_CALL_IDS] = {
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1
    };

    for (i = 0; i < NUM_CALL_IDS; i++) {
        callids[i] = OSPPCallIdNew(lens[i], (const unsigned char *)val[i]);
        if (callids[i] == OSPC_OSNULL) {
            errcode = OSPC_ERR_OS_FAILURE;
            break;
        }
    }

    if (errcode)
        testFreeCallIds();

    return errcode;
}

int testBuildUsageFromScratch(int IsSource, int BuildNew)
{
    int errcode = 0;
    unsigned detaillogsize = 0;
    OSPTUINT64 server_txn_id;
    OSPT_PRICING_INFO PricingInfo;
    OSPT_PRICING_INFO *ospvPricingInfo[MAX_PRICING_INFO_ALLOWED];
    server_txn_id = 2111133232;

    PricingInfo.amount = 10;
    PricingInfo.increment = 2;
    OSPM_STRCPY(PricingInfo.unit, "sec");
    OSPM_STRCPY(PricingInfo.currency, "USD");

    ospvPricingInfo[0] = &PricingInfo;
    ospvPricingInfo[1] = NULL;

    callid = (void *)c_id;

    /* User must have created a new transaction before this. */

    /* Gateway part starts here */
    if (errcode == OSPC_ERR_NO_ERROR && BuildNew) {

        errcode = OSPPTransactionNew(OSPVProviderHandle, &OSPVTransactionHandle);
        if (errcode == OSPC_ERR_NO_ERROR) {
            printf("Successful TransactionNew.\n");
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR && BuildNew) {
        errcode = OSPPTransactionSetServiceType(OSPVTransactionHandle, OSPC_SERVICE_VOICE);
        errcode = OSPPTransactionSetPricingInfo(OSPVTransactionHandle, ospvPricingInfo);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {

        errcode = OSPPTransactionBuildUsageFromScratch(OSPVTransactionHandle,
            (OSPTUINT64)server_txn_id,    /* Some hard coded Server Tx Id */
            IsSource, SourceIP,
            DestIP, SourceDevIP,
            DestDevIP,
            callingnumber,
            CallingNumFormat,
            callednumber,
            CalledNumFormat,
            callidsize, callid,
            (OSPEFAILREASON)TCcode,
            &detaillogsize,
            NULL);
    }

    if (errcode == OSPC_ERR_NO_ERROR && BuildNew) {
        printf("Transaction Built\n");
    } else if ((errcode == OSPC_ERR_NO_ERROR) && (!BuildNew)) {
        printf("Destination Added to transaction\n");
    } else {
        if (OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID) {
            OSPPTransactionDelete(OSPVTransactionHandle);
            OSPVTransactionHandle = OSPC_TRAN_HANDLE_INVALID;
        }
        printf("Errorcode = %d\n", errcode);
    }

    return errcode;
}

int testSetDestinationCount()
{
    int errcode = 0;
    unsigned destinationCount = 0;

    printf("Enter destination count (0 if N/A)");

    if (scanf("%d", &destinationCount) == 1) {
        printf("Setting destination count to %d\n", destinationCount);
        errcode = OSPPTransactionSetDestinationCount(OSPVTransactionHandle, destinationCount);
    } else {
        errcode = -1;
    }

    getchar();

    return errcode;
}

int testSetDuration()
{
    int errcode = 0;

    printf("Enter duration (in seconds) : ");

    if (scanf("%d", &duration) != 1) {
        errcode = -1;
    }

    getchar();

    return errcode;
}

int testSetTCCode()
{
    int errcode = 0;

    printf("Enter TC Code: ");

    if (scanf("%d", &TCcode) != 1) {
        errcode = -1;
    }

    getchar();

    return errcode;
}

int testSetStartTime()
{
    int errcode = 0;

    printf("Enter Call Start Time (in seconds since 1970 GMT) : ");

    if (scanf("%ld", &call_start_time) != 1) {
        errcode = -1;
    }

    getchar();

    return errcode;
}

int testSetEndTime()
{
    int errcode = 0;

    printf("Enter Call End Time (in seconds since 1970 GMT) : ");

    if (scanf("%ld", &call_end_time) != 1) {
        errcode = -1;
    }

    getchar();

    return errcode;
}

int testSetAlertTime()
{
    int errcode = 0;

    printf("Enter Call Alert Time (in seconds since 1970 GMT) : ");

    if (scanf("%ld", &call_alert_time) != 1) {
        errcode = -1;
    }

    getchar();

    return errcode;
}

int testSetConnectTime()
{
    int errcode = 0;

    printf("Enter Call Connect Time (in seconds since 1970 GMT) : ");

    if (scanf("%ld", &call_connect_time) != 1) {
        errcode = -1;
    }

    getchar();

    return errcode;
}

int testSetServiceType()
{
    int errcode;

    errcode = OSPPTransactionSetServiceType(OSPVTransactionHandle, OSPC_SERVICE_STIRQUERY);

    return errcode;
}

int testGetServiceType()
{
    OSPE_SERVICE srvtype = OSPC_SERVICE_UNKNOWN;
    int errcode;

    if ((errcode = OSPPTransactionGetServiceType(OSPVTransactionHandle, &srvtype)) == OSPC_ERR_NO_ERROR) {
        printf("The Service Tpye is : %d\n", srvtype);
    }

    return errcode;
}

int testOSPPTransactionInitializeAtDevice(int IsSource)
{
    int errcode = 0;
    unsigned detaillogsize = 0, authorised = OSPC_TRAN_NOT_AUTHORISED, timelimit = 0;
    unsigned char token2[TOKEN_SZ];
    OSPT_PRICING_INFO PricingInfo;
    OSPT_PRICING_INFO *ospvPricingInfo[MAX_PRICING_INFO_ALLOWED];

    callid = (void *)c_id;
    token = (void *)c_token;

    PricingInfo.amount = 10;
    PricingInfo.increment = 2;
    OSPM_STRCPY(PricingInfo.unit, "sec");
    OSPM_STRCPY(PricingInfo.currency, "USD");

    ospvPricingInfo[0] = &PricingInfo;
    ospvPricingInfo[1] = NULL;


    OSPM_MEMSET(token2, 0, TOKEN_SZ);

    /* User must have created a new transaction before this. */

    /* Gateway part starts here */

    if (errcode == OSPC_ERR_NO_ERROR) {

        errcode = OSPPTransactionNew(OSPVProviderHandle, &OSPVTransactionHandle);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPTransactionSetServiceType(OSPVTransactionHandle, OSPC_SERVICE_VOICE);
        errcode = OSPPTransactionSetPricingInfo(OSPVTransactionHandle, ospvPricingInfo);
    }

    tokensize = TOKEN_SZ;

    if (errcode == OSPC_ERR_NO_ERROR) {

        printf("Successful TransactionNew.\n");

        errcode = OSPPTransactionInitializeAtDevice(OSPVTransactionHandle,
            IsSource, SourceIP,
            DestIP, SourceDevIP,
            NULL, callingnumber,
            CallingNumFormat,
            callednumber,
            CalledNumFormat,
            callidsize, callid,
            tokensize, token,
            &authorised, &timelimit,
            &detaillogsize, NULL,
            tokenalgo);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (authorised) {
        case OSPC_TRAN_AUTHORISED:
            printf("Initialization authorised\n");
            break;
        case OSPC_TRAN_REAUTHORISED:
            printf("Initialization reauthorised\n");
            break;
        case OSPC_TRAN_NOT_AUTHORISED:
            printf("Initialization not authorised\n");
            break;
        default:
            printf("Unknown authorisation code\n");
            break;
        }
    } else {
        printf("Errorcode TransactionInitialize = %d\n", errcode);
    }

    return errcode;
}

int testOSPPTransactionRecordFailure()
{
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionRecordFailure(OSPVTransactionHandle, (OSPEFAILREASON)TCcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        printf("OSPPTransactionRecordFailure Successful\n");
    } else {
        printf("Errorcode TransactionRecordFailure = %d\n", errcode);
    }

    return errcode;
}

/* Should only be called by OGW */
int testOSPPTransactionReinitializeAtDevice()
{
    int errcode = 0;
    unsigned IsSource = OSPC_ROLE_SOURCE;
    unsigned detaillogsize = 0, authorised = OSPC_TRAN_NOT_AUTHORISED, timelimit = 0;
    unsigned char token2[TOKEN_SZ];

    OSPM_MEMSET(token2, 0, TOKEN_SZ);

    callid = (void *)c_id;
    token = (void *)c_token;

    tokensize = TOKEN_SZ;

    /* Gateway does reinit */

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPTransactionReinitializeAtDevice(tranhandle2,
            (OSPEFAILREASON)TCcode, IsSource,
            SourceIP, DestIP,
            SourceDevIP,
            OSPC_OSNULL,
            callingnumber,
            callednumber,
            callidsize, callid,
            tokensize, token,
            &authorised,
            &timelimit,
            &detaillogsize, NULL,
            tokenalgo);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        switch (authorised) {
        case OSPC_TRAN_AUTHORISED:
            printf("Initialization authorised\n");
            break;
        case OSPC_TRAN_REAUTHORISED:
            printf("Initialization reauthorised\n");
            break;
        case OSPC_TRAN_NOT_AUTHORISED:
            printf("Initialization not authorised\n");
            break;
        default:
            printf("Unknown authorisation code\n");
            break;
        }
    } else {
        printf("Errorcode TransactionReInitialize = %d\n", errcode);
    }

    return errcode;
}

int testOSPPTransactionRequestSuggestedAuthorisation()
{
    int errcode = 0;
    unsigned detaillogsize = 0;
    const char *preferredDest[] = { "[172.16.4.10]:5060", "[172.16.4.10]:5061", NULL };

    errcode = testInitializeCallIds();

    numdestinations = NUM_CALL_IDS;
    if (errcode == OSPC_ERR_NO_ERROR)
        errcode = OSPPTransactionRequestAuthorisation(OSPVTransactionHandle,
            SourceIP,
            SourceDevIP,    /* Some random IP address that would probably not be in the Server */
            callingnumber,
            CallingNumFormat,
            callednumber,
            CalledNumFormat,
            "919404556#4444",
            NUM_CALL_IDS, callids,
            preferredDest,
            &numdestinations,
            &detaillogsize,
            (void *)NULL);

    if (errcode == 0 && !quietmode) {
        printf("num dest = %u\n", numdestinations);
    }
    testFreeCallIds();

    return errcode;
}

int testOSPPTransactionRequestAuthorisation()
{
    int errcode = 0;
    unsigned detaillogsize = 0;

    errcode = testInitializeCallIds();

    numdestinations = NUM_CALL_IDS;

    if (errcode == OSPC_ERR_NO_ERROR)
        errcode = OSPPTransactionRequestAuthorisation(OSPVTransactionHandle,
            SourceIP,
            SourceDevIP,
            callingnumber,
            (OSPE_NUMBER_FORMAT)
            CallingNumFormat,
            callednumber,
            (OSPE_NUMBER_FORMAT)
            CalledNumFormat,
            "919404556#4444",
            NUM_CALL_IDS, callids,
            (const char **)NULL,
            &numdestinations,
            &detaillogsize,
            (void *)NULL);

    if (errcode == 0 && !quietmode) {
        printf("num dest = %u\n", numdestinations);
    }
    testFreeCallIds();

    return errcode;
}

int testOSPPTransactionIndicateCapabilities()
{
    int errcode = 0;
    unsigned detaillogsize = 0;

    if (errcode == OSPC_ERR_NO_ERROR)
        errcode = OSPPTransactionIndicateCapabilities(OSPVTransactionHandle,
            SourceIP,
            SourceDevIP,
            "SrcNetworkId",
            almostOutOfResources,
            &detaillogsize,
            (void *)NULL);

    return errcode;
}

int testOSPPProviderSetCapabilitiesURLs()
{
    int errcode = 0;

    errcode = OSPPProviderSetCapabilitiesURLs(OSPVProviderHandle, num_capURLs, CapMsgCount, (const char **)capURLs);

    return errcode;
}

int testOSPPProviderSetServicePoints()
{
    int errcode = 0;

    errcode = OSPPProviderSetServicePoints(OSPVProviderHandle, num_serv_points, MsgCount, (const char **)servicepoints);

    return errcode;
}

int testOSPPTransactionRequestReauthorisation()
{
    int errcode = 0;
    unsigned authorised = 0, detaillogsize = 0;

    token = (void *)c_token;
    tokensize = TOKEN_SZ;

    OSPM_MEMSET(token, 0, TOKEN_SZ);

    if (errcode == 0) {

        errcode = OSPPTransactionRequestReauthorisation(OSPVTransactionHandle, 10,
            &tokensize, token,
            &authorised, &timelimit,
            &detaillogsize,
            (void *)NULL);
    }

    printf("Errorcode = %d.\nAuthorised = %u.\nTimelimit = %u.\nToken = %s.", errcode, authorised, timelimit, (char *)token);

    return errcode;
}

int testGetCallingNumber()
{
    int errcode = 0;
    printf("The Current Calling Number is : %s\n", callingnumber);
    return errcode;
}

int testGetCalledNumber()
{
    int errcode = 0;
    printf("The Current Called Number is : %s\n", callednumber);
    return errcode;
}

int testSetCallingNumber()
{
    int errcode = 0;
    int i = 0;
    printf("Enter the new value for Calling number : ");
    fflush(stdin);
    while ((callingnumber[i] = getchar()) != '\n')
        i++;
    callingnumber[i] = '\0';
    if (!OSPM_STRCMP(callingnumber, "")) {
        printf("WARNING : You have set an Empty Calling Number !!\n");
    }
    printf("Calling Number Set to the new value\n");
    return errcode;
}

int testSetCallId()
{
    int errcode = 0;

    OSPM_STRCPY((char *)ret_cid, "");
    printf("Call Id Set to the Empty for Validate Authorization\n");
    return errcode;
}

int testSetCalledNumber()
{
    int errcode = 0;
    int i = 0;
    printf("Enter the new value for Called number : ");
    fflush(stdin);
    while ((callednumber[i] = getchar()) != '\n')
        i++;
    callednumber[i] = '\0';
    if (!OSPM_STRCMP(callednumber, "")) {
        printf("WARNING : You have set an Empty Called Number !!\n");
    }
    printf("Called Number Set to the new value\n");
    return errcode;
}

int testOSPPTransactionModifyDeviceIdentifiersAgain()
{
    int errcode = 0;

    errcode = OSPPTransactionModifyDeviceIdentifiers(OSPVTransactionHandle,
        "99.99.99.99",
        "100.100.100.100",
        "101.101.101.101",
        "102.102.102.102");

    printf("errcode = %d\n", errcode);
    return errcode;
}

int testOSPPTransactionModifyDeviceIdentifiers()
{
    int errcode = 0;

    errcode = OSPPTransactionModifyDeviceIdentifiers(OSPVTransactionHandle,
        ModifiedSourceIP,
        ModifiedSourceDevIP,
        ModifiedDestIP,
        ModifiedDestDevIP);

    printf("errcode = %d\n", errcode);
    return errcode;
}

int testOSPPTransactionGetLookAheadInfoIfPresent()
{
    int errcode = 0;
    OSPE_PROTOCOL_NAME DestProt = OSPC_PROTNAME_UNDEFINED;
    OSPE_DEST_OSPENABLED DestOSPStatus = OSPC_DOSP_UNDEFINED;
    char LookAheadDest[DESTINATION_SZ] = { "" };
    OSPTBOOL HasLookAheadInfo = OSPC_FALSE;

    if (errcode == 0) {
        if (errcode == 0) {
            errcode = OSPPTransactionGetLookAheadInfoIfPresent
                (OSPVTransactionHandle, &HasLookAheadInfo,
                LookAheadDest, &DestProt, &DestOSPStatus);
        }

        if (errcode == 0 && HasLookAheadInfo) {
            printf("Look Ahead Info Present ..\nLookAheadDest = %s\n", LookAheadDest);
            switch (DestProt) {
            case OSPC_PROTNAME_UNDEFINED:
                printf("Destination Protocol is Not Configured at Server\n");
                break;
            case OSPC_PROTNAME_SIP:
                printf("Destination Protocol is SIP\n");
                break;
            case OSPC_PROTNAME_Q931:
                printf("Destination Protocol is H.323-Q931\n");
                break;
            case OSPC_PROTNAME_LRQ:
                printf("Destination Protocol is H.323-LRQ\n");
                break;
            case OSPC_PROTNAME_IAX:
                printf("Destination Protocol is IAX\n");
                break;
            case OSPC_PROTNAME_T37:
                printf("Destination Protocol is Fax-T.37\n");
                break;
            case OSPC_PROTNAME_T38:
                printf("Destination Protocol is Fax-T.38\n");
                break;
            case OSPC_PROTNAME_SKYPE:
                printf("Destination Protocol is Skype\n");
                break;
            case OSPC_PROTNAME_SMPP:
                printf("Destination Protocol is SMPP\n");
                break;
            case OSPC_PROTNAME_XMPP:
                printf("Destination Protocol is XMPP\n");
                break;
            case OSPC_PROTNAME_SMS:
                printf("Destination Protocol is SMS\n");
                break;
            case OSPC_PROTNAME_UNKNOWN:
                printf("Destination Protocol is Unknown\n");
                break;
            default:
                printf("Wrong Destination Protocol\n");
                break;
            }

            switch (DestOSPStatus) {
            case OSPC_DOSP_UNDEFINED:
                printf("Destination OSP Version Not Configured at Server\n");
                break;
            case OSPC_DOSP_TRUE:
                printf("Destination is OSP Enabled\n");
                break;
            case OSPC_DOSP_FALSE:
                printf("Destination is Not OSP Enabled\n");
                break;
            case OSPC_DOSP_UNKNOWN:
                printf("Destination OSP Status is Unknown\n");
                break;
            }
        } else if (errcode == 0 && (!HasLookAheadInfo)) {
            printf("Look Ahead Info Not Present\n");
        }
        printf("errcode = %d\n", errcode);

    }

    return errcode;
}


int testOSPPTransactionValidateAuthorisation()
{
    int errcode = 0;
    unsigned authorised = 0, detaillogsize = 0;
    OSPTTRANS *trans = OSPC_OSNULL;

    errcode = testInitializeCallIds();

    /* Remove the authrsp so reportusage doesn't get confused. */

    trans = OSPPTransactionGetContext(OSPVTransactionHandle, &errcode);

    if (errcode == 0) {
        OSPPTransactionDeleteResponse(trans);

        if (errcode == 0) {
            errcode = OSPPTransactionValidateAuthorisation(OSPVTransactionHandle,
                SourceIP, dest,
                SourceDevIP, destdev,
                callingnumber,
                CallingNumFormat,
                callednumber,
                CalledNumFormat,
                callidsize, ret_cid,
                tokensize, c_token,
                &authorised, &timelimit,
                &detaillogsize,
                (void *)NULL,
                tokenalgo);
        }

        if (errcode == 0 && !quietmode)
            printf("authorised = %u\n", authorised);

        testFreeCallIds();
    }

    return errcode;
}

int testOSPPTransactionReportUsage()
{
    int errcode = 0;
    unsigned detaillogsize = 0;

    if (OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID) {

        if (!quietmode)
            printf("Reporting Usage for OSPVTransactionHandle %d\n", (int)OSPVTransactionHandle);

        errcode = OSPPTransactionReportUsage(OSPVTransactionHandle,
            duration, call_start_time, call_end_time, call_alert_time, call_connect_time,
            IS_PDD_INFO_AVAILABLE, 1010,            /* PDD */
            release,                                /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06", /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize, (void *)NULL);
    }

    if (tranhandle2 != OSPC_TRAN_HANDLE_INVALID) {

        printf("Reporting Usage for tranhandle2\n");

        errcode = OSPPTransactionReportUsage(tranhandle2,
            duration, call_start_time, call_end_time, call_alert_time, call_connect_time,
            IS_PDD_INFO_AVAILABLE, 1020,            /* PDD */
            release,                                /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06", /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize, (void *)NULL);
    }

    if ((tranhandle2 == OSPC_TRAN_HANDLE_INVALID) &&
        (OSPVTransactionHandle == OSPC_TRAN_HANDLE_INVALID)) {
        printf("Transaction handle invalid\n");
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    }
    return errcode;
}

int testOSPPSecSignatureVerify()
{
    int errcode = 0;
    int tokenfd = -1;
    OSPTSEC *security = OSPC_OSNULL;
    unsigned char *signature = OSPC_OSNULL;
    unsigned char *content = OSPC_OSNULL;
    unsigned int signatureLength = 0;
    unsigned int contentLength = 0;
    OSPTPROVIDER *provider = OSPC_OSNULL;

    tokenfd = _Open("../test_app/testtoken.out", O_RDONLY, 00664);
    if (tokenfd == -1) {
        errcode = -1;
        OSPM_DBGERRORLOG(errcode, "Unable to open ../test_app/testtoken.out");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (_Read(tokenfd, (unsigned char *)&signatureLength, 4) == 4) {
            OSPM_MALLOC(signature, unsigned char, signatureLength);
            if (_Read(tokenfd, signature, signatureLength) != signatureLength) {
                errcode = -1;
            }
        } else {
            errcode = -1;
        }

        _Close(tokenfd);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        provider = OSPPProviderGetContext(OSPVProviderHandle, &errcode);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPProviderGetSecurity(provider, &security);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPSecSignatureVerify(security, &content, &contentLength,
            signature, signatureLength,
            OSPC_SEC_SIGNATURE_AND_CONTENT);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(errcode, "XMLTEST Test Signature Verified");
        } else {
            OSPM_DBGERRORLOG(errcode, "Error verifying XMLTEST Test Signature");
        }
    }

    OSPM_FREE(signature);
    OSPM_FREE(content);

    return errcode;
}

int testOSPPSecCertShowChain()
{
    int errcode = 0;
    unsigned char *certificate;
    unsigned int certificateLength;
    int caIndex = 0;
    int nextCAIndex = 0;
    char msg[100] = "";
    OSPTPROVIDER *provider = OSPC_OSNULL;
    OSPTSEC *security = OSPC_OSNULL;

    OSPM_MALLOC(certificate, unsigned char, CERT_SZ);

    if (certificate == OSPC_OSNULL) {
        errcode = -1;
        OSPM_DBGERRORLOG(errcode, "Error allocating space for certificate");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        provider = OSPPProviderGetContext(OSPVProviderHandle, &errcode);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPProviderGetSecurity(provider, &security);
    }


    for (caIndex = -1; errcode == OSPC_ERR_NO_ERROR;
         caIndex = nextCAIndex) {
        if (errcode == OSPC_ERR_NO_ERROR) {
            /* Reset buffer size */
            certificateLength = CERT_SZ;
            nextCAIndex = caIndex;

            /* Look for the next certificate in the chain */
            errcode = OSPPSecValidCertChain(security, &nextCAIndex, certificate, &certificateLength);
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            if (caIndex == nextCAIndex) {
                /* Last certificate tested was self signed */
                sprintf(msg, "CAIndex=%d is self signed", caIndex);
                OSPM_DBGERRORLOG(errcode, msg);
                break;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            sprintf(msg, "CAIndex=%d, Next CAIndex=%d", caIndex, nextCAIndex);
            OSPTNLOGDUMP(certificate, certificateLength, msg);

            OSPM_DBGERRORLOG(errcode, msg);
        }

    }

    OSPM_FREE(certificate);

    return errcode;
}

int testOSPToolkitVersion()
{
    printf("OSP Client Toolkit Version: %d.%d.%d\n",
        OSP_CLIENT_TOOLKIT_VERSION_MAJOR,
        OSP_CLIENT_TOOLKIT_VERSION_MINOR,
        OSP_CLIENT_TOOLKIT_VERSION_BUGFIX);

    return 0;
}

int testSetRoleInfo()
{
    int errcode = 0;

    errcode = OSPPTransactionSetRoleInfo(
        OSPVTransactionHandle, OSPC_RSTATE_STOP1, OSPC_RFORMAT_RADIUS, OSPC_RVENDOR_CISCO);

    return errcode;
}

int testSetTermCause()
{
    int errcode = 0;

    errcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_Q850, 1, "tc_q850");
    errcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_H323, 2, "tc_h323");
    errcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_SIP, 3, "tc_sip");
    errcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_XMPP, 4, "tc_xmpp");

    return errcode;
}

int testSetNumberPortability()
{
    int errcode = 0;

    errcode = OSPPTransactionSetNumberPortability(OSPVTransactionHandle, "13243546576", "NPCic", 1);

    return errcode;
}

int testSetOperatorName()
{
    int errcode = 0;

    errcode = OSPPTransactionSetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_SPID, "spid");
    errcode = OSPPTransactionSetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_ALTSPID, "altspid");
    errcode = OSPPTransactionSetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_OCN, "ocn");
    errcode = OSPPTransactionSetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_SPN, "spn");
    errcode = OSPPTransactionSetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_ALTSPN, "altspn");
    errcode = OSPPTransactionSetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_MCC, "mcc");
    errcode = OSPPTransactionSetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_MNC, "mnc");

    return errcode;
}

int testSetSIPHeaders()
{
    #define number 3
    const char *fingerprints[number] = { "fingerprint1", "fingerprint2", "fingerprint3" };
    int errcode = 0;

    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_FROM, OSPC_NFORMAT_E164, "FromE164");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_FROM, OSPC_NFORMAT_SIP, "<sip:1001@sip.transnexus.com>");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_FROM, OSPC_NFORMAT_DISPLAYNAME, "Hello Günter ");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_FROM, OSPC_NFORMAT_TRANSPORT, "1.2.3.4");

    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_TO, OSPC_NFORMAT_SIP, "<sips:1002@transnexus.com>");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_TO, OSPC_NFORMAT_TRANSPORT, "transnexus.com");

    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_PAI, OSPC_NFORMAT_E164, "AssertedIdE164");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_PAI, OSPC_NFORMAT_SIP, "<sip:1003@sip.transnexus.com>");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_PAI, OSPC_NFORMAT_DISPLAYNAME, "AssertedIdDisplayName");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_PAI, OSPC_NFORMAT_TRANSPORT, "sip.transnexus.com");

    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_RPID, OSPC_NFORMAT_DISPLAYNAME, "RPIDDisplayName");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_RPID, OSPC_NFORMAT_SIP, "\"'Eric Smith' of AT&T\" <sip:2001@att.com>;privacy=off;screen=no");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_RPID, OSPC_NFORMAT_TRANSPORT, "att.com");

    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_PCI, OSPC_NFORMAT_E164, "ChargeInfoE164");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_PCI, OSPC_NFORMAT_DISPLAYNAME, "ChargeInfoDisplayName");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_PCI, OSPC_NFORMAT_TRANSPORT, "a.b.c.d");

    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_DIV, OSPC_NFORMAT_URL, "DivSrcInfoURL");
    errcode = OSPPTransactionSetDiversion(OSPVTransactionHandle, "DiversionSrcInfoE164", "DiversionDevInfo");
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_DIV, OSPC_NFORMAT_TRANSPORT, "2.3.4.5");

    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_CONTACT, OSPC_NFORMAT_TRANSPORT, "3.4.5.6");

    errcode = OSPPTransactionSetRequestDate(OSPVTransactionHandle, time(OSPC_OSNULL));
    errcode = OSPPTransactionSetFingerprint(OSPVTransactionHandle, number, fingerprints);
    errcode = OSPPTransactionSetSIPHeader(OSPVTransactionHandle, OSPC_SIPHEADER_IDENTITY, OSPC_NFORMAT_SIP, "IdentityHeader");

    errcode = OSPPTransactionSetAttestInfo(OSPVTransactionHandle, "A");
    errcode = OSPPTransactionSetOrigId(OSPVTransactionHandle, "1b541919-d4e1-42dc-bbc7-a13c06ad45c4");

    return errcode;
}

int testSetApplicationId()
{
    int errcode = 0;

    errcode = OSPPTransactionSetApplicationId(OSPVTransactionHandle, "ApplicationId");

    return errcode;
}

int testSetProtocol()
{
    int errcode = 0;

    errcode = OSPPTransactionSetProtocol(OSPVTransactionHandle, OSPC_PROTTYPE_SOURCE, OSPPDestProtocolGetPart("h323-LRQ"));
    errcode = OSPPTransactionSetProtocol(OSPVTransactionHandle, OSPC_PROTTYPE_DESTINATION, OSPPDestProtocolGetPart("h323-Q931"));
    errcode = OSPPTransactionSetProtocol(OSPVTransactionHandle, OSPC_PROTTYPE_NA, OSPPDestProtocolGetPart("iax"));

    return errcode;
}

int testSetCodec()
{
    int errcode = 0;

    errcode = OSPPTransactionSetCodec(OSPVTransactionHandle, OSPC_CODEC_SOURCE, "g729");
    errcode = OSPPTransactionSetCodec(OSPVTransactionHandle, OSPC_CODEC_DESTINATION, "g723");
    errcode = OSPPTransactionSetVideoCodec(OSPVTransactionHandle, OSPC_CODEC_SOURCE, "ulaw");
    errcode = OSPPTransactionSetVideoCodec(OSPVTransactionHandle, OSPC_CODEC_DESTINATION, "alaw");

    return errcode;
}

int testSetNetworkId()
{
    int errcode = 0;

    errcode = OSPPTransactionSetNetworkIds(OSPVTransactionHandle, "SrcNetworkId", "DestNetworkId");

    return errcode;
}

int testSetSwitchId()
{
    int errcode = 0;

    errcode = OSPPTransactionSetSrcSwitchId(OSPVTransactionHandle, "SrcSwitchId");
    errcode = OSPPTransactionSetDestSwitchId(OSPVTransactionHandle, "DestSwitchId");

    return errcode;
}

int testSetSessionId()
{
    int errcode = 0;

    OSPT_CALL_ID *callid;
    callid = OSPPCallIdNew(12, (const unsigned char *)"sourcecallid");
    errcode = OSPPTransactionSetSessionId(OSPVTransactionHandle, OSPC_SESSIONID_SOURCE, callid);
    OSPPCallIdDelete(&callid);
    callid = OSPPCallIdNew(17, (const unsigned char *)"destinationcallid");
    errcode = OSPPTransactionSetSessionId(OSPVTransactionHandle, OSPC_SESSIONID_DESTINATION, callid);
    OSPPCallIdDelete(&callid);
    callid = OSPPCallIdNew(17, (const unsigned char *)"correlationcallid");
    errcode = OSPPTransactionSetSessionId(OSPVTransactionHandle, OSPC_SESSIONID_CORRELATION, callid);
    OSPPCallIdDelete(&callid);
    callid = OSPPCallIdNew(11, (const unsigned char *)"localcallid");
    errcode = OSPPTransactionSetSessionId(OSPVTransactionHandle, OSPC_SESSIONID_LOCAL, callid);
    OSPPCallIdDelete(&callid);
    callid = OSPPCallIdNew(12, (const unsigned char *)"remotecallid");
    errcode = OSPPTransactionSetSessionId(OSPVTransactionHandle, OSPC_SESSIONID_REMOTE, callid);
    OSPPCallIdDelete(&callid);

    return errcode;
}

int testSetCustomInfo(int flag)
{
    int errcode = 0;

    if (flag) {
        errcode = OSPPTransactionSetCustomInfo(OSPVTransactionHandle, 0, "CustomInfo_first");
        errcode = OSPPTransactionSetCustomInfo(OSPVTransactionHandle, 16, "true");
    } else {
        errcode = OSPPTransactionSetCustomInfo(OSPVTransactionHandle, 31, "CustomInfo_32");
    }

    return errcode;
}

int testSetReleaseSource()
{
    int errcode = 0;

    switch (release) {
    case OSPC_RELEASE_UNKNOWN:
        release = OSPC_RELEASE_SOURCE;
        break;
    case OSPC_RELEASE_SOURCE:
        release = OSPC_RELEASE_DESTINATION;
        break;
    case OSPC_RELEASE_DESTINATION:
        release = OSPC_RELEASE_INTERNAL;
        break;
    case OSPC_RELEASE_INTERNAL:
        release = OSPC_RELEASE_EXTERNAL;
        break;
    case OSPC_RELEASE_EXTERNAL:
    default:
        release = OSPC_RELEASE_UNKNOWN;
        break;
    }

    return errcode;
}

int testSetTotalSetupAttempts()
{
    int errcode = 0;

    errcode = OSPPTransactionSetTotalSetupAttempts(OSPVTransactionHandle, 12);

    return errcode;
}

int testSetCallingPartyInfo()
{
    int errcode = 0;

    errcode = OSPPTransactionSetCallPartyInfo(OSPVTransactionHandle, OSPC_CPARTY_SOURCE, "callingusername", "callinguserid", "callingusergroup");

    return errcode;
}

int testSetCalledPartyInfo()
{
    int errcode = 0;

    errcode = OSPPTransactionSetCallPartyInfo(OSPVTransactionHandle, OSPC_CPARTY_DESTINATION, "calledusername", "calleduserid", "calledusergroup");

    return errcode;
}

int testSetTransferId()
{
    int errcode = 0;

    errcode = OSPPTransactionSetTransferId(OSPVTransactionHandle, "transferid");

    return errcode;
}

int testSetTransferStatus()
{
    int errcode = 0;

    errcode = OSPPTransactionSetTransferStatus(OSPVTransactionHandle, OSPC_TSTATUS_DESTTRANSFERFROM);

    return errcode;
}

int testSetNetTransCalled()
{
    int errcode = 0;

    errcode = OSPPTransactionSetNetworkTranslatedCalledNumber(OSPVTransactionHandle, OSPC_NFORMAT_E164, "2345678901");

    return errcode;
}

int testSetSrcServiceProvider()
{
    int errcode = 0;

    errcode = OSPPTransactionSetSrcServiceProvider(OSPVTransactionHandle, "srcproviderofservice");

    return errcode;
}

int testSetDestServiceProvider()
{
    int errcode = 0;

    errcode = OSPPTransactionSetDestServiceProvider(OSPVTransactionHandle, "dstproviderofservice");

    return errcode;
}

int testSetSystemId()
{
    int errcode = 0;

    errcode = OSPPTransactionSetSystemId(OSPVTransactionHandle, "systemid");

    return errcode;
}

int testSetRelatedReason()
{
    int errcode = 0;

    errcode = OSPPTransactionSetRelatedReason(OSPVTransactionHandle, "relatedcallidreason");

    return errcode;
}

int testSetUserAgent()
{
    int errcode = 0;

    errcode = OSPPTransactionSetUserAgent(OSPVTransactionHandle, "Cisco-SIPGateway/IOS-12.x");

    return errcode;
}

int testSetSrcMediaAddresses()
{
    int errcode = 0;

    errcode = OSPPTransactionSetSrcAudioAddr(OSPVTransactionHandle, "[172.16.0.1]:6000");
    errcode = OSPPTransactionSetSrcVideoAddr(OSPVTransactionHandle, "gateway.transnexus.com:6001");

    return errcode;
}

int testSetDestMediaAddresses()
{
    static int index = 6002;
    char buf[64];
    int errcode = 0;

    sprintf(buf, "[172.16.0.1]:%d", index++);
    errcode = OSPPTransactionSetDestAudioAddr(OSPVTransactionHandle, buf);
    sprintf(buf, "gateway.transnexus.com:%d", index++);
    errcode = OSPPTransactionSetDestVideoAddr(OSPVTransactionHandle, buf);

    return errcode;
}

int testSetProxyIngressAddresses()
{
    int errcode = 0;

    errcode = OSPPTransactionSetProxyIngressAddr(OSPVTransactionHandle, "[172.16.0.1]:5060");

    return errcode;
}

int testSetProxyEgressAddresses()
{
    static int index = 5070;
    char buf[64];
    int errcode = 0;

    sprintf(buf, "[192.168.0.1]:%d", index++);
    errcode = OSPPTransactionSetProxyEgressAddr(OSPVTransactionHandle, buf);

    return errcode;
}

int testSetProviderPDD(int pdd)
{
    int errcode = 0;

    errcode = OSPPTransactionSetProviderPDD(OSPVTransactionHandle, pdd);

    return errcode;
}

int testSetCallType()
{
    int errcode = 0;

    errcode = OSPPTransactionSetCallType(OSPVTransactionHandle, "calltype");
    errcode = OSPPTransactionSetCallCategory(OSPVTransactionHandle, "callcategory");
    errcode = OSPPTransactionSetNetworkType(OSPVTransactionHandle, "networktype");

    return errcode;
}

int testGetNumberPortability()
{
    int errcode = 0;
    char rn[OSPC_SIZE_E164NUM];
    char cic[OSPC_SIZE_NORID];
    int npdi;

    errcode = OSPPTransactionGetNumberPortabilityParameters(OSPVTransactionHandle, sizeof(rn), rn, sizeof(cic), cic, &npdi);
    printf("rn = '%s'\n", rn);
    printf("cic = '%s'\n", cic);
    printf("npdi = %d\n", npdi);

    return errcode;
}

int testGetOperatorName()
{
    int errcode = 0;
    char opname[OSPC_SIZE_NORID];

    errcode = OSPPTransactionGetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_SPID, sizeof(opname), opname);
    printf("spid = '%s'\n", opname);
    errcode = OSPPTransactionGetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_ALTSPID, sizeof(opname), opname);
    printf("altspid = '%s'\n", opname);
    errcode = OSPPTransactionGetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_OCN, sizeof(opname), opname);
    printf("ocn = '%s'\n", opname);
    errcode = OSPPTransactionGetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_SPN, sizeof(opname), opname);
    printf("spn = '%s'\n", opname);
    errcode = OSPPTransactionGetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_ALTSPN, sizeof(opname), opname);
    printf("altspn = '%s'\n", opname);
    errcode = OSPPTransactionGetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_MCC, sizeof(opname), opname);
    printf("mcc = '%s'\n", opname);
    errcode = OSPPTransactionGetOperatorName(OSPVTransactionHandle, OSPC_OPNAME_MNC, sizeof(opname), opname);
    printf("mnc = '%s'\n", opname);

    return errcode;
}

int testGetURL()
{
    int errcode = 0;
    char url[OSPC_SIZE_NORID];

    errcode = OSPPTransactionGetURL(OSPVTransactionHandle, OSPC_URL_SIP, sizeof(url), url);
    printf("sip = '%s'\n", url);
    errcode = OSPPTransactionGetURL(OSPVTransactionHandle, OSPC_URL_SMS, sizeof(url), url);
    printf("sms = '%s'\n", url);
    errcode = OSPPTransactionGetURL(OSPVTransactionHandle, OSPC_URL_MMS, sizeof(url), url);
    printf("mms = '%s'\n", url);

    return errcode;
}

int testGetCNAM()
{
    int errcode = 0;
    char cnam[OSPC_SIZE_NORID];

    errcode = OSPPTransactionGetCNAM(OSPVTransactionHandle, sizeof(cnam), cnam);
    printf("CNAM = '%s'\n", cnam);

    return errcode;
}

int testStatsLost()
{
    int errcode = 0;

    errcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 1, 2);
    errcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, 3, -1);
    errcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, -1, 4);
    errcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, -1, -1);
    errcode = OSPPTransactionSetVideoLost(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 2, 4);
    errcode = OSPPTransactionSetVideoLost(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, 6, -1);
    errcode = OSPPTransactionSetVideoLost(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, -1, 8);
    errcode = OSPPTransactionSetVideoLost(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, -1, -1);

    return errcode;
}

int testStatsJitter()
{
    int errcode = 0;

    errcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 1, 2, 3, 4, 5);
    errcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, -1, 6, 7, 8, 9);
    errcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, 10, -1, 11, 12, 13);
    errcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, 14, 15, -1, 16, 17);
    errcode = OSPPTransactionSetVideoJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 1, 2, 3, 4, 5);
    errcode = OSPPTransactionSetVideoJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, -1, 12, 14, 16, 18);
    errcode = OSPPTransactionSetVideoJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, 20, -1, 22, 24, 26);
    errcode = OSPPTransactionSetVideoJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, 28, 30, -1, 32, 34);

    return errcode;
}

int testStatsDelay()
{
    int errcode = 0;

    errcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 1, 2, 3, 4, 5);
    errcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, -1, 6, 7, 8, 9);
    errcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, 10, -1, 11, 12, 13);
    errcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, 14, 15, -1, 16, 17);
    errcode = OSPPTransactionSetVideoDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 2, 4, 6, 8, 10);
    errcode = OSPPTransactionSetVideoDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, -1, 12, 14, 16, 18);
    errcode = OSPPTransactionSetVideoDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, 20, -1, 22, 24, 26);
    errcode = OSPPTransactionSetVideoDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, 28, 30, -1, 32, 34);

    return errcode;
}

int testStatsRTDelay()
{
    int errcode = 0;

    errcode = OSPPTransactionSetRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SLEG_SOURCE, 1, 2, 3, 4, 5);
    errcode = OSPPTransactionSetRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SLEG_DESTINATION, -1, 6, 7, 8, 9);
    errcode = OSPPTransactionSetRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SLEG_SOURCE, 10, -1, 11, 12, 13);
    errcode = OSPPTransactionSetRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SLEG_DESTINATION, 14, 15, -1, 16, 17);
    errcode = OSPPTransactionSetVideoRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SLEG_SOURCE, 2, 4, 6, 8, 10);
    errcode = OSPPTransactionSetVideoRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SLEG_DESTINATION, -1, 12, 14, 16, 18);
    errcode = OSPPTransactionSetVideoRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SLEG_SOURCE, 20, -1, 22, 24, 26);
    errcode = OSPPTransactionSetVideoRTDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SLEG_DESTINATION, 28, 30, -1, 32, 34);

    return errcode;
}

int testStatsOctets()
{
    int errcode = 0;

    errcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 1);
    errcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, 2);
    errcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, -1);
    errcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, 3);
    errcode = OSPPTransactionSetVideoOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 2);
    errcode = OSPPTransactionSetVideoOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, 4);
    errcode = OSPPTransactionSetVideoOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, -1);
    errcode = OSPPTransactionSetVideoOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, 6);

    return errcode;
}

int testStatsPackets()
{
    int errcode = 0;

    errcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 1);
    errcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, 2);
    errcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, 3);
    errcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, -1);
    errcode = OSPPTransactionSetVideoPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_SRCREP, 2);
    errcode = OSPPTransactionSetVideoPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SDIR_DESTREP, 4);
    errcode = OSPPTransactionSetVideoPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_SRCDEST, 6);
    errcode = OSPPTransactionSetVideoPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SDIR_DESTSRC, -1);

    return errcode;
}

int testStatsRFactor()
{
    int errcode = 0;

    errcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SDIR_SRCREP, 1.1);
    errcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SDIR_DESTREP, 2.2);
    errcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SDIR_SRCDEST, -1);
    errcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SDIR_DESTSRC, 3.3);

    return errcode;
}

int testStatsMOSCQ()
{
    int errcode = 0;

    errcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SDIR_SRCREP, 1.1);
    errcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SDIR_DESTREP, 2.2);
    errcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SDIR_SRCDEST, 3.3);
    errcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SDIR_DESTSRC, -1);

    return errcode;
}

int testStatsMOSLQ()
{
    int errcode = 0;

    errcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SDIR_SRCREP, 1.1);
    errcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SDIR_DESTREP, 2.2);
    errcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SDIR_SRCDEST, -1);
    errcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SDIR_DESTSRC, 3.3);

    return errcode;
}

int testStatsICPIF()
{
    int errcode = 0;

    errcode = OSPPTransactionSetICPIF(OSPVTransactionHandle, OSPC_SDIR_SRCREP, 1);
    errcode = OSPPTransactionSetICPIF(OSPVTransactionHandle, OSPC_SDIR_DESTREP, 54);

    return errcode;
}

int testSetRealm()
{
    int errcode = 0;

    errcode = OSPPTransactionSetSrcRealm(OSPVTransactionHandle, "SourceRealm");
    errcode = OSPPTransactionSetDestRealm(OSPVTransactionHandle, "DestinationRealm");

    return errcode;
}

int testSetCDRProxy()
{
    int errcode = 0;

    errcode = OSPPTransactionSetCDRProxy(OSPVTransactionHandle, "CDRProxyHost", "CDRProxyFolder", "CDRProxySubfolder");

    return errcode;
}

int testSetJIP()
{
    int errcode = 0;

    errcode = OSPPTransactionSetJIP(OSPVTransactionHandle, "404526");

    return errcode;
}

int testGetDestSwitchId()
{
    char swid[OSPC_SIZE_NORID];
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionGetDestSwitchId(OSPVTransactionHandle, sizeof(swid), swid);
    if (errcode == OSPC_ERR_NO_ERROR) {
        printf("Destination switch ID: %s\n", swid);
    }

    return errcode;
}

int testSetChargingVector()
{
    const char *icid = "PCVICID";
    const char *genat = "PCVGENAT";
    const char *origioi = "PCVORIGIOI";
    const char *termioi = "PCVTERMIOI";
    int errcode = 0;

    errcode = OSPPTransactionSetChargingVector(OSPVTransactionHandle, icid, genat, origioi, termioi);

    return errcode;
}

int testSetUserRatePlan()
{
    int errcode = 0;

    errcode = OSPPTransactionSetUserRatePlan(OSPVTransactionHandle, OSPC_CPARTY_SOURCE, "SourceUserRatePlan");
    errcode = OSPPTransactionSetUserRatePlan(OSPVTransactionHandle, OSPC_CPARTY_DESTINATION, "DestinationUserRatePlan");

    return errcode;
}

int testGetIdentity()
{
    char identity[OSPC_SIZE_HEADER];
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionGetIdentity(OSPVTransactionHandle, sizeof(identity), identity);
    if (errcode == OSPC_ERR_NO_ERROR) {
        printf("Identity: %s\n", identity);
    }

    return errcode;
}

int testGetTermCause()
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTBOOL has = OSPC_FALSE;
    unsigned code = 0;
    char desc[OSPC_SIZE_NORSTR] = {0};

    errcode = OSPPTransactionHasTermCause(OSPVTransactionHandle, OSPC_TCAUSE_SIP, &has);
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (has) {
            errcode = OSPPTransactionGetTCCode(OSPVTransactionHandle, OSPC_TCAUSE_SIP, &code);
            if (errcode == OSPC_ERR_NO_ERROR) {
                printf("TCCode: %d\n", code);

                errcode = OSPPTransactionGetTCDesc(OSPVTransactionHandle, OSPC_TCAUSE_SIP, sizeof(desc), desc);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    printf("TCDesc: %s\n", desc);
                }
            }
        } else {
            printf("Without termination cause\n");
        }
    }

    return errcode;
}

int testGetVerstat()
{
    OSPE_VERIFICATION_STATUS status = OSPC_VSTATUS_UNKNOWN;
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionGetVerstat(OSPVTransactionHandle, &status);
    if (errcode == OSPC_ERR_NO_ERROR) {
        printf("Verstat: %d\n", status);
    }

    return errcode;
}

int testGetAttest()
{
    char attest[OSPC_SIZE_NORSTR];
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionGetAttestInfo(OSPVTransactionHandle, sizeof(attest), attest);
    if (errcode == OSPC_ERR_NO_ERROR) {
        printf("Attestation: %s\n", attest);
    }

    return errcode;
}

int testGetOrigId()
{
    char origid[OSPC_SIZE_NORID];
    int errcode = OSPC_ERR_NO_ERROR;

    errcode = OSPPTransactionGetOrigId(OSPVTransactionHandle, sizeof(origid), origid);
    if (errcode == OSPC_ERR_NO_ERROR) {
        printf("OriginationId: %s\n", origid);
    }

    return errcode;
}

int testAPI(int apinumber)
{
    OSPTTHREADID MultProviderThrId[OSPC_MAX_PROVIDERS];
    int i, errcode = 0;
    int trans_to_run, num_providers;
    int build_new_trans = 0;

    switch (apinumber) {

    case 1:
        errcode = testOSPPProviderNew(&OSPVProviderHandle);
        break;
    case 2:
        errcode = testOSPPProviderDelete();
        break;
    case 3:
        errcode = testNotImplemented();
        /*errcode = testOSPPProviderGetAuthorityCertificates(); */
        break;
    case 4:
        errcode = testOSPPProviderSetServicePoints();
        break;
    case 5:
        errcode = testOSPPProviderGetHTTPMaxConnections();
        break;
    case 6:
        errcode = testOSPPProviderSetHTTPMaxConnections();
        break;
    case 7:
        errcode = testOSPPProviderGetHTTPPersistence();
        break;
    case 8:
        errcode = testOSPPProviderSetHTTPPersistence();
        break;
    case 9:
        errcode = testOSPPProviderGetHTTPRetryDelay();
        break;
    case 10:
        errcode = testOSPPProviderSetHTTPRetryDelay();
        break;
    case 11:
        errcode = testOSPPProviderGetHTTPTimeout();
        break;
    case 12:
        errcode = testOSPPProviderSetHTTPTimeout();
        break;
    case 13:
        errcode = testNotImplemented();
        break;
    case 14:
        errcode = testOSPPProviderSetCapabilitiesURLs();
        break;
    case 15:
        errcode = testOSPPProviderGetLocalValidation();
        break;
    case 16:
        errcode = testOSPPProviderSetLocalValidation();
        break;
    case 17:
        errcode = testOSPPProviderGetServicePoints();
        break;
    case 18:
        errcode = testOSPPSetServicePoints();
        break;
    case 19:
        errcode = testOSPPProviderGetSSLLifetime();
        break;
    case 20:
        errcode = testOSPPProviderSetSSLLifetime();
        break;
    case 21:
        errcode = testOSPPProviderGetNumberOfAuthorityCertificates();
        break;
    case 22:
        errcode = testOSPPProviderGetNumberOfServicePoints();
        break;
    case 23:
        errcode = testOSPPTransactionNew();
        break;
    case 24:
        errcode = testOSPPTransactionDelete();
        break;
    case 25:
        errcode = testOSPPTransactionAccumulateOneWayDelay();
        break;
    case 26:
        errcode = testOSPPTransactionAccumulateTwoWayDelay();
        break;
    case 27:
        errcode = testOSPPTransactionGetFirstDestination();
        break;
    case 28:
        errcode = testOSPPTransactionGetNextDestination();
        break;
    case 29:
        errcode = testOSPPTransactionRequestAuthorisation();
        break;
    case 30:
        errcode = testOSPPTransactionRequestSuggestedAuthorisation();
        break;
    case 31:
        errcode = testOSPPTransactionValidateAuthorisation();
        break;
    case 32:
        errcode = testOSPPTransactionReportUsage();
        break;
    case 99:
        OSPM_SLEEP(2);
        break;
    case 33:
        errcode = testOSPPTransactionInitializeAtDevice(OSPC_ROLE_SOURCE);
        break;
    case 34:
        errcode = testOSPPTransactionInitializeAtDevice(OSPC_ROLE_DESTINATION);
        break;
    case 35:
        errcode = testOSPPTransactionSetNetworkId();
        break;
    case 36:
        errcode = testOSPPTransactionRecordFailure();
        break;
    case 37:
        errcode = testOSPPTransactionIndicateCapabilities();
        break;
    case 38:
        errcode = testOSPPTransactionRequestReauthorisation();
        break;
    case 39:
        errcode = testOSPPTransactionGetDestProtocol();
        break;
    case 40:
        errcode = testOSPPTransactionIsDestOSPEnabled();
        break;
    case 41:
        errcode = testTestCalls();
        break;
    case 42:
        errcode = testOSPToolkitVersion();
        break;
    case 43:
        printf("Build a new transaction ? Press 1 for Yes, 0 for No : ");
        if (scanf("%d", &build_new_trans) == 1) {
            errcode = testBuildUsageFromScratch(OSPC_ROLE_SOURCE, build_new_trans);
        } else {
            errcode = -1;
        }
        getchar();
        break;
    case 44:
        printf("Build a new transaction ? Press 1 for Yes, 0 for No : ");
        if (scanf("%d", &build_new_trans) == 1) {
            errcode = testBuildUsageFromScratch(OSPC_ROLE_DESTINATION, build_new_trans);
        } else {
            errcode = -1;
        }
        getchar();
        break;
    case 45:
        errcode = testOSPPTransactionGetLookAheadInfoIfPresent();
        break;
    case 46:
        errcode = testOSPPTransactionModifyDeviceIdentifiers();
        break;
    case 47:
        errcode = testOSPPTransactionModifyDeviceIdentifiersAgain();
        break;
    case 48:
        errcode = testSetDestinationCount();
        break;
    case 50:
        errcode = testSetCallingNumber();
        break;
    case 51:
        errcode = testSetCalledNumber();
        break;
    case 52:
        errcode = testGetCallingNumber();
        break;
    case 53:
        errcode = testGetCalledNumber();
        break;
    case 54:
        errcode = testSetCallId();
        break;
    case 55:
        errcode = testOSPPTransactionSetPricingInfo();
        break;
    case 56:
        errcode = testSetDuration();
        break;
    case 57:
        errcode = testSetTCCode();
        break;
    case 58:
        errcode = testSetStartTime();
        break;
    case 59:
        errcode = testSetEndTime();
        break;
    case 60:
        errcode = testSetAlertTime();
        break;
    case 61:
        errcode = testSetConnectTime();
        break;
    case 62:
        errcode = testSetServiceType();
        break;
    case 63:
        errcode = testGetServiceType();
        break;
    case 100:
        printf("Enter the number of Providers to be created .. ");
        if (scanf("%d", &num_providers) == 1) {
            if (num_providers > OSPC_MAX_PROVIDERS) {
                printf("Cannot run the test. The entered value is greater than the maximum providers allowed\n");
                errcode = OSPC_ERR_PROV_MAX_PROVIDERS;
            } else {
                printf("Enter the number of Transactions to be run .. ");
                if (scanf("%d", &trans_to_run) == 1) {
                    if (2 * trans_to_run > OSPC_MAX_TRANS) {
                        errcode = OSPC_ERR_TRAN_NO_TRANS_SPACE;
                        printf("Cannot run the test. The entered value is greater than the maximum transactions allowed\n");
                    }

                    if ((errcode == 0) && (trans_to_run > MAX_QUEUE_SIZE)) {
                        printf("Warning !!! The toolkit may not be able to process - %d calls because the maximum queue size is - %d\n",
                             trans_to_run, MAX_QUEUE_SIZE);
                    }

                    /*
                     * Launch the threads
                     */
                    if (errcode == 0) {
                        for (i = 0; i < num_providers; i++) {
                            OSPM_CREATE_THREAD(MultProviderThrId[i],
                                NULL,
                                testNonBlockingPerformanceTest,
                                (void *)&trans_to_run, errcode);

                            printf("Created Thread [%d] with thread id: [%lu]\n", i, (unsigned long int)MultProviderThrId[i]);
                        }
                        for (i = 0; i < num_providers; i++) {
                            OSPM_THR_JOIN(MultProviderThrId[i], NULL);
                        }
                    }
                }
                getchar();
            }
        } else {
            errcode = -1;
        }
        getchar();
        break;
    case 101:
        errcode = testNonBlockingPerformanceTestForCapabilities();
        break;
    case 200:
        errcode = testSetRoleInfo();
        break;
    case 201:
        errcode = testSetTermCause();
        break;
    case 210:
        errcode = testSetNumberPortability();
        break;
    case 211:
        errcode = testSetOperatorName();
        break;
    case 216:
        errcode = testSetRealm();
        break;
    case 217:
        errcode = testSetApplicationId();
        break;
    case 218:
        errcode = testSetSIPHeaders();
        break;
    case 219:
        errcode = testSetTotalSetupAttempts();
        break;
    case 220:
        errcode = testSetProtocol();
        break;
    case 221:
        errcode = testSetCodec();
        break;
    case 222:
        errcode = testSetNetworkId();
        errcode = testSetSwitchId();
        break;
    case 223:
        errcode = testSetSessionId();
        break;
    case 224:
        errcode = testSetCustomInfo(1);
        break;
    case 225:
        errcode = testSetReleaseSource();
        break;
    case 226:
        errcode = testSetCallingPartyInfo();
        errcode = testSetCalledPartyInfo();
        break;
    case 227:
        errcode = testSetTransferId();
        break;
    case 228:
        errcode = testSetTransferStatus();
        break;
    case 229:
        errcode = testSetNetTransCalled();
        break;
    case 230:
        errcode = testSetSrcServiceProvider();
        errcode = testSetDestServiceProvider();
        break;
    case 231:
        errcode = testSetSystemId();
        break;
    case 232:
        errcode = testSetRelatedReason();
        break;
    case 233:
        errcode = testSetCDRProxy();
        break;
    case 234:
        errcode = testSetUserAgent();
        break;
    case 235:
        errcode = testSetSrcMediaAddresses();
        errcode = testSetDestMediaAddresses();
        break;
    case 236:
        errcode = testSetProxyIngressAddresses();
        errcode = testSetProxyEgressAddresses();
        break;
    case 237:
        errcode = testSetProviderPDD(2010);
        break;
    case 238:
        errcode = testSetJIP();
        break;
    case 239:
        errcode = testSetCallType();
        break;
    case 240:
        errcode = testSetChargingVector();
        break;
    case 241:
        errcode = testSetUserRatePlan();
        break;
    case 250:
        errcode = testGetNumberPortability();
        break;
    case 251:
        errcode = testGetOperatorName();
        break;
    case 252:
        errcode = testGetURL();
        break;
    case 253:
        errcode = testGetCNAM();
        break;
    case 254:
        errcode = testGetDestSwitchId();
        break;
    case 255:
        errcode = testGetIdentity();
        break;
    case 256:
        errcode = testGetTermCause();
        break;
    case 257:
        errcode = testGetVerstat();
        break;
    case 258:
        errcode = testGetAttest();
        break;
    case 259:
        errcode = testGetOrigId();
        break;
    case 300:
        errcode = testStatsLost();
        break;
    case 301:
        errcode = testStatsJitter();
        break;
    case 302:
        errcode = testStatsDelay();
        break;
    case 303:
        errcode = testStatsRTDelay();
        break;
    case 304:
        errcode = testStatsOctets();
        break;
    case 305:
        errcode = testStatsPackets();
        break;
    case 306:
        errcode = testStatsRFactor();
        break;
    case 307:
        errcode = testStatsMOSCQ();
        break;
    case 308:
        errcode = testStatsMOSLQ();
        break;
    case 309:
        errcode = testStatsICPIF();
        break;
    case 999:
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPProviderNew(&OSPVProviderHandle);
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPTransactionNew();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetNetworkId();
            errcode = testSetSwitchId();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetOperatorName();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetSIPHeaders();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetApplicationId();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetRealm();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetUserAgent();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetSrcMediaAddresses();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetProxyIngressAddresses();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetJIP();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetSrcServiceProvider();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetCallingPartyInfo();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetCustomInfo(1);
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetChargingVector();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPTransactionRequestAuthorisation();
        }
        /* 1st destination */
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPTransactionGetFirstDestination();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetDestMediaAddresses();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetProxyEgressAddresses();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetProviderPDD(3010);
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetDestServiceProvider();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPTransactionRecordFailure();
        }
        /* 2nd destination */
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPTransactionGetNextDestination();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = OSPPTransactionSetDestinationCount(OSPVTransactionHandle, 2);
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetTotalSetupAttempts();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetRoleInfo();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetTermCause();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetRealm();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetProtocol();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetDestMediaAddresses();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetProxyEgressAddresses();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetProviderPDD(4010);
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetCodec();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetSessionId();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetCustomInfo(0);
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetReleaseSource();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetCalledPartyInfo();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetTransferId();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetTransferStatus();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetNetTransCalled();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetSystemId();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetRelatedReason();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetCDRProxy();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testSetCallType();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsLost();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsJitter();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsDelay();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsRTDelay();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsOctets();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsPackets();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsRFactor();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsMOSCQ();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsMOSLQ();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testStatsICPIF();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            call_start_time = time(NULL);
            call_alert_time = call_start_time + 3;
            call_connect_time = call_start_time + 14;
            call_end_time = call_start_time + 38;
            errcode = testOSPPTransactionReportUsage();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPTransactionDelete();
        }
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = testOSPPProviderDelete();
        }
        break;
    default:
        errcode = -1;
    }

    return errcode;
}

int testTestCalls()
{
    register int i = 0, j = 0;
    int errcode = OSPC_ERR_NO_ERROR;
    time_t start_time, end_time;
#define NUM_API_OPS 7
    int apinumber_idx[NUM_API_OPS] = { 23, 29, 27, 28, 31, 32, 24 };

    time(&start_time);
    quietmode = 1;
    while (i < num_test_calls && errcode == OSPC_ERR_NO_ERROR) {
        for (j = 0; j < NUM_API_OPS; j++) {
            if (apinumber_idx[j] == 28 && numdestinations == 1)
                continue;
            errcode = testAPI(apinumber_idx[j]);
            if (errcode != OSPC_ERR_NO_ERROR) {
                fprintf(stderr, "Test Failed. Iteration = %d Function = %d ErrorCode = %d\n",
                    i, apinumber_idx[j], errcode);
                break;
            }
        }
        i++;
    }
    time(&end_time);
    printf("Elapsed Time: %ld seconds\n", end_time - start_time);
    quietmode = 0;
    return errcode;
}

int testMenu()
{
    char inbuf[256];
    int funcnum;

    if (!quietmode) {
        printf("\nProvider API functions\n");
        printf("---------------------------------------------------------------------\n");
        printf(" 1) New                                2) Delete\n");
        printf(" 3) For future Enhancements            4) SetServicePoints\n");
        printf(" 5) GetHTTPMaxConnections              6) SetHTTPMaxConnections\n");
        printf(" 7) GetHTTPPersistence                 8) SetHTTPPersistence\n");
        printf(" 9) GetHTTPRetryDelay                 10) SetHTTPRetryDelay\n");
        printf("11) GetHTTPTimeout                    12) SetHTTPTimeout\n");
        printf("13) For future Enhancements           14) SetCapabilitiesURLs\n");
        printf("15) GetLocalValidation                16) SetLocalValidation\n");
        printf("17) GetServicePoints                  18) SetServicePoints\n");
        printf("19) GetSSLLifetime                    20) SetSSLLifetime\n");
        printf("21) GetNumberOfAuthorityCertificates  22) GetNumberOfServicePoints\n");
        printf("---------------------------------------------------------------------\n");
        printf("Transaction API functions\n");
        printf("---------------------------------------------------------------------\n");
        printf("23) New                               24) Delete\n");
        printf("25) AccumulateOneWayDelay             26) AccumulateTwoWayDelay\n");
        printf("27) GetFirstDestination               28) GetNextDestination\n");
        printf("29) RequestAuthorisation              30) RequestSuggestedAuthorization\n");
        printf("31) ValidateAuthorisation             32) ReportUsage\n");
        printf("33) TransactionInitializeAtDevice(OGW)34) TransactionInitialize(TGW)\n");
        printf("35) SetNetworkId                      36) TransactionRecordFailure\n");
        printf("37) IndicateCapabilities              38) RequestReauthorization\n");
        printf("---------------------------------------------------------------------\n");
        printf("Miscellaneous Tests\n");
        printf("---------------------------------------------------------------------\n");
        printf("39) GetDestinationProtocol            40) IsDestOSPEnabled\n");
        printf("41) %-6d Test Calls                 42) Get OSP Toolkit Version\n", num_test_calls);
        printf("43) BuildUsageFromScratch(OGW)        44) BuildUsageFromScratch(TGW)\n");
        printf("45) GetLookAheadInfoIfPresent         46) ModifyDeviceIdentifiers\n");
        printf("47) ModifyDeviceIdentifiersAgain      48) SetDestinationCount\n");
        printf("99) Sleep for 2 seconds\n");
        printf("---------------------------------------------------------------------\n");
        printf("Configuration Parameters\n");
        printf("---------------------------------------------------------------------\n");
        printf("50) Set Calling Number                51) Set Called Number\n");
        printf("52) Get Calling Number                53) Get Called Number\n");
        printf("54) Set CallId to Empty for Token Validation\n");
        printf("55) Set Pricing Info\n");
        printf("56) Set Duration                      57) Set TC Code\n");
        printf("58) Set Start Time                    59) Set End Time\n");
        printf("60) Set Alert Time                    61) Set Connect Time\n");
        printf("62) Set Service Type                  63) Get Service Type\n");
        printf("---------------------------------------------------------------------\n");
        printf("Performance tests\n");
        printf("---------------------------------------------------------------------\n");
        printf("100) Run Multiple calls\n");
        printf("101) Run Multiple Capabilities Indications\n");
        printf("---------------------------------------------------------------------\n");
        printf("Other tests\n");
        printf("---------------------------------------------------------------------\n");
        printf("200) Set Role Info                    201) Termination Cause\n");
        printf("210) Set NP Parameters                211) Set Operator Names\n");
        printf("216) Set Realms                       217) Set Application ID\n");
        printf("218) Set SIP Headers                  219) Set Total Setup Attempts\n");
        printf("220) Set Signaling Protocol           221) Set Codec\n");
        printf("222) Set Network/Switch ID            223) Set Session ID\n");
        printf("224) Set Custom Info                  225) Set Release Source\n");
        printf("226) Set Call Party Info              227) Set Transfer ID\n");
        printf("228) Set Transfer Status              229) Set Network Translated Called Number\n");
        printf("230) Set Service Provider ID          231) Set System ID\n");
        printf("232) Set Related Call-ID Reason       233) Set CDR Proxy\n");
        printf("234) Set User Agent                   235) Set Media Addresses\n");
        printf("236) Set Proxy Addresses              237) Set Provider PDD\n");
        printf("238) Set JIP                          239) Set Call Type\n");
        printf("240) Set Charging Vector              241) Set User Rate Plan\n");
        printf("250) Get NP parameters                251) Get Operator Names\n");
        printf("252) Get URLs                         253) Get CNAM\n");
        printf("254) Get Destination Switch ID        255) Get Identity\n");
        printf("256) Get Termination Cause            257) Get Verification status\n");
        printf("258) Get Attestation                  259) Get Origination ID\n");
        printf("300) Set Lost                         301) Set Jitter\n");
        printf("302) Set Delay                        303) Set Round Trip Delay\n");
        printf("304) Set Octets                       305) Set Packets\n");
        printf("306) Set R-Factor                     307) Set MOS-CQ\n");
        printf("308) Set MOS-LQ                       309) Set ICPIF\n");
        printf("---------------------------------------------------------------------\n");
        printf("Enter function number or 'q' to quit => ");
    }
    if (fgets(inbuf, 256, stdin) != NULL) {
        if (inbuf[0] == 'q' || inbuf[0] == 'Q') {
            funcnum = -1;
        } else {
            funcnum = atoi(inbuf);
        }
    } else {
        funcnum = 0;
    }

    return funcnum;
}

int GetConfiguration()
{
    FILE *fp = NULL;
    int errcode = 0;
    char inbuf[512];
    char tmp_addr[20];
    int spindex = 0, Capspindex = 0;

    if ((fp = fopen(CONFIG_FILENAME, "r")) == (FILE *)NULL) {
        fprintf(stderr, "Cannot open config file %s for reading. %s\n",
                CONFIG_FILENAME, strerror(errno));
        errcode = errno;
    } else {
        while (fgets(inbuf, 512, fp)) {
            if (inbuf[0] == '#' || inbuf[0] == '\n')
                continue;

            inbuf[OSPM_STRLEN(inbuf) - 1] = '\0';

            if (OSPM_STRNCMP(inbuf, "AUDIT=", 6) == 0) {
                auditurl = _Strdup(&inbuf[6]);
            } else if (OSPM_STRNCMP(inbuf, "SP=", 3) == 0) {
                servicepoints[num_serv_points++] = _Strdup(&inbuf[3]);

                if (num_serv_points == MAX_SERVICE_POINTS) {
                    fprintf(stderr,
                            "Too many service points configured. Max = %d\n",
                            MAX_SERVICE_POINTS);
                    errcode = 1;
                    break;
                }
            } else if (OSPM_STRNCMP(inbuf, "CapURL=", OSPM_STRLEN("CapURL=")) == 0) {
                capURLs[num_capURLs++] = _Strdup(&inbuf[OSPM_STRLEN("CapURL=")]);

                if (num_capURLs == MAX_SERVICE_POINTS) {
                    fprintf(stderr,
                            "Too many capabilities URLs configured. Max = %d\n",
                            MAX_SERVICE_POINTS);
                    errcode = 1;
                    break;
                }
            } else if (OSPM_STRNCMP(inbuf, "CUSTID=", 7) == 0) {
                custid = atol((const char *)&inbuf[7]);
            } else if (OSPM_STRNCMP(inbuf, "DEVID=", 6) == 0) {
                devid = atol((const char *)&inbuf[6]);
            } else if (OSPM_STRNCMP(inbuf, "CALLED=", 7) == 0) {
                OSPM_STRCPY(callednumber, (&inbuf[7]));
            } else if (OSPM_STRNCMP(inbuf, "CALLING=", 8) == 0) {
                OSPM_STRCPY(callingnumber, (&inbuf[8]));
            } else if (OSPM_STRNCMP(inbuf, "TEST_CALLS=", 11) == 0) {
                num_test_calls = atoi(&inbuf[11]);
            } else if (OSPM_STRNCMP(inbuf, "TOKENALGO=", 10) == 0) {
                tokenalgo = (token_algo_t) atoi(&inbuf[10]);
            } else if (OSPM_STRNCMP(inbuf, "SRC=", 4) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[4]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    SourceIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "SRCDEV=", 7) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[7]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    SourceDevIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "DST=", 4) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[4]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    DestIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "DSTDEV=", 7) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[7]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    DestDevIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "OUTOFRESOURCES=", 15) == 0) {
                almostOutOfResources = atoi(&inbuf[15]);
            } else if (OSPM_STRNCMP(inbuf, "HWSUPPORT=", 10) == 0) {
                hardwareSupport = atoi(&inbuf[10]);
            } else if (OSPM_STRNCMP(inbuf, "TCCODE=", 7) == 0) {
                TCcode = atoi(&inbuf[7]);
            } else if (OSPM_STRNCMP(inbuf, "CALLING_NUM_FORMAT=", 19) == 0) {
                CallingNumFormat = (OSPE_NUMBER_FORMAT)atoi(&inbuf[19]);
            } else if (OSPM_STRNCMP(inbuf, "CALLED_NUM_FORMAT=", 18) == 0) {
                CalledNumFormat = (OSPE_NUMBER_FORMAT)atoi(&inbuf[18]);
            } else if (OSPM_STRNCMP(inbuf, "MSGCOUNT=", 9) == 0) {
                if (OSPM_STRCMP(&inbuf[9], "NULL") == 0) {
                    MsgCount = NULL;
                } else {
                    SPMsgCount[spindex++] = atoi(&inbuf[9]);
                }
            } else if (OSPM_STRNCMP(inbuf, "DEF_HTTP_MAXCONN=", 17) == 0) {
                DEF_HTTP_MAXCONN = atoi(&inbuf[17]);
                WORK_THREAD_NUM = DEF_HTTP_MAXCONN;
            } else if (OSPM_STRNCMP(inbuf, "CapMSGCOUNT=", 12) == 0) {
                if (OSPM_STRCMP(&inbuf[12], "NULL") == 0) {
                    CapMsgCount = NULL;
                } else {
                    CapSPMsgCount[Capspindex++] = atoi(&inbuf[12]);
                }
            } else if (OSPM_STRNCMP(inbuf, "ModifiedSRC=", 12) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[12]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    ModifiedSourceIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "ModifiedSRCDEV=", 15) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[15]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    ModifiedSourceDevIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "ModifiedDST=", 12) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[12]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    ModifiedDestIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "ModifiedDSTDEV=", 15) == 0) {
                OSPM_STRCPY(tmp_addr, (&inbuf[15]));
                if (OSPM_STRCMP(tmp_addr, "0") != 0) {
                    ModifiedDestDevIP = _Strdup(tmp_addr);
                }
            } else if (OSPM_STRNCMP(inbuf, "IS_PDD_INFO_AVAILABLE=", 22) == 0) {
                IS_PDD_INFO_AVAILABLE = atoi(&inbuf[22]);
            }
        }
        fclose(fp);
    }

    if (num_serv_points == 0 || auditurl == NULL) {
        fprintf(stderr,
                "Both service points and an audit URL must be configured\n");
        errcode = 1;
    }
    return errcode;
}

void CleanupServicePoints()
{
    int i = 0;

    for (i = 0; i < num_serv_points; i++) {
        free(servicepoints[i]);
    }

    for (i = 0; i < num_capURLs; i++) {
        free(capURLs[i]);
    }
    free(auditurl);
}


int main(int argc, char *argv[])
{
    int menuopt = 1, retcode = 0;

    OSPTNINITLOG("test_app.log");

    if (GetConfiguration()) {
        exit(1);
    }
    if (argc == 2) {
        if (OSPM_STRCMP(argv[1], "-q") == 0)
            quietmode = 1;
        else {
            if (servicepoints[0] != OSPC_OSNULL)
                OSPM_FREE(servicepoints[0]);

            servicepoints[0] = _Strdup(argv[1]);
        }
    }

    if ((retcode = OSPPInit((OSPTBOOL) hardwareSupport)) != OSPC_ERR_NO_ERROR) {
        printf("initialization failed. errcode = %d\n", retcode);
        exit(1);
    }

    NonBlockingQueueMonitorNew(&nbMonitor, WORK_THREAD_NUM, MAX_QUEUE_SIZE, (500 * 1000));

    do {
        menuopt = testMenu();
        if (menuopt >= 0) {
            retcode = testAPI(menuopt);
            if (!quietmode) {
                printf("function return code = %d\n", retcode);
                printf("press any key to continue...");
                getchar();
            }
        }

    } while (menuopt >= 0);


    NonBlockingQueueMonitorDelete(&nbMonitor);

    OSPPCleanup();
    CleanupServicePoints();
    printf("Program Over.\n");
    return 0;
}



/********************************************************
*
* Function name : testNonBlockingPerformanceTest
* Description   : This function simulates O and T gateways
*                 There are 5 phases.
*                 Phase I:    Create new transactions for O and T
*                 Phase II:   Request authorization for O
*                 Phase IIIO: Get first destination for O
*                 Phase IIIT: Validate authorization for T
*                 Phase IV:   Report Usage for O and T
*                 Phase V:    Delete transactions for O and T
*
*                 Each Phase is timed in seconds.
*
*                 Phases I, III, and V are non-blocking and a
*                 single thread is used to perform theses operations.
*
*                 Phases II and IV are blocking and, in order to
*                 increase performance by executing blocking requests
*                 in parallel, NonBlockingQueueMonitor module is used.
*
*                 NonBlockingQueueMonitor module allows consumers,
*                 AuthorisationRequest and ReportUsage, to put requests
*                 on the non-blocking queue.  These messages are later
*                 processed by a pull of work threads that make blocking
*                 ToolKit API calls in parallel.
*
*                 Non-Blocking versions of AuthorisationRequest and
*                 ReportUsage, in addition to the parameters that the
*                 corresponding blocking functions take, take a handle
*                 to the NonBlockingQueueMonitor module and pointer to the
*                 place where return value from the blocking ToolKit API
*                 should be stored.  Initially the return value is set to
*                 OSPC_AUTH_REQUEST_BLOCK or OSPC_REPORT_USAGE_BLOCK
*                 (both defined in osptrans.h) and, when the ToolKit blocking
*                 function returns, the value is set to the actual return value.
*
*                 In order to check for the status of the Non-Blocking transaction,
*                 check the value of the 2nd parameter.  If it equals to
*                 OSPC_AUTH_REQUEST_BLOCK or OSPC_REPORT_USAGE_BLOCK than the
*                 transaction is still in progress.
*
*                 In order to make sure that all Non-Blocking transactions have
*                 completed, use the NonBlockingQueueMonitorBlockWhileQueueNotEmpty mehtod
*
* Returns       : 0 on Success
*
********************************************************/
OSPTTHREADRETURN testNonBlockingPerformanceTest(void *arg)
{
    int errcode = 0;
    unsigned detaillogsize = 0;

    int *OErrorCodes = NULL;
    int *TErrorCodes = NULL;
    OSPTTRANHANDLE *OTransactionHandles = NULL;
    OSPTTRANHANDLE *TTransactionHandles = NULL;
    unsigned *NumOfDestinations = NULL;
    unsigned *authorised = NULL;
    unsigned *TokenSizes = NULL;
    OSPT_CALL_ID **CallIds = NULL;
    unsigned *CallIdsNum = NULL;
    unsigned *CallIdsLen = NULL;
    int i = 0;

    OSPTPROVHANDLE provHandle;
    OSPTTHREADID thr_id;
    int *test_calls = (int *)arg;
    int num_test_calls = *test_calls;
    char **Tokens;
    NBMONITOR *nbMonitor = NULL;

    char Localcallednumber[CALLED_NUM_SZ];
    char Localcallingnumber[CALLING_NUM_SZ];
    char Localdest[DESTINATION_SZ] = { "" };
    char Localdestdev[DESTINATION_SZ] = { "" };
    char Localvalidafter[TIMESTAMP_SZ], Localvaliduntil[TIMESTAMP_SZ];
    unsigned Localtimelimit = 0;

    /*
     * Used for calculating performance
     */
    time_t start_time, end_time;
    time_t thr_start_time, thr_end_time;

    time(&thr_start_time);
    thr_id = OSPM_THR_SELF();
    printf("Thread Id: %lu Started\n", (unsigned long int) thr_id);

    NonBlockingQueueMonitorNew(&nbMonitor, WORK_THREAD_NUM, MAX_QUEUE_SIZE, (500 * 1000));

    OSPM_STRCPY(Localcallednumber, callednumber);

    Tokens = (char **)malloc(sizeof(char *)* num_test_calls);
    if (Tokens == NULL) {
        printf("Malloc Failed !! Exiting !\n");
        exit(0);
    }

    for (i = 0; i < num_test_calls; i++) {
        Tokens[i] = (char *)malloc(sizeof(char) * TOKEN_SIZE);
        if (Tokens[i] == NULL) {
            printf("Malloc Failed !! Exiting !\n");
            exit(0);
        }
    }

    /*
     * Allocate Memory
     */
    OErrorCodes = (int *)malloc((sizeof(int) * num_test_calls));
    TErrorCodes = (int *)malloc((sizeof(int) * num_test_calls));
    OTransactionHandles = (OSPTTRANHANDLE *)malloc((sizeof(OSPTTRANHANDLE) * num_test_calls));
    TTransactionHandles = (OSPTTRANHANDLE *)malloc((sizeof(OSPTTRANHANDLE) * num_test_calls));
    NumOfDestinations = (unsigned *)malloc((sizeof(unsigned) * num_test_calls));
    authorised = (unsigned *)malloc((sizeof(unsigned) * num_test_calls));
    TokenSizes = (unsigned *)malloc((sizeof(unsigned) * num_test_calls));
    CallIds = (OSPT_CALL_ID **)malloc((sizeof(OSPT_CALL_ID *)* num_test_calls));
    CallIdsNum = (unsigned *)malloc((sizeof(unsigned) * num_test_calls));
    CallIdsLen = (unsigned *)malloc((sizeof(unsigned) * num_test_calls));

    if ((OErrorCodes == NULL) || (TErrorCodes == NULL) ||
        (OTransactionHandles == NULL)
        || (TTransactionHandles == NULL) || (NumOfDestinations == NULL) ||
        (TokenSizes == NULL)
        || (CallIds == NULL) || (CallIdsNum == NULL) || (CallIdsLen == NULL)
        || (authorised == NULL))
    {
        printf("Malloc Failed !! Exiting !\n");
        exit(0);
    }

    /*
     * Start non-blocking Queue/Monitor
     */

    /*
     * Init variables
     */
    for (i = 0; i < num_test_calls; i++) {
        /*
         * authorization codes
         */
        authorised[i] = 0;

        /*
         * ErrorCodes
         */
        OErrorCodes[i] = 0;
        TErrorCodes[i] = 0;

        /*
         * Number of requested (IN) and returned (OUT) destinations
         */
        NumOfDestinations[i] = NUM_CALL_IDS;

        /*
         * Prepare space for tokens
         */
        TokenSizes[i] = TOKEN_SIZE;
        OSPM_MEMSET(Tokens[i], 0, TOKEN_SIZE);

        /*
         * Initialize call ids
         */
        CallIdsNum[i] = 1;
        CallIdsLen[i] = OSPM_STRLEN("123");
        CallIds[i] = OSPPCallIdNew(3, (const unsigned char *)"123");
    }

    errcode = testOSPPProviderNew(&provHandle);
    printf("Thread Id: %lu, ProviderNew returned: %d, ProviderId: %d\n", (unsigned long int)thr_id, errcode, provHandle);

    /*
     * Phase I Creating new transactions / 2 (O + T) transactions for every call
     */
    printf("\n\n");
    printf("Thread Id: %lu, Phase I (Source and Destination): OSPPTransactionNew.\n", (unsigned long int)thr_id);
    time(&start_time);
    for (i = 0; i < num_test_calls; i++) {
        if ((errcode = OSPPTransactionNew(provHandle, &OTransactionHandles[i])) != OSPC_ERR_NO_ERROR ||
            (errcode = OSPPTransactionNew(provHandle, &TTransactionHandles[i])) != OSPC_ERR_NO_ERROR)
        {
            printf("OSPPTransactionNew failed, aborting the test.\n");
            exit(0);
        }
    }
    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    /*
     * Phase II Sending AuthorizationRequests
     */
    printf("\n\n");
    printf("Thread Id: %lu,Phase II (Source): OSPPTransactionRequestAuthorisation.\n", (unsigned long int)thr_id);
    time(&start_time);
    for (i = 0; i < num_test_calls; i++) {
        errcode = OSPPTransactionRequestAuthorisation_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &OErrorCodes[i],
            OTransactionHandles
            [i], SourceIP,
            SourceDevIP,
            callingnumber,
            CallingNumFormat,
            callednumber,
            CalledNumFormat,
            "919404556#4444",
            CallIdsNum[i],
            &CallIds[i],
            (const char **)
            NULL,
            &NumOfDestinations
            [i],
            &detaillogsize,
            (void *)NULL);

        if (errcode != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionRequestAuthorisation_nb failed, aborting the test.\n");
            exit(0);
        }
    }

    /*
     * Wait
     */
    printf("Waiting on queued up transactions\n");
    NonBlockingQueueMonitorBlockWhileQueueNotEmpty(nbMonitor);


    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    printf("Checking ReturnCodes.\n");
    for (i = 0; i < num_test_calls; i++) {
        if (OErrorCodes[i] != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionRequestAuthorisation failed transaction/code = <%d>/<%d>, aborting the test.\n", i, OErrorCodes[i]);
            exit(0);
        }
    }

    /*
     * Phase III (Source) Getting 1st destination
     */
    printf("\n\n");
    printf("Thread Id: %lu,Phase III (Source): OSPPTransactionGetFirstDestination.\n", (unsigned long int)thr_id);
    time(&start_time);
    for (i = 0; i < num_test_calls; i++) {
        errcode = OSPPTransactionGetFirstDestination(OTransactionHandles[i],
            /* TIMESTAMP_SZ, */
            0,
            Localvalidafter,
            Localvaliduntil,
            &Localtimelimit,
            &CallIdsLen[i],
            CallIds[i],
            CALLED_NUM_SZ,
            Localcallednumber,
            CALLING_NUM_SZ,
            Localcallingnumber,
            DESTINATION_SZ,
            Localdest,
            DESTINATION_SZ,
            Localdestdev,
            &TokenSizes[i],
            Tokens[i]);

        if (errcode != OSPC_ERR_NO_ERROR) {
            printf
                ("OSPPTransactionGetFirstDestination failed, aborting the test.\n");
            exit(0);
        }
    }

    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    /*
     * Phase III (Destination) Validate destination
     */
    printf("\n\n");
    printf("Thread Id: %lu,Phase III (Destination): OSPPTransactionValidateAuthorisation.\n", (unsigned long int)thr_id);
    time(&start_time);
    for (i = 0; i < num_test_calls; i++) {
        errcode = OSPPTransactionValidateAuthorisation_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &TErrorCodes[i],
            TTransactionHandles
            [i], SourceIP,
            Localdest, NULL,
            NULL,
            Localcallingnumber,
            CallingNumFormat,
            Localcallednumber,
            CalledNumFormat,
            CallIds[i]->Length,
            CallIds[i]->Value,
            TokenSizes[i],
            Tokens[i],
            &authorised[i],
            &Localtimelimit,
            &detaillogsize,
            (void *)NULL,
            tokenalgo);

        if (errcode != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionValidateAuthorisation_nb failed, aborting the test.\n");
            exit(0);
        }
    }

    /*
     * Wait
     */
    printf("Waiting on queued up transactions\n");
    NonBlockingQueueMonitorBlockWhileQueueNotEmpty(nbMonitor);

    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    printf("Checking ReturnCodes.\n");
    for (i = 0; i < num_test_calls; i++) {
        if (TErrorCodes[i] != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionValidateAuthorisation failed transaction/code = <%d>/<%d>, aborting the test.\n", i, TErrorCodes[i]);
            exit(0);
        }
    }

    /*
     * Phase IV Sending 2 (Source and Destination) UsageIndications for each call
     */
    printf("\n\n");
    printf("Thread Id: %lu,Phase IV (Source and Destination): OSPPTransactionReportUsage.\n", (unsigned long int)thr_id);
    time(&start_time);
    for (i = 0; i < num_test_calls; i++) {
        errcode = OSPPTransactionReportUsage_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &OErrorCodes[i],
            OTransactionHandles[i],
            duration, time(NULL) - 10, time(NULL) + 20, time(NULL) - 10, time(NULL) - 8,
            IS_PDD_INFO_AVAILABLE, 1030,            /* PDD */
            OSPC_RELEASE_UNKNOWN,                   /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06", /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize,
            (void *)NULL);
        if (errcode != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionReportUsage_nb failed, aborting the test.\n");
            exit(0);
        }

        errcode = OSPPTransactionReportUsage_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &TErrorCodes[i],
            TTransactionHandles[i],
            duration, time(NULL) - 10, time(NULL) + 20, time(NULL) - 10, time(NULL) - 8,
            IS_PDD_INFO_AVAILABLE, 1040,            /* PDD */
            OSPC_RELEASE_UNKNOWN,                   /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06", /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize,
            (void *)NULL);
        if (errcode != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionReportUsage_nb failed, aborting the test.\n");
            exit(0);
        }
    }

    /*
     * Wait
     */
    printf("Waiting on queued up transactions\n");
    NonBlockingQueueMonitorBlockWhileQueueNotEmpty(nbMonitor);

    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);


    printf("Checking ReturnCodes.\n");
    for (i = 0; i < num_test_calls; i++) {
        if (OErrorCodes[i] != OSPC_ERR_NO_ERROR) {
            printf("Source OSPPTransactionReportUsage failed transaction/code = <%d>/<%d>, aborting the test.\n", i, OErrorCodes[i]);
            exit(0);
        }

        if (TErrorCodes[i] != OSPC_ERR_NO_ERROR) {
            printf("Destination OSPPTransactionReportUsage failed transaction/code = <%d>/<%d>, aborting the test.\n", i, TErrorCodes[i]);
            exit(0);
        }
    }

    /*
     * Phase V Deleting transactions
     */
    printf("\n\n");
    printf("Thread Id: %lu,Phase V: OSPPTransactionDelete.\n",
           (unsigned long int) thr_id);
    time(&start_time);
    for (i = 0; i < num_test_calls; i++) {
        if ((errcode = OSPPTransactionDelete(OTransactionHandles[i])) != OSPC_ERR_NO_ERROR ||
            (errcode = OSPPTransactionDelete(TTransactionHandles[i])) != OSPC_ERR_NO_ERROR)
        {
            printf("OSPPTransactionDelete failed, aborting the test.\n");
            exit(0);
        }
        OSPPCallIdDelete(&CallIds[i]);
    }
    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    if (OErrorCodes != NULL)
        free(OErrorCodes);

    if (TErrorCodes != NULL)
        free(TErrorCodes);

    if (OTransactionHandles != NULL)
        free(OTransactionHandles);

    if (TTransactionHandles != NULL)
        free(TTransactionHandles);

    if (NumOfDestinations != NULL)
        free(NumOfDestinations);

    if (TokenSizes != NULL)
        free(TokenSizes);

    if (CallIds != NULL)
        free(CallIds);

    if (CallIdsNum != NULL)
        free(CallIdsNum);

    if (CallIdsLen != NULL)
        free(CallIdsLen);

    for (i = 0; i < num_test_calls; i++) {
        if (Tokens[i] != NULL) {
            free(Tokens[i]);
        }
    }

    if (Tokens != NULL)
        free(Tokens);

    if (authorised != NULL)
        free(authorised);

    errcode = OSPPProviderDelete(provHandle, DEF_TIME_LIMIT);

    if (errcode != 0) {
        printf("OSPPProviderDelete failed, aborting the test.\n");
        exit(0);
    }
    time(&thr_end_time);
    printf("Thread Id: %lu,Elapsed Time: %ld seconds\n", (unsigned long int)thr_id, thr_end_time - thr_start_time);

    NonBlockingQueueMonitorDelete(&nbMonitor);

    OSPTTHREADRETURN_NULL();
}

int testNonBlockingPerformanceTestForCapabilities()
{
    int errcode = 0;
    unsigned detaillogsize = 0;

    int *OErrorCodes = NULL;
    OSPTTRANHANDLE *OTransactionHandles = NULL;
    int i = 0;

    /*
     * Used for calculating performance
     */
    time_t start_time, end_time;

    fflush(stdin);
    printf("Enter the number of Simultaneous Calls : ");
    if (scanf("%d", &TEST_NUM) != 1) {
        TEST_NUM = OSPC_MAX_TRANS > MAX_QUEUE_SIZE ? MAX_QUEUE_SIZE : OSPC_MAX_TRANS;
    }

    if (TEST_NUM > OSPC_MAX_TRANS)
        printf("Warning !! The toolkit may not be able to process - %d Calls because the maximum transactions that can be created is - %d\n",
            TEST_NUM, OSPC_MAX_TRANS);

    if (TEST_NUM > MAX_QUEUE_SIZE)
        printf("Warning !! The toolkit may not be able to process - %d Calls because the maximum queue size is - %d\n",
            TEST_NUM, MAX_QUEUE_SIZE);

    /*
     * Allocate Memory
     */
    OErrorCodes = (int *)malloc((sizeof(int) * TEST_NUM));
    OTransactionHandles =
        (OSPTTRANHANDLE *)malloc((sizeof(OSPTTRANHANDLE) * TEST_NUM));

    if ((OErrorCodes == NULL) || (OTransactionHandles == NULL)) {
        printf("Malloc Failed !! Exiting !\n");
        exit(0);
    }

    /*
     * Start non-blocking Queue/Monitor
     */

    /*
     * Init variables
     */
    for (i = 0; i < TEST_NUM; i++) {
        /*
         * ErrorCodes
         */
        OErrorCodes[i] = 0;
    }

    /*
     * Phase I Creating new transactions transactions for every call
     */
    printf("\n\n");
    printf("Phase I OSPPTransactionNew.\n");
    time(&start_time);
    for (i = 0; i < TEST_NUM; i++) {
        if ((errcode = OSPPTransactionNew(OSPVProviderHandle, &OTransactionHandles[i])) != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionNew failed, aborting the test.\n");
            return errcode;
        }
    }
    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    /*
     * Phase II Sending CapabilitiesIndication
     */
    printf("\n\n");
    printf("Phase II OSPPTransactionIndicateCapabilities.\n");
    time(&start_time);
    for (i = 0; i < TEST_NUM; i++) {
        errcode = OSPPTransactionIndicateCapabilities_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &OErrorCodes[i],
            OTransactionHandles
            [i], SourceIP,
            SourceDevIP,
            "SrcNetworkId",
            i % 2,
            &detaillogsize,
            (void *)NULL);

        if (errcode != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionIndicateCapabilities_nb failed, aborting the test.\n");
            return errcode;
        }
    }

    /*
     * Wait
     */
    printf("Waiting on queued up transactions\n");
    NonBlockingQueueMonitorBlockWhileQueueNotEmpty(nbMonitor);

    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    printf("Checking ReturnCodes.\n");
    for (i = 0; i < TEST_NUM; i++) {
        if (OErrorCodes[i] != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionIndicateCapabilities failed transaction/code = <%d>/<%d>, aborting the test.\n", i, OErrorCodes[i]);
            return OErrorCodes[i];
        }
    }

    /*
     * Phase V Deleting transactions
     */
    printf("\n\n");
    printf("Phase V: OSPPTransactionDelete.\n");
    time(&start_time);
    for (i = 0; i < TEST_NUM; i++) {
        if ((errcode =
             OSPPTransactionDelete(OTransactionHandles[i])) !=
            OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionDelete failed, aborting the test.\n");
            return errcode;
        }
    }
    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    if (OErrorCodes != NULL)
        free(OErrorCodes);

    if (OTransactionHandles != NULL)
        free(OTransactionHandles);

    return errcode;
}
