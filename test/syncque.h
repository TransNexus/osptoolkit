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
 *  syncque.h - Structures and prototypes for sync queue object.
 *  
 *  This module implements a MT Safe Queue.  It provides a non-blocking
 *  method - SyncQueueAddTransaction - for producers to add transactions 
 *  to the queue, and a blocking method - SyncQueueRemoveTransaction -
 *  for consumers to remove transactions from the queue.
 *  SyncQueueBlockWhileNotEmpty - allows the caller to wait while all
 *  transactions have been processed by consumers.
 *
 *  The queue is implemented using a linked list - OSPTLIST.
 *  This implementation assumes that the first member of the transaction
 *  structure is of type OSPTLISTLINK.  This member is used by OSPTLIST
 *  and must not be used by consumers or producers.
 */
#ifndef _SYNCQUE_H
#define _SYNCQUE_H

#include "osp/osp.h"
#include "osp/osplist.h"


/*-------------------------------------------*/
/* sync queue typedef                        */
/*-------------------------------------------*/ 
typedef struct _SYNCQUEUE
{
    unsigned            NumberOfTransactions;
    OSPTMUTEX           Mutex; 
    OSPTCONDVAR         CondVarNotEmpty;  
    OSPTCONDVAR         CondVarEmpty; 
    OSPTLIST            PayloadList;
} SYNCQUEUE;

#ifdef __cplusplus
extern "C" 
{
#endif

    int       SyncQueueNew(                     SYNCQUEUE **);
    void      SyncQueueDelete(                  SYNCQUEUE **);
    unsigned  SyncQueueGetNumberOfTransactions( SYNCQUEUE *);
    int       SyncQueueBlockWhileNotEmpty(      SYNCQUEUE *);
    int       SyncQueueAddTransaction(          SYNCQUEUE *, void *);   //  non-blocking
    int       SyncQueueRemoveTransaction(       SYNCQUEUE *, void **);  //  blocks if the queue is emtpy

#ifdef __cplusplus
}
#endif

#endif
















