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
 * nonblocking.c - NonBlockingMonitor object funcions.
 */

#include "nonblocking.h"
#include "osptrans.h"

OSPTTHREADRETURN WorkThread(void *);
void      NonBlockingQueueMonitorIncrementActiveWorkThreadCounter(NBMONITOR *);
void      NonBlockingQueueMonitorDecrementActiveWorkThreadCounter(NBMONITOR *);
unsigned  NonBlockingQueueMonitorGetActiveWorkThreadCounter(NBMONITOR *);

typedef struct _NBAUTHREQ
{
  OSPTTRANHANDLE  ospvTransaction;              /* In - Transaction Handle  */
  const char      *ospvSource;                  /* In - Source of call      */
  const char      *ospvSourceDevice;            /* In - SourceDevice of call*/
  const char      *ospvCallingNumber;           /* In - Calling number      */
  const char      *ospvCalledNumber;            /* In - Called number       */
  const char      *ospvUser;                    /* In - End user (optional) */
  unsigned        ospvNumberOfCallIds;          /* In - Number of call identifiers */
  OSPTCALLID      **ospvCallIds;                /* In - List of call identifiers */
  const char      **ospvPreferredDestinations;  /* In - List of preferred destinations for call */
  unsigned        *ospvNumberOfDestinations;    /* In\Out - Max number of destinations \ Actual number of dests authorised */
  unsigned        *ospvSizeOfDetailLog;         /* In\Out - Max size of detail log \ Actual size of detail log */
  void            *ospvDetailLog;               /* In\Out - Location of detail log storage */
} NBAUTHREQ;


typedef struct _NBUSEIND
{
  OSPTTRANHANDLE  ospvTransaction;              /* In - Transaction handle */
  unsigned        ospvDuration;                 /* In - Length of call */
  unsigned        ospvLossPacketsSent;          /* In - Packets not received by peer */
  signed          ospvLossFractionSent;         /* In - Fraction of packets not received by peer */
  unsigned        ospvLossPacketsReceived;      /* In - Packets not received that were expected */
  signed          ospvLossFractionReceived;     /* In - Fraction of packets expected but not received */
  unsigned        *ospvSizeOfDetailLog;         /* In\Out - Max size of detail log \ Actual size of detail log */
  void            *ospvDetailLog;               /* In\Out - Location of detail log storage */
} NBUSEIND;




typedef union _OSPMESSAGE
{
  NBAUTHREQ       AuthReq;
  NBUSEIND        UseInd;
} OSPMESSAGE;






#define MESSAGE_TYPE_INVALID        0
#define MESSAGE_TYPE_AUTH_REQ       1
#define MESSAGE_TYPE_USE_IND        2
#define MESSAGE_TYPE_TIME_TO_EXIT   3

typedef struct _NBDATA
{
  OSPTLISTLINK    NextItem;
  char            bla_1[1000];
  int             MessageType;
  int             *ErrorCode;
  union
  {
    NBAUTHREQ       AuthReq;
    NBUSEIND        UseInd;
  } Message;
  char            bla_2[1000];
} NBDATA;


