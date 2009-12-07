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

/* ospssl.c - SSL common functions */

#include "osp/osp.h"
#include "osp/ospssl.h"
#include "osp/osphttp.h"
#include "osp/ospcomm.h"
#include "osp/ospsocket.h"
#include "osp/osputils.h"
#include "osp/ospsecurity.h"

void OSPPSSLSessionDelete(
    OSPTBOOL ospvGracefulShutdown,
    OSPTSSLSESSION **ospvSSLSession)
{
    OSPM_DBGENTER(("ENTER: OSPPSSLSessionDelete()\n"));
    if (*ospvSSLSession != OSPC_OSNULL) {
        if (OSPPSSLSessionHasContext(*ospvSSLSession)) {
            if (ospvGracefulShutdown == OSPC_TRUE) {
                OSPPSSLWrapSessionGracefulShutdown(*ospvSSLSession);
            }
            OSPPSSLWrapSessionContextDelete(*ospvSSLSession);
        }
        OSPM_FREE(*ospvSSLSession);
        *ospvSSLSession = OSPC_OSNULL;
    }
    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionDelete()\n"));
}

int OSPPSSLSessionNew(
    OSPTHTTP *ospvHttp,
    OSPTSEC *ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR, rootcacertlen = 0;
    void *rootcacert = OSPC_OSNULL;

    OSPM_DBGENTER(("ENTER: OSPPSSLSessionNew()\n"));
    /*
     * If https is required, ensure that a SSL Session is currently
     * active for this socket and that its session-id has not expired
     */
    if (OSPM_COMM_SECURED_IO(ospvHttp->ServicePoint)) {
        /* if no ssl context, then create a new one */
        if (ospvHttp->SSLSession == OSPC_OSNULL) {
            errorcode = OSPPSSLSessionInitialize(ospvHttp, ospvSecurity);

            /* do the SSL handshake */
            if (errorcode == OSPC_ERR_NO_ERROR) {
                errorcode = OSPPSSLSessionNegotiate(ospvHttp);
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    errorcode = OSPPSSLWrapGetServerRootCACert(&rootcacert, &rootcacertlen, ospvHttp->SSLSession);
                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        /* ------------------------------------------------------
                         * Verify that we trust this server's Root CA Certificate
                         * OpenSSL Verifies this for you.  Uncomment if you need this
                         * ------------------------------------------------------
                         *errorcode = OSPPSecVerifyRootAuthorityCertificate(ospvSecurity,
                         *   rootcacert, rootcacertlen);
                         */
                    }
                    if (rootcacert != OSPC_OSNULL) {
                        OSPPSSLWrapFreeServerRootCACert(&rootcacert);
                    }
                } else {
                    /* clean up any unused session ids */
                    if (ospvHttp->SSLSession && ospvHttp->SSLSession->SessionId) {
                        OSPPSecSSLSessionIdDelete(ospvSecurity, &(ospvHttp->SSLSession->SessionId), OSPC_TRUE);
                    }
                }
            }
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionNew()\n"));
    return errorcode;
}

int OSPPSSLSessionInitialize(
    OSPTHTTP *ospvHttp,
    OSPTSEC *ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLSessionInitialize()\n"));
    /*
     * initialize the SSL Session object
     */
    ospvHttp->SSLSession = OSPPSSLSessionAlloc();
    if (ospvHttp->SSLSession == OSPC_OSNULL) {
        errorcode = OSPC_ERR_HTTP_SSL_NEW_FAILED;
    } else {
        /*
         * initialize the SSL Session Context
         */
        errorcode = OSPPSSLWrapSessionContextNew((void *)ospvHttp, (void *)ospvSecurity);
        if (errorcode != OSPC_ERR_NO_ERROR) {
            errorcode = OSPC_ERR_HTTP_SSL_CTX_NEW_FAILED;
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionInitialize() (%d)\n", errorcode));
    return errorcode;
}

int OSPPSSLSessionNegotiate(
    OSPTHTTP *ospvHttp)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLSessionNegotiate()\n"));

    /*
     * attach the connected socket to the SSL library
     */
    errorcode = OSPPSSLWrapAttachConnection(ospvHttp->SSLSession, ospvHttp);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        errorcode = OSPPSSLWrapHandshake(ospvHttp->SSLSession);
    }
    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionNegotiate() (%d)\n", errorcode));
    return errorcode;
}

