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
#include "osp/osptrans.h"

OSPTTHREADRETURN WorkThread(void *);
void NonBlockingQueueMonitorIncrementActiveWorkThreadCounter(NBMONITOR *);
void NonBlockingQueueMonitorDecrementActiveWorkThreadCounter(NBMONITOR *);
unsigned NonBlockingQueueMonitorGetActiveWorkThreadCounter(NBMONITOR *);

OSPTUINT64 GetDeltaMS(OSPTTIME FromTimeSec, unsigned int FromTimeMS, OSPTTIME ToTimeSec, unsigned int ToTimeMS);

typedef struct _NBAUTHREQ {
    OSPTTRANHANDLE ospvTransaction;             /* In - Transaction Handle  */
    const char *ospvSource;                     /* In - Source of call      */
    const char *ospvSourceDevice;               /* In - SourceDevice of call */
    const char *ospvCallingNumber;              /* In - Calling number      */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat; /* In - Calling number Format */
    const char *ospvCalledNumber;               /* In - Called number       */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat;  /* In - Called number Format */
    const char *ospvUser;                       /* In - End user (optional) */
    unsigned ospvNumberOfCallIds;               /* In - Number of call identifiers */
    OSPT_CALL_ID **ospvCallIds;                 /* In - List of call identifiers */
    const char **ospvPreferredDestinations;     /* In - List of preferred destinations for call */
    unsigned *ospvNumberOfDestinations;         /* In\Out - Max number of destinations \ Actual number of dests authorised */
    unsigned *ospvSizeOfDetailLog;              /* In\Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog;                        /* In\Out - Location of detail log storage */
} NBAUTHREQ;

typedef struct _NBUSEIND {
    OSPTTRANHANDLE ospvTransaction;             /* In - Transaction handle */
    unsigned ospvDuration;                      /* In - Length of call */
    OSPTTIME ospvStartTime;                     /* In - StartTime of call */
    OSPTTIME ospvEndTime;                       /* In - EndTime of call */
    OSPTTIME ospvAlertTime;                     /* In - AlertTime of call */
    OSPTTIME ospvConnectTime;                   /* In - ConnectTime of call */
    OSPTBOOL ospvHasPDDInfo;                    /* In - Is PDD info present */
    unsigned ospvPostDialDelay;                 /* In - PDD in milliseconds */
    OSPE_RELEASE ospvReleaseSource;             /* In - Rel Src */
    char ospvConferenceId[OSPC_SIZE_CONFID];    /* In - ConferenceId */
    int ospvLossPacketsSent;                    /* In - Packets not received by peer */
    int ospvLossFractionSent;                   /* In - Fraction of packets not received by peer */
    int ospvLossPacketsReceived;                /* In - Packets not received that were expected */
    int ospvLossFractionReceived;               /* In - Fraction of packets expected but not received */
    unsigned *ospvSizeOfDetailLog;              /* In\Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog;                        /* In\Out - Location of detail log storage */
} NBUSEIND;

typedef struct _NBCAPIND {
    OSPTTRANHANDLE ospvTransaction;     /* In - Transaction Handle  */
    const char *ospvSource;             /* In - Source of call      */
    const char *ospvSourceDevice;       /* In - SourceDevice of call */
    const char *ospvSourceNetworkId;    /* In - NetworkId of call */
    unsigned ospvAlmostOutOfResources;  /* In - A Boolean flag indicating device's availability */
    unsigned *ospvSizeOfDetailLog;      /* In\Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog;                /* In\Out - Location of detail log storage */
} NBCAPIND;

typedef struct _NBVALIDATEAUTH {
    OSPTTRANHANDLE ospvTransaction;             /* In - Transaction Handle  */
    const char *ospvSource;                     /* In - Source of call      */
    const char *ospvDestination;                /* In - Dest of call      */
    const char *ospvSourceDevice;               /* In - SourceDevice of call */
    const char *ospvDestinationDevice;          /* In - DestDevice of call      */
    const char *ospvCallingNumber;              /* In - Calling number      */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat; /* In - Calling number Format */
    const char *ospvCalledNumber;               /* In - Called number       */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat;  /* In - Called number Format */
    unsigned ospvSizeOfCallId;                  /* In - Size of call id value */
    const void *ospvCallId;                     /* In - Call Id for this call */
    unsigned ospvSizeOfToken;                   /* In - Size of authorization token */
    const void *ospvToken;                      /* In - Authorization token */
    unsigned *ospvAuthorised;                   /* Out - Call authorization indicator */
    unsigned *ospvTimeLimit;                    /* Out - Number of seconds call is authorized for */
    unsigned *ospvSizeOfDetailLog;              /* In\Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog;                        /* In\Out - Location of detail log storage */
    unsigned ospvTokenAlgo;                     /* In - Algorithm to be used for Validating Token */
} NBVALIDATEAUTH;

typedef union _OSPMESSAGE {
    NBAUTHREQ AuthReq;
    NBUSEIND UseInd;
    NBCAPIND CapInd;
    NBVALIDATEAUTH ValidateAuth;
} OSPMESSAGE;

