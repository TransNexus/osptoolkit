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
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "osp.h"
#include "ospproviderapi.h"
#include "ospprovider.h"
#include "osppkcs1.h"
#include "osptransapi.h"
#include "ospfail.h"
#include "osptnlog.h"
#include "ospasn1.h"
#include "ospsecurity.h"
#include "osppkcs8.h"
#include "ospx509.h"
#include "osptrans.h"
#include "nonblocking.h"
#include "openssl/bio.h"
#include "openssl/pem.h"
#include "openssl/evp.h"


#ifdef  WIN32
#define _Open   _open
#define _Read   _read
#define _Close  _close
#define _Lseek  _lseek
#define _Strdup _strdup
#else
#define _Open   open
#define _Read   read
#define _Close  close
#define _Lseek  lseek
#define _Strdup strdup
#define  O_BINARY  (0)
#endif


#define LOCAL_VALIDATION      1
#define DEF_SSL_LIFETIME      300
#define DEF_HTTP_MAXCONN      50
#define DEF_HTTP_PERSIST      60000
#define DEF_HTTP_RETRYDELAY   0
#define DEF_HTTP_RETRYLIMIT   1
#define DEF_HTTP_TIMEOUT      (60 * 1000)
#define DEF_TIME_LIMIT        4
#define DEF_CUST_ID           1000L
#define DEF_DEVICE_ID         1000L

#define CALL_ID_SZ       256
#define DESTINATION_SZ   64
#define TOKEN_SZ         2096*8

// WARNING: BEFORE CHANGING THE VALUE OF NUM_CALL_IDS, BE SURE
//          TO MODIFY THE FUNCTION testInitializeCallIds SO THAT
//          THE NUMBER OF ELEMENTS IN THE ARRAYS MATCH THIS VALUE.
#define NUM_CALL_IDS     8

#define TIMESTAMP_SZ     32

#define SVC_PT_SZ        262
#define CERT_SZ          1024
#define PKEY_SZ          1024

#define FAILURE_REASON  OSPC_FAIL_TEMPORARY_FAILURE

/*----------------------------------------------*
 *            request info globals              *
 *----------------------------------------------*/
char     validafter[TIMESTAMP_SZ],
         validuntil[TIMESTAMP_SZ];
unsigned timelimit                  = 0;
unsigned callidsize                 = CALL_ID_SZ;
void     *callid                    = NULL;
unsigned char ret_cid[CALL_ID_SZ];
unsigned char c_id[CALL_ID_SZ+1]    = { "1234567890123456"};
char     callednumber[CALL_ID_SZ];
char     callingnumber[CALL_ID_SZ];
/*char     *callednumber              = { "14048724799" };
char     *callingnumber             = { "14045551212" };*/
char     dest[DESTINATION_SZ]       = { "" };
char     destdev[DESTINATION_SZ]    = { "" };
unsigned tokensize                  = TOKEN_SZ;
char     c_token[TOKEN_SZ]          = { "" };
void     *token                     = NULL;

char     *New_ServicePoint          = { "http://osptestserver.transnexus.com:80/osp" };

static  OSPTCALLID *callids[NUM_CALL_IDS];



NBMONITOR *nbMonitor    = NULL;
#define WORK_THREAD_NUM 40    /* make sure that this number does not exceed DEF_HTTP_MAXCONN */
#define TEST_NUM        1000
#define TOKEN_SIZE      2000
#define TEST_ERROR 1
char      Tokens[TEST_NUM][TOKEN_SIZE];


/*----------------------------------------------*
 *            service point globals             *
 *----------------------------------------------*/
#define CONFIG_FILENAME               "test.cfg"
#define MAX_SERVICE_POINTS        10

char *auditurl = NULL;
char *servicepoints[MAX_SERVICE_POINTS];

int   num_serv_points = 0;

long custid = DEF_CUST_ID;
long devid  = DEF_DEVICE_ID;

/*----------------------------------------------*
 *            certificate globals               *
 *----------------------------------------------*/
static  size_t  NUM_CA_CERTS=0;
static  size_t  NUM_LOCAL_CERTS=0;
static  size_t  NUM_PKEYS=0;
static  unsigned char  **certs=NULL;
static  OSPTCERT				*authCerts[10];
static  int           *certssizes=NULL;
static  unsigned char  **localcerts=NULL;
static  int             *localcertssizes=NULL;
static OSPTCERT         localcert = { NULL, 0 };
static  unsigned char **pkeys=NULL;
static  int            *pkeyssizes=NULL;
static OSPTPRIVATEKEY privatekey = { NULL, 0 };

int quietmode = 0;
unsigned numdestinations = NUM_CALL_IDS;
int num_test_calls = 500;

OSPTCERT TheAuthCert;
unsigned char Reqbuf[4096],LocalBuf[4096],AuthBuf[4096];

/************ Local prototypes */
static off_t  FileSizeGet(int fd);
static int    FileArrayRead(unsigned char ***array,size_t *arrayNum, 
                            int **filesizes,const char *baseName,
                            const char *ext,size_t max);
static void    FileArrayDelete(unsigned char ***array,int **filesizes, 
                              size_t num);
int testTestCalls(void);
int testNonBlockingPerformanceTest();

/*----------------------------------------------*
 *              accumulate table                *
 *----------------------------------------------*/
typedef struct _ACCUM
{
    unsigned    Number;
    unsigned    Min;
    unsigned    Mean;
    float       Variance;
} ACCUM;

ACCUM   accumtable10[10] = {
    {1,3335,3335, 0.0f},
    {1, 822, 822, 0.0f},
    {1, 268, 268, 0.0f},
    {1, 524, 524, 0.0f},
    {1,  97,  97, 0.0f},
    {1, 874, 874, 0.0f},
    {1, 171, 171, 0.0f},
    {1, 595, 595, 0.0f},
    {1, 1196,1196, 0.0f},
    {1,  958, 958, 0.0f}
};

ACCUM accumtable2[2] = {
    {10, 6, 718, 485206.00f},
    {20, 1,  141, 39733.00f}
};

/*----------------------------------------------*
 *               global handles                 *
 *----------------------------------------------*/
OSPTPROVHANDLE OSPVProviderHandle       = 0;
OSPTTRANHANDLE OSPVTransactionHandle    = OSPC_TRAN_HANDLE_INVALID;
OSPTTRANHANDLE tranhandle2              = OSPC_TRAN_HANDLE_INVALID;

int
testNotImplemented()
{
  printf("not implemented\n");
  return(0);
}

/*----------------------------------------------*
 *               Loads the Certificate          *
 *----------------------------------------------*/
int testOSPPloadPemCert(unsigned char *FileName, unsigned char *buffer, int *len)
{
    int length = 0;
    unsigned char *temp;
    BIO *bioIn = NULL;
    X509 *cert=NULL;
    int retVal = OSPC_ERR_NO_ERROR;

    temp = buffer;
    bioIn = BIO_new_file((const char*)FileName,"r");
    if (bioIn == NULL)
    {
        printf("Failed to find the File - %s \n",FileName);
        retVal = TEST_ERROR;
    }
    else
    {
        cert = PEM_read_bio_X509(bioIn,NULL,NULL,NULL);
        if (cert == NULL)
        {
            printf("Failed to parse the Certificate from the File - %s \n",FileName);
            retVal = TEST_ERROR;
        }
        else
        {
            length = i2d_X509(cert,&temp);
            if (cert == 0)
            {
                printf("Failed to parse the Certificate from the File - %s ,Length = 0\n",FileName);
                retVal = TEST_ERROR;
            }
            else
            {
               *len = length;
            }
        }
    }

    if (bioIn != NULL)
    {
        BIO_free(bioIn);
    }

    if (cert != NULL)
    {
        X509_free(cert);
    }
    return retVal;    
}





/*----------------------------------------------*
 *               Loads the Private Key          *
 *----------------------------------------------*/
