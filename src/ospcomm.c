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

/* ospcomm.cpp - Communication object functions. */

#include "osp/osp.h"
#include "osp/ospcomm.h"
#include "osp/ospsocket.h"
#include "osp/osphttp.h"
#include "osp/osplist.h"
#include "osp/ospmsgque.h"
#include "osp/ospsecurity.h"

/*
 * monitors the message queue for new communication requests.
 * When a new request is placed on the message queue, the conditional variable
 * (CondVar) within the message queue structure is signalled and the number of
 * transactions (NumberOfTransactions) will be incremented. This function will
 * awaken and assign the request to an available HTTP connection. Upon the
 * handoff of the request, this function will return to the wait condition
 * until the next message queue request arrives.
 *
 * returns void (runs until the communication manager thread is terminated)
 */
static OSPTTHREADRETURN osppCommMonitor(
    void *ospvArg)  /* In - Comm pointer casted to a void */
{
    OSPTCOMM **comm = OSPC_OSNULL;
    OSPTMSGQUEUE *msgqueue = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR, tmperror = OSPC_ERR_NO_ERROR;
    OSPTBOOL shutdown = OSPC_FALSE, do_forever = OSPC_TRUE;

    comm = (OSPTCOMM **)ospvArg;
    msgqueue = (*comm)->MsgQueue;

    while (do_forever) {
        /*
         * acquire message queue mutex
         */
        OSPM_MUTEX_LOCK(msgqueue->Mutex, errorcode);
        /* assert(errorcode == OSPC_ERR_NO_ERROR);
           assert is being handled already below */

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /*
             * wait for conditional variable on the transaction count
             */
            while (msgqueue->NumberOfTransactions == 0 && ((*comm)->Flags & OSPC_COMM_SHUTDOWN_BIT) != OSPC_COMM_SHUTDOWN_BIT) {
                OSPM_CONDVAR_WAIT(msgqueue->CondVar, msgqueue->Mutex, errorcode);

                /* assert(errorcode == OSPC_ERR_NO_ERROR); */
            }

            if (msgqueue->NumberOfTransactions > 0 && errorcode == OSPC_ERR_NO_ERROR) {
                /*
                 * release the mutex lock
                 */
                OSPM_MUTEX_UNLOCK(msgqueue->Mutex, tmperror);
                assert(tmperror == OSPC_ERR_NO_ERROR);

                /*
                 * send the new msg request to the HTTP connection
                 * module
                 */
                errorcode = OSPPHttpRequestHandoff((*comm), msgqueue);
            } else {
                /*
                 * NewTransactionsAllowed == OSPC_FALSE
                 *
                 * The provider has been deleted. See if a Timelimit
                 * has been specified to wait for pending transactions.
                 * If Timelimit < 0; wait indefinitely
                 *              > 0; wait for specified seconds
                 *             == 0; do not wait. terminate all connections immediately
                 */
                OSPPCommShutdownConnections((*comm), (*comm)->ShutdownTimeLimit);
                shutdown = OSPC_TRUE;
                /*
                 * release the mutex lock
                 */
                OSPM_MUTEX_UNLOCK(msgqueue->Mutex, tmperror);
                assert(tmperror == OSPC_ERR_NO_ERROR);
            }

            if (shutdown)
                break;
        }
    }

    /*
     * destroy the comm object. Make sure all http connections have been
     * freed.
     */
    while ((*comm)->HttpConnCount)
        OSPM_SLEEP(2);

    OSPPCommDelete(comm);

    OSPTTHREADRETURN_NULL();
}

/*
 * Starts the communication worker thread.
 * This function will create the new communication thread responsible
 * for monitoring the message queue for new requests.
 *
 * return OSPD_ERR_NO_ERROR if successful, else an error code.
 */
static int osppCommStartWorker(
    OSPTCOMM **ospvComm)        /* In - Pointer to the Comm Mgr */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTTHRATTR thread_attr;

    /*
     * verify Provider, Comm, and MsgQueue pointers
     */
    if (!(*ospvComm) || !(*ospvComm)->MsgQueue) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm/MsgQueue is null");
    } else {
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_THRATTR_INIT(thread_attr, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPM_SETDETACHED_STATE(thread_attr, errorcode);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * create the new thread which will monitor the
                     * the provider's message queue for new requests
                     */
                    OSPM_CREATE_THREAD((*ospvComm)->ThreadId, &thread_attr, osppCommMonitor, ospvComm, errorcode);
                }
                OSPM_THRATTR_DESTROY(thread_attr);
            }
        }
        if (errorcode != OSPC_ERR_NO_ERROR) {
            errorcode = OSPC_ERR_COMM_THREAD_INIT_FAILED;
            OSPM_DBGERRORLOG(errorcode, "thread initialization failed");
        }
    }

    return errorcode;
}