#define MESSAGE_TYPE_INVALID        0
#define MESSAGE_TYPE_AUTH_REQ       1
#define MESSAGE_TYPE_USE_IND        2
#define MESSAGE_TYPE_CAP_IND        3
#define MESSAGE_TYPE_VALIDATE_AUTH  4
#define MESSAGE_TYPE_TIME_TO_EXIT   10

typedef struct _NBDATA {
    OSPTLISTLINK NextItem;
    int MessageType;
    int *ErrorCode;
    union {
        NBAUTHREQ AuthReq;
        NBUSEIND UseInd;
        NBCAPIND CapInd;
        NBVALIDATEAUTH ValidateAuth;
    } Message;
    int ShouldBlock;            /* Ussed to indicate if the caller is blocked */
    OSPTMUTEX Mutex;            /* Used to wake up blocked call */
    OSPTCONDVAR CondVar;        /* Used to wake up blocked call */

    /* Stats */
    OSPTTIME InQuequeTime;
    unsigned int InQuequeTimeMS;
    OSPTTIME OutQuequeTime;
    unsigned int OutQuequeTimeMS;
    OSPTTIME OutToolKitTime;
    unsigned int OutToolKitTimeMS;
} NBDATA;

int newNBDATA(NBDATA **nbData, int MessageType);
int deleteNBDATA(NBDATA *nbData);

/*
 * Allocate memory
 * Set MessageType
 * Init Mutex and CondVar
 */
int newNBDATA(NBDATA **nbData, int MessageType)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC((*nbData), NBDATA, sizeof(NBDATA));

    if ((*nbData) != OSPC_OSNULL) {
        OSPM_MEMSET((*nbData), 0, sizeof(NBDATA));

        (*nbData)->MessageType = MessageType;

        OSPM_MUTEX_INIT((*nbData)->Mutex, OSPC_OSNULL, errorcode);

        if (OSPC_ERR_NO_ERROR == errorcode) {
            OSPM_CONDVAR_INIT((*nbData)->CondVar, OSPC_OSNULL, errorcode);
        }
    } else {
        errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
    }

    return errorcode;
}

/*
 * Destroy Mutex and CondVar
 * De-allocate memory
 */
int deleteNBDATA(NBDATA * nbData)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    //  check input
    if (nbData == OSPC_OSNULL) {
        errorcode = OSPC_ERR_UTIL_INVALID_ARG;
    } else {
        OSPM_MUTEX_DESTROY(nbData->Mutex, errorcode);
        OSPM_CONDVAR_DESTROY(nbData->CondVar, errorcode);
        OSPM_FREE(nbData);
    }

    return errorcode;
}

int NonBlockingQueueMonitorNew(NBMONITOR **nbMonitor,
    unsigned NumberOfWorkThreads,
    unsigned MaxQueSize,
    unsigned MaxQueWaitMS)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned int i = 0;

    //  check input
    if ((nbMonitor == OSPC_OSNULL) ||
        (NumberOfWorkThreads > MAX_WORK_THREAD_NUM)) {
        errorcode = OSPC_ERR_UTIL_INVALID_ARG;
    }
    //  allocate space
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MALLOC(*nbMonitor, NBMONITOR, sizeof(NBMONITOR));

        if (*nbMonitor != OSPC_OSNULL) {
            //  Zero out newly allocated space
            OSPM_MEMSET(*nbMonitor, 0, sizeof(NBMONITOR));

            //  Init memeber variables
            (*nbMonitor)->NumberOfWorkThreads = NumberOfWorkThreads;
            (*nbMonitor)->MaxQueSize = MaxQueSize;
            (*nbMonitor)->MaxQueWaitMS = MaxQueWaitMS;
            (*nbMonitor)->NumberOfActiveWorkThreads = 0;
            (*nbMonitor)->TimeToShutDown = 0;

            //  Init mutex variable
            OSPM_MUTEX_INIT((*nbMonitor)->Mutex, OSPC_OSNULL, errorcode);
            assert(errorcode == OSPC_ERR_NO_ERROR);

            //  Init conditional variable
            OSPM_CONDVAR_INIT((*nbMonitor)->CondVarNoActiveWorkThreads, OSPC_OSNULL, errorcode);
            assert(errorcode == OSPC_ERR_NO_ERROR);
        } else {
            errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
        }
    }
    //  start sync queue
    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = SyncQueueNew(&((*nbMonitor)->SyncQue));
    }
    //  start work threads queue
    if (errorcode == OSPC_ERR_NO_ERROR) {
        for (i = 0; i < (*nbMonitor)->NumberOfWorkThreads; i++) {
            OSPM_CREATE_THREAD((*nbMonitor)->WorkThreadIDs[i], NULL, WorkThread, (*nbMonitor), errorcode);

            if (errorcode != OSPC_ERR_NO_ERROR) {
                break;
            }
        }
    }

    return errorcode;
}