int testOSPPloadPemPrivateKey(unsigned char *FileName, unsigned char *buffer, int *len)
{
    int length = 0;
    unsigned char *temp;
    BIO *bioIn = NULL;
    RSA *pKey;
    int retVal = OSPC_ERR_NO_ERROR;

    temp = buffer;

    bioIn = BIO_new_file((const char*)FileName,"r");
    if (bioIn == NULL)
    {
        printf("Failed to find the File - %s \n",FileName);
        retVal = TEST_ERROR;
    }
    else
    {
        pKey = PEM_read_bio_RSAPrivateKey(bioIn,NULL,NULL,NULL);
        if (pKey == NULL)
        {
            printf("Failed to parse the Private Key from the File - pkey.pem \n");
            retVal = TEST_ERROR;
        }
        else
        {
            length = i2d_RSAPrivateKey(pKey,&temp);
            if (length == 0)
            {
                printf("Failed to parse the Private Key from the File - pkey.pem, length = 0\n");
                retVal = TEST_ERROR;
            }
            else
            {
                *len = length;
                printf ("Loaded 1 Private Key \n");
            }   
        }
    }
    if (bioIn != NULL)
    {
        BIO_free(bioIn);
    }

    if (pKey != NULL)
    {
       RSA_free(pKey);
    }
    return retVal;    
}

int 
testOSPPSetServicePoints()
{
    int errorcode = 0;
    const char **servpts;

    servpts = (const char **)&New_ServicePoint;

    errorcode = OSPPProviderSetServicePoints(OSPVProviderHandle,1,servpts);
    return errorcode;
}


int
testOSPPProviderNew()
{
int errorcode = 0;
const char **servpts;
int i;
char msg[100];

char customer_id[64];
char device_id[64];

    int length = 0;
    unsigned char *temp;

    errorcode = testOSPPloadPemPrivateKey((unsigned char *)"pkey.pem", (unsigned char *)Reqbuf,&length);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        privatekey.PrivateKeyData = Reqbuf;
        privatekey.PrivateKeyLength = length;
    }
    else
    {
         return errorcode;
    }

   length = 0;
    errorcode = testOSPPloadPemCert((unsigned char *)"localcert.pem", (unsigned char *)LocalBuf,&length);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        localcert.CertData = LocalBuf;
        localcert.CertDataLength = length;
        printf ("Loaded 1 Local Certificate \n");
    }
    else
    {
         return errorcode;
    }


    length = 0;
    errorcode = testOSPPloadPemCert((unsigned char *)"cacert.pem", (unsigned char *)AuthBuf,&length);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        TheAuthCert.CertData = AuthBuf;
        TheAuthCert.CertDataLength = length;
        authCerts[0] = &TheAuthCert;
        printf ("Loaded 1 Auhorization Certificate \n");
    }
    else
    {
         return errorcode;
    }


    NUM_CA_CERTS = 1;


    sprintf(customer_id, "%ld", custid);
    sprintf(device_id, "%ld", devid);

    servpts = (const char **)servicepoints;
      errorcode = OSPPProviderNew(
        num_serv_points,
        servpts,
        auditurl,
        (const OSPTPRIVATEKEY *)&privatekey,
        /* Local certificate */
        &localcert,
        /* CA certificates */
        NUM_CA_CERTS,
        (const OSPTCERT **)authCerts,
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
        (OSPTPROVHANDLE *)&OSPVProviderHandle);

    return errorcode;
}

int
testOSPPProviderDelete()
{
    int errorcode = 0;

    errorcode = OSPPProviderDelete(
        OSPVProviderHandle,
        DEF_TIME_LIMIT);

/*    FileArrayDelete(&localcerts, &localcertssizes, NUM_LOCAL_CERTS);
    FileArrayDelete(&pkeys, &pkeyssizes, NUM_PKEYS);
    FileArrayDelete(&certs, &certssizes, NUM_CA_CERTS);
*/
    NUM_CA_CERTS=0;
    NUM_LOCAL_CERTS=0;
    NUM_PKEYS=0;
    
    return errorcode;
}

int
testOSPPProviderGetAuthorityCertificates()
{
    int errorcode = 0;
    void *certs[100];
    unsigned int i;
    unsigned int certCount = 0;
    char msg[100]= "";
    OSPTASN1ELEMENTINFO *eInfo;

    OSPM_MEMSET(&eInfo, 0, sizeof(eInfo));

    errorcode = OSPPProviderGetNumberOfAuthorityCertificates(
        OSPVProviderHandle,
        &certCount);

    if (certCount != NUM_CA_CERTS) 
    {
      printf("Count Mismatch: Authority Certificates Loaded: %d, Authority "
             "Certificates Stored: %d\n", NUM_CA_CERTS, certCount);
    }
    else
    {
      printf("Authority Certificates Loaded/Stored: %d\n", certCount);
    }

    for (i = 0; i < certCount; i++) 
    {
        OSPM_MALLOC(certs[i], void, CERT_SZ);
        if (!certs[i])
            return -1;
    }

    errorcode = OSPPProviderGetAuthorityCertificates(
        OSPVProviderHandle,
        CERT_SZ,
        &certCount,
        certs);



    for (i = 0; i < certCount; i++) 
    {
        errorcode = OSPPASN1ElementDecode((unsigned char *)certs[i], 
                                            &eInfo, 0);

        sprintf(msg, "Authority Certificate #%02d of %02d (%d bytes)", 
                i+1, certCount, eInfo->ElementLength);
        OSPTNLOGDUMP(eInfo->Element, eInfo->ElementLength, msg);
        OSPM_FREE(certs[i]);
        printf( "%s dumped to test_app.log\n", msg);
    }

    OSPPASN1ElementDelete(&eInfo,0);

    return errorcode;
}

int
testOSPPProviderSetAuthorityCertificates()
{
    int errorcode = 0;
    int length = 0;
    unsigned char *temp;

    errorcode = testOSPPloadPemCert((unsigned char *)"cacert.pem", (unsigned char *)AuthBuf,&length);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        TheAuthCert.CertData = AuthBuf;
        TheAuthCert.CertDataLength = length;
        authCerts[0] = &TheAuthCert;
        printf ("Read 1 Auhorization Certificate \n");
    }
    else
    {
         return errorcode;
    }

    NUM_CA_CERTS = 1;


    errorcode = OSPPProviderSetAuthorityCertificates(
        OSPVProviderHandle,
        NUM_CA_CERTS,
        (const OSPTCERT **)authCerts);

    return errorcode;
}

int
testOSPPProviderGetHTTPMaxConnections()
{
    int errorcode = 0;
    unsigned maxconns;

    errorcode = OSPPProviderGetHTTPMaxConnections(
        OSPVProviderHandle,
        &maxconns);

    printf("max conns = %u\n", maxconns);
    return errorcode;
}

int
testOSPPProviderSetHTTPMaxConnections()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPMaxConnections(
        OSPVProviderHandle,
        DEF_HTTP_MAXCONN);

    return errorcode;
}

int
testOSPPProviderGetHTTPPersistence()
{
    int errorcode = 0;
    unsigned persist;

    errorcode = OSPPProviderGetHTTPPersistence(
        OSPVProviderHandle,
        &persist);

    printf("http persistence = %u\n", persist);

    return errorcode;
}

int
testOSPPProviderSetHTTPPersistence()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPPersistence(
        OSPVProviderHandle,
        DEF_HTTP_PERSIST);

    return errorcode;
}

int
testOSPPProviderGetHTTPRetryDelay()
{
    int errorcode = 0;
    unsigned retrydelay;

    errorcode = OSPPProviderGetHTTPRetryDelay(
        OSPVProviderHandle,
        &retrydelay);

    printf("http retry delay = %u\n", retrydelay);
    return errorcode;
}

int
testOSPPProviderSetHTTPRetryDelay()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPRetryDelay(
        OSPVProviderHandle,
        DEF_HTTP_RETRYDELAY);

    return errorcode;
}

int
testOSPPProviderGetHTTPTimeout()
{
    int errorcode = 0;

    unsigned timeout;

    errorcode = OSPPProviderGetHTTPTimeout(
        OSPVProviderHandle,
        &timeout);

    printf("http timeout = %u\n", timeout);
    return errorcode;
}