/*
 * create a new communication manager object.
 * This function creates the communication object which processes requests
 * to and from the TransNexus Service Points.
 *
 * return OSPC_ERR_NO_ERROR if successful, else error code
 */
int OSPPCommNew(
    OSPTCOMM **ospvComm)    /* In - Pointer to a Comm address */
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (*ospvComm != OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is not NULL");
    } else {
        /*
         * create a new communication manager
         */
        OSPM_MALLOC(*ospvComm, OSPTCOMM, sizeof(OSPTCOMM));
        if (*ospvComm == OSPC_OSNULL) {
            errorcode = OSPC_ERR_COMM_NO_MEMORY;
            OSPM_DBGERRORLOG(errorcode, "ospvComm malloc failed");
        } else {
            OSPM_MEMSET(*ospvComm, 0, sizeof(OSPTCOMM));

            /*
             * initialize the communication manager mutex
             */
            OSPM_MUTEX_INIT((*ospvComm)->Mutex, 0, errorcode);
            /* assert(errorcode == OSPC_ERR_NO_ERROR); */

            /*
             * Initialize the Mutex to used for Http Obj Selection
             */
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPM_MUTEX_INIT((*ospvComm)->HttpSelectMutex, 0, errorcode);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    OSPM_CONDVAR_INIT((*ospvComm)->HttpSelCondVar, NULL, errorcode);
                }
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /*
                 * create the message queue to the Communication Manager
                 */
                errorcode = OSPPMsgQueueNew(&(*ospvComm)->MsgQueue);
                if (errorcode != OSPC_ERR_NO_ERROR) {
                    OSPPCommDelete(ospvComm);
                } else {
                    /*
                     * start up the communication manager worker thread. This
                     * thread will monitor the queue for new requests and hand
                     * them off to an available HTTP connection pool for
                     * fulfillment.
                     */
                    errorcode = osppCommStartWorker(ospvComm);
                }
            }
        }
    }

    return errorcode;
}

void OSPPCommSetSecurity(
    OSPTCOMM *ospvComm,
    OSPTSEC *ospvSecurity)
{
    ospvComm->Security = ospvSecurity;
}

OSPTSEC *OSPPCommGetSecurity(
    OSPTCOMM *ospvComm)
{
    return ospvComm->Security;
}

int OSPPCommGetPersistence(
    OSPTCOMM *ospvComm,
    unsigned *ospvPersistence)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvPersistence = ospvComm->HttpPersistence;

    return errorcode;
}

int OSPPCommGetRetryDelay(
    OSPTCOMM *ospvComm,
    unsigned *ospvRetryDelay)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvRetryDelay = ospvComm->HttpRetryDelay;

    return errorcode;
}

int OSPPCommGetRetryLimit(
    OSPTCOMM *ospvComm,
    unsigned *ospvRetryLimit)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvRetryLimit = ospvComm->HttpRetryLimit;

    return errorcode;
}

int OSPPCommGetConnectTimeout(
    OSPTCOMM *ospvComm,
    unsigned *ospvTimeout)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvTimeout = ospvComm->ConnectTimeout;

    return errorcode;
}

int OSPPCommGetTimeout(
    OSPTCOMM *ospvComm,
    unsigned *ospvTimeout)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvTimeout = ospvComm->HttpTimeout;

    return errorcode;
}

int OSPPCommSetPersistence(
    OSPTCOMM *ospvComm,
    unsigned ospvPersistence)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->HttpPersistence = ospvPersistence;

    return errorcode;
}

int OSPPCommSetRetryDelay(
    OSPTCOMM *ospvComm,
    unsigned ospvRetryDelay)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->HttpRetryDelay = ospvRetryDelay;

    return errorcode;
}

int OSPPCommSetRetryLimit(
    OSPTCOMM *ospvComm,
    unsigned ospvRetryLimit)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->HttpRetryLimit = ospvRetryLimit;

    return errorcode;
}