int NonBlockingQueueMonitorDelete(NBMONITOR **nbMonitor)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned int i = 0;

    //  check input
    if ((nbMonitor == OSPC_OSNULL) || (*nbMonitor == OSPC_OSNULL)) {
        errorcode = OSPC_ERR_UTIL_INVALID_ARG;
    }
    //  Signal that no more requests should be processed
    (*nbMonitor)->TimeToShutDown = 1;

    //  Wait for all current transactions to stop
    errorcode = NonBlockingQueueMonitorBlockWhileQueueNotEmpty(*nbMonitor);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        //  For every work thread / put TIME_TO_EXIT message
        for (i = 0; i < (*nbMonitor)->NumberOfWorkThreads; i++) {
            NBDATA *nbData = OSPC_OSNULL;

            errorcode = newNBDATA(&nbData, MESSAGE_TYPE_TIME_TO_EXIT);

            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode =
                    SyncQueueAddTransaction((*nbMonitor)->SyncQue, nbData);
            } else {
                errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
            }

            // break on any ERROR
            if (errorcode != OSPC_ERR_NO_ERROR) {
                break;
            }
        }
    }
    // delete the queu
    if (errorcode == OSPC_ERR_NO_ERROR) {
        NonBlockingQueueMonitorBlockWhileQueueNotEmpty(*nbMonitor);
        SyncQueueDelete(&((*nbMonitor)->SyncQue));
        for (i = 0; i < (*nbMonitor)->NumberOfWorkThreads; i++) {
            OSPM_THR_JOIN((*nbMonitor)->WorkThreadIDs[i], NULL);
        }
    }
    // release memory
    OSPM_FREE((*nbMonitor));
    *nbMonitor = OSPC_OSNULL;

    return errorcode;
}

void NonBlockingQueueMonitorIncrementActiveWorkThreadCounter(NBMONITOR *nbMonitor)
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    OSPM_MUTEX_LOCK(nbMonitor->Mutex, errcode);
    assert(errcode == OSPC_ERR_NO_ERROR);

    nbMonitor->NumberOfActiveWorkThreads++;

    OSPM_MUTEX_UNLOCK(nbMonitor->Mutex, errcode);
    assert(errcode == OSPC_ERR_NO_ERROR);
}

void NonBlockingQueueMonitorDecrementActiveWorkThreadCounter(NBMONITOR *nbMonitor)
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    OSPM_MUTEX_LOCK(nbMonitor->Mutex, errcode);
    assert(errcode == OSPC_ERR_NO_ERROR);

    nbMonitor->NumberOfActiveWorkThreads--;

    // there are no more active work threads
    if (nbMonitor->NumberOfActiveWorkThreads == 0) {
        OSPM_CONDVAR_SIGNAL(nbMonitor->CondVarNoActiveWorkThreads, errcode);
        assert(errcode == OSPC_ERR_NO_ERROR);
    }

    OSPM_MUTEX_UNLOCK(nbMonitor->Mutex, errcode);
    assert(errcode == OSPC_ERR_NO_ERROR);
}

unsigned NonBlockingQueueMonitorGetActiveWorkThreadCounter(NBMONITOR *nbMonitor)
{
    return nbMonitor->NumberOfActiveWorkThreads;
}

int NonBlockingQueueMonitorBlockWhileQueueNotEmpty(NBMONITOR *nbMonitor)
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    errcode = SyncQueueBlockWhileNotEmpty(nbMonitor->SyncQue);

    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPM_MUTEX_LOCK(nbMonitor->Mutex, errcode);
        assert(errcode == OSPC_ERR_NO_ERROR);

        while (NonBlockingQueueMonitorGetActiveWorkThreadCounter(nbMonitor) >
               0) {
            OSPM_CONDVAR_WAIT(nbMonitor->CondVarNoActiveWorkThreads,
                              nbMonitor->Mutex, errcode);
            assert(errcode == OSPC_ERR_NO_ERROR);
        }

        OSPM_MUTEX_UNLOCK(nbMonitor->Mutex, errcode);
        assert(errcode == OSPC_ERR_NO_ERROR);
    }

    return errcode;
}

