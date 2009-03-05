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
 * osptnprobe.h - structures and prototypes for functions that probe
 *             response time to different systems
 */
/*
 * To call OSPPTNProbe, fill in the ospmipaddr field of an array of OSPT_TN_PROBE
 * structures and indicating the size of the array. The function will
 * return by filling in the ospmTime field for each element with the
 * number of milliseconds the system took to respond to a message sent
 * to its echo/udp service.
 *
 * The uMaxWait parameter indicates the maximum number of milliseconds
 * to wait before giving up.
 *
 * The function works by sending a small UDP datagram to each remote
 * host and measuring the time that elapses until the reply. There
 * are advantages and disadvantages to this approach:
 *
 * Good:
 *   -  cleaner, easier to port code (compared to ICMP echo)
 *   -  closer simulation of voice traffic (which uses RTP/UDP)
 *      especially since many routers will treat ICMP special
 *
 * Bad:
 *   -  not all hosts (e.g. WinNT 4.0) automatically install
 *      deamons on the echo port (for WinNT 4.0, it's part of
 *      the optionally installed Simple TCP/IP Services)
 *
 * Enhancements:
 *   -  currently, the function only sends a single datagram to each
 *      host. With next hop caching in routers, as well as problems
 *      sending back-to-back packets in some OSs (e.g. Win95), it
 *      might be better to send several datagrams to each host,
 *      ignore the first reply (which establishes the routers' caches),
 *      and average the remainder.
 */

#ifndef _OSPTNPROBE_H
#define _OSPTNPROBE_H

#include "osp/osp.h"

typedef struct {                /* structure to pass probe information */
    OSPTIPADDR ospmipaddr;
    unsigned long ospmTime;     /* 0xFFFFFFFF = unreachable */
    int ospmSocket;             /* only used internally */
    unsigned ospmPrStatus;      /* only used internally */
    unsigned ospmPrRef;         /* Initial order in list */
} OSPT_TN_PROBE;

#define OSPC_TN_PROBE_UNREACHABLE 0xFFFFFFFF

#define OSPC_ERR_TNPROBE_ERROR  0x01
#define OSPC_ERR_TNPROBE_NORMAL 0x00

#define OSPC_TNPROBE_MAXTIMER   0xFFFF
#define OSPC_TNPROBE_MAXWAIT    500
#define OSPC_TNPROBE_TIMERMOD   10000000
#define OSPC_TNPROBE_TIMERMULT  1000

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPTNProbe(OSPT_TN_PROBE *pProbeList, unsigned uNumHosts, unsigned uMaxWait);
    /* 0 - maxFD ; !0 - error code */
    int  OSPPTNProbeInit(OSPT_TN_PROBE *pProbeList,     /* list to initialize */
            unsigned *lpNumHosts,                       /* number of hosts */
            fd_set *pSocketSet,                         /* socket set */
            int *nMinFd);                               /*pointer to min val for socket fds */
    /* 0 - socket descriptor; !0 - error code */
    int OSPPTNProbeConnect(OSPTIPADDR ipAddr);          /* remote address to connect */
    /* no return values */
    void OSPPTNProbeCleanup(OSPT_TN_PROBE *pProbeList,  /* list to cleanup */
            unsigned uNumHosts);                        /* number of hosts */
    /* no return value */
    void OSPPTNProbeEcho(fd_set *pSockets,              /* sockets to probe */
            OSPT_TN_PROBE *pProbeList,                  /* place to store results */
            unsigned uNumHosts,                         /* number of hosts to probe */
            unsigned uMaxWait,                          /* max ms to wait */
            int nMaxFd,                                 /* max fd number for sockets */
            int nMinFd);                                /* min fd number for sockets */
    int OSPPTNProbeCompare(const void *, const void *);
    unsigned long OSPPTNProbeTimerMS(void);
    void OSPPTNProbePruneList(OSPTLIST *, OSPT_TN_PROBE *, unsigned, unsigned *);
    void OSPPTNProbeArrangeList(OSPTLIST *, OSPT_TN_PROBE *, unsigned);

#ifdef __cplusplus
}
#endif

#endif /* _OSPTNPROBE_H */
