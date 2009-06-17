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

/*osptnprobe.c
    Summary:        Functions that probe a list of IP addresses and
                    return the response time value for each system.

    Modifications:  MM/DD/YYYY  Who Description
                    ----------  --- -----------------------------------------


    Release #:      0.9.0

    Description
        OSPPTNProbe() provides a means of testing the response time of each
        server in a list provided by the caller.  The response time for
        each server is captured and returned to the caller.


        To call OSPPTNProbe, fill in the ospmipaddr field of an array of OSPT_TN_PROBE
        structures and indicating the size of the array. The function will
        return by filling in the ospmTime field for each element with the
        number of milliseconds the system took to respond to a message sent
        to its echo/udp service.

        The uMaxWait parameter indicates the maximum number of milliseconds
        to wait before giving up.

        The function works by sending a small UDP datagram to each remote
        host and measuring the time that elapses until the reply. There
        are advantages and disadvantages to this approach:

        Good:
        -  cleaner, easier to port code (compared to ICMP echo)
        -  closer simulation of voice traffic (which uses RTP/UDP)
            especially since many routers will treat ICMP special

        Bad:
        -  not all hosts (e.g. WinNT 4.0) automatically install
            deamons on the echo port (for WinNT 4.0, it's part of
            the optionally installed Simple TCP/IP Services)

        Enhancements:
        -  currently, the function only sends a single datagram to each
            host. With next hop caching in routers, as well as problems
            sending back-to-back packets in some OSs (e.g. Win95), it
            might be better to send several datagrams to each host,
            ignore the first reply (which establishes the routers' caches),
            and average the remainder.


    Notes
*/

/*******************************************************************************
    INCLUDE FILES
*******************************************************************************/
#include <time.h>

#ifndef _WIN32
#include <sys/timeb.h>
#else
#include <sys\timeb.h>
#endif

#include "osp/osptnprobe.h"
#include "osp/ospdest.h"
#include "osp/osputils.h"
#include "osp/ospcomm.h"
#include "osp/ospconfig.h"
#include "osp/ospsocket.h"

#ifdef TESTMODE
#include <stdio.h>
#endif

/* private header file stuff */

/* different status values */
enum socket_stats {
    OSPE_PRINIT,    /*  0  initialized */
    OSPE_PRCONN,    /*  1  connected */
    OSPE_PRSENT,    /*  2  request sent */
    OSPE_PRREAD,    /*  3  reply received */
    OSPE_PRDONE     /*  4  probe complete */
};

/* miscellaneous constants */

#define OSPC_TN_UDP_ECHOPORT    7   /* UDP echo port is 7 */

/* private globals - for multithreading, these MUST be read-only */

static char tnBuffer[] = "test message";

/* OSPPTNProbe - probe systems and return response times */
int OSPPTNProbe(                /*0 - normal; < 0 - error code  */
    OSPT_TN_PROBE *pProbeList,  /* list to probe */
    unsigned uNumHosts,         /* number of hosts to check */
    unsigned uMaxWait)          /* maximum wait in milliseconds */
{
    fd_set fdSocketSet; /* set of sockets for testing */
    int nMaxFd = 0, nMinFd = 0;
    int uErr = OSPC_ERR_TNPROBE_ERROR;

    do {    /* Break out of loop on error */
        /*
         * Nothing real fancy here, we initialize a socket for each host
         * (and set up the socket set), do the actual probes on the sockets,
         * and then clean up. Note that we do the cleanup step even if
         * there is an error in the intermediate processing. That way we
         * can be sure to release resources. It means, though, that the
         * intermediate functions can't just "bail out" when they encounter
         * an error. They must leave things in a reasonably consistent
         * state so that the cleanup process can function okay.
         */

        /* Step 1: Prepare the sockets for testing */
        if ((nMaxFd = OSPPTNProbeInit(pProbeList, &uNumHosts, &fdSocketSet, &nMinFd)) == -1) {
            break;
        }

        /* Step 2: Do the probes */
        OSPPTNProbeEcho(&fdSocketSet, pProbeList, uNumHosts, uMaxWait, nMaxFd, nMinFd);

        uErr = OSPC_ERR_NO_ERROR;

    } while (0);

    /* Step 3: Clean up the Sockets */
    OSPPTNProbeCleanup(pProbeList, uNumHosts);

    return uErr;
}