OSPTTHREADRETURN WorkThread(void *arg)
{
    NBMONITOR *nbMonitor = (NBMONITOR *)arg;
    NBDATA *transaction = NULL;
    int TimeToExit = 0;
    int errcode = 0;
    OSPTUINT64 QTime = 0;
    OSPTUINT64 TKTime = 0;

    while (TimeToExit == 0) {
        /*
         * block waiting for a message to process
         */
        if (OSPC_ERR_NO_ERROR == SyncQueueRemoveTransaction(nbMonitor->SyncQue, (void **)&transaction)) {
            NonBlockingQueueMonitorIncrementActiveWorkThreadCounter(nbMonitor);

            // Time stamp OUT QUEUE
            OSPPOSTimeGetTime(&transaction->OutQuequeTime, &transaction->OutQuequeTimeMS);

            // Calculate Queue time
            QTime = GetDeltaMS(transaction->InQuequeTime,
                transaction->InQuequeTimeMS,
                transaction->OutQuequeTime,
                transaction->OutQuequeTimeMS);

            /*
             * What kind of message is it? (AuthReq or UseInd)
             */
            switch (transaction->MessageType) {
            case MESSAGE_TYPE_TIME_TO_EXIT:
                TimeToExit = 1;
                break;
            case MESSAGE_TYPE_AUTH_REQ:    //  AuthorisationRequest
                /*
                 * Make sure that the request has not expired
                 */
                if (QTime < nbMonitor->MaxQueWaitMS) {
                    *(transaction->ErrorCode) = OSPPTransactionRequestAuthorisation(transaction->Message.AuthReq.ospvTransaction,
                        transaction->Message.AuthReq.ospvSource,
                        transaction->Message.AuthReq.ospvSourceDevice,
                        transaction->Message.AuthReq.ospvCallingNumber,
                        transaction->Message.AuthReq.ospvCallingNumberFormat,
                        transaction->Message.AuthReq.ospvCalledNumber,
                        transaction->Message.AuthReq.ospvCalledNumberFormat,
                        transaction->Message.AuthReq.ospvUser,
                        transaction->Message.AuthReq.ospvNumberOfCallIds,
                        transaction->Message.AuthReq.ospvCallIds,
                        transaction->Message.AuthReq.ospvPreferredDestinations,
                        transaction->Message.AuthReq.ospvNumberOfDestinations,
                        transaction->Message.AuthReq.ospvSizeOfDetailLog,
                        transaction->Message.AuthReq.ospvDetailLog);
                } else {
                    /*
                     * AuthRequest has expired
                     */
                    OSPM_DBGPRINTF("AUTH REQ EXPIRED\n");
                    *(transaction->ErrorCode) = -1;
                }
                break;
            case MESSAGE_TYPE_USE_IND:    //  UseageReport
                *(transaction->ErrorCode) = OSPPTransactionReportUsage(transaction->Message.UseInd.ospvTransaction,
                    transaction->Message.UseInd.ospvDuration,
                    transaction->Message.UseInd.ospvStartTime,
                    transaction->Message.UseInd.ospvEndTime,
                    transaction->Message.UseInd.ospvAlertTime,
                    transaction->Message.UseInd.ospvConnectTime,
                    transaction->Message.UseInd.ospvHasPDDInfo,
                    transaction->Message.UseInd.ospvPostDialDelay,
                    transaction->Message.UseInd.ospvReleaseSource,
                    transaction->Message.UseInd.ospvConferenceId,
                    transaction->Message.UseInd.ospvLossPacketsSent,
                    transaction->Message.UseInd.ospvLossFractionSent,
                    transaction->Message.UseInd.ospvLossPacketsReceived,
                    transaction->Message.UseInd.ospvLossFractionReceived,
                    transaction->Message.UseInd.ospvSizeOfDetailLog,
                    transaction->Message.UseInd.ospvDetailLog);
                break;
            case MESSAGE_TYPE_CAP_IND:    //  CapabilitiesIndication
                *(transaction->ErrorCode) = OSPPTransactionIndicateCapabilities(transaction->Message.CapInd.ospvTransaction,
                    transaction->Message.CapInd.ospvSource,
                    transaction->Message.CapInd.ospvSourceDevice,
                    transaction->Message.CapInd.ospvSourceNetworkId,
                    transaction->Message.CapInd.ospvAlmostOutOfResources,
                    transaction->Message.CapInd.ospvSizeOfDetailLog,
                    transaction->Message.CapInd.ospvDetailLog);
                break;
            case MESSAGE_TYPE_VALIDATE_AUTH:    //  ValidationRequest
                /*
                 * Make sure that the request has not expired
                 */
                if (QTime < nbMonitor->MaxQueWaitMS) {
                    *(transaction->ErrorCode) = OSPPTransactionValidateAuthorisation(transaction->Message.ValidateAuth.ospvTransaction,
                        transaction->Message.ValidateAuth.ospvSource,
                        transaction->Message.ValidateAuth.ospvDestination,
                        transaction->Message.ValidateAuth.ospvSourceDevice,
                        transaction->Message.ValidateAuth.ospvDestinationDevice,
                        transaction->Message.ValidateAuth.ospvCallingNumber,
                        transaction->Message.ValidateAuth.ospvCallingNumberFormat,
                        transaction->Message.ValidateAuth.ospvCalledNumber,
                        transaction->Message.ValidateAuth.ospvCalledNumberFormat,
                        transaction->Message.ValidateAuth.ospvSizeOfCallId,
                        transaction->Message.ValidateAuth.ospvCallId,
                        transaction->Message.ValidateAuth.ospvSizeOfToken,
                        transaction->Message.ValidateAuth.ospvToken,
                        transaction->Message.ValidateAuth.ospvAuthorised,
                        transaction->Message.ValidateAuth.ospvTimeLimit,
                        transaction->Message.ValidateAuth.ospvSizeOfDetailLog,
                        transaction->Message.ValidateAuth.ospvDetailLog,
                        transaction->Message.ValidateAuth.ospvTokenAlgo);
                } else {
                    /*
                     * Validate Req has expired
                     */
                    OSPM_DBGPRINTF("VALIDATE REQ EXPIRED\n");
                    *(transaction->ErrorCode) = -1;
                }
                break;
            default:
                break;
            }

            /*
             * Log a message
             */
            if (MESSAGE_TYPE_TIME_TO_EXIT != transaction->MessageType) {
                // Time stamp OUT ToolKit
                OSPPOSTimeGetTime(&transaction->OutToolKitTime, &transaction->OutToolKitTimeMS);

                // Calculate TK time
                TKTime = GetDeltaMS(transaction->OutQuequeTime,
                    transaction->OutQuequeTimeMS,
                    transaction->OutToolKitTime,
                    transaction->OutToolKitTimeMS);

                OSPM_DBGPRINTF ("InQ[%llu:%3llu] OutQ[%llu:%3llu] OutTK[%llu:%3llu] QTime[%7llu] TKTime[%7llu] QSize[%5u] MsgType[%d]\n",
                    (OSPTUINT64) transaction->InQuequeTime,
                    (OSPTUINT64) transaction->InQuequeTimeMS,
                    (OSPTUINT64) transaction->OutQuequeTime,
                    (OSPTUINT64) transaction->OutQuequeTimeMS,
                    (OSPTUINT64) transaction->OutToolKitTime,
                    (OSPTUINT64) transaction->OutToolKitTimeMS, QTime,
                    TKTime,
                    SyncQueueGetNumberOfTransactions(nbMonitor->SyncQue),
                    transaction->MessageType);
            }

            /*
             *  This is a bit odd.  If the caller wants to wait,
             *    it is responsible for deleting nbData.
             *  Oterwise, the work thread will clean up
             */
            if (1 == transaction->ShouldBlock) {
                /*
                 *  Yes, wake it up
                 */
                OSPM_CONDVAR_SIGNAL(transaction->CondVar, errcode);
            } else {
                /*
                 *  No, we are responsible for cleaning up
                 */
                deleteNBDATA(transaction);
            }

            NonBlockingQueueMonitorDecrementActiveWorkThreadCounter(nbMonitor);
        } else {
            /*
             * Error removing transaction for the queue
             */
        }
    }

    OSPTTHREADRETURN_NULL();
}

