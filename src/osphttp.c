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
 * osphttp.c 
 */
#include "osp.h"
#include "osphttp.h"
#include "ospsocket.h"
#include "osplist.h"
#include "ospcomm.h"
#include "ospmsgque.h"
#include "ospmsginfo.h"
#include "ospconfig.h"
#include "osputils.h"

int
OSPPHttpNew(
    OSPTCOMM *ospvComm,
    OSPTHTTP **ospvHttp)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPHttpNew()\n"));
    /*
     * create space for the new HTTP connection object
     */
    OSPM_MALLOC(*ospvHttp, OSPTHTTP, sizeof(OSPTHTTP));
    if (*ospvHttp != (OSPTHTTP *)OSPC_OSNULL) 
    {

        /*
         * initialize HTTP connection object space
         */
        OSPM_MEMSET(*ospvHttp, 0, sizeof(OSPTHTTP));

        /*
         * initialize the HTTP connection object mutex
         */
        OSPM_MUTEX_INIT((*ospvHttp)->Mutex, NULL, errorcode);
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            OSPPHttpDelete(ospvHttp);
        }

        /*
         * initialize the HTTP connection conditional variable
         */

        OSPM_CONDVAR_INIT((*ospvHttp)->CondVar, OSPC_OSNULL, errorcode);
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            OSPPHttpDelete(ospvHttp);
        }

        /*
         * initialize the HTTP connection message queue list
         */
        OSPPListNew((OSPTLIST *)&((*ospvHttp)->MsgInfoList));

        /*
         * save the pointer to the Comm Mgr so that the communication
         * attribute fields (timeout, retrydelay, retrylimit, etc...)
         * can be accessed from within the HTTP connection i/o functions
         */
        (*ospvHttp)->Comm = (void *)ospvComm;


        /*
         * initialize the Socket File Descriptor to -1. 
         * This indicates that a connection has never been established.
         */
        (*ospvHttp)->SockFd = OSPC_SOCK_INVALID;

    }
    OSPM_DBGEXIT(("EXIT : OSPPHttpNew()\n"));
    return errorcode;
}

OSPTSEC *
    OSPPHttpGetSecurity(
    OSPTHTTP *ospvHttp)
{
    OSPM_DBGENTER(("ENTER: OSPPHttpGetSecurity()\n"));
    OSPM_DBGEXIT(("EXIT : OSPPHttpGetSecurity()\n"));
    return OSPPCommGetSecurity((OSPTCOMM *)ospvHttp->Comm);
}

void
OSPPHttpDelete(
    OSPTHTTP **ospvHttp)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPHttpDelete()\n"));
    if (*ospvHttp)
    {
        OSPM_CONDVAR_DESTROY((*ospvHttp)->CondVar, errorcode);
        OSPM_MUTEX_DESTROY((*ospvHttp)->Mutex, errorcode);

        (void)OSPPSockClose(OSPC_TRUE, &((*ospvHttp)->SockFd), &((*ospvHttp)->SSLSession));

    }
    OSPM_FREE(*ospvHttp);
    *ospvHttp = OSPC_OSNULL;
    OSPM_DBGEXIT(("EXIT : OSPPHttpDelete()\n"));
    return;
}

static
int
osppHttpBuildMsg(
    unsigned char **ospvHttpMsg,
    unsigned      *ospvHttpMsgSz,
    unsigned char *ospvRequestMsg,
    unsigned      ospvRequestSz, 
    OSPTSVCPT     *ospvSvcPt,
    unsigned char *ospvContentType,
    unsigned      ospvContentTypeSz)
{
    int  headersz  = 0,
         errorcode = OSPC_ERR_NO_ERROR;
    char *header   = OSPC_OSNULL;

    OSPM_DBGENTER(("ENTER: osppHttpBuildMsg()\n"));

    /*
     * determine the maximum size of the HTTP header
     */
    headersz  = OSPM_STRLEN(ospvSvcPt->URI) + OSPM_STRLEN(ospvSvcPt->HostName) +
        ospvContentTypeSz + OSPM_STRLEN(OSPC_HTTP_HEADER_MSG_FMT) + 1;

    /*
     * create space for the header
     */
    OSPM_MALLOC(header, char, headersz);
    if (header != OSPC_OSNULL)
    {
        /*
         * format the header with the information from the Service Point 
         * structure
         */
        OSPM_SPRINTF(header, OSPC_HTTP_HEADER_MSG_FMT, ospvSvcPt->URI, 
            ospvSvcPt->HostName, ospvContentType, ospvRequestSz);

        /*
         * adjust headersz to be exact size
         */
        headersz = OSPM_STRLEN(header);
        *ospvHttpMsgSz = headersz + ospvRequestSz;

        /*
         * create space for the entire message
         */

        OSPM_MALLOC(*ospvHttpMsg, unsigned char, *ospvHttpMsgSz + 1);
        if (*ospvHttpMsg != OSPC_OSNULL)
        {
            /* Put a null character at the end */
            OSPM_MEMSET(*ospvHttpMsg, 0, *ospvHttpMsgSz + 1);

            /*
             * copy in the header
             */
            OSPM_MEMCPY(*ospvHttpMsg, header, headersz);

            /*
             * copy in the body
             */
            OSPM_MEMCPY(((*ospvHttpMsg) + headersz), ospvRequestMsg, 
                ospvRequestSz);

            /*
             * free the temporary header string
             */
            OSPM_FREE(header);
        }
    }

    if (errorcode != OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPC_ERR_HTTP_BAD_MESSAGE;
        OSPM_DBGERRORLOG(errorcode, "http message could not be built");
    }
    OSPM_DBGEXIT(("EXIT : osppHttpBuildMsg()\n"));
    return errorcode;
}

