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
char validafter[TIMESTAMP_SZ], validuntil[TIMESTAMP_SZ];
unsigned timelimit = 0;
unsigned callidsize = CALL_ID_SZ;
void *callid = NULL;
unsigned char ret_cid[CALL_ID_SZ];
unsigned char c_id[CALL_ID_SZ + 1] = { "1234567890123456" };
char callednumber[CALLED_NUM_SZ];
char callingnumber[CALLING_NUM_SZ];
char dest[DESTINATION_SZ] = { "" };
char destdev[DESTINATION_SZ] = { "" };
unsigned tokensize = TOKEN_SZ;
char c_token[TOKEN_SZ] = { "" };
void *token = NULL;
const char *New_ServicePoint = { "http://osptestserver.transnexus.com:1080/osp" };
static OSPT_CALL_ID *callids[NUM_CALL_IDS];
token_algo_t tokenalgo = TOKEN_ALGO_SIGNED;
char *SourceIP = NULL, *SourceDevIP = NULL, *DestIP = NULL, *DestDevIP = NULL;
char *ModifiedSourceIP = NULL, *ModifiedSourceDevIP = NULL, *ModifiedDestIP = NULL, *ModifiedDestDevIP = NULL;
unsigned almostOutOfResources = 0;
unsigned hardwareSupport = 0;
unsigned TCcode = 0;
unsigned duration = 30;
time_t call_start_time = 0;
time_t call_end_time = 0;
time_t call_alert_time = 0;
time_t call_connect_time = 0;
NBMONITOR *nbMonitor = NULL;
int WORK_THREAD_NUM = 30;        /* make sure that this number does not exceed DEF_HTTP_MAXCONN */

#define TOKEN_SIZE          2000
#define TEST_ERROR          1
#define MAX_QUEUE_SIZE      10000

int TEST_NUM = 0;
char **Tokens;
OSPE_NUMBER_FORMAT CallingNumFormat, CalledNumFormat;
unsigned long SPMsgCount[50];
unsigned long *MsgCount = SPMsgCount;
int DEF_HTTP_MAXCONN = 30;
unsigned IS_PDD_INFO_AVAILABLE = 0;
unsigned long CapSPMsgCount[50];
unsigned long *CapMsgCount = CapSPMsgCount;

/*
 * service point globals
 */
#define CONFIG_FILENAME     "test.cfg"
#define MAX_SERVICE_POINTS  10

char *auditurl = NULL;
char *servicepoints[MAX_SERVICE_POINTS];
char *capURLs[MAX_SERVICE_POINTS];

int num_serv_points = 0;
int num_capURLs = 0;

long custid = DEF_CUST_ID;
long devid = DEF_DEVICE_ID;

/*
 * certificate globals
 */
static unsigned NUM_CA_CERTS = 0;

int quietmode = 0;
unsigned numdestinations = NUM_CALL_IDS;
int num_test_calls = 500;

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

ACCUM accumtable10[10] = {
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

ACCUM accumtable2[2] = {
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
    int errorcode = 0;
    const char **servpts;

    servpts = &New_ServicePoint;

    errorcode = OSPPProviderSetServicePoints(OSPVProviderHandle, 1, MsgCount, servpts);

    return errorcode;
}

int testOSPPProviderNew(OSPTPROVHANDLE *ProvHandle)
{
    unsigned int i;
    int errorcode = 0;
    const char **servpts;

    char customer_id[64];
    char device_id[64];
    char searchstr[20];
    OSPT_CERT localcert;
    OSPTPRIVATEKEY privatekey;
    OSPT_CERT *authCerts[OSPC_SEC_MAX_AUTH_CERTS];
    OSPT_CERT TheAuthCert[OSPC_SEC_MAX_AUTH_CERTS];

    errorcode = OSPPUtilLoadPEMPrivateKey((unsigned char *)"pkey.pem", &privatekey);
    if (errorcode != OSPC_ERR_NO_ERROR) {
        return errorcode;
    }
    errorcode = OSPPUtilLoadPEMCert((unsigned char *)"localcert.pem", &localcert);
    if (errorcode != OSPC_ERR_NO_ERROR) {
        return errorcode;
    }
    i = 0;
    while (i < OSPC_SEC_MAX_AUTH_CERTS) {
        sprintf(searchstr, "cacert_%d.pem", i);
        errorcode = OSPPUtilLoadPEMCert((unsigned char *)searchstr, &(TheAuthCert[i]));
        if (errorcode == OSPC_ERR_NO_ERROR) {
            authCerts[i] = &(TheAuthCert[i]);
            i++;
            printf("Loaded %d Authorization Certificate \n", i);
        } else {
            if (errorcode == OSPC_ERR_CRYPTO_FILE_OPEN_ERROR) {
                /*
                 * If i!=0 then we have read at least one cacert.
                 * No problem in that case.
                 * Otherwise return an error
                 */
                if (i == 0) {
                    printf("Failed to find the File - %s \n", searchstr);
                    return errorcode;
                } else {
                    /*
                     * Break out of thew loop
                     */
                    printf("There are no more cert files\n");
                    break;
                }
            } else {
                return errorcode;
            }
        }
    }
    NUM_CA_CERTS = i;

    printf("Number of cert files %d\n", NUM_CA_CERTS);

    sprintf(customer_id, "%ld", custid);
    sprintf(device_id, "%ld", devid);

    servpts = (const char **)servicepoints;
    errorcode = OSPPProviderNew(num_serv_points,
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

    return errorcode;
}

int testOSPPProviderDelete()
{
    int errorcode = 0;

    errorcode = OSPPProviderDelete(OSPVProviderHandle, DEF_TIME_LIMIT);

    NUM_CA_CERTS = 0;

    return errorcode;
}

int testOSPPProviderGetAuthorityCertificates()
{
    int errorcode = 0;
    void *certs[100];
    unsigned int i;
    unsigned int certCount = 0;
    char msg[100] = "";
    OSPTASN1ELEMENTINFO *eInfo;

    OSPM_MEMSET(&eInfo, 0, sizeof(eInfo));

    errorcode = OSPPProviderGetNumberOfAuthorityCertificates(OSPVProviderHandle, &certCount);

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

    errorcode = OSPPProviderGetAuthorityCertificates(OSPVProviderHandle, CERT_SZ, &certCount, certs);

    for (i = 0; i < certCount; i++) {
        errorcode = OSPPASN1ElementDecode((unsigned char *)certs[i], &eInfo, 0);

        sprintf(msg, "Authority Certificate #%02d of %02d (%d bytes)", i + 1, certCount, eInfo->ElementLength);
        OSPTNLOGDUMP(eInfo->Element, eInfo->ElementLength, msg);
        OSPM_FREE(certs[i]);
        printf("%s dumped to test_app.log\n", msg);
    }

    OSPPASN1ElementDelete(&eInfo, 0);

    return errorcode;
}

int testOSPPProviderSetAuthorityCertificates()
{
    int errorcode = 0;
    int length = 0;
    unsigned int i;
    char searchstr[20];
    OSPT_CERT *authCerts[OSPC_SEC_MAX_AUTH_CERTS];
    OSPT_CERT TheAuthCert[OSPC_SEC_MAX_AUTH_CERTS];

    i = 0;
    while (i < OSPC_SEC_MAX_AUTH_CERTS) {
        sprintf(searchstr, "cacert_%d.pem", i);
        length = 0;
        errorcode = OSPPUtilLoadPEMCert((unsigned char *)searchstr, &(TheAuthCert[i]));
        authCerts[i] = &(TheAuthCert[i]);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            i++;
            printf("Read %d Authorization Certificate \n", i);
        } else {
            if (errorcode == OSPC_ERR_CRYPTO_FILE_OPEN_ERROR) {
                /*
                 * If i!=0 then we have read at least one cacert.
                 * No problem in that case.
                 * Otherwise return an error
                 */
                if (i == 0) {
                    printf("Failed to find the File - %s \n", searchstr);
                    return errorcode;
                } else {
                    /*
                     * Break out of thew loop
                     */
                    break;
                }
            } else {
                return errorcode;
            }
        }
    }
    NUM_CA_CERTS = i;

    errorcode = OSPPProviderSetAuthorityCertificates(OSPVProviderHandle, NUM_CA_CERTS, (const OSPT_CERT **)authCerts);

    /* Free memory allocated while loading crypto information from PEM-encoded files */
    for (i = 0; i < NUM_CA_CERTS; i++) {
        if (TheAuthCert[i].CertData != NULL) {
            free(TheAuthCert[i].CertData);
        }
    }
    return errorcode;
}

int testOSPPProviderGetHTTPMaxConnections()
{
    int errorcode = 0;
    unsigned maxconns;

    errorcode = OSPPProviderGetHTTPMaxConnections(OSPVProviderHandle, &maxconns);

    printf("max conns = %u\n", maxconns);
    return errorcode;
}

int testOSPPProviderSetHTTPMaxConnections()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPMaxConnections(OSPVProviderHandle, DEF_HTTP_MAXCONN);

    return errorcode;
}