int OSPPTransactionValidateAuthorisation_nb(NBMONITOR *nbMonitor,   /* In - NBMonitor Pointer   */
    int ShouldBlock,                                                /* In - 1 WILL block, 0 - will NOT block */
    int *OSPErrorCode,                                              /* Out- Error code returned by the blocking function */
    OSPTTRANHANDLE ospvTransaction,                                 /* In - Transaction handle */
    const char *ospvSource,                                         /* In - Source of call */
    const char *ospvDestination,                                    /* In - Destination for call */
    const char *ospvSourceDevice,                                   /* In - SourceDevice of call */
    const char *ospvDestinationDevice,                              /* In - DestinationDevice for call */
    const char *ospvCallingNumber,                                  /* In - Calling number string */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat,                     /* In - Calling number Format */
    const char *ospvCalledNumber,                                   /* In - Called number string */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat,                      /* In - Calling number Format */
    unsigned ospvSizeOfCallId,                                      /* In - Size of call id value */
    const void *ospvCallId,                                         /* In - Call Id for this call */
    unsigned ospvSizeOfToken,                                       /* In - Size of authorization token */
    const void *ospvToken,                                          /* In - Authorization token */
    unsigned *ospvAuthorised,                                       /* Out - Call authorization indicator */
    unsigned *ospvTimeLimit,                                        /* Out - Number of seconds call is authorized for */
    unsigned *ospvSizeOfDetailLog,                                  /* In\Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog,                                            /* In\Out - Location of detail log storage */
    unsigned ospvTokenAlgo)                                         /* In - Algorithm to be used for Validating Token */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    NBDATA *nbData = OSPC_OSNULL;

    if (nbMonitor->TimeToShutDown == 1) {
        /*
         * Q is shutting down
         */
        errorcode = -1;
    } else if (SyncQueueGetNumberOfTransactions(nbMonitor->SyncQue) >
               nbMonitor->MaxQueSize) {
        /*
         * There are already to many requests in the Q
         */
        errorcode = -1;
    } else {
        /*
         * allocate space
         * the space will be deallocated by one of the working thread
         */
        errorcode = newNBDATA(&nbData, MESSAGE_TYPE_VALIDATE_AUTH);

        if (OSPC_ERR_NO_ERROR == errorcode) {
            // fill out the data structure
            nbData->ShouldBlock = ShouldBlock;
            nbData->ErrorCode = OSPErrorCode;
            nbData->Message.ValidateAuth.ospvTransaction = ospvTransaction;
            nbData->Message.ValidateAuth.ospvSource = ospvSource;
            nbData->Message.ValidateAuth.ospvDestination = ospvDestination;
            nbData->Message.ValidateAuth.ospvSourceDevice = ospvSourceDevice;
            nbData->Message.ValidateAuth.ospvDestinationDevice = ospvDestinationDevice;
            nbData->Message.ValidateAuth.ospvCallingNumber = ospvCallingNumber;
            nbData->Message.ValidateAuth.ospvCallingNumberFormat = ospvCallingNumberFormat;
            nbData->Message.ValidateAuth.ospvCalledNumber = ospvCalledNumber;
            nbData->Message.ValidateAuth.ospvCalledNumberFormat = ospvCalledNumberFormat;
            nbData->Message.ValidateAuth.ospvSizeOfCallId = ospvSizeOfCallId;
            nbData->Message.ValidateAuth.ospvCallId = ospvCallId;
            nbData->Message.ValidateAuth.ospvSizeOfToken = ospvSizeOfToken;
            nbData->Message.ValidateAuth.ospvToken = ospvToken;
            nbData->Message.ValidateAuth.ospvAuthorised = ospvAuthorised;
            nbData->Message.ValidateAuth.ospvTimeLimit = ospvTimeLimit;
            nbData->Message.ValidateAuth.ospvSizeOfDetailLog = ospvSizeOfDetailLog;
            nbData->Message.ValidateAuth.ospvDetailLog = ospvDetailLog;
            nbData->Message.ValidateAuth.ospvTokenAlgo = ospvTokenAlgo;

            // time stamp (AuthReq) IN QUEUE event
            OSPPOSTimeGetTime(&nbData->InQuequeTime, &nbData->InQuequeTimeMS);

            // put a new message to the non-blocking queue
            *(nbData->ErrorCode) = OSPC_AUTH_REQUEST_BLOCK;
            errorcode = SyncQueueAddTransaction(nbMonitor->SyncQue, nbData);

            if (OSPC_ERR_NO_ERROR == errorcode) {
                /*
                 *  This is a bit odd.  If the caller wants to wait,
                 *    it is responsible for deleting nbData.
                 *  Oterwise, the work thread will clean up
                 */
                if (1 == nbData->ShouldBlock) {
                    /*
                     * SHOULD wait
                     */
                    OSPM_MUTEX_LOCK(nbData->Mutex, errorcode);
                    while ((*(nbData->ErrorCode) == OSPC_AUTH_REQUEST_BLOCK) && (OSPC_ERR_NO_ERROR == errorcode)) {
                        OSPM_CONDVAR_WAIT(nbData->CondVar, nbData->Mutex, errorcode);
                    }
                    OSPM_MUTEX_UNLOCK(nbData->Mutex, errorcode);
                    deleteNBDATA(nbData);
                } else {
                    /*
                     * should NOT wait
                     */
                }
            }
        } else {
            errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
        }
    }

    return errorcode;
}

