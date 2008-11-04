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
 * syncque.cpp - SyncQueue object funcions.
 */

#include "syncque.h"

static int SyncQueueInitSync(SYNCQUEUE *);
void SyncQueueIncrementNumberOfTransactions(SYNCQUEUE *);
void SyncQueueDecrementNumberOfTransactions(SYNCQUEUE *);

/*
 * Private API
 */

static int SyncQueueInitSync(SYNCQUEUE *syncQueue)
{
    int errorcode = OSPC_ERR_NO_ERROR, tmperror = OSPC_ERR_NO_ERROR;

    OSPM_MUTEX_INIT(syncQueue->Mutex, OSPC_OSNULL, errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_CONDVAR_INIT(syncQueue->CondVarNotEmpty, OSPC_OSNULL, errorcode);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_CONDVAR_INIT(syncQueue->CondVarEmpty, OSPC_OSNULL, errorcode);
        }

        if (errorcode != OSPC_ERR_NO_ERROR) {
            OSPM_MUTEX_DESTROY(syncQueue->Mutex, tmperror);
        }
    }

    return errorcode;
}

void SyncQueueIncrementNumberOfTransactions(SYNCQUEUE *syncQueue)
{
    syncQueue->NumberOfTransactions++;
}

void SyncQueueDecrementNumberOfTransactions(SYNCQUEUE *syncQueue)
{
    syncQueue->NumberOfTransactions--;
}

/*
 * Public API
 */

int SyncQueueNew(SYNCQUEUE **syncQueue)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC(*syncQueue, SYNCQUEUE, sizeof(SYNCQUEUE));

    if (*syncQueue != (SYNCQUEUE *) OSPC_OSNULL) {
        OSPM_MEMSET(*syncQueue, 0, sizeof(SYNCQUEUE));

        (*syncQueue)->NumberOfTransactions = 0;

        /*
         * initialize the mutex and conditional variable for the message queue
         */
        errorcode = SyncQueueInitSync(*syncQueue);

        /*
         * initialize the list used to hold message info structures (items)
         */
        OSPPListNew(&((*syncQueue)->PayloadList));

        if (errorcode != OSPC_ERR_NO_ERROR)
            OSPM_FREE(*syncQueue);
    } else {
        errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
    }

    return errorcode;
}

void SyncQueueDelete(SYNCQUEUE **syncQueue)
{
    if (*syncQueue != (SYNCQUEUE *) OSPC_OSNULL) {
        OSPM_FREE(*syncQueue);
        *syncQueue = (SYNCQUEUE *) OSPC_OSNULL;
    }
}

unsigned SyncQueueGetNumberOfTransactions(SYNCQUEUE *syncQueue)
{
    unsigned RetVal = 0;
    int errorcode = OSPC_ERR_NO_ERROR;

    /* acquire mutex lock */
    OSPM_MUTEX_LOCK(syncQueue->Mutex, errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        RetVal = syncQueue->NumberOfTransactions;

        /* release mutex lock */
        OSPM_MUTEX_UNLOCK(syncQueue->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);
    }

    return (RetVal);
}

int SyncQueueAddTransaction(SYNCQUEUE *syncQueue, void *transaction)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * acquire message queue mutex
     */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MUTEX_LOCK(syncQueue->Mutex, errorcode);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*
         * add the item to the message queue list
         */
        OSPPListAppend(&(syncQueue->PayloadList), transaction);

        /*
         * increment number of transactions in the queue
         */
        SyncQueueIncrementNumberOfTransactions(syncQueue);

        /*
         * signal the communication manager that a new
         * transaction is now in the queue
         */
        OSPM_CONDVAR_SIGNAL(syncQueue->CondVarNotEmpty, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

        /* release msg queue mutex lock */
        OSPM_MUTEX_UNLOCK(syncQueue->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

    }
    return errorcode;
}

int SyncQueueRemoveTransaction(SYNCQUEUE *syncQueue, void **transaction)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * acquire message queue mutex
     */
    OSPM_MUTEX_LOCK(syncQueue->Mutex, errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR) {

        /*
         * wait for conditional variable on the transaction count
         */
        while (0 == syncQueue->NumberOfTransactions) {
            OSPM_CONDVAR_WAIT(syncQueue->CondVarNotEmpty, syncQueue->Mutex, errorcode);
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /*
             * thereis at least one transaction on the queue
             */
            *transaction = OSPPListRemove(&(syncQueue->PayloadList));
            SyncQueueDecrementNumberOfTransactions(syncQueue);

            /* 
             * Are there any more transactions on the queue ?
             */
            if (0 == syncQueue->NumberOfTransactions) {
                OSPM_CONDVAR_SIGNAL(syncQueue->CondVarEmpty, errorcode);
            }
        }

        /*
         * release the mutex lock
         */
        OSPM_MUTEX_UNLOCK(syncQueue->Mutex, errorcode);
    }

    return errorcode;
}

int SyncQueueBlockWhileNotEmpty(SYNCQUEUE *syncQueue)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * acquire message queue mutex
     */
    OSPM_MUTEX_LOCK(syncQueue->Mutex, errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR) {

        /*
         * wait for conditional variable on the transaction count
         */
        while (syncQueue->NumberOfTransactions > 0) {
            OSPM_CONDVAR_WAIT(syncQueue->CondVarEmpty, syncQueue->Mutex, errorcode);
        }

        /*
         * release the mutex lock
         */
        OSPM_MUTEX_UNLOCK(syncQueue->Mutex, errorcode);
    }

    return errorcode;
}