int
testOSPPProviderSetHTTPTimeout()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetHTTPTimeout(
        OSPVProviderHandle,
        DEF_HTTP_TIMEOUT);

    return errorcode;
}

int
testOSPPProviderGetLocalKeys()
{
    int errorcode = 0;
    OSPTPRIVATEKEY privkey;
    void *localcert;
    char msg[100];
    OSPTASN1ELEMENTINFO *eInfo;

    OSPM_MEMSET(&privkey, 0, sizeof(privkey));

    OSPM_MALLOC(localcert, unsigned char, CERT_SZ);
    if (localcert == OSPC_OSNULL) 
    {
      errorcode = OSPC_ERR_OS_FAILURE;
      OSPM_DBGERRORLOG(errorcode, 
                      "Unable to allocate space for local certificate");
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
      OSPM_MALLOC(privkey.PrivateKeyData, unsigned char, PKEY_SZ);
      if (privkey.PrivateKeyData == OSPC_OSNULL)
      {
        errorcode = OSPC_ERR_OS_FAILURE;
        OSPM_DBGERRORLOG(errorcode, 
                        "Unable to allocate space for private key");
      }
      privkey.PrivateKeyLength = 0;
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
      errorcode = OSPPProviderGetLocalKeys( OSPVProviderHandle, &privkey,
                                            CERT_SZ, localcert);
    }

    /* Dump the local certificate */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
      errorcode = OSPPASN1ElementDecode((unsigned char *)localcert, &eInfo, 0);

      sprintf(msg, "Local Certificate (%d bytes)", eInfo->ElementLength);
      OSPTNLOGDUMP(eInfo->Element, eInfo->ElementLength, msg);
      printf( "%s dumped to test_app.log\n", msg);
    }  

    /* Dump the private key */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
      sprintf(msg, "Private Key (%d bytes)", privkey.PrivateKeyLength);
      OSPTNLOGDUMP(privkey.PrivateKeyData, privkey.PrivateKeyLength, msg);
      printf( "%s dumped to test_app.log\n", msg);
    }  

    OSPM_FREE(localcert);
    OSPM_FREE(privkey.PrivateKeyData);
    privkey.PrivateKeyLength = 0;

    OSPPASN1ElementDelete(&eInfo,0);

    return errorcode;
}

int
testOSPPProviderSetLocalKeys()
{
    int errorcode = 0;
    int length;

    errorcode = testOSPPloadPemPrivateKey((unsigned char *)"pkey.pem", (unsigned char *) Reqbuf,&length);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        privatekey.PrivateKeyData = Reqbuf;
        privatekey.PrivateKeyLength = length;
        length = 0;

        errorcode = testOSPPloadPemCert((unsigned char *)"localcert.pem", (unsigned char *)LocalBuf,&length);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            localcert.CertData = LocalBuf;
            localcert.CertDataLength = length;
            printf ("Read 1 Local Certificate \n");
            localcerts = &(localcert.CertData);
        }
        else
        {
            printf("testOSPPloadPemCertreturned Error ! \n");
        } 
    }
    else
    {
        printf("testOSPPloadPemPrivateKey returned Error ! \n");
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPProviderSetLocalKeys(
        OSPVProviderHandle,
        &privatekey,
        localcerts[0]);
    }

    return errorcode;
}

int
testOSPPProviderGetLocalValidation()
{
    int errorcode = 0;
    unsigned    localvalidation;
    char        *local = "LOCAL";
    char        *remote = "REMOTE";

    errorcode = OSPPProviderGetLocalValidation(
        OSPVProviderHandle,
        &localvalidation);

    if(localvalidation)
        printf("Token Validation = %s\n", local);
    else
        printf("Token Validation = %s\n", remote);

    return errorcode;
}

int
testOSPPProviderSetLocalValidation()
{
    int errorcode = 0;

    return errorcode;
}

int
testOSPPProviderGetServicePoints()
{
    int errorcode = 0;
    int i = 0;
    char *svcpts[MAX_SERVICE_POINTS];

    for (i = 0; i < MAX_SERVICE_POINTS; i++)
        OSPM_MALLOC(svcpts[i], char, SVC_PT_SZ);

    errorcode = OSPPProviderGetServicePoints(
        OSPVProviderHandle,
        MAX_SERVICE_POINTS,
        SVC_PT_SZ,
        svcpts);

    for (i = 0; i < MAX_SERVICE_POINTS; i++) 
    {
        printf("svcpt[%d] = %s\n", i, svcpts[i]);
        OSPM_FREE(svcpts[i]);
    }

    return errorcode;
}

int
testOSPPProviderGetSSLLifetime()
{
    int errorcode = 0;
    unsigned ssllifetime;

    errorcode = OSPPProviderGetSSLLifetime(
        OSPVProviderHandle,
        &ssllifetime);

    printf("ssl lifetime = %u\n", ssllifetime);
    return errorcode;
}

int
testOSPPProviderSetSSLLifetime()
{
    int errorcode = 0;

    errorcode = OSPPProviderSetSSLLifetime(
        OSPVProviderHandle,
        DEF_SSL_LIFETIME);

    return errorcode;
}

int
testOSPPProviderGetNumberOfAuthorityCertificates()
{
    int errorcode = 0;
    unsigned numofcacerts = 0;

    errorcode = OSPPProviderGetNumberOfAuthorityCertificates(
        OSPVProviderHandle,
        &numofcacerts);

    printf("num of ca certs = %u\n", numofcacerts);
    return errorcode;
}

int
testOSPPProviderGetNumberOfServicePoints()
{
    int errorcode = 0;
    unsigned numofsvcpts = 0;

    errorcode = OSPPProviderGetNumberOfServicePoints(
        OSPVProviderHandle,
        &numofsvcpts);

    if (errorcode == 0)
        printf("num of svc pts = %u\n", numofsvcpts);
    return errorcode;
}

int 
testOSPPTransactionSetNetworkId()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetNetworkId(
        OSPVTransactionHandle,
        "termid01");

    return errorcode;

}


int 
testOSPPTransactionSetNewNetworkId()
{
    int errorcode = 0;

    errorcode = OSPPTransactionSetNetworkId(
        OSPVTransactionHandle,
        "termidNew");

    return errorcode;

}

int
testOSPPTransactionNew()
{
    int errorcode = 0;

    errorcode = OSPPTransactionNew(
        OSPVProviderHandle,
        &OSPVTransactionHandle);

    return errorcode;
}

int
testOSPPTransactionDelete()
{
    int errorcode = 0;

    if(OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID){

        errorcode = OSPPTransactionDelete(
                      OSPVTransactionHandle);
        if(errorcode == OSPC_ERR_NO_ERROR){
            if (!quietmode)
               printf("\nOSPVTransactionHandle deleted.\n");

            OSPVTransactionHandle = OSPC_TRAN_HANDLE_INVALID;
        }
    }

    if(tranhandle2 != OSPC_TRAN_HANDLE_INVALID){
        errorcode = OSPPTransactionDelete(
                      tranhandle2);

        if(errorcode == OSPC_ERR_NO_ERROR){
            printf("\ntranhandle2 deleted.\n");

            tranhandle2 = OSPC_TRAN_HANDLE_INVALID;
        }
        
    }

    return errorcode;
}

int
testOSPPTransactionAccumulateOneWayDelay()
{
    int         errorcode   = OSPC_ERR_NO_ERROR,
                i           = 0;

    for(i = 0; i < 10; i++){

        errorcode = OSPPTransactionAccumulateOneWayDelay(OSPVTransactionHandle, 
                     accumtable10[i].Number, accumtable10[i].Min, 
                     accumtable10[i].Mean, accumtable10[i].Variance);
    
        printf("\nOSPPTransactionAccumulateOneWayDelay errorcode = %d", errorcode);
    }
    return errorcode;
}

int
testOSPPTransactionAccumulateRoundTripDelay()
{
    int   errorcode  = OSPC_ERR_NO_ERROR,
          i          = 0;

    for(i = 0; i < 2; i++){
        errorcode = OSPPTransactionAccumulateRoundTripDelay(OSPVTransactionHandle, 
                     accumtable2[i].Number, accumtable2[i].Min, 
                     accumtable2[i].Mean, accumtable2[i].Variance);
    
        printf("\nOSPPTransactionAccumulateRoundTripDelay errorcode = %d", errorcode);
    }
    return errorcode;
}

