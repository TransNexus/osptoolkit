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
* ospopenssl.c - SSL API bridge to openSSL library implementation (v0.9.4)
*/

#include "osp.h"
#include "ospsocket.h"
#include "ospssl.h"
#include "osputils.h"
#include "ospcomm.h"
#include "ospsecurity.h"

/* 
** OpenSSL headers 
*/

#include "openssl/bio.h"
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/rand.h"

#ifdef OSPC_HW_ACCEL
  #include "openssl/engine.h"
#endif


#define OSPC_MAX_CERT_BUFFER         4096

/*
** Local Prototype
*/

int OSPPSSLVerifyCallback(int ok, X509_STORE_CTX *ctx);
long bio_dump_cb(BIO *bio, int cmd, const char *argp, int argi, long argl, long ret);
int OSPPSSLLoadCerts(OSPTSEC *ospvRef);
int rand_init();
int cha_engine_init();

/*
** BIO_stdout = File handle for output of SSL debugging
*/
BIO *bio_stdout=NULL;

/*
** OSPC_DBG_SSL constant for debugging OpenSSL SSL session
**  uncomment the define for ssl debugging
**
#define OSPC_DBG_SSL 1
*/

int
OSPPSSLWrapInit(void *ospvRef)
{
    int          errorcode = OSPC_ERR_NO_ERROR,
        off       = 0;
    SSL_CTX    **ctx       = OSPC_OSNULL;
    SSL_METHOD  *version   = OSPC_OSNULL;
    OSPTSEC     *security  = OSPC_OSNULL;
    
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapInit()\n"));
    
    security = (OSPTSEC *)ospvRef;
    
    if (security != OSPC_OSNULL)
    {
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
				OpenSSL_add_all_digests();
        
/* The following macro definition is to fix an anomaly between
   openssl and OSP. The anomaly only occurs on Windows */
#ifdef _WIN32
        bio_stdout=BIO_new_fp((FILE *)SSLGetSSLstdout(),BIO_NOCLOSE);
#else
        bio_stdout=BIO_new_fp(stdout,BIO_NOCLOSE);
#endif
        /*
        **  Seed random generator
        */
        rand_init();

        /*
        **  Set Cryptographic Hardware Acceleration Engine
        */
        cha_engine_init();

        ctx = (SSL_CTX **)&(security->ContextRef);
        version = SSLv3_client_method();
        *ctx = SSL_CTX_new(version);
        
        if (*ctx != OSPC_OSNULL)
        {
            SSL_CTX_set_options(*ctx,off);
            SSL_CTX_set_timeout(*ctx, OSPPSecGetSSLLifetime(security));
            SSL_CTX_set_verify(*ctx,SSL_VERIFY_PEER|SSL_VERIFY_CLIENT_ONCE ,OSPPSSLVerifyCallback);
        }
    }
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapInit() (%d)\n",errorcode));
    return errorcode;
}

void
OSPPSSLWrapCleanup(void *ospvRef)
{
    SSL_CTX    **ctx       = OSPC_OSNULL;
    OSPTSEC     *security  = OSPC_OSNULL;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapCleanup()\n"));
    security = (OSPTSEC *)ospvRef;
    if (security != OSPC_OSNULL)
    {
        ctx = (SSL_CTX **)&(security->ContextRef);
        SSL_CTX_free(*ctx);
    }
    if (bio_stdout != OSPC_OSNULL)
		{
        BIO_free(bio_stdout);
        bio_stdout = OSPC_OSNULL;
		}
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapCleanup()\n"));
    return;
}

