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
 * osptrans.cpp - Global functions for transaction object.
 */
#include "osp.h"
#include "osptrans.h"
#include "ospprovider.h"
#include "ospsecurity.h"
#include "ospmime.h"
#include "osputils.h"
#include "ospmsgque.h"
#include "ospmsginfo.h"
#include "ospxml.h"
#include "ospmsg.h"
#include "ospcomm.h"
#include "ospauthreq.h"
#include "ospreauthreq.h"
#include "ospauthcnf.h"
#include "ospusagecnf.h"
#include "ospfail.h"
#include "ospaltinfo.h"


/* Build a Reauthorisation Request and put in transaction */
int
OSPPTransactionBuildReauthRequest(
    OSPTTRANS   *ospvTrans,
    unsigned    ospvDuration
)
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPTTOKEN   *token      = OSPC_OSNULL,
        *token1     = OSPC_OSNULL;
    OSPTALTINFO *altinfo    = OSPC_OSNULL,
        *altinfo2   = OSPC_OSNULL;
    OSPTTIME    timestamp   = 0;
 
    if(ospvTrans == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
    }

    if(ospvTrans->ReauthReq == OSPC_OSNULL)
    {
        ospvTrans->ReauthReq = OSPPReauthReqNew();

        if(ospvTrans->ReauthReq == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        }
    }

    /* Set Role  = OSPC_SOURCE */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        /* Set Timestamp */
        timestamp = time(NULL);
        OSPPReauthReqSetTimestamp(ospvTrans->ReauthReq, timestamp);

        OSPPReauthReqSetRole(ospvTrans->ReauthReq, OSPC_SOURCE);

        /* Get Callid from trans->currdest->callid */
        if(ospvTrans->CurrentDest != OSPC_OSNULL)
        {

            if(OSPPDestHasCallId(ospvTrans->CurrentDest))
            {

                OSPPReauthReqSetCallId(ospvTrans->ReauthReq, 
                    OSPPDestGetCallId(ospvTrans->CurrentDest));
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_CALLID_NOT_FOUND;
                OSPM_DBGERRORLOG(errorcode, "Callid not found");
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                /* Get token from trans->currdest->token */
                if(OSPPDestHasToken(ospvTrans->CurrentDest))
                {

                    /* add any tokens */
                    for (token = (OSPTTOKEN *)OSPPDestFirstToken(ospvTrans->CurrentDest);
                        (token != (OSPTTOKEN *)OSPC_OSNULL);
                        token = (OSPTTOKEN *)OSPPDestNextToken(ospvTrans->CurrentDest, token))
                    {

                        token1 = OSPPTokenNew(token->ospmTokenLen, token->ospmTokenVal);
                        if(token1 != OSPC_OSNULL)
                        {
                            OSPPReauthReqAddToken(ospvTrans->ReauthReq, token1);
                        }
                        token = OSPC_OSNULL;
                        token1 = OSPC_OSNULL;
                    }
                }
                else
                {
                    errorcode = OSPC_ERR_TRAN_TOKEN_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "Token not found");
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                /* Get Duration for UsageDetail */
                if(OSPPDestHasLimit(ospvTrans->CurrentDest))
                {

                    OSPPReauthReqSetDuration(ospvTrans->ReauthReq, 
                        ospvDuration);
                }
            }
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
        }
    }

    /* Get Calling number from trans->AuthReq->SourceNumber */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        if(ospvTrans->AuthReq != OSPC_OSNULL)
        {
            if(OSPPAuthReqHasSourceNumber(ospvTrans->AuthReq))
            {
                OSPPReauthReqSetSourceNumber(ospvTrans->ReauthReq, 
                    OSPPAuthReqGetSourceNumber(ospvTrans->AuthReq));
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_SOURCE_NUMBER_NOT_FOUND;
            }

        }
        else
        {
            errorcode = OSPC_ERR_TRAN_AUTH_REQ_NOT_FOUND;
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get Called number from trans->AuthReq->DestinationNumber */
            if(OSPPAuthReqHasDestNumber(ospvTrans->AuthReq))
            {
                OSPPReauthReqSetDestNumber(ospvTrans->ReauthReq, 
                    OSPPAuthReqGetDestNumber(ospvTrans->AuthReq));
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_DEST_NUMBER_NOT_FOUND;
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get device info from trans->AuthReq->ospmAuthReqDeviceInfo */
            if(ospvTrans->AuthReq->ospmAuthReqDeviceInfo != NULL)
            {
                for(altinfo = (OSPTALTINFO *)OSPPListFirst( &(ospvTrans->AuthReq->ospmAuthReqDeviceInfo));
                    altinfo!= (OSPTALTINFO *)OSPC_OSNULL;
                    altinfo = (OSPTALTINFO *)OSPPListNext( &(ospvTrans->AuthReq->ospmAuthReqDeviceInfo), altinfo))
                {

                    altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo), 
                        OSPPAltInfoGetValue(altinfo), 
                        OSPPAltInfoGetType(altinfo));

                    if(altinfo2 != OSPC_OSNULL)
                    {
                        OSPPListAppend(&(ospvTrans->ReauthReq->ospmReauthReqDevInfo), altinfo2);
                    }
                    altinfo = OSPC_OSNULL;
                    altinfo2 = OSPC_OSNULL;
                }
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get SourceAlternates from trans->AuthReq->SourceAlternates */
            if(OSPPAuthReqHasSourceAlt(ospvTrans->AuthReq))
            {
                for (altinfo = (OSPTALTINFO *)OSPPAuthReqFirstSourceAlt(ospvTrans->AuthReq);
                    altinfo != (OSPTALTINFO *)OSPC_OSNULL;
                    altinfo = (OSPTALTINFO *)OSPPAuthReqNextSourceAlt(ospvTrans->AuthReq, altinfo))
                {

                    altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo), 
                        OSPPAltInfoGetValue(altinfo), 
                        OSPPAltInfoGetType(altinfo));

                    if(altinfo2 != OSPC_OSNULL)
                    {
                        OSPPReauthReqAddSourceAlt(ospvTrans->ReauthReq, altinfo2);
                    }
                    altinfo = OSPC_OSNULL;
                    altinfo2 = OSPC_OSNULL;
                }
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get DestinationAlternates from trans->AuthReq->DestinationAlternates */
            if(OSPPAuthReqHasDestinationAlt(ospvTrans->AuthReq))
            {
                for (altinfo = (OSPTALTINFO *)OSPPAuthReqFirstDestinationAlt(ospvTrans->AuthReq);
                    altinfo != (OSPTALTINFO *)OSPC_OSNULL;
                    altinfo = (OSPTALTINFO *)OSPPAuthReqNextDestinationAlt(ospvTrans->AuthReq, altinfo))
                {

                    altinfo2 = OSPPAltInfoNew(OSPPAltInfoGetSize(altinfo), 
                        OSPPAltInfoGetValue(altinfo), 
                        OSPPAltInfoGetType(altinfo));

                    if(altinfo2 != OSPC_OSNULL)
                    {
                        OSPPReauthReqAddDestinationAlt(ospvTrans->ReauthReq, altinfo2);
                    }
                    altinfo = OSPC_OSNULL;
                    altinfo2 = OSPC_OSNULL;
                }
            }
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get Transaction ID from trans->transactionID */
        OSPPReauthReqSetTrxId(ospvTrans->ReauthReq, ospvTrans->TransactionID);
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get TNCustId from trans->provider->tncustid */
        OSPPReauthReqSetTNCustId(ospvTrans->ReauthReq, 
            OSPPProviderGetTNCustId(ospvTrans->Provider));

        /* Get TNDeviceID from trans->provider->tndeviceid */
        OSPPReauthReqSetTNDeviceId(ospvTrans->ReauthReq, 
            OSPPProviderGetTNDeviceId(ospvTrans->Provider));
    }

    return errorcode;

}

/* Build a Usage Indication list
 */