int testOSPPProviderGetHTTPPersistence()
{
    int errorcode = 0;
    unsigned persist;

    errorcode = OSPPProviderGetHTTPPersistence(OSPVProviderHandle, &persist);

    printf("http persistence = %u\n", persist);

    return errorcode;
}

int testOSPPProviderSetHTTPPersistence()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPPersistence(OSPVProviderHandle, DEF_HTTP_PERSIST);

    return errorcode;
}

int testOSPPProviderGetHTTPRetryDelay()
{
    int errorcode = 0;
    unsigned retrydelay;

    errorcode = OSPPProviderGetHTTPRetryDelay(OSPVProviderHandle, &retrydelay);

    printf("http retry delay = %u\n", retrydelay);
    return errorcode;
}

int testOSPPProviderSetHTTPRetryDelay()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPRetryDelay(OSPVProviderHandle, DEF_HTTP_RETRYDELAY);

    return errorcode;
}

int testOSPPProviderGetHTTPTimeout()
{
    int errorcode = 0;
    unsigned timeout;

    errorcode = OSPPProviderGetHTTPTimeout(OSPVProviderHandle, &timeout);

    printf("http timeout = %u\n", timeout);
    return errorcode;
}

int testOSPPProviderSetHTTPTimeout()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPTimeout(OSPVProviderHandle, DEF_HTTP_TIMEOUT);

    return errorcode;
}

int testOSPPProviderGetLocalKeys()
{
    int errorcode = 0;
    OSPTPRIVATEKEY privkey;
    void *localcert;
    char msg[100];
    OSPTASN1ELEMENTINFO *eInfo;

    OSPM_MEMSET(&privkey, 0, sizeof(privkey));

    OSPM_MALLOC(localcert, unsigned char, CERT_SZ);
    if (localcert == OSPC_OSNULL) {
        errorcode = OSPC_ERR_OS_FAILURE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for local certificate");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(privkey.PrivateKeyData, unsigned char, PKEY_SZ);
        if (privkey.PrivateKeyData == OSPC_OSNULL) {
            errorcode = OSPC_ERR_OS_FAILURE;
            OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for private key");
        }
        privkey.PrivateKeyLength = 0;
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPProviderGetLocalKeys(OSPVProviderHandle, &privkey, CERT_SZ, localcert);
    }

    /* Dump the local certificate */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPASN1ElementDecode((unsigned char *)localcert, &eInfo, 0);

        sprintf(msg, "Local Certificate (%d bytes)", eInfo->ElementLength);
        OSPTNLOGDUMP(eInfo->Element, eInfo->ElementLength, msg);
        printf("%s dumped to test_app.log\n", msg);
    }

    /* Dump the private key */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        sprintf(msg, "Private Key (%d bytes)", privkey.PrivateKeyLength);
        OSPTNLOGDUMP(privkey.PrivateKeyData, privkey.PrivateKeyLength, msg);
        printf("%s dumped to test_app.log\n", msg);
    }

    OSPM_FREE(localcert);
    OSPM_FREE(privkey.PrivateKeyData);
    privkey.PrivateKeyLength = 0;

    OSPPASN1ElementDelete(&eInfo, 0);

    return errorcode;
}

int testOSPPProviderSetLocalKeys()
{
    int errorcode = 0;
    OSPT_CERT localcert;
    OSPTPRIVATEKEY privatekey;

    errorcode = OSPPUtilLoadPEMPrivateKey((unsigned char *)"pkey.pem", &privatekey);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPUtilLoadPEMCert((unsigned char *)"localcert.pem", &localcert);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            printf("Read 1 Local Certificate \n");
        } else {
            printf("OSPPUtilLoadPEMCertreturned Error ! \n");
        }
    } else {
        printf("OSPPUtilLoadPEMPrivateKey returned Error ! \n");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPProviderSetLocalKeys(OSPVProviderHandle, &privatekey, localcert.CertData);
    }

    /* Free memory allocated while loading crypto information from PEM-encoded files */
    if (privatekey.PrivateKeyData != NULL) {
        free(privatekey.PrivateKeyData);
    }

    if (localcert.CertData != NULL) {
        free(localcert.CertData);
    }

    return errorcode;
}

int testOSPPProviderGetLocalValidation()
{
    int errorcode = 0;
    unsigned localvalidation;
    const char *local = "LOCAL";
    const char *remote = "REMOTE";

    errorcode = OSPPProviderGetLocalValidation(OSPVProviderHandle, &localvalidation);

    if (localvalidation)
        printf("Token Validation = %s\n", local);
    else
        printf("Token Validation = %s\n", remote);

    return errorcode;
}

int testOSPPProviderSetLocalValidation()
{
    int errorcode = 0;

    return errorcode;
}

int testOSPPProviderGetServicePoints()
{
    int errorcode = 0;
    int i = 0;
    char *svcpts[MAX_SERVICE_POINTS];

    for (i = 0; i < MAX_SERVICE_POINTS; i++)
        OSPM_MALLOC(svcpts[i], char, SVC_PT_SZ);

    errorcode = OSPPProviderGetServicePoints(OSPVProviderHandle, MAX_SERVICE_POINTS, SVC_PT_SZ, svcpts);

    for (i = 0; i < MAX_SERVICE_POINTS; i++) {
        printf("svcpt[%d] = %s\n", i, svcpts[i]);
        OSPM_FREE(svcpts[i]);
    }

    return errorcode;
}

int testOSPPProviderGetSSLLifetime()
{
    int errorcode = 0;
    unsigned ssllifetime;

    errorcode = OSPPProviderGetSSLLifetime(OSPVProviderHandle, &ssllifetime);

    printf("ssl lifetime = %u\n", ssllifetime);

    return errorcode;
}

int testOSPPProviderSetSSLLifetime()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetSSLLifetime(OSPVProviderHandle, DEF_SSL_LIFETIME);

    return errorcode;
}

int testOSPPProviderGetNumberOfAuthorityCertificates()
{
    int errorcode = 0;
    unsigned numofcacerts = 0;

    errorcode = OSPPProviderGetNumberOfAuthorityCertificates(OSPVProviderHandle, &numofcacerts);

    printf("num of ca certs = %u\n", numofcacerts);

    return errorcode;
}

int testOSPPProviderGetNumberOfServicePoints()
{
    int errorcode = 0;
    unsigned numofsvcpts = 0;

    errorcode = OSPPProviderGetNumberOfServicePoints(OSPVProviderHandle, &numofsvcpts);

    if (errorcode == 0)
        printf("num of svc pts = %u\n", numofsvcpts);

    return errorcode;
}

int testOSPPTransactionSetPricingInfo()
{
    int errorcode = 0;
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

    errorcode = OSPPTransactionSetPricingInfo(OSPVTransactionHandle, ospvPricingInfo);

    return errorcode;
}

int testOSPPTransactionSetNetworkId()
{
    int errorcode = 0, i = 0;
    char SrcNetId[128];
    char DestNetId[128];

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

    errorcode = OSPPTransactionSetNetworkIds(OSPVTransactionHandle, SrcNetId, DestNetId);

    return errorcode;
}

int testOSPPTransactionNew()
{
    int errorcode = 0;

    errorcode = OSPPTransactionNew(OSPVProviderHandle, &OSPVTransactionHandle);

    return errorcode;
}