int OSPPCommSetConnSelectionTimeout(
    OSPTCOMM *ospvComm,
    OSPTUINT64 ospvTimeout)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->ConnSelectionTimeout = ospvTimeout;

    return errorcode;
}

int OSPPCommSetConnectTimeout(
    OSPTCOMM *ospvComm,
    unsigned ospvTimeout)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->ConnectTimeout = ospvTimeout;

    return errorcode;
}

int OSPPCommSetTimeout(
    OSPTCOMM *ospvComm,
    unsigned ospvTimeout)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->HttpTimeout = ospvTimeout;

    return errorcode;
}

int OSPPCommGetMaxConnections(
    OSPTCOMM *ospvComm,
    unsigned *ospvMaxConnections)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvMaxConnections = ospvComm->HttpMaxConnections;

    return errorcode;
}

int OSPPCommSetMaxConnections(
    OSPTCOMM *ospvComm,
    unsigned ospvMaxConnections)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->HttpMaxConnections = ospvMaxConnections;

    return errorcode;
}

int OSPPCommIncrementHttpConnCount(
    OSPTCOMM *ospvComm)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->HttpConnCount++;

    return errorcode;
}

int OSPPCommDecrementHttpConnCount(
    OSPTCOMM *ospvComm)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        ospvComm->HttpConnCount--;

    return errorcode;
}

int OSPPCommGetHttpConnCount(
    OSPTCOMM *ospvComm,
    unsigned *ospvHttpConnCount)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvHttpConnCount = ospvComm->HttpConnCount;

    return errorcode;
}

void OSPPCommSetShutdown(
    OSPTCOMM **ospvComm,
    int ospvTimeLimit)
{
    int errcode = OSPC_ERR_NO_ERROR;

    (*ospvComm)->Flags |= OSPC_COMM_SHUTDOWN_BIT;
    (*ospvComm)->ShutdownTimeLimit = ospvTimeLimit;

    OSPM_CONDVAR_SIGNAL(OSPPMsgQueueGetCondVar((*ospvComm)->MsgQueue), errcode);

    while (*ospvComm != OSPC_OSNULL)
        OSPM_SLEEP(1);
}

void OSPPCommSignalAllConnections(
    OSPTCOMM *ospvComm)
{
    OSPTHTTP *httpconn = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm != OSPC_OSNULL) {
        /*
         * Lock COMM object while iterating though the list
         * of HTTP connections and signaling them to exit.
         */
        OSPM_MUTEX_LOCK(ospvComm->Mutex, errorcode);
        httpconn = (OSPTHTTP *)OSPPListFirst((OSPTLIST *)&(ospvComm->HttpConnList));
    }
    if (httpconn != OSPC_OSNULL) {
        do {
            OSPM_CONDVAR_SIGNAL(httpconn->CondVar, errorcode);

            if (!OSPPListEmpty((OSPTLIST *)&(ospvComm->HttpConnList))) {
                httpconn = (OSPTHTTP *)OSPPListNext((OSPTLIST *)&(ospvComm->HttpConnList), (void *)httpconn);
            } else {
                httpconn = OSPC_OSNULL;
            }
        } while (httpconn != OSPC_OSNULL);
    }

    if (ospvComm != OSPC_OSNULL) {
        /*
         * Finished iterating, unlocking...
         */
        OSPM_MUTEX_UNLOCK(ospvComm->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);
    }
}

void OSPPCommShutdownConnections(
    OSPTCOMM *ospvComm,
    int ospvTimeLimit)
{
    unsigned count = 0;
    OSPTBOOL do_forever = OSPC_TRUE;

    /*
     * check how many http connections are currently open
     */
    OSPPCommGetHttpConnCount(ospvComm, &count);
    if (count != 0) {
        /*
         * we've got open http connections
         */

        /*
         * sleep a specified time if requested
         */
        if (ospvTimeLimit > 0) {
            OSPM_SLEEP(ospvTimeLimit);
        } else {
            /*
             * if ospvTimeLimit is a negative number, sleep indefinitely.
             * Check connection status every 2 seconds
             */
            if (ospvTimeLimit < 0) {
                while (do_forever) {
                    OSPPCommGetHttpConnCount(ospvComm, &count);
                    if (count == 0)
                        break;

                    OSPM_SLEEP(2);
                }
            }
        }

        /*
         * make sure we still have connections
         */
        if (ospvComm->HttpConnCount) {
            /*
             * set the shutdown notification flag
             */
            ospvComm->Flags |= OSPC_COMM_HTTPSHUTDOWN_BIT;

            /*
             * signal each active HTTP connection
             */
            OSPPCommSignalAllConnections(ospvComm);
        }
    }
}