int
OSPPTransactionBuildUsage(
    OSPTTRANS           *ospvTrans, /* In - Pointer to transaction context */
    OSPTUSAGEIND        **ospvUsage,/* In - Pointer to usage to be initialized*/
    OSPTDEST            *ospvDest,  /* In - Pointer to dest associated w/usage*/
    OSPE_MSG_DATATYPES  ospvType    /* In - Indicates what usage to build */
    )
{
    int             errorcode   = OSPC_ERR_NO_ERROR;
    unsigned char   *dest       = OSPC_OSNULL;
    OSPTALTINFO     *altinfo    = OSPC_OSNULL;

    *ospvUsage = OSPPUsageIndNew();

    if(*ospvUsage != OSPC_OSNULL)
    {
        if(ospvType == OSPC_MSG_ARESP)
        {
            /* Originating */
            OSPPUsageIndSetRole(*ospvUsage, OSPC_SOURCE);

            /* Get CallId */
            if(OSPPDestHasCallId(ospvDest))
            {
                OSPPUsageIndSetCallId(*ospvUsage, 
                    OSPPDestGetCallId(ospvDest));
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_CALLID_NOT_FOUND;
                OSPM_DBGERRORLOG(errorcode, "Callid not found");
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                /* Get Source Number (Calling) */
                if(OSPPAuthReqHasSourceNumber(ospvTrans->AuthReq))
                {

                    OSPPUsageIndSetSourceNumber(*ospvUsage, 
                        OSPPAuthReqGetSourceNumber(ospvTrans->AuthReq));
                }
                else
                {
                    /*
                     * It is now acceptable to report source usage indications w/o calling number
                    errorcode = OSPC_ERR_TRAN_SOURCE_NUMBER_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "Source number not found");
                     */
                }
            }

            /* Get Source Alternates (copying a pointer) */    
            if ((errorcode == OSPC_ERR_NO_ERROR) && 
                OSPPAuthReqHasSourceAlt(ospvTrans->AuthReq))
            {
                OSPPUsageIndCopySourceAlt(*ospvUsage, 
                    &((*ospvTrans->AuthReq).ospmAuthReqSourceAlternate));
            }

            /* Get Device Info (copying a pointer) */    
            if ((errorcode == OSPC_ERR_NO_ERROR) && 
                (ospvTrans->AuthReq->ospmAuthReqDeviceInfo != NULL))
            {
                OSPPUsageIndCopyDeviceInfo(*ospvUsage, 
                    &((*ospvTrans->AuthReq).ospmAuthReqDeviceInfo));
            }


            /* Get Destination Number (Called) */
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                if (OSPPDestHasNumber(ospvDest)) 
                {
                    OSPPUsageIndSetDestNumber(*ospvUsage,OSPPDestGetNumber(ospvDest));
                }
                else
                {
                    /*
                     * There was no destination number in the AuthRsp, or it was empty
                     * Use the number in the AuthReq.
                     */
                    if(OSPPAuthReqHasDestNumber(ospvTrans->AuthReq))
                    {

                        OSPPUsageIndSetDestNumber(*ospvUsage, 
                        OSPPAuthReqGetDestNumber(ospvTrans->AuthReq));
                    }
                    else
                    {
                        errorcode = OSPC_ERR_TRAN_DEST_NUMBER_NOT_FOUND;
                        OSPM_DBGERRORLOG(errorcode, "Dest number not found");
                    }
                }
            }


            /* add DestinationSignalAddress to DestinationAlternates for Usage */
            if ((errorcode == OSPC_ERR_NO_ERROR) &&
                OSPPDestHasAddr(ospvDest))
            {
                dest = OSPPDestGetAddr(ospvDest);
                altinfo = OSPPAltInfoNew(strlen((const char *)dest),dest,ospeTransport);

                OSPPUsageIndAddDestinationAlt(*ospvUsage, altinfo);
                altinfo = OSPC_OSNULL;
            }

            /* Get Destination Alternates */
            if ((errorcode == OSPC_ERR_NO_ERROR) && 
                OSPPAuthReqHasDestinationAlt(ospvTrans->AuthReq))
            {
                OSPPUsageIndMoveDestinationAlt(*ospvUsage, 
                    &(ospvTrans->AuthReq->ospmAuthReqDestinationAlternate));
            /* Function above may be patterned after OSPPUsageIndMoveDestinationAlt */
            }

        }
        else if(ospvType == OSPC_MSG_AIND)
        {
            /* Terminating */
            OSPPUsageIndSetRole(*ospvUsage, OSPC_DESTINATION);

            /* Get CallId */
            if(OSPPAuthIndHasCallId(ospvTrans->AuthInd))
            {
                OSPPUsageIndSetCallId(*ospvUsage, 
                    OSPPAuthIndGetCallId(ospvTrans->AuthInd));
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_CALLID_NOT_FOUND;
                OSPM_DBGERRORLOG(errorcode, "Callid not found");
            }

            /* Source Number (Calling)*/
            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                if(OSPPAuthIndHasSourceNumber(ospvTrans->AuthInd))
                {

                    OSPPUsageIndSetSourceNumber(*ospvUsage, 
                        OSPPAuthIndGetSourceNumber(ospvTrans->AuthInd));
                }
                else
                {
                    /*
                     * It is now acceptable to report dest usage indications w/o calling number
                    errorcode = OSPC_ERR_TRAN_SOURCE_NUMBER_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "Source number not found");
                     */
                }
            }

            /* Get Source Alternates */
            if ((errorcode == OSPC_ERR_NO_ERROR) && 
                OSPPAuthIndHasSourceAlt(ospvTrans->AuthInd))
            {
                OSPPUsageIndMoveSourceAlt(*ospvUsage, 
                    &(ospvTrans->AuthInd->ospmAuthIndSourceAlternate));
            }


            /* Get Device Info (copying a pointer) */
            if ((errorcode == OSPC_ERR_NO_ERROR) &&
                (ospvTrans->AuthInd->ospmAuthIndDeviceInfo != NULL))
            {
                OSPPUsageIndMoveDeviceInfo(*ospvUsage,
                    &(ospvTrans->AuthInd->ospmAuthIndDeviceInfo));
            }



            /* Destination Number (CALLED) */
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                if(OSPPAuthIndHasDestNumber(ospvTrans->AuthInd))
                {

                    OSPPUsageIndSetDestNumber(*ospvUsage, 
                        OSPPAuthIndGetDestNumber(ospvTrans->AuthInd));
                }
                else
                {
                    errorcode = OSPC_ERR_TRAN_DEST_NUMBER_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "Dest number not found");
                }
            }

            /* Get Destination Alternates */
            if ((errorcode == OSPC_ERR_NO_ERROR) && 
                OSPPAuthIndHasDestinationAlt(ospvTrans->AuthInd))
            {
                OSPPUsageIndMoveDestinationAlt(*ospvUsage, 
                    &(ospvTrans->AuthInd->ospmAuthIndDestinationAlternate));
            }
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errorcode, "Usage type not found.");
        }
    }
    else
    {
        errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errorcode, "Malloc failed for usage");
    }

    /* set timestamp */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPUsageIndSetTimestamp(*ospvUsage,time(OSPC_OSNULL));

        /* Get Transaction ID */
        if(ospvTrans->HasTransactionID != OSPC_FALSE)
        {
            OSPPUsageIndSetTransactionId(*ospvUsage,
                ospvTrans->TransactionID);
            
            /* Set ComponentID */
            errorcode = OSPPUtilBuildString(ospvTrans->TransactionID, 
                                            OSPPTransactionGetCounter(ospvTrans),
                                            (char **)&((*ospvUsage)->ospmUsageIndComponentId));


            /* Update the componentId Unique counter */
            OSPPTransactionUpdateCounter(ospvTrans);

            /* Set MessageId */
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPUtilBuildString(ospvTrans->TransactionID, 
                                            OSPPTransactionGetCounter(ospvTrans),
                                            (char **)&((*ospvUsage)->ospmUsageIndMessageId));
            }

            /* Update the ComponentId Unique counter again to 
             * keep the MessageId and ComponentId values unique 
             */
            OSPPTransactionUpdateCounter(ospvTrans);
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_TXID_NOT_FOUND;
            OSPM_DBGERRORLOG(errorcode, "Transaction id not found");
        }
    }

    /* TransNexus extensions */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* set Customer Id */
        OSPPUsageIndSetTNCustId(*ospvUsage, OSPPProviderGetTNCustId(ospvTrans->Provider));
        /* set Device Id */
        OSPPUsageIndSetTNDeviceId(*ospvUsage, OSPPProviderGetTNDeviceId(ospvTrans->Provider));
        /* set Fail Reason */
        OSPPUsageIndSetTNFailReason(*ospvUsage, OSPPDestGetTNFailReason(ospvTrans->CurrentDest));
    }
    else
    {
        /* Some error occurred. Get rid of this usage */
        if(ospvUsage != OSPC_OSNULL)
        {
            OSPPUsageIndDelete(ospvUsage);
        }
    }

    return errorcode;
}

/*
 * get pointer to transaction indicated by index.
 *
 * returns OSPTTRANS pointer or OSPC_OSNULL if unsuccessful.
 */
OSPTTRANS *
    OSPPTransactionCollectionGetItem(
    OSPTTRANCOLLECTION  *ospvTranColl, /* In - Transaction Collection pointer */
    OSPTCOLLECTIONINDEX tranindex)     /* In - Index of needed item */
{
    OSPTTRANS   *tmptrans = OSPC_OSNULL;

    if (ospvTranColl->Trans[tranindex.Index] != (OSPTTRANS *)OSPC_OSNULL)
        tmptrans = ospvTranColl->Trans[tranindex.Index];

    return tmptrans;
}

/*
 * gets handle for new transaction.
 *
 * returns valid OSPTTRANHANDLE if successful, 
 *     OSPC_TRAN_HANDLE_INVALID (-1) otherwise 
 */
OSPTTRANHANDLE
    OSPPTransactionCollectionGetNewItem(
    OSPTPROVHANDLE      ospvProvider,  /* In - Provider handle */
    OSPTTRANCOLLECTION  *ospvTranColl, /* In - Transaction collection pointer */
    int                 *ospvError)    /* In - Error object pointer */
{
    int             count         = 0;
    OSPTTRANHANDLE  tmptranhandle = OSPC_TRAN_HANDLE_INVALID; 

    /*
     * go through transaction collection til we find one that is set to 
     * OSPC_OSNULL
     */

    for (count = 0; count < OSPC_MAX_TRANS; count++) 
    {
        if (ospvTranColl->Trans[count] == (OSPTTRANS *)OSPC_OSNULL)
            break;
    }

    /* we looked at all of them and there is no empty slot */
    if (count == OSPC_MAX_TRANS) 
    {
        *ospvError = OSPC_ERR_TRAN_NO_TRANS_SPACE;
        OSPM_DBGERRORLOG(*ospvError, "no empty slot for new transaction.");
    }
    else 
    {
        OSPM_MALLOC(ospvTranColl->Trans[count], OSPTTRANS, sizeof(OSPTTRANS));

        if (ospvTranColl->Trans[count] != (OSPTTRANS *)OSPC_OSNULL) 
        {
            OSPM_MEMSET(ospvTranColl->Trans[count], 0, sizeof(OSPTTRANS));

            /* Set up transaction handle */
            tmptranhandle = (count | (ospvProvider << OSPC_PROV_SHIFT));
        }
        else 
        {
            /* Set to generic malloc failure with this module identifier */
            *ospvError = OSPC_ERR_TRAN_MALLOC_FAILED;
            OSPM_DBGERRORLOG(*ospvError, "malloc new transaction failed.");
        }
    }

    return tmptranhandle;
}