int
OSPPSSLWrapSessionContextNew(
                             void  *ospvConnection,
                             void  *ospvContextRef)
{
    int                 errorcode  = OSPC_ERR_NO_ERROR;
    SSL_CTX         *ctx        = OSPC_OSNULL;
    SSL            **conref     = OSPC_OSNULL;
    OSPTSEC         *security   = OSPC_OSNULL;
    OSPTSSLSESSION  *sslsession = OSPC_OSNULL;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSessionContextNew()\n"));
    
    OSPM_ARGUSED(ospvConnection); /* not needed for SSLEAY */
    
    security = (OSPTSEC *)ospvContextRef;
    
    /* 
    *Make sure we have security before we 
    * start de-referencing
    */
    
    if(security != OSPC_OSNULL)
    {
        ctx = (SSL_CTX *)security->ContextRef;
        sslsession = ((OSPTHTTP *)ospvConnection)->SSLSession;
        
        conref = (SSL **)&(sslsession->Context);
        
        if(OSPPSSLLoadCerts(security)==OSPC_ERR_NO_ERROR)
        {
            *conref = (SSL *)SSL_new(ctx);
            
            if (*conref == OSPC_OSNULL)
            {
                ERR_print_errors(bio_stdout);
                errorcode = OSPC_ERR_SSL_MALLOC_FAILED;
                OSPM_DBGERRORLOG(errorcode, "malloc of new SSL Context failed");
            }
#ifdef OSPC_DBG_SSL
            (*conref)->debug = 1;
#endif
        }
    }
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSessionContextNew() (%d)\n", errorcode));
    
    return errorcode;
}

int
OSPPSSLWrapAttachConnection(
                            OSPTSSLSESSION *ospvSSLSession, 
                            void           *ospvConnection)
{
    int             errorcode  = OSPC_ERR_NO_ERROR;
    BIO            *sbio       = OSPC_OSNULL;
    SSL            *conref     = OSPC_OSNULL;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapAttachConnection()\n"));
    
    conref = (SSL *)OSPPSSLSessionGetContext(ospvSSLSession);
    
    sbio = BIO_new_socket(((OSPTHTTP *)ospvConnection)->SockFd, BIO_NOCLOSE);
    
    if (sbio != OSPC_OSNULL)
    {
                                SSL_set_bio(conref, sbio, sbio);
                                
#ifdef OSPC_DBG_SSL                             
                                BIO_set_callback(sbio,bio_dump_cb);
                                BIO_set_callback_arg(sbio,bio_stdout);
#endif
                                
                                SSL_set_connect_state(conref);
                                SSL_set_bio(conref, sbio, sbio);
                                SSL_set_connect_state(conref);
    }
    else
    {
                                errorcode = OSPC_ERR_SSL_ATTACH_SOCK_FAILED;
    }
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapAttachConnection() (%d)\n", errorcode));
    return errorcode;
}

int
OSPPSSLWrapHandshake( 
                     OSPTSSLSESSION *ospvSSLSession)
{
    int    errorcode      = OSPC_ERR_NO_ERROR;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapHandshake()\n"));
    OSPM_ARGUSED(ospvSSLSession);
    
    /* 
    ** automagically done by SSLeay 
    */
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapHandshake() (%d)\n", errorcode));
    return errorcode;
}

int
OSPPSSLWrapSessionContextDelete(
                                OSPTSSLSESSION *ospvSSLSession)
{
    int    errorcode  = OSPC_ERR_NO_ERROR;
    SSL   *conref     = OSPC_OSNULL;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSessionContextDelete()\n"));
    
    conref = (SSL *)OSPPSSLSessionGetContext(ospvSSLSession);
    SSL_free(conref);
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSessionContextDelete()\n"));
    
    return errorcode;
}


int
OSPPSSLWrapGetData(
                   void           *ospvBuffer, 
                   unsigned int   *ospvLength, 
                   OSPTSSLSESSION *ospvSSLSession)
{
    int  errorcode      = OSPC_ERR_NO_ERROR,
        expected       = 0,
        sslerr         = 0,
        bytesread      = 0;
    SSL *conref         = OSPC_OSNULL;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapGetData()\n"));
    conref = (SSL *)OSPPSSLSessionGetContext(ospvSSLSession);
    
    do
    {
                                bytesread = SSL_read(conref, (char *)ospvBuffer + expected, *ospvLength - expected);
                                expected += bytesread;
    } while (expected != (int)*ospvLength && bytesread > 0);
    
    if (expected != (int)*ospvLength)
    {
                                sslerr = SSL_get_error(conref, bytesread);
                                OSPM_DBGERRORLOG(sslerr, "SSL_read() failed");
                                errorcode = OSPC_ERR_SSL_READ_FAILED;
    }
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapGetData() (%d)\n", errorcode));
    
    return errorcode;
}

