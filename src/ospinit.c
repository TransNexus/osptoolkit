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
 * ospinit.cpp - Provider space initialization.
 */
#include "osp/osp.h"
#include "osp/ospprovider.h"

OSPTPROVIDER    OSPVProviderCollection[OSPC_MAX_PROVIDERS];
OSPTMUTEX       OSPVProviderMutex;

#ifdef OSPC_GK_SIM
char *OSPVDeleteAllowed;
#endif


/*
 * The OSPPInit function performs internal housekeeping necessary to 
 * prepare the SDK software for operation.
 *
 * returns OSPC_ERR_NO_ERROR if successful, OSPC_ERR_XXX otherwise.
 */
int
OSPPInit(OSPTBOOL hw_enabled)
{
    int         providerindex = 0;
    int         errorcode     = OSPC_ERR_NO_ERROR,
                tmperror      = OSPC_ERR_NO_ERROR;

    /*
     * create global provider mutex
     */
    OSPM_MUTEX_INIT(OSPVProviderMutex, NULL, errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        /*
         * cycle thru collection, initializing each element.
         */
        for(providerindex = 0; providerindex < OSPC_MAX_PROVIDERS; providerindex++)
        {
            OSPM_MEMSET(&OSPVProviderCollection[providerindex], 0, 
                sizeof(OSPTPROVIDER));
        }

        /*
         * initialize Winsock Library if necessary
         */
        OSPM_INITWINSOCK(errorcode);

        /*
         * if initialization failed, destroy the mutex and return failure
         */
        if (errorcode != OSPC_ERR_NO_ERROR)
            OSPM_MUTEX_DESTROY(OSPVProviderMutex, tmperror);
    }

    if (errorcode != OSPC_ERR_NO_ERROR)
        errorcode = OSPC_ERR_PROV_INIT_FAILURE;

#ifdef OSPC_GK_SIM
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPVDeleteAllowed = OSPM_GETENV("GKSIM_DELETE_ALLOWED");
    }
#endif

    /*
     * Initialize openssl global parameters
     */
    OSPPOpenSSLInit(hw_enabled);

    return errorcode;
}

void
OSPPCleanup(void)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * called to cleanup any resources allocated by OSPPinit
     */
    OSPM_MUTEX_DESTROY(OSPVProviderMutex, errorcode);
    OSPM_CLEANUPWINSOCK();

    /*
     * Clean up openssl global parameters
     */
    OSPPOpenSSLCleanUp();
    return;
}