/*
 * set indicated transaction pointer to OSPC_OSNULL.
 *
 * returns void.
 */
void
OSPPTransactionCollectionRemoveItem(
    OSPTTRANCOLLECTION  *ospvTranColl,  /* In - Transaction collection ptr  */
    OSPTCOLLECTIONINDEX ospvTranIndex)  /* In - Index of item to be removed */
{
    ospvTranColl->Trans[ospvTranIndex.Index] = OSPC_OSNULL;
    ospvTranColl->NumberOfTrans--;
    return;
}

/* delete authcnf structure and all associated structures and pointers.
 *
 * returns void
 */
void
OSPPTransactionDeleteAuthCnf(
    OSPTTRANS       *ospvTrans) /* In - Pointer to transaction */
{
    OSPPAuthCnfDelete(&(ospvTrans)->AuthCnf);
    ospvTrans->AuthCnf = (OSPTAUTHCNF *)OSPC_OSNULL; 
    return;
}

/* delete authind structure and all associated structures and pointers.
 *
 * returns void
 */
void
OSPPTransactionDeleteAuthInd(
    OSPTTRANS       *ospvTrans) /* In - Pointer to transaction */
{
    OSPPAuthIndDelete(&(ospvTrans)->AuthInd);
    ospvTrans->AuthInd = (OSPTAUTHIND *)OSPC_OSNULL; 
    return;
}

/* delete reauthrequest structure and all associated structures and pointers.
 *
 * returns void
 */
void
OSPPTransactionDeleteReauthReq(
    OSPTTRANS       *ospvTrans) /* In - Pointer to transaction */
{
    OSPPReauthReqDelete(&(ospvTrans)->ReauthReq);
    ospvTrans->ReauthReq = (OSPTREAUTHREQ *)OSPC_OSNULL; 
    return;
}

/* delete reauthresponse structure and all associated structures and pointers.
 *
 * returns void
 */
void
OSPPTransactionDeleteReauthRsp(
    OSPTTRANS       *ospvTrans) /* In - Pointer to transaction */
{
    OSPPReauthRspDelete(&(ospvTrans)->ReauthRsp);
    ospvTrans->ReauthRsp = (OSPTREAUTHRSP *)OSPC_OSNULL; 
    return;
}

/* delete request structure and all associated structures and pointers.
 *
 * returns void
 */
void
OSPPTransactionDeleteRequest(
    OSPTTRANS       *ospvTrans) /* In - Pointer to transaction */
{
    OSPPAuthReqDelete(&(ospvTrans)->AuthReq);
    ospvTrans->AuthReq = (OSPTAUTHREQ *)OSPC_OSNULL; 
    return;
}

/* 
 * delete response structure and all associated structures and pointers.
 *
 * returns void.
 */
void
OSPPTransactionDeleteResponse(
    OSPTTRANS   *ospvTrans) /* In - Pointer to transaction */
{
    OSPPAuthRspDelete(&(ospvTrans)->AuthRsp);
    ospvTrans->AuthRsp = OSPC_OSNULL;
    return;
}

/* 
 * delete statistics structure and all associated structures and pointers.
 *
 * returns void.
 */
void
OSPPTransactionDeleteStatistics(
    OSPTTRANS   *ospvTrans) /* In - Pointer to transaction */
{
    OSPPStatisticsDelete(&(ospvTrans)->TNStatistics);
    ospvTrans->TNStatistics = OSPC_OSNULL;
    return;
}

/* delete usagecnf structure and all associated structures and pointers.
 *
 * returns void
 */
void
OSPPTransactionDeleteUsageCnf(
    OSPTTRANS       *ospvTrans) /* In - Pointer to transaction */
{
    OSPPUsageCnfDelete(&(ospvTrans)->UsageCnf);
    ospvTrans->UsageCnf = (OSPTUSAGECNF *)OSPC_OSNULL; 
    return;
}

/* delete usageind structure and all associated structures and pointers.
 *
 * returns void
 */
void
OSPPTransactionDeleteUsageInd(
    OSPTTRANS       *ospvTrans) /* In - Pointer to transaction */
{
    OSPTUSAGEIND    *usage = OSPC_OSNULL;

    if((ospvTrans != OSPC_OSNULL) &&
        (ospvTrans->UsageInd != OSPC_OSNULL))
    {
        while(!OSPPListEmpty(&((ospvTrans)->UsageInd)))
        {

            usage = (OSPTUSAGEIND *)OSPPListRemove(&((ospvTrans)->UsageInd));

            if(usage != OSPC_OSNULL)
            {

                OSPPUsageIndDelete(&(usage));
            }
        }  

        OSPPListDelete(&((ospvTrans)->UsageInd));
    }
    return;
}



/*
 * If the transaction has a CapCnf component, it will be deleted
 * and the pointer set to NULL
 */
void
OSPPTransactionDeleteCapCnf(
    OSPTTRANS  *ospvTrans /* In - Pointer to transaction */
)
{
    OSPPCapCnfDelete( &(ospvTrans->CapCnf) );
}



/*  Make sure accumulation is allowed in this state 
*/
void         
OSPPTransactionGetAccumAllowed(
    OSPTTRANS *ospvTrans,
    OSPTBOOL  *ospvAccumAllowed)
{
    switch(ospvTrans->State)
    {
        case    OSPC_TRANSNEW:
        case    OSPC_AUTH_REQUEST_FAIL:
        case    OSPC_AUTH_REQUEST_SUCCESS:
        case    OSPC_GET_DEST_FAIL:
        case    OSPC_REPORT_USAGE_BLOCK:
        case    OSPC_REPORT_USAGE_SUCCESS:
        case    OSPC_VALIDATE_AUTH_FAIL:
        case    OSPC_INITIALIZE_FAIL:
        case    OSPC_REINITIALIZE_FAIL:
        case    OSPC_AUTH_REQUEST_BLOCK:
        case    OSPC_REPORT_USAGE_FAIL:
        *ospvAccumAllowed = OSPC_FALSE;
        break;

        case    OSPC_INITIALIZE_SUCCESS:
        case    OSPC_REINITIALIZE_SUCCESS:
        case    OSPC_VALIDATE_AUTH_SUCCESS:
        case    OSPC_GET_DEST_SUCCESS:
        case    OSPC_ACCUMULATE_SUCCESS:
        case    OSPC_ACCUMULATE_FAIL:
        *ospvAccumAllowed = OSPC_TRUE;
        break;

        default:
        /* unknown state */
        *ospvAccumAllowed = OSPC_FALSE;
    }
    return;
}

/*
 * Get the value of the counter in the transaction structure.
 * Returns stored value as int.
 */
int            
    OSPPTransactionGetCounter(
    OSPTTRANS *ospvTransaction      /* In - Transaction handle */
    )
{
    int returnval = 0;

    if (ospvTransaction != OSPC_OSNULL)
    {
        returnval = ospvTransaction->Counter;  
    }

    return returnval;
}


/*
 * gets pointer to transaction structure.
 *
 * returns pointer if successful, else OSPC_OSNULL.
 */

OSPTTRANS *
    OSPPTransactionGetContext(
    OSPTTRANHANDLE  ospvTransaction,    /* In - Transaction handle */
    int             *ospvError)         /* Out - Error return pointer */
{
    OSPTTRANS           *transctxt = OSPC_OSNULL;
    OSPTTRANCOLLECTION  *trancoll  = OSPC_OSNULL;
    OSPTCOLLECTIONINDEX tranindex;
    OSPTTRANHANDLE      trans      = OSPC_TRAN_HANDLE_INVALID;
    OSPTPROVHANDLE      provider   = OSPC_TRAN_HANDLE_INVALID;
    OSPTPROVIDER        *prov      = OSPC_OSNULL;

    OSPM_MEMSET(&tranindex, 0, sizeof(tranindex));

    if (*ospvError == OSPC_ERR_NO_ERROR) 
    {
        /* Get transaction index. Mask off provider bits */
        trans = OSPM_GET_TRANSACTION_INDEX(ospvTransaction);

        if (trans != OSPC_TRAN_HANDLE_INVALID) 
        {
            /* Get provider index. Top 8 bits of transaction handle. */
            provider = OSPM_GET_PROV_INDEX_FM_TRANS_HANDLE(ospvTransaction);
        }
        else 
        {
            *ospvError = OSPC_ERR_TRAN_HANDLE_INVALID;
            OSPM_DBGERRORLOG(*ospvError, "can't get valid transaction handle.");
        }

        if (provider != OSPC_TRAN_HANDLE_INVALID) 
        {
            /* Get provider context pointer */
            prov = OSPPProviderGetContext(provider, ospvError);
        }
        else 
        {
            *ospvError = OSPC_ERR_TRAN_HANDLE_INVALID;
            OSPM_DBGERRORLOG(*ospvError, "can't get valid provider handle.");
        }

        if (*ospvError == OSPC_ERR_NO_ERROR) 
        {
            /* Get transaction collection pointer */
            *ospvError = OSPPProviderGetTransactionCollection(prov, &trancoll);
        }

        if (*ospvError == OSPC_ERR_NO_ERROR) 
        {
            tranindex.Index = OSPM_GET_TRANSACTION_INDEX(trans);
            transctxt = OSPPTransactionCollectionGetItem(trancoll, tranindex);
        }

        if (transctxt == (OSPTTRANS *)OSPC_OSNULL) 
        {
            *ospvError = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
            OSPM_DBGERRORLOG(*ospvError, "transaction not found.");

        }
    }

    return transctxt;
}