/* OSPPTNProbeInit - set up the sockets for probing */
int OSPPTNProbeInit(            /* returns maxFD (< 0 on error) */
    OSPT_TN_PROBE *pProbeList,  /* list to initialize */
    unsigned *lpNumHosts,       /* number of hosts */
    fd_set *pSocketSet,         /* socket set */
    int *nMinFd)                /* Min fdSocket val */
{
    int fdSocket;               /* single socket */
    register unsigned uCnt;     /* simple counter */
    int nMaxFd;                 /* max fdSocket value */
    int errorcode = 0;
    int one = 1;

    /*
     * This function tries to set up things as much as possible prior
     * to the actual probes. Nothing in here is timed, but it assumes
     * that everything after it returns will be. We start out by
     * setting the maximum fd value to -1. If no socket it successfully
     * opened, then it will stay there and we'll return an error.
     */

    nMaxFd = -1;                /* so far, no error */
    *nMinFd = 0x0fffffff;

    /* ZERO OUT THE SOCKET SET
     */

    FD_ZERO(pSocketSet);

    /*
     * The maxFd stuff is a little tricky, but we need to set it up
     * to make the select processing reasonably efficient later on.
     * Each socket is identified by a standard UNIX file descriptor,
     * which is a simple integer. The select call takes an array,
     * indexed by file descriptor value, and returns when any of the
     * referenced sockets has activity. On return, we need to find
     * out which specific socket needs service, so we're going to
     * have to scan the array. Without maxFd, we'd have to scan the
     * whole array each time. That could be inefficient, since the
     * array is likely to be a whole bunch bigger than the number
     * of hosts we're probing. Therefore, we keep track of the
     * maximum file descriptor value ever assigned to us. In scanning
     * the array, we can stop once we've reached this value.
     */

    /* iterate through the list of hosts to try */
    for (uCnt = 0; uCnt < *lpNumHosts; uCnt++) {

        /*
         * initialize the status value - note that we must go through the entire
         * list here to make sure that each entry is set to the OSPE_PRINIT state;
         * otherwise cleanup will be confusing. That means no breaking out of
         * the for loop.
         */

        pProbeList[uCnt].ospmPrStatus = OSPE_PRINIT;

        /* open a socket to each remote host */
        fdSocket = OSPPTNProbeConnect(pProbeList[uCnt].ospmipaddr);

        pProbeList[uCnt].ospmSocket = fdSocket;

        if (fdSocket >= 0) {    /* negative socket is an error */
            /* if no errors, update the status to connected */
            pProbeList[uCnt].ospmPrStatus = OSPE_PRCONN;

            /* remember the socket */
            OSPM_BLOCKIO(fdSocket, one, errorcode);
            if (errorcode == 0) {
                /* add it to select list */
                FD_SET((unsigned) fdSocket, pSocketSet);

                /* and update the maxFd variable if needed */
                if (fdSocket > nMaxFd) {
                    nMaxFd = fdSocket;
                }
                if (fdSocket < *nMinFd) {
                    *nMinFd = fdSocket;
                }
            }
        }
    }

    return nMaxFd;
}

/* OSPPTNProbeConnect - connect socket to specific address for probes */
int OSPPTNProbeConnect( /* returns socket descriptor, < 0 == error */
    OSPTIPADDR ipAddr)  /* remote address to connect */
{
    int fdSocket;       /* socket file descriptor */
    int uErr = OSPC_ERR_NO_ERROR;
    struct timeval timeout;
    OSPTSSLSESSION *sslsess = OSPC_OSNULL;

    /* allocate a socket */
    OSPM_UDP_SOCKET(fdSocket, uErr);

    timeout.tv_sec = OSPC_DEFAULT_PROBE_CONNECT / 1000;
    timeout.tv_usec = OSPC_DEFAULT_PROBE_CONNECT % 1000 * 1000;

    if ((fdSocket >= 0) && uErr == 0) {

        /* connect the socket */
        uErr = OSPPSockConnect(&fdSocket, OSPC_DEFAULT_BLOCKING_FLAG, ipAddr, htons(OSPC_TN_UDP_ECHOPORT), &timeout, &sslsess);

        if (uErr) {
            fdSocket = -1;
        }
    }

    return fdSocket;
}

