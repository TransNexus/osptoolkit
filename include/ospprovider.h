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
 * ospprovider.h - Prototypes, macros and constants for global provider
 * functions.
 */
#ifndef _OSP_PROVIDER_H
#define _OSP_PROVIDER_H

#include "osp.h"
#include "osptrans.h"
#include "ospcomm.h"
#include "ospsecurity.h"
#include "ospaudit.h"
#include "osptransids.h"

#define     OSPC_PROV_SHIFT  24
/*-------------------------------------------*/
/* provider typedef                          */
/*-------------------------------------------*/ 
typedef struct _OSPTPROVIDER
{
    unsigned char              ProviderInUse;
    OSPTCOMM                   *Comm;
    struct _OSPTTRANCOLLECTION *TransCollection;
    OSPTAUDIT                  *Audit;
    OSPTTXID                   *Id;
    OSPTMUTEX                  ProviderMutex;
    OSPTSEC                    *Security;
    unsigned char              Flags;      /* bit 1 -  Trans Allowed     - 0x01; 
                                                       Trans not allowed - 0x00 */
    unsigned long               TNCustId;
    unsigned long               TNDeviceId;
    OSPTTRANSID                 *TreeRoot;
    OSPTTRANSID                 Sentinel;
    OSPTMUTEX                   TransIdMutex;
} OSPTPROVIDER;

/* MACROS */

#define OSPC_TRANSACTION_ALLOWED    1
#define OSPM_GET_PROV_INDEX_FM_PROV_HANDLE(e)  (e);
#define OSPM_GET_PROV_INDEX_FM_TRANS_HANDLE(e) (e >> OSPC_PROV_SHIFT);

#ifdef __cplusplus
extern "C" 
{
#endif

    /* Function Definitions */

    OSPTPROVIDER *OSPPProviderGetContext(OSPTPROVHANDLE, int *);
    int           OSPPProviderGetNewCollectionItem(OSPTPROVHANDLE *);
    void          OSPPProviderGetNewTransactionAllowed(OSPTPROVIDER *, unsigned char *);
    int           OSPPProviderGetTransactionCollection(OSPTPROVIDER *, 
                      struct _OSPTTRANCOLLECTION **);
    OSPTTRANSID   **OSPPProviderGetTransIdRoot(OSPTPROVIDER *);
    OSPTTRANSID   *OSPPProviderGetTransIdSentinel(OSPTPROVIDER *);
    int           OSPPProviderLockTransIdMutex(OSPTPROVIDER *);
    int           OSPPProviderUnLockTransIdMutex(OSPTPROVIDER *);
    void          OSPPProviderSetNewTransactionAllowed(OSPTPROVIDER *, unsigned char);
    void          OSPPProviderTransactionCollectionDelete(struct _OSPTTRANCOLLECTION **);
    int           OSPPProviderTransactionCollectionNew(struct _OSPTTRANCOLLECTION **);
    int           OSPPProviderInitializeStorage(OSPTPROVIDER *);

    unsigned long OSPPProviderGetTNCustId(OSPTPROVIDER *);
    void          OSPPProviderSetTNCustId(OSPTPROVIDER *, unsigned long);
    unsigned long OSPPProviderGetTNDeviceId(OSPTPROVIDER *);
    void          OSPPProviderSetTNDeviceId(OSPTPROVIDER *, unsigned long);
    int           OSPPProviderGetSecurity(OSPTPROVIDER *, OSPTSEC **);

#ifdef __cplusplus
}
#endif

#endif