int testOSPPTransactionDelete()
{
    int errorcode = 0;

    if ((OSPVTransactionHandle == OSPC_TRAN_HANDLE_INVALID) &&
        (tranhandle2 == OSPC_TRAN_HANDLE_INVALID)) {
        printf("No Transaction to Delete \n");
        errorcode = OSPC_ERR_TRAN_HANDLE_INVALID;
    }

    if (OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID) {

        errorcode = OSPPTransactionDelete(OSPVTransactionHandle);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            if (!quietmode)
                printf("OSPVTransactionHandle deleted.\n");

            OSPVTransactionHandle = OSPC_TRAN_HANDLE_INVALID;
        }
    }

    if (tranhandle2 != OSPC_TRAN_HANDLE_INVALID) {
        errorcode = OSPPTransactionDelete(tranhandle2);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            printf("tranhandle2 deleted.\n");

            tranhandle2 = OSPC_TRAN_HANDLE_INVALID;
        }
    }

    return errorcode;
}

int testOSPPTransactionAccumulateOneWayDelay()
{
    int errorcode = OSPC_ERR_NO_ERROR, i = 0;

    for (i = 0; i < 10; i++) {
        errorcode = OSPPTransactionAccumulateOneWayDelay(OSPVTransactionHandle,
            accumtable10[i].Number,
            accumtable10[i].Min,
            accumtable10[i].Mean,
            accumtable10[i].Variance);

        printf("OSPPTransactionAccumulateOneWayDelay errorcode = %d\n", errorcode);
    }

    return errorcode;
}

int testOSPPTransactionAccumulateRoundTripDelay()
{
    int errorcode = OSPC_ERR_NO_ERROR, i = 0;

    for (i = 0; i < 2; i++) {
        errorcode = OSPPTransactionAccumulateRoundTripDelay(OSPVTransactionHandle,
            accumtable2[i].Number,
            accumtable2[i].Min,
            accumtable2[i].Mean,
            accumtable2[i].Variance);

        printf("OSPPTransactionAccumulateRoundTripDelay errorcode = %d\n", errorcode);
    }

    return errorcode;
}

int testOSPPTransactionGetDestProtocol()
{
    OSPE_DEST_PROTOCOL dest_prot;
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPTransactionGetDestProtocol(OSPVTransactionHandle, &dest_prot);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        switch (dest_prot) {
        case OSPC_DPROT_UNDEFINED:
            printf("Destination Protocol is Not Configured at Server \n");
            break;
        case OSPC_DPROT_SIP:
            printf("Destination Protocol is SIP \n");
            break;
        case OSPC_DPROT_Q931:
            printf("Destination Protocol is H.323-Q931\n");
            break;
        case OSPC_DPROT_LRQ:
            printf("Destination Protocol is H.323-LRQ \n");
            break;
        case OSPC_DPROT_IAX:
            printf("Destination Protocol is IAX \n");
            break;
        case OSPC_DPROT_T37:
            printf("Destination Protocol is Fax-T.37 \n");
            break;
        case OSPC_DPROT_T38:
            printf("Destination Protocol is Fax-T.38 \n");
            break;
        case OSPC_DPROT_SKYPE:
            printf("Destination Protocol is Skype \n");
            break;
        case OSPC_DPROT_SMPP:
            printf("Destination Protocol is SMPP \n");
            break;
        case OSPC_DPROT_XMPP:
            printf("Destination Protocol is XMPP \n");
            break;
        case OSPC_DPROT_UNKNOWN:
            printf("Destination Protocol is Unknown \n");
            break;
        default:
            printf("Wrong Destination Protocol \n");
            break;
        }
    }

    return errorcode;
}

int testOSPPTransactionIsDestOSPEnabled()
{
    OSPE_DEST_OSPENABLED dest_osp_ver;
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPTransactionIsDestOSPEnabled(OSPVTransactionHandle, &dest_osp_ver);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        switch (dest_osp_ver) {
        case OSPC_DOSP_UNDEFINED:
            printf("Destination OSP Version Not Configured at Server \n");
            break;
        case OSPC_DOSP_TRUE:
            printf("Destination is OSP Enabled\n");
            break;
        case OSPC_DOSP_FALSE:
            printf("Destination is Not OSP Enabled\n");
            break;
        case OSPC_DOSP_UNKNOWN:
            printf("Destination OSP Status is Unknown \n");
            break;
        }
    }

    return errorcode;
}

int testOSPPTransactionGetFirstDestination()
{
    int errorcode = 0;
    char msg[100];
    token = (void *)c_token;
    tokensize = TOKEN_SZ;

    OSPM_MEMSET(ret_cid, 0, CALL_ID_SZ);
    callid = (void *)ret_cid;
    callidsize = CALL_ID_SZ;

    errorcode = OSPPTransactionGetFirstDestination(OSPVTransactionHandle,
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

    if (errorcode == 0 && !quietmode) {
        printf("callid size = %d value = %.*s\n", callidsize, callidsize, (char *)callid);
        OSPM_SPRINTF(msg, "DEST = %s", dest);
        printf("%s\n", dest);
        OSPTNLOGDUMP(token, tokensize, msg);
    }
    return errorcode;
}

int testOSPPTransactionGetNextDestination()
{
    int errorcode = 0;
    char msg[100];

    token = (void *)c_token;
    tokensize = TOKEN_SZ;
    OSPM_MEMSET(ret_cid, 0, CALL_ID_SZ);
    callid = (void *)ret_cid;
    callidsize = CALL_ID_SZ;

    errorcode = OSPPTransactionGetNextDestination(OSPVTransactionHandle,
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

    if (errorcode == 0 && !quietmode) {
        printf("callid size = %d value = %.*s\n", callidsize, callidsize, (char *)callid);
        OSPM_SPRINTF(msg, "DEST = %s", dest);
        printf("%s\n", dest);
        OSPTNLOGDUMP(token, tokensize, msg);
    }
    return errorcode;
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
    int errorcode = 0;

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
            errorcode = OSPC_ERR_OS_FAILURE;
            break;
        }
    }

    if (errorcode)
        testFreeCallIds();

    return errorcode;
}