int
OSPPSSLWrapSendData(
                    void           *ospvBuffer, 
                    unsigned int   *ospvLength, 
                    OSPTSSLSESSION *ospvSSLSession)
{
    int  errorcode      = OSPC_ERR_NO_ERROR,
        expected       = 0,
        sslerr         = 0,
        byteswritten   = 0;
    SSL *conref         = OSPC_OSNULL;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSendData()\n"));
    
    conref = (SSL *)OSPPSSLSessionGetContext(ospvSSLSession);
    
    ERR_print_errors(bio_stdout);
    
    do
    {
        byteswritten = SSL_write(conref, (char *)ospvBuffer, *ospvLength);
        expected += byteswritten;
    } while (expected != (int)*ospvLength && byteswritten > 0);
    
    if (expected != (int)*ospvLength)
    {
        sslerr = SSL_get_error(conref, byteswritten);
        OSPM_PRINTF("SSLERROR: Error: %d\n",sslerr);
        OSPM_DBGERRORLOG(sslerr, "SSL_write() failed");
        errorcode = OSPC_ERR_SSL_WRITE_FAILED;
    }
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSendData() (%d)\n", errorcode));
    return errorcode;
}

int
OSPPSSLWrapSessionGracefulShutdown(OSPTSSLSESSION *ospvSSLSession)
{
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSessionGracefulShutdown()\n"));
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSessionGracefulShutdown() (%d)\n", 0));
    return OSPC_ERR_NO_ERROR;
}

int 
OSPPSSLWrapGetServerRootCACert(
                               void **ospvRootCACert,
                               int *ospvRootCACertLen,
                               OSPTSSLSESSION *ospvSSLSession)
{
    
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapGetServerRootCACert()\n"));
    
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_ARGUSED(ospvRootCACert);
    OSPM_ARGUSED(ospvRootCACertLen);
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapGetServerRootCACert() (%d)\n", 0));
    return OSPC_ERR_NO_ERROR;
    
}

void OSPPSSLWrapFreeServerRootCACert(void **ospvRootCACert)
{
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapFreeServerRootCACert()\n"));
    OSPM_ARGUSED(ospvRootCACert);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapFreeServerRootCACert() (%d)\n", 0));
    return ;
}

int 
OSPPSSLVerifyCallback(int ok, X509_STORE_CTX *ctx)
{
    int verify_depth=1;
    int verify_error=X509_V_OK;
    
    char buf[256];
    X509 *err_cert;
    int err,depth;
    
    err_cert=X509_STORE_CTX_get_current_cert(ctx);
    err=    X509_STORE_CTX_get_error(ctx);
    depth=  X509_STORE_CTX_get_error_depth(ctx);
    
    X509_NAME_oneline(X509_get_subject_name(err_cert),buf,256);
    BIO_printf(bio_stdout,"depth=%d %s\n",depth,buf);
    if (!ok)
    {
        BIO_printf(bio_stdout,"verify error:num=%d:%s\n",err,
            X509_verify_cert_error_string(err));
        if (verify_depth >= depth || err == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT)
        {
            ok=1;
            verify_error=X509_V_OK;
        }
        else
        {
            ok=0;
            verify_error=X509_V_ERR_CERT_CHAIN_TOO_LONG;
        }
    }
    switch (ctx->error)
    {
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
        X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert),buf,256);
        BIO_printf(bio_stdout,"issuer= %s\n",buf);
        break;
    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
        BIO_printf(bio_stdout,"notBefore=");
        ASN1_TIME_print(bio_stdout,X509_get_notBefore(ctx->current_cert));
        BIO_printf(bio_stdout,"\n");
        break;
    case X509_V_ERR_CERT_HAS_EXPIRED:
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
        BIO_printf(bio_stdout,"notAfter=");
        ASN1_TIME_print(bio_stdout,X509_get_notAfter(ctx->current_cert));
        BIO_printf(bio_stdout,"\n");
        break;
    }
    BIO_printf(bio_stdout,"verify return:%d\n",ok);
    return(ok);
}