int OSPPCommGetNumberOfServicePoints(
    OSPTCOMM *ospvComm,
    unsigned *ospvNumberOfServicePoints)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        *ospvNumberOfServicePoints = ospvComm->NumberOfServicePoints;

    return errorcode;
}

void OSPPCommGetServicePointList(
    OSPTCOMM *ospvComm,
    OSPTSVCPT **ospvSvcPt)
{
    *ospvSvcPt = ospvComm->ServicePointList;
}

int OSPPCommBuildServicePoint(
    OSPTSVCPT *ospvSvcPt,
    char *ospvBuffer)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (!ospvSvcPt->HostName || !ospvSvcPt->URI) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else
        OSPM_SPRINTF(ospvBuffer, "%s://%s:%d%s",
            (ospvSvcPt->Flags & OSPC_COMM_SVCPT_SECURITY_BIT) ?
            "https" : "http", ospvSvcPt->HostName, ntohs(ospvSvcPt->Port), ospvSvcPt->URI);

    return errorcode;
}

int OSPPCommGetServicePoints(
    OSPTCOMM *ospvComm,
    unsigned ospvNumberOfServicePoints,
    unsigned ospvSizeOfServicePoint,
    char *ospvServicePoints[])
{
    unsigned count = 0;
    OSPTSVCPT *svcpt = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm == OSPC_OSNULL) {
        errorcode = OSPC_ERR_COMM_INVALID_ARG;
        OSPM_DBGERRORLOG(errorcode, "ospvComm is NULL");
    } else {
        for (count = 0,
             svcpt = (OSPTSVCPT *)OSPPListFirst((OSPTLIST *)&(ospvComm->ServicePointList));
             count < ospvComm->NumberOfServicePoints &&
             count < ospvNumberOfServicePoints;
             count++, svcpt = (OSPTSVCPT *)OSPPListNext((OSPTLIST *)&(ospvComm->ServicePointList), (void *)svcpt)) {
            if (svcpt == OSPC_OSNULL)
                break;

            if ((OSPM_STRLEN(svcpt->HostName) + OSPM_STRLEN(svcpt->URI)
                 + OSPC_COMM_SPARE_CHARS) > ospvSizeOfServicePoint) {
                errorcode = OSPC_ERR_COMM_INVALID_SIZE;
                count = 0;
                break;
            }

            errorcode = OSPPCommBuildServicePoint(svcpt, ospvServicePoints[count]);
            if (errorcode != OSPC_ERR_NO_ERROR) {
                count = 0;
                break;
            }
        }
    }
    while (count < ospvNumberOfServicePoints)
        ospvServicePoints[count++][0] = '\0';

    return errorcode;
}

static void osppCommDeleteServicePointList(
    OSPTSVCPT **svcpt,
    int ospvNumberOfServicePoints)
{
    int count = 0;
    OSPTSVCPT *deletesvcpt = OSPC_OSNULL;

    for (count = 0; count < ospvNumberOfServicePoints; count++) {
        deletesvcpt = (OSPTSVCPT *)OSPPListRemove((OSPTLIST *)svcpt);
        if (deletesvcpt != OSPC_OSNULL) {
            if (deletesvcpt->HostName)
                OSPM_FREE(deletesvcpt->HostName);

            if (deletesvcpt->URI)
                OSPM_FREE(deletesvcpt->URI);

            OSPM_FREE(deletesvcpt);
        }
    }
    OSPPListDelete((OSPTLIST *)svcpt);
}

/*
 * URLs have the following format
 *
 * http[s] : // FQDN [: port] / URI
 */