int OSPPTransactionRequestAuthorisation_nb(NBMONITOR *nbMonitor,    /* In - NBMonitor Pointer   */
    int ShouldBlock,                                                /* In - 1 WILL block, 0 - will NOT block */
    int *OSPErrorCode,                                              /* Out- Error code returned by the blocking function */
    OSPTTRANHANDLE ospvTransaction,                                 /* In - Transaction Handle  */
    const char *ospvSource,                                         /* In - Source of call      */
    const char *ospvSourceDevice,                                   /* In - SourceDevice of call */
    const char *ospvCallingNumber,                                  /* In - Calling number      */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat,                     /* In - Calling number Format */
    const char *ospvCalledNumber,                                   /* In - Called number       */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat,                      /* In - Called number Format */
    const char *ospvUser,                                           /* In - End user (optional) */
    unsigned ospvNumberOfCallIds,                                   /* In - Number of call identifiers */
    OSPT_CALL_ID *ospvCallIds[],                                    /* In - List of call identifiers */
    const char *ospvPreferredDestinations[],                        /* In - List of preferred destinations for call */
    unsigned *ospvNumberOfDestinations,                             /* In\Out - Max number of destinations \ Actual number of dests authorised */
    unsigned *ospvSizeOfDetailLog,                                  /* In\Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog)                                            /* In\Out - Location of detail log storage */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    NBDATA *nbData = OSPC_OSNULL;

    if (nbMonitor->TimeToShutDown == 1) {
        /*
         * Q is shutting down
         */
        errorcode = -1;
    } else if (SyncQueueGetNumberOfTransactions(nbMonitor->SyncQue) > nbMonitor->MaxQueSize) {
        /*
         * There are already to many requests in the Q
         */
        errorcode = -1;
    } else {
        /*
         * allocate space
         * the space will be deallocated by one of the working thread
         */
        errorcode = newNBDATA(&nbData, MESSAGE_TYPE_AUTH_REQ);

        if (OSPC_ERR_NO_ERROR == errorcode) {
            // fill out the data structure
            nbData->ShouldBlock = ShouldBlock;
            nbData->ErrorCode = OSPErrorCode;
            nbData->Message.AuthReq.ospvTransaction = ospvTransaction;
            nbData->Message.AuthReq.ospvSource = ospvSource;
            nbData->Message.AuthReq.ospvSourceDevice = ospvSourceDevice;
            nbData->Message.AuthReq.ospvCallingNumber = ospvCallingNumber;
            nbData->Message.AuthReq.ospvCallingNumberFormat = ospvCallingNumberFormat;
            nbData->Message.AuthReq.ospvCalledNumber = ospvCalledNumber;
            nbData->Message.AuthReq.ospvCalledNumberFormat = ospvCalledNumberFormat;
            nbData->Message.AuthReq.ospvUser = ospvUser;
            nbData->Message.AuthReq.ospvNumberOfCallIds = ospvNumberOfCallIds;
            nbData->Message.AuthReq.ospvCallIds = ospvCallIds;
            nbData->Message.AuthReq.ospvPreferredDestinations = ospvPreferredDestinations;
            nbData->Message.AuthReq.ospvNumberOfDestinations = ospvNumberOfDestinations;
            nbData->Message.AuthReq.ospvSizeOfDetailLog = ospvSizeOfDetailLog;
            nbData->Message.AuthReq.ospvDetailLog = ospvDetailLog;

            // time stamp (AuthReq) IN QUEUE event
            OSPPOSTimeGetTime(&nbData->InQuequeTime, &nbData->InQuequeTimeMS);

            // put a new message to the non-blocking queue
            *(nbData->ErrorCode) = OSPC_AUTH_REQUEST_BLOCK;
            errorcode = SyncQueueAddTransaction(nbMonitor->SyncQue, nbData);

            if (OSPC_ERR_NO_ERROR == errorcode) {
                /*
                 *  This is a bit odd.  If the caller wants to wait,
                 *    it is responsible for deleting nbData.
                 *  Oterwise, the work thread will clean up
                 */
                if (1 == nbData->ShouldBlock) {
                    /*
                     * SHOULD wait
                     */
                    OSPM_MUTEX_LOCK(nbData->Mutex, errorcode);
                    while ((*(nbData->ErrorCode) == OSPC_AUTH_REQUEST_BLOCK) && (OSPC_ERR_NO_ERROR == errorcode)) {
                        OSPM_CONDVAR_WAIT(nbData->CondVar, nbData->Mutex, errorcode);
                    }
                    OSPM_MUTEX_UNLOCK(nbData->Mutex, errorcode);
                    deleteNBDATA(nbData);
                } else {
                    /*
                     * should NOT wait
                     */
                }
            }
        } else {
            errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
        }
    }

    return errorcode;
}