/*
 * determine if delete is allowed or not depending on transaction state.
 *
 * returns void.
 */
void
OSPPTransactionGetDeleteAllowed(
    OSPTTRANS   *ospvTrans,           /* In - Pointer to transaction */
    OSPTBOOL    *ospvDeleteAllowed)   /* Out - Indicates permission to delete */
{
#ifdef OSPC_GK_SIM
    extern char *OSPVDeleteAllowed;
    if (OSPVDeleteAllowed != OSPC_OSNULL)
    {
        *ospvDeleteAllowed = OSPC_TRUE;
        return;
    }
#endif

    /* If NO_DELETE_CHECK is defined, delete will always be allowed */
#ifndef OSP_NO_DELETE_CHECK

    switch(ospvTrans->State)
    {
        case    OSPC_TRANSNEW:
        case    OSPC_AUTH_REQUEST_FAIL:
        case    OSPC_AUTH_REQUEST_SUCCESS:
        case    OSPC_REPORT_USAGE_SUCCESS:
        case    OSPC_VALIDATE_AUTH_FAIL:
        case    OSPC_INITIALIZE_FAIL:
        case    OSPC_REINITIALIZE_FAIL:
        case    OSPC_REPORT_USAGE_FAIL:
        case    OSPC_CAP_IND_FAIL:
        case    OSPC_CAP_IND_SUCCESS:

        *ospvDeleteAllowed = OSPC_TRUE;
        break;

        case    OSPC_REPORT_USAGE_BLOCK:
        case    OSPC_AUTH_REQUEST_BLOCK:
        case    OSPC_VALIDATE_AUTH_SUCCESS:
        case    OSPC_GET_DEST_SUCCESS:
        case    OSPC_INITIALIZE_SUCCESS:
        case    OSPC_REINITIALIZE_SUCCESS:
        case    OSPC_ACCUMULATE_SUCCESS:
        case    OSPC_ACCUMULATE_FAIL:
        *ospvDeleteAllowed = OSPC_FALSE;
        break;

        case    OSPC_GET_DEST_FAIL:
                   if (ospvTrans->HasGetDestSucceeded == OSPC_TRUE)
                      *ospvDeleteAllowed = OSPC_FALSE;
                   else
                      *ospvDeleteAllowed = OSPC_TRUE;
                   break;

        default:
        /* unknown state */
        *ospvDeleteAllowed = OSPC_FALSE;
    }
#else
    *ospvDeleteAllowed = OSPC_TRUE;
#endif /* OSP_NO_DELETE_CHECK */

    return;
}


/* Dests only available in approved states */
int         
OSPPTransactionGetDestAllowed(
    OSPTTRANS *ospvTrans
)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    switch(ospvTrans->State)
    {
        case    OSPC_AUTH_REQUEST_SUCCESS:
        case    OSPC_GET_DEST_FAIL:
        case    OSPC_GET_DEST_SUCCESS:
        case    OSPC_ACCUMULATE_FAIL:
        break;

        case    OSPC_TRANSNEW:
        case    OSPC_INVALID_STATE:
        case    OSPC_AUTH_REQUEST_BLOCK:
        case    OSPC_AUTH_REQUEST_FAIL:
        case    OSPC_INITIALIZE_SUCCESS:
        case    OSPC_INITIALIZE_FAIL:
        case    OSPC_REINITIALIZE_SUCCESS:
        case    OSPC_REINITIALIZE_FAIL:
        case    OSPC_VALIDATE_AUTH_SUCCESS:
        case    OSPC_VALIDATE_AUTH_FAIL:
        case    OSPC_REPORT_USAGE_BLOCK:
        case    OSPC_REPORT_USAGE_FAIL:
        case    OSPC_REPORT_USAGE_SUCCESS:
        case    OSPC_ACCUMULATE_SUCCESS:
        errorcode = OSPC_ERR_TRAN_GET_DEST_NOT_ALLOWED;
        break;

        default:
        /* unknown state */
        errorcode = OSPC_ERR_TRAN_GET_DEST_NOT_ALLOWED;
    }

    return errorcode;
}

int
OSPPTransactionGetDestination(

    OSPTTRANS            *ospvTrans,             
                        /* In  - Transaction pointer */

    enum OSPEFAILREASON  ospvFailureReason,      
                        /* In  - Failure Reason for prev dest req */

    unsigned             ospvSizeOfTimestamp,    
                        /* In  - Max size for timestamp string */

    char                 *ospvValidAfter,        
                        /* Out - Valid After time in string format */

    char                 *ospvValidUntil,        
                        /* Out - Valid Until time in string format */

    unsigned             *ospvTimeLimit,         
                        /* Out - Number of seconds call is authorised for */

    unsigned             *ospvSizeOfCallId,      
                        /* In/Out - Max size for CallId string \
                                    Actual size of CallId string */

    void                 *ospvCallId,            
                        /* Out - Call Id string */

    unsigned             ospvSizeOfCalledNumber, 
                        /* In - Max size of called number */

    char                 *ospvCalledNumber,      
                        /* Out - Called number string */

    unsigned             ospvSizeOfDestination,  
                        /* In - Max size of destination string */

    char                 *ospvDestination,       
                        /* Out - Destination string */

    unsigned             ospvSizeOfDestinationDevice,
                        /* In - Max size of destination device string */

    char                 *ospvDestinationDevice,
                        /* Out - DestinationDevice string */

    unsigned             *ospvSizeOfToken,       
                        /* In/Out - Max size of token string
                                    Actual size of token string */ 

    void                 *ospvToken)             
