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
 * ospmsgque.h - Structures and prototypes for message queue object.
 */

#ifndef _OSPMSGQUE_H
#define _OSPMSGQUE_H

#include "osp/osp.h"
#include "osp/ospmsginfo.h"

#define OSPPMsgQueueGetCondVar(ospvMsgQueue)    (ospvMsgQueue)->CondVar

/*
 * message queue typedef
 */
typedef struct _OSPTMSGQUEUE {
    unsigned NumberOfTransactions;
    OSPTMUTEX Mutex;
    OSPTCONDVAR CondVar;
    OSPTMSGINFO *MsgInfoList;
} OSPTMSGQUEUE;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPMsgQueueNew(OSPTMSGQUEUE **);
    void OSPPMsgQueueDelete(OSPTMSGQUEUE **);
    int OSPPMsgQueueAddTransaction(OSPTMSGQUEUE *, OSPTMSGINFO *);
    void OSPPMsgQueueIncrementNumberOfTransactions(OSPTMSGQUEUE *);
    void OSPPMsgQueueDecrementNumberOfTransactions(OSPTMSGQUEUE *);
    int OSPPMsgQueueGetNumberOfTransactions(OSPTMSGQUEUE *, unsigned *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPMSGQUE_H */