int OSPPTransactionReportUsage_nb(
    NBMONITOR *nbMonitor,           /* In - NBMonitor Pointer   */
    int ShouldBlock,                /* In - 1 WILL block, 0 - will NOT block */
    int *OSPErrorCode,              /* Out- Error code returned by the blocking function */
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvDuration,          /* In - Length of call in milliseconds */
    OSPTTIME ospvStartTime,         /* In - StartTime of call */
    OSPTTIME ospvEndTime,           /* In - EndTime of call */
    OSPTTIME ospvAlertTime,         /* In - AlertTime of call */
    OSPTTIME ospvConnectTime,       /* In - ConnectTime of call */
    OSPTBOOL ospvHasPDDInfo,        /* In - Is PDD info available */
    unsigned ospvPostDialDelay,     /* In - PDD in milliseconds */
    OSPE_RELEASE ospvReleaseSource, /* In - Release Src */
    const char *ospvConferenceId,   /* In - ConferenceId */
    int ospvLossPacketsSent,        /* In - Packets not received by peer */
    int ospvLossFractionSent,       /* In - Fraction of packets not received by peer */
    int ospvLossPacketsReceived,    /* In - Packets not received that were expected */
    int ospvLossFractionReceived,   /* In - Fraction of packets expected but not received */
    unsigned *ospvSizeOfDetailLog,  /* In/Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog)            /* Out - Pointer to detail log storage */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    NBDATA *nbData = OSPC_OSNULL;

    if (nbMonitor->TimeToShutDown == 0) {
        /*
         * allocate space
         * the space will be deallocated by one of the working thread
         */
        errorcode = newNBDATA(&nbData, MESSAGE_TYPE_USE_IND);

        if (OSPC_ERR_NO_ERROR == errorcode) {
            // fill out the data structure
            nbData->ShouldBlock = ShouldBlock;
            nbData->ErrorCode = OSPErrorCode;
            nbData->Message.UseInd.ospvTransaction = ospvTransaction;
            nbData->Message.UseInd.ospvDuration = ospvDuration;
            nbData->Message.UseInd.ospvStartTime = ospvStartTime;
            nbData->Message.UseInd.ospvEndTime = ospvEndTime;
            nbData->Message.UseInd.ospvAlertTime = ospvAlertTime;
            nbData->Message.UseInd.ospvConnectTime = ospvConnectTime;
            nbData->Message.UseInd.ospvHasPDDInfo = ospvHasPDDInfo;
            nbData->Message.UseInd.ospvPostDialDelay = ospvPostDialDelay;
            nbData->Message.UseInd.ospvReleaseSource = ospvReleaseSource;
            if (ospvConferenceId) {
                OSPM_STRCPY(nbData->Message.UseInd.ospvConferenceId, (const char *)ospvConferenceId);
            }
            nbData->Message.UseInd.ospvLossPacketsSent = ospvLossPacketsSent;
            nbData->Message.UseInd.ospvLossFractionSent = ospvLossFractionSent;
            nbData->Message.UseInd.ospvLossPacketsReceived = ospvLossPacketsReceived;
            nbData->Message.UseInd.ospvLossFractionReceived = ospvLossFractionReceived;
            nbData->Message.UseInd.ospvSizeOfDetailLog = ospvSizeOfDetailLog;
            nbData->Message.UseInd.ospvDetailLog = ospvDetailLog;

            // time stamp (UseInd) IN QUEUE event
            OSPPOSTimeGetTime(&nbData->InQuequeTime, &nbData->InQuequeTimeMS);

            // put a new message to the non-blocking queue
            *(nbData->ErrorCode) = OSPC_REPORT_USAGE_BLOCK;
            errorcode = SyncQueueAddTransaction(nbMonitor->SyncQue, nbData);

            if (OSPC_ERR_NO_ERROR == errorcode) {
                /*
                 *  This is a bit odd.  If the caller wants to wait,
                 *    it is responsible for deleting nbData.
                 *  Oterwise, the work thread will clean up
                 */
                if (1 == ShouldBlock) {
                    /*
                     * SHOULD wait
                     */
                    OSPM_MUTEX_LOCK(nbData->Mutex, errorcode);
                    while ((*(nbData->ErrorCode) == OSPC_REPORT_USAGE_BLOCK) && (OSPC_ERR_NO_ERROR == errorcode)) {
                        OSPM_CONDVAR_WAIT(nbData->CondVar, nbData->Mutex, errorcode);
                    }
                    OSPM_MUTEX_UNLOCK(nbData->Mutex, errorcode);
                    deleteNBDATA(nbData);
                } else {
                    /*
                     * should NOT wait
                     */
                }
            }
        } else {
            errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
        }
    } else {
        errorcode = -1;
    }

    return errorcode;
}

