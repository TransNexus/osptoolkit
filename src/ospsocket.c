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
 * ospsocket.cpp - Socket functions.
 */
#include "osp/osp.h"
#include "osp/ospconfig.h"
#include "osp/ospsocket.h"
#include "osp/osputils.h"
#include "osp/ospcomm.h"
#include "osp/osphttp.h"
#include "osp/ospssl.h"

int
OSPPSockClose(
    OSPTBOOL ospvGracefulSSLShutdown,
    OSPTSOCKET      *ospvSockFd,
    OSPTSSLSESSION  **SSLSession)
{
    int  tmperror  = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSockClose()\n"));

    /* delete the SSL context if it's still hanging around */
    OSPPSSLSessionDelete(ospvGracefulSSLShutdown, SSLSession);
    if (*ospvSockFd != OSPC_SOCK_INVALID)
    {
        OSPM_CLOSE(*ospvSockFd, tmperror);
        *ospvSockFd = OSPC_SOCK_INVALID;
    }

    OSPM_DBGEXIT(("EXIT : OSPPSockClose()\n"));
    return OSPC_ERR_NO_ERROR;
}

char *OSPM_INET_NTOA(OSPTIPADDR ip) 
{
    struct in_addr sin;
    sin.s_addr     = (ip);
    return(inet_ntoa(sin));
}

int
OSPPSockConnect(
    OSPTSOCKET          *ospvSockFd,
    OSPTBOOL            ospvBlocking,
    OSPTIPADDR          ospvIpAddr,
    unsigned short      ospvPort,
    struct timeval      *ospvTimeout,
    OSPTSSLSESSION      **ospvSSLSession)
{
    int  errorcode = OSPC_ERR_NO_ERROR;
    char ErrStr[200];

    /*
     * set the connection socket. 
     * This will also affect reads and writes.
     */
    errorcode = OSPPSockSetBlockingIO(*ospvSockFd, ospvBlocking);

    /*
     * attempt connection
     */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {

        OSPM_CONNECT(*ospvSockFd, ospvIpAddr, ospvPort, errorcode);
    
        if ((errorcode != OSPC_ERR_NO_ERROR) && (errorcode != OSPC_ERR_SOCK_CONN_IN_PROGRESS))
        {
            errorcode = OSPC_ERR_SOCK_CONNECT_FAILED;
#ifdef _WIN32
            OSPM_DBGERROR(("Could not connect to %s:%d .Check IP Address and Port Numbers again. Error = %d\n",OSPM_INET_NTOA(ospvIpAddr), ntohs(ospvPort),errorcode));
            sprintf(ErrStr,"Cannot communicate with application. Connection timed out to IP address: %s",OSPM_INET_NTOA(ospvIpAddr));
            OSPM_DBGERRORLOG(errorcode,ErrStr);
#else
            OSPM_DBGERROR(("%s to %s:%d .Check IP Address and Port Numbers again. Error = %d\n",strerror(errno), OSPM_INET_NTOA(ospvIpAddr), ntohs(ospvPort),errorcode));
            sprintf(ErrStr,"Cannot communicate with application. Connection timed out to IP address: %s",OSPM_INET_NTOA(ospvIpAddr));
            OSPM_DBGERRORLOG(errorcode,ErrStr);
#endif
            OSPPSockClose(OSPC_FALSE, ospvSockFd, ospvSSLSession);
        }
        else
        {
            /*
             * use the configured timeout to limit how 
             * long to wait for a connection
             */
            errorcode = OSPPSockWaitTillReady(*ospvSockFd, OSPC_FALSE, ospvTimeout);
            if (errorcode != OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPC_ERR_SOCK_CONNECT_FAILED;
#ifdef _WIN32
                OSPM_DBGERROR(("Could not connect to %s:%d .Check IP Address and Port Numbers again. Error = %d\n",OSPM_INET_NTOA(ospvIpAddr), ntohs(ospvPort),errorcode));
                sprintf(ErrStr,"Cannot communicate with application. Connection timed out to IP address: %s",OSPM_INET_NTOA(ospvIpAddr));
                OSPM_DBGERRORLOG(errorcode,ErrStr);
#else
                OSPM_DBGERROR(("%s to %s:%d .Check IP Address and Port Numbers again. Error = %d\n",strerror(errno), OSPM_INET_NTOA(ospvIpAddr), ntohs(ospvPort),errorcode));
                sprintf(ErrStr,"Cannot communicate with application. Connection timed out to IP address: %s",OSPM_INET_NTOA(ospvIpAddr));
                OSPM_DBGERRORLOG(errorcode,ErrStr);
#endif
                OSPPSockClose(OSPC_FALSE, ospvSockFd, ospvSSLSession);
            }
        }

    }

    return errorcode;
}