/* Out - Token string */
{
    int           errorcode   = OSPC_ERR_NO_ERROR,
                  timeflag    = OSPC_TRUE;
    OSPTCALLID    *callid     = OSPC_OSNULL;
    OSPTDEST      *dest       = OSPC_OSNULL;
    OSPTSTATUS    *status     = OSPC_OSNULL;
    OSPTTIME      validtime   = 0;
    unsigned char *destnum    = OSPC_OSNULL,
                  *sigaddr    = OSPC_OSNULL;
    OSPTTOKEN     *token      = OSPC_OSNULL;

    /* Make sure there is enough space for the timestamp
     * copies.
     */
    if(ospvSizeOfTimestamp == 0)
    {
        timeflag = OSPC_FALSE;
    }
    else if(ospvSizeOfTimestamp < OSPC_TIMESTRINGSIZE)
    {
        errorcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
        OSPM_DBGERRORLOG(errorcode, "Not enough space for timestrings.");
    }

    /* We set this in getFirst and check it in getNext, so if it gets
     * here, it IS getFirst calling
     */

    if ((ospvFailureReason == OSPC_FAIL_NONE) &&
        (errorcode == OSPC_ERR_NO_ERROR))
    {
        /*
         * Get the status code for first time calls. If the status code < 200 or > 299
         * then there is no use in continuing.
         */
        if (OSPPAuthRspHasStatus(ospvTrans->AuthRsp) == OSPC_FALSE)
        {
            errorcode = OSPC_ERR_TRAN_STATUS_INVALID;
            OSPM_DBGERRORLOG(errorcode, "status not found");
        }
        else
        {
            status = OSPPAuthRspGetStatus(ospvTrans->AuthRsp);
            if(status != (OSPTSTATUS *)OSPC_OSNULL)
            {
                if (!OSPM_STATUSCODE_SUCCESSFUL(status->ospmStatusCode))
                { 
                    errorcode = status->ospmStatusCode;
                    OSPM_DBGERRORLOG(errorcode, "server returned a status error");
                }
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_STATUS_NOT_FOUND;
                OSPM_DBGERRORLOG(errorcode, "status not found");
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            dest = (OSPTDEST *)OSPPAuthRspFirstDest(ospvTrans->AuthRsp);
            ospvTrans->CurrentDest = dest;
        }
    }
    else
    {
        /* 
         * looking for next destination
         */

        if(ospvTrans->CurrentDest != OSPC_OSNULL)
        {

            dest = (OSPTDEST *)OSPPAuthRspNextDest(ospvTrans->AuthRsp, 
                ospvTrans->CurrentDest);
            /* are we at the end? */
            if(dest != OSPC_OSNULL)
            {
                /* put the failure reason into the destination failure code field 
                 * and move current destination pointer to next dest
                 */

                /*
                 * We had set the ospvFailureReason parameter to DEFAULT_GETNEXTDEST_NO_ERROR when
                 * we had received the application had passed OSPC_FAIL_NONE in the ospvFailureReason.
                 * This was done to identify the parent calling function as GetNext rather than Getfirst
                 * Having done that, now we need to log the FailReason as the actual value thatwas passed.
                 * So, change it back to NONE 
                 */
                if (ospvFailureReason == DEFAULT_GETNEXTDEST_NO_ERROR)
                {
                    ospvFailureReason = OSPC_FAIL_NONE;
                }

                OSPPDestSetTNFailReason(ospvTrans->CurrentDest, ospvFailureReason);

                ospvTrans->CurrentDest = dest;
            }
        }
    }


    /*
     * if no errors have occurred, get the destination information
     */
    if ((dest == (OSPTDEST *)OSPC_OSNULL) &&
        (errorcode == OSPC_ERR_NO_ERROR))
    {

        errorcode = OSPC_ERR_TRAN_DEST_INVALID;
        OSPM_DBGERRORLOG(errorcode, "destination not found");
    }
    else
    {
        if ((errorcode == OSPC_ERR_NO_ERROR) &&
            (timeflag == OSPC_TRUE))
        {
            if(OSPPDestHasValidAfter(dest))
            {
                validtime = OSPPDestGetValidAfter(dest);
                errorcode = OSPPOSTimeCalToString(validtime, ospvValidAfter);
            }
            else
            {
                validtime = OSPC_TIMEMIN;
                errorcode = OSPPOSTimeCalToString(validtime, ospvValidAfter);
            }
        }

        if ((errorcode == OSPC_ERR_NO_ERROR) &&
            (timeflag == OSPC_TRUE))
        {
            if(OSPPDestHasValidUntil(dest))
            {
                validtime = OSPPDestGetValidUntil(dest);
                errorcode = OSPPOSTimeCalToString(validtime, ospvValidUntil);
            }
            else
            {
                validtime = OSPC_TIMEMAX;
                errorcode = OSPPOSTimeCalToString(validtime, ospvValidUntil);
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Is time limit required ? */
            if(OSPPDestHasLimit(dest))
            {
                *ospvTimeLimit = OSPPDestGetLimit(dest);
            }

            if (!OSPPDestHasCallId(dest))
            {
                errorcode = OSPC_ERR_TRAN_CALL_ID_INVALID;
                OSPM_DBGERRORLOG(errorcode, "null pointer for callid");
            }
            else
            {
                callid = OSPPDestGetCallId(dest);
                if (OSPPCallIdGetSize(callid) > *ospvSizeOfCallId)
                {
                    errorcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                    OSPM_DBGERRORLOG(errorcode, "not enough space for callid");
                }
                else
                {
                    /* 
                     * Get call id
                     */
                    OSPM_MEMCPY(ospvCallId, OSPPCallIdGetValue(callid), 
                        OSPPCallIdGetSize(callid)); 
                }
                *ospvSizeOfCallId = OSPPCallIdGetSize(callid);
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            if (!OSPPDestHasNumber(dest))
            {
                /* get dest number from authreq */
                if(OSPPAuthReqHasDestNumber(ospvTrans->AuthReq))
                {
                    destnum = OSPPAuthReqGetDestNumber(ospvTrans->AuthReq);
                }
            }
            else
            {
                destnum = OSPPDestGetNumber(dest);
            }

            if (destnum == OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_TRAN_DEST_INVALID;
                OSPM_DBGERRORLOG(errorcode, "null pointer for dest number.");
            }
            else
            {
                if (ospvSizeOfCalledNumber < strlen((const char *)destnum)+1)
                {
                    errorcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                    OSPM_DBGERRORLOG(errorcode, 
                        "not enough space for called number");
                }
                else
                {
                    /*
                     * Get the destination number 
                     */
                    OSPM_MEMCPY(ospvCalledNumber, 
                        destnum,
                        strlen((const char *)destnum)+1);
                }
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            if (!OSPPDestHasAddr(dest))
            {
                errorcode = OSPC_ERR_TRAN_SIGADDR_INVALID;
                OSPM_DBGERRORLOG(errorcode, "null pointer for signal address.");
            }
            else
            {
                sigaddr = OSPPDestGetAddr(dest);
                if (sigaddr == OSPC_OSNULL)
                {
                    errorcode = OSPC_ERR_TRAN_SIGADDR_INVALID;
                    OSPM_DBGERRORLOG(errorcode, "null pointer for signal address.");
                }
                else
                {
                    if (ospvSizeOfDestination < strlen((const char *)sigaddr)+1)
                    {
                        errorcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errorcode, 
                            "not enough space for signal addr");
                    }
                    else
                    {
                        /*
                         * Get the signal address (Destination) 
                         */
                        OSPM_MEMCPY(ospvDestination, 
                            sigaddr,
                            strlen((const char *)sigaddr)+1);
                    }
                }
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            if (OSPPDestDevHasAddr(dest))
            {
                sigaddr = OSPC_OSNULL;

                sigaddr = OSPPDestDevGetAddr(dest);
                if (sigaddr == OSPC_OSNULL)
                {
                    errorcode = OSPC_ERR_TRAN_SIGADDR_INVALID;
                    OSPM_DBGERRORLOG(errorcode, "null pointer for device address.");
                }
                else
                {
                    if (ospvSizeOfDestinationDevice < strlen((const char *)sigaddr)+1)
                    {
                        errorcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errorcode, 
                            "not enough space for device addr");
                    }
                    else
                    {
                        /*
                         * Get the signal address (DestinationDevice) 
                         */
                        OSPM_MEMCPY(ospvDestinationDevice, 
                            sigaddr,
                            strlen((const char *)sigaddr)+1);
                    }
                }
            }
        }


        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            if (!OSPPDestHasToken(dest))
            {
                errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                OSPM_DBGERRORLOG(errorcode, 
                    "null pointer for token.");
            }
            else
            {
                token =(OSPTTOKEN *)OSPPDestFirstToken(dest);
                if (token == (OSPTTOKEN *)OSPC_OSNULL)
                {
                    errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                    OSPM_DBGERRORLOG(errorcode, 
                        "null pointer for token.");
                }
                else
                {
                    if (*ospvSizeOfToken < (unsigned) OSPPTokenGetSize(token))
                    {
                        errorcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errorcode, 
                            "not enough space for token");
                    }
                    else
                    {
                        /*
                         * Get the token
                         */
                        OSPM_MEMCPY(ospvToken, 
                            OSPPTokenGetValue(token),
                            OSPPTokenGetSize(token));
                    }
                    *ospvSizeOfToken = OSPPTokenGetSize(token);
                }
            }
        }
    }
    return errorcode;
}

/*
 * get new transaction handle.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */
int
OSPPTransactionGetNewContext(
    OSPTPROVHANDLE      ospvProvider,       /* In - Provider handle */
    OSPTTRANHANDLE      *ospvTransaction)   /* Out - Transaction handle */
{

    int                 errorcode    = OSPC_ERR_NO_ERROR;
    unsigned char       transallowed = OSPC_FALSE;
    OSPTPROVIDER        *prov        = OSPC_OSNULL;
    OSPTTRANCOLLECTION  *trancoll    = OSPC_OSNULL;

    prov = OSPPProviderGetContext(ospvProvider, &errorcode);

    if (prov != (OSPTPROVIDER *)OSPC_OSNULL) 
    {
        OSPPProviderGetNewTransactionAllowed(prov, &transallowed);

        if (transallowed == OSPC_TRUE) 
        {
            errorcode = OSPPProviderGetTransactionCollection(prov, &trancoll);
            if (errorcode == OSPC_ERR_NO_ERROR) 
            {
                OSPM_MUTEX_LOCK(trancoll->TransactionMutex,errorcode);
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                    *ospvTransaction = OSPPTransactionCollectionGetNewItem(
                    ospvProvider, trancoll, &errorcode);
                    if (errorcode == OSPC_ERR_NO_ERROR)
                        trancoll->NumberOfTrans++;
                    OSPM_MUTEX_UNLOCK(trancoll->TransactionMutex,errorcode);
                }
            }
        }
        else 
        {
            errorcode = OSPC_ERR_TRAN_TRANSACTION_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errorcode, "new transaction not allowed.");
        }

        if ((errorcode == OSPC_ERR_NO_ERROR) && 
            (*ospvTransaction == OSPC_TRAN_HANDLE_INVALID)) 
        {
            errorcode = OSPC_ERR_TRAN_HANDLE_INVALID;
            OSPM_DBGERRORLOG(errorcode, "new transaction handle not found.");
        }
    }
    return errorcode;
}

void
OSPPTransactionGetReportUsageAllowed(
    OSPTTRANS   *ospvTrans, 
    OSPTBOOL    *ospvReportUsageAllowed)
{
    switch(ospvTrans->State)
    {        
        case    OSPC_REPORT_USAGE_SUCCESS:
        *ospvReportUsageAllowed = OSPC_FALSE;
        break;

        case    OSPC_TRANSNEW:
        case    OSPC_AUTH_REQUEST_FAIL:
        case    OSPC_AUTH_REQUEST_SUCCESS:
        case    OSPC_GET_DEST_FAIL:
        case    OSPC_REPORT_USAGE_BLOCK:
        case    OSPC_VALIDATE_AUTH_FAIL:
        case    OSPC_INITIALIZE_FAIL:
        case    OSPC_REINITIALIZE_FAIL:
        case    OSPC_AUTH_REQUEST_BLOCK:
        case    OSPC_VALIDATE_AUTH_SUCCESS:
        case    OSPC_GET_DEST_SUCCESS:
        case    OSPC_REPORT_USAGE_FAIL:
        case    OSPC_INITIALIZE_SUCCESS:
        case    OSPC_REINITIALIZE_SUCCESS:
        case    OSPC_ACCUMULATE_SUCCESS:
        case    OSPC_ACCUMULATE_FAIL:
        *ospvReportUsageAllowed = OSPC_TRUE;
        break;

        default:
        /* unknown state */
        *ospvReportUsageAllowed = OSPC_FALSE;
    }
}

/*
 * get transaction state.
 *
 * returns void.
 */
void
OSPPTransactionGetState(
    OSPTTRANS               *ospvTrans,      /* In - Transaction pointer */
    OSPE_TRANS_STATE        *ospvState      /* Out - State pointer */
    )
{
    if (ospvTrans != (OSPTTRANS *)OSPC_OSNULL)
    {

        *ospvState =  ospvTrans->State;
    }

    return;
}

/* Get copy of stats from transaction. 
 */