int testBuildUsageFromScratch(int IsSource, int BuildNew)
{
    int errorcode = 0;
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
    if (errorcode == OSPC_ERR_NO_ERROR && BuildNew) {

        errorcode = OSPPTransactionNew(OSPVProviderHandle, &OSPVTransactionHandle);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            printf("Successful TransactionNew.\n");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR && BuildNew) {
        errorcode = OSPPTransactionSetServiceType(OSPVTransactionHandle, OSPC_SERVICE_VOICE);
        errorcode = OSPPTransactionSetPricingInfo(OSPVTransactionHandle, ospvPricingInfo);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {

        errorcode = OSPPTransactionBuildUsageFromScratch(OSPVTransactionHandle,
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

    if (errorcode == OSPC_ERR_NO_ERROR && BuildNew) {
        printf("Transaction Built \n");
    } else if ((errorcode == OSPC_ERR_NO_ERROR) && (!BuildNew)) {
        printf("Destination Added to transaction \n");
    } else {
        if (OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID) {
            OSPPTransactionDelete(OSPVTransactionHandle);
            OSPVTransactionHandle = OSPC_TRAN_HANDLE_INVALID;
        }
        printf("Errorcode = %d\n", errorcode);
    }

    return errorcode;
}

int testSetDestinationCount()
{
    int errorcode = 0;
    unsigned destinationCount = 0;

    printf("Enter destination count (0 if N/A)");

    scanf("%d", &destinationCount);
    getchar();

    printf("Setting destination count to %d\n", destinationCount);

    errorcode = OSPPTransactionSetDestinationCount(OSPVTransactionHandle, destinationCount);

    return errorcode;
}

int testSetDuration()
{
    printf("Enter duration (in seconds) : ");

    scanf("%d", &duration);
    getchar();

    return 0;
}

int testSetTCCode()
{
    printf("Enter TC Code: ");

    scanf("%d", &TCcode);
    getchar();

    return 0;
}

int testSetStartTime()
{
    printf("Enter Call Start Time (in seconds since 1970 GMT) : ");

    scanf("%ld", &call_start_time);
    getchar();

    return 0;
}

int testSetEndTime()
{
    printf("Enter Call End Time (in seconds since 1970 GMT) : ");

    scanf("%ld", &call_end_time);
    getchar();

    return 0;
}

int testSetAlertTime()
{
    printf("Enter Call Alert Time (in seconds since 1970 GMT) : ");

    scanf("%ld", &call_alert_time);
    getchar();

    return 0;
}

int testSetConnectTime()
{
    printf("Enter Call Connect Time (in seconds since 1970 GMT) : ");

    scanf("%ld", &call_connect_time);
    getchar();

    return 0;
}

int testSetServiceType()
{
	int errorcode;

    errorcode = OSPPTransactionSetServiceType(OSPVTransactionHandle, OSPC_SERVICE_NPQUERY);

    return errorcode;
}

int testOSPPTransactionInitializeAtDevice(int IsSource)
{
    int errorcode = 0;
    unsigned detaillogsize = 0,
        authorised = OSPC_TRAN_NOT_AUTHORISED, timelimit = 0;
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

    if (errorcode == OSPC_ERR_NO_ERROR) {

        errorcode = OSPPTransactionNew(OSPVProviderHandle, &OSPVTransactionHandle);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPTransactionSetServiceType(OSPVTransactionHandle, OSPC_SERVICE_VOICE);
        errorcode = OSPPTransactionSetPricingInfo(OSPVTransactionHandle, ospvPricingInfo);
    }

    tokensize = TOKEN_SZ;

    if (errorcode == OSPC_ERR_NO_ERROR) {

        printf("Successful TransactionNew.\n");

        errorcode = OSPPTransactionInitializeAtDevice(OSPVTransactionHandle,
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

    if (errorcode == OSPC_ERR_NO_ERROR) {
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
        printf("Errorcode TransactionInitialize = %d\n", errorcode);
    }

    return errorcode;
}

int testOSPPTransactionRecordFailure()
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPTransactionRecordFailure(OSPVTransactionHandle, (OSPEFAILREASON)TCcode);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        printf("OSPPTransactionRecordFailure Successful\n");
    } else {
        printf("Errorcode TransactionRecordFailure = %d\n", errorcode);
    }

    return errorcode;
}

/* Should only be called by OGW */
int testOSPPTransactionReinitializeAtDevice()
{
    int errorcode = 0;
    unsigned IsSource = OSPC_ROLE_SOURCE,
        detaillogsize = 0,
        authorised = OSPC_TRAN_NOT_AUTHORISED, timelimit = 0;
    unsigned char token2[TOKEN_SZ];

    OSPM_MEMSET(token2, 0, TOKEN_SZ);

    callid = (void *)c_id;
    token = (void *)c_token;

    tokensize = TOKEN_SZ;

    /* Gateway does reinit */

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPTransactionReinitializeAtDevice(tranhandle2,
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

    if (errorcode == OSPC_ERR_NO_ERROR) {
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
        printf("Errorcode TransactionReInitialize = %d\n", errorcode);
    }

    return errorcode;
}

int testOSPPTransactionRequestSuggestedAuthorisation()
{
    int errorcode = 0;
    unsigned detaillogsize = 0;
    const char *preferredDest[] = { "172.16.4.10", "172.16.4.10", NULL };

    errorcode = testInitializeCallIds();

    numdestinations = NUM_CALL_IDS;
    if (errorcode == OSPC_ERR_NO_ERROR)
        errorcode = OSPPTransactionRequestAuthorisation(OSPVTransactionHandle,
            SourceIP,
            SourceDevIP,    /* Some random IP address that would probably not be in the Server */
            callingnumber,
            CallingNumFormat,
            callednumber,
            CalledNumFormat,
            "919404556#4444",
            NUM_CALL_IDS, callids,
            (const char **)preferredDest,
            &numdestinations,
            &detaillogsize,
            (void *)NULL);

    if (errorcode == 0 && !quietmode) {
        printf("num dest = %u\n", numdestinations);
    }
    testFreeCallIds();

    return errorcode;
}

int testOSPPTransactionRequestAuthorisation()
{
    int errorcode = 0;
    unsigned detaillogsize = 0;

    errorcode = testInitializeCallIds();

    numdestinations = NUM_CALL_IDS;

    if (errorcode == OSPC_ERR_NO_ERROR)
        errorcode = OSPPTransactionRequestAuthorisation(OSPVTransactionHandle,
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

    if (errorcode == 0 && !quietmode) {
        printf("num dest = %u\n", numdestinations);
    }
    testFreeCallIds();

    return errorcode;
}

int testOSPPTransactionIndicateCapabilities()
{
    int errorcode = 0;
    unsigned detaillogsize = 0;

    if (errorcode == OSPC_ERR_NO_ERROR)
        errorcode = OSPPTransactionIndicateCapabilities(OSPVTransactionHandle,
            SourceIP,
            SourceDevIP,
            "SrcNetworkId",
            almostOutOfResources,
            &detaillogsize,
            (void *)NULL);

    return errorcode;
}

int testOSPPProviderSetCapabilitiesURLs()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetCapabilitiesURLs(OSPVProviderHandle, num_capURLs, CapMsgCount, (const char **)capURLs);

    return errorcode;
}

int testOSPPProviderSetServicePoints()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetServicePoints(OSPVProviderHandle, num_serv_points, MsgCount, (const char **)servicepoints);

    return errorcode;
}

int testOSPPTransactionRequestReauthorisation()
{
    int errorcode = 0;
    unsigned authorised = 0, detaillogsize = 0;

    token = (void *)c_token;
    tokensize = TOKEN_SZ;

    OSPM_MEMSET(token, 0, TOKEN_SZ);

    if (errorcode == 0) {

        errorcode = OSPPTransactionRequestReauthorisation(OSPVTransactionHandle, 10,
            &tokensize, token,
            &authorised, &timelimit,
            &detaillogsize,
            (void *)NULL);
    }

    printf("Errorcode = %d. \nAuthorised = %u. \nTimelimit = %u. \nToken = %s.", errorcode, authorised, timelimit, (char *)token);

    return errorcode;
}

int testGetCallingNumber()
{
    int errorcode = 0;
    printf("The Current Calling Number is : %s \n", callingnumber);
    return errorcode;
}

int testGetCalledNumber()
{
    int errorcode = 0;
    printf("The Current Called Number is : %s \n", callednumber);
    return errorcode;
}

int testSetCallingNumber()
{
    int errorcode = 0;
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
    return errorcode;
}

int testSetCallId()
{
    int errorcode = 0;

    OSPM_STRCPY((char *)ret_cid, "");
    printf("Call Id Set to the Empty for Validate Authorization \n");
    return errorcode;
}

int testSetCalledNumber()
{
    int errorcode = 0;
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
    return errorcode;
}

int testOSPPTransactionModifyDeviceIdentifiersAgain()
{
    int errorcode = 0;

    errorcode = OSPPTransactionModifyDeviceIdentifiers(OSPVTransactionHandle,
        "99.99.99.99",
        "100.100.100.100",
        "101.101.101.101",
        "102.102.102.102");

    printf("errorcode = %d\n", errorcode);
    return errorcode;
}

int testOSPPTransactionModifyDeviceIdentifiers()
{
    int errorcode = 0;

    errorcode = OSPPTransactionModifyDeviceIdentifiers(OSPVTransactionHandle,
        ModifiedSourceIP,
        ModifiedSourceDevIP,
        ModifiedDestIP,
        ModifiedDestDevIP);

    printf("errorcode = %d\n", errorcode);
    return errorcode;
}

int testOSPPTransactionGetLookAheadInfoIfPresent()
{
    int errorcode = 0;
    OSPE_DEST_PROTOCOL DestProt = OSPC_DPROT_UNDEFINED;
    OSPE_DEST_OSPENABLED DestOSPStatus = OSPC_DOSP_UNDEFINED;
    char LookAheadDest[DESTINATION_SZ] = { "" };
    OSPTBOOL HasLookAheadInfo = OSPC_FALSE;

    if (errorcode == 0) {
        if (errorcode == 0) {
            errorcode = OSPPTransactionGetLookAheadInfoIfPresent
                (OSPVTransactionHandle, &HasLookAheadInfo,
                LookAheadDest, &DestProt, &DestOSPStatus);
        }

        if (errorcode == 0 && HasLookAheadInfo) {
            printf("Look Ahead Info Present .. \nLookAheadDest = %s\n", LookAheadDest);
            switch (DestProt) {
            case OSPC_DPROT_UNDEFINED:
                printf("Destination Protocol is Not Configured at Server \n");
                break;
            case OSPC_DPROT_SIP:
                printf("Destination Protocol is SIP \n");
                break;
            case OSPC_DPROT_Q931:
                printf("Destination Protocol is H.323-Q931\n");
                break;
            case OSPC_DPROT_LRQ:
                printf("Destination Protocol is H.323-LRQ \n");
                break;
            case OSPC_DPROT_IAX:
                printf("Destination Protocol is IAX \n");
                break;
            case OSPC_DPROT_T37:
                printf("Destination Protocol is Fax-T.37 \n");
                break;
            case OSPC_DPROT_T38:
                printf("Destination Protocol is Fax-T.38 \n");
                break;
            case OSPC_DPROT_SKYPE:
                printf("Destination Protocol is Skype \n");
                break;
            case OSPC_DPROT_SMPP:
                printf("Destination Protocol is SMPP \n");
                break;
            case OSPC_DPROT_XMPP:
                printf("Destination Protocol is XMPP \n");
                break;
            case OSPC_DPROT_UNKNOWN:
                printf("Destination Protocol is Unknown \n");
                break;
            default:
                printf("Wrong Destination Protocol \n");
                break;
            }

            switch (DestOSPStatus) {
            case OSPC_DOSP_UNDEFINED:
                printf("Destination OSP Version Not Configured at Server \n");
                break;
            case OSPC_DOSP_TRUE:
                printf("Destination is OSP Enabled\n");
                break;
            case OSPC_DOSP_FALSE:
                printf("Destination is Not OSP Enabled\n");
                break;
            case OSPC_DOSP_UNKNOWN:
                printf("Destination OSP Status is Unknown \n");
                break;
            }
        } else if (errorcode == 0 && (!HasLookAheadInfo)) {
            printf("Look Ahead Info Not Present\n");
        }
        printf("errorcode = %d\n", errorcode);

    }

    return errorcode;
}


int testOSPPTransactionValidateAuthorisation()
{
    int errorcode = 0;
    unsigned authorised = 0, detaillogsize = 0;
    OSPTTRANS *trans = OSPC_OSNULL;

    errorcode = testInitializeCallIds();

    /* Remove the authrsp so reportusage doesn't get confused. */

    trans = OSPPTransactionGetContext(OSPVTransactionHandle, &errorcode);

    if (errorcode == 0) {
        OSPPTransactionDeleteResponse(trans);

        if (errorcode == 0) {
            errorcode = OSPPTransactionValidateAuthorisation(OSPVTransactionHandle,
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

        if (errorcode == 0 && !quietmode)
            printf("authorised = %u\n", authorised);

        testFreeCallIds();
    }

    return errorcode;
}

int testOSPPTransactionReportUsage()
{
    int errorcode = 0;
    unsigned detaillogsize = 0;

    if (OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID) {

        if (!quietmode)
            printf("Reporting Usage for OSPVTransactionHandle %d\n", (int)OSPVTransactionHandle);

        errorcode = OSPPTransactionReportUsage(OSPVTransactionHandle,
            duration, call_start_time, call_end_time, call_alert_time, call_connect_time, IS_PDD_INFO_AVAILABLE, 1,    /* PDD */
            0,    /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06",    /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize, (void *)NULL);
    }

    if (tranhandle2 != OSPC_TRAN_HANDLE_INVALID) {

        printf("Reporting Usage for tranhandle2\n");

        errorcode = OSPPTransactionReportUsage(tranhandle2,
            duration, call_start_time, call_end_time, call_alert_time, call_connect_time, IS_PDD_INFO_AVAILABLE, 1,    /* PDD */
            0,    /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06",    /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize, (void *)NULL);
    }

    if ((tranhandle2 == OSPC_TRAN_HANDLE_INVALID) &&
        (OSPVTransactionHandle == OSPC_TRAN_HANDLE_INVALID)) {
        printf("Transaction handle invalid\n");
        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    }
    return errorcode;
}

int testOSPPSecSignatureVerify()
{
    int errorcode = 0;
    int tokenfd = -1;
    OSPTSEC *security = OSPC_OSNULL;
    unsigned char *signature = OSPC_OSNULL;
    unsigned char *content = OSPC_OSNULL;
    unsigned int signatureLength = 0;
    unsigned int contentLength = 0;
    OSPTPROVIDER *provider = OSPC_OSNULL;

    tokenfd = _Open("../test_app/testtoken.out", O_RDONLY, 00664);
    if (tokenfd == -1) {
        errorcode = -1;
        OSPM_DBGERRORLOG(errorcode, "Unable to open ../test_app/testtoken.out");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        _Read(tokenfd, (unsigned char *)&signatureLength, 4);

        OSPM_MALLOC(signature, unsigned char, signatureLength);

        _Read(tokenfd, signature, signatureLength);

        _Close(tokenfd);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        provider = OSPPProviderGetContext(OSPVProviderHandle, &errorcode);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPProviderGetSecurity(provider, &security);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSecSignatureVerify(security, &content, &contentLength,
            signature, signatureLength,
            OSPC_SEC_SIGNATURE_AND_CONTENT);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(errorcode, "XMLTEST Test Signature Verified");
        } else {
            OSPM_DBGERRORLOG(errorcode, "Error verifying XMLTEST Test Signature");
        }
    }

    OSPM_FREE(signature);
    OSPM_FREE(content);

    return errorcode;
}

int testOSPPSecCertShowChain()
{
    int errorcode = 0;
    unsigned char *certificate;
    unsigned int certificateLength;
    int caIndex = 0;
    int nextCAIndex = 0;
    char msg[100] = "";
    OSPTPROVIDER *provider = OSPC_OSNULL;
    OSPTSEC *security = OSPC_OSNULL;

    OSPM_MALLOC(certificate, unsigned char, CERT_SZ);

    if (certificate == OSPC_OSNULL) {
        errorcode = -1;
        OSPM_DBGERRORLOG(errorcode, "Error allocating space for certificate");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        provider = OSPPProviderGetContext(OSPVProviderHandle, &errorcode);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPProviderGetSecurity(provider, &security);
    }


    for (caIndex = -1; errorcode == OSPC_ERR_NO_ERROR;
         caIndex = nextCAIndex) {
        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Reset buffer size */
            certificateLength = CERT_SZ;
            nextCAIndex = caIndex;

            /* Look for the next certificate in the chain */
            errorcode = OSPPSecValidCertChain(security, &nextCAIndex, certificate, &certificateLength);
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            if (caIndex == nextCAIndex) {
                /* Last certificate tested was self signed */
                sprintf(msg, "CAIndex=%d is self signed", caIndex);
                OSPM_DBGERRORLOG(errorcode, msg);
                break;
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            sprintf(msg, "CAIndex=%d, Next CAIndex=%d", caIndex, nextCAIndex);
            OSPTNLOGDUMP(certificate, certificateLength, msg);

            OSPM_DBGERRORLOG(errorcode, msg);
        }

    }

    OSPM_FREE(certificate);

    return errorcode;
}

int testOSPToolkitVersion()
{
    printf("OSP Client Toolkit Version: %d.%d.%d\n",
        OSP_CLIENT_TOOLKIT_VERSION_MAJOR,
        OSP_CLIENT_TOOLKIT_VERSION_MINOR,
        OSP_CLIENT_TOOLKIT_VERSION_BUGFIX);

    return 0;
}

int testTermCause()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_Q850, 1, "tc_q850");
    errorcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_H323, 2, "tc_h323");
    errorcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_SIP, 3, "tc_sip");
    errorcode = OSPPTransactionSetTermCause(OSPVTransactionHandle, OSPC_TCAUSE_XMPP, 4, "tc_xmpp");

    return errorcode;
}

int testNumberPortability()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetNumberPortability(OSPVTransactionHandle, "NPRn", "NPCic", 1);

    return errorcode;
}

int testDiversion()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetDiversion(OSPVTransactionHandle, "DiversionSrcInfo", "DiversionDevInfo");

    return errorcode;
}