int
OSPPSockConnectAuditURL(
    OSPTHTTP  *ospvHttp,
    OSPTBOOL  *ospvConnected)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;
    OSPTCOMM        *comm       = OSPC_OSNULL;
    struct timeval  timeout;
    unsigned        socktimeout = 0;

    OSPM_DBGENTER(("ENTER: OSPPSockConnectAuditURL()\n"));
    if (ospvHttp == (OSPTHTTP *)OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_HTTP_INVALID_ARG;
    }

    /*
     * see if the socket is or has been connected. If so, shut it 
     * down now.
     */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        if (ospvHttp->SockFd != OSPC_SOCK_INVALID)
        {
            errorcode = OSPPSockClose(OSPC_FALSE, &(ospvHttp->SockFd), &(ospvHttp->SSLSession));
            if (errorcode != OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPC_ERR_SOCK_CLOSE_FAILED;
                OSPM_DBGERRORLOG(errorcode, "cannot close socket");
            }
        }
    }
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /*
         * create a new socket 
         */
        errorcode = OSPPSockNew(ospvHttp);
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPC_ERR_SOCK_CREATE_FAILED;
            OSPM_DBGERRORLOG(errorcode, "cannot create socket");
        }
        else
        {
            comm = (OSPTCOMM *)ospvHttp->Comm;

            /* get auditURL svcpt */
            ospvHttp->ServicePoint = comm->AuditURL;

            /*
             * perform actual connection attempt
             */
            OSPM_DBGNET(("connecting to %s%s", 
                ospvHttp->ServicePoint->HostName,
                ospvHttp->ServicePoint->URI));

            if (ospvHttp->ServicePoint != OSPC_OSNULL)
            {
                ospvHttp->Flags = (unsigned char)
                    (ospvHttp->Flags & OSPC_SOCK_CONNECTED_MASK);

                /*
                 * set the response timeout for the socket
                 */
                OSPPCommGetTimeout((OSPTCOMM *)ospvHttp->Comm,
                    &socktimeout);

                timeout.tv_sec  = socktimeout / 1000;
                timeout.tv_usec = socktimeout % 1000 * 1000;

                                /*
                                ** CHANGED OSPCFALSE -> OSPCTRUE in Sock conenct experimental
                                */

                errorcode = OSPPSockConnect(&(ospvHttp->SockFd), OSPC_DEFAULT_BLOCKING_FLAG, 
                                            ospvHttp->ServicePoint->IpAddr,
                                            ospvHttp->ServicePoint->Port,
                                            &timeout, &(ospvHttp->SSLSession));

                if(errorcode == OSPC_ERR_NO_ERROR)
                {
                    if (errorcode == OSPC_ERR_NO_ERROR)
                    {
                        /*
                         * begin an SSL Session if a secured connection
                         * is required
                         */
                        errorcode = OSPPSSLSessionNew(ospvHttp, comm->Security);

                        if (errorcode == OSPC_ERR_NO_ERROR)
                        {
                            *ospvConnected = OSPC_TRUE;
                            ospvHttp->Flags |= OSPC_SOCK_CONNECTED_BIT;
                        }
                        else
                        {
                            OSPPSockClose(OSPC_FALSE, &(ospvHttp->SockFd), &(ospvHttp->SSLSession));

                        }
                    }
                }
            }
            else
            {
                errorcode = OSPC_ERR_COMM_NO_SVC_PTS_AVAIL;
                OSPM_DBGERRORLOG(errorcode, "no svc pt avail");
            }
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPSockConnectAuditURL() (%d)\n", errorcode));
    return errorcode;
}

