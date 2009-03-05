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
 * ospnossl.c - SSL API bridge to NO SSL IMPLEMENTATION !!!
 */
#include "osp/osp.h"
#include "osp/ospsocket.h"
#include "osp/ospssl.h"
#include "osp/osputils.h"
#include "osp/ospcomm.h"
#include "osp/ospsecurity.h"

int OSPPSSLWrapInit(
    void *ospvRef)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapInit()\n"));
    OSPM_ARGUSED(ospvRef);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapInit()\n"));

    return errorcode;
}

void OSPPSSLWrapCleanup(
    void *ospvRef)
{
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapCleanup()\n"));
    OSPM_ARGUSED(ospvRef);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapCleanup()\n"));
}

int OSPPSSLWrapSessionContextNew(
    void *ospvConnection, 
    void *ospvContextRef)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSessionContextNew()\n"));
    OSPM_ARGUSED(ospvConnection);
    OSPM_ARGUSED(ospvContextRef);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSessionContextNew() (%d)\n", errorcode));

    return errorcode;
}

int OSPPSSLWrapAttachConnection(
    OSPTSSLSESSION *ospvSSLSession, 
    void *ospvConnection)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapAttachConnection()\n"));
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_ARGUSED(ospvConnection);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapAttachConnection() (%d)\n", errorcode));

    return errorcode;
}

int OSPPSSLWrapHandshake(
    OSPTSSLSESSION *ospvSSLSession)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapHandshake()\n"));
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapHandshake() (%d)\n", errorcode));
    OSPM_ARGUSED(ospvSSLSession);

    return errorcode;
}

int OSPPSSLWrapSessionContextDelete(
    OSPTSSLSESSION *ospvSSLSession)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSessionContextDelete()\n"));
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSessionContextDelete() (%d)\n", errorcode));

    return errorcode;
}

int OSPPSSLWrapGetData(
    void *ospvBuffer, 
    unsigned int *ospvLength, 
    OSPTSSLSESSION *ospvSSLSession)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapGetData()\n"));
    OSPM_ARGUSED(ospvBuffer);
    OSPM_ARGUSED(ospvLength);
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapGetData() (%d)\n", errorcode));

    return errorcode;
}

int OSPPSSLWrapSendData(
    void *ospvBuffer, 
    unsigned int *ospvLength, 
    OSPTSSLSESSION *ospvSSLSession)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSendData()\n"));
    OSPM_ARGUSED(ospvBuffer);
    OSPM_ARGUSED(ospvLength);
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSendData() (%d)\n", errorcode));

    return errorcode;
}

void OSPPSSLWrapFreeServerRootCACert(
    void **ospvRootCACert)
{
    OSPM_DBGENTER(("ENTER: OSPPSSLWrapFreeServerRootCACert()\n"));
    OSPM_ARGUSED(ospvRootCACert);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapFreeServerRootCACert()\n"));
}

int OSPPSSLWrapGetServerRootCACert(
    void **ospvRootCACert, 
    int *ospvRootCACertLen, 
    OSPTSSLSESSION *ospvSSLSession)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapGetServerRootCACert()\n"));
    OSPM_ARGUSED(ospvRootCACert);
    OSPM_ARGUSED(ospvRootCACertLen);
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapGetServerRootCACert() (%d)\n", errorcode));

    return errorcode;
}

int OSPPSSLWrapSessionGracefulShutdown(
    OSPTSSLSESSION *ospvSSLSession)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPSSLWrapSessionGracefulShutdown()\n"));
    OSPM_ARGUSED(ospvSSLSession);
    OSPM_DBGEXIT(("EXIT : OSPPSSLWrapSessionGracefulShutdown() (%d)\n", errorcode));

    return errorcode;
}