int OSPPTransactionIndicateCapabilities_nb(NBMONITOR *nbMonitor,    /* In - NBMonitor Pointer   */
    int ShouldBlock,                                                /* In - 1 WILL block, 0 - will NOT block */
    int *OSPErrorCode,                                              /* Out- Error code returned by the blocking function */
    OSPTTRANHANDLE ospvTransaction,                                 /* In - Transaction Handle  */
    const char *ospvSource,                                         /* In - Source of call      */
    const char *ospvSourceDevice,                                   /* In - SourceDevice of call */
    const char *ospvSourceNetworkId,                                /* In - NetworkId of call */
    unsigned ospvAlmostOutOfResource,                               /* In - A Boolean flag indicating device's availability */
    unsigned *ospvSizeOfDetailLog,                                  /* In\Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog)                                            /* In\Out - Location of detail log storage */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    NBDATA *nbData = OSPC_OSNULL;

    if (nbMonitor->TimeToShutDown == 1) {
        /*
         * Q is shutting down
         */
        errorcode = -1;
    } else if (SyncQueueGetNumberOfTransactions(nbMonitor->SyncQue) > nbMonitor->MaxQueSize) {
        /*
         * There are already to many requests in the Q
         */
        errorcode = -1;
    } else {
        /*
         * allocate space
         * the space will be deallocated by one of the working thread
         */
        errorcode = newNBDATA(&nbData, MESSAGE_TYPE_CAP_IND);

        if (OSPC_ERR_NO_ERROR == errorcode) {
            // fill out the data structure
            nbData->ShouldBlock = ShouldBlock;
            nbData->ErrorCode = OSPErrorCode;
            nbData->Message.CapInd.ospvTransaction = ospvTransaction;
            nbData->Message.CapInd.ospvSource = ospvSource;
            nbData->Message.CapInd.ospvSourceDevice = ospvSourceDevice;
            nbData->Message.CapInd.ospvAlmostOutOfResources = ospvAlmostOutOfResource;
            nbData->Message.CapInd.ospvSourceNetworkId = ospvSourceNetworkId;
            nbData->Message.CapInd.ospvSizeOfDetailLog = ospvSizeOfDetailLog;
            nbData->Message.CapInd.ospvDetailLog = ospvDetailLog;

            // time stamp (AuthReq) IN QUEUE event
            OSPPOSTimeGetTime(&nbData->InQuequeTime, &nbData->InQuequeTimeMS);

            // put a new message to the non-blocking queue
            *(nbData->ErrorCode) = OSPC_AUTH_REQUEST_BLOCK;
            errorcode = SyncQueueAddTransaction(nbMonitor->SyncQue, nbData);

            if (OSPC_ERR_NO_ERROR == errorcode) {
                /*
                 *  This is a bit odd.  If the caller wants to wait,
                 *    it is responsible for deleting nbData.
                 *  Oterwise, the work thread will clean up
                 */
                if (1 == nbData->ShouldBlock) {
                    /*
                     * SHOULD wait
                     */
                    OSPM_MUTEX_LOCK(nbData->Mutex, errorcode);
                    while ((*(nbData->ErrorCode) == OSPC_CAP_IND_BLOCK) && (OSPC_ERR_NO_ERROR == errorcode)) {
                        OSPM_CONDVAR_WAIT(nbData->CondVar, nbData->Mutex, errorcode);
                    }
                    OSPM_MUTEX_UNLOCK(nbData->Mutex, errorcode);
                    deleteNBDATA(nbData);
                } else {
                    /*
                     * should NOT wait
                     */
                }
            }
        } else {
            errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
        }
    }

    return errorcode;
}

OSPTUINT64 GetDeltaMS(OSPTTIME FromTimeSec, unsigned int FromTimeMS,
    OSPTTIME ToTimeSec, unsigned int ToTimeMS)
{
    return(1000 * (ToTimeSec - FromTimeSec) + (ToTimeMS - FromTimeMS));
}