int
OSPPSockConnectServicePoint(
    OSPTHTTP  *ospvHttp,
    OSPTBOOL  *ospvRollover,
    OSPTBOOL  *ospvConnected)
{
    unsigned        numsvcpts   = 0,
                    socktimeout = 0;
    OSPTSVCPT       *svcptlist  = OSPC_OSNULL,
                    *svcptitem  = OSPC_OSNULL;
    int             errorcode   = OSPC_ERR_NO_ERROR;
    struct timeval  timeout;
    OSPTCOMM        *comm       = OSPC_OSNULL;

    OSPM_DBGENTER(("ENTER: OSPPSockConnectServicePoint()\n"));
    if (ospvHttp == (OSPTHTTP *)OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_HTTP_INVALID_ARG;
    }

    /*
     * see if the socket is or has been connected. If so, shut it 
     * down now.
     */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        if (ospvHttp->SockFd != OSPC_SOCK_INVALID)
        {

            errorcode = OSPPSockClose(OSPC_FALSE, &(ospvHttp->SockFd), &(ospvHttp->SSLSession));

            if (errorcode != OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPC_ERR_SOCK_CLOSE_FAILED;
                OSPM_DBGERRORLOG(errorcode, "cannot close socket");
            }
        }
    }
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /*
         * create a new socket 
         */
        errorcode = OSPPSockNew(ospvHttp);
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPC_ERR_SOCK_CREATE_FAILED;
            OSPM_DBGERRORLOG(errorcode, "cannot create socket");
        }
        else
        {
            comm = (OSPTCOMM *)ospvHttp->Comm;

            errorcode = OSPPCommGetNumberOfServicePoints(comm, 
                &numsvcpts);
            if (numsvcpts == 0) 
            {
                OSPM_DBGERRORLOG(OSPC_ERR_COMM_NO_SVC_PTS, 
                    "num of svc pts = 0");
                errorcode = OSPC_ERR_COMM_NO_SVC_PTS;
            }
            else 
            {
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                    /*
                     * get a pointer to the service point list
                     */
                    osppHttpGetServicePointList(ospvHttp, &svcptlist);

                    /*
                     * if the http connection has never been connected or
                     * the service point list has been exhausted, get the
                     * first service point in the list
                     */
                    if (ospvHttp->ServicePoint == 
                        (OSPTSVCPT *)OSPC_OSNULL)
                    {
                        ospvHttp->ServicePoint  = 
                            (OSPTSVCPT *)OSPPListFirst(
                            (OSPTLIST *)&svcptlist);
                    }
                    else
                    {
                        ospvHttp->ServicePoint = 
                            (OSPTSVCPT *)OSPPListNext(
                            (OSPTLIST *)&svcptlist, 
                            ospvHttp->ServicePoint);

                        /*
                         * let the calling function know that the 
                         * service point list has been rolled over 
                         * to the beginning
                         */
                        if (ospvHttp->ServicePoint == 
                            (OSPTSVCPT *)OSPC_OSNULL)
                        {
                            ospvHttp->ServicePoint = 
                                (OSPTSVCPT *)OSPPListFirst(
                                (OSPTLIST *)&svcptlist);

                        }
                    }

                    /* 
                     * if the next item in the service point list is null,
                     * then the list is exhausted
                     */
                    if ((svcptitem = (OSPTSVCPT *)OSPPListNext(
                        (OSPTLIST *)&svcptlist,
                        ospvHttp->ServicePoint)) == (OSPTSVCPT *)OSPC_OSNULL)
                    {
                        *ospvRollover = OSPC_TRUE;
                    }

                    /*
                     * perform actual connection attempt
                     */
                    OSPM_DBGNET(("connecting to %s%s", 
                        ospvHttp->ServicePoint->HostName,
                        ospvHttp->ServicePoint->URI));

                    if (ospvHttp->ServicePoint != OSPC_OSNULL)
                    {
                        ospvHttp->Flags = (unsigned char)
                            (ospvHttp->Flags & OSPC_SOCK_CONNECTED_MASK);

                        /*
                         * set the response timeout for the socket
                         */

                        OSPPCommGetTimeout((OSPTCOMM *)ospvHttp->Comm,
                            &socktimeout);

                        timeout.tv_sec  = socktimeout / 1000;
                        timeout.tv_usec = socktimeout % 1000 * 1000;

                                                /*
                                                ** CHANGED OSPCFALSE -> TRUE
                                                */
                        errorcode = OSPPSockConnect(&(ospvHttp->SockFd), OSPC_DEFAULT_BLOCKING_FLAG, 
                                                    ospvHttp->ServicePoint->IpAddr,
                                                    ospvHttp->ServicePoint->Port,
                                                    &timeout, &(ospvHttp->SSLSession));

                        if (errorcode == OSPC_ERR_NO_ERROR)
                        {
                            /*
                             * begin an SSL Session if a secured connection
                             * is required
                             */
                            errorcode = OSPPSSLSessionNew(ospvHttp, comm->Security);

                            if (errorcode == OSPC_ERR_NO_ERROR)
                            {
                                *ospvConnected = OSPC_TRUE;
                                ospvHttp->Flags |= OSPC_SOCK_CONNECTED_BIT;
                            }
                            else
                            {
                                OSPPSockClose(OSPC_FALSE, &(ospvHttp->SockFd), &(ospvHttp->SSLSession));
                            }
                        }
                    }
                    else
                    {
                        errorcode = OSPC_ERR_COMM_NO_SVC_PTS_AVAIL;
                        OSPM_DBGERRORLOG(errorcode, "no svc pts avail");
                    }
                }
            }
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPSockConnectServicePoint() (%d)\n", errorcode));
    return errorcode;
}

