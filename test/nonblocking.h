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
 *  nonblocking.h - Structures and prototypes for non-blocking.
 *  
 *  This module implements non-blocking API on top of blocking 
 *  OSP Toolkit API.
 *
 *  Before the non-blocking API may be used, an instance of
 *  NonBlockingQueueMonitor must be created using NonBlockingQueueMonitorNew.
 *  This method allocates space, initializes private variables and starts
 *  consumer/work threads which will be responsible for making blocking calls.
 *  NOTE: Number of work/consumer threads should not exceed the maximum number
 *  of HTTP connections set in the OSPPProviderNew.
 *
 *  When the non-blocking API is no longer needed the NonBlockingQueueMonitor
 *  should be deleted using NonBlockingQueueMonitorDelete.  This method
 *  signals that the shutdown is in progress (which prevets non-blocking API
 *  from accepting any more transactions), blocks while current transactions
 *  are still being processed, releases consumer/work threads and deallocates space.
 *
 *  Method NonBlockingQueueMonitorBlockWhileQueueNotEmpty allows the caller to block
 *  while there are transactions being processed.  NOTE: This function call does not
 *  prevent from adding more non-blocking transactions and maybe starved if there is a 
 *  sufficient flow of transactions.
 *
 *  There are 2 non-blocking methods OSPPTransactionRequestAuthorisation_nb, and 
 *  OSPPTransactionReportUsage_nb which correspond to the OSP Toolkit API methods
 *  OSPPTransactionRequestAuthorisation and OSPPTransactionReportUsage respectively.
 *  The non-blocking methods follow the same naming conventions with an exception of
 *  having a postfix - '_nb'.
 *  The non-blocking methods, in addition to expecting the same variables as their
 *  blocking counterparts, expect 2 variables of types: NBMONITOR* and int*.
 *  The 1st one is a pointer to the NonBlockingQueueMonitor (which should have been
 *  initialized using NonBlockingQueueMonitorNew).
 *  The 2nd one is a pointer to the space where return value from the blocking 
 *  function should be stored.  The caller is responsible for allocating space and
 *  passing a valid pointer.  Initially the value is set to OSPC_REPORT_USAGE_BLOCK or
 *  OSPC_AUTH_REQUEST_BLOCK to indicate that a blocking call is in progress.  When
 *  the blocking function returns and the value is set to the actual return value.
 *
 *  NOTE: (1) The caller must not delete transaction handle or any other resources that may
 *  be used by the blocking functions while a transaction is in progress.  Failing to do 
 *  so will result in unpredictable behavior. (2) The caller must not attempt to call
 *  OSPPTransactionGetFirstDestination while OSPPTransactionRequestAuthorisation is still in
 *  progress.
 */

#ifndef _NON_BLOCKING_H
#define _NON_BLOCKING_H

#include "osp.h"
#include "syncque.h"



/*-------------------------------------------*/
/* non-blocking monitor typedef              */
/*-------------------------------------------*/ 

#define MAX_WORK_THREAD_NUM 100

typedef struct _NBMONITOR
{
  SYNCQUEUE     *SyncQue;
  OSPTTHREADID  WorkThreadIDs[MAX_WORK_THREAD_NUM];
  unsigned      TimeToShutDown;
  OSPTMUTEX     Mutex; 
  OSPTCONDVAR   CondVarNoActiveWorkThreads; 
  unsigned      NumberOfWorkThreads;
  unsigned      NumberOfActiveWorkThreads;
  /* apply only to Auth Requests */
  unsigned      MaxQueSize;
  unsigned      MaxQueWaitMS;

} NBMONITOR;


