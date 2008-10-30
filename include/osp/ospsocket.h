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
 * ospsocket.h - Prototypes for socket functions.
 */

#ifndef _OSPSOCKET_H
#define _OSPSOCKET_H

#include "osp/ospdatatypes.h"
#include "osp/ospcomm.h"
#include "osp/osphttp.h"

#ifdef _WIN32
#  define OSPC_SOCK_INVALID         SOCKET_ERROR
#else
#  define OSPC_SOCK_INVALID         -1
#endif

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPSockClose(OSPTBOOL, OSPTSOCKET *, OSPTSSLSESSION **);
    int OSPPSockConnect(OSPTSOCKET *, OSPTBOOL, OSPTIPADDR, unsigned short, struct timeval *, OSPTSSLSESSION **);
    int OSPPSockConnectAuditURL(OSPTHTTP *, OSPTBOOL *);
    int OSPPSockConnectServicePoint(OSPTHTTP *, OSPTBOOL *, OSPTBOOL *);
    int OSPPSockDisableNagle(OSPTHTTP *);
    int OSPPSockGetHostIP(char *, OSPTIPADDR *);
    int OSPPSockNew(OSPTHTTP *);
    void OSPPSockProcessRequest(OSPTHTTP *, unsigned char *, unsigned, unsigned char **, unsigned *, unsigned char **, unsigned *, int *);
    int OSPPSockRead(OSPTHTTP *, unsigned char *, unsigned int *);
    int OSPPSockSetBlockingIO(OSPTSOCKET, OSPTBOOL);
    int OSPPSockSetKeepAlive(OSPTHTTP *);
    int OSPPSockWaitTillReady(OSPTSOCKET, OSPTBOOL, struct timeval *);
    int OSPPSockWrite(OSPTHTTP *, unsigned char *, unsigned int *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSOCKET_H */
