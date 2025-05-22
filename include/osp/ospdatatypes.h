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

/* ospdatatypes.h - Structures and defines for OSP API. */
#ifndef _OSPDATATYPES_H
#define _OSPDATATYPES_H

#include "osp/osposincl.h"
#include "osp/osplist.h"
#include "osp/osptoken.h"

/* NOTE: ospcallid.h is included at the end of this file. */

/* common data type typedefs */
#ifndef _WIN32

#ifdef __alpha__
typedef unsigned long OSPTUINT64;
#else
typedef unsigned long long OSPTUINT64;
#endif /* __alpha__ */

#ifdef _POSIX_THREADS
/* POSIX thread typedefs */
typedef pthread_t OSPTTHREADID;
typedef pthread_attr_t OSPTTHRATTR;
typedef pthread_cond_t OSPTCONDVAR;
typedef pthread_mutex_t OSPTMUTEX;
typedef void *OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL() return (OSPTTHREADRETURN)NULL
#else
/* SVR4 thread typedefs */
typedef thread_t OSPTTHREADID;
typedef long OSPTTHRATTR;
typedef cond_t OSPTCONDVAR;
typedef mutex_t OSPTMUTEX;
typedef void *OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL() return (OSPTTHREADRETURN)NULL
#endif /* _POSIX_THREADS */

typedef ssize_t OSPTSSIZE_T;
typedef int OSPTSOCKET;

#else /* _WIN32 */

#ifdef _POSIX_THREADS
/* POSIX thread typedefs */
typedef pthread_t OSPTTHREADID;
typedef pthread_attr_t OSPTTHRATTR;
typedef pthread_cond_t OSPTCONDVAR;
typedef pthread_mutex_t OSPTMUTEX;
typedef void *OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL() return (OSPTTHREADRETURN)NULL
#else
/* WIN32 thread typedefs */
typedef unsigned long OSPTTHREADID;
typedef unsigned OSPTTHRATTR;
typedef struct {
    HANDLE Semaphore;
    int SemaCount;
    unsigned WaitLimit;
} OSPTCONDVAR;
typedef HANDLE OSPTMUTEX;
typedef void OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL() return
#endif /* _POSIX_THREADS */

typedef unsigned OSPTSSIZE_T;
typedef unsigned __int64 OSPTUINT64;
typedef SOCKET OSPTSOCKET;

#endif /* _WIN32 */

typedef long OSPTIPADDR;
typedef unsigned OSPTBOOL;

#define OSPC_OSNULL     NULL
#define OSPTTIME        time_t

#define OSPC_TRUE       1
#define OSPC_FALSE      0

typedef int OSPTPROVHANDLE;
typedef int OSPTTRANHANDLE;

#define OSPM_MIN(a,b) ((a) < (b) ? (a) : (b))

/* Transaction Id typedef */
typedef OSPTUINT64 OSPTTRXID;

/* transaction id tracking typedef */
typedef struct _OSPTTXID {
    OSPTUINT64 TransId;
    struct _OSPTIDTRACK *Next;
} OSPTTXID;

/* collection index typedef */
typedef struct _OSPTCOLLECTIONINDEX {
    int Index;
} OSPTCOLLECTIONINDEX;

/* Security - Private Key */
typedef struct _OSPTPRIVATEKEY {
    unsigned char *PrivateKeyData;
    unsigned PrivateKeyLength;
} OSPTPRIVATEKEY;

/* Security - Cert */
typedef struct {
    unsigned char *CertData;
    unsigned CertDataLength;
} OSPT_CERT;

/* Misc Lock Modes */
typedef enum {
    OSPC_LOCK_WRITE,
    OSPC_LOCK_READ,
    OSPC_LOCK_TRY
} OSPE_LOCK_MODE;

/* Enumerated Data Types for Auditing */
typedef enum {
    OSPC_AUDIT_LOWER_BOUND = 10,
    OSPC_AUDIT_INIT,
    OSPC_AUDIT_FLUSH_BUFFER,
    OSPC_AUDIT_OK,
    OSPC_AUDIT_UPPER_BOUND
} OSPE_AUDIT_STATE;

#define OSPC_SIZE_CURRENCY  5       /* We support only 3 digit Currency codes */
#define OSPC_SIZE_CONFID    128     /* Same as Call Id Size */
#define OSPC_SIZE_UNIT      20

typedef enum {
    OSPC_NFORMAT_START = 0,
    OSPC_NFORMAT_CNAM = OSPC_NFORMAT_START,
    OSPC_NFORMAT_DISPLAYNAME,
    OSPC_NFORMAT_E164,
    OSPC_NFORMAT_SIP,
    OSPC_NFORMAT_TRANSPORT,
    OSPC_NFORMAT_URL,
    /* Number of call party formats */
    OSPC_NFORMAT_NUMBER
} OSPE_NUMBER_FORMAT;

typedef enum {
    OSPC_TCAUSE_START = 0,
    OSPC_TCAUSE_H323 = OSPC_TCAUSE_START,
    OSPC_TCAUSE_Q850,
    OSPC_TCAUSE_SIP,
    OSPC_TCAUSE_XMPP,
    /* Number of termination cause types */
    OSPC_TCAUSE_NUMBER
} OSPE_TERM_CAUSE;

typedef enum {
    OSPC_SIPHEADER_START = 0,
    OSPC_SIPHEADER_CONTACT = OSPC_SIPHEADER_START,
    OSPC_SIPHEADER_DIV,
    OSPC_SIPHEADER_FROM,
    OSPC_SIPHEADER_IDBYPASS,
    OSPC_SIPHEADER_IDENTITY,
    OSPC_SIPHEADER_PAI,
    OSPC_SIPHEADER_PCI,
    OSPC_SIPHEADER_RPID,
    OSPC_SIPHEADER_RURI,
    OSPC_SIPHEADER_TO,
    /* Number of termination cause types */
    OSPC_SIPHEADER_NUMBER
} OSPE_SIP_HEADER;

typedef struct {
    float amount;
    unsigned increment;
    char unit[OSPC_SIZE_UNIT];
    char currency[OSPC_SIZE_CURRENCY];
} OSPT_PRICING_INFO;

typedef enum {
    OSPC_STISERVICE_START = 0,
    OSPC_STISERVICE_AUTH = OSPC_STISERVICE_START,
    OSPC_STISERVICE_VERI,
    /* Number of STI service types */
    OSPC_STISERVICE_NUMBER
} OSPE_STI_SERVICE;

#include "ospcallid.h"

#endif /* _OSPDATATYPES_H */
