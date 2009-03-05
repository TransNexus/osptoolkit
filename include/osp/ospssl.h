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
 * ospssl.h - common SSL object and function definitions along with
 *            wrapper prototypes
 */

#ifndef _OSPSSL_H
#define _OSPSSL_H

#include "osp/osp.h"
#include "osp/osphttp.h"
#include "osp/ospsecurity.h"
#include "osp/ospsslsess.h"

#define OSPPSSLSessionHasContext(sslsess)   ((sslsess)->Context != OSPC_OSNULL)
#define OSPPSSLSessionGetContext(sslsess)   ((sslsess)->Context)

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * OSP SDK SSL Entry Point Macros
     */
    /* called only once for each provider */
#define OSPPSSLSessionInit(secptr)     OSPPSSLWrapInit(secptr)
#define OSPPSSLSessionCleanup(secptr)  OSPPSSLWrapCleanup(secptr)

    /*
     * OSP SDK SSL Entry Point Functions
     */
    int OSPPSSLSessionNew(OSPTHTTP *, OSPTSEC *);
    int OSPPSSLSessionRead(OSPTHTTP *, void *, unsigned int *, char *);
    int OSPPSSLSessionWrite(OSPTHTTP *, void *, unsigned int *);
    void OSPPSSLSessionDelete(OSPTBOOL, OSPTSSLSESSION **);

    /*
     * SSL Session Member functions
     */
    OSPTSSLSESSION *OSPPSSLSessionAlloc(void);
    OSPTBOOL OSPPSSLSessionHasSessionId(OSPTSSLSESSION *);

    /* these member functions are called by OSPPSSLSessionNew() */
    int OSPPSSLSessionInitialize(OSPTHTTP *, OSPTSEC *);
    int OSPPSSLSessionNegotiate(OSPTHTTP *);

    /*
     * SSL implementation wrapper functions 
     */
    int OSPPSSLWrapInit(void *);
    void OSPPSSLWrapCleanup(void *);
    int OSPPSSLWrapSessionContextNew(void *, void *);
    int OSPPSSLWrapSessionContextDelete(OSPTSSLSESSION *);
    int OSPPSSLWrapAttachConnection(OSPTSSLSESSION *, void *);
    int OSPPSSLWrapSessionGracefulShutdown(OSPTSSLSESSION *);
    int OSPPSSLWrapHandshake(OSPTSSLSESSION *);
    int OSPPSSLWrapGetData(void *, unsigned int *, OSPTSSLSESSION *);
    int OSPPSSLWrapSendData(void *, unsigned int *, OSPTSSLSESSION *);
    int OSPPSSLWrapGetServerRootCACert(void **, int *, OSPTSSLSESSION *);
    void OSPPSSLWrapFreeServerRootCACert(void **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSSL_H */