int testAssertedId()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetAssertedId(OSPVTransactionHandle, "AssertedId");

    return errorcode;
}

int testDestProtocol()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetDestProtocol(OSPVTransactionHandle, OSPPDestProtocolGetPart("h323-LRQ"));

    return errorcode;
}

int testCodec()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetForwardCodec(OSPVTransactionHandle, "g729");
    errorcode = OSPPTransactionSetReverseCodec(OSPVTransactionHandle, "g723");

    return errorcode;
}

int testNetworkId()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetSrcNetworkId(OSPVTransactionHandle, "SrcNetworkId");
    errorcode = OSPPTransactionSetDestNetworkId(OSPVTransactionHandle, "DestNetworkId");

    return errorcode;
}

int testSessionId()
{
    int errorcode = 0;

    OSPT_CALL_ID *callid;
    callid = OSPPCallIdNew(8, (const unsigned char *)"incallid");
    errorcode = OSPPTransactionSetSessionId(OSPVTransactionHandle, OSPC_CLEG_INBOUND, callid);
    OSPPCallIdDelete(&callid);
    callid = OSPPCallIdNew(9, (const unsigned char *)"outcallid");
    errorcode = OSPPTransactionSetSessionId(OSPVTransactionHandle, OSPC_CLEG_OUTBOUND, callid);
    OSPPCallIdDelete(&callid);

    return errorcode;
}