static
void
osppHttpRemoveConnection(
    OSPTHTTP *ospvHttp)
{
    OSPTCOMM *comm     = OSPC_OSNULL;
    int      errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: osppHttpRemoveConnection()\n"));

    comm = (OSPTCOMM *)ospvHttp->Comm;

    /*
     * acquire the comm mgr mutex so that we can modify
     * the http connection data exclusively
     */

    if (comm != (OSPTCOMM *)OSPC_OSNULL)
    {
        OSPM_MUTEX_LOCK(comm->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

        /*
         * remove the connection from the http connection list
         */
        (void)OSPPListRemoveSpecificItem((OSPTLIST *)&(comm->HttpConnList),
            ospvHttp);

        /*
         * decrement the number of http connections
         */
        (void)OSPPCommDecrementHttpConnCount(comm);

        OSPM_MUTEX_UNLOCK(comm->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

    }
    OSPM_DBGEXIT(("EXIT : osppHttpRemoveConnection()\n"));
    return;
}

static
int
osppHttpBuildAndSend(
    OSPTHTTP      *ospvHttp,
    OSPTMSGINFO   *ospvMsginfo)
{
    int            errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *httpmsg   = OSPC_OSNULL;
    unsigned       httpmsgsz = 0;

    OSPM_DBGENTER(("ENTER: osppHttpBuildAndSend() ospHttp = <%lx>\n",
        (unsigned long)ospvHttp));

    errorcode = osppHttpBuildMsg(&httpmsg, &httpmsgsz,
        ospvMsginfo->RequestMsg, ospvMsginfo->RequestSz,
        ospvHttp->ServicePoint, ospvMsginfo->ContentType, 
        OSPM_STRLEN((const char *)ospvMsginfo->ContentType));

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPM_DBGNET(("MISC : osppHttpBuildAndSend() sending\n[%s]\n",
            httpmsg));
        /*
         * send the new msg request to the Service
         * Point. place the response in the ospvMsginfo
         * structure. Also get the Content-Type and
         * related size for the MIME parser
         */
        OSPPSockProcessRequest(ospvHttp,
            httpmsg, httpmsgsz,
            &(ospvMsginfo->ResponseMsg),
            &(ospvMsginfo->ResponseSz),
            &(ospvMsginfo->ContentType),
            &(ospvMsginfo->ContentSz),
            &errorcode);
    }

    if (httpmsg != OSPC_OSNULL)
        OSPM_FREE(httpmsg);

    OSPM_DBGEXIT(("EXIT : osppHttpBuildAndSend()\n"));
    return errorcode;
}

/*
 * monitors the message queue for new HTTP requests.
 * When a new request is placed on the HTTP message queue, the conditional 
 * variable (CondVar) within the HTTP connection structure is signalled and
 * the number of transactions (NumberOfTransactions) will be incremented. 
 * This function will awake and assign the request to an available HTTP 
 * connection. Upon the handoff of the request, this function will return to
 * the wait condition until the next message queue request arrives or until
 * the HTTP connection persistence expires.
 *
 * returns void.
 */
static
OSPTTHREADRETURN
osppHttpSetupAndMonitor(
    void *ospvArg)  /* In - HTTP pointer casted to a void */
{
    OSPTHTTP      *httpconn          = OSPC_OSNULL;
    OSPTMSGINFO   *msginfo           = OSPC_OSNULL;
    OSPTCOMM      *comm              = OSPC_OSNULL;
    int           errorcode          = OSPC_ERR_NO_ERROR,
                  tmperror           = OSPC_ERR_NO_ERROR;
    unsigned      attempts           = 0,
                  retrylimit         = 0,
                  retrydelay         = 0,
                  persistancetimeout = 0;
    OSPTBOOL      connected          = OSPC_FALSE,
        rollover           = OSPC_FALSE,
        do_forever         = OSPC_TRUE;

    OSPM_DBGENTER(("ENTER: osppHttpSetupAndMonitor() ospHttp = <%lx>\n",
        (unsigned long)ospvArg));
    httpconn = (OSPTHTTP *)ospvArg;
    while (do_forever)
    {
        OSPM_DBGNET(("MISC : osppHttpSetupAndMonitor() monitor start\n"));

        msginfo = (OSPTMSGINFO *)OSPC_OSNULL;
        comm    = (OSPTCOMM *)httpconn->Comm;

        /*
         * acquire message queue mutex
         */
        OSPM_MUTEX_LOCK(httpconn->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /*
             * wait for conditional variable on the transaction count
             */
            while (httpconn->NumberOfTransactions == 0 &&
                errorcode != OSPC_ERR_OS_CONDVAR_TIMEOUT &&
                (comm && !(comm->Flags & OSPC_COMM_HTTPSHUTDOWN_BIT)))
            {
                if (comm != (OSPTCOMM *)OSPC_OSNULL)
                    (void)OSPPCommGetPersistence(comm, &persistancetimeout);

                OSPM_DBGNET(("MISC : osppHttpSetupAndMonitor() before timed wait. %d seconds\n",  
                    persistancetimeout));

                OSPM_CONDVAR_TIMEDWAIT(httpconn->CondVar,
                    httpconn->Mutex, persistancetimeout, errorcode);

                assert(errorcode == OSPC_ERR_NO_ERROR ||
                    errorcode == OSPC_ERR_OS_CONDVAR_TIMEOUT);
            }

            OSPM_DBGNET(("MISC : osppHttpSetupAndMonitor() triggered event occurred\n"));

            /*
             * see if the connection has timed out due to inactivity
             * (HttpPersistance). If the connection has timed out, 
             * free up all the resources associated with this connection
             * and exit the thread gracefully. Also, if the provider has
             * been deleted and the Shutdown Flag has been set, shutdown this
             * connection.
             */
            if ((httpconn->NumberOfTransactions == 0 &&
                errorcode == OSPC_ERR_OS_CONDVAR_TIMEOUT) ||
                (comm == (OSPTCOMM *)OSPC_OSNULL || 
                (comm->Flags & OSPC_COMM_HTTPSHUTDOWN_BIT) == OSPC_COMM_HTTPSHUTDOWN_BIT))
            {
                OSPM_DBGNET(("MISC : osppHttpSetupAndMonitor() remove connection requested\n"));
                OSPM_MUTEX_UNLOCK(httpconn->Mutex, errorcode);
                osppHttpRemoveConnection(httpconn);
                break;
            }

            /*
             * get the msginfo item from the queue
             */
            msginfo = (OSPTMSGINFO *)OSPPListFirst(
                (OSPTLIST *)&(httpconn->MsgInfoList));

            if (msginfo == (OSPTMSGINFO *)OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_HTTP_BAD_QUEUE;
                OSPM_DBGERRORLOG(errorcode, "http msg queue corrupted");
            }
            else
            {
                if(msginfo->Flags & OSPC_MSGINFO_AUDIT_TYPE)
                {
                    /* set up retry delay and retry limit */
                    retrydelay = OSPC_AUDIT_RETRY_DELAY;
                    retrylimit = OSPC_AUDIT_RETRY_LIMIT;
                }
                else
                {

                    /*
                     * get the current retry delay and retry limit values
                     */
                    OSPPCommGetRetryDelay(comm, &retrydelay);
                    OSPPCommGetRetryLimit(comm, &retrylimit);
                }

                attempts = 0;
                rollover = OSPC_FALSE;

                /* -----------------------------------------------------
                 * this loop will connect to a service point and attempt
                 * to send a request. If the service point is unavailable,
                 * then the next service point will be tried. If all service
                 * points are exhausted then the list will be tried again 
                 * based on the value of retrylimit. if retrylimit is 1, 
                 * then 2 transaction attempts will be performed for each
                 * service point until completed failed.
                 * ------------------------------------------------------
                 */
                while (attempts <= retrylimit && 
                    (comm->Flags & OSPC_COMM_HTTPSHUTDOWN_BIT) == 0)
                {
                    errorcode = OSPC_ERR_NO_ERROR;

                    OSPM_DBGMISC(
                        ("MISC : osppHttpSetupAndMonitor() connected = <%s>\n",
                        (connected == OSPC_TRUE ? "TRUE" : "FALSE")));

                    /* AuditURL should never be connected since audit thread
                     * dies after every send.
                     */
                    if(msginfo->Flags & OSPC_MSGINFO_AUDIT_TYPE)
                    {

                        /* try auditurl */

                        errorcode = OSPPSockConnectAuditURL(httpconn, &connected);
                        
                        rollover = OSPC_TRUE;
                        attempts++;

                    }
                    else
                    {
                        if (!connected)
                        {
                            /* try service point list */
                            errorcode = OSPPSockConnectServicePoint(httpconn,
                               &rollover, &connected);

                            if (rollover)
                               attempts++;
                        }
                    }

                    if (connected == OSPC_TRUE && errorcode == OSPC_ERR_NO_ERROR)
                    {
                        errorcode = osppHttpBuildAndSend(httpconn, msginfo);
                        /*
                         * Successful send to service point
                         */
                        if (errorcode == OSPC_ERR_NO_ERROR)
                        {
                            time(&httpconn->LastTransactionTime);
                            break; 
                        }
                        else
                        {
                            connected = OSPC_FALSE;
                        }
                    }

                    if (rollover && retrydelay && retrylimit &&
                        attempts <= retrylimit)
                    { 
                        OSPM_SLEEP(retrydelay);
                        rollover = OSPC_FALSE;
                    }

                    OSPM_DBGNET(("MISC : attempt iteration %d limit = %d\n", 
                        attempts+1, retrylimit));
                }

                OSPM_DBGNET(("MISC : osppHttpSetupAndMonitor() monitor exit. errorcode = %d\n", errorcode));

                if (errorcode != OSPC_ERR_NO_ERROR)
                    msginfo->ErrorCode = errorcode;

                if (errorcode == OSPC_ERR_NO_ERROR && 
                    attempts < retrylimit &&
                    comm->Flags & OSPC_COMM_HTTPSHUTDOWN_BIT)

                    msginfo->ErrorCode = OSPC_ERR_HTTP_SHUTDOWN;

                /*
                 * remove the processed msg from the queue
                 */
                msginfo = (OSPTMSGINFO *)OSPPListRemove(
                    (OSPTLIST *)&(httpconn->MsgInfoList));

                if (msginfo == (OSPTMSGINFO *)OSPC_OSNULL)
                {
                    errorcode = OSPC_ERR_HTTP_BAD_QUEUE;
                    OSPM_DBGERRORLOG(errorcode, "http msg queue corrupted");
                }

                /*
                 * decrement the number of transactions on
                 * the queue
                 */
                httpconn->NumberOfTransactions--;

                OSPM_DBGNET(("MISC : osppHttpSetupAndMonitor() msg dequeued. queue = %d\n",  
                    httpconn->NumberOfTransactions));
            }

            /*
             * release the mutex lock
             */
            OSPM_MUTEX_UNLOCK(httpconn->Mutex, tmperror);
            assert(tmperror == OSPC_ERR_NO_ERROR);

            /*
             * now signal the application thread that
             * we've got a response for it.
             */
            if (msginfo != (OSPTMSGINFO *)OSPC_OSNULL)
            {
                errorcode = OSPPMsgInfoProcessResponse(msginfo);
                assert(errorcode == OSPC_ERR_NO_ERROR);
            }
        }

        if ( OSPC_SOCK_INVALID == httpconn->SockFd )
        {
          /*
           * Connection is no longer valid
           * time to quit this thread
           */
          OSPM_DBGNET(("MISC : osppHttpSetupAndMonitor() remove connection requested\n"));
          OSPM_MUTEX_UNLOCK(httpconn->Mutex, errorcode);
          osppHttpRemoveConnection(httpconn);
          break;
        }
    }

    /*
     * finally clean up the memory for the connection
     */
    OSPPHttpDelete(&httpconn);

    comm->Flags &= ~OSPC_COMM_HTTPSHUTDOWN_BIT;

    OSPM_DBGEXIT(("EXIT : osppHttpSetupAndMonitor()\n"));

    OSPTTHREADRETURN_NULL();
}

int
OSPPHttpVerifyResponse(
    char     *ospvResponse,
    int      *ospvResponseType)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /* Before verifying the response, change all the characters
     * to lowercase. 
     */
    if(ospvResponse != OSPC_OSNULL)
    {
        OSPPUtilStringToLowercase(&ospvResponse);
    }

    /* 
     * If we have service unavailable, just return an errorcode. Don't
     * be concerned with response type.
     */

		/*
		 * Try to determine the response type. Anything other than
		 * 1xx or 2xx will be considered and error.
		 */


		if (OSPM_STRSTR(ospvResponse, OSPC_HTTP_200_OK) != (char *)OSPC_OSNULL)
		{
				*ospvResponseType = 200;
		}
    else if (OSPM_STRSTR(ospvResponse, OSPC_HTTP_400_BAD_REQUEST) != (char *)OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_HTTP_BAD_REQUEST;
				OSPM_DBGERRORLOG(errorcode, "HTTP Status: 400 Bad Request");
    }
    else if (OSPM_STRSTR(ospvResponse, OSPC_HTTP_401_UNAUTHORIZED) != (char *)OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_HTTP_UNAUTHORIZED;
				OSPM_DBGERRORLOG(errorcode, "HTTP Status: 401 Unauthorized");
    }
    else if (OSPM_STRSTR(ospvResponse, OSPC_HTTP_404_NOT_FOUND) != (char *)OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_HTTP_NOT_FOUND;
				OSPM_DBGERRORLOG(errorcode, "HTTP Status: 404 Not Found");
    }
    else if (OSPM_STRSTR(ospvResponse, OSPC_HTTP_503_SERV_UNAVAIL) != (char *)OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_HTTP_SERVICE_UNAVAILABLE;
				OSPM_DBGERRORLOG(errorcode, "HTTP Status: 503 Service Unavailable");
    }
    else if (OSPM_STRSTR(ospvResponse, OSPC_HTTP_100_CONTINUE) != (char *)OSPC_OSNULL)
		{
				*ospvResponseType = 100;
		}
		else
		{
				errorcode = OSPC_ERR_HTTP_SERVER_ERROR;
		}

    return errorcode;
}