int
testOSPPTransactionGetDestProtocol()
{
    OSPE_DEST_PROT dest_prot;
    int   errorcode  = OSPC_ERR_NO_ERROR;
    
    errorcode = OSPPTransactionGetDestProtocol(OSPVTransactionHandle,&dest_prot);    
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
       switch (dest_prot)
       {
           case 0 :
                      printf("Destination Protocol is Not Configured at Server \n");
                      break;
           case 2 :
                      printf("Destination Protocol is SIP \n");
                      break;
           case 4 :
                      printf("Destination Protocol is h323-LRQ \n");
                      break;
           case 8 :
                      printf("Destination Protocol is h323-Q931\n");
                      break;
           case 16:
                      printf("Destination Protocol is Unknown \n");
                      break;
       }
    }
    return errorcode;

}

int 
testOSPPTransactionIsDestOSPEnabled()
{
    OSPE_DEST_OSP_ENABLED dest_osp_ver;
    int   errorcode  = OSPC_ERR_NO_ERROR;

    errorcode = OSPPTransactionIsDestOSPEnabled(OSPVTransactionHandle,&dest_osp_ver);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
       switch (dest_osp_ver)
       {
           case 0 :
                      printf("Destination OSP Version Not Configured at Server \n");
                      break;
           case 128 :
                      printf("Destination is OSP Enabled\n");
                      break;
           case 256 :
                      printf("Destination is Not OSP Enabled\n");
                      break;
           case 512 :
                      printf("Destination OSP Status is Unknown \n");
                      break;
       }
    }
    return errorcode;

}


int
testOSPPTransactionGetFirstDestination()
{

    unsigned             i,
                        errorcode = 0;
    char         msg[100]; 
    token  = (void *)c_token;
    tokensize = TOKEN_SZ;

    OSPM_MEMSET(ret_cid, 0, CALL_ID_SZ);
    callid = (void *)ret_cid;

    errorcode = OSPPTransactionGetFirstDestination(
        OSPVTransactionHandle,
        /* TIMESTAMP_SZ,*/
        0,
        validafter,
        validuntil,
        &timelimit,
        &callidsize,
        callid,
        strlen(callednumber) + 1,
        callednumber,
        DESTINATION_SZ,
        dest,
        DESTINATION_SZ,
        destdev,
        &tokensize,
        token);

    if (errorcode == 0 && !quietmode)
    {
        printf("callid size = %d value = ", callidsize);
        for (i = 0; i < callidsize; i++)
                printf("<%x>",(*((unsigned char *)callid+i)));
        printf("\n");
        OSPM_SPRINTF(msg, "DEST = %s", dest);
        printf("%s\n", dest);
        OSPTNLOGDUMP(token, tokensize, msg);
    }
    return errorcode;
}

int
testOSPPTransactionGetNextDestination()
{
    unsigned    i           = 0;
    int         errorcode   = 0;
        char msg[100];

    token  = (void *)c_token;
    tokensize = TOKEN_SZ;
    OSPM_MEMSET(ret_cid, 0, 20);
    callid = (void *)ret_cid;

    errorcode = OSPPTransactionGetNextDestination(
        OSPVTransactionHandle,
        FAILURE_REASON,
        TIMESTAMP_SZ,
        validafter,
        validuntil,
        &timelimit,
        &callidsize,
        callid,
        strlen(callednumber) + 1,
        callednumber,
        DESTINATION_SZ,
        dest,
        DESTINATION_SZ,
        destdev,
        &tokensize,
        token);

    if (errorcode == 0 && !quietmode)
    {
        printf("callid size = %d value = ", callidsize);
        for (i = 0; i < callidsize; i++)
                printf("<%x>",(*((unsigned char *)callid+i)));
        printf("\n");
        OSPM_SPRINTF(msg, "DEST = %s", dest);
        printf("%s\n", dest);
        OSPTNLOGDUMP(token, tokensize, msg);
    }
    return errorcode;
}

void
testFreeCallIds()
{

    register int i = 0;
    for (i = 0; i < NUM_CALL_IDS; i++) 
        OSPPCallIdDelete(&(callids[i]));
    return;
}

int
testInitializeCallIds()
{
    register int i         = 0;
    int          errorcode = 0;

    unsigned char *val[NUM_CALL_IDS] ={  
                                         (unsigned char *)"1",
                                         (unsigned char *)"2",
                                         (unsigned char *)"3",
                                         (unsigned char *)"4",
                                         (unsigned char *)"5",
                                         (unsigned char *)"6",
                                         (unsigned char *)"7",
                                         (unsigned char *)"8"
                                      };
    unsigned    lens[NUM_CALL_IDS] =  {   
                                          1,
                                          1,
                                          1,
                                          1,
                                          1,
                                          1,
                                          1,
                                          1
                                      };



    for (i = 0; i < NUM_CALL_IDS; i++) 
    {
        callids[i] = OSPPCallIdNew(lens[i], (const unsigned char *)val[i]);
        if (callids[i] == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_OS_FAILURE;
            break;
        }
    }

    if (errorcode)
        testFreeCallIds();

    return errorcode;
}

int
testOSPPTransactionInitializeAtDevice(int IsSource)
{
    int             errorcode       = 0;
    unsigned        detaillogsize   = 0,
                    authorised      = OSPC_TRAN_NOT_AUTHORISED,
                    timelimit       = 0;
    unsigned char   token2[TOKEN_SZ];

                    callid = (void *)c_id;
                    token  = (void *)c_token;

    OSPM_MEMSET(token2, 0, TOKEN_SZ);

    /* User must have created a new transaction before this.*/

    /* Gateway part starts here */

    if(errorcode == OSPC_ERR_NO_ERROR){

        errorcode = OSPPTransactionNew(
            OSPVProviderHandle,
            &tranhandle2);
    }

    tokensize = TOKEN_SZ;

    if (errorcode == OSPC_ERR_NO_ERROR){

        printf("\nSuccessful TransactionNew.");

        errorcode = OSPPTransactionInitializeAtDevice(
        tranhandle2,
        IsSource, "[172.16.1.101]",
        "[172.16.16.16]", NULL,
        NULL, callingnumber,
        callednumber, 
        callidsize,
        callid,
        tokensize,
        token,
        &authorised,
        &timelimit,
        &detaillogsize,
        NULL);
    }


    if(errorcode == OSPC_ERR_NO_ERROR){
        switch(authorised){
        case OSPC_TRAN_AUTHORISED:
            printf("\nInitialization authorised\n");
            break;
        case OSPC_TRAN_REAUTHORISED:
            printf("\nInitialization reauthorised\n");
            break;
        case OSPC_TRAN_NOT_AUTHORISED:
            printf("\nInitialization not authorised\n");
            break;
        default:
            printf("\nUnknown authorisation code\n");
            break;
        }
    }
    else
    {
        printf("\nErrorcode TransactionInitialize = %d\n", errorcode);
    }

    return errorcode;
}

int
testOSPPTransactionRecordFailure()
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPTransactionRecordFailure(
                        OSPVTransactionHandle,
                        FAILURE_REASON);


    if(errorcode == OSPC_ERR_NO_ERROR){
        printf("\nOSPPTransactionRecordFailure Successful\n");
    }
    else
    {
        printf("\nErrorcode TransactionRecordFailure = %d\n", errorcode);
    }

    return errorcode;
}                                   