int
NonBlockingQueueMonitorNew(
      NBMONITOR       **nbMonitor,
      unsigned        NumberOfWorkThreads)
{

  					int errorcode = OSPC_ERR_NO_ERROR;
	unsigned	int i 				= 0;

  //  check input
  if( (nbMonitor == OSPC_OSNULL) || (NumberOfWorkThreads > MAX_WORK_THREAD_NUM) )
  {
      errorcode = OSPC_ERR_UTIL_INVALID_ARG;
  }

  //  allocate space
  if( errorcode == OSPC_ERR_NO_ERROR )
  {
    OSPM_MALLOC(*nbMonitor, NBMONITOR, sizeof(NBMONITOR));

    if( *nbMonitor != (NBMONITOR *)OSPC_OSNULL ) 
    {
        //  Zero out newly allocated space
        OSPM_MEMSET(*nbMonitor, 0, sizeof(NBMONITOR));

        //  Init memeber variables
        (*nbMonitor)->NumberOfWorkThreads       = NumberOfWorkThreads;
        (*nbMonitor)->NumberOfActiveWorkThreads = 0;
        (*nbMonitor)->TimeToShutDown            = 0;  

        //  Init mutex variable
        OSPM_MUTEX_INIT((*nbMonitor)->Mutex, OSPC_OSNULL, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

        //  Init conditional variable
        OSPM_CONDVAR_INIT((*nbMonitor)->CondVarNoActiveWorkThreads, OSPC_OSNULL, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);
    }
    else
    {
       errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
    }
  }

  //  start sync queue
  if( errorcode == OSPC_ERR_NO_ERROR )
  {
    errorcode = SyncQueueNew( &((*nbMonitor)->SyncQue) );
  }

  //  start work threads queue
  if( errorcode == OSPC_ERR_NO_ERROR )
  {
    for(i = 0; i < (*nbMonitor)->NumberOfWorkThreads; i++)
    {
      OSPM_CREATE_THREAD((*nbMonitor)->WorkThreadIDs[i],
                         NULL,
                         WorkThread,
                         (*nbMonitor),
                         errorcode);
      
      if( errorcode != OSPC_ERR_NO_ERROR )
      {
        break;
      }
    }
  }


  return errorcode;
}





int
NonBlockingQueueMonitorDelete(
            NBMONITOR       **nbMonitor)
{

  					int errorcode = OSPC_ERR_NO_ERROR;
  unsigned	int i         = 0;

  //  check input
  if( (nbMonitor == OSPC_OSNULL) || (*nbMonitor == OSPC_OSNULL) )
  {
      errorcode = OSPC_ERR_UTIL_INVALID_ARG;
  }

  //  Signal that no more requests should be processed
  (*nbMonitor)->TimeToShutDown  = 1;

  //  Wait for all current transactions to stop
  errorcode = NonBlockingQueueMonitorBlockWhileQueueNotEmpty( *nbMonitor );

  if( errorcode == OSPC_ERR_NO_ERROR )
  {
    //  For every work thread / put TIME_TO_EXIT message
    for(i=0; i < (*nbMonitor)->NumberOfWorkThreads; i++)
    {
      NBDATA* nbData    = OSPC_OSNULL;

      OSPM_MALLOC(nbData, NBDATA, sizeof(NBDATA));

      if( nbData != (NBDATA *)OSPC_OSNULL ) 
      {
        OSPM_MEMSET(nbData, 0, sizeof(NBDATA));

        // fill out the data structure
        nbData->MessageType = MESSAGE_TYPE_TIME_TO_EXIT;
        errorcode = SyncQueueAddTransaction( (*nbMonitor)->SyncQue,nbData);
      }
      else
      {
        errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
      }

      // break on any ERROR
      if( errorcode != OSPC_ERR_NO_ERROR )
      {
        break;
      }
    }
  }

  // delete the queu
  if( errorcode == OSPC_ERR_NO_ERROR )
  {
    NonBlockingQueueMonitorBlockWhileQueueNotEmpty( *nbMonitor );
    SyncQueueDelete( &((*nbMonitor)->SyncQue) );
  }

  // release memory
  OSPM_FREE( (*nbMonitor) );
  *nbMonitor = OSPC_OSNULL;



  return errorcode;
}











void
NonBlockingQueueMonitorIncrementActiveWorkThreadCounter(
                    NBMONITOR       *nbMonitor)
{
  unsigned errorcode = OSPC_ERR_NO_ERROR;

  OSPM_MUTEX_LOCK(nbMonitor->Mutex, errorcode);
  assert(errorcode == OSPC_ERR_NO_ERROR);

  nbMonitor->NumberOfActiveWorkThreads++;

  OSPM_MUTEX_UNLOCK(nbMonitor->Mutex, errorcode);
  assert(errorcode == OSPC_ERR_NO_ERROR);
}







void
NonBlockingQueueMonitorDecrementActiveWorkThreadCounter(
                    NBMONITOR       *nbMonitor)
{
  unsigned errorcode = OSPC_ERR_NO_ERROR;

  OSPM_MUTEX_LOCK(nbMonitor->Mutex, errorcode);
  assert(errorcode == OSPC_ERR_NO_ERROR);

  nbMonitor->NumberOfActiveWorkThreads--;


  // there are no more active work threads
  if( nbMonitor->NumberOfActiveWorkThreads == 0 )
  {
    OSPM_CONDVAR_SIGNAL(nbMonitor->CondVarNoActiveWorkThreads, errorcode);
    assert(errorcode == OSPC_ERR_NO_ERROR);
  }

  OSPM_MUTEX_UNLOCK(nbMonitor->Mutex, errorcode);
  assert(errorcode == OSPC_ERR_NO_ERROR);
}







unsigned
NonBlockingQueueMonitorGetActiveWorkThreadCounter(
                    NBMONITOR       *nbMonitor)
{
  return nbMonitor->NumberOfActiveWorkThreads;
}







int 
NonBlockingQueueMonitorBlockWhileQueueNotEmpty(
                    NBMONITOR       *nbMonitor)
{
  unsigned errorcode = OSPC_ERR_NO_ERROR;

  errorcode = SyncQueueBlockWhileNotEmpty(nbMonitor->SyncQue);

  if( errorcode == OSPC_ERR_NO_ERROR )
  {
    OSPM_MUTEX_LOCK(nbMonitor->Mutex, errorcode);
    assert(errorcode == OSPC_ERR_NO_ERROR);

    while( NonBlockingQueueMonitorGetActiveWorkThreadCounter(nbMonitor) > 0 )
    {
      OSPM_CONDVAR_WAIT(nbMonitor->CondVarNoActiveWorkThreads, nbMonitor->Mutex, errorcode);
      assert(errorcode == OSPC_ERR_NO_ERROR);
    }

    OSPM_MUTEX_UNLOCK(nbMonitor->Mutex, errorcode);
    assert(errorcode == OSPC_ERR_NO_ERROR);
  }

  return errorcode;
}













OSPTTHREADRETURN
WorkThread(void *arg)
{
  NBMONITOR *nbMonitor    = (NBMONITOR *)arg;
  NBDATA    *transaction  = NULL;
  int       TimeToExit    = 0;

  while( TimeToExit == 0 )
  {
    /*
     * block waiting for a message to process
    */
    if( OSPC_ERR_NO_ERROR == SyncQueueRemoveTransaction(nbMonitor->SyncQue, (void **)&transaction) )
    {
        NonBlockingQueueMonitorIncrementActiveWorkThreadCounter(nbMonitor);

        /*
         * What kind of message is it? (AuthReq or UseInd)
        */
        switch(transaction->MessageType)
        {
          case MESSAGE_TYPE_TIME_TO_EXIT:
              TimeToExit = 1;
              break;
          case MESSAGE_TYPE_AUTH_REQ:   //  AuthorisationRequest
              *(transaction->ErrorCode) = OSPPTransactionRequestAuthorisation( 
                                                        transaction->Message.AuthReq.ospvTransaction,
                                                        transaction->Message.AuthReq.ospvSource,
                                                        transaction->Message.AuthReq.ospvSourceDevice,
                                                        transaction->Message.AuthReq.ospvCallingNumber,
                                                        transaction->Message.AuthReq.ospvCalledNumber,
                                                        transaction->Message.AuthReq.ospvUser,
                                                        transaction->Message.AuthReq.ospvNumberOfCallIds,
                                                        transaction->Message.AuthReq.ospvCallIds,
                                                        transaction->Message.AuthReq.ospvPreferredDestinations,
                                                        transaction->Message.AuthReq.ospvNumberOfDestinations,
                                                        transaction->Message.AuthReq.ospvSizeOfDetailLog,
                                                        transaction->Message.AuthReq.ospvDetailLog);
              break;
          case MESSAGE_TYPE_USE_IND:    //  UseageReport
              *(transaction->ErrorCode) = OSPPTransactionReportUsage(       
                                                        transaction->Message.UseInd.ospvTransaction,
                                                        transaction->Message.UseInd.ospvDuration,
                                                        transaction->Message.UseInd.ospvLossPacketsSent,
                                                        transaction->Message.UseInd.ospvLossFractionSent,
                                                        transaction->Message.UseInd.ospvLossPacketsReceived,
                                                        transaction->Message.UseInd.ospvLossFractionReceived,
                                                        transaction->Message.UseInd.ospvSizeOfDetailLog,
                                                        transaction->Message.UseInd.ospvDetailLog);
                break;
          default:
              break;
        }

        OSPM_FREE(transaction);
        transaction = OSPC_OSNULL;

        NonBlockingQueueMonitorDecrementActiveWorkThreadCounter(nbMonitor);
    }
    else
    {
      /*
       * Error removing transaction for the queue
      */
    }
  }

  OSPTTHREADRETURN_NULL();
}







int
OSPPTransactionRequestAuthorisation_nb(
        NBMONITOR       *nbMonitor,                   /* In - NBMonitor Pointer   */
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
        void            *ospvDetailLog)               /* In\Out - Location of detail log storage */
{
  int     errorcode = OSPC_ERR_NO_ERROR;
  NBDATA* nbData    = OSPC_OSNULL;

  if( nbMonitor->TimeToShutDown == 0 )
  {
    /*
     * allocate space
     * the space will be deallocated by one of the working thread
    */
    OSPM_MALLOC(nbData, NBDATA, sizeof(NBDATA));

    if( nbData != (NBDATA *)OSPC_OSNULL ) 
    {
      OSPM_MEMSET(nbData, 0, sizeof(NBDATA));

      // fill out the data structure
      nbData->MessageType                                 = MESSAGE_TYPE_AUTH_REQ;
      nbData->ErrorCode                                   = OSPErrorCode;
      nbData->Message.AuthReq.ospvTransaction             = ospvTransaction;
      nbData->Message.AuthReq.ospvSource                  = ospvSource;
      nbData->Message.AuthReq.ospvSourceDevice            = ospvSourceDevice;
      nbData->Message.AuthReq.ospvCallingNumber           = ospvCallingNumber;
      nbData->Message.AuthReq.ospvCalledNumber            = ospvCalledNumber;
      nbData->Message.AuthReq.ospvUser                    = ospvUser;
      nbData->Message.AuthReq.ospvNumberOfCallIds         = ospvNumberOfCallIds;
      nbData->Message.AuthReq.ospvCallIds                 = ospvCallIds;
      nbData->Message.AuthReq.ospvPreferredDestinations   = ospvPreferredDestinations;
      nbData->Message.AuthReq.ospvNumberOfDestinations    = ospvNumberOfDestinations;
      nbData->Message.AuthReq.ospvSizeOfDetailLog         = ospvSizeOfDetailLog;
      nbData->Message.AuthReq.ospvDetailLog               = ospvDetailLog;

      // put a new message to the non-blocking queue
      *(nbData->ErrorCode)  = OSPC_AUTH_REQUEST_BLOCK;
      errorcode             = SyncQueueAddTransaction(nbMonitor->SyncQue,nbData);
    }
    else
    {
      errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
    }
  }
  else
  {
    errorcode = -1;
  }

  return errorcode;
}








int OSPPTransactionReportUsage_nb(
        NBMONITOR       *nbMonitor,                   /* In - NBMonitor Pointer   */
        int             *OSPErrorCode,                /* Out- Error code returned by the blocking function */
        OSPTTRANHANDLE  ospvTransaction,              /* In - Transaction handle */
        unsigned        ospvDuration,                 /* In - Length of call */
        unsigned        ospvLossPacketsSent,          /* In - Packets not received by peer */
        signed          ospvLossFractionSent,         /* In - Fraction of packets not received by peer */
        unsigned        ospvLossPacketsReceived,      /* In - Packets not received that were expected */
        signed          ospvLossFractionReceived,     /* In - Fraction of packets expected but not received */
        unsigned        *ospvSizeOfDetailLog,         /* In/Out - Max size of detail log \ Actual size of detail log */
        void            *ospvDetailLog)               /* Out - Pointer to detail log storage */
{
  int     errorcode = OSPC_ERR_NO_ERROR;
  NBDATA* nbData    = OSPC_OSNULL;

  if( nbMonitor->TimeToShutDown == 0 )
  {
    /*
     * allocate space
     * the space will be deallocated by one of the working thread
    */
    OSPM_MALLOC(nbData, NBDATA, sizeof(NBDATA));

    if( nbData != (NBDATA *)OSPC_OSNULL ) 
    {
      OSPM_MEMSET(nbData, 0, sizeof(NBDATA));

      // fill out the data structure
      nbData->MessageType                                 = MESSAGE_TYPE_USE_IND;
      nbData->ErrorCode                                   = OSPErrorCode;
      nbData->Message.UseInd.ospvTransaction              = ospvTransaction;
      nbData->Message.UseInd.ospvDuration                 = ospvDuration;
      nbData->Message.UseInd.ospvLossPacketsSent          = ospvLossPacketsSent;
      nbData->Message.UseInd.ospvLossFractionSent         = ospvLossFractionSent;
      nbData->Message.UseInd.ospvLossPacketsReceived      = ospvLossPacketsReceived;
      nbData->Message.UseInd.ospvLossFractionReceived     = ospvLossFractionReceived;
      nbData->Message.UseInd.ospvSizeOfDetailLog          = ospvSizeOfDetailLog;
      nbData->Message.UseInd.ospvDetailLog                = ospvDetailLog;

      // put a new message to the non-blocking queue
      *(nbData->ErrorCode)  = OSPC_REPORT_USAGE_BLOCK;
      errorcode             = SyncQueueAddTransaction(nbMonitor->SyncQue,nbData);
    }
    else
    {
      errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
    }
  }
  else
  {
    errorcode = -1;
  }

  return errorcode;
}