void
OSPPTransactionGetStatistics(
    OSPTTRANS           *ospvTrans,     /* In- pointer to transaction */
    OSPTSTATISTICS      *ospvStats      /* In - pointer to stats struct */
)
{
    if((ospvTrans != OSPC_OSNULL) && (ospvStats != OSPC_OSNULL))
    {
        if(OSPPTransactionHasStatistics(ospvTrans))
        {
            OSPM_MEMCPY(ospvStats, 
                ospvTrans->TNStatistics, 
                sizeof(OSPTSTATISTICS));
        }
    }

    return;
}


/* Does transaction have statistics? */
OSPTBOOL
    OSPPTransactionHasStatistics(
    OSPTTRANS *ospvTrans
    )
{
    OSPTBOOL    hasstats = OSPC_FALSE;

    if(ospvTrans != OSPC_OSNULL)
    {
        if(ospvTrans->TNStatistics != OSPC_OSNULL)
        {
            hasstats = OSPC_TRUE;   
        }
    }

    return hasstats;
}

/*
 * get message ready to go out and place on queue.
 *
 * If new transactions allowed for this Provider, create the SMIME signature, 
 * create the MIME multipart message, put into msginfo structure and place
 * msginfo structure on queue.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */
int
OSPPTransactionPrepareAndQueMessage(
    OSPTTRANS      *ospvTrans,       /* In - Pointer to transaction structure */
    unsigned char  *ospvXMLDoc,      /* In - Pointer to form data */
    unsigned       ospvSizeOfXMLDoc, /* In - Size of form data */
    OSPTMSGINFO    **ospvMsgInfo)    /* In - Pointer to msginfo structure */
{
    int             errorcode        = OSPC_ERR_NO_ERROR;
    unsigned char   *signature       = OSPC_OSNULL;
    unsigned        sizeofsignature  = 0;
    unsigned char   *outgoingmessage = OSPC_OSNULL;
    unsigned        sizeofoutmsg     = 0;
    OSPTSEC         *security        = OSPC_OSNULL;
    OSPTPROVIDER    *provider        = OSPC_OSNULL;
    OSPTCOMM        *commManager     = OSPC_OSNULL;



    errorcode = OSPPTransactionGetProvider(ospvTrans, &provider);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPProviderGetSecurity(provider, &security);
    }

#ifdef ENABLESMIME
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPSecSignatureCreate( security, 
            ospvXMLDoc, ospvSizeOfXMLDoc, 
            &signature, &sizeofsignature,
            OSPC_SEC_SIGNATURE_ONLY);

        if((errorcode == OSPC_ERR_NO_ERROR) && (signature == OSPC_OSNULL))
        {
            OSPM_MALLOC(signature,unsigned char,64);
            if(signature!=OSPC_OSNULL)
            {
                OSPM_STRCPY((char *)signature,"Signature placeholder");
                sizeofsignature=OSPM_STRLEN((char *)signature);
            }
            else
            {
                errorcode=OSPC_ERR_SEC_NO_MEMORY;
            }
        }
    }
#endif

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMimeMessageCreate(
            ospvXMLDoc, ospvSizeOfXMLDoc,
            signature, sizeofsignature, 
            (unsigned char **)&outgoingmessage, 
            &sizeofoutmsg);

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            if (*ospvMsgInfo != (OSPTMSGINFO *)OSPC_OSNULL)
            {

                /* set the content type depending on the presence 
                 * or absence of a signature
                 */
                if(signature != OSPC_OSNULL)
                {
                    OSPM_MALLOC((*ospvMsgInfo)->ContentType, 
                                 unsigned char, 
                                 strlen(OSPC_COMM_MULTI_MSG)+1);
                    if((*ospvMsgInfo)->ContentType != OSPC_OSNULL)
                    {
                        OSPM_MEMSET((*ospvMsgInfo)->ContentType,
                                     0,
                                     strlen(OSPC_COMM_MULTI_MSG)+1);

                        OSPM_MEMCPY((*ospvMsgInfo)->ContentType,
                                     OSPC_COMM_MULTI_MSG,
                                     strlen(OSPC_COMM_MULTI_MSG));
                    }
                }
                else
                {
                    OSPM_MALLOC((*ospvMsgInfo)->ContentType, 
                                 unsigned char, 
                                 strlen(OSPC_COMM_TEXT_MSG)+1);
                    if((*ospvMsgInfo)->ContentType != OSPC_OSNULL)
                    {
                        OSPM_MEMSET((*ospvMsgInfo)->ContentType,
                                     0,
                                     strlen(OSPC_COMM_TEXT_MSG)+1);

                        OSPM_MEMCPY((*ospvMsgInfo)->ContentType,
                                     OSPC_COMM_TEXT_MSG,
                                     strlen(OSPC_COMM_TEXT_MSG));
                    }
                }


            }
            else
            {
                errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            }
        }
        else
        {
            fprintf(stderr, "Create Mime failed\n");
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {  
            OSPPMsgInfoAssignRequestMsg(*ospvMsgInfo,
                outgoingmessage, sizeofoutmsg);

            /*
             * Decide which manager to handof the message too.
             * Assign CapabilitiesIndication to the new CommForCapabilities
             * manager.  For all other types of messages, preserver the original
             * behaviour and assign the messages to the main manager - Comm
             */
            if (OSPC_CAP_IND_BLOCK == ospvTrans->State)
            {
                commManager = ospvTrans->Provider->CommForCapabilities;
            }
            else
            {
                commManager = ospvTrans->Provider->Comm;
            }

            errorcode = OSPPCommAddTransaction(commManager,
                *ospvMsgInfo);

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = (*ospvMsgInfo)->ErrorCode;
            }
        }

    }
    if (signature != (unsigned char *)OSPC_OSNULL)
    {
        OSPM_FREE(signature);
        signature = OSPC_OSNULL;
    }

    return errorcode;
}

/*
 * verifies signature and calls parser to retrieve data from returned message.
 *
 * Extract data and signature from MIME message, verify SMIME signature, 
 * parse data out of message.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */
int
OSPPTransactionProcessReturn(
    OSPTTRANS         *ospvTrans,   /* In - Pointer to transaction */
    OSPTMSGINFO       *ospvMsgInfo) /* In - Pointer to pointer to msginfo structure */
{

    int                errorcode          = OSPC_ERR_NO_ERROR;
    unsigned char      *incomingmsg       = OSPC_OSNULL;
    unsigned char      *incomingsignature = OSPC_OSNULL;
    unsigned           sizeofmsg          = 0;
    unsigned           sizeofsig          = 0;
    void               *resultrsp         = OSPC_OSNULL;
    OSPE_MSG_DATATYPES msgtype            = OSPC_MSG_LOWER_BOUND;
    OSPTAUDIT          *audit             = OSPC_OSNULL;
    OSPTUSAGECNF       *usagecnf          = OSPC_OSNULL;

#ifdef ENABLESMIME
    OSPTPROVIDER        *provider           = OSPC_OSNULL;
    OSPTSEC             *security           = OSPC_OSNULL;
#endif

    if((ospvTrans == (OSPTTRANS *)OSPC_OSNULL) ||
        (ospvMsgInfo == (OSPTMSGINFO *)OSPC_OSNULL))
    {

        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "invalid args");
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        audit = ospvTrans->Provider->Audit;
        /*
         * Parse the incoming message into its MIME components,
         * XML doc and Signature
         */
        errorcode = OSPPMimeMessageParse(
            ospvMsgInfo->ResponseMsg, 
            ospvMsgInfo->ResponseSz,
            ospvMsgInfo->ContentType, 
            ospvMsgInfo->ContentSz,
            &incomingmsg, &sizeofmsg,
            &incomingsignature, &sizeofsig);
    }

#ifdef ENABLESMIME

    /*
     * Validate the S/MIME signature
     */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        if(incomingsignature != OSPC_OSNULL)
        {

            errorcode = OSPPTransactionGetProvider(ospvTrans, &provider);
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPProviderGetSecurity(provider, &security);
            }

            errorcode = OSPPSecSignatureVerify( security,
                &incomingmsg, &sizeofmsg,
                incomingsignature, sizeofsig, OSPC_SEC_SIGNATURE_ONLY);
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_VALUE_NOT_FOUND;
        }
    }