int testCustomInfo()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetCustomInfo(OSPVTransactionHandle, 0, "CustomInfo_first");
    errorcode = OSPPTransactionSetCustomInfo(OSPVTransactionHandle, 31, "CustomInfo_32");

    return errorcode;
}

int testStatsLost()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1, 2);
    errorcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, 3, -1);
    errorcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, -1, 4);
    errorcode = OSPPTransactionSetLost(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, -1, -1);

    return errorcode;
}

int testStatsJitter()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1, 2, 3, 4, 5);
    errorcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, -1, 6, 7, 8, 9);
    errorcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, 10, -1, 11, 12, 13);
    errorcode = OSPPTransactionSetJitter(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, 14, 15, -1, 16, 17);

    return errorcode;
}

int testStatsDelay()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1, 2, 3, 4, 5);
    errorcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, -1, 6, 7, 8, 9);
    errorcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, 10, -1, 11, 12, 13);
    errorcode = OSPPTransactionSetDelay(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, 14, 15, -1, 16, 17);

    return errorcode;
}

int testStatsOctets()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1);
    errorcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, 2);
    errorcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, -1);
    errorcode = OSPPTransactionSetOctets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, 3);

    return errorcode;
}

int testStatsPackets()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1);
    errorcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, 2);
    errorcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, 3);
    errorcode = OSPPTransactionSetPackets(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, -1);

    return errorcode;
}

int testStatsRFactor()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1.1);
    errorcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, 2.2);
    errorcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, -1);
    errorcode = OSPPTransactionSetRFactor(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, 3.3);

    return errorcode;
}

int testStatsMOSCQ()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1.1);
    errorcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, 2.2);
    errorcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, 3.3);
    errorcode = OSPPTransactionSetMOSCQ(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, -1);

    return errorcode;
}

int testStatsMOSLQ()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_DOWNSTREAM, 1.1);
    errorcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SMETRIC_RTP, OSPC_SFLOW_UPSTREAM, 2.2);
    errorcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_DOWNSTREAM, -1);
    errorcode = OSPPTransactionSetMOSLQ(OSPVTransactionHandle, OSPC_SMETRIC_RTCP, OSPC_SFLOW_UPSTREAM, 3.3);

    return errorcode;
}

int testStatsICPIF()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetICPIF(OSPVTransactionHandle, OSPC_SFLOW_DOWNSTREAM, 1);
    errorcode = OSPPTransactionSetICPIF(OSPVTransactionHandle, OSPC_SFLOW_UPSTREAM, 54);

    return errorcode;
}

int testStatsRoundTrip()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetRoundTripDelay(OSPVTransactionHandle, 1, 2, 3, 4, 5);

    return errorcode;
}

int testGetNumberPortability()
{
    int errorcode = 0;
    char rn[OSPC_SIZE_E164NUM];
    char cic[OSPC_SIZE_NORID];
    int npdi;

    errorcode = OSPPTransactionGetNumberPortabilityParameters(OSPVTransactionHandle, sizeof(rn), rn, sizeof(cic), cic, &npdi);
    printf("rn = '%s'\n", rn);
    printf("cic = '%s'\n", cic);
    printf("npdi = %d\n", npdi);

    return errorcode;
}

int testGetServiceProvider()
{
    int errorcode = 0;
    char spid[OSPC_SIZE_NORID];
    char ocn[OSPC_SIZE_NORID];

    errorcode = OSPPTransactionGetServiceProviderId(OSPVTransactionHandle, sizeof(spid), spid);
    errorcode = OSPPTransactionGetOperatingCompanyNumber(OSPVTransactionHandle, sizeof(ocn), ocn);
    printf("spid = '%s'\n", spid);
    printf("ocn = '%s'\n", ocn);

    return errorcode;
}