long bio_dump_cb(BIO *bio, int cmd, const char *argp, int argi, long argl, long ret)
{
    BIO *out;
    
    out=(BIO *)BIO_get_callback_arg(bio);
    
    if (out == NULL) return(ret);
    
    if (cmd == (BIO_CB_READ|BIO_CB_RETURN))
    {
        BIO_printf(out,"read from %08X [%08lX] (%d bytes => %ld (0x%X))\n",
            bio,argp,argi,ret,ret);
        BIO_dump(out,argp,(int)ret);
        return(ret);
    }
    else if (cmd == (BIO_CB_WRITE|BIO_CB_RETURN))
    {
        BIO_printf(out,"write to %08X [%08lX] (%d bytes => %ld (0x%X))\n",
            bio,argp,argi,ret,ret);
        BIO_dump(out,argp,(int)ret);
    }
    return(ret);
}

int OSPPSSLLoadCerts(OSPTSEC *security)
{
    unsigned        count       = 0,
                    i           = 0;
    unsigned        certlen     = 0;
    unsigned char   *ca         = OSPC_OSNULL, 
                    *pkey       = OSPC_OSNULL,
                    certbuf[OSPC_MAX_CERT_BUFFER]="";
    X509            *x509       = OSPC_OSNULL;
    SSL_CTX         **ctx       = OSPC_OSNULL;
    int             errorcode   = OSPC_ERR_SEC_MODULE;
    
    OSPM_DBGENTER(("ENTER: OSPPSSLLoadCerts()\n"));
    
    /*
    ** Make sure the Security Object is present, this object contains
    ** the certificates (CA,LOCAL,PRIVATE)
    */
    
    if (security != OSPC_OSNULL)
    {
        if((ctx = (SSL_CTX **)&(security->ContextRef))!=OSPC_OSNULL)
            if((errorcode=OSPPSecGetNumberOfAuthorityCertificates(security,&count))==OSPC_ERR_NO_ERROR) 
        {
                if(count<=0)
                {
                    errorcode= OSPC_ERR_SEC_NO_AUTHORITY_CERTIFICATES;
                    OSPM_DBGERRORLOG(errorcode, "There are no CA Certificates available");
                }
                else
                {
                    for(i=0;i<count;i++) 
                    {
                        if(security->AuthorityCertInfo[i]!=OSPC_OSNULL) 
                        {
                            if((errorcode=OSPPX509CertGetCertificate(security->AuthorityCertInfo[i], 
                                &ca, &certlen))==OSPC_ERR_NO_ERROR)
                {
                                if((x509=d2i_X509(NULL,&ca,certlen))!=OSPC_OSNULL) 
                                {
                                    SSL_CTX_add_client_CA(*ctx,x509);
                                } 
                                else 
                                {
                                    errorcode= OSPC_ERR_SEC_NO_SPACE_FOR_CERTIFICATE ;
                                    OSPM_DBGERRORLOG(errorcode, "Unable to create X509 certificate authority");
                                    ERR_print_errors(bio_stdout);
                                    break;
                                }
                }
                        }
                    }
                }
                
                if(errorcode==OSPC_ERR_NO_ERROR) 
                {
                    certlen=OSPC_MAX_CERT_BUFFER;
                    if((errorcode=OSPPSecCopyLocalCertificate(security,
                        &certlen,&certbuf[0]))!=OSPC_ERR_NO_ERROR) 
                    {
                        errorcode=OSPC_ERR_SEC_LOCAL_CERTINFO_UNDEFINED;
                        OSPM_DBGERRORLOG(errorcode, "Unable to get Local Certificate");
                    }
                    else
                    {
                        ca=&certbuf[0];
                        if((x509=d2i_X509(NULL,&ca,certlen))!=OSPC_OSNULL) 
                        {
                            if(SSL_CTX_use_certificate(*ctx,x509)>0) 
                            {
                                if(certlen>OSPC_MAX_CERT_BUFFER)
                                {
                                    errorcode=OSPC_ERR_SEC_CERTIFICATE_TOO_BIG;
                                    OSPM_DBGERRORLOG(errorcode, "Private Key is too big");
                                }
                                else
                                {
                                    if((errorcode=OSPPSecGetPrivateKeyData(security,&pkey,
                                        (unsigned int *)&certlen))!=OSPC_ERR_NO_ERROR)
                                    {
                                        errorcode=OSPC_ERR_SEC_PRIVATE_KEY_NOT_FOUND;
                                        OSPM_DBGERRORLOG(errorcode, "Unable to get private key");
                                    }
                                    else
                                    {
                                        if(!SSL_CTX_use_RSAPrivateKey_ASN1(*ctx,pkey,(long)certlen)) 
                                        {
                                            errorcode= OSPC_ERR_SEC_NO_PRIVATE_KEY ;
                                            OSPM_DBGERRORLOG(errorcode, "Unable to load RSAPrivate Key");
                                            ERR_print_errors(bio_stdout);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
        }
    }
    else
    {
        errorcode=OSPC_ERR_SEC_MODULE;
        OSPM_DBGERRORLOG(errorcode, "Security Context is not valid");
    }
    
    OSPM_DBGEXIT(("EXIT : OSPPSSLLoadCerts() (%d)\n", 0));
    return 0;
}

/*
**  Read from ".rnd" file located in the current directory and
**  seed the PRNG.  If the file does not exist or is smaller
**  than 128 bits (128 / 8 = 16 bytes) long, an error occures.
*/
int rand_init()
{
  int  errorcode = OSPC_ERR_NO_ERROR;
  int  bytesread = -1;

  OSPM_DBGENTER(("ENTER: rand_init()\n"));

  bytesread =  RAND_load_file(".rnd",-1);

  if( bytesread < 16 )
  {
    errorcode=OSPC_ERR_SEC_MODULE;
    OSPM_DBGERRORLOG(errorcode, "Random seed file does not exist or is to small");
  }

  OSPM_DBGEXIT(("EXIT : rand_init() (%d)\n", errorcode));

  return( errorcode );
}


/*
**  If compile time flag "hardware_accel" is set,
**  iterate through all supported engines and set
**  the first available one for all crypto operations
*/
int cha_engine_init()
{
  int  errorcode = OSPC_ERR_NO_ERROR;

#ifdef OSPC_HW_ACCEL
  ENGINE *eng    = OSPC_OSNULL;

  OSPM_DBGENTER(("ENTER: cha_engine_init()\n"));

  for(eng  = ENGINE_get_first();
      eng != OSPC_OSNULL;
      eng  = ENGINE_get_next(eng))
  {
    /* skip 'openssl' because it is a software engine */
    if( strcmp("openssl",ENGINE_get_id(eng)) != 0 )
    {
      if( ENGINE_set_default(eng,ENGINE_METHOD_ALL) != 0)
      {
        /* Success */
        break;
      }
    }
  }

  if( OSPC_OSNULL == eng )
  {
    /* Iterated though all supported engines and failed to set any of them */
    errorcode=OSPC_ERR_SEC_MODULE;
    OSPM_DBGERRORLOG(errorcode, "Failed to set hardware engine support");
  }

  OSPM_DBGEXIT(("EXIT : cha_engine_init() (%d)\n", errorcode));
#endif

  return( errorcode );
}