#ifdef __cplusplus
extern "C" 
{
#endif
    int NonBlockingQueueMonitorNew(
            NBMONITOR       **nbMonitor,                  /* In\Out - NBMonitor Pointer */
            unsigned        NumberOfWorkThreads,          /* Max number of concurent tool kit requets */
            unsigned        MaxQueSize,                   /* Apply only to AuthReq, limit number of requets in the queue */
            unsigned        MaxQueWaitMS);                /* Apply only to AuthReq, queue expiration time in ms */

    int NonBlockingQueueMonitorBlockWhileQueueNotEmpty(
            NBMONITOR       *nbMonitor);                  /* In     - NBMonitor Pointer */

    int NonBlockingQueueMonitorDelete(
            NBMONITOR       **nbMonitor);                 /* In\Out - NBMonitor Pointer */

    int OSPPTransactionRequestAuthorisation_nb(
            NBMONITOR       *nbMonitor,                   /* In - NBMonitor Pointer   */
            int             ShouldBlock,                  /* In - 1 WILL block, 0 - will NOT block */
            int             *OSPErrorCode,                /* Out- Error code returned by the blocking function */
            OSPTTRANHANDLE  ospvTransaction,              /* In - Transaction Handle  */
            const char      *ospvSource,                  /* In - Source of call      */
            const char      *ospvSourceDevice,            /* In - SourceDevice of call*/
            const char      *ospvCallingNumber,           /* In - Calling number      */
            const char      *ospvCalledNumber,            /* In - Called number       */
            const char      *ospvUser,                    /* In - End user (optional) */
            unsigned        ospvNumberOfCallIds,          /* In - Number of call identifiers */
            OSPTCALLID      *ospvCallIds[],               /* In - List of call identifiers */
            const char      *ospvPreferredDestinations[], /* In - List of preferred destinations for call */
            unsigned        *ospvNumberOfDestinations,    /* In\Out - Max number of destinations \ Actual number of dests authorised */
            unsigned        *ospvSizeOfDetailLog,         /* In\Out - Max size of detail log \ Actual size of detail log */
            void            *ospvDetailLog);              /* In\Out - Location of detail log storage */

    int OSPPTransactionReportUsage_nb(
            NBMONITOR       *nbMonitor,                   /* In - NBMonitor Pointer   */
            int             ShouldBlock,                  /* In - 1 WILL block, 0 - will NOT block */
            int             *OSPErrorCode,                /* Out- Error code returned by the blocking function */
            OSPTTRANHANDLE  ospvTransaction,              /* In - Transaction handle */
            unsigned        ospvDuration,                 /* In - Length of call */
            OSPTTIME        ospvStartTime,                /* In - StartTime of call */
            unsigned        ospvLossPacketsSent,          /* In - Packets not received by peer */
            signed          ospvLossFractionSent,         /* In - Fraction of packets not received by peer */
            unsigned        ospvLossPacketsReceived,      /* In - Packets not received that were expected */
            signed          ospvLossFractionReceived,     /* In - Fraction of packets expected but not received */
            unsigned        *ospvSizeOfDetailLog,         /* In/Out - Max size of detail log \ Actual size of detail log */
            void            *ospvDetailLog);              /* Out - Pointer to detail log storage */

    int OSPPTransactionIndicateCapabilities_nb(
            NBMONITOR       *nbMonitor,                   /* In - NBMonitor Pointer   */
            int             ShouldBlock,                  /* In - 1 WILL block, 0 - will NOT block */
            int             *OSPErrorCode,                /* Out- Error code returned by the blocking function */
            OSPTTRANHANDLE  ospvTransaction,              /* In - Transaction Handle  */
            const char      *ospvSource,                  /* In - Source of call      */
            const char      *ospvSourceDevice,            /* In - SourceDevice of call*/
            unsigned         ospvAlmostOutOfResource,     /* In - A Boolean flag indicating device's availability */
            unsigned        *ospvSizeOfDetailLog,         /* In\Out - Max size of detail log \ Actual size of detail log */
            void            *ospvDetailLog);              /* In\Out - Location of detail log storage */

#ifdef __cplusplus
}
#endif

#endif
