int testAPI(int apinumber)
{
    OSPTTHREADID MultProviderThrId[OSPC_MAX_PROVIDERS];
    int i, errorcode = 0;
    int trans_to_run, num_providers;
    int build_new_trans = 0;

    switch (apinumber) {

    case 1:
        errorcode = testOSPPProviderNew(&OSPVProviderHandle);
        break;
    case 2:
        errorcode = testOSPPProviderDelete();
        break;
    case 3:
        errorcode = testNotImplemented();
        /*errorcode = testOSPPProviderGetAuthorityCertificates(); */
        break;
    case 4:
        errorcode = testOSPPProviderSetServicePoints();
        break;
    case 5:
        errorcode = testOSPPProviderGetHTTPMaxConnections();
        break;
    case 6:
        errorcode = testOSPPProviderSetHTTPMaxConnections();
        break;
    case 7:
        errorcode = testOSPPProviderGetHTTPPersistence();
        break;
    case 8:
        errorcode = testOSPPProviderSetHTTPPersistence();
        break;
    case 9:
        errorcode = testOSPPProviderGetHTTPRetryDelay();
        break;
    case 10:
        errorcode = testOSPPProviderSetHTTPRetryDelay();
        break;
    case 11:
        errorcode = testOSPPProviderGetHTTPTimeout();
        break;
    case 12:
        errorcode = testOSPPProviderSetHTTPTimeout();
        break;
    case 13:
        errorcode = testNotImplemented();
        break;
    case 14:
        errorcode = testOSPPProviderSetCapabilitiesURLs();
        break;
    case 15:
        errorcode = testOSPPProviderGetLocalValidation();
        break;
    case 16:
        errorcode = testOSPPProviderSetLocalValidation();
        break;
    case 17:
        errorcode = testOSPPProviderGetServicePoints();
        break;
    case 18:
        errorcode = testOSPPSetServicePoints();
        break;
    case 19:
        errorcode = testOSPPProviderGetSSLLifetime();
        break;
    case 20:
        errorcode = testOSPPProviderSetSSLLifetime();
        break;
    case 21:
        errorcode = testOSPPProviderGetNumberOfAuthorityCertificates();
        break;
    case 22:
        errorcode = testOSPPProviderGetNumberOfServicePoints();
        break;
    case 23:
        errorcode = testOSPPTransactionNew();
        break;
    case 24:
        errorcode = testOSPPTransactionDelete();
        break;
    case 25:
        errorcode = testOSPPTransactionAccumulateOneWayDelay();
        break;
    case 26:
        errorcode = testOSPPTransactionAccumulateRoundTripDelay();
        break;
    case 27:
        errorcode = testOSPPTransactionGetFirstDestination();
        break;
    case 28:
        errorcode = testOSPPTransactionGetNextDestination();
        break;
    case 29:
        errorcode = testOSPPTransactionRequestAuthorisation();
        break;
    case 30:
        errorcode = testOSPPTransactionRequestSuggestedAuthorisation();
        break;
    case 31:
        errorcode = testOSPPTransactionValidateAuthorisation();
        break;
    case 32:
        errorcode = testOSPPTransactionReportUsage();
        break;
    case 99:
        OSPM_SLEEP(2);
        break;
    case 33:
        errorcode = testOSPPTransactionInitializeAtDevice(OSPC_ROLE_SOURCE);
        break;
    case 34:
        errorcode = testOSPPTransactionInitializeAtDevice(OSPC_ROLE_DESTINATION);
        break;
    case 35:
        errorcode = testOSPPTransactionSetNetworkId();
        break;
    case 36:
        errorcode = testOSPPTransactionRecordFailure();
        break;
    case 37:
        errorcode = testOSPPTransactionIndicateCapabilities();
        break;
    case 38:
        errorcode = testOSPPTransactionRequestReauthorisation();
        break;
    case 39:
        errorcode = testOSPPTransactionGetDestProtocol();
        break;
    case 40:
        errorcode = testOSPPTransactionIsDestOSPEnabled();
        break;
    case 41:
        errorcode = testTestCalls();
        break;
    case 42:
        errorcode = testOSPToolkitVersion();
        break;
    case 43:
        printf("Build a new transaction ? Press 1 for Yes, 0 for No : ");
        scanf("%d", &build_new_trans);
        getchar();
        errorcode = testBuildUsageFromScratch(OSPC_ROLE_SOURCE, build_new_trans);
        break;
    case 44:
        printf("Build a new transaction ? Press 1 for Yes, 0 for No : ");
        scanf("%d", &build_new_trans);
        getchar();
        errorcode =
            testBuildUsageFromScratch(OSPC_ROLE_DESTINATION, build_new_trans);
        break;
    case 45:
        errorcode = testOSPPTransactionGetLookAheadInfoIfPresent();
        break;
    case 46:
        errorcode = testOSPPTransactionModifyDeviceIdentifiers();
        break;
    case 47:
        errorcode = testOSPPTransactionModifyDeviceIdentifiersAgain();
        break;
    case 48:
        errorcode = testSetDestinationCount();
        break;
    case 50:
        errorcode = testSetCallingNumber();
        break;
    case 51:
        errorcode = testSetCalledNumber();
        break;
    case 52:
        errorcode = testGetCallingNumber();
        break;
    case 53:
        errorcode = testGetCalledNumber();
        break;
    case 54:
        errorcode = testSetCallId();
    case 55:
        errorcode = testOSPPTransactionSetPricingInfo();
        break;
    case 56:
        errorcode = testSetDuration();
        break;
    case 57:
        errorcode = testSetTCCode();
        break;
    case 58:
        errorcode = testSetStartTime();
        break;
    case 59:
        errorcode = testSetEndTime();
        break;
    case 60:
        errorcode = testSetAlertTime();
        break;
    case 61:
        errorcode = testSetConnectTime();
        break;
    case 62:
        errorcode = testSetServiceType();
        break;
    case 100:
        printf("Enter the number of Providers to be created .. ");
        scanf("%d", &num_providers);
        getchar();
        if (num_providers > OSPC_MAX_PROVIDERS) {
            printf("Cannot run the test. The entered value is greater than the maximum providers allowed\n");
            errorcode = OSPC_ERR_PROV_MAX_PROVIDERS;
        } else {
            printf("Enter the number of Transactions to be run .. ");
            scanf("%d", &trans_to_run);
            getchar();
            if (2 * trans_to_run > OSPC_MAX_TRANS) {
                errorcode = OSPC_ERR_TRAN_NO_TRANS_SPACE;
                printf("Cannot run the test. The entered value is greater than the maximum transactions allowed\n");
            }

            if ((errorcode == 0) && (trans_to_run > MAX_QUEUE_SIZE)) {
                printf("Warning !!! The toolkit may not be able to process - %d calls because the maximum queue size is - %d \n",
                     trans_to_run, MAX_QUEUE_SIZE);
            }

            /*
             * Launch the threads
             */
            if (errorcode == 0) {
                for (i = 0; i < num_providers; i++) {
                    OSPM_CREATE_THREAD(MultProviderThrId[i],
                        NULL,
                        testNonBlockingPerformanceTest,
                        (void *)&trans_to_run, errorcode);

                    printf("Created Thread [%d] with thread id: [%lu]\n", i, (unsigned long int)MultProviderThrId[i]);
                }
                for (i = 0; i < num_providers; i++) {
                    OSPM_THR_JOIN(MultProviderThrId[i], NULL);
                }
            }
        }
        break;
    case 101:
        errorcode = testNonBlockingPerformanceTestForCapabilities();
        break;
    case 200:
        errorcode = testTermCause();
        break;
    case 210:
        errorcode = testNumberPortability();
        break;
    case 211:
        errorcode = testDiversion();
        break;
    case 212:
        errorcode = testAssertedId();
        break;
    case 220:
        errorcode = testDestProtocol();
        break;
    case 221:
        errorcode = testCodec();
        break;
    case 222:
        errorcode = testNetworkId();
        break;
    case 223:
        errorcode = testSessionId();
        break;
    case 224:
        errorcode = testCustomInfo();
        break;
    case 230:
        errorcode = testStatsLost();
        break;
    case 231:
        errorcode = testStatsJitter();
        break;
    case 232:
        errorcode = testStatsDelay();
        break;
    case 233:
        errorcode = testStatsOctets();
        break;
    case 234:
        errorcode = testStatsPackets();
        break;
    case 235:
        errorcode = testStatsRFactor();
        break;
    case 236:
        errorcode = testStatsMOSCQ();
        break;
    case 237:
        errorcode = testStatsMOSLQ();
        break;
    case 238:
        errorcode = testStatsICPIF();
        break;
    case 239:
        errorcode = testStatsRoundTrip();
        break;
    case 300:
        errorcode = testGetNumberPortability();
        break;
    case 301:
        errorcode = testGetServiceProvider();
        break;
    default:
        errorcode = -1;
    }

    return errorcode;
}

int testTestCalls()
{
    register int i = 0, j = 0;
    int errorcode = OSPC_ERR_NO_ERROR;
    time_t start_time, end_time;
#define NUM_API_OPS 7
    int apinumber_idx[NUM_API_OPS] = { 23, 29, 27, 28, 31, 32, 24 };

    time(&start_time);
    quietmode = 1;
    while (i < num_test_calls && errorcode == OSPC_ERR_NO_ERROR) {
        for (j = 0; j < NUM_API_OPS; j++) {
            if (apinumber_idx[j] == 28 && numdestinations == 1)
                continue;
            errorcode = testAPI(apinumber_idx[j]);
            if (errorcode != OSPC_ERR_NO_ERROR) {
                fprintf(stderr, "Test Failed. Iteration = %d Function = %d ErrorCode = %d\n",
                    i, apinumber_idx[j], errorcode);
                break;
            }
        }
        i++;
    }
    time(&end_time);
    printf("Elapsed Time: %ld seconds\n", end_time - start_time);
    quietmode = 0;
    return errorcode;
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
        printf("25) AccumulateOneWayDelay             26) AccumulateRoundTripDelay\n");
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
        printf("Configuration Parameters \n");
        printf("---------------------------------------------------------------------\n");
        printf("50) Set Calling Number                51) Set Called Number\n");
        printf("52) Get Calling Number                53) Get Called Number\n");
        printf("54) Set CallId to Empty for Token Validation\n");
        printf("55) Set Pricing Info\n");
        printf("56) Set Duration                      57) Set TC Code\n");
        printf("58) Set Start Time                    59) Set End Time\n");
        printf("60) Set Alert Time                    61) Set Connect Time\n");
        printf("62) Set Service Type\n");
        printf("---------------------------------------------------------------------\n");
        printf("Performance tests\n");
        printf("---------------------------------------------------------------------\n");
        printf("100) Run Multiple calls\n");
        printf("101) Run Multiple Capabilities Indications\n");
        printf("---------------------------------------------------------------------\n");
        printf("Other tests\n");
        printf("---------------------------------------------------------------------\n");
        printf("200) Set Termination Cause\n");
        printf("210) Set NP Parameters                211) Set Diversion\n");
        printf("212) Set Asserted ID\n");
        printf("220) Set Destination Protocol         221) Set Codec\n");
        printf("222) Set Network ID                   223) Set Session ID\n");
        printf("224) Set Custom Info\n");
        printf("230) Set Lost                         231) Set Jitter\n");
        printf("232) Set Delay                        233) Set Octets\n");
        printf("234) Set Packets                      235) Set R-Factor\n");
        printf("236) Set MOS-CQ                       237) Set MOS-LQ\n");
        printf("238) Set ICPIF                        239) Set Round Trip Delay\n");
        printf("300) Get NP parameters                301) Get SPID & OCN\n");
        printf("---------------------------------------------------------------------\n");
        printf("Enter function number or 'q' to quit => ");
    }
    fgets(inbuf, 256, stdin);
    if (inbuf[0] == 'q' || inbuf[0] == 'Q')
        funcnum = -1;
    else
        funcnum = atoi(inbuf);

    return funcnum;
}