int OSPPCommParseSvcPt(
    const char *ospvURL,
    OSPTSVCPT **ospvSvcPt,
    unsigned ospvIndex)
{
#define OSPTMPBUFSZ    16

    int result = 0, hasport = 0, hasbracket = 0;
    OSPTSVCPT *svcpt = OSPC_OSNULL;
    char *curr = OSPC_OSNULL, *next = OSPC_OSNULL, TmpBuf[OSPTMPBUFSZ] = { "" };
    unsigned short secport = OSPC_COMM_DEFAULT_SECURE_PORT, nonsecport = OSPC_COMM_DEFAULT_NONSECURE_PORT;
    unsigned int length = 0;
    char ErrStr[100];

    if (ospvURL == OSPC_OSNULL) {
        OSPM_DBGERRORLOG(OSPC_ERR_COMM_PARSER, "URL is NULL");
        return OSPC_ERR_COMM_PARSER;
    }

    OSPM_MALLOC(svcpt, OSPTSVCPT, sizeof(OSPTSVCPT));
    if (svcpt == OSPC_OSNULL)
        return OSPC_ERR_COMM_NO_MEMORY;

    OSPM_MEMSET(svcpt, 0, sizeof(OSPTSVCPT));

    /*
     * Parse security flag bit
     *
     * -- START --
     */
    /* This is "http" or "https" */
    if ((result = OSPM_MEMCMP(ospvURL, "http", 4)) == 0) {

        if (ospvURL[4] == 's') {
            OSPM_DBGMISC(("Setting security bit 1...\n"));
            svcpt->Flags |= OSPC_COMM_SVCPT_SECURITY_BIT;
            curr = (char *)&ospvURL[5];
        } else {
            curr = (char *)&ospvURL[4];
        }
    } else if ((result = OSPM_MEMCMP(&ospvURL[0], "[", 1)) == 0) {

        /* This is raw IP address in brackets "[172.65.4.2]" */
        OSPM_DBGMISC(("Setting security bit 2...\n"));
        svcpt->Flags |= OSPC_COMM_SVCPT_SECURITY_BIT;
        curr = (char *)&ospvURL[1];
        hasbracket = 1;
    } else {
        /* This is either FQDN or error */
        OSPM_DBGMISC(("Setting security bit 3...\n"));
        svcpt->Flags |= OSPC_COMM_SVCPT_SECURITY_BIT;
        curr = (char *)&ospvURL[0];
    }

    /* -- END -- */


    if ((result = OSPM_MEMCMP(curr, "://", 3)) == 0) {
        curr += 3;
    }

    if ((result = OSPM_MEMCMP(curr, "[", 1)) == 0) {

        curr++;
        hasbracket = 1;
    }

    /*
     * Parse FQDN (Host Name)
     *
     * -- START --
     */
    /* first check for port, i.e. randr:4484 */
    next = strchr(curr, ':');
    if (!next) {
        /* now, check for url, i.e. ra/scripts/simulator.dll?handler */
        next = strchr(curr, '/');
        if (!next) {
            length = OSPM_STRLEN(curr);
            next = curr + length;
        }
    } else {
        hasport = 1;
    }

    OSPM_MALLOC(svcpt->HostName, char, next - curr + 1);
    if (!svcpt->HostName) {
        OSPM_FREE(svcpt);
        OSPM_DBGERRORLOG(OSPC_ERR_COMM_PARSER, "hostname syntax error");
        return OSPC_ERR_COMM_PARSER;
    }
    OSPM_MEMCPY(svcpt->HostName, curr, next - curr);
    svcpt->HostName[next - curr] = '\0';

    /* if bracket at the end, NULL it out */
    if (hasbracket) {
        svcpt->HostName[OSPM_STRLEN(svcpt->HostName) - 1] = '\0';
    }

    curr = next + (hasport ? 1 : 0);

    /* get ip address in network byte order */
    OSPPSockGetHostIP(svcpt->HostName, &(svcpt->IpAddr));
    if (!svcpt->IpAddr) {
        sprintf(ErrStr, "Host not found. Invalid Address: %s", svcpt->HostName);
        OSPM_DBGERRORLOG(OSPC_ERR_COMM_PARSER, ErrStr);
        OSPM_FREE(svcpt->HostName);
        OSPM_FREE(svcpt);
        return OSPC_ERR_COMM_PARSER;
    }

    /* -- END -- */

    /*
     * (optional)Parse Port Number
     *
     * -- START --
     */
    if (hasport) {
        /* now, check for url, i.e. ra/scripts/simulator.dll?handler */
        next = strchr(curr, '/');
        if (!next) {
            length = OSPM_STRLEN(curr);
            next = curr + length;
        }

        if (next - curr > OSPTMPBUFSZ) {
            OSPM_FREE(svcpt->HostName);
            OSPM_FREE(svcpt);
            OSPM_DBGERRORLOG(OSPC_ERR_COMM_PARSER, "port syntax error");
            return OSPC_ERR_COMM_PARSER;
        }

        OSPM_MEMCPY(TmpBuf, curr, next - curr);
        TmpBuf[next - curr] = '\0';
        svcpt->Port = htons((short) atoi(TmpBuf));
        curr = next;

    } else if (svcpt->Flags & OSPC_COMM_SVCPT_SECURITY_BIT) {
        svcpt->Port = htons(secport);
    } else {
        svcpt->Port = htons(nonsecport);
    }

    /* -- END -- */

    /*
     * Load URI
     *
     * -- START --
     */
    OSPM_MALLOC(svcpt->URI, char, OSPM_STRLEN(curr) + 1);
    if (svcpt->URI == OSPC_OSNULL) {
        OSPM_FREE(svcpt->HostName);
        OSPM_FREE(svcpt);
        OSPM_DBGERRORLOG(OSPC_ERR_COMM_PARSER, "URI is OSPC_OSNULL");
        return OSPC_ERR_COMM_PARSER;
    }
    OSPM_MEMCPY(svcpt->URI, curr, OSPM_STRLEN(curr));
    svcpt->URI[OSPM_STRLEN(curr)] = '\0';

    /* -- END -- */

    svcpt->Index = ospvIndex;
    *ospvSvcPt = svcpt;

    return OSPC_ERR_NO_ERROR;
}