/* Should only be called by OGW */
int
testOSPPTransactionReinitializeAtDevice()
{
    int         errorcode       = 0;
    unsigned    IsSource        = OSPC_SOURCE,
                detaillogsize   = 0,
                authorised      = OSPC_TRAN_NOT_AUTHORISED,
                timelimit       = 0;
    unsigned char   token2[TOKEN_SZ];

    OSPM_MEMSET(token2, 0, TOKEN_SZ);

    callid = (void *)c_id;
    token  = (void *)c_token;

    tokensize = TOKEN_SZ;


    /* Gateway does reinit */

    if(errorcode == OSPC_ERR_NO_ERROR){
        errorcode = OSPPTransactionReinitializeAtDevice(
                                                tranhandle2,
                                                FAILURE_REASON,
                                                IsSource,
                                                "[1.1.1.1]",
                                                "[3.3.3.3]",
                                                OSPC_OSNULL,
                                                OSPC_OSNULL,
                                                callingnumber,
                                                callednumber,
                                                callidsize,
                                                callid,
                                                tokensize,
                                                token,
                                                &authorised,
                                                &timelimit,
                                                &detaillogsize,
                                                NULL);
    }

    if(errorcode == OSPC_ERR_NO_ERROR){
        switch(authorised){
        case OSPC_TRAN_AUTHORISED:
            printf("\nInitialization authorised\n");
            break;
        case OSPC_TRAN_REAUTHORISED:
            printf("\nInitialization reauthorised\n");
            break;
        case OSPC_TRAN_NOT_AUTHORISED:
            printf("\nInitialization not authorised\n");
            break;
        default:
            printf("\nUnknown authorisation code\n");
            break;
        }
    }
    else
    {
        printf("\nErrorcode TransactionReInitialize = %d\n", errorcode);
    }

    return errorcode;
}

int
testOSPPTransactionRequestAuthorisation()
{
    int      errorcode       = 0;
    unsigned detaillogsize   = 0;

    errorcode = testInitializeCallIds(); 

    numdestinations = NUM_CALL_IDS;

    if (errorcode == OSPC_ERR_NO_ERROR) 
        errorcode = OSPPTransactionRequestAuthorisation(
        OSPVTransactionHandle,
        "[1.1.1.1]",
        "[2.2.2.2]",
        callingnumber,
        callednumber,
        "bgates",
        NUM_CALL_IDS,
        callids,
        (const char **)NULL,
        &numdestinations,
        &detaillogsize,
        (void *)NULL);

    if (errorcode == 0 && !quietmode)
    {
        printf("num dest = %u\n", numdestinations);
    }
    testFreeCallIds();

    return errorcode;
}

int
testOSPPTransactionRequestReauthorisation()
{
    int      errorcode     = 0;
    unsigned authorised    = 0,
             detaillogsize = 0;
    
    token     = (void *)c_token;
    tokensize = TOKEN_SZ;
    
    OSPM_MEMSET(token, 0, TOKEN_SZ);

    if(errorcode == 0){

        errorcode = OSPPTransactionRequestReauthorisation(
            OSPVTransactionHandle,
            10,
            &tokensize,
            token,
            &authorised,
            &timelimit,
            &detaillogsize,
            (void *)NULL);
    }

    printf("\nErrorcode = %d. \nAuthorised = %u. \nTimelimit = %u. \nToken = %s.",
            errorcode, authorised, timelimit, token);

    return errorcode;
}

int
testGetCallingNumber()
{
     int errorcode = 0;
     printf("The Current Calling Number is : %s \n",callingnumber);
     return errorcode;
}

int
testGetCalledNumber()
{
     int errorcode = 0;
     printf("The Current Called Number is : %s \n",callednumber);
     return errorcode;
}

int
testSetCallingNumber()
{
    int errorcode = 0;
    printf("Enter the new value for Calling number : ");
    gets(callingnumber);
    if (!strcmp(callingnumber,""))
    {
        printf("WARNING : You have set an Empty Calling Number !!\n");
    }
    printf("Calling Number Set to the new value\n");
    return errorcode;
}

int
testSetCalledNumber()
{
    int errorcode = 0;
    printf("Enter the new value for Calling number : ");
    gets(callednumber);
    if (!strcmp(callednumber,""))
    {
        printf("WARNING : You have set an Empty Called Number !!\n");
    }
    printf("Called Number Set to the new value\n");
    return errorcode;
}

int
testOSPPTransactionValidateAuthorisation()
{
    int         errorcode       = 0;
    unsigned    authorised      = 0,
                detaillogsize   = 0;
    OSPTTRANS  *trans           = OSPC_OSNULL;

    errorcode = testInitializeCallIds();

    /* Remove the authrsp so reportusage doesn't
        get confused.
    */


    trans = OSPPTransactionGetContext(OSPVTransactionHandle, &errorcode);
    
    OSPPTransactionDeleteResponse(trans);


    if(errorcode == 0)
    {
        errorcode = OSPPTransactionValidateAuthorisation(
            OSPVTransactionHandle,
            "[1.1.1.1]",
            dest,
            "[2.2.2.2]",
            destdev,
            callingnumber,
            callednumber,
            callidsize,
            ret_cid,
            tokensize,
            c_token,
            &authorised,
            &timelimit,
            &detaillogsize,
            (void *)NULL);
    }

    if (errorcode == 0 &&!quietmode)
        printf("authorised = %u\n", authorised);

    testFreeCallIds();

    return errorcode;
}

int
testOSPPTransactionReportUsage()
{
    int      errorcode     = 0;
    unsigned detaillogsize = 0;

    if(OSPVTransactionHandle != OSPC_TRAN_HANDLE_INVALID){
        
        if (!quietmode)
            printf("\nReporting Usage for OSPVTransactionHandle %d\n",(int)OSPVTransactionHandle);

        errorcode = OSPPTransactionReportUsage(
            OSPVTransactionHandle,
            30,
						time(NULL)-10,
            1,
            2,
            100,
            10,
            &detaillogsize,
            (void *)NULL);
    }

    if(tranhandle2 != OSPC_TRAN_HANDLE_INVALID){

        printf("\nReporting Usage for tranhandle2\n");

        errorcode = OSPPTransactionReportUsage(
            tranhandle2,
            30,
						time(NULL)-10,
            1,
            2,
            100,
            10,
            &detaillogsize,
            (void *)NULL);
    }

    return errorcode;
}

int
testOSPPSecSignatureVerify()
{
    int      errorcode     = 0;
        int tokenfd = -1;
        OSPTSEC *security = OSPC_OSNULL;
        unsigned char *signature = OSPC_OSNULL;
        unsigned char *content = OSPC_OSNULL;
        unsigned int        signatureLength = 0;
        unsigned int        contentLength = 0;
        OSPTPROVIDER *provider = OSPC_OSNULL;

        tokenfd = _Open("../test_app/testtoken.out", O_RDONLY,  00664);
        if (tokenfd == -1)
        {
                errorcode = -1;
                OSPM_DBGERRORLOG(errorcode, "Unable to open ../test_app/testtoken.out");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
                _Read(tokenfd, 
                        (unsigned char*) &signatureLength, 4);

                OSPM_MALLOC(signature, unsigned char, signatureLength);

                _Read(tokenfd, signature, signatureLength); 
        
                _Close(tokenfd);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
                provider = OSPPProviderGetContext(OSPVProviderHandle,  &errorcode);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPProviderGetSecurity( provider, &security);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
                errorcode = OSPPSecSignatureVerify( security, &content, &contentLength, 
                                                                                signature, signatureLength, 
                                                                                OSPC_SEC_SIGNATURE_AND_CONTENT);
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                        OSPM_DBGERRORLOG(errorcode, "XMLTEST Test Signature Verified");
                }
                else
                {
                        OSPM_DBGERRORLOG(errorcode, 
                                                                "Error verifying XMLTEST Test Signature");
                }
        }
                                                        
        OSPM_FREE(signature);
        OSPM_FREE(content);

    return errorcode;
}

