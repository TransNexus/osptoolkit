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
 * ospfail.h - Structures and prototypes for linked lists.
 */
#ifndef _OSP_FAIL_H
#define _OSP_FAIL_H

#include "osp.h"

enum OSPEFAILREASON
{
    OSPC_FAIL_NONE=0,                    /* no failure (call successful) 1*/

    OSPC_FAIL_LUSER_DISC=101,                   /* local user disconnect 2*/
    OSPC_FAIL_LUSER_ADMIN=180,   /* admin. failure prior to local system 3*/

    OSPC_FAIL_LOCAL_NORSRC=201,          /* insufficient local resources 4*/
    OSPC_FAIL_LOCAL_NOAUTH=202,        /* inadequate local authorisation 5*/
    OSPC_FAIL_LOCAL_BADAUTH=203,    /* inappropriate local authorisation 6*/
    OSPC_FAIL_LOCAL_BADROUTE=204,   /* bad call routing for local system 7*/
    OSPC_FAIL_LOCAL_ADMIN=280,         /* admin. failure in local system 8*/

    OSPC_FAIL_NET_UNREACH=301,                /* destination unreachable 9*/
    OSPC_FAIL_NET_BADPATH=302,             /* inappropriate network path10*/
    OSPC_FAIL_NET_BADPERF=303,         /* inadequate network performance11*/
    OSPC_FAIL_NET_ADMIN=380,           /* admin. failure between systems12*/

    OSPC_FAIL_REMOTE_GENERAL=410,          /* generic call setup failure13*/
    OSPC_FAIL_REMOTE_NORESP=411,       /* no response from remote system14*/
    OSPC_FAIL_REMOTE_TCPFIN=412,               /* TCP connection refused15*/
    OSPC_FAIL_REMOTE_BADSEC=413,     /* incompatible security parameters16*/
    OSPC_FAIL_REMOTE_BADCOMM=414,       /* incompatible comm. parameters17*/
    OSPC_FAIL_REMOTE_REFUSE=415,       /* setup refused by remote system18*/
    OSPC_FAIL_REMOTE_NORSRC=420,        /* insufficient remote resources19*/
    OSPC_FAIL_REMOTE_INT=421,          /* internal resources unavailable20*/
    OSPC_FAIL_REMOTE_EXT=422,          /* external resources unavailable21*/
    OSPC_FAIL_ADMIN=480,             /* admin. failure in remote systems22*/

    OSPC_FAIL_RUSER_BUSY=501,                        /* remote user busy23*/
    OSPC_FAIL_RUSER_NETBUSY=502,      /* remote network busy (fast busy)24*/
    OSPC_FAIL_RUSER_UNAVAIL=503,  /* remote user unavailable (no answer)25*/
    OSPC_FAIL_RUSER_DISC=504,                  /* remote user disconnect26*/
    OSPC_FAIL_RUSER_ADMIN=580, /* admin. failure external to remote sys.27*/

    OSPC_FAIL_GENERAL=999                             /* generic failure28*/

};

int OSPPFailReasonFind(enum OSPEFAILREASON ospvFailureReason);

#endif