int
OSPPSockDisableNagle(
    OSPTHTTP *ospvHttp)
{
    int    errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSockDisableNagle()\n"));

    OSPM_DISABLE_NAGLE(ospvHttp->SockFd, errorcode);
    if (errorcode != OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPC_ERR_SOCK_DISABLE_NAGLE_FAILED;
    }
    OSPM_DBGEXIT(("EXIT : OSPPSockDisableNagle()\n"));
    return errorcode;
}

int
OSPPSockGetHostIP(
    char   *Host, 
    OSPTIPADDR *ospvIpAddr)
{
    int        errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSockGetHostIP()\n"));
    /*
     * see if char IP was passed
     */
    OSPM_INET_ADDR((const char *)Host, *ospvIpAddr);
    if (*ospvIpAddr)
        return errorcode;

    OSPM_GETHOSTBYNAME(Host, *ospvIpAddr, errorcode);

    OSPM_DBGEXIT(("EXIT : OSPPSockGetHostIP() (%d)\n", errorcode));
    return errorcode; 
}    

int
OSPPSockNew(
    OSPTHTTP  *ospvHttp)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSockNew()\n"));
    /* create the socket */
    OSPM_SOCKET(ospvHttp->SockFd, errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPSockSetKeepAlive(ospvHttp);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPSockDisableNagle(ospvHttp);
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPSockNew() (%d)\n", errorcode));
    return errorcode;
}