/* OSPPTNProbeCleanup - free resources, etc. after probing */
void OSPPTNProbeCleanup(
    OSPT_TN_PROBE *pProbeList,  /* list to cleanup */
    unsigned uNumHosts)         /* number of hosts */
{
    register unsigned uCnt; /* simple counter */
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * Two things happening here: First, we close all the sockets
     * that we opened. Second, if we weren't able to get a
     * response from a host, we set the response time for that
     * host to infinity.
     */

    for (uCnt = 0; uCnt < uNumHosts; uCnt++) {
        /* if we were able to open a socket, close it */
        if (pProbeList[uCnt].ospmPrStatus > OSPE_PRINIT) {
            OSPM_CLOSE(pProbeList[uCnt].ospmSocket, errorcode);
        }

        /* if an error, set the response time to infinite */
        if (pProbeList[uCnt].ospmPrStatus < OSPE_PRDONE) {
            pProbeList[uCnt].ospmTime = (unsigned long)(-1);
        }
    }
}

/* OSPPTNProbeEcho - actually do the echo and wait for replies */
void OSPPTNProbeEcho(
    fd_set *pSockets,           /* sockets to probe */
    OSPT_TN_PROBE *pProbeList,  /* place to store results */
    unsigned uNumHosts,         /* number of hosts to probe */
    unsigned uMaxWait,          /* max ms to wait */
    int nMaxFd,                 /* max fd number for sockets */
    int nMinFd)                 /* min fd number for sockets */
{
    fd_set fdReadSet;               /* current working set */
    int fdSocket = 0;               /* current socket */
    unsigned uHostsLeft = 0;        /* hosts still pending */
    unsigned uTimeLeft = 0;         /* time remaining */
    register unsigned uHost = 0;    /* element number in pProbeList */
    unsigned uSent = 0;             /* bytes sent */
    int nRecv = 0;                  /* bytes received */
    register int nCnt = 0;          /* simple counter */
    int nRetVal = 0;                /* return value from select */
    unsigned long uTime1 = 0;       /* temp value for time calculations */
    unsigned long uTime2 = 0;       /* temp value for time calculations */
    struct timeval timeout;         /* timeout value for select */
    int uErr = OSPC_ERR_NO_ERROR;   /* Error value */
    char recvbuf[100];

    OSPM_MEMSET(recvbuf, 0, sizeof(recvbuf));
    OSPM_MEMSET(&timeout, 0, sizeof(struct timeval));

    do {                        /* Break out of loop on error */
        /*
         * This function actually sends the test datagram and waits
         * for a response. The general steps are
         *   -  iterate through the list and send a datagram
         *   -  note the time that the datagram was sent
         *   -  go into a select() loop, waiting for replies
         *   -  as each reply arrives, note its time
         *   -  calculate the difference between request and reply
         */

        /* assume every host will respond */
        uHostsLeft = uNumHosts;

        /* start the echo on each socket */
        for (uHost = 0; uHost < uNumHosts; uHost++) {
            if (pProbeList[uHost].ospmSocket >= 0) {
                /* transmit the datagram */
                OSPM_SEND(pProbeList[uHost].ospmSocket, uSent, tnBuffer, sizeof(tnBuffer), uErr);
                if (uSent != sizeof(tnBuffer)) {
                    /* if the send didn't succeed, ... */
                    uHostsLeft--;   /* there's one less host that will respond */
                } else {
                    pProbeList[uHost].ospmPrStatus = OSPE_PRSENT;
                }

                /* remember the current time */
                pProbeList[uHost].ospmTime = OSPPTNProbeTimerMS();

                /* reset uSent */
                uSent = 0;
            } else {
                /* invalid socket; decrement number of hosts that will respond */
                uHostsLeft--;
            }
        }

        /*
         * At this point, we've sent the requests to each host. Now we wait
         * for their replies. Before starting the wait loop, we need to set
         * up time variables. uTimeLeft keeps track of how long the caller
         * is allowing us to wait, so we start by setting that to uMaxWait.
         * After each call to select, we'll update that variable with the
         * remaining time. We also need to initialize uTime1, which marks
         * the time of the call to select. After select returns successfully,
         * we'll use uTime2 to note the current time, so that the difference
         * between uTime2 and uTime1 indicates how long select took.
         *
         * Also, notice that the wait loop is officially gated by uHostsLeft,
         * which is the number of hosts from whom we're still expecting to
         * hear a response. There are, however, several ways to break out
         * of the loop without uHostsLeft reaching zero. In particular,
         * select errors, select timeouts, or exhausting the time alotted
         * while outside of select.
         */

        uTimeLeft = uMaxWait;           /* to start out, we've got the full time */
        uTime1 = OSPPTNProbeTimerMS();  /* remember starting time */

        while (uHostsLeft > 0) {
            /* make a working copy of our socket set */
            memcpy(&fdReadSet, pSockets, sizeof(fdReadSet));

            /* convert time left into appropriate structure */
            timeout.tv_sec = uTimeLeft / 1000;
            timeout.tv_usec = (uTimeLeft % 1000) * 1000;

            /* call select and wait for replies */
            nRetVal = select(nMaxFd + 1,    /* Higest FD to test for */
                &fdReadSet,                 /* set for reading */
                (fd_set *)0,                /* set for writing */
                (fd_set *)0,                /* set for exceptions */
                &timeout);                  /* time to wait */

            /* CHECK FOR ERRORS/TIMEOUT
             */
            if (nRetVal <= 0) {
                /*
                 * Time ran out or Error Occurred;
                 */
                break;
            }

            /* There wasn't an error or timeout, so remember the current time */
            uTime2 = OSPPTNProbeTimerMS();

            /* check to see if timer has rolled over and make adjustment */
            if (uTime2 < uTime1) {
                uTime2 = (OSPC_TNPROBE_MAXTIMER - uTime1) + uTime2;
            }

            /* Now see who replied */
            for (nCnt = nRetVal, fdSocket = nMinFd; ((nCnt > 0) && (fdSocket <= nMaxFd)); ++fdSocket) {
                if (FD_ISSET(fdSocket, &fdReadSet)) {
                    /* which host responded? */
                    for (uHost = 0; uHost < uNumHosts; uHost++) {
                        if (pProbeList[uHost].ospmSocket == (fdSocket)) {
                            break;
                        }
                    }

                    /* make sure we found a host */
                    if (uHost < uNumHosts) {
                        if (!(pProbeList[uHost].ospmPrStatus == OSPE_PRSENT)) {
                            break;
                        }

                        /* receive the datagram */
                        nRecv = recv(fdSocket, recvbuf, sizeof(recvbuf), 0);
                        if (nRecv <= 0) {
                            break;
                        } else if (memcmp(tnBuffer, recvbuf, nRecv) != 0) {
                            break;
                        }

                        /* calculate how long it took */
                        pProbeList[uHost].ospmTime = uTime2 - pProbeList[uHost].ospmTime;

                        /* update the status of the current socket */
                        pProbeList[uHost].ospmPrStatus = OSPE_PRDONE;
                    }

                    /*
                     * Now that we've finished with this socket, clear it from
                     * the read set, and decrement the number of pending sockets
                     * left to check as well as the number of hosts on which
                     * we're still waiting.
                     */
                    FD_CLR((unsigned) fdSocket, pSockets);
                    nCnt--;
                    uHostsLeft--;
                }
            }

            /* update the time left */
            if ((uTime2 - uTime1) < uTimeLeft) {
                /*
                 * Note that we have to check to make sure the delta time is
                 * greater than the time left; we can't simply subtract the
                 * delta. That's because uTimeLeft is unsigned.
                 */
                uTimeLeft -= (uTime2 - uTime1);
                uTime1 = uTime2;
            } else {
                /*
                 * In this case, our allotted time has expired. This is probably
                 * pretty unlikely, as we'd expect most expirations to happen
                 * while in the select call. It is a loose end that has to be
                 * checked, however.
                 */
                break;
            }
        }
    } while (0);
}

