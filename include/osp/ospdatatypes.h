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
 * ospdatatypes.h - Structures and defines for OSP api.
 */

#ifndef _OSP_DATATYPES_H
#define _OSP_DATATYPES_H

#include "osp/osposincl.h"
#include "osp/osplist.h"
#include "osp/osptoken.h"

/* NOTE: ospcallid.h is included at the end of this file. */

/*-------------------------------------------*/
/* common data type typedefs                 */
/*-------------------------------------------*/               
#ifndef _WIN32

#ifdef __alpha__
typedef unsigned long       OSPTUINT64;
#else
typedef unsigned long long  OSPTUINT64;
#endif /* __alpha__*/

#ifdef _POSIX_THREADS
/* POSIX thread typedefs */
typedef pthread_t           OSPTTHREADID;
typedef pthread_attr_t      OSPTTHRATTR;
typedef pthread_cond_t      OSPTCONDVAR;
typedef pthread_mutex_t     OSPTMUTEX;
typedef void *              OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL()	return (OSPTTHREADRETURN)NULL
#else
/* SVR4 thread typedefs */
typedef thread_t            OSPTTHREADID;
typedef long                OSPTTHRATTR;
typedef cond_t              OSPTCONDVAR;
typedef mutex_t             OSPTMUTEX;
typedef void *              OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL()	return (OSPTTHREADRETURN)NULL
#endif /* _POSIX_THREADS */

typedef ssize_t             OSPTSSIZE_T;
typedef int                 OSPTSOCKET;

#else /* _WIN32 */

#ifdef _POSIX_THREADS
/* POSIX thread typedefs */
typedef pthread_t           OSPTTHREADID;
typedef pthread_attr_t      OSPTTHRATTR;
typedef pthread_cond_t      OSPTCONDVAR;
typedef pthread_mutex_t     OSPTMUTEX;
typedef void *              OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL()	return (OSPTTHREADRETURN)NULL
#else
/* WIN32 thread typedefs */
typedef unsigned long       OSPTTHREADID;
typedef unsigned            OSPTTHRATTR;
typedef struct
{
    HANDLE     Semaphore;
    int        SemaCount;
    unsigned   WaitLimit;
} OSPTCONDVAR;
typedef HANDLE              OSPTMUTEX;
typedef void                OSPTTHREADRETURN;
#define OSPTTHREADRETURN_NULL()	return
#endif /* _POSIX_THREADS */

typedef unsigned            OSPTSSIZE_T;
typedef unsigned __int64    OSPTUINT64;
typedef SOCKET              OSPTSOCKET;

#endif /* _WIN32 */

typedef long            OSPTIPADDR;
typedef unsigned char   OSPTBOOL;

#define OSPC_OSNULL     NULL
#define OSPTTIME        time_t

#define OSPC_TRUE       1
#define OSPC_FALSE      0

typedef int OSPTPROVHANDLE;
typedef int OSPTTRANHANDLE;

#ifndef tr_min
#define tr_min(a,b) ((a) < (b) ? (a) : (b)) 
#endif

/*-------------------------------------------*/
/* Transaction Id typedef                    */
/*-------------------------------------------*/ 
typedef OSPTUINT64 OSPTTRXID;

/*-------------------------------------------*/
/* transaction id tracking typedef           */
/*-------------------------------------------*/ 
typedef struct _OSPTTXID
{
    OSPTUINT64          TransId;
    struct _OSPTIDTRACK *Next;
} OSPTTXID;

/*-------------------------------------------*/
/* call delay typedef                        */
/*-------------------------------------------*/ 
typedef struct _OSPTDELAY
{
    unsigned    HasValue;
    unsigned    NumberOfSamples;
    unsigned    Minimum;
    unsigned    Mean;
    float       Variance;
    double      SumOfSampSquares;
} OSPTDELAY;

/*-------------------------------------------*/
/* collection index typedef                  */
/*-------------------------------------------*/ 
typedef struct  _OSPTCOLLECTIONINDEX
{
    int Index;
} OSPTCOLLECTIONINDEX;


/*-------------------------------------------*/
/* Security - Private Key                    */
/*-------------------------------------------*/
typedef struct _OSPTPRIVATEKEY
{
    unsigned char *PrivateKeyData;
    unsigned PrivateKeyLength;
} OSPTPRIVATEKEY;


/*-------------------------------------------*/
/* Security - Cert                           */
/*-------------------------------------------*/
typedef struct _OSPTCERT
{
    unsigned char *CertData;
    unsigned CertDataLength;
} OSPTCERT;

/*-------------------------------------------*/
/* Misc Lock Modes                           */
/*-------------------------------------------*/
typedef enum _OSPELOCKMODE
{
    OSPE_LOCK_WRITE,
    OSPE_LOCK_READ,
    OSPE_LOCK_TRY
} OSPELOCKMODE;

/*-------------------------------------------*/
/* Enumerated Data Types for Auditing        */
/*-------------------------------------------*/
typedef enum {
    OSPC_AUDIT_LOWER_BOUND=10,
    OSPC_AUDIT_INIT,
    OSPC_AUDIT_FLUSH_BUFFER,
    OSPC_AUDIT_OK,
    OSPC_AUDIT_UPPER_BOUND
} OSPE_AUDIT_STATE;

#define OSPC_CURRENCYSIZE 5 /* We support only 3 digit Currency codes */
#define OSPC_CONFIDSIZE 128 /* Same as Call Id Size */
#define OSPC_UNITSIZE 20

typedef enum
{
    OSPC_E164=0,
    OSPC_SIP,
    OSPC_URL
}OSPE_NUMBERING_FORMAT;

typedef enum
{
    OSPC_VOICE=0,
    OSPC_DATA
}OSPE_SERVICE_TYPE;

typedef struct
{
    unsigned amount;
    unsigned increment;
    unsigned char unit[OSPC_UNITSIZE];
    unsigned char currency[OSPC_CURRENCYSIZE]; 
}OSPT_PRICING_INFO;

#include "ospcallid.h"

#endif
