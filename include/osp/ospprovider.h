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

#ifndef _OSPPROVIDER_H
#define _OSPPROVIDER_H

#include "osp/osp.h"
#include "osp/osptrans.h"
#include "osp/ospcomm.h"
#include "osp/ospsecurity.h"
#include "osp/ospaudit.h"
#include "osp/osptransids.h"

#define OSPC_PROV_SHIFT  24
/*
 * provider typedef
 */
typedef struct _OSPTPROVIDER {
    unsigned char ProviderInUse;
    OSPTCOMM *Comm;
    OSPTCOMM *CommForCapabilities;
    struct _OSPTTRANCOLLECTION *TransCollection;
    OSPTAUDIT *Audit;
    OSPTTXID *Id;
    OSPTMUTEX ProviderMutex;
    OSPTSEC *Security;
    /* 
     * bit 1 -  Trans Allowed     - 0x01; 
     *          Trans not allowed - 0x00 
     */
    unsigned char Flags;
    unsigned long CustId;
    unsigned long DeviceId;
    OSPTTRANSID *TreeRoot;
    OSPTTRANSID Sentinel;
    OSPTMUTEX TransIdMutex;
} OSPTPROVIDER;

/* MACROS */

#define OSPC_TRANSACTION_ALLOWED    1
#define OSPM_GET_PROV_INDEX_FM_PROV_HANDLE(e)  (e);
#define OSPM_GET_PROV_INDEX_FM_TRANS_HANDLE(e) (e >> OSPC_PROV_SHIFT);

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTPROVIDER *OSPPProviderGetContext(OSPTPROVHANDLE, int *);
    int OSPPProviderGetNewCollectionItem(OSPTPROVHANDLE *);
    void OSPPProviderGetNewTransactionAllowed(OSPTPROVIDER *, unsigned char *);
    int OSPPProviderGetTransactionCollection(OSPTPROVIDER *, struct _OSPTTRANCOLLECTION **);
    OSPTTRANSID **OSPPProviderGetTransIdRoot(OSPTPROVIDER *);
    OSPTTRANSID *OSPPProviderGetTransIdSentinel(OSPTPROVIDER *);
    int OSPPProviderLockTransIdMutex(OSPTPROVIDER *);
    int OSPPProviderUnLockTransIdMutex(OSPTPROVIDER *);
    void OSPPProviderSetNewTransactionAllowed(OSPTPROVIDER *, unsigned char);
    void OSPPProviderTransactionCollectionDelete(struct _OSPTTRANCOLLECTION **);
    int OSPPProviderTransactionCollectionNew(struct _OSPTTRANCOLLECTION **);
    int OSPPProviderInitializeStorage(OSPTPROVIDER *);
    unsigned long OSPPProviderGetCustId(OSPTPROVIDER *);
    void OSPPProviderSetCustId(OSPTPROVIDER *, unsigned long);
    unsigned long OSPPProviderGetDeviceId(OSPTPROVIDER *);
    void OSPPProviderSetDeviceId(OSPTPROVIDER *, unsigned long);
    int OSPPProviderGetSecurity(OSPTPROVIDER *, OSPTSEC **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPPROVIDER_H */