/* OSPPTNProbeTimerMS - current time in milliseconds  */
unsigned long OSPPTNProbeTimerMS(void)  /* returns current time in ms */
{
    unsigned long millisecs = 0;
    int errcode = 0;

#ifndef _WIN32  /* UNIX */
    struct timeb timenow;

    OSPM_MEMSET(&timenow, 0, sizeof(struct timeb));
    errcode = ftime(&timenow);
#else

    struct _timeb timenow;

    OSPM_MEMSET(&timenow, 0, sizeof(struct _timeb));
    _ftime(&timenow);
#endif

    if (errcode != -1) {
        millisecs = ((timenow.time % OSPC_TNPROBE_TIMERMOD) * OSPC_TNPROBE_TIMERMULT) + timenow.millitm;
    }

    return millisecs;
}

void OSPPTNProbePruneList(
    OSPTLIST *ospvDests,
    OSPT_TN_PROBE *ospvProbes,
    unsigned ospvDelayLimit,
    unsigned *ospvNumDests)
{

    OSPT_TN_PROBE *tmpprobelist = ospvProbes;
    unsigned probecnt = 0, deccnt = 0, numleft = *ospvNumDests;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPTSVCPT *svcpt = OSPC_OSNULL;

    while (probecnt < *ospvNumDests) {
        if (tmpprobelist->ospmTime > ospvDelayLimit) {

            /* find specific node in destlist that matches ipaddr
             * in probelist and remove it
             */
            for (dest = (OSPT_DEST *)OSPPListFirst(ospvDests); dest != OSPC_OSNULL; dest = (OSPT_DEST *)OSPPListNext(ospvDests, dest)) {

                OSPPCommParseSvcPt(OSPPDestGetAddr(dest), &svcpt, 0);

                if (svcpt != OSPC_OSNULL) {

                    if (svcpt->IpAddr == tmpprobelist->ospmipaddr) {
                        OSPPListRemoveSpecificItem(ospvDests, dest);

                        if (svcpt->HostName)
                            OSPM_FREE(svcpt->HostName);

                        if (svcpt->URI)
                            OSPM_FREE(svcpt->URI);

                        OSPM_FREE(svcpt);

                        OSPPDestDelete(&dest);
                        break;
                    }

                    if (svcpt->HostName)
                        OSPM_FREE(svcpt->HostName);

                    if (svcpt->URI)
                        OSPM_FREE(svcpt->URI);

                    OSPM_FREE(svcpt);
                }
            }

            numleft--;
            deccnt = probecnt;

            /* now remove that node from probelist */
            while (deccnt < numleft) {
                ospvProbes[deccnt] = ospvProbes[deccnt + 1];
                deccnt++;
            }
        } else {
            /* move probe pointer only if copy has not been done */
            tmpprobelist++;
        }

        probecnt++;
    }

    /* reset NumDests */
    *ospvNumDests = numleft;
}