void
OSPPSockProcessRequest(
    OSPTHTTP      *ospvHttp,
    unsigned char *ospvRequestBuffer,
    unsigned      ospvRequestBufferSz,
    unsigned char **ospvResponseBuffer,
    unsigned      *ospvResponseBufferSz,
    unsigned char **ospvContentBuffer,
    unsigned      *ospvContentBufferSz,
    int           *ospvError)
{

    #define OSPC_RECVBUF_SZ    256
    unsigned char *tmprecvbuf                   = OSPC_OSNULL;
    char           recvheadbuf[OSPC_RECVBUF_SZ] = { "" };
    unsigned       recvbufsz                    = OSPC_RECVBUF_SZ;
    int            responsetype                 = 0;
    unsigned       ospvConnectionBufferSz       = 0;
    int            connectionCloseFlag          = OSPC_FALSE;
    unsigned char  *ospvConnectionBuffer        = OSPC_OSNULL;
    unsigned char  *tmpContentBuffer            = OSPC_OSNULL;
    unsigned       tmpContentBufferSz           = 0;

    OSPM_DBGENTER(("ENTER: OSPPSockProcessRequest()\n"));

    *ospvError = OSPC_ERR_NO_ERROR;

    if (ospvHttp == (OSPTHTTP *)OSPC_OSNULL)
    {
        *ospvError = OSPC_ERR_HTTP_INVALID_ARG;
    }

    /*
     * send the outbound message to the service point.
     * this consists of the HTTP header and the multipart MIME message
     */

    if (*ospvError == OSPC_ERR_NO_ERROR)
    {
        *ospvError = OSPPSSLSessionWrite(ospvHttp, ospvRequestBuffer, 
            &ospvRequestBufferSz);

        if(*ospvError == OSPC_ERR_NO_ERROR)
        {
            /* make sure socket was not reset by peer */
            OSPM_GETSOCKERR(ospvHttp, *ospvError);
        }
    }

    if (*ospvError == OSPC_ERR_NO_ERROR)
    {
        /*
         * the first response that the server may send back to us
         * could look like this:
         *
         * HTTP/1.1 100 Continue
         * Server: Microsoft-IIS/4.0
         * Date: Tue, 21 Jul 1998 00:39:54 GMT
         *
         * this simply confirms that the server has received our request.
         * verify the '100 Continue' and continue.
         */
        *ospvError = OSPPSSLSessionRead(ospvHttp, 
            (unsigned char *)recvheadbuf, 
            &recvbufsz, 
            "\r\n\r\n");

        /*
        ** Check this header to see what type of response we are getting.
        ** The type will be returned in the responsetype variable. If the
        ** type is something other than a 1xx or a 2xx, an errorcode will
        ** be returned by the function.
        */

        if ((*ospvError != OSPC_ERR_NO_ERROR))
        {
            if ((*ospvError != OSPC_ERR_SOCK_SELECT_FAILED) && (*ospvError != OSPC_ERR_SOCK_RECV_FAILED) && (*ospvError != OSPC_ERR_SSL_READ_FAILED))
            {
                OSPM_DBGERRORLOG(*ospvError, "http recv init header failed");
            }
        }
        else if ((*ospvError = OSPPHttpVerifyResponse(recvheadbuf, &responsetype,ospvHttp)) !=
            OSPC_ERR_NO_ERROR)
        {
            if (*ospvError == OSPC_ERR_HTTP_BAD_REQUEST)
                OSPM_DBGERRORLOG(*ospvError, "http response unexpected: Server unavailable at the URL specified.Modify URL and try again.");
        }
        else 
        {
            if (responsetype == 100)
            {
                /*
                 * Read again for the 200 header.
                 */
                recvbufsz = 256;
                *ospvError = OSPPSSLSessionRead(ospvHttp, 
                    (unsigned char *)recvheadbuf, 
                    &recvbufsz, 
                    "\r\n\r\n");

                responsetype = 0;
            }

            /*
             * the header for the server response will look like this:
             *
             * HTTP/1.1 200 OK
             * Server: Microsoft-IIS/4.0
             * Date: Tue, 21 Jul 1998 00:39:56 GMT
             * Connection: Keep-Alive
             * Content-Length: 1042
             * Content-type: .....
             */
            
            /*
             * parse the header response and place the content-length
             * in the ospvInBufferSz variable. This is used to determine
             * the size of the inbound response message buffer.
             */

            if (*ospvError == OSPC_ERR_NO_ERROR)
            {
                /*
                 * make sure we get an OK response back from the server
                 */
                if (((*ospvError = OSPPHttpVerifyResponse(recvheadbuf, &responsetype,ospvHttp)) != 
                    OSPC_ERR_NO_ERROR) || (responsetype != 200))
                {
                    OSPM_DBGERRORLOG(*ospvError, "http bad server response error");
                    OSPM_DBGERROR(("expected 2xx code: received = [%s]\n", recvheadbuf));
                }
                else
                {

                    /* parse the header response and place the MIME
                     * connection-type into the ospvConnectionBuffer variable.
                     * ospvConnectionBufferSz is set to the size of the
                     * ospvConnectionBuffer.
                     */

                    OSPPHttpParseHeader((unsigned char *) recvheadbuf, 
                        &ospvConnectionBuffer, &ospvConnectionBufferSz,
                        OSPC_HTTP_CONNECTION_TYPE, ospvError);

                    /* check to see if the connection type contains "close".
                     * If so, then set a flag to eventually close the socket connection.
                     */

                    if((ospvConnectionBuffer != OSPC_OSNULL)&&
                        (ospvConnectionBufferSz > 0))
                    {
                        if(OSPM_STRSTR((char *)ospvConnectionBuffer, (const char *)"close") != OSPC_OSNULL)
                        {
                            connectionCloseFlag = OSPC_TRUE;
                        }
                    }

                    if(ospvConnectionBuffer != OSPC_OSNULL)
                    {
                        OSPM_FREE(ospvConnectionBuffer);
                    }

                    OSPM_DBGNET(("http header = [%s]", 
                        recvheadbuf));

                    /*
                     * parse the header response and place the MIME 
                     * content-type into the tmpContentBuffer variable.
                     * tmpContentBufferSz is set to the size of the 
                     * tmpContentBuffer.
                     */

                    OSPPHttpParseHeader((unsigned char *)recvheadbuf, 
                        &tmpContentBuffer, &tmpContentBufferSz,
                        OSPC_HTTP_CONTENT_TYPE, ospvError);

                    if (*ospvError == OSPC_ERR_NO_ERROR)
                    {
                        /*
                         * parse the header response and place the 
                         * content-length in the ospvResponseBufferSz 
                         * variable. This is used to determine
                         * the size of the inbound response message 
                         * buffer. Alloc space for the tmprecvbuf in order
                         * receive the content data.
                         */
                        OSPPHttpParseHeader((unsigned char *)recvheadbuf, 
                            &tmprecvbuf, ospvResponseBufferSz,
                            OSPC_HTTP_CONTENT_LENGTH, ospvError);

                        if (*ospvError == OSPC_ERR_NO_ERROR)
                        {
                            /*
                             * now receive the body of the HTTP response
                             */
                            *ospvError = OSPPSSLSessionRead(
                                ospvHttp, tmprecvbuf,
                                ospvResponseBufferSz, OSPC_OSNULL);


                            if (*ospvError != OSPC_ERR_NO_ERROR)
                            {
                                if (tmprecvbuf != OSPC_OSNULL)
                                {
                                    OSPM_FREE(tmprecvbuf);
                                    tmprecvbuf = OSPC_OSNULL;
                                }
                                if (*ospvContentBuffer != OSPC_OSNULL)
                                {
                                    OSPM_FREE(*ospvContentBuffer);
                                }
                                *ospvResponseBuffer = 
                                    *ospvContentBuffer = OSPC_OSNULL; 
                            }
                            else
                            {
                                /*
                                 * assign the response buffer pointer after all of the 
                                 * reading is complete. This needs to be an autonomous
                                 * operation because the application thread is waiting for
                                 * the buffer to become non-null in order to indicate that
                                 * data is available to process. If the conditional variable
                                 * receives a spurous wakeup (which happens on many SMP platforms)
                                 * the buffer pointer must point to a complete response otherwise
                                 * a partial message will attempt to be processed which will reek
                                 * havoc on the request and possibly the application as well.
                                 */
                                *ospvResponseBuffer = tmprecvbuf;

                                OSPM_DBGNET(("http body = [%s]", 
                                    tmprecvbuf));
                            }
                        }
                    }

                    if(*ospvContentBuffer != OSPC_OSNULL)
                    {
                        OSPM_FREE(*ospvContentBuffer);
                        *ospvContentBufferSz = 0;
                    }

                    /* Save the content buffer and content buffer size
                     * for the next time through this function 
                     */
                    *ospvContentBuffer = tmpContentBuffer;
                    *ospvContentBufferSz = tmpContentBufferSz;

                }
            }
        }
    }
		else
		{
                    OSPM_DBGERROR(("Connection to application has been reset. IP address: %s",OSPM_INET_NTOA(ospvHttp->ServicePoint->IpAddr)));
		}

    /* If the Connection-Type indicated "close" in the MIME header, 
     * then close the connection.
     */
    if(connectionCloseFlag == OSPC_TRUE)
    {
        *ospvError = OSPPSockClose(OSPC_TRUE,
            &(ospvHttp->SockFd),
            &(ospvHttp->SSLSession));
    }

    OSPM_DBGEXIT(("EXIT : OSPPSockProcessRequest() (%d)\n", *ospvError));
    return;
}