int OSPPCommSetAuditURL(
    OSPTCOMM *ospvComm,
    const char *ospvAuditURL)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvComm != OSPC_OSNULL) {
        errorcode = OSPPCommParseSvcPt(ospvAuditURL, &ospvComm->AuditURL, 0);

        if (errorcode != OSPC_ERR_NO_ERROR) {
            if (ospvComm->AuditURL) {
                if (ospvComm->AuditURL->HostName)
                    OSPM_FREE(ospvComm->AuditURL->HostName);

                if (ospvComm->AuditURL->URI)
                    OSPM_FREE(ospvComm->AuditURL->URI);

                OSPM_FREE(ospvComm->AuditURL);
            }
        }
    }

    return errorcode;
}

void OSPPCommSetAuditFlag(
    OSPTCOMM *ospvComm,
    unsigned ospvAuditFlag)
{
    if (ospvComm != OSPC_OSNULL) {
        if (ospvAuditFlag == OSPC_COMM_AUDIT_ON)
            ospvComm->Flags = (unsigned char)(ospvComm->Flags | ospvAuditFlag);
        else if (ospvAuditFlag == OSPC_COMM_AUDIT_OFF)
            ospvComm->Flags = (unsigned char)(ospvComm->Flags & ospvAuditFlag);
    }
}

/*
 * Validate Input - at least one valid URL is required
 * Signal to all current connections to exit, and then set
 * the new set of URLs
 */
int OSPPCommUpdateURLs(OSPTCOMM *ospvCommManager,   /* In - Provider handle     */
    unsigned ospvNumberOfURLs,                      /* In - New svc url cnt     */
    const char *ospvURLs[])                         /* In - New svc url strings */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    int timeLimit = 0;
    unsigned char origFlags = ospvCommManager->Flags;

    /*
     * Check input parameters, we want at least one URL
     */
    if (ospvNumberOfURLs <= 0 || ospvURLs == OSPC_OSNULL) {
        errorcode = OSPC_ERR_PROV_INVALID_VALUE;
        OSPM_DBGERRORLOG(errorcode, "Invalid input for OSPPCommUpdateURLs");
        OSPM_DBGERRORLOG(errorcode, "At least one URLs is required");
    }

    /*
     * Make sure the URLs are valid
     */
    if (OSPC_ERR_NO_ERROR == errorcode) {
        errorcode = OSPPCommValidateSvcPts(ospvNumberOfURLs, ospvURLs);
    }

    /*
     * Signal to the manager to shut down, wait for the operation to complete,
     * and then update the service points
     */
    if (OSPC_ERR_NO_ERROR == errorcode) {
        OSPPCommShutdownConnections(ospvCommManager, timeLimit);

        while (ospvCommManager->HttpConnCount > 0) {
            /* OSPM_DBGMISC(( "waiting another sec...\n" )); */
            OSPM_SLEEP(1);
        }

        /*
         * All HTTP connection have exited, restore the Flag
         * It was set to SHUTDOWN state by a call to OSPPCommShutdownConnections
         */
        ospvCommManager->Flags = origFlags;

        errorcode = OSPPCommSetServicePoints(ospvCommManager, ospvNumberOfURLs, ospvURLs);
    }

    return errorcode;
}