int OSPPSSLSessionRead(
    OSPTHTTP *ospvHttp,
    void *ospvBuffer,
    unsigned int *ospvLength,
    char *ospvDelimiter)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    register unsigned bufidx = 0;
    unsigned char *tmpptr = OSPC_OSNULL;
    unsigned delimitsz = 0, delimitidx = 0;
    unsigned int len = 1;

    OSPM_DBGENTER(("ENTER: OSPPSSLSessionRead() delim = <%s> len = <%d>\n", ospvDelimiter ? ospvDelimiter : "", *ospvLength));

    /*
     * with a delimiter present, read from buffer until
     * a delimiter is encountered
     */
    if (ospvDelimiter != OSPC_OSNULL) {
        delimitsz = OSPM_STRLEN(ospvDelimiter);
        tmpptr = (unsigned char *)ospvBuffer;

        while (delimitsz != delimitidx && bufidx < *ospvLength && errorcode == OSPC_ERR_NO_ERROR) {
            if (OSPM_COMM_SECURED_IO(ospvHttp->ServicePoint)) {
                errorcode = OSPPSSLWrapGetData((void *)&((unsigned char *)ospvBuffer)[bufidx], &len,
                    (OSPTSSLSESSION *)ospvHttp->SSLSession);
            } else {
                errorcode = OSPPSockRead(ospvHttp, &((unsigned char *)ospvBuffer)[bufidx], &len);
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /*
                 * does the input character match the corresponding
                 * character in our delimiter string?
                 */
                if (((unsigned char *)ospvBuffer)[bufidx] == ospvDelimiter[delimitidx]) {
                    /*
                     * yes, we have a character match.
                     * if delimitidx is 0, set the tmpptr to the
                     * beginning buffer address which may contain the
                     * delimiter.
                     */
                    if (!delimitidx) {
                        tmpptr = &((unsigned char *)ospvBuffer)[bufidx];
                    }
                    delimitidx++;
                } else
                    /*
                     * not a match, reset delimitidx
                     */
                    delimitidx = 0;

                bufidx++;
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /*
             * overwrite the current length. set to the actual
             * bytes received in total
             */
            *ospvLength = bufidx;

            /*
             * terminate the delimited input buffer
             */
            if (bufidx < *ospvLength) {
                ((unsigned char *)ospvBuffer)[bufidx] = '\0';
            }
        }
    } else {
        /*
         * read the whole message from the SSL internal buffer/socket
         */
        len = *ospvLength;

        if (OSPM_COMM_SECURED_IO(ospvHttp->ServicePoint)) {
            errorcode = OSPPSSLWrapGetData(ospvBuffer, ospvLength, (OSPTSSLSESSION *)ospvHttp->SSLSession);
        } else {
            errorcode = OSPPSockRead(ospvHttp, &((unsigned char *)ospvBuffer)[bufidx], &len);
        }
    }
/*
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGNET),("NETIO: OSPPSSLSessionRead() recv [%*s]\n",
        len, (char *)ospvBuffer));
*/

    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionRead() (%d)\n", errorcode));
    return errorcode;
}

int OSPPSSLSessionWrite(
    OSPTHTTP *ospvHttp,
    void *ospvBuffer,
    unsigned int *ospvLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLSessionWrite()\n"));
    if (OSPM_COMM_SECURED_IO(ospvHttp->ServicePoint)) {
        errorcode = OSPPSSLWrapSendData(ospvBuffer, ospvLength, (OSPTSSLSESSION *)ospvHttp->SSLSession);
    } else {
        errorcode = OSPPSockWrite(ospvHttp, (unsigned char *)ospvBuffer, ospvLength);
    }
    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionWrite() (%d)\n", errorcode));
    return errorcode;
}

OSPTSSLSESSION *OSPPSSLSessionAlloc(void)
{
    OSPTSSLSESSION *sslsess = OSPC_OSNULL;

    OSPM_DBGENTER(("ENTER: OSPPSSLSessionAlloc()\n"));

    OSPM_MALLOC(sslsess, OSPTSSLSESSION, sizeof(OSPTSSLSESSION));
    if (sslsess != OSPC_OSNULL) {
        OSPM_MEMSET(sslsess, 0, sizeof(OSPTSSLSESSION));
    }

    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionAlloc() (%lx)\n", (unsigned long)sslsess));

    return sslsess;
}

OSPTBOOL OSPPSSLSessionHasSessionId(
    OSPTSSLSESSION *ospvSSLSession)
{
    OSPTBOOL hassessionid = OSPC_FALSE;

    OSPM_DBGENTER(("ENTER: OSPPSSLSessionHasSessionId()\n"));

    if (ospvSSLSession != OSPC_OSNULL) {
        hassessionid = (ospvSSLSession->SessionId != OSPC_OSNULL);
    }

    OSPM_DBGEXIT(("EXIT : OSPPSSLSessionHasSessionId() (%d)\n", hassessionid));

    return hassessionid;
}