int
OSPPSockRead(
    OSPTHTTP       *ospvHttp,
    unsigned char  *ospvBuffer,
    unsigned int   *ospvBufferSz)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;
    unsigned        length      = 0;
    struct timeval  timeout;
    unsigned        socktimeout = 0;
    char   ErrStr[200];

    OSPM_DBGENTER(("ENTER: OSPPSockRead()\n"));

    OSPM_DBGNET(("NET  : OSPPSockRead() bufsz = <%d>\n", *ospvBufferSz));
    do
    {
        /*
         * set the response timeout for the socket
         */
        OSPPCommGetTimeout((OSPTCOMM *)ospvHttp->Comm,
            &socktimeout);

        timeout.tv_sec  = socktimeout / 1000;
        timeout.tv_usec = socktimeout % 1000 * 1000;

        errorcode = OSPPSockWaitTillReady(ospvHttp->SockFd, OSPC_TRUE, &timeout);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPM_RECV(ospvHttp, ospvBuffer + length,
                *ospvBufferSz - length, errorcode);
            length += ospvHttp->ByteCount;
        }
        else
        {
            OSPM_DBGERROR(("Response timed out. Server unavailable on %s:%d Error = %d\n",OSPM_INET_NTOA(ospvHttp->ServicePoint->IpAddr),ntohs(ospvHttp->ServicePoint->Port),errorcode));
            sprintf(ErrStr,"Application slow in responding, response timed out to IP address: %s",OSPM_INET_NTOA(ospvHttp->ServicePoint->IpAddr));
            OSPM_DBGERRORLOG(errorcode,ErrStr);
        }

    } while (errorcode == OSPC_ERR_NO_ERROR &&
        ospvHttp->ByteCount > 0 && length < *ospvBufferSz);

    if (errorcode != OSPC_ERR_SOCK_SELECT_FAILED)
    if (length != *ospvBufferSz || errorcode != OSPC_ERR_NO_ERROR)
    {
        OSPM_DBGNET(
            ("NET  : OSPPSockRead() failed len = <%d> bufsz = <%d> err = <%d>\n", 
            length, *ospvBufferSz, errorcode));

        errorcode = OSPC_ERR_SOCK_RECV_FAILED;
        OSPM_DBGERROR(("Connection reset by peer on %s:%d Error = %d\n",OSPM_INET_NTOA(ospvHttp->ServicePoint->IpAddr),ntohs(ospvHttp->ServicePoint->Port),errorcode));
        sprintf(ErrStr,"Connection to application has been reset. IP address: %s",OSPM_INET_NTOA(ospvHttp->ServicePoint->IpAddr));
        OSPM_DBGERRORLOG(errorcode,ErrStr);
    }
    *ospvBufferSz = length;
    OSPM_DBGEXIT(("EXIT : OSPPSockRead() (%d)\n", errorcode));
    return errorcode;
}