int
testOSPPSecCertShowChain()
{
    int      errorcode     = 0;
        unsigned char *certificate;
        unsigned int certificateLength;
        int caIndex=0; 
        int nextCAIndex = 0;
        char msg[100] = "";
        OSPTPROVIDER *provider = OSPC_OSNULL;
        OSPTSEC *security = OSPC_OSNULL;

        
        OSPM_MALLOC(certificate, unsigned char, CERT_SZ);

        if (certificate == OSPC_OSNULL)
        {
                errorcode = (-1);
                OSPM_DBGERRORLOG(errorcode, 
                                                        "Error allocating space for certificate");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
                provider = OSPPProviderGetContext(OSPVProviderHandle,  &errorcode);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPProviderGetSecurity( provider, &security);
        }


        for (caIndex = (-1); errorcode == OSPC_ERR_NO_ERROR; caIndex = nextCAIndex)
        {
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                        /* Reset buffer size */
                        certificateLength = CERT_SZ;
                        nextCAIndex = caIndex;

                        /* Look for the next certificate in the chain */
                        errorcode = OSPPSecValidCertChain(security, &nextCAIndex, 
                                                                                certificate, &certificateLength);

                }

                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                        if (caIndex == nextCAIndex)
                        {
                                /* Last certificate tested was self signed */
                                sprintf(msg, "CAIndex=%d is self signed", caIndex);
                                OSPM_DBGERRORLOG(errorcode, msg);
                                break;
                        }
                        
                }

                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                        sprintf(msg, "CAIndex=%d, Next CAIndex=%d", caIndex, nextCAIndex);
                        OSPTNLOGDUMP(certificate, certificateLength, msg);

                        OSPM_DBGERRORLOG(errorcode, msg);
                }

        }

        OSPM_FREE(certificate);
        
        return errorcode;
}

int
testOSPPSecCertObjectCreateDelete()
{
    int      errorcode     = 0;
        OSPTASN1OBJECT *certificate = OSPC_OSNULL;
        unsigned long customerId = 0;
        unsigned long deviceId = 0;

        /* Read the local certificate into the array */
    FileArrayDelete(&localcerts, &localcertssizes, NUM_LOCAL_CERTS);
    (void)FileArrayRead(&localcerts,&NUM_LOCAL_CERTS,&localcertssizes, 
                        "localcert","dat",1);
    if (!quietmode)
        printf("Loaded %u local certificates\n",NUM_LOCAL_CERTS);

        errorcode = OSPPX509CertCreate(localcerts[0], &certificate);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
                errorcode = OSPPX509CertGetCustDeviceId( certificate,
                                                                                                        &customerId, &deviceId);
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                        printf("Certificate Customer ID: %ld\n", customerId);
                        printf("Certificate Device ID: %ld\n", deviceId);
                }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
                errorcode = OSPPX509CertDelete(&certificate);
        }

        return(errorcode);
}

int
testOSPPSecKeyObjectCreateDelete()
{
    int      errorcode     = 0;
        OSPTASN1OBJECT *key = OSPC_OSNULL;
        OSPTPRIVATEKEY privateKey;

        /* Read the local key into the array */
    FileArrayDelete(&pkeys, &pkeyssizes, NUM_PKEYS);
    (void)FileArrayRead(&pkeys,&NUM_PKEYS,&pkeyssizes, "pkey","dat",1);
    if (!quietmode)
        printf("Loaded %u private keys\n",NUM_PKEYS);

    privateKey.PrivateKeyData=pkeys[0];
    privateKey.PrivateKeyLength = pkeyssizes[0];
    

        errorcode = OSPPPKCS8KeyInfoCreate(&key, &privateKey);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
                OSPPPKCS8KeyInfoDelete(&key);
        }

        return(errorcode);
}


int
testAPI(int apinumber)
{
    int errorcode = 0;
    extern char *OSPVBuildVersion; 

    switch(apinumber) 
    {

        case 1:
        errorcode = testOSPPProviderNew();
        break;
        case 2:
        errorcode = testOSPPProviderDelete();
        break;
        case 3:
        errorcode = testOSPPProviderGetAuthorityCertificates();
        break;
        case 4:
        errorcode = testOSPPProviderSetAuthorityCertificates();
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
        errorcode = testOSPPProviderSetLocalKeys();
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
        errorcode = testNotImplemented();
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
        errorcode = testOSPPTransactionInitializeAtDevice(OSPC_SOURCE);
        break;
        case 34:
        errorcode = testOSPPTransactionInitializeAtDevice(OSPC_DESTINATION);
        break;
        case 35:
        errorcode = testOSPPTransactionSetNetworkId();
        break;
        case 36:
        errorcode = testOSPPTransactionRecordFailure();
        break;
        case 37:
        errorcode = testOSPPTransactionSetNewNetworkId();
        break;
        case 38:
        errorcode = testOSPPTransactionGetDestProtocol();
        break;
        case 39:
        errorcode = testOSPPTransactionIsDestOSPEnabled();
        break;
        case 40:
        errorcode = testNotImplemented();
        break;
        case 41:
        errorcode = testTestCalls();
        break;
#ifndef _WIN32
        case 42:
        printf("OSP Version: %s\n", OSPVBuildVersion);
        break;
#endif
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
        case 100:
        errorcode = testNonBlockingPerformanceTest();
        break;
        default:
        errorcode = -1;
    }

    return errorcode;
}
int
testTestCalls()
{
   register int i = 0, j = 0;
   int errorcode = OSPC_ERR_NO_ERROR;
   time_t start_time, end_time;
#define NUM_API_OPS 7
   int apinumber_idx[NUM_API_OPS] = { 23, 29, 27, 28, 31, 32, 24 };
 
   time(&start_time);
   quietmode = 1;
   while (i < num_test_calls && errorcode == OSPC_ERR_NO_ERROR)
   {
       for (j = 0; j < NUM_API_OPS; j++)
       {
           if (apinumber_idx[j] == 28 && numdestinations == 1)
                continue;
           errorcode = testAPI(apinumber_idx[j]);
           if (errorcode != OSPC_ERR_NO_ERROR)
           {
              fprintf(stderr, "Test Failed. Iteration = %d Function = %d ErrorCode = %d\n",
                                  i, apinumber_idx[j], errorcode);
              break;
           }
       }
       i++; 
   }
   time(&end_time);
   printf("Elapsed Time: %d seconds\n", end_time - start_time);
   quietmode = 0;
   return errorcode;
}