int OSPPTNProbeCompare(
    const void *probeptr1,
    const void *probeptr2)
{
    return ((OSPT_TN_PROBE *)probeptr1)->ospmTime - ((OSPT_TN_PROBE *)probeptr2)->ospmTime;
}

void OSPPTNProbeArrangeList(
    OSPTLIST *ospvDests,
    OSPT_TN_PROBE *ospvProbes,
    unsigned ospvNumDests)
{
    OSPTLIST newlist = OSPC_OSNULL;
    unsigned probecnt = 0;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPTSVCPT *svcpt = OSPC_OSNULL;

    qsort((void *)ospvProbes, (size_t)ospvNumDests, sizeof(OSPT_TN_PROBE), OSPPTNProbeCompare);

    OSPPListNew(&newlist);

    for (probecnt = 0; probecnt < ospvNumDests; probecnt++) {

        /* find specific node in destlist that matches ipaddr
         * in probelist, remove it and add to new list
         */
        for (dest = (OSPT_DEST *)OSPPListFirst(ospvDests); dest != OSPC_OSNULL; dest = (OSPT_DEST *)OSPPListNext(ospvDests, dest)) {

            OSPPCommParseSvcPt(OSPPDestGetAddr(dest), &svcpt, 0);
            if (svcpt != OSPC_OSNULL) {
                if (svcpt->IpAddr == ospvProbes[probecnt].ospmipaddr) {
                    OSPPListRemoveSpecificItem(ospvDests, dest);
                    OSPPListAppend(&newlist, dest);
                    dest = OSPC_OSNULL;

                    if (svcpt->HostName)
                        OSPM_FREE(svcpt->HostName);

                    if (svcpt->URI)
                        OSPM_FREE(svcpt->URI);

                    OSPM_FREE(svcpt);

                    break;
                } else {
                    if (svcpt->HostName)
                        OSPM_FREE(svcpt->HostName);

                    if (svcpt->URI)
                        OSPM_FREE(svcpt->URI);

                    OSPM_FREE(svcpt);
                }
            }
        }
    }

    *ospvDests = newlist;
}