int
OSPPSockSetBlockingIO(
    OSPTSOCKET          ospvSockFd,
    OSPTBOOL            ospvBlocking)
{
    int    errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSockSetBlockingIO()\n"));

    OSPM_BLOCKIO(ospvSockFd, ospvBlocking, errorcode);
    if (errorcode != OSPC_ERR_NO_ERROR)
        errorcode = OSPC_ERR_SOCK_BLOCKIO_FAILED;

    OSPM_DBGEXIT(("EXIT : OSPPSockSetBlockingIO()\n"));
    return errorcode;
}

int
OSPPSockSetKeepAlive( 
    OSPTHTTP *ospvHttp)
{
    int    optval    = 1;
    int    errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSockSetKeepAlive()\n"));

    OSPM_SETSOCKOPT(ospvHttp->SockFd, SOL_SOCKET, SO_KEEPALIVE, optval, 
        sizeof(optval), errorcode);

    OSPM_DBGEXIT(("EXIT : OSPPSockSetKeepAlive()\n"));
    return errorcode;
}

int
OSPPSockWaitTillReady(
    OSPTSOCKET      ospvSockFd,
    OSPTBOOL        ospvWaitForRead,
    struct timeval  *ospvTimeout)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    fd_set            fdlist;
    unsigned          fdready     = 0;

    OSPM_DBGENTER(("ENTER: OSPPSockWaitTillReady()\n"));

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        FD_ZERO(&fdlist);
        FD_SET(ospvSockFd, &fdlist);


        /*
         * make sure there is data available to read
         */
        if (ospvWaitForRead)
        {
            OSPM_SELECT(ospvSockFd +1, &fdlist, (fd_set *)OSPC_OSNULL, 
                (fd_set *)OSPC_OSNULL, ospvTimeout, fdready);
        }
        else
        {
            /*
             * make sure there is data available to write
             */

            OSPM_SELECT(ospvSockFd +1, (fd_set *)OSPC_OSNULL, &fdlist, 
                (fd_set *)OSPC_OSNULL, ospvTimeout, fdready);
        }

        if (fdready != 1)
            errorcode = OSPC_ERR_SOCK_SELECT_FAILED;
    }
    OSPM_DBGEXIT(("EXIT : OSPPSockWaitTillReady() select on %s to.sec(%ld) to.usec(%ld) fdready(%d) err(%d)\n", 
        (ospvWaitForRead ? "read" : "write"), ospvTimeout->tv_sec, ospvTimeout->tv_usec, 
         fdready, errorcode));

    return errorcode;
}