/*
 * Starts the HTTP worker thread.
 * This function will create the new HTTP thread responsible
 * for monitoring the HTTP message queue for new requests.
 *
 * return OSPC_ERR_NO_ERROR if successful, else an error code.
 */
static
int
osppHttpStartWorker(
    OSPTHTTP  *ospvHttp)     /* In - Pointer to the HTTP connection */
{
    int         errorcode = OSPC_ERR_NO_ERROR;
    OSPTTHRATTR thread_attr;

    OSPM_DBGENTER(("ENTER: osppHttpStartWorker\n"));

    /*
     * verify HTTP pointer
     */
    assert(ospvHttp != OSPC_OSNULL);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPM_THRATTR_INIT(thread_attr, errorcode);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPM_SETDETACHED_STATE(thread_attr, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                /*
                 * create the new thread which will monitor the
                 * the HTTP connection's message queue for new requests
                 */
                OSPM_CREATE_THREAD(ospvHttp->ThreadId, &thread_attr,
                    osppHttpSetupAndMonitor, ospvHttp, errorcode);
            }
            OSPM_THRATTR_DESTROY(thread_attr);
        }
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPC_ERR_COMM_THREAD_INIT_FAILED;
            OSPM_DBGERRORLOG(errorcode, "thread initialization failed");
        }
    }
    OSPM_DBGEXIT(("EXIT : osppHttpStartWorker\n"));
    return errorcode;
}