int OSPPCommSetServicePoints(
    OSPTCOMM *ospvComm,
    unsigned ospvNumberOfServicePoints,
    const char **ospvServicePoint)
{
    unsigned count = 0;
    OSPTSVCPT *newsvcpt = OSPC_OSNULL, *newroot = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * construct the new service point list
     */
    OSPPListNew((OSPTLIST *)&newroot);

    for (count = 0; count < ospvNumberOfServicePoints; count++) {

        errorcode = OSPPCommParseSvcPt(ospvServicePoint[count], &newsvcpt, count + 1);

        if (errorcode != OSPC_ERR_NO_ERROR)
            break;

        OSPPListAppend((OSPTLIST *)&newroot, (void *)newsvcpt);
    }

    /*
     * if memory has been allocated and a parse error has
     * occurred, free the partial list.
     */
    if (errorcode != OSPC_ERR_NO_ERROR)
        osppCommDeleteServicePointList(&newroot, count);
    else {
        /*
         * If the new list has been successfully constructed,
         * free the old one and assign the new one.
         */
        osppCommDeleteServicePointList(&(ospvComm->ServicePointList), ospvComm->NumberOfServicePoints);

        ospvComm->ServicePointList = newroot;
        ospvComm->NumberOfServicePoints = count;
    }

    return errorcode;
}

void OSPPCommDelete(
    OSPTCOMM **ospvComm)
{
    int errcode = 0;

    /*
     * check the communication manager first:
     */
    if (ospvComm == OSPC_OSNULL) {
        OSPM_DBGMISC(("%s%s", "Warning; attempted to delete null reference to ", "communications manager.\n"));
        return;
    }

    if (*ospvComm == OSPC_OSNULL) {
        OSPM_DBGMISC(("Warning: attempted to delete null communications manager.\n"));
        return;
    }

    /*
     * destroy service point list
     */
    osppCommDeleteServicePointList(&(*ospvComm)->ServicePointList, (*ospvComm)->NumberOfServicePoints);

    /*
     * destroy AuditURL, if necessary:
     */
    if ((*ospvComm)->AuditURL != OSPC_OSNULL) {
        if ((*ospvComm)->AuditURL->HostName)
            OSPM_FREE((*ospvComm)->AuditURL->HostName);

        if ((*ospvComm)->AuditURL->URI)
            OSPM_FREE((*ospvComm)->AuditURL->URI);

        OSPM_FREE((*ospvComm)->AuditURL);
    }

    /*
     * destroy the message queue object
     */
    OSPPMsgQueueDelete(&(*ospvComm)->MsgQueue);

    /*
     * Destroy the mutex and the condition variable.
     */
    OSPM_MUTEX_DESTROY((*ospvComm)->HttpSelectMutex, errcode);
    OSPM_CONDVAR_DESTROY((*ospvComm)->HttpSelCondVar, errcode);

    /*
     * destroy the comm object
     */
    OSPM_FREE(*ospvComm);
    *ospvComm = OSPC_OSNULL;
}

int OSPPCommValidateSvcPts(
    unsigned ospvNumberOfServicePoints,
    const char **ospvServicePoint)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned count = 0;
    OSPTSVCPT *svcpt = OSPC_OSNULL;

    /*
     * Iterate through all Svc points and see if we can parse them
     */
    for (count = 0; count < ospvNumberOfServicePoints; count++) {
        errorcode = OSPPCommParseSvcPt(ospvServicePoint[count], &svcpt, count + 1);

        /*
         * Now free the Svc point
         */
        if (svcpt != OSPC_OSNULL) {
            if (svcpt->HostName)
                OSPM_FREE(svcpt->HostName);

            if (svcpt->URI)
                OSPM_FREE(svcpt->URI);

            OSPM_FREE(svcpt);
        }

        /* Break on error */
        if (errorcode != OSPC_ERR_NO_ERROR)
            break;
    }

    return errorcode;
}

int OSPPCommGetNumberOfTransactions(OSPTCOMM *ospvComm, unsigned *ospvNumberOfTransactions)
{
    int errorcode = OSPPMsgQueueGetNumberOfTransactions(ospvComm->MsgQueue, ospvNumberOfTransactions);
    return errorcode;
}
