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
 * ospprovider.cpp - Global provider functions.
 */
#include "osp/osp.h"
#include "osp/ospinit.h"    
#include "osp/ospprovider.h"
#include "osp/osptrans.h"

extern OSPTPROVIDER OSPVProviderCollection[];
extern OSPTMUTEX    OSPVProviderMutex;

/*
 * get pointer to current provider object.
 *
 * returns OSPTPROVIDER pointer or OSPC_OSNULL if object not found.
 */
OSPTPROVIDER *
    OSPPProviderGetContext(
    OSPTPROVHANDLE  ospvProvider,       /* In  - Provider handle */
    int             *ospvError)         /* Out - Error code      */
{
    OSPTPROVIDER         *tmpprov = OSPC_OSNULL;
    OSPTCOLLECTIONINDEX  provindex;

    provindex.Index = OSPM_GET_PROV_INDEX_FM_PROV_HANDLE(ospvProvider);

    if ((provindex.Index < OSPC_MAX_PROVIDERS) && 
        (provindex.Index > OSPC_NO_PROVIDERS))
    {

        tmpprov = &OSPVProviderCollection[ospvProvider];
    }

    if(tmpprov == OSPC_OSNULL)
    {
        *ospvError = OSPC_ERR_PROV_HANDLE_INVALID;
        OSPM_DBGERRORLOG(*ospvError, "provider handle is NULL");
    }
    else
    {
        if (tmpprov->ProviderInUse == OSPC_FALSE)
        {
            *ospvError = OSPC_ERR_PROV_HANDLE_INVALID;
            OSPM_DBGERRORLOG(*ospvError, "provider handle not is use");
        }
    }   
    return tmpprov;
}

/*
 * gets handle of first available provider object.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise errorcode.
 */
int
OSPPProviderGetNewCollectionItem(
    OSPTPROVHANDLE  *ospvProvider) /* Out - Provider handle */
{
    int count     = 0;
    int tmperror  = OSPC_ERR_NO_ERROR,
        errorcode = OSPC_ERR_NO_ERROR;

    /*
     * obtain mutex lock on global provider data
     */
    OSPM_MUTEX_LOCK(OSPVProviderMutex, errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* go through provider collection looking for next available */
        for (count = 0; count < OSPC_MAX_PROVIDERS; count++)
        {
            if (OSPVProviderCollection[count].ProviderInUse == 0)
            {
                /* Found one, set up new handle, break out */
                *ospvProvider = (OSPTPROVHANDLE)count;
                OSPVProviderCollection[*ospvProvider].ProviderInUse = OSPC_TRUE;
                break;
            }

        }
        /*
         * release mutex lock on global provider data
         */
        OSPM_MUTEX_UNLOCK(OSPVProviderMutex, tmperror);
    }

    if (errorcode != OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPC_ERR_PROV_MUTEX_FAILURE;
        OSPM_DBGERRORLOG(errorcode, "mutex lock/unlock failed");
    }
    else
    {
        /* There are no available provider objects */
        if (count == OSPC_MAX_PROVIDERS)
        {
            errorcode = OSPC_ERR_PROV_NO_PROV_SPACE;
            OSPM_DBGERRORLOG(errorcode, "max providers reached");
        }
    }

    return errorcode;
}

/* Get New-Transaction-Allowed Flag from Provider Data Structure.
    OSPProviderGetNewTransactionAllowed() retrievs the value of a flag from
    the provider structure and returns it in the ospvNTAFlag parameter.
*/
void 
OSPPProviderGetNewTransactionAllowed(
    OSPTPROVIDER  *ospvProvider,     /* In - ptr to provider structure */
    unsigned char *ospvNTAFlag)     /* Out - flag value */
{
    *ospvNTAFlag = (unsigned char)(ospvProvider->Flags & 
        OSPC_TRANSACTION_ALLOWED);

    return;
}

/*
 * gets pointer to transaction collection within provider object.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise errorcode.
 */
int
OSPPProviderGetTransactionCollection(
    OSPTPROVIDER        *ospvProvider,   /* In - Provider pointer */
    OSPTTRANCOLLECTION  **ospvTranColl)  /* Out - Transaction collection ptr */
{
    int errcode = OSPC_ERR_NO_ERROR;

    *ospvTranColl = ospvProvider->TransCollection;

    if (*ospvTranColl == (OSPTTRANCOLLECTION *)OSPC_OSNULL)
    {

        errcode = OSPC_ERR_PROV_TRANSCOLLECTION_NOT_FOUND;
        OSPM_DBGERRORLOG(errcode, "transaction collection OSPC_OSNULL");
    }

    return errcode;
}

/* Get the **root node for the transaction Id tree.
 */
OSPTTRANSID   **
OSPPProviderGetTransIdRoot(
    OSPTPROVIDER *ospvProvider
)
{
    OSPTTRANSID **transid = OSPC_OSNULL;

    if(ospvProvider != OSPC_OSNULL)
    {
        return (&(ospvProvider->TreeRoot));
    }
    else
    {
        return transid;
    }
}

/* Get the transid tree sentinel node.
 */
OSPTTRANSID   *
OSPPProviderGetTransIdSentinel(
    OSPTPROVIDER *ospvProvider
)
{
    if(ospvProvider != OSPC_OSNULL)
    {
        return &(ospvProvider->Sentinel);
    }
    else
    {
        return (OSPTTRANSID *)OSPC_OSNULL;
    }
}