static
OSPTHTTP *
osppHttpIdleCheck(
    OSPTHTTP **ospvHttpList,
    OSPTHTTP *ospvHttp)
{
    OSPM_DBGENTER(("ENTER : osppHttpIdleCheck\n"));
    do
    {
        if (ospvHttp->NumberOfTransactions == 0)
            break;

        ospvHttp = (OSPTHTTP *)OSPPListNext((OSPTLIST *)ospvHttpList,
            (void *)ospvHttp);

    } while (ospvHttp != (OSPTHTTP *)OSPC_OSNULL && 
        ospvHttp->NumberOfTransactions);

    OSPM_DBGEXIT(("EXIT : osppHttpIdleCheck\n"));
    return ospvHttp;
}

static
int
osppHttpMinTransCheck(
    OSPTHTTP **ospvHttpList,
    OSPTHTTP **ospvHttp,
    unsigned timeout)
{
    OSPTTIME     startTime        = 0L;
    OSPTTIME     currentTime      = 0L;
    unsigned int startMilliTime   = 0;
    unsigned int currentMilliTime = 0;
    unsigned int elapsedTime      = 0;

    int      errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER : osppHttpMinTransCheck\n"));

    /* Get a reference time */
    errorcode = OSPPOSTimeGetTime(&startTime, &startMilliTime);

    while( (errorcode == OSPC_ERR_NO_ERROR) && (*ospvHttp == (OSPTHTTP *)OSPC_OSNULL) )
    {
            /* Get the first item in the queue */
            *ospvHttp = (OSPTHTTP *)OSPPListFirst((OSPTLIST *)ospvHttpList);
            if(*ospvHttp == OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_HTTP_BAD_QUEUE;
                OSPM_DBGERRORLOG(errorcode, "http msg queue corrupted");
                break;
            }

            /* Keep the search going until it gets to the end of the list */
            while (*ospvHttp != (OSPTHTTP *)OSPC_OSNULL)
            {

                /* If this item in the list has an empty queue... */
                if ((*ospvHttp)->NumberOfTransactions == 0)
                {
                    /* ...break out of the while loop with ospvHttp pointing */
                    /* to the item */
                    break;
                }

                /* ... if it is not empty, get the next item in the list */
                *ospvHttp = (OSPTHTTP *)OSPPListNext((OSPTLIST *)ospvHttpList,
                   (void *)*ospvHttp);
            }

            /* Get the current time */
            errorcode = OSPPOSTimeGetTime(&currentTime, &currentMilliTime);

            /* Check to make sure there are no problems... */
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                /* ...if no problems, calculate the elapsed time in millisieconds */
                elapsedTime = ((currentTime - startTime) * 1000) + ((int)currentMilliTime - (int)startMilliTime);

                /* Check for time out */
                if(elapsedTime >= timeout)
                {
                  errorcode = OSPC_ERR_HTTP_CONN_SRCH_TIMEOUT;
                  OSPM_DBGERRORLOG(errorcode, "search for connection timedout");
                }
            }
    }

    OSPM_DBGEXIT(("EXIT : osppHttpMinTransCheck\n"));
    return errorcode;
}

