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
 * ospproviderapi.h - Prototypes for API provider functions.
 */

#ifndef _OSPPROVIDERAPI_H
#define _OSPPROVIDERAPI_H

#include "osp/osp.h"
#include "osp/osppkcs1.h"

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPProviderDelete(OSPTPROVHANDLE, int);
    int OSPPProviderGetAuthorityCertificates(OSPTPROVHANDLE, unsigned, unsigned *, void *[]);
    int OSPPProviderGetHTTPMaxConnections(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetHTTPPersistence(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetHTTPRetryDelay(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetHTTPRetryLimit(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetHTTPTimeout(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetLocalKeys(OSPTPROVHANDLE, OSPTPRIVATEKEY *, unsigned, void *);
    int OSPPProviderGetLocalValidation(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetNumberOfAuthorityCertificates(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetNumberOfServicePoints(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderGetServicePoints(OSPTPROVHANDLE, unsigned, unsigned, char *[]);
    int OSPPProviderGetSSLLifetime(OSPTPROVHANDLE, unsigned *);
    int OSPPProviderNew(unsigned, const char *[], unsigned long[], const char *, const OSPTPRIVATEKEY *, const OSPT_CERT *, unsigned,
            const OSPT_CERT *[], unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, const char *, const char *, OSPTPROVHANDLE *);
    int OSPPProviderSetAuthorityCertificates(OSPTPROVHANDLE, unsigned, const OSPT_CERT *[]);
    int OSPPProviderSetHTTPMaxConnections(OSPTPROVHANDLE, unsigned);
    int OSPPProviderSetHTTPPersistence(OSPTPROVHANDLE, unsigned);
    int OSPPProviderSetHTTPRetryDelay(OSPTPROVHANDLE, unsigned);
    int OSPPProviderSetHTTPRetryLimit(OSPTPROVHANDLE, unsigned);
    int OSPPProviderSetHTTPTimeout(OSPTPROVHANDLE, unsigned);
    int OSPPProviderSetLocalKeys(OSPTPROVHANDLE, const OSPTPRIVATEKEY *, const void *);
    int OSPPProviderSetLocalValidation(OSPTPROVHANDLE, unsigned);
    int OSPPProviderSetServicePoints(OSPTPROVHANDLE, unsigned, unsigned long[], const char *[]);
    int OSPPProviderSetCapabilitiesURLs(OSPTPROVHANDLE, unsigned, unsigned long[], const char *[]);
    int OSPPProviderSetSSLLifetime(OSPTPROVHANDLE, unsigned);
    int OSPPProviderSetSPMessageCount(void *, unsigned long[]);

#ifdef __cplusplus
}
#endif

#endif /* _OSPPROVIDERAPI_H */
