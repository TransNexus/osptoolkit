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
 * ospmsgque.cpp - MsgQueue object funcions.
 */

#include "osp.h"
#include "ospmsgque.h"
#include "ospmsginfo.h"
#include "osplist.h"

static
int
osppMsgQueueInitSync(
    OSPTMSGQUEUE *ospvMsgQueue)
{
    int errorcode = OSPC_ERR_NO_ERROR,
        tmperror  = OSPC_ERR_NO_ERROR;

    OSPM_MUTEX_INIT(ospvMsgQueue->Mutex, OSPC_OSNULL, errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPM_CONDVAR_INIT(ospvMsgQueue->CondVar, OSPC_OSNULL, errorcode);
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            OSPM_MUTEX_DESTROY(ospvMsgQueue->Mutex, tmperror);
        }
    }        
    return errorcode;
}

int
OSPPMsgQueueNew(
    OSPTMSGQUEUE **ospvMsgQueue)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC(*ospvMsgQueue, OSPTMSGQUEUE, sizeof(OSPTMSGQUEUE));
    if (*ospvMsgQueue != (OSPTMSGQUEUE *)OSPC_OSNULL) 
    {

        OSPM_MEMSET(*ospvMsgQueue, 0, sizeof(OSPTMSGQUEUE));

        /*
         * initialize the mutex and conditional variable
         * for the message queue
         */
        errorcode = osppMsgQueueInitSync(*ospvMsgQueue);

        /*
         * initialize the list used to hold message info
         * structures (items)
         */
        OSPPListNew((OSPTLIST *)&((*ospvMsgQueue)->MsgInfoList));

        if (errorcode != OSPC_ERR_NO_ERROR)
            OSPM_FREE(*ospvMsgQueue);
    }

    if (errorcode != OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPC_ERR_MSGQ_NO_MEMORY;
        OSPM_DBGERRORLOG(errorcode, "ospvMsgQueue malloc failed");
    }

    return errorcode;
}

void
OSPPMsgQueueDelete(
    OSPTMSGQUEUE **ospvMsgQueue)
{

    if (*ospvMsgQueue != (OSPTMSGQUEUE *)OSPC_OSNULL)
    {
        OSPM_FREE(*ospvMsgQueue);
        *ospvMsgQueue = (OSPTMSGQUEUE *)OSPC_OSNULL;
    }

    return;
}

void
OSPPMsgQueueIncrementNumberOfTransactions(
    OSPTMSGQUEUE *ospvMsgQueue)
{
    if (ospvMsgQueue != OSPC_OSNULL)
    {
        ospvMsgQueue->NumberOfTransactions++;
    }
    return;
}

void
OSPPMsgQueueDecrementNumberOfTransactions(
    OSPTMSGQUEUE *ospvMsgQueue)
{
    if (ospvMsgQueue != OSPC_OSNULL)
    {
        ospvMsgQueue->NumberOfTransactions--;
    }
    return;
}

int
OSPPMsgQueueAddTransaction(
    OSPTMSGQUEUE *ospvMsgQueue,
    OSPTMSGINFO  *ospvMsgInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvMsgQueue == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_UTIL_INVALID_ARG;
    }
    if (ospvMsgInfo == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_UTIL_INVALID_ARG;
    }

    /*
     * acquire message queue mutex
     */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPM_MUTEX_LOCK(ospvMsgQueue->Mutex, errorcode);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /*
         * add the item to the message queue list
         */
        OSPPListAppend((OSPTLIST *)&(ospvMsgQueue->MsgInfoList), 
            (void *)ospvMsgInfo);

        /*
         * increment number of transactions in the queue
         */
        OSPPMsgQueueIncrementNumberOfTransactions(ospvMsgQueue);

        /*
         * signal the communication manager that a new
         * transaction is now in the queue
         */
        OSPM_CONDVAR_SIGNAL(ospvMsgQueue->CondVar, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

        /* release msg queue mutex lock */
        OSPM_MUTEX_UNLOCK(ospvMsgQueue->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

        /*
         * wait for transaction response from the
         * the communication manager
         */
        errorcode = OSPPMsgInfoWaitForMsg(ospvMsgInfo);
    }
    return errorcode;
}