static
int
osppHttpSelectConnection(
    OSPTCOMM        *ospvComm,
    OSPTHTTP        **ospvHttp,
    unsigned char   ospvMsgInfoType)
{
    OSPTBOOL createnew  = OSPC_FALSE;
    unsigned httpcount  = 0,
             maxcount   = 0;
    int      errorcode  = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER : osppHttpSelectConnection\n"));

    /*
     * HTTP selection criteria:
     *
     * a new connection will be created if one of the following
     * conditions is TRUE:
     *
     * number of connections == 0
     * number of idle connections == 0 && 
     *     number of connections < max connections
     *
     * a connection is reused if one of the following conditions is TRUE:
     *
     * number of idle connections > 0
     * number of connections == max connections
     */

    /*
     * first see if any connections exist
     */
    *ospvHttp = (OSPTHTTP *)OSPPListFirst(
        (OSPTLIST *)&(ospvComm->HttpConnList));
    if (*ospvHttp == (OSPTHTTP *)OSPC_OSNULL)
    {
        /* no current connections */
        createnew = OSPC_TRUE;
    }
    else
    {
        /*
         * check for idle connections. The first idle HTTP connection
         * will be returned. If NULL is returned, all queues are busy.
         */
        *ospvHttp = osppHttpIdleCheck(&(ospvComm->HttpConnList), *ospvHttp);
        if (*ospvHttp == (OSPTHTTP *)OSPC_OSNULL)
        {
            /*
             * no connections are idle. All are in use
             */

            /*
             * see if we can add a new one
             */
            (void)OSPPCommGetHttpConnCount(ospvComm, &httpcount);
            (void)OSPPCommGetMaxConnections(ospvComm, &maxcount);
            if (httpcount < maxcount)
            {
                createnew = OSPC_TRUE;
            }
            else
            {
                /*
                 * connection limit has been reached. Find a queue with no
                 * transactions and use it; otherwise, wait for a queue to
                 * become empty.  The call may time out.
                 */

                errorcode = osppHttpMinTransCheck(&(ospvComm->HttpConnList),
                    &(*ospvHttp), ospvComm->HttpTimeout);
            }
        }
        else
        {
            /* 
             * check connection type. make sure it syncs up with the type of
             * msginfo we are about to send.
             */
            if ((ospvMsgInfoType & OSPC_MSGINFO_AUDIT_TYPE) == 0)
            {
                if (((*ospvHttp)->Flags & OSPC_HTTP_AUDIT_TYPE) == OSPC_HTTP_AUDIT_TYPE)
                       createnew = OSPC_TRUE;
            }
            else
            {
                if (((*ospvHttp)->Flags & OSPC_HTTP_AUDIT_TYPE) != OSPC_HTTP_AUDIT_TYPE)
                       createnew = OSPC_TRUE;
            } 
        }
    }

    if (createnew)
    {
        /*
         * create the new http connection object. In doing so,
         * setup a persistance socket to the selected Service
         * point
         */
        errorcode = OSPPHttpNew(ospvComm, ospvHttp);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* set up the connection type */
            if((ospvMsgInfoType & OSPC_MSGINFO_AUDIT_TYPE) == OSPC_MSGINFO_AUDIT_TYPE)
            {
                (*ospvHttp)->Flags |= OSPC_HTTP_AUDIT_TYPE;
            }

            /*
             * add the new http connection object to the HttpConnList
             */

            OSPPListAppend((OSPTLIST *)&(ospvComm->HttpConnList), 
                (void *)*ospvHttp);
            (void)OSPPCommIncrementHttpConnCount(ospvComm);

            /*
             * startup the http connection monitor thread for the
             * new connection.
             */
            errorcode = osppHttpStartWorker(*ospvHttp);
        }

        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPC_ERR_HTTP_NEWCONN_FAILED;
            OSPM_DBGERRORLOG(errorcode, "new http connection failed");
        }
    }
    OSPM_DBGNET(("HTTP connection: %s\n", (createnew ? "created" : "reused")));

    OSPM_DBGEXIT(("EXIT : osppHttpSelectConnection\n"));
    return errorcode;
}


