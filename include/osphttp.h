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

#include "osp.h"
#include "ospcomm.h"
#include "ospsslsess.h"

#define OSPC_HTTP_CONTENT_LENGTH    1
#define OSPC_HTTP_CONTENT_TYPE      2
#define OSPC_HTTP_CONNECTION_TYPE   3

#define OSPC_HTTP_200_OK            "200 "
#define OSPC_HTTP_100_CONTINUE      "100 continue"
#define OSPC_HTTP_503_SERV_UNAVAIL  "503 service unavailable"
#define OSPC_HTTP_HEADER_MSG_FMT    "POST %s HTTP/1.1\r\nHost: %s\r\ncontent-type: %s\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\n\r\n"

#define OSPC_SOCK_CONNECTED_MASK    0xFE
#define OSPC_SOCK_CONNECTED_BIT     0x01
#define OSPC_HTTP_AUDIT_TYPE        0x02

/*-------------------------------------------*/
/* http connection typedef                   */
/*-------------------------------------------*/ 
typedef struct _OSPTHTTP
{
    OSPTLISTLINK        Link;
    OSPTTHREADID        ThreadId;
    OSPTMUTEX           Mutex;
    OSPTCONDVAR         CondVar;
    OSPTSVCPT           *ServicePoint;
    void                *Comm;
    unsigned            NumberOfTransactions;
    time_t              LastTransactionTime;
    OSPTSOCKET          SockFd;
    OSPTSSLSESSION      *SSLSession;
    OSPTSSIZE_T         ByteCount;
    unsigned char       Flags;
    OSPTMSGINFO         *MsgInfoList;
} OSPTHTTP;

#ifdef __cplusplus
extern "C" 
{
#endif

    int      OSPPHttpNew(OSPTCOMM *, OSPTHTTP **);
    void     OSPPHttpDelete(OSPTHTTP **ospvHttp);
    int      OSPPHttpRequestHandoff(OSPTCOMM *, OSPTMSGQUEUE *);
    int      OSPPHttpVerifyResponse(char *, int *);
    void     OSPPHttpParseHeader(unsigned char *, unsigned char **, unsigned *, int , int *);
    OSPTSEC *OSPPHttpGetSecurity(OSPTHTTP *);

#ifdef __cplusplus
}
#endif

#endif