int
testMenu()
{
    char inbuf[256];
    int funcnum;

    if (!quietmode)
    {
      printf("\nProvider API functions\n");
      printf("---------------------------------------------------------------------\n");
      printf(" 1) New                                2) Delete\n");
      printf(" 3) GetAuthorityCertificates           4) SetAuthorityCertificates\n");
      printf(" 5) GetHTTPMaxConnections              6) SetHTTPMaxConnections\n");
      printf(" 7) GetHTTPPersistence                 8) SetHTTPPersistence\n");
      printf(" 9) GetHTTPRetryDelay                 10) SetHTTPRetryDelay\n");
      printf("11) GetHTTPTimeout                    12) SetHTTPTimeout\n");
      printf("13) For future Enhancements           14) SetLocalKeys\n");
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
      printf("29) RequestAuthorisation              30) For future Enhancements\n");
      printf("31) ValidateAuthorisation             32) ReportUsage\n");
      printf("33) TransactionInitializeAtDevice(OGW)34) TransactionInitialize(TGW)\n");
      printf("35) SetNetworkId                      36) TransactionRecordFailure\n");
      printf("---------------------------------------------------------------------\n");
      printf("Miscellaneous Tests\n");
      printf("---------------------------------------------------------------------\n");
      printf("37) Reset NetworkId                   38) GetDestinationProtocol\n");
      printf("39) IsDestOSPEnabled	              40) For future Enhancements\n");
      printf("41) %-6d Test Calls                 42) Show Version\n", num_test_calls);
      printf("99) Sleep for 2 seconds\n");
      printf("---------------------------------------------------------------------\n");
      printf("Configuration Parameters \n");
      printf("---------------------------------------------------------------------\n");
      printf("50) Set Calling Number                51) Set Called Number\n");
      printf("52) Get Calling Number                53) Get Called Number\n");
      printf("---------------------------------------------------------------------\n");
      printf("Performance tests\n");
      printf("---------------------------------------------------------------------\n");
      printf("100) 1000 Full calls\n");
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

int
GetConfiguration()
{
    FILE *fp = NULL;
    int errorcode = 0;
    char inbuf[512],*tmp;

    if ((fp = fopen(CONFIG_FILENAME, "r")) == (FILE *)NULL)
    {
        fprintf(stderr, "Cannot open config file %s for reading. %s\n",
            CONFIG_FILENAME, strerror(errno));
        errorcode = errno;
    }
    else
    {
        while (fgets(inbuf, 512, fp))
        {
            if (inbuf[0] == '#' || inbuf[0] == '\n')
                continue;

            inbuf[strlen(inbuf)-1] = '\0';

            if (strncmp(inbuf, "AUDIT=", 6) == 0)
                auditurl = _Strdup(&inbuf[6]);
            else
            {
                if (strncmp(inbuf, "SP=", 3) == 0)
                {
                    servicepoints[num_serv_points++] = _Strdup(&inbuf[3]);

                    if (num_serv_points == MAX_SERVICE_POINTS)
                    {
                        fprintf(stderr, "Too many service points configured. Max = %d\n",
                            MAX_SERVICE_POINTS);
                        errorcode = 1;
                        break;
                    }
                }
                else
                {
                    if (strncmp(inbuf, "CUSTID=", 7) == 0)
                    {
                        custid = atol((const char *)&inbuf[7]);
                    } 
                    else
                    {
                        if (strncmp(inbuf, "DEVID=", 6) == 0)
                        {
                            devid = atol((const char *)&inbuf[6]);
                        } 
                        else
                        {
                            if (strncmp(inbuf, "CALLED=", 7) == 0)
                            {
                                  tmp = _Strdup(&inbuf[7]);
                                  strcpy(callednumber,tmp);
                                  free(tmp);
                                  tmp = NULL;
                            }
                            else
                            {
                                if (strncmp(inbuf, "CALLING=", 8) == 0)
                                {
                                      tmp = _Strdup(&inbuf[8]);
                                      strcpy(callingnumber,tmp);
                                      free(tmp);
                                      tmp = NULL;
                                }
                                else
                                {
                                    if (strncmp(inbuf, "TEST_CALLS=", 11) == 0)
                                    {
                                        num_test_calls = atoi(&inbuf[11]);
                                    }
                                }
                            }
                        } 
                    }
                }
            }
        }
        fclose(fp);
     }

     if (num_serv_points == 0 || auditurl == NULL)
     {
          fprintf(stderr, "Both service points and an audit URL must be configured\n");
          errorcode = 1;
     }
     return errorcode;
}

void
CleanupServicePoints()
{
    int i = 0;

    for (i = 0; i < num_serv_points; i++)
    {
        free(servicepoints[i]);
    }
    free(auditurl);
    return;
}


int
main(int argc, char *argv[])
{
    int menuopt = 1, retcode = 0;

    OSPTNINITLOG("test_app.log");

    if (GetConfiguration())
    {
        exit(1);
    }
    if (argc == 2) {
      if (strcmp(argv[1], "-q") == 0)
         quietmode = 1;
      else
      {
         if(servicepoints[0] != OSPC_OSNULL)
             OSPM_FREE(servicepoints[0]);

         servicepoints[0] = _Strdup(argv[1]);
      }
    }

    if ((retcode = OSPPInit()) != OSPC_ERR_NO_ERROR) 
    {
        printf("initialization failed. errorcode = %d\n", retcode);
        exit(1);
    }

    NonBlockingQueueMonitorNew(&nbMonitor,WORK_THREAD_NUM,1000,(500*1000) );

    do 
    {
        menuopt = testMenu();
        if (menuopt >= 0) 
        {
            retcode = testAPI(menuopt);
            if (!quietmode)
            {
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
    exit(0);
}



static int FileArrayRead(unsigned char ***array,size_t *arrayNum,int *filesizes[],const char *baseName,const char *ext,size_t maxFiles)
{
int    Fretval;
char  file[1024+1];
int    doIt;
int    fd;
int   *fs;
unsigned char  **cp;

  Fretval=(-1);
  doIt=1;
  if((array!=NULL)&&(arrayNum!=NULL))
  {
    *array=NULL;
    *arrayNum=0;
    *filesizes=(int *)NULL;
    if(((cp=(unsigned char **)calloc(1,maxFiles*sizeof(cp)))!=NULL)
        && ((fs=(int *)malloc(maxFiles*sizeof(int)))!=NULL)) 
    {
      size_t  x;

      Fretval=0;
      for(x=0;(x<maxFiles)&&(Fretval==0);x++)
      {
        sprintf(file,"%s_%05d.%s",baseName,x+1,ext);
        if((fd=_Open(file,O_RDONLY | O_BINARY))>=0)
        {
          int    s;
          unsigned char  *data;

          if((s=FileSizeGet(fd))>0)
          {
            if((data=(unsigned char *)malloc(s+1))!=NULL)
            {
              cp[x]=data;
              fs[x]=s;
              data[s]=0;  /* So it looks like a string */
              if(_Read(fd,data,s) != s)
              {
                printf("Unable to read file %s size %u\n",file,s);
                Fretval=(-1);
              }
            }
            else
            {
              printf("Unable to allocate %u bytes for file %s\n",s,file);
              Fretval=(-1);
            }
          }
          else
          {
            printf("Unable to get file size of <%s>\n",file);
            Fretval=(-1);
          }
          (void)_Close(fd);
        }
        else
        {
          break;
        }
      } /* For every file */
      if(Fretval==0)
      {
        *arrayNum=x;
        *array=cp;
        *filesizes=fs;
      }
      else
      {
        FileArrayDelete(&cp,&fs,maxFiles);
      }
    }
    else
    {
      printf("Unable to allocate file pointers\n");
    }
  }
  else
  {
    printf("Bad function parameters\n");
  }
  return(Fretval);
} /* FileArrayRead */


static void FileArrayDelete(unsigned char ***array, int **filesizes,size_t num)
{
unsigned char    **p;
size_t  x;

  if((array!=NULL)&&(num>=1)&&(filesizes!=NULL))
  {
    p=*array;
    for(x=0;x<num;x++)
    {
      if(p[x]!=NULL)
      {
        free(p[x]);
      }
    }
    free(p);
    *array=NULL;
    free(*filesizes);
    *filesizes=NULL;
  }
  return;
}


/********************************************************
*
* Function name : FileSizeGet
* Description   :
* Returns       : -1: error else file size
*
********************************************************/
static off_t FileSizeGet(int fd)
{
off_t x;
off_t Fretval;

  Fretval=(-1);
  if(fd!=(-1))
  {
    if((x=_Lseek(fd,0,SEEK_CUR))!=(-1))
    {
      if(_Lseek(fd,0,SEEK_SET)!=(-1))
      {
        Fretval=_Lseek(fd,0,SEEK_END);
      }
      if(_Lseek(fd,x,SEEK_SET)==(-1))
      {
        Fretval=(-1);
      }
    }
  }
  return(Fretval);
} /* FileSizeGet */





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
int testNonBlockingPerformanceTest()
{
    int      errorcode       = 0;
    unsigned detaillogsize   = 0;


    int             OErrorCodes[TEST_NUM];
    int             TErrorCodes[TEST_NUM];
    OSPTTRANHANDLE  OTransactionHandles[TEST_NUM];
    OSPTTRANHANDLE  TTransactionHandles[TEST_NUM];
    unsigned        NumOfDestinations[TEST_NUM];
    unsigned        TokenSizes[TEST_NUM];
    OSPTCALLID*     CallIds[TEST_NUM];
    unsigned        CallIdsNum[TEST_NUM];
    unsigned        CallIdsLen[TEST_NUM];
    int             i = 0;

    /*
     * Used for calculating performance
    */
    time_t start_time, end_time;


    /*
     * Start non-blocking Queue/Monitor
    */

    /*
     * Init variables
    */
    for(i = 0; i < TEST_NUM; i++)
    {
      /*
       * ErrorCodes
      */
      OErrorCodes[i]            = 0;
      TErrorCodes[i]            = 0;

      /*
       * Number of requested (IN) and returned (OUT) destinations
      */
      NumOfDestinations[i]      = NUM_CALL_IDS;

      /*
       * Prepare space for tokens
      */
      TokenSizes[i]             = TOKEN_SIZE;
      OSPM_MEMSET(Tokens[i], 0, TOKEN_SIZE);

      /*
       * Initialize call ids
      */
      CallIdsNum[i]             = 1;
      CallIdsLen[i]             = strlen("123");
      CallIds[i]                = OSPPCallIdNew(3,(const unsigned char *)"123");

    }



    /* 
     * Phase I Creating new transactions / 2 (O + T) transactions for every call
    */
    printf("\n\n");
    printf("Phase I (Source and Destination): OSPPTransactionNew.\n");
    time(&start_time);
    for(i = 0; i < TEST_NUM; i++)
    {
        if( (errorcode=OSPPTransactionNew(OSPVProviderHandle, &OTransactionHandles[i])) != OSPC_ERR_NO_ERROR ||
            (errorcode=OSPPTransactionNew(OSPVProviderHandle, &TTransactionHandles[i])) != OSPC_ERR_NO_ERROR )
        {
          printf("OSPPTransactionNew failed, aborting the test.\n");
          return errorcode;
        }
    }
    time(&end_time);
    printf("Time elapsed <%d>\n",end_time - start_time);



    /* 
     * Phase II Sending AuthorizationRequests
    */
    printf("\n\n");
    printf("Phase II (Source): OSPPTransactionRequestAuthorisation.\n");
    time(&start_time);
    for(i = 0; i < TEST_NUM; i++)
    {
        errorcode = OSPPTransactionRequestAuthorisation_nb( nbMonitor,
                                                            0, /* DON'T BLOCK */
                                                            &OErrorCodes[i],
                                                            OTransactionHandles[i],
                                                            "[1.1.1.1]",
                                                            "[2.2.2.2]",
                                                            callingnumber,
                                                            callednumber,
                                                            "bgates",
                                                            CallIdsNum[i],
                                                            &CallIds[i],
                                                            (const char **)NULL,
                                                            &NumOfDestinations[i],
                                                            &detaillogsize,
                                                            (void *)NULL);

        if(errorcode != OSPC_ERR_NO_ERROR) 
        {
          printf("OSPPTransactionRequestAuthorisation_nb failed, aborting the test.\n");
          return errorcode;
        }
    }

    /* 
     * Wait
    */
    printf("Waiting on queued up transactions\n");
    NonBlockingQueueMonitorBlockWhileQueueNotEmpty(nbMonitor);


    time(&end_time);
    printf("Time elapsed <%d>\n",end_time - start_time);

    printf("Checking ReturnCodes.\n");
    for(i = 0; i < TEST_NUM; i++)
    {
      if( OErrorCodes[i] != OSPC_ERR_NO_ERROR ) 
      {
        printf("OSPPTransactionRequestAuthorisation failed transaction/code = <%d>/<%d>, aborting the test.\n", i,OErrorCodes[i]);
        return OErrorCodes[i];
      }
    }




    /* 
     * Phase III (Source) Getting 1st destination
    */
    printf("\n\n");
    printf("Phase III (Source): OSPPTransactionGetFirstDestination.\n");
    time(&start_time);
    for(i = 0; i < TEST_NUM; i++)
    {


        errorcode = OSPPTransactionGetFirstDestination( OTransactionHandles[i],
                                                        /* TIMESTAMP_SZ,*/
                                                        0,
                                                        validafter,
                                                        validuntil,
                                                        &timelimit,
                                                        &CallIdsLen[i],
                                                        CallIds[i],
                                                        strlen(callednumber) + 1,
                                                        callednumber,
                                                        DESTINATION_SZ,
                                                        dest,
                                                        DESTINATION_SZ,
                                                        destdev,
                                                        &TokenSizes[i],
                                                        Tokens[i]);

        if(errorcode != OSPC_ERR_NO_ERROR) 
        {
          printf("OSPPTransactionGetFirstDestination failed, aborting the test.\n");
          return errorcode;
        }
    }

    time(&end_time);
    printf("Time elapsed <%d>\n",end_time - start_time);




    /* 
     * Phase III (Destination) Validate destination
    */
    printf("\n\n");
    printf("Phase III (Destination): OSPPTransactionValidateAuthorisation.\n");
    time(&start_time);
    for(i = 0; i < TEST_NUM; i++)
    {
        unsigned    authorised      = 0;

        errorcode = OSPPTransactionValidateAuthorisation(TTransactionHandles[i],
                                                        "[1.1.1.1]",
                                                        "[2.2.2.2]",
                                                        NULL,
                                                        NULL,
                                                        callingnumber,
                                                        callednumber,
                                                        CallIds[i]->ospmCallIdLen,
                                                        CallIds[i]->ospmCallIdVal,
                                                        TokenSizes[i],
                                                        Tokens[i],
                                                        &authorised,
                                                        &timelimit,
                                                        &detaillogsize,
                                                        (void *)NULL);

        if( (errorcode != OSPC_ERR_NO_ERROR) || (authorised != 1)) 
        {
          printf("OSPPTransactionValidateAuthorisation failed, aborting the test.\n");
          return errorcode;
        }
    }

    time(&end_time);
    printf("Time elapsed <%d>\n",end_time - start_time);




    /* 
     * Phase IV Sending 2 (Source and Destination) UsageIndications for each call
    */
    printf("\n\n");
    printf("Phase IV (Source and Destination): OSPPTransactionReportUsage.\n");
    time(&start_time);
    for(i = 0; i < TEST_NUM; i++)
    {
        errorcode = OSPPTransactionReportUsage_nb(  nbMonitor,
                                                    0, /* DON'T BLOCK */
                                                    &OErrorCodes[i],
                                                    OTransactionHandles[i],
                                                    30,
																										time(NULL)-10,
                                                    1,
                                                    2,
                                                    100,
                                                    10,
                                                    &detaillogsize,
                                                    (void *)NULL);
        if(errorcode != OSPC_ERR_NO_ERROR) 
        {
          printf("OSPPTransactionReportUsage_nb failed, aborting the test.\n");
          return errorcode;
        }


        errorcode = OSPPTransactionReportUsage_nb(  nbMonitor,
                                                    0, /* DON'T BLOCK */
                                                    &TErrorCodes[i],
                                                    TTransactionHandles[i],
                                                    30,
																										time(NULL)-10,
                                                    1,
                                                    2,
                                                    100,
                                                    10,
                                                    &detaillogsize,
                                                    (void *)NULL);
        if(errorcode != OSPC_ERR_NO_ERROR) 
        {
          printf("OSPPTransactionReportUsage_nb failed, aborting the test.\n");
          return errorcode;
        }
    }

    /* 
     * Wait
    */
    printf("Waiting on queued up transactions\n");
    NonBlockingQueueMonitorBlockWhileQueueNotEmpty(nbMonitor);

    time(&end_time);
    printf("Time elapsed <%d>\n",end_time - start_time);


    printf("Checking ReturnCodes.\n");
    for(i = 0; i < TEST_NUM; i++)
    {
      if( OErrorCodes[i] != OSPC_ERR_NO_ERROR ) 
      {
        printf("Source OSPPTransactionReportUsage failed transaction/code = <%d>/<%d>, aborting the test.\n", i,OErrorCodes[i]);
        return OErrorCodes[i];
      }

      if( TErrorCodes[i] != OSPC_ERR_NO_ERROR ) 
      {
        printf("Destination OSPPTransactionReportUsage failed transaction/code = <%d>/<%d>, aborting the test.\n", i,TErrorCodes[i]);
        return TErrorCodes[i];
      }
    }


    /* 
     * Phase V Deleting transactions
    */
    printf("\n\n");
    printf("Phase V: OSPPTransactionDelete.\n");
    time(&start_time);
    for(i = 0; i < TEST_NUM; i++)
    {
        if( (errorcode=OSPPTransactionDelete(OTransactionHandles[i])) != OSPC_ERR_NO_ERROR ||
            (errorcode=OSPPTransactionDelete(TTransactionHandles[i])) != OSPC_ERR_NO_ERROR )
        {
          printf("OSPPTransactionDelete failed, aborting the test.\n");
          return errorcode;
        }
        OSPPCallIdDelete(&CallIds[i]);
    }
    time(&end_time);
    printf("Time elapsed <%d>\n",end_time - start_time);


    return errorcode;
}
/* EOF */