static
int
osppHttpAddRequest(
    OSPTHTTP *ospvHttp,
    OSPTMSGINFO *ospvMsgInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER : osppHttpAddRequest\n"));

    /*
     * obtain the HTTP connection mutex. This could fail when
     * an idle HTTP connection is passed to this function and
     * the connection thread is in the process of closing itself
     * down. if this timing issue occurs, this function will return
     * a failure code and the calling function will attempt to add
     * the transaction to another HTTP connection object.
     */
    OSPM_MUTEX_LOCK(ospvHttp->Mutex, errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPListAppend((OSPTLIST *)&(ospvHttp->MsgInfoList), 
            (void *)ospvMsgInfo);
        ospvHttp->NumberOfTransactions++;

        OSPM_MUTEX_UNLOCK(ospvHttp->Mutex, errorcode);
        assert(errorcode == OSPC_ERR_NO_ERROR);
    }

    OSPM_DBGEXIT(("EXIT : osppHttpAddRequest\n"));
    return errorcode;
}

int
OSPPHttpRequestHandoff(
    OSPTCOMM     *ospvComm, 
    OSPTMSGQUEUE *ospvMsgQueue)
{
    int         errorcode  = OSPC_ERR_NO_ERROR;
    OSPTMSGINFO *msginfo   = OSPC_OSNULL;
    OSPTHTTP    *httpconn  = OSPC_OSNULL;
    int         attempts   = 0;

    OSPM_DBGENTER(("ENTER : OSPPHttpRequestHandoff\n"));

    /*
     * because this function is called by the Comm Msg monitor,
     * we currently have exclusive access to the message queue.
     * we need to hand-off the work as quickly as possible to avoid
     * creating a transaction bottleneck.
     */

    /*
     * get the first msginfo item from the msg queue list
     */
    msginfo = (OSPTMSGINFO *)OSPPListFirst(
        (OSPTLIST *)&(ospvMsgQueue->MsgInfoList));
    if (msginfo == (OSPTMSGINFO *)OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_HTTP_BAD_QUEUE;
        OSPM_DBGERRORLOG(errorcode, "msg queue prematurely empty");
    }
    else
    {
        /*
         * Now remove the message info item from the comm
         * message queue
         */
        msginfo = (OSPTMSGINFO *)OSPPListRemove(
            (OSPTLIST *)&(ospvMsgQueue->MsgInfoList));
        if (msginfo == (OSPTMSGINFO *)OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_HTTP_MALLOC_FAILED;
            OSPM_DBGERRORLOG(errorcode, "msg queue item delete failed");
        }

        /*
         * decrement the message queue transaction count
         */
        OSPPMsgQueueDecrementNumberOfTransactions(ospvMsgQueue);

        /*
         * try at most 3 times to get an HTTP connection object. in
         * almost all cases, the httpconn object will be obtained on
         * the first pass. however, there is a race condition between
         * an existing idle HTTP connection thread and the 
         * osppHttpAddRequest thread. if osppHttpAddRequest is waiting
         * for an idle connection mutex and this connection in is the
         * process of shutting down, osppHttpAddRequest will fail. 
         * try 2 more times to get another connection or create a 
         * new one. The race condition will never occur when a new
         * HTTP connection is created.
         */

        for (attempts = 0; attempts < 3; attempts++)
        {
            /*
             * determine which HTTP connection object to hand the 
             * request off to.
             */
            errorcode = osppHttpSelectConnection(ospvComm, &httpconn, msginfo->Flags);
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                /*
                 * add the msginfo item to the selected HTTP connection
                 * queue and increment the count. This is alittle tricky
                 * because the msginfo structure is shared between the
                 * comm message queue and the http connection queue.
                 * The link to msginfo has been broken by the above
                 * call to OSPPListRemove. A new link will be added for
                 * the http connection queue.
                 */
                msginfo->ErrorCode = osppHttpAddRequest(httpconn, msginfo);
                if (msginfo->ErrorCode == OSPC_ERR_NO_ERROR)
                    break;
            }
            else
            {
                /*
                 * SelectConnection failed.  Save the error code and break
                 */
                msginfo->ErrorCode = errorcode;
                break;
            }
        }

        /*
         * signal the HTTP connection that a new transaction is now available
         * this must be done for both successful and non-successful requests
         */
        if (msginfo->ErrorCode == OSPC_ERR_NO_ERROR)
        {
            OSPM_MUTEX_LOCK(httpconn->Mutex, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPM_CONDVAR_SIGNAL(httpconn->CondVar, errorcode);
                assert(errorcode == OSPC_ERR_NO_ERROR);

                OSPM_MUTEX_UNLOCK(httpconn->Mutex, errorcode);
                assert(errorcode == OSPC_ERR_NO_ERROR);
            }
        }
        else
        {
          /*
           * Signal caller that the request failed
           */
            OSPM_MUTEX_LOCK(msginfo->Mutex, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPM_CONDVAR_SIGNAL(msginfo->CondVar, errorcode);
                assert(errorcode == OSPC_ERR_NO_ERROR);

                OSPM_MUTEX_UNLOCK(msginfo->Mutex, errorcode);
                assert(errorcode == OSPC_ERR_NO_ERROR);
            }
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPHttpRequestHandoff\n"));
    return errorcode;
}

void
OSPPHttpParseHeader(
    unsigned char *ospvInBuffer,
    unsigned char **ospvOutBuffer,
    unsigned      *ospvLength,
    int           ospvHeaderType,
    int           *ospvError)
{

    char *begptr = OSPC_OSNULL,
         *endptr = OSPC_OSNULL;

    OSPM_DBGENTER(("ENTER : OSPPHttpParseHeader\n"));
    switch(ospvHeaderType)
    {

        case OSPC_HTTP_CONTENT_LENGTH:
        /*
         * Parse Content-Length:
         */

        begptr = OSPM_STRSTR((const char *)ospvInBuffer, "content-length: ");
        if (begptr != OSPC_OSNULL)
        {

            if ((OSPM_SSCANF(begptr + 16, "%u", ospvLength)) != 1)
            {
                *ospvError = OSPC_ERR_HTTP_BAD_HEADER_200;
                OSPM_DBGERRORLOG(*ospvError, "bad http header");
            }
            else
            {
                OSPM_MALLOC(*ospvOutBuffer, unsigned char, *ospvLength + 1);
                if (*ospvOutBuffer == (unsigned char *)OSPC_OSNULL)
                {
                    *ospvError = OSPC_ERR_HTTP_MALLOC_FAILED;
                    OSPM_DBGERRORLOG(*ospvError, "bad http header");
                }
                else
                    OSPM_MEMSET(*ospvOutBuffer, 0, *ospvLength + 1);
            }
        }
        else
        {
            *ospvError = OSPC_ERR_HTTP_BAD_HEADER_200;
            OSPM_DBGERRORLOG(*ospvError, "bad http header");
        }
        break;

        case OSPC_HTTP_CONTENT_TYPE:
        /*
         * Parse Content-Type:
         */

        begptr = OSPM_STRSTR((const char*)ospvInBuffer, "content-type: ");
        if (begptr != OSPC_OSNULL)
        {
            endptr = OSPM_STRSTR(begptr, "\r\n");
            if (endptr != OSPC_OSNULL)
            {
                *ospvLength = endptr - begptr;
                OSPM_MALLOC(*ospvOutBuffer, unsigned char, *ospvLength+1);
                if (*ospvOutBuffer != (unsigned char *)OSPC_OSNULL)
                {
                    OSPM_MEMSET(*ospvOutBuffer, 0, *ospvLength+1);
                    OSPM_MEMCPY(*ospvOutBuffer, begptr, *ospvLength);
                }
                else
                {
                    *ospvError = OSPC_ERR_HTTP_MALLOC_FAILED;
                    OSPM_DBGERRORLOG(*ospvError, "bad http header");
                }
            }
            else
            {
                *ospvError = OSPC_ERR_HTTP_BAD_HEADER_200;
                OSPM_DBGERRORLOG(*ospvError, "bad http header");
            }
        }
        else
        {
            *ospvError = OSPC_ERR_HTTP_BAD_HEADER_200;
            OSPM_DBGERRORLOG(*ospvError, "content-type not found\n");
        }
        break;

        /*
         * The Connection type is optional in the header. As a result,
         * we do not generate an error if it does not exist. When it 
         * does not exist, assume Connection: Keep-alive.
         */

        case OSPC_HTTP_CONNECTION_TYPE:
        /*
         * Parse Connection:
         */

        begptr = OSPM_STRSTR((const char *)ospvInBuffer, "connection: ");
        if (begptr != OSPC_OSNULL)
        {
            endptr = OSPM_STRSTR(begptr, "\r\n");
            if (endptr != OSPC_OSNULL)
            {
                *ospvLength = endptr - begptr;
                OSPM_MALLOC(*ospvOutBuffer, unsigned char, *ospvLength+1);
                if (*ospvOutBuffer != (unsigned char *)OSPC_OSNULL)
                {
                    OSPM_MEMSET(*ospvOutBuffer, 0, *ospvLength+1);
                    OSPM_MEMCPY(*ospvOutBuffer, begptr, *ospvLength);
                }
                else
                {
                    *ospvError = OSPC_ERR_HTTP_MALLOC_FAILED;
                    OSPM_DBGERRORLOG(*ospvError, "bad http header");
                }
            }
            else
            {
                *ospvError = OSPC_ERR_HTTP_BAD_HEADER_200;
                OSPM_DBGERRORLOG(*ospvError, "bad http header");
            }
        }
        break;

        default:
        *ospvError = OSPC_ERR_HTTP_BAD_HEADER_200;
        OSPM_DBGERRORLOG(*ospvError, "bad http header");

    }
    OSPM_DBGEXIT(("EXIT : OSPPHttpParseHeader\n"));
    return;
}