#endif

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /*
         * Send the XML message doc to the XML parser
         */
        errorcode = OSPPXMLMessageParse(incomingmsg, sizeofmsg,
            &resultrsp, &msgtype);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {

            OSPPAuditCheck(audit, resultrsp, msgtype);
            
            switch (msgtype)
            {

                /*
                 * Authorization Response received
                 */
                case OSPC_MSG_ARESP:
                ospvTrans->AuthRsp = (OSPTAUTHRSP*)resultrsp;
                ospvTrans->TransactionID = ospvTrans->AuthRsp->ospmAuthRspTrxId;
                ospvTrans->HasTransactionID = OSPC_TRUE;
                break;

                /*
                 * Usage Confirmation received
                 */
                case OSPC_MSG_UCNF:
               for((usagecnf = (OSPTUSAGECNF *)OSPPListFirst((OSPTLIST *)&(resultrsp)));
                    (usagecnf != OSPC_OSNULL);
                    (usagecnf = (OSPTUSAGECNF *)OSPPListNext((OSPTLIST *)&(resultrsp), usagecnf)))
                {
                    errorcode = OSPPTransactionVerifyUsageCnf(usagecnf);
                }
                break;

                /*
                 * Authorization Confirmation received
                 */
                case OSPC_MSG_ACNF:
                errorcode = OSPPTransactionVerifyAuthCnf((OSPTAUTHCNF *)resultrsp);
                if (resultrsp != OSPC_OSNULL)
                    OSPPAuthCnfDelete((OSPTAUTHCNF **)&resultrsp);
                break;

                /*
                 * Reauthorization Response received
                 */
                case OSPC_MSG_REARESP:
                ospvTrans->ReauthRsp = (OSPTREAUTHRSP*)resultrsp;
                ospvTrans->TransactionID = ospvTrans->ReauthRsp->ospmReauthRspTrxId;
                ospvTrans->HasTransactionID = OSPC_TRUE;
                break;

                case OSPC_MSG_CAPCNF:
                ospvTrans->CapCnf =(OSPTCAPCNF*)resultrsp;
                break;

                /*
                 * The Unknown received
                 */
                default:
                errorcode = OSPC_ERR_DATA_INVALID_TYPE;
                break;
            }
        }
    }


        /*
        ** Delete the usage confirmations
        */

    if(msgtype == OSPC_MSG_UCNF)
    {
        if((OSPTLIST *)&(resultrsp) != OSPC_OSNULL)
        {
            while(!OSPPListEmpty((OSPTLIST *)&(resultrsp)))
            {

                usagecnf = (OSPTUSAGECNF *)OSPPListRemove((OSPTLIST *)&(resultrsp));

                if(usagecnf != OSPC_OSNULL)
                {

                    OSPPUsageCnfDelete(&(usagecnf));
                }
            }  

            OSPPListDelete((OSPTLIST *)&(resultrsp));
        }
    }

    if (usagecnf != OSPC_OSNULL)
    {
        OSPPUsageCnfDelete(&usagecnf);
    }

    if (incomingmsg != (unsigned char *)OSPC_OSNULL)
    {
        OSPM_FREE(incomingmsg);
        incomingmsg = OSPC_OSNULL;
    }

    if (incomingsignature != (unsigned char *)OSPC_OSNULL)
    {
        OSPM_FREE(incomingsignature);
        incomingsignature = OSPC_OSNULL;
    }
    return errorcode;
}

/*
 * create space and load request information.
 *
 * Check and store incoming data
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */
int
OSPPTransactionRequestNew(
    OSPTTRANS   *ospvTrans,                 /* In - Transaction Pointer */
    const char  *ospvSource,                /* In - Source of call      */
    const char  *ospvSourceDevice,          /* In - SourceDevice of call*/
    const char  *ospvCallingNumber,         /* In - Calling number      */
    const char  *ospvCalledNumber,          /* In - Called number       */
    const char  *ospvUser,                  /* In - End user (optional) */

    unsigned    ospvNumberOfCallIds, 
                /* In - Number of call identifiers */

    OSPTCALLID  *ospvCallIds[],             
                /* In - List of call identifiers */

    const char  *ospvPreferredDestinations[],
                /* In - List of preferred destinations for call */

    unsigned    *ospvNumberOfDestinations,  
                /* In\Out - Max number of destinations \
                            Actual number of dests authorised */

    unsigned    *ospvSizeOfDetailLog, 
                /* In\Out - Max size of detail log \
                           Actual size of detail log */

    void        *ospvDetailLog)   /* In\Out - Location of detail log storage */
{
    int         errorcode       = OSPC_ERR_NO_ERROR,
                numbytesrandom  = 0,
                numbytescounter = 0;
    unsigned    i               = 0;
    OSPTTIME    timestamp       = 0;
    OSPTALTINFO *altinfo        = OSPC_OSNULL;
    OSPTCALLID  *callid         = OSPC_OSNULL;
    char         random[OSPC_MAX_RANDOM];
    char         counter[OSPC_MAX_RANDOM];

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);
    OSPM_MEMSET(counter, 0, OSPC_MAX_RANDOM);
    ospvTrans->SizeOfDetailLog = *ospvSizeOfDetailLog;
    ospvTrans->DetailLog       = ospvDetailLog;

    /* so far no error */
    errorcode = OSPC_ERR_NO_ERROR;

    /*
     * create the authorisation request object
     */
    ospvTrans->AuthReq = OSPPAuthReqNew();

    if (ospvTrans->AuthReq == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_DATA_NO_AUTHREQ;
        OSPM_DBGERRORLOG(errorcode, "AuthReq struct not created.");

    }
    else
    {

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /*
             * Fill in the AuthReq structure
             */

            /* ----------------------------------
             * ospmAuthReqTimestamp (Timestamp)
             * ----------------------------------
             */
            timestamp = time(NULL);
            OSPPAuthReqSetTimestamp(ospvTrans->AuthReq, timestamp);

            /* ----------------------------------
             * ospmAuthReqCallId (CallId)
             * ----------------------------------
             */

            /* call ids - create a linked list */
            OSPPListNew((OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqCallId));

            /* now append each requested call id to the list */
            for (i = 0; i < ospvNumberOfCallIds; i++)
            {
                callid = OSPPCallIdNew(ospvCallIds[i]->ospmCallIdLen,
                    ospvCallIds[i]->ospmCallIdVal);

                if(callid != OSPC_OSNULL)
                {
                    OSPPListAppend((OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqCallId),
                        (void *)callid);
                    callid = OSPC_OSNULL;
                }
            }

            /* --------------------------------------
           * ospmAuthReqSourceNumber (SourceInfo)
           * --------------------------------------
           */
            OSPPAuthReqSetSourceNumber(ospvTrans->AuthReq, (const unsigned char *)ospvCallingNumber);

            if(ospvSourceDevice != OSPC_OSNULL && strlen(ospvSourceDevice) > 0)
            {
                OSPPListNew((OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqDeviceInfo));
                altinfo = OSPPAltInfoNew(strlen(ospvSourceDevice),
                                     (const unsigned char *)ospvSourceDevice,
                                      ospeTransport);

                if(OSPC_OSNULL != altinfo)
                {
                    OSPPListAppend((OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqDeviceInfo),
                                altinfo);
                }
                altinfo = OSPC_OSNULL;
            }

            /* --------------------------------------
             * ospmAuthReqSourceAlternate (SourceAlternate)
             * --------------------------------------
             */

            if((ospvSource != OSPC_OSNULL) ||
                (ospvTrans->NetworkId !=OSPC_OSNULL) ||
                (ospvUser != OSPC_OSNULL))
            {

                /* source alternates - create a linked list */
                OSPPListNew((OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqSourceAlternate));

                if(ospvTrans->NetworkId != OSPC_OSNULL)
                {

                    altinfo = OSPPAltInfoNew(strlen(ospvTrans->NetworkId), 
                        (const unsigned char *)ospvTrans->NetworkId,
                        ospeNetwork);

                    if(altinfo != OSPC_OSNULL)
                    {

                        OSPPListAppend(
                            (OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqSourceAlternate),
                            (void *)altinfo);
                    }
                }

                altinfo = OSPC_OSNULL;

                if(ospvSource != OSPC_OSNULL)
                {

                    altinfo = OSPPAltInfoNew(strlen(ospvSource), 
                        (const unsigned char *)ospvSource,
                        ospeTransport);

                    if(altinfo != OSPC_OSNULL)
                    {

                        OSPPListAppend(
                            (OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqSourceAlternate),
                            (void *)altinfo);
                    }
                }

                altinfo = OSPC_OSNULL;

                if(ospvUser != OSPC_OSNULL)
                {

                    altinfo = OSPPAltInfoNew(strlen(ospvUser),
                        (const unsigned char *)ospvUser,
                        ospeSubscriber);

                    if (altinfo != OSPC_OSNULL)
                    {

                        OSPPListAppend(
                            (OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqSourceAlternate),
                            (void *)altinfo);
                    }
                }

                altinfo = OSPC_OSNULL;
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_SOURCE_INVALID;
            }

            /* ----------------------------------------
             * ospmAuthReqDestNumber (DestinationInfo)
             * ----------------------------------------
             */
            OSPPAuthReqSetDestNumber(ospvTrans->AuthReq, (const unsigned char *)ospvCalledNumber);

            /* -------------------------------------------
             * ospmAuthReqDestAlt (DestinationAlternative)
             * --------------------------------------------
             * Preferred destination list.
             */

            altinfo = OSPC_OSNULL;

            if(ospvPreferredDestinations != OSPC_OSNULL)
            {

                /* destination alternates - create a linked list */
                OSPPListNew((OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqDestinationAlternate));

                i = 0;

                while(ospvPreferredDestinations[i] != OSPC_OSNULL)
                {

                    if(*(ospvPreferredDestinations[i]) != '\0')
                    {
                        altinfo = OSPPAltInfoNew(strlen(ospvPreferredDestinations[i]), 
                            (const unsigned char *)ospvPreferredDestinations[i],
                            ospeTransport);


                        if(altinfo != OSPC_OSNULL)
                        {
                            OSPPListAppend(
                                (OSPTLIST *)&(ospvTrans->AuthReq->ospmAuthReqDestinationAlternate),
                                (void *)altinfo);
                        }
                    }
                    else
                    {
                        /* we're done */
                        break;
                    }

                    i++;
                    altinfo = OSPC_OSNULL;
                }
            }

            /* -------------------------------------------
             * ospmAuthReqService (Service)
             * --------------------------------------------
             * Default (Basic Internet telephony service)
             */

            /* -----------------------------------------
             * ospmAuthReqMaxDest (MaximumDestinations)
             * -----------------------------------------
             */
            OSPPAuthReqSetMaxDest(ospvTrans->AuthReq, *ospvNumberOfDestinations);

            /* !!!TK added following lines */
            /* -----------------------------------------
             * ospmAuthReqTNCustId (TransNexus Customer Id)
             * -----------------------------------------
             */
            OSPPAuthReqSetTNCustId(ospvTrans->AuthReq, 
                OSPPProviderGetTNCustId(ospvTrans->Provider));

            /* -----------------------------------------
            * ospmAuthReqTNDeviceId (TransNexus Device Id)
            * -----------------------------------------
            */
            OSPPAuthReqSetTNDeviceId(ospvTrans->AuthReq, 
                OSPPProviderGetTNDeviceId(ospvTrans->Provider));

            /* Set ComponentID. We have to use the random generator
               because there is no transaction id yet.
            */

            numbytesrandom = OSPPUtilGetRandom(random, 0);
            numbytescounter = OSPM_SPRINTF(counter, "%d", OSPPTransactionGetCounter(ospvTrans));

            OSPM_MALLOC((ospvTrans->AuthReq)->ospmAuthReqComponentId, 
                        unsigned char, 
                        numbytesrandom + numbytescounter +1);
            OSPM_MEMSET((ospvTrans->AuthReq)->ospmAuthReqComponentId, 
                        0, 
                        numbytesrandom + numbytescounter +1);
            OSPM_MEMCPY((ospvTrans->AuthReq)->ospmAuthReqComponentId, 
                        random, 
                        numbytesrandom);
            OSPM_MEMCPY(((ospvTrans->AuthReq)->ospmAuthReqComponentId + numbytesrandom), 
                        counter, 
                        numbytescounter);

            /* Update the componentId Unique counter */
            OSPPTransactionUpdateCounter(ospvTrans);

            /* Set MessageId. The reasons are the same as for componentId */
            numbytesrandom = OSPPUtilGetRandom(random, 0);
            numbytescounter = OSPM_SPRINTF(counter, "%d", OSPPTransactionGetCounter(ospvTrans));

            OSPM_MALLOC((ospvTrans->AuthReq)->ospmAuthReqMessageId, 
                        unsigned char, 
                        numbytesrandom + numbytescounter +1);
            OSPM_MEMSET((ospvTrans->AuthReq)->ospmAuthReqMessageId, 
                        0, 
                        numbytesrandom + numbytescounter +1);
            OSPM_MEMCPY((ospvTrans->AuthReq)->ospmAuthReqMessageId, 
                        random, 
                        numbytesrandom);
            OSPM_MEMCPY(((ospvTrans->AuthReq)->ospmAuthReqMessageId + numbytesrandom), 
                        counter, 
                        numbytescounter);

            /* Update the ComponentId Unique counter again to 
             * keep the MessageId and ComponentId values unique. 
             */
            OSPPTransactionUpdateCounter(ospvTrans);
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_TXID_NOT_FOUND;
            OSPM_DBGERRORLOG(errorcode, "Transaction id not found");
        }
    }

    if(errorcode != OSPC_ERR_NO_ERROR)
    {

        OSPPAuthReqDelete(&(ospvTrans->AuthReq));
        if(callid != OSPC_OSNULL)
        {
            OSPPCallIdDelete(&callid);
        }

        if(altinfo != OSPC_OSNULL)
        {
            OSPPAltInfoDelete(&altinfo);
        }
    }

    return errorcode;
}

