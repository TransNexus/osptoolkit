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
 * ospcomm.h - Constants and prototypes for Comm object.
 */

#ifndef _OSPCOMM_H
#define _OSPCOMM_H

#include "osp/osp.h"
#include "osp/ospmsgque.h"
#include "osp/ospmsginfo.h"
#include "osp/ospsecurity.h"

#define OSPC_COMM_MULTI_MSG     "multipart/signed; protocol=\"application/pkcs7-signature\"; micalg=sha1; boundary=bar"
#define OSPC_COMM_TEXT_MSG      "text/plain"

#define OSPC_COMM_MAX_DEGRADED_TIME         60
#define OSPC_COMM_INUSE_BIT                 1
#define OSPC_COMM_SPARE_CHARS               16

#define OSPC_COMM_SVCPT_SECURITY_BIT        0x01
#define OSPC_COMM_SVCPT_DEGRADED_BIT        0x02

#define OSPC_COMM_SHUTDOWN_BIT              0x01
#define OSPC_COMM_HTTPSHUTDOWN_BIT          0x02
#define OSPC_COMM_AUDIT_ON                  0x04
#define OSPC_COMM_AUDIT_OFF                 0xFB    /* turn off auditing */

#define OSPC_COMM_DEFAULT_SECURE_PORT       443
#define OSPC_COMM_DEFAULT_NONSECURE_PORT    80

#define OSPM_COMM_SECURED_IO(sp) (sp->Flags & OSPC_COMM_SVCPT_SECURITY_BIT)

/*
 * service point typedef
 */
typedef struct _OSPTSVCPT {
    OSPTLISTLINK Link;
    OSPTIPADDR IpAddr;      /* network byte order */
    time_t DegradedTime;
    unsigned Index;
    unsigned short Port;    /* network byte order */
    /* bit 1: 0 - http      1 - https    */
    /* bit 2: 0 - ok        1 - degraded */
    unsigned char Flags;    
    char *HostName;
    char *URI;
    unsigned long MaxMsgAllowed;
} OSPTSVCPT;

/*
 * communication manager typedef
 */
typedef struct _OSPTCOMM {
    OSPTMUTEX Mutex;
    OSPTTHREADID ThreadId;
    /* bit 1: 0 - run       1 - commshutdown */
    /* bit 2: 0 - run       1 - http shutdown */
    /* bit 3: 0 - audit off 1- audit on */
    unsigned char Flags;
    OSPTMSGQUEUE *MsgQueue;
    unsigned NumberOfServicePoints;
    unsigned HttpMaxConnections;
    unsigned HttpPersistence;
    unsigned HttpRetryDelay;
    unsigned HttpRetryLimit;
    unsigned HttpTimeout;
    OSPTUINT64 ConnSelectionTimeout;
    OSPTSVCPT *ServicePointList;
    OSPTSVCPT *AuditURL;
    unsigned HttpConnCount;
    struct _OSPTHTTP *HttpConnList;
    int ShutdownTimeLimit;
    OSPTSEC *Security;
    int RoundRobinIndex;
    OSPTMUTEX HttpSelectMutex;
    OSPTCONDVAR HttpSelCondVar;
} OSPTCOMM;

#define OSPPCommAddTransaction(comm,msginfo)    OSPPMsgQueueAddTransaction(comm->MsgQueue, msginfo)

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPCommNew(OSPTCOMM **);
    void OSPPCommDelete(OSPTCOMM **);
    int OSPPCommGetPersistence(OSPTCOMM *, unsigned *);
    int OSPPCommGetRetryDelay(OSPTCOMM *, unsigned *);
    int OSPPCommGetRetryLimit(OSPTCOMM *, unsigned *);
    int OSPPCommGetTimeout(OSPTCOMM *, unsigned *);
    int OSPPCommSetPersistence(OSPTCOMM *, unsigned);
    int OSPPCommSetRetryDelay(OSPTCOMM *, unsigned);
    int OSPPCommSetRetryLimit(OSPTCOMM *, unsigned);
    int OSPPCommSetTimeout(OSPTCOMM *, unsigned);
    int OSPPCommSetConnSelectionTimeout(OSPTCOMM *, OSPTUINT64);
    int OSPPCommGetMaxConnections(OSPTCOMM *, unsigned *);
    int OSPPCommSetMaxConnections(OSPTCOMM *, unsigned);
    int OSPPCommIncrementHttpConnCount(OSPTCOMM *);
    int OSPPCommDecrementHttpConnCount(OSPTCOMM *);
    int OSPPCommGetHttpConnCount(OSPTCOMM *, unsigned *);
    int OSPPCommGetNumberOfServicePoints(OSPTCOMM *, unsigned *);
    void OSPPCommGetServicePointList(OSPTCOMM *, OSPTSVCPT **);
    int OSPPCommGetServicePoints(OSPTCOMM *, unsigned, unsigned, char *[]);
    int OSPPCommSetServicePoints(OSPTCOMM *, unsigned, const char **);
    int OSPPCommSetAuditURL(OSPTCOMM *, const char *);
    void OSPPCommSetAuditFlag(OSPTCOMM *, unsigned);
    int OSPPCommBuildServicePoint(OSPTSVCPT *, char *);
    void OSPPCommSetShutdown(OSPTCOMM **, int);
    void OSPPCommShutdownConnections(OSPTCOMM *, int);
    void OSPPCommSetSecurity(OSPTCOMM *, OSPTSEC *);
    OSPTSEC *OSPPCommGetSecurity(OSPTCOMM *);
    void OSPPCommSignalAllConnections(OSPTCOMM *ospvComm);
    int OSPPCommParseSvcPt(const char *ospvURL, OSPTSVCPT **ospvSvcPt, unsigned ospvIndex);
    int OSPPCommValidateSvcPts(unsigned ospvNumberOfServicePoints, const char **ospvServicePoint);
    int OSPPCommUpdateURLs(OSPTCOMM *, unsigned, const char **);
    int OSPPCommGetNumberOfTransactions(OSPTCOMM *, unsigned *);
    void OSPPHttpDecrementConnectionCount(OSPTCOMM *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPCOMM_H */