int
OSPPSockWrite(
    OSPTHTTP       *ospvHttp,
    unsigned char  *ospvBuffer,
    unsigned int   *ospvBufferSz)
{

    int             errorcode = OSPC_ERR_NO_ERROR;
    unsigned        length    = 0;
    struct timeval  timeout;

    OSPM_DBGENTER(("ENTER: OSPPSockWrite()\n"));

    do
    {
        timeout.tv_sec = 0l;
        timeout.tv_usec = 0l;
        errorcode = OSPPSockWaitTillReady(ospvHttp->SockFd, OSPC_FALSE, &timeout);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPM_SEND(ospvHttp->SockFd, ospvHttp->ByteCount, ospvBuffer + length,
                *ospvBufferSz - length, errorcode);
            length += ospvHttp->ByteCount;
        }
    } while (errorcode == OSPC_ERR_NO_ERROR && 
        ospvHttp->ByteCount > 0 && length < *ospvBufferSz);

    if (length != *ospvBufferSz || errorcode != OSPC_ERR_NO_ERROR)
        errorcode = OSPC_ERR_SOCK_SEND_FAILED;
    *ospvBufferSz = length;

    OSPM_DBGEXIT(("EXIT : OSPPSockWrite() (%d)\n", errorcode));
    return errorcode;
}