/* 
 * OSPPTransactionResponseBuild
 * If authrsp is not here create a new one
 * including destination
 * else just add destination
 * Set CurrentDest to point to the newly added destination
 */
int
OSPPTransactionResponseBuild(OSPTTRANS    *ospvTrans, 
                           const char   *ospvDestination, 
                           unsigned     ospvSizeOfCallId,
                           const void   *ospvCallId, 
                           unsigned     ospvSizeOfToken, 
                           const void   *ospvToken)
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPTTIME    t           = time(0);
    OSPTDEST    *dest       = (OSPTDEST *)OSPC_OSNULL;
    OSPTTOKEN   *token      = (OSPTTOKEN *)OSPC_OSNULL;

    /* check for new one */
    if(ospvTrans->AuthRsp == (OSPTAUTHRSP *)OSPC_OSNULL)
    {

        /* create the authorisation response object */
        ospvTrans->AuthRsp = OSPPAuthRspNew();

        if (ospvTrans->AuthRsp == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_DATA_NO_AUTHRSP;
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPPAuthRspSetTimestamp(ospvTrans->AuthRsp,t);
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        /* create the destination object */
        dest = OSPPDestNew(); 
        /* dest = OSPPAuthRspAddDest(ospvTrans->AuthRsp,(OSPTDEST *)OSPC_OSNULL); */

        if (dest == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_DATA_NO_DEST;
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        token = OSPPTokenNew(ospvSizeOfToken, 
            (const unsigned char *)ospvToken);

        if(token != (OSPTTOKEN *)OSPC_OSNULL)
        {
            OSPPDestAddToken(dest, token);
            token = OSPC_OSNULL;
        }
        else
        {
            errorcode = OSPC_ERR_DATA_NO_TOKEN;
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPDestSetCallId(dest, 
                          (const unsigned char *)ospvCallId,
                          ospvSizeOfCallId);

        OSPPDestSetAddr(dest, (const unsigned char *)ospvDestination);

        OSPPAuthRspAddDest(ospvTrans->AuthRsp,dest);

        ospvTrans->CurrentDest = dest;
        dest = OSPC_OSNULL;

    }
    else
    {
        OSPPAuthRspDelete(&(ospvTrans->AuthRsp));
        if(dest != OSPC_OSNULL)
        {
            OSPPDestDelete(&dest);
        }

        if(token != OSPC_OSNULL)
        {
            OSPPTokenDelete(&token);
        }
    }

    return errorcode;
}

/*
 * set transaction state.
 *
 * returns void.
 */
void
OSPPTransactionSetState(
    OSPTTRANS               *ospvTrans,      /* In - Transaction pointer */
    OSPE_TRANS_STATE        ospvState)       /* In - Transaction state */
{
    if (ospvTrans != (OSPTTRANS *)OSPC_OSNULL)
    {
        /* Once there is a successful usage report, do not change the state */
        if(ospvTrans->State != OSPC_REPORT_USAGE_SUCCESS)
        {
            ospvTrans->State = ospvState;
        }
    }
    return;
}

/*
 * Update the counter in the transaction structure. Add one unless it is
 * time to roll over.
 */
void        
OSPPTransactionUpdateCounter(
    OSPTTRANS *ospvTrans        /* In - Transaction pointer */
    )
{
    if ( ospvTrans != OSPC_OSNULL )
    {
        if(ospvTrans->Counter < INT_MAX)
            ospvTrans->Counter++;
        else
            ospvTrans->Counter = 0;
    }

    return;
}

int
OSPPTransactionVerifyAuthCnf(
    OSPTAUTHCNF *ospvAuthCnf)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_ARGUSED(ospvAuthCnf);
    return errorcode;
}

/* Note: this function needs to be expanded to return some specific
 * errorcode for each bad status code.
 */
int
OSPPTransactionVerifyUsageCnf(
    OSPTUSAGECNF *ospvUsageCnf)
{
    OSPTSTATUS *status    = OSPC_OSNULL; 
    int         errorcode = OSPC_ERR_TRAN_STATUS_INVALID;
    unsigned    code      = 0;

    if (OSPPUsageCnfHasStatus(ospvUsageCnf))
    {
        status = OSPPUsageCnfGetStatus(ospvUsageCnf);
        if (OSPPStatusHasCode(status))
        {
            code = OSPPStatusGetCode(status);
            if (OSPM_STATUSCODE_SUCCESSFUL(code))
                errorcode = OSPC_ERR_NO_ERROR;
        }
    }

    return errorcode;
}

int 
OSPPTransactionGetProvider(
    OSPTTRANS *ospvTransaction,
    OSPTPROVIDER **ospvProvider)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    *ospvProvider = ospvTransaction->Provider;

    return errorcode;
}

int         
OSPPTransactionValidateTokenCert(
    OSPTTRANS       *ospvTrans,
    unsigned char   *ospvToken, 
    unsigned        ospvTokenLen
)
{
    int             errorcode   = OSPC_ERR_NO_ERROR;
    OSPTSVCPT       *svcpt      = OSPC_OSNULL;
    OSPTSVCPT       *svcptlist  = OSPC_OSNULL;
    OSPTPROVIDER    *provider   = OSPC_OSNULL;
    unsigned char   *commonname = OSPC_OSNULL;
    unsigned        cnlen       = 0;
    int             found       = OSPC_FALSE;

    errorcode = OSPPTransactionGetProvider(ospvTrans, &provider);

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPCommGetServicePointList(provider->Comm, &svcptlist);
        if(svcptlist == OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_COMM_NO_SVC_PTS;
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPSecGetSignerCertSubjectName(provider->Security,
                                                    ospvToken,
                                                    ospvTokenLen,
                                                    &commonname,
                                                    &cnlen);
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        for(svcpt = (OSPTSVCPT *)OSPPListFirst((OSPTLIST *)&svcptlist);
            svcpt != OSPC_OSNULL;
            svcpt = (OSPTSVCPT *)OSPPListNext((OSPTLIST *)&svcptlist, svcpt))
        {
                fprintf(stderr, "CN:%s\nHN:%s\n", (char *)commonname, (char *)svcpt->HostName);
            if((strstr((const char *)commonname, (const char *)svcpt->HostName)) != OSPC_OSNULL)
            {
                found = OSPC_TRUE;
                break;
            }
        }
    }

    if(!found)
    {
        errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
    }

    if(commonname != OSPC_OSNULL)
    {
        OSPM_FREE(commonname);
    }

    return errorcode;
}