/* Lock the mutex associated with the transactionid tree.
 */
int           
OSPPProviderLockTransIdMutex(
    OSPTPROVIDER *ospvProvider
)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MUTEX_LOCK(ospvProvider->TransIdMutex, errorcode);

    return errorcode;
}

/* Unlock the mutex associated with the transactionid tree.
 */
int           
OSPPProviderUnLockTransIdMutex(
    OSPTPROVIDER *ospvProvider
)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MUTEX_UNLOCK(ospvProvider->TransIdMutex, errorcode);

    return errorcode;
}

/* Set New-Transaction-Allowed Flag in Provider Data Structure.
    OSPProviderSetNewTransactionAllowed() sets a flag in the provider
    structure to the value of the ospvNTAFlag parameter.  
*/
void 
OSPPProviderSetNewTransactionAllowed(
    OSPTPROVIDER  *ospvProvider,   /* In - ptr to provider structure */
    unsigned char ospvNTAFlag)     /* In - flag value                */
{

    /* Set/Reset the flag OSPC_TRANSACTION_ALLOWED */
    OSPM_SET_FLAG_BIT(ospvProvider->Flags, 
        OSPC_TRANSACTION_ALLOWED, ospvNTAFlag);
    return;
}

/*
 * delete transaction collection.
 *
 * returns void.
 */
void
OSPPProviderTransactionCollectionDelete(
    OSPTTRANCOLLECTION    **ospvTranColl)  /* In  - Transaction Collection ptr */
{
    int                 count = 0;
    OSPTCOLLECTIONINDEX tranidx;

    if ((*ospvTranColl)->NumberOfTrans > 0)
    {
        for (count = 0; count < OSPC_MAX_TRANS; count++)
        {
            if ((*ospvTranColl)->Trans[count] != OSPC_OSNULL)
            {

                tranidx.Index = count;
                OSPPTransactionCollectionRemoveItem(*ospvTranColl, tranidx); 
            }
        }
    }

    if (*ospvTranColl != (OSPTTRANCOLLECTION *)OSPC_OSNULL) 
    {
        OSPM_FREE(*ospvTranColl);
        *ospvTranColl = OSPC_OSNULL;
    }
    return;
}

/*
 * initialize transaction collection.
 *
 * returns OSPC_ERR_NO_ERROR if successful, error code otherwise.
 */
int
OSPPProviderTransactionCollectionNew(
    OSPTTRANCOLLECTION **ospvTransColl) /* In - Addr of ptr to trans coll */
{
    int errcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC(*ospvTransColl, OSPTTRANCOLLECTION, sizeof(OSPTTRANCOLLECTION));
    if (*ospvTransColl == (OSPTTRANCOLLECTION *)OSPC_OSNULL)
    {
        errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errcode, "malloc failed for init trans collection");
    }
    else
    {

        OSPM_MEMSET(*ospvTransColl, 0, sizeof(OSPTTRANCOLLECTION));
    }

    /*
     * initialize the transaction collection mutex
     */
    OSPM_MUTEX_INIT((*ospvTransColl)->TransactionMutex, 0, errcode);

    return errcode;
}


/*
 * initialize new provider storage space
 *
 * returns OSPC_ERR_NO_ERROR if successful, else specific error code
 */
int
OSPPProviderInitializeStorage(
    OSPTPROVIDER *ospvProvider) /* In - Addr of ptr to trans collection */
{
    int errcode = OSPC_ERR_NO_ERROR;

    if (ospvProvider == (OSPTPROVIDER *)OSPC_OSNULL)
    {
        errcode = OSPC_ERR_PROV_NULL_POINTER;
        OSPM_DBGERRORLOG(errcode, "provider is NULL");
    }
    else
    {
        ospvProvider->ProviderInUse = '\0';
    }

    return errcode;
}

/* !!!TK added following functions/methods */
unsigned long 
OSPPProviderGetTNCustId(
    OSPTPROVIDER *ospvProvider)
{
    unsigned long custid = 0;
    if (ospvProvider != OSPC_OSNULL)
    {
        custid = ospvProvider->TNCustId;
    }
    return custid;
}

void
OSPPProviderSetTNCustId(
    OSPTPROVIDER *ospvProvider,
    unsigned long ospvCustId)
{
    if (ospvProvider != OSPC_OSNULL)
    {
        ospvProvider->TNCustId = ospvCustId;
    }
}

unsigned long 
OSPPProviderGetTNDeviceId(
    OSPTPROVIDER *ospvProvider)
{
    unsigned long deviceid = 0;
    if (ospvProvider != OSPC_OSNULL)
    {
        deviceid = ospvProvider->TNDeviceId;
    }
    return deviceid;
}

void
OSPPProviderSetTNDeviceId(
    OSPTPROVIDER *ospvProvider,
    unsigned long ospvDeviceId)
{
    if (ospvProvider != OSPC_OSNULL)
    {
        ospvProvider->TNDeviceId = ospvDeviceId;
    }
}

int
OSPPProviderGetSecurity(
    OSPTPROVIDER *ospvProvider,
    OSPTSEC      **ospvSecurity)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    *ospvSecurity = ospvProvider->Security;

    return errorcode;
}