int GetConfiguration()
{
    FILE *fp = NULL;
    int errorcode = 0;
    char inbuf[512];
    char tmp_addr[20];
    int spindex = 0, Capspindex = 0;

    if ((fp = fopen(CONFIG_FILENAME, "r")) == (FILE *)NULL) {
        fprintf(stderr, "Cannot open config file %s for reading. %s\n",
                CONFIG_FILENAME, strerror(errno));
        errorcode = errno;
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
                    errorcode = 1;
                    break;
                }
            } else if (OSPM_STRNCMP(inbuf, "CapURL=", OSPM_STRLEN("CapURL=")) == 0) {
                capURLs[num_capURLs++] = _Strdup(&inbuf[OSPM_STRLEN("CapURL=")]);

                if (num_capURLs == MAX_SERVICE_POINTS) {
                    fprintf(stderr,
                            "Too many capabilities URLs configured. Max = %d\n",
                            MAX_SERVICE_POINTS);
                    errorcode = 1;
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
        errorcode = 1;
    }
    return errorcode;
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
        printf("initialization failed. errorcode = %d\n", retcode);
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
    int errorcode = 0;
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
        printf("Malloc Failed !! Exiting ! \n");
        exit(0);
    }

    for (i = 0; i < num_test_calls; i++) {
        Tokens[i] = (char *)malloc(sizeof(char) * TOKEN_SIZE);
        if (Tokens[i] == NULL) {
            printf("Malloc Failed !! Exiting ! \n");
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
        printf("Malloc Failed !! Exiting ! \n");
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

    errorcode = testOSPPProviderNew(&provHandle);
    printf("Thread Id: %lu, ProviderNew returned: %d, ProviderId: %d\n", (unsigned long int)thr_id, errorcode, provHandle);

    /*
     * Phase I Creating new transactions / 2 (O + T) transactions for every call
     */
    printf("\n\n");
    printf("Thread Id: %lu, Phase I (Source and Destination): OSPPTransactionNew.\n", (unsigned long int)thr_id);
    time(&start_time);
    for (i = 0; i < num_test_calls; i++) {
        if ((errorcode = OSPPTransactionNew(provHandle, &OTransactionHandles[i])) != OSPC_ERR_NO_ERROR ||
            (errorcode = OSPPTransactionNew(provHandle, &TTransactionHandles[i])) != OSPC_ERR_NO_ERROR)
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
        errorcode = OSPPTransactionRequestAuthorisation_nb(nbMonitor, 0,    /* DON'T BLOCK */
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

        if (errorcode != OSPC_ERR_NO_ERROR) {
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
        errorcode = OSPPTransactionGetFirstDestination(OTransactionHandles[i],
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

        if (errorcode != OSPC_ERR_NO_ERROR) {
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
        errorcode = OSPPTransactionValidateAuthorisation_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &TErrorCodes[i],
            TTransactionHandles
            [i], SourceIP,
            Localdest, NULL,
            NULL,
            Localcallingnumber,
            CallingNumFormat,
            Localcallednumber,
            CalledNumFormat,
            CallIds[i]->ospmCallIdLen,
            CallIds[i]->ospmCallIdVal,
            TokenSizes[i],
            Tokens[i],
            &authorised[i],
            &Localtimelimit,
            &detaillogsize,
            (void *)NULL,
            tokenalgo);

        if (errorcode != OSPC_ERR_NO_ERROR) {
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
        errorcode = OSPPTransactionReportUsage_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &OErrorCodes[i],
            OTransactionHandles[i],
            duration, time(NULL) - 10, time(NULL) + 20, time(NULL) - 10, time(NULL) - 8, IS_PDD_INFO_AVAILABLE, 1,    /* PDD */
            3,    /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06",    /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize,
            (void *)NULL);
        if (errorcode != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionReportUsage_nb failed, aborting the test.\n");
            exit(0);
        }

        errorcode = OSPPTransactionReportUsage_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &TErrorCodes[i],
            TTransactionHandles[i],
            duration, time(NULL) - 10, time(NULL) + 20, time(NULL) - 10, time(NULL) - 8, IS_PDD_INFO_AVAILABLE, 1,    /* PDD */
            3,    /* Release Source */
            "E4596A7B-2C27-11D9-816A-EA39F2B2CD06",    /*Conf id */
            1,
            2,
            100,
            10,
            &detaillogsize,
            (void *)NULL);
        if (errorcode != OSPC_ERR_NO_ERROR) {
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
        if ((errorcode = OSPPTransactionDelete(OTransactionHandles[i])) != OSPC_ERR_NO_ERROR ||
            (errorcode = OSPPTransactionDelete(TTransactionHandles[i])) != OSPC_ERR_NO_ERROR)
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

    errorcode = OSPPProviderDelete(provHandle, DEF_TIME_LIMIT);

    if (errorcode != 0) {
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
    int errorcode = 0;
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
    scanf("%d", &TEST_NUM);

    if ((TEST_NUM) > OSPC_MAX_TRANS)
        printf("Warning !! The toolkit may not be able to process - %d Calls because the maximum transactions that can be created is - %d \n",
            TEST_NUM, OSPC_MAX_TRANS);

    if (TEST_NUM > MAX_QUEUE_SIZE)
        printf("Warning !! The toolkit may not be able to process - %d Calls because the maximum queue size is - %d \n",
            TEST_NUM, MAX_QUEUE_SIZE);

    /*
     * Allocate Memory
     */
    OErrorCodes = (int *)malloc((sizeof(int) * TEST_NUM));
    OTransactionHandles =
        (OSPTTRANHANDLE *)malloc((sizeof(OSPTTRANHANDLE) * TEST_NUM));

    if ((OErrorCodes == NULL) || (OTransactionHandles == NULL)) {
        printf("Malloc Failed !! Exiting ! \n");
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
        if ((errorcode = OSPPTransactionNew(OSPVProviderHandle, &OTransactionHandles[i])) != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionNew failed, aborting the test.\n");
            return errorcode;
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
        errorcode = OSPPTransactionIndicateCapabilities_nb(nbMonitor, 0,    /* DON'T BLOCK */
            &OErrorCodes[i],
            OTransactionHandles
            [i], SourceIP,
            SourceDevIP,
            "SrcNetworkId",
            i % 2,
            &detaillogsize,
            (void *)NULL);

        if (errorcode != OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionIndicateCapabilities_nb failed, aborting the test.\n");
            return errorcode;
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
        if ((errorcode =
             OSPPTransactionDelete(OTransactionHandles[i])) !=
            OSPC_ERR_NO_ERROR) {
            printf("OSPPTransactionDelete failed, aborting the test.\n");
            return errorcode;
        }
    }
    time(&end_time);
    printf("Time elapsed <%ld>\n", end_time - start_time);

    if (OErrorCodes != NULL)
        free(OErrorCodes);

    if (OTransactionHandles != NULL)
        free(OTransactionHandles);

    return errorcode;
}
