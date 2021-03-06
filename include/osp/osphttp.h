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

#ifndef _OSPHTTP_H
#define _OSPHTTP_H

#include "osp/osp.h"
#include "osp/ospcomm.h"
#include "osp/ospsslsess.h"

#define OSPC_HTTP_CONTENT_LENGTH    1
#define OSPC_HTTP_CONTENT_TYPE      2
#define OSPC_HTTP_CONNECTION_TYPE   3

#define OSPC_HTTP_100_CONTINUE          " 100 continue"
#define OSPC_HTTP_200_OK                " 200 "
#define OSPC_HTTP_400_BAD_REQUEST       " 400"
#define OSPC_HTTP_401_UNAUTHORIZED      " 401"
#define OSPC_HTTP_404_NOT_FOUND         " 404"
#define OSPC_HTTP_503_SERV_UNAVAIL      " 503"
#define OSPC_HTTP_504_SERV_NOT_READY    " 504"
#define OSPC_HTTP_HEADER_MSG_FMT        "POST %s HTTP/1.1\r\nHost: %s\r\ncontent-type: %s\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\n\r\n"
/*
 * Instead of using Content-Type and Content-Length in the header and payload in the message,
 * use URI?queryString in the header.
 */
#define OSPC_HTTP_HEADER_MSG_FMT_GET "GET %s?%s HTTP/1.1\r\nHost: %s\r\nConnection: Keep-Alive\r\n\r\n"


#define OSPC_SOCK_CONNECTED_MASK    0xFE
#define OSPC_SOCK_CONNECTED_BIT     0x01
#define OSPC_HTTP_AUDIT_TYPE        0x02

/* http connection typedef */
typedef struct _OSPTHTTP {
    OSPTLISTLINK Link;
    OSPTTHREADID ThreadId;
    OSPTMUTEX Mutex;
    OSPTCONDVAR CondVar;
    OSPTSVCPT *ServicePoint;
    void *Comm;
    unsigned NumberOfTransactions;
    time_t LastTransactionTime;
    OSPTSOCKET SockFd;
    OSPTSSLSESSION *SSLSession;
    OSPTSSIZE_T ByteCount;
    unsigned char Flags;
    OSPT_MSG_INFO *MsgInfoList;
    OSPTSVCPT *ServicePointList;
    unsigned long CurrentMsgCount;
} OSPTHTTP;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    int OSPPHttpNew(OSPTCOMM *, OSPTHTTP **);
    void OSPPHttpDelete(OSPTHTTP **ospvHttp);
    int OSPPHttpRequestHandoff(OSPTCOMM *, OSPTMSGQUEUE *);
    int OSPPHttpVerifyResponse(char *, int *, OSPTHTTP *ospvHttp);
    void OSPPHttpParseHeader(unsigned char *, unsigned char **, unsigned *, int, int *);
    OSPTSEC *OSPPHttpGetSecurity(OSPTHTTP *);
    int osppHttpGetIdleHttpConn(OSPTHTTP **, OSPTHTTP **, unsigned, int, int);
    void osppHttpCopySPList(OSPTCOMM *, OSPTHTTP **, int);
    void osppHttpGetServicePointList(OSPTHTTP *, OSPTSVCPT **);
    void osppHttpDeleteServicePointList(OSPTSVCPT **, int);

#ifdef __cplusplus
}
#endif

#endif /* _OSPHTTP_H */
