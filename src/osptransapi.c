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
 * osptransapi.cpp - API functions for transaction object.
 */

#include "osp.h"
#include "osptrans.h"
#include "ospprovider.h"
#include "ospxml.h"
#include "ospmsginfo.h"
#include "ospdest.h"
#include "ospstatus.h"
#include "ospusageind.h"
#include "ospauthind.h"
#include "osptokeninfo.h"
#include "ospmsg.h"
#include "ospfail.h"
#include "osputils.h"
#include "osptnprobe.h"
#include "ospstatistics.h"


/*
 * OSPPTransactionGetDestProtocol() :
 * Reports the Protocol Information at the Current destination. 
 * This API should be called right after Calling GetFirstDestination 
 * or should be called right after Calling GetNextDestination.
 * The parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvDestinationProtocol: the memory location in which 
 *                            the Toolkit puts the Destination Protocol.
 *                            The returned value is one of the types 
 *                            defined in OSPE_DEST_PROT.
 * returns OSPC_ERR_NO_ERROR if successful, else a 'Request out of Sequence' errorcode.
 */
int
OSPPTransactionGetDestProtocol(
    OSPTTRANHANDLE  ospvTransaction,    /* In - Transaction handle             */
    OSPE_DEST_PROT *ospvDestinationProtocol)      /* Out - Destination Protocol Info     */
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPTTRANS   *trans      = OSPC_OSNULL;
    OSPTDEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

    if (trans != (OSPTTRANS*) NULL) 
    {
        dest = trans->CurrentDest;
        if (trans->State == OSPC_GET_DEST_SUCCESS)
        {
            if (dest == (OSPTDEST *)NULL)
            {
               errorcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
               OSPM_DBGERRORLOG(errorcode, "Could not find Destination for this Transaction \n");
            }
            else
            {
               *ospvDestinationProtocol = dest->ospmDestProtocol;
            }
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errorcode, "Called API Not In Sequence \n");
        }
    }
    return errorcode;
}


/*
 * OSPPTransactionIsDestOSPEnabled() :
 * Reports whether the destination is OSP Enabled or Not. 
 * This API should be called right after Calling GetFirstDestination 
 * or should be called right after Calling GetNextDestination.
 * The parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvDestinationOSPStatus: the memory location in which 
 *                            the Toolkit puts the destination OSP Status.
 *                            The returned value is one of the types 
 *                            defined in OSPE_DEST_OSP_ENABLED.
 * returns OSPC_ERR_NO_ERROR if successful, else a 'Request out of Sequence' errorcode.
 */
int
OSPPTransactionIsDestOSPEnabled(
    OSPTTRANHANDLE  ospvTransaction,    /* In - Transaction handle             */
    OSPE_DEST_OSP_ENABLED *ospvDestinationOSPStatus)      /* Out - Destination OSP Status     */
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPTTRANS   *trans      = OSPC_OSNULL;
    OSPTDEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

    if (trans != (OSPTTRANS*) NULL) 
    {
        dest = trans->CurrentDest;
        if (trans->State == OSPC_GET_DEST_SUCCESS)
        {
            if (dest == (OSPTDEST *)NULL)
            {
               errorcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
               OSPM_DBGERRORLOG(errorcode, "Could not find Destination for this Transaction \n");
            }
            else
            {
               *ospvDestinationOSPStatus = dest->ospmDestOSPVersion;
            }
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errorcode, "Called API Not In Sequence \n");
        }
    }
    return errorcode;
}


/*
 * OSPPTransactionSetNetworkId()
 *
 * Reports the network id for a particular transaction
 * This function can be used to report any network specific 
 * information to the server.
 * Reporting the termination group info is a good use of this interface
 * The parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvNetworkId: the network specific information to be reported to the server.
 * returns OSPC_ERR_NO_ERROR if successful, else error code.
 */
int
OSPPTransactionSetNetworkId(
    OSPTTRANHANDLE  ospvTransaction,    /* In - Transaction handle             */
    const char*    ospvNetworkId)      /* In - network specific information     */
{
    int         errorcode   = OSPC_ERR_NO_ERROR;
    OSPTTRANS   *trans      = OSPC_OSNULL;
    OSPTALTINFO  *altinfo = OSPC_OSNULL;

    if ((ospvNetworkId == OSPC_OSNULL) ||
        (!(strcmp(ospvNetworkId,""))))
    {
        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "Invalid input for OSPPTransactionSetNetworkId");
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);
    }

    if ((trans != (OSPTTRANS*) NULL) && (errorcode == OSPC_ERR_NO_ERROR))
    {
        if (trans->State == OSPC_REPORT_USAGE_SUCCESS)
        {
            errorcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errorcode,"Calling OSPPTransactionSetNetworkId After Usage has been Reported");
        }

        if (errorcode == OSPC_ERR_NO_ERROR) 
        {
            if (trans->NetworkId != OSPC_OSNULL)
            {
                errorcode =OSPC_ERR_TRAN_DUPLICATE_REQUEST;
                OSPM_DBGERRORLOG(errorcode,"Duplicate Calls to OSPPTransactionSetNetworkId");
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR) 
        {
            if ((trans->AuthReq == OSPC_OSNULL) && (trans->AuthInd == OSPC_OSNULL))
            {
                /* 
                 * Neither Authorization Request nor Validate Authorization has
                 * been called. We dont care if it is the source or destn in 
                 * this case. Just add the info to the transaction handler
                 */
                OSPM_MALLOC(trans->NetworkId,char,strlen(ospvNetworkId)+1);
                if (trans->NetworkId != OSPC_OSNULL)
                {
                    OSPM_MEMCPY(trans->NetworkId,ospvNetworkId,strlen(ospvNetworkId)+1);
                }
                else
                {
                    errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                }
            }
            else
            if ((trans->AuthReq != OSPC_OSNULL) && (trans->AuthInd == OSPC_OSNULL))
            {
                /*
                 * End point is a source, however it is calling the 
                 * function out of sequence as it has already called 
                 * AuthReq, return an error
                 */
                errorcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                OSPM_DBGERRORLOG(errorcode,"Calling OSPPTransactionSetNetworkId After Authorization has been requested");
            }
            else
            {
                /* 
                 * For the other two cases in which AuthInd!= NULL
                 * End point is a destination, and Validate Authorization has already been called
                 * Since out of sequence calls are allowed on the destination, add this to the Authind req
                 * Check if there exists a list for suource alternates.If not then,
                 * Make a new list.
                 */
                OSPM_MALLOC(trans->NetworkId,char,strlen(ospvNetworkId)+1);
                if (trans->NetworkId != OSPC_OSNULL)
                {
                    OSPM_MEMCPY(trans->NetworkId,ospvNetworkId,strlen(ospvNetworkId)+1);
                }
                else
                {
                    errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                }

                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                    if (trans->AuthInd->ospmAuthIndDestinationAlternate == OSPC_OSNULL) 
                    {
                        /*
                         * Make a new list
                         */
                        OSPPListNew((OSPTLIST *)&(trans->AuthInd->ospmAuthIndDestinationAlternate));
                    }
                    /*
                     * add to the list
                     */
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvNetworkId),(const unsigned char *)ospvNetworkId,ospeNetwork);
                    if(altinfo != OSPC_OSNULL)
                    {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->ospmAuthIndDestinationAlternate),(void *)altinfo);
                        altinfo = OSPC_OSNULL;
                    }
                } /* errorcode == OSPC_ERR_NO_ERROR */
            }
        } /* errorcode == OSPC_ERR_NO_ERROR */
    } /* trans != (OSPTTRANS*) NULL */

    return errorcode;
}





/*
 * OSPPTransactionAccumulateOneWayDelay()
 *
 * Accumulates one way delay for transaction.
 *
 * The OSPPTransactionAccumulateOneWayDelay function accumulates one-way
 * delay statistics for the call. It is used to report one way delay from
 * the remote peer to the reporting system. This value may be calculated 
 * by comparing the network time protocol (NTP) timestamp included in RTCP
 * messages from the peer with the local NTP time in the  reporting system.
 * Applications may call this function an unlimited number of times during
 * a transaction, but only after the transaction has been authorised and 
 * before its usage details are reported (i.e. after calling either the 
 * function OSPPTransactionRequestAuthorisation or the function 
 * OSPPTransactionValidateAuthorisation and before calling the function 
 * OSPPTransactionReportUsage). Also, each call to this function must report
 * statistics for a separate and distinct set of measurements. In other words,
 * once OSPPTransactionAccumulateOneWayDelay is successfully called, the 
 * application should discard (at least for subsequent calls to the function)
 * the data and start calculating minimum, mean, variance measures anew.
 * Applications may use this function to report a single sample, or they may
 * report statistical measurements from a collection of samples. 
 * The parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvNumberOfSamples: the number of samples included in these statistics.
 *   ospvMinimum: the minimum delay, in milliseconds, measured within the 
 *      current set of samples. If the function call is used to report a single 
 *      sample, this parameter should indicate that measurement, and it should 
 *      be equal to the value of the ospvMean parameter.
 *   ospvMean: the mean of the delay, in milliseconds, measured within the 
 *      current set of samples. If the function call is used to report a single
 *      sample, this parameter should indicate that measurement, and it should 
 *      be equal to the value of the ospvMinimum parameter.
 *   ospvVariance: the variance of the delay, in square milliseconds, measured
 *      within the current set of samples. If the function call is used to 
 *      report a single sample, this parameter should be zero.
 * The SDK library is able to perform this function without network interaction,
 * and, therefore, does not block for network input or output during its 
 * execution.
 *
 * returns OSPC_ERR_NO_ERROR if successful, else error code.
 */

int
OSPPTransactionAccumulateOneWayDelay(
    OSPTTRANHANDLE  ospvTransaction,    /* In - Transaction handle             */
    unsigned        ospvNumberOfSamples,/* In - Number of samples included     */
    unsigned        ospvMinimum,        /* In - Minimum delay in milliseconds  */
    unsigned        ospvMean,           /* In - Mean of delay in milliseconds  */
    float           ospvVariance)       /* In - Variance of delay in square ms */
{
    int         errorcode   = OSPC_ERR_NO_ERROR,
                tnisnan     = OSPC_TRUE;
    OSPTTRANS   *trans      = OSPC_OSNULL;
    unsigned    currnumber  = 0;
    double      topower     = 0,
                mean        = 0,
                intpart     = 0;
    OSPTBOOL    accumallowed = OSPC_FALSE;
    OSPTDELAY   tmpstats;

    OSPM_MEMSET(&tmpstats, 0, sizeof(OSPTDELAY));

    if((ospvNumberOfSamples == 0) ||
        ((ospvNumberOfSamples == 1) &&
        ((ospvMinimum != ospvMean) ||
        (ospvVariance != 0))))
    {
        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "Invalid input for AccumulateOneWayDelay");
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPTransactionGetAccumAllowed(trans, &accumallowed);
        if(!accumallowed)
        {
            errorcode = OSPC_ERR_TRAN_ACCUMULATE_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errorcode, 
                "AccumulateOneWay not allowed in this transaction state.");
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        /* if no statistics structure, make one */
        if(trans->TNStatistics == OSPC_OSNULL)
        {
            trans->TNStatistics = OSPPStatisticsNew();

            if(trans->TNStatistics == OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
                OSPM_DBGERRORLOG(errorcode, "New failed for stats struct.");
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* make temporary copy so we don't corrupt our accumulator */
            OSPM_MEMCPY(&tmpstats, &(trans->TNStatistics->ospmOneWay), sizeof(OSPTDELAY));

            /* number of measurements */
            currnumber = tmpstats.NumberOfSamples;

            tmpstats.NumberOfSamples += ospvNumberOfSamples;

            /* minimum measured value */
            if(tmpstats.HasValue)
            {
                tmpstats.Minimum = tr_min(tmpstats.Minimum, ospvMinimum);
            }
            else
            {
                tmpstats.Minimum = ospvMinimum;
            }

            /* sample mean - have to cast NumberOfSamples to a float to get some precision
             * on the mean */

            mean = ((tmpstats.Mean * currnumber) + (ospvMean * ospvNumberOfSamples)) / 
                (float)tmpstats.NumberOfSamples;

            /* make sure we don't have overflow */
            OSPM_ISNAN(mean, tnisnan);

            if(tnisnan)
            {
                errorcode = OSPC_ERR_TRAN_INVALID_CALC;
            }
            else
            {
                /* if remainder is >= .5, round up, else round down */
                if(OSPM_MODF(mean, &intpart) >= .5)
                {
                    tmpstats.Mean = (unsigned)OSPM_CEIL(mean);
                }
                else
                {
                    tmpstats.Mean = (unsigned)OSPM_FLOOR(mean);
                }

                /* sum of squares of samples */
                OSPM_POW((double)ospvMean, 2, topower);
                if (topower != OSPC_ERR_POW)
                {
                    tmpstats.SumOfSampSquares = tmpstats.SumOfSampSquares + 
                        ((ospvNumberOfSamples - 1) * (double)ospvVariance) + 
                        (ospvNumberOfSamples * topower);
                    topower = 0;
                }
                else
                {
                    errorcode = (int)topower;
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                /* variance */
                OSPM_POW((double)tmpstats.Mean, 2, topower);
                if(topower != OSPC_ERR_POW)
                {
                    tmpstats.Variance = (float)(tmpstats.SumOfSampSquares - 
                        (tmpstats.NumberOfSamples * topower)) / 
                        (tmpstats.NumberOfSamples - 1);
                    topower = 0;
                }
                else
                {
                    errorcode = (int)topower;
                }
            }

            /* Only set state if we have actually done something, otherwise no
             * change in state.
             */
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                tmpstats.HasValue = OSPC_TRUE;
                trans->TNStatistics->ospmHasOneWay = OSPC_TRUE;
                /* now copy values back to permanent accumulator */
                OSPM_MEMCPY(&(trans->TNStatistics->ospmOneWay), 
                    &(tmpstats), sizeof(OSPTDELAY));
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_SUCCESS);
            }
            else
            {
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_FAIL);    
            }
        }
    }

    return errorcode;
}


/*
 * OSPPTransactionAccumulateRoundTripDelay()
 *
 * Accumulates round trip delay for transaction.
 * 
 * The OSPPTransactionAccumulateRoundTripDelay function accumulates round trip 
 * delay statistics for the call. These measurements can be made using, for 
 * example, H.245 round trip delay requests during the call. Applications may 
 * call this function an unlimited number of times during a transaction, but 
 * only after the transaction has been authorised and before its usage details 
 * are reported (i.e. after calling either the function 
 * OSPPTransactionRequestAuthorisation or the function 
 * OSPPTransactionValidateAuthorisation and before calling the function 
 * OSPPTransactionReportUsage). Also, each call to this function must report 
 * statistics for a separate and distinct set of measurements. In other words, 
 * once OSPPTransactionAccumulateRoundTripDelay is successfully called, 
 * the application should discard (at least for subsequent calls to the function)
 * the data and start calculating minimum, mean, variance measures anew.
 * Applications may use this function to report a single sample, or they may 
 * report statistical measurements from a collection of samples. 
 * The parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvNumberOfSamples: the number of samples included in these statistics.
 *   ospvMinimum: the minimum delay, in milliseconds, measured within the current
 *                set of samples. If the function call is used to report a single
 *                sample, this parameter should indicate that measurement, and it
 *                should be equal to the value of the ospvMean parameter.
 *   ospvMean: the mean of the delay, in milliseconds, measured within the 
 *             current set of samples. If the function call is used to report a 
 *             single sample, this parameter should indicate that measurement, 
 *             and it should be equal to the value of the ospvMinimum parameter.
 *   ospvVariance: the variance of the delay, in square milliseconds, measured 
 *                 within the current set of samples. If the function call is 
 *                 used to report a single sample, this parameter should be zero.
 * The SDK library is able to perform this function without network interaction,
 * and, therefore, does not block for network input or output during its 
 * execution.
 *
 * returns OSPC_ERR_NO_ERROR if successful, else error code.
 */

int
OSPPTransactionAccumulateRoundTripDelay(
    OSPTTRANHANDLE ospvTransaction,    /* In - Transaction handle */
    unsigned       ospvNumberOfSamples,/* In - Number of samples included */
    unsigned       ospvMinimum,        /* In - Minimum delay in milliseconds */
    unsigned       ospvMean,           /* In - Mean of delay in milliseconds */
    float          ospvVariance)       /* In - Variance of delay in square 
                           milliseconds */
{
    int         errorcode   = OSPC_ERR_NO_ERROR,
                tnisnan       = OSPC_TRUE;
    OSPTTRANS   *trans      = OSPC_OSNULL;
    unsigned    currnumber  = 0;
    double      topower     = 0,
                mean        = 0,
                intpart     = 0;
    OSPTBOOL    accumallowed = OSPC_FALSE;
    OSPTDELAY   tmpstats;

    OSPM_MEMSET(&tmpstats, 0, sizeof(OSPTDELAY));

    if((ospvNumberOfSamples == 0) ||
        ((ospvNumberOfSamples == 1) &&
        ((ospvMinimum != ospvMean) ||
        (ospvVariance != 0))))
    {
        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "Invalid input for AccumulateOneWayDelay");
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPTransactionGetAccumAllowed(trans, &accumallowed);
        if(!accumallowed)
        {
            errorcode = OSPC_ERR_TRAN_ACCUMULATE_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errorcode, 
                "AccumulateRoundTrip not allowed in this transaction state.");
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        /* if no statistics structure, make one */
        if(trans->TNStatistics == OSPC_OSNULL)
        {
            trans->TNStatistics = OSPPStatisticsNew();

            if(trans->TNStatistics == OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
                OSPM_DBGERRORLOG(errorcode, "New failed for stats struct.");
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* make temporary copy so we don't corrupt our accumulator */
            OSPM_MEMCPY(&tmpstats, 
                                &(trans->TNStatistics->ospmRoundTrip), 
                                sizeof(OSPTDELAY));

            /* number of measurements */
            currnumber = tmpstats.NumberOfSamples;

            tmpstats.NumberOfSamples += ospvNumberOfSamples;

            /* minimum measured value */
            if(tmpstats.HasValue)
            {
                tmpstats.Minimum = tr_min(tmpstats.Minimum, ospvMinimum);
            }
            else
            {
                tmpstats.Minimum = ospvMinimum;
            }

            /* sample mean - have to cast NumberOfSamples to a float to get some precision
             * on the mean */
            mean = ((tmpstats.Mean * currnumber) + (ospvMean * ospvNumberOfSamples)) / 
                (float)tmpstats.NumberOfSamples;

            OSPM_ISNAN(tmpstats.Mean, tnisnan);

            if(tnisnan)
            {
                errorcode = OSPC_ERR_TRAN_INVALID_CALC;
            }
            else
            {
                /* if remainder is >= .5, round up, else round down */
                if(OSPM_MODF(mean, &intpart) >= .5)
                {
                    tmpstats.Mean = (unsigned)OSPM_CEIL(mean);
                }
                else
                {
                    tmpstats.Mean = (unsigned)OSPM_FLOOR(mean);
                }

                /* sum of squares of samples */
                OSPM_POW((double)ospvMean, 2, topower);
                if(topower != OSPC_ERR_POW)
                {
                    tmpstats.SumOfSampSquares = tmpstats.SumOfSampSquares + 
                        ((ospvNumberOfSamples - 1) * (ospvVariance)) + 
                        ((ospvNumberOfSamples) * (float)topower);
                    topower = 0;
                }
                else
                {
                    errorcode = (int)topower;
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                /* variance */
                OSPM_POW((double)  tmpstats.Mean, 2, topower);
                if(topower != OSPC_ERR_POW)
                { 
                    tmpstats.Variance = (float)(tmpstats.SumOfSampSquares - 
                        (tmpstats.NumberOfSamples * topower)) / 
                        (tmpstats.NumberOfSamples - 1);
                    topower = 0;
                }
                else
                {
                    errorcode = (int)topower;
                }
            }

            /* Only set state if we have actually done something, otherwise no
             * change in state.
             */
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                tmpstats.HasValue = OSPC_TRUE;

                trans->TNStatistics->ospmHasRoundTrip = OSPC_TRUE;
                /* now copy values back to permanent accumulator */
                OSPM_MEMCPY(&(trans->TNStatistics->ospmRoundTrip), 
                    &(tmpstats), sizeof(OSPTDELAY));
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_SUCCESS);
            }
            else
            {
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_FAIL);
            }
        }
    }

    return errorcode;
}

/*
 * OSPPTransactionDelete()
 *
 * Delete the transaction in the transaction collections associated with this 
 * transaction handle.
 *
 * The OSPPTransactionDelete function destroys the ospvTransaction object and 
 * releases the resources it consumes. Once this function is called, the 
 * application is prohibited from subsequent interaction with the object. 
 * (Attempts to do so are refused with an appropriate error code.) The library 
 * may continue to use the transaction's resources, however, until it has 
 * concluded communication regarding this transaction with the settlement 
 * server. An application can ensure the release of all resources 
 * only by specifying a time limit in a call to OSPPProviderDelete.
 *
 * returns OSPC_ERR_NO_ERROR if successful, or error code.
 */

int
OSPPTransactionDelete(
    OSPTTRANHANDLE  ospvTransaction)    /* In - Transaction handle. */
{
    int         errorcode       = OSPC_ERR_NO_ERROR;
    OSPTTRANS   *trans          = OSPC_OSNULL;
    OSPTBOOL    deleteallowed   = OSPC_FALSE;

    OSPTTRANCOLLECTION  *trancoll = NULL;
    OSPTCOLLECTIONINDEX tranindex;

    trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        OSPPTransactionGetDeleteAllowed(trans, &deleteallowed);

        if (deleteallowed == OSPC_TRUE) 
        {
            OSPPTransactionDeleteRequest(trans);

            OSPPTransactionDeleteResponse(trans);

            OSPPTransactionDeleteAuthInd(trans);

            OSPPTransactionDeleteAuthCnf(trans);

            OSPPTransactionDeleteUsageInd(trans);

            OSPPTransactionDeleteUsageCnf(trans);

            OSPPTransactionDeleteStatistics(trans);

            errorcode = OSPPProviderGetTransactionCollection(trans->Provider, &trancoll);

            if (errorcode == OSPC_ERR_NO_ERROR) 
            {
                tranindex.Index = OSPM_GET_TRANSACTION_INDEX(ospvTransaction);
                OSPPTransactionCollectionRemoveItem(trancoll, tranindex);
            }

            if (trans->NetworkId != (char *)OSPC_OSNULL)
            {
                OSPM_FREE(trans->NetworkId);
            }

            if(trans != (OSPTTRANS *)NULL)
            {
                OSPM_FREE(trans);
                trans = NULL;
            }
        }
        else 
        {
            errorcode = OSPC_ERR_TRAN_DELETE_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errorcode, "delete not allowed in this trans state");
        }
    }

    return errorcode;
}

/*
 * OSPPTransactionGetFirstDestination()
 *
 * The OSPPTransactionGetFirstDestination function returns the identity of the
 * first authorised destination for a call. The SDK library obtains this 
 * information during the execution of the OSPPTransactionRequestAuthorisation 
 * function. The parameters to this function consist of the following:
 *  ospvTransaction: handle of the transaction object.
 *  ospvSizeOfTimestamp: size of the character strings (including the 
 *      terminating '\0') in which the function should store validity times 
 *      for the destination. If this value is zero, then validity times are not
 *      returned. If this size is non-zero but not large enough to store either
 *      validity time, then an error is indicated and no destination is 
 *      returned.
 *  ospvValidAfter: character string in which to store the earliest time for 
 *      which the call is authorised to the destination. The format for the 
 *      string is the same as indicated in the OSP protocol specification. 
 *      For example, 3:03 P.M. on May 2, 1997, Eastern Daylight Time in the 
 *          United States is represented as "1997-05-02T19:03:00Z".
 *  ospvValidUntil: character string in which to store the latest time for 
 *      which the call is authorised to the destination. The format for the 
 *      string is the same as for the ospvValidAfter parameter.
 *  ospvTimeLimit: pointer to a variable in which to place the number of 
 *      seconds for which the call is initially authorised. A value of zero 
 *      indicates that no limit exists. Note that the initial time limit may be
 *      extended during the call by either party.
 *  ospvSizeOfCallId: pointer to a variable which, on input, contains the size 
 *      of the memory buffer in which the function should place the H.323 call 
 *      identifier for the destination. If the value is not large enough to 
 *      accommodate the call identifier, then an error is indicated and no 
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the call identifier.
 *  ospvCallId: memory location in which to store the H.323 call identifier 
 *      for the destination. The call identifier returned here is the same 
 *      format as the call identifier passed to the 
 *      OSPPTransactionRequestAuthorisation function.
 *  ospvSizeOfCalledNumber: size of the character string (including the 
 *      terminating '\0') in which the function should store the called number.
 *      If the value is not large enough to accommodate the called number, then
 *      an error is indicated and no destination is returned.
 *  ospvCalledNumber: character string in which to store the called number. In
 *      general, the called number returned here will be the same as the called
 *      number that the application passed to the 
 *      OSPPTransactionRequestAuthorisation function; however, the settlement 
 *      service provider may perform a number translation on the called number,
 *      resulting in a new called number that should be signaled to the peer 
 *      gateway.
 *  ospvSizeOfDestination: size of the character string (including the 
 *      terminating '\0') in which the function should store the destination
 *      information. If the value is not large enough to accommodate the 
 *      destination, then an error is indicated and no destination is returned.
 *  ospvDestination: character string in which to store the identity of the 
 *      destination. The value is expressed as either a DNS name or an IP 
 *      address enclosed in square brackets, followed by an optional colon and
 *      TCP port number. Examples of valid destinations include 
 *                      "gateway1.carrier.com" and "[172.16.1.2]:112".
 *  ospvSizeOfDestinationDevice: size of the character string (including the 
 *      terminating '\0') in which the function should store the destination 
 *      device identity. If the value is not large enough to accommodate the 
 *      destination device identity, then an error is indicated and no 
 *      destination is returned.
 *  ospvDestinationDevice: character string in which to store the identity of 
 *      the destination device in a protocol specific manner (e.g. H.323 
 *      Identifier); this value is optional and, if it is not present, the 
 *      returned string is empty.
 *  ospvSizeOfToken: pointer to a variable which, on input, contains the size 
 *      of the memory buffer in which the function should store the 
 *      authorisation token for the destination. If the value is not large 
 *      enough to accommodate the token, then an error is indicated and no 
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the authorisation token.
 *  ospvToken: memory location in which to store the authorisation token for 
 *      this destination. In general, tokens are opaque, binary objects.
 * The SDK library is able to perform this function without network 
 * interaction, and, therefore, does not block for network input or output 
 * during its execution.
 * The function returns an error code or zero (if the operation was successful).
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */

int
OSPPTransactionGetFirstDestination(
    OSPTTRANHANDLE  ospvTransaction,        /* In  - Transaction handle */
    unsigned        ospvSizeOfTimestamp,    /* In  - Max size for timestamp string */
    char            *ospvValidAfter,        /* Out - Valid After time in string format */
    char            *ospvValidUntil,        /* Out - Valid Until time in string format */
    unsigned        *ospvTimeLimit,         /* Out - Number of seconds call is authorised for */
    unsigned        *ospvSizeOfCallId,      /* In/Out - Max size for CallId string Actual size of CallId string */
    void            *ospvCallId,            /* Out - Call Id string */
    unsigned        ospvSizeOfCalledNumber, /* In - Max size of called number */
    char            *ospvCalledNumber,      /* Out - Called number string */
    unsigned        ospvSizeOfDestination,  /* In - Max size of destination string */
    char            *ospvDestination,       /* Out - Destination string */
    unsigned        ospvSizeOfDestinationDevice,    /* In - Max size of dest device string */
    char            *ospvDestinationDevice,         /* Out - Dest device string */
    unsigned        *ospvSizeOfToken,       /* In/Out - Max size of token string Actual size of token string */ 
    void            *ospvToken)             /* Out - Token string */
{
    int           errorcode   = OSPC_ERR_NO_ERROR;
    OSPTTRANS     *trans      = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

    if((trans != (OSPTTRANS *)NULL) &&
        (errorcode == OSPC_ERR_NO_ERROR))
    {
        errorcode = OSPPTransactionGetDestAllowed(trans);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* 
         * Make sure we have a response
         */
        if (trans->AuthRsp == (OSPTAUTHRSP *)NULL)
            errorcode = OSPC_ERR_TRAN_RESPONSE_NOT_FOUND;

        if(errorcode == OSPC_ERR_NO_ERROR)
        { 
            /*
             * if no errors have occurred, get the destination information
             */

            if ((errorcode == OSPC_ERR_NO_ERROR) && 
                OSPPAuthRspHasDest(trans->AuthRsp) == OSPC_FALSE)
            {
                errorcode = OSPC_ERR_TRAN_DEST_INVALID;
                OSPM_DBGERRORLOG(errorcode, "destination not found");
            }
            else
            {
                errorcode = OSPPTransactionGetDestination(trans,
                    OSPC_FAIL_NONE,
                    ospvSizeOfTimestamp,
                    ospvValidAfter,
                    ospvValidUntil,
                    ospvTimeLimit,
                    ospvSizeOfCallId,
                    ospvCallId,
                    ospvSizeOfCalledNumber,
                    ospvCalledNumber,
                    ospvSizeOfDestination,
                    ospvDestination,
                    ospvSizeOfDestinationDevice,
                    ospvDestinationDevice,
                    ospvSizeOfToken,
                    ospvToken);
            }

            /* Set transaction state */
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPTransactionSetState(trans, OSPC_GET_DEST_SUCCESS);
                trans->HasGetDestSucceeded = OSPC_TRUE;
            }
            else
            {
                OSPPTransactionSetState(trans, OSPC_GET_DEST_FAIL);
            }

        }
    }

    return errorcode;
}

/*
 * OSPPTransactionGetNextDestination()
 *
 * The OSPPTransactionGetNextDestination function returns the identity of the 
 * next authorised destination for a call. Applications may use this function 
 * when attempts to use previously identified authorised destinations (starting
 * with the first destination) fail. The SDK library obtains the necessary 
 * information for this function during its execution of the 
 * OSPPTransactionRequestAuthorisation. The parameters to this function consist
 * of the following:
 *  ospvTransaction: handle of the transaction object.
 *  ospvFailureReason: the reason that attempts to use the previously 
 *      identified destination failed; values for this parameter are listed in
 *      the ospfail.h file.
 *  ospvSizeOfTimestamp: size of the character strings (including the 
 *      terminating '\0') in which the function should store validity times for
 *      the destination. If this value is zero, then validity times are not 
 *      returned. If this size is non-zero but not large enough to store either
 *      validity time, then an error is indicated and no destination is 
 *      returned.
 *  ospvValidAfter: character string in which to store the earliest time for 
 *      which the call is authorised to the destination. The format for the 
 *      string is the same as indicated in the OSP protocol specification. 
 *      For example, 3:03 P.M. on May 2, 1997, Eastern Daylight Time in the 
 *          United States is represented as "1997-05-02T19:03:00Z".
 *  ospvValidUntil: character string in which to store the latest time for 
 *      which the call is authorised to the destination. The format for the 
 *      string is the same as for the ospvValidAfter parameter.
 *  ospvTimeLimit: pointer to a variable in which to place the number of 
 *      seconds for which the call is initially authorised. A value of zero 
 *      indicates that no limit exists. Note that the initial time limit may 
 *      be extended during the call by either party.
 *  ospvSizeOfCallId: pointer to a variable which, on input, contains the size 
 *      of the memory buffer in which the function should place the H.323 call 
 *      identifier for the destination. If the value is not large enough to 
 *      accommodate the call identifier, then an error is indicated and no 
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the call identifier.
 *  ospvCallId: memory location in which to store the H.323 call identifier for
 *      the destination. The call identifier returned here is the same format 
 *      as the call identifier passed to the OSPPTransactionRequestAuthorisation
 *      function.
 *  ospvSizeOfCalledNumber: size of the character string (including the 
 *      terminating '\0') in which the function should store the called number.
 *      If the value is not large enough to accommodate the called number, then
 *      an error is indicated and no destination is returned.
 *  ospvCalledNumber: character string in which to store the called number. In 
 *      general, the called number returned here will be the same as the called
 *      number that the application passed to the 
 *      OSPPTransactionRequestAuthorisation function; however, the settlement 
 *      service provider may perform a number translation on the called number,
 *      resulting in a new called number that should be signaled to the peer 
 *      gateway.
 *  ospvSizeOfDestination: size of the character string (including the 
 *      terminating '\0') in which the function should store the destination 
 *      information. If the value is not large enough to accommodate the destination, 
 *      then an error is indicated and no destination is returned.
 *  ospvDestination: character string in which to store the identity of the 
 *      destination. The value is expressed as either a DNS name or an IP 
 *      address enclosed in square brackets, followed by an optional colon 
 *      and TCP port number. 
 *          Examples of valid destinations include "gateway1.carrier.com" and 
 *                                                          "[172.16.1.2]:112".
 *  ospvSizeOfDestinationDevice: size of the character string (including the 
 *      terminating '\0') in which the function should store the destination 
 *      device identity. If the value is not large enough to accommodate the 
 *      destination device identity, then an error is indicated and no 
 *      destination is returned.
 *  ospvDestinationDevice: character string in which to store the identity 
 *      of the destination device in a protocol specific manner (e.g. H.323 
 *      Identifier); this value is optional and, if it is not present, the 
 *      returned string is empty.
 *  ospvSizeOfToken: pointer to a variable which, on input, contains the size 
 *      of the memory buffer in which the function should store the 
 *      authorisation token for the destination. If the value is not large 
 *      enough to accommodate the token, then an error is indicated and no 
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the authorisation token.
 *  ospvToken: memory location in which to store the authorisation token for 
 *      this destination. In general, tokens are opaque, binary objects.
 * The SDK library is able to perform this function without network 
 * interaction, and, therefore, does not block for network input or output 
 * during its execution.
 * The function returns an error code or zero (if the operation was successful).
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */

int
OSPPTransactionGetNextDestination(
    OSPTTRANHANDLE      ospvTransaction,        /* In - Transaction handle */
    enum OSPEFAILREASON ospvFailureReason,      /* In - Failure code */
    unsigned            ospvSizeOfTimestamp,    /* In - Max size of timestamp string */
    char                *ospvValidAfter,        /* Out - Valid after time string */
    char                *ospvValidUntil,        /* Out - Valid until time string */
    unsigned            *ospvTimeLimit,         /* Out - Number of seconds call is authorised for */
    unsigned            *ospvSizeOfCallId,      /* In - Max size of call id string */
    void                *ospvCallId,            /* Out - Call Id string */
    unsigned            ospvSizeOfCalledNumber, /* In - Max size of called number */
    char                *ospvCalledNumber,      /* Out - Called number string */
    unsigned            ospvSizeOfDestination,  /* In - Max size of destination string */
    char                *ospvDestination,       /* Out - Destination string */
    unsigned            ospvSizeOfDestinationDevice,    /* In - Max size of dest device string */
    char                *ospvDestinationDevice,         /* Out - Dest device string */
    unsigned            *ospvSizeOfToken,       /* In/Out - Max size of token string Actual size of token string */
    void                *ospvToken)             /* Out - Token string */
{
    int          errorcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS    *trans    = NULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPTransactionGetDestAllowed(trans);
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        if (ospvFailureReason == OSPC_FAIL_NONE) 
        {
            errorcode = OSPC_ERR_TRAN_INVALID_FAILURE_CODE;
            OSPM_DBGERRORLOG(errorcode, "failure code invalid");
        }

        /* Now check for acceptable failure code */
        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPFailReasonFind(ospvFailureReason);
        }


        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            if (trans->CurrentDest == (OSPTDEST *)OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_TRAN_NO_MORE_RESPONSES;
                OSPM_DBGERRORLOG(errorcode, "No more destinations.");
            }

        }

        if ((errorcode == OSPC_ERR_NO_ERROR) && 
            (trans != (OSPTTRANS *)NULL)) 
        { 
            errorcode = OSPPTransactionGetDestination(trans,
                ospvFailureReason,
                ospvSizeOfTimestamp,
                ospvValidAfter,
                ospvValidUntil,
                ospvTimeLimit,
                ospvSizeOfCallId,
                ospvCallId,
                ospvSizeOfCalledNumber,
                ospvCalledNumber,
                ospvSizeOfDestination,
                ospvDestination,
                ospvSizeOfDestinationDevice,
                ospvDestinationDevice,
                ospvSizeOfToken,
                ospvToken);
        }

        /* Set transaction state */
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPPTransactionSetState(trans, OSPC_GET_DEST_SUCCESS);
            trans->HasGetDestSucceeded = OSPC_TRUE;
        }
        else
        {
            OSPPTransactionSetState(trans, OSPC_GET_DEST_FAIL);
        }


        if (errorcode == OSPC_ERR_TRAN_DEST_INVALID)
        {
            errorcode = OSPC_ERR_TRAN_NO_MORE_RESPONSES;
        }
    }

    return errorcode;
}

/*
 * OSPPTransactionInitializeAtDevice()
 *
 * The OSPPTransactionInitializeAtDevice function initializes a (newly created)
 * transaction object. Applications can use this with a distributed 
 * architecture in which the systems requesting and validating authorisation 
 * (e.g. H.323 gatekeepers) are different than the systems that ultimately 
 * report usage information (e.g. H.323 gateways). As such, this function is 
 * (in a source device) an alternative to the combination of  the 
 * OSPPTransactionRequestAuthorisation  function (to initiate a call) and the 
 * OSPPTransactionGetFirstDestination function (to define the endpoints of the 
 * call. In the destination device, this function serves as an alternative to 
 * the function  OSPPTransactionValidateAuthorisation. 
 * Parameters to the function are:
 *  ospvTransaction: handle of the (previously created) transaction object.
 *  ospvIsSource: indicates whether the system calling this function is acting 
 *      as the source (if non-zero) or destination (if zero) for the call.
 *  ospvSource: character string identifying the source of the call. The value 
 *      is expressed as either a DNS name or an IP address enclosed in square 
 *      brackets, followed by an optional colon and TCP port number. 
 *      Examples of valid sources include "gateway1.carrier.com" and 
 *                                                  "[172.16.1.2]:112".
 *  ospvDestination: character string identifying the destination for the call. 
 *      The value is expressed as either a DNS name or an IP address enclosed 
 *      in square brackets, followed by an optional colon and TCP port number. 
 *      Examples of valid destinations include "gateway1.carrier.com" and 
 *                                                      "[172.16.1.2]:112".
 *  ospvSourceDevice: character string identifying the source device in a 
 *      protocol specific manner (e.g. H.323 Identifier); this string is 
 *      optional and may be empty.
 *  ospvDestinationDevice: character string identifying the destination device 
 *      in a protocol specific manner (e.g. H.323 Identifier); this string is 
 *      optional and may be empty.
 *  ospvCallingNumber: character string containing the calling party's number 
 *      expressed as a full international number conforming to the ITU E.164 
 *      standard (with no punctuation).
 *  ospvCalledNumber: character string containing the called number, expressed 
 *      as a full international number conforming to the ITU E.164 standard 
 *      (with no punctuation).
 *  ospvSizeOfCallId: size of the memory buffer containing the call identifier.
 *  ospvCallId: memory location containing the H.323 call identifier for the 
 *      call.
 *  ospvSizeOfToken: size of the memory buffer containing an authorisation 
 *      token for the call.
 *  ospvToken: memory location containing an authorisation token.
 *  ospvAuthorised: pointer to a variable in which the function will indicate 
 *      whether or not the call is authorised. On return, a non-zero value 
 *      indicates that the call is authorised by the provider, while a zero 
 *      value indicates an authorisation failure.
 *  ospvTimeLimit: pointer to a variable in which to place the number of 
 *      seconds for which the call is initially authorised. A value of zero 
 *      indicates that no limit exists. Note that the initial time limit may be
 *      extended during the call by using the function 
 *      OSPPTransactionRequestReauthorisation.
 *  ospvSizeOfDetailLog: pointer to a variable which, on input, contains the 
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero, 
 *      applications indicate that they do not wish a detail log for the 
 *      authorisation validation.
 *  ospvDetailLog: pointer to a location in which to store a detail log for the
 *      validation. If this pointer is not NULL, and if the ospvSizeOfDetailLog
 *      parameter is non-zero, then the library will store a copy of the 
 *      authorisation confirmation obtained from the settlement provider, 
 *      including the settlement provider's digital signature.
 * If the provider has been configured to perform local validation, the SDK 
 * library is able to perform this function without network interaction, and, 
 * therefore, does not block for network input or output during its execution. 
 * If local validation is not used, this function blocks until authorisation 
 * has been validated, refused, or an error has been detected. The Open 
 * Settlement Protocol SDK Porting Guide includes information on modifying that
 * behavior to prevent blocking.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
*/

int
OSPPTransactionInitializeAtDevice(
    OSPTTRANHANDLE  ospvTransaction,        /*In - Transaction handle */
    unsigned        ospvIsSource,           /*In - Is this the ogw or tgw */
    const char     *ospvSource,             /*In - Source of call */
    const char     *ospvDestination,        /*In - Destination for call */
    const char     *ospvSourceDevice,       /*In - SourceDevice */
    const char     *ospvDestinationDevice,  /*In - DestinationDevice */
    const char     *ospvCallingNumber,      /*In - Calling number */
    const char     *ospvCalledNumber,       /*In - Called number */
    unsigned        ospvSizeOfCallId,       /*In - Size of Callid */
    const void     *ospvCallId,             /*In - Call identifier */
    unsigned        ospvSizeOfToken,        /*In - Size of Token */
    const void     *ospvToken,              /*In - token */
    unsigned       *ospvAuthorised,         /*Out - indicates authorisation */
    unsigned       *ospvTimeLimit,          /*Out - number of seconds allowed*/
    unsigned       *ospvSizeOfDetailLog,    /* In/Out - Max size of detail log Actual size of detail log */
    void           *ospvDetailLog
)
{
    int                 errorcode   = OSPC_ERR_NO_ERROR;
    OSPTTRANS          *trans       = (OSPTTRANS *)OSPC_OSNULL;
    unsigned            numcallids  = 1;
    OSPTCALLID          *callid     = (OSPTCALLID *)OSPC_OSNULL;

    /* verify input */
    if(((ospvDestination == (const char *)OSPC_OSNULL)&&
        (ospvDestinationDevice == (const char *)OSPC_OSNULL)) ||
        (ospvCallingNumber == (const char *)OSPC_OSNULL) ||
        (ospvCalledNumber == (const char *)OSPC_OSNULL)  ||
        (ospvSizeOfCallId == 0)                          ||
        (ospvCallId == (const void *)OSPC_OSNULL)   ||
        (ospvSizeOfToken == 0)                      ||
        (ospvToken == (const void *)OSPC_OSNULL))
    {
        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "invalid input for Initialize");
    }

    /* Get transaction context */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(ospvTransaction, 
            &errorcode);
    }

    /* call validate */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPTransactionValidateAuthorisation(ospvTransaction, 
            ospvSource,
            ospvDestination, 
            ospvSourceDevice,
            ospvDestinationDevice,
            ospvCallingNumber, ospvCalledNumber,
            ospvSizeOfCallId, ospvCallId, 
            ospvSizeOfToken, ospvToken, 
            ospvAuthorised,
            ospvTimeLimit, ospvSizeOfDetailLog,
            ospvDetailLog);
    }

    /* 
     * move data into authreq & authrsp, (OGW) struct so 
     * report usage will work. 
     * We are adding a destination to authrsp->dest, setting current dest to
     * point to it.
     */

    if((errorcode == OSPC_ERR_NO_ERROR) &&
        (*ospvAuthorised == OSPC_TRAN_AUTHORISED))
    {
        if(ospvIsSource == OSPC_SOURCE)
        {
            if(trans->AuthReq != (OSPTAUTHREQ *)OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                OSPM_DBGERRORLOG(errorcode, "Transaction already initialized");
            }
            else
            {
                /* create callid structure */
                callid = OSPPCallIdNew(ospvSizeOfCallId, (const unsigned char *)ospvCallId);

                if(callid == (OSPTCALLID *)OSPC_OSNULL)
                {
                    errorcode = OSPC_ERR_DATA_NOCALLID;
                }

                if(errorcode == OSPC_ERR_NO_ERROR)
                {
                    /* we need to build authreq and authrsp */
                    errorcode = OSPPTransactionRequestNew(trans, ospvSource,
                        ospvSourceDevice,
                        ospvCallingNumber,
                        ospvCalledNumber, 
                        (const char *)OSPC_OSNULL,
                        numcallids,&callid,
                        (const char **)OSPC_OSNULL,
                        &numcallids, 
                        ospvSizeOfDetailLog, 
                        ospvDetailLog);
                }

                /* delete callid - TransactionRequestNew created new one */
                OSPPCallIdDelete(&callid);
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPTransactionResponseBuild(trans, 
                    ospvDestination,
                    ospvSizeOfCallId,
                    ospvCallId, 
                    ospvSizeOfToken, 
                    ospvToken);
            }

            /* Set correct role */
            OSPPAuthIndSetRole(trans->AuthInd,OSPC_SOURCE);
        }
        else if(ospvIsSource == OSPC_DESTINATION)
        {

            /* authind  already built by validate, just make sure role is correct */
            OSPPAuthIndSetRole(trans->AuthInd,OSPC_DESTINATION);

        }
        else
        {
            errorcode = OSPC_ERR_TRAN_NOT_IMPLEMENTED;
            OSPM_DBGERRORLOG(errorcode, "Invalid system type.");

        }
    }
    else
    {
        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            /* no error from validate, but token not authorised */
            errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
            OSPM_DBGERRORLOG(errorcode, "Token invalid");

        }
    }

    /* Set transaction state */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPTransactionSetState(trans, OSPC_INITIALIZE_SUCCESS);
    }
    else
    {
        OSPPTransactionSetState(trans, OSPC_INITIALIZE_FAIL);
    }

    return errorcode;
}

/*
 * The OSPPTransactionNew function creates a new transaction object for 
 * ospvProvider. A handle to that object is returned to the location pointed 
 * to by ospvTransaction.
 * After calling this function to allocate storage for a transaction object, 
 * applications should call one of the following three functions to initialize 
 * the object:
 *  OSPPTransactionRequestAuthorisation: used by the source of a call.
 *  OSPPTransactionValidateAuthorisation: used by the destination for a call.
 *  OSPPTransactionInitialize: used primarily in architectures that separate 
 *      the call authorisation functions from call setup and usage reporting 
 *      functions.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */

int
OSPPTransactionNew(
    OSPTPROVHANDLE  ospvProvider,       /* In - Provider Handle */
    OSPTTRANHANDLE  *ospvTransaction)   /* Out - Transaction Handle */
{
    int         errorcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS   *trans = NULL;

    errorcode = OSPPTransactionGetNewContext(ospvProvider, ospvTransaction);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(*ospvTransaction, &errorcode);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        trans->Provider = OSPPProviderGetContext(ospvProvider, &errorcode);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPTransactionSetState(trans, OSPC_TRANSNEW);
        trans->HasGetDestSucceeded = OSPC_FALSE;
    }

    return errorcode;
}

/*
 * The OSPPTransactionRecordFailure function allows an application to record 
 * the failure of a call attempt. Applications can use this function when they 
 * wish to abandon a call attempt without exhausting the list of possible 
 * destinations, and in a distributed architecture in which the system 
 * retrieving successive destinations (e.g. an H.323 gatekeeper) is different 
 * than the system that ultimately reports usage information (e.g. an H.323 
 * gateway).
 * The parameters to this function consist of the following:
 *  ospvTransaction: handle of the transaction object.
 *  ospvFailureReason: the reason that attempts to use the previously 
 *      identified destination failed; values for this parameter are listed in 
 *      the ospfail.h file.
 * The SDK library is able to perform this function without network interaction
 * and, therefore, does not block for network input or output during its 
 * execution.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
*/
int
OSPPTransactionRecordFailure(
    OSPTTRANHANDLE       ospvTransaction,
    enum OSPEFAILREASON  ospvFailureReason
)
{
    int         errorcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS  *trans     = (OSPTTRANS *)OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

    if((errorcode == OSPC_ERR_NO_ERROR) &&
        (trans != (OSPTTRANS *)OSPC_OSNULL))
    {

        if (ospvFailureReason == OSPC_FAIL_NONE) 
        {
            errorcode = OSPC_ERR_TRAN_INVALID_FAILURE_CODE;
            OSPM_DBGERRORLOG(errorcode, "failure code invalid");
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {

            errorcode = OSPPFailReasonFind(ospvFailureReason);
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {

            /* Set failure reason for current destination */
            OSPPDestSetTNFailReason(trans->CurrentDest, ospvFailureReason);

        }
    }
    else if(errorcode == OSPC_ERR_NO_ERROR)
    {

        errorcode = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
        OSPM_DBGERRORLOG(errorcode, "Transaction pointer is NULL.");

    }

    return errorcode;
}

/*
* OSPPTransactionReinitializeAtDevice
* The OSPPTransactionReinitializeAtDevice function re-initializes a (previously
* initialized) transaction object. Applications can use this with a distributed
* architecture in which the systems requesting and validating authorisation 
* (e.g. H.323 gatekeepers) are different than the systems that ultimately 
* report usage information (e.g. H.323 gateways). The reporting device can call
* this function after failing to reach a previous destination. As such, this
* function is an alternative to the OSPPTransactionGetNextDestination function.
* Parameters to the function are:
*   ospvTransaction: handle of the (previously created) transaction object.
*   ospvFailureReason: the reason that attempts to use the previously 
*       identified destination failed; values for this parameter are listed in
*       the ospfail.h file.
* ospvIsSource: indicates whether the system calling this function is acting as
*       the source (if non-zero) or destination (if zero) for the call.
* ospvSource: character string identifying the source of the call. The value is
*       expressed as either a DNS name or an IP address enclosed in square 
*       brackets, followed by an optional colon and TCP port number. 
*       Examples of valid sources include "gateway1.carrier.com" and 
*           "[172.16.1.2]:112".
* ospvDestination: character string identifying the destination for the call. 
*       The value is expressed as either a DNS name or an IP address enclosed 
*       in square brackets, followed by an optional colon and TCP port number. 
*       Examples of valid destinations include "gateway1.carrier.com" and 
*           "[172.16.1.2]:112".
* ospvSourceDevice: character string identifying the source device in a 
*       protocol specific manner (e.g. H.323 Identifier); this string is 
*       optional and may be empty.
* ospvDestinationDevice: character string identifying the destination device 
*       in a protocol specific manner (e.g. H.323 Identifier); this string is 
*       optional and may be empty.
* ospvCallingNumber: character string containing the calling party's number 
*       expressed as a full international number conforming to the ITU E.164 
*       standard (with no punctuation).
* ospvCalledNumber: character string containing the called number, expressed 
*       as a full international number conforming to the ITU E.164 standard 
*       (with no punctuation).
* ospvSizeOfCallId: size of the memory buffer containing the call identifier.
* ospvCallId: memory location containing the H.323 call identifier for the 
*       call.
* ospvSizeOfToken: size of the memory buffer containing an authorisation token
*       for the call.
* ospvToken: memory location containing an authorisation token.
* ospvAuthorised: pointer to a variable in which the function will indicate 
*       whether or not the call is authorised. On return, a non-zero value 
*       indicates that the call is authorised by the provider, while a zero 
*       value indicates an authorisation failure.
* ospvTimeLimit: pointer to a variable in which to place the number of seconds
*       for which the call is initially authorised. A value of zero indicates 
*       that no limit exists. Note that the initial time limit may be extended 
*       during the call by using the function 
*       OSPPTransactionRequestReauthorisation.
* ospvSizeOfDetailLog: pointer to a variable which, on input, contains the 
*       maximum size of the detail log; on output, the variable will be updated
*       with the actual size of the detail log. By setting this value to zero, 
*       applications indicate that they do not wish a detail log for the 
*       authorisation validation.
* ospvDetailLog: pointer to a location in which to store a detail log for the 
*       validation. If this pointer is not NULL, and if the ospvSizeOfDetailLog
*       parameter is non-zero, then the library will store a copy of the 
*       authorisation confirmation obtained from the settlement provider, 
*       including the settlement provider's digital signature.
* If the provider has been configured to perform local validation, the SDK 
* library is able to perform this function without network interaction, and, 
* therefore, does not block for network input or output during its execution. 
* If local validation is not used, this function blocks until authorisation has
* been validated, refused, or an error has been detected. The Open Settlement 
* Protocol SDK Porting Guide includes information on modifying that behavior to
* prevent blocking.
* The function returns an error code or zero (if the operation was successful).
* Specific error codes and their meanings can be found in the osperrno.h file.
*/
int
OSPPTransactionReinitializeAtDevice(
    OSPTTRANHANDLE       ospvTransaction,
    enum OSPEFAILREASON  ospvFailureReason,
    unsigned             ospvIsSource,
    const char          *ospvSource,
    const char          *ospvDestination,
    const char          *ospvSourceDevice,
    const char          *ospvDestinationDevice,
    const char          *ospvCallingNumber,
    const char          *ospvCalledNumber,
    unsigned             ospvSizeOfCallId,
    const void          *ospvCallId,
    unsigned             ospvSizeOfToken,
    const void          *ospvToken,
    unsigned            *ospvAuthorised,
    unsigned            *ospvTimeLimit,
    unsigned            *ospvSizeOfDetailLog,
    void                *ospvDetailLog
)
{
    int         errorcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS  *trans     = (OSPTTRANS *)OSPC_OSNULL;

    /* verify input */
    if(((ospvSource == (const char *)OSPC_OSNULL) && 
        (ospvSourceDevice == (const char *)OSPC_OSNULL)) ||
        ((ospvDestination == (const char *)OSPC_OSNULL)&&
        (ospvDestinationDevice == (const char *)OSPC_OSNULL)) ||
        (ospvCallingNumber == (const char *)OSPC_OSNULL) ||
        (ospvCalledNumber == (const char *)OSPC_OSNULL)  ||
        (ospvSizeOfCallId == 0)                          ||
        (ospvCallId == (const void *)OSPC_OSNULL)   ||
        (ospvSizeOfToken == 0)                      ||
        (ospvToken == (const void *)OSPC_OSNULL))
    {

        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "Invalid input for ReInitialize");
    }
    else
    {

        trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

        /* First make sure they sent us a failure code */
        if (ospvFailureReason == OSPC_FAIL_NONE) 
        {
            errorcode = OSPC_ERR_FAILRSN_NO_DATA;
            OSPM_DBGERRORLOG(errorcode, "Failure code missing");
        }

        /* Set failure reason for current destination, validate token and add new
           destination. only on OGW
           */

        /* Now check for acceptable failure code */
        if(errorcode == OSPC_ERR_NO_ERROR)
        {

            errorcode = OSPPFailReasonFind(ospvFailureReason);
        }

        /* call validate */
        if(errorcode == OSPC_ERR_NO_ERROR)
        {

            errorcode = OSPPTransactionValidateAuthorisation(ospvTransaction, 
                ospvSource,
                ospvDestination, 
                ospvSourceDevice,
                ospvDestinationDevice,
                ospvCallingNumber, ospvCalledNumber,
                ospvSizeOfCallId, ospvCallId, 
                ospvSizeOfToken, ospvToken, 
                ospvAuthorised,
                ospvTimeLimit, ospvSizeOfDetailLog,
                ospvDetailLog);
        }

        if((errorcode == OSPC_ERR_NO_ERROR) &&
            (*ospvAuthorised == OSPC_TRAN_AUTHORISED))
        {
            /* should only be called by OGW */
            if(ospvIsSource == OSPC_SOURCE)
            {
                /* we are only adding a destination */
                /* first set failure code in authrsp->currentdest */
                OSPPDestSetTNFailReason(trans->CurrentDest, ospvFailureReason);
                /* now build new dest */
                errorcode = OSPPTransactionResponseBuild(trans, 
                    ospvDestination, ospvSizeOfCallId,
                    ospvCallId, ospvSizeOfToken, ospvToken);
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_NOT_IMPLEMENTED;
                OSPM_DBGERRORLOG(errorcode, "Invalid system type.");
            }

        }
        else
        {
            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                /* no error from validate, but token not authorised */
                errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                OSPM_DBGERRORLOG(errorcode, "Token invalid");

            }
        }

    } /* end else (parameters are correct) */

    /* Set transaction state */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPTransactionSetState(trans, OSPC_REINITIALIZE_SUCCESS);
    }
    else
    {
        OSPPTransactionSetState(trans, OSPC_REINITIALIZE_FAIL);
    }

    return errorcode;
}

/*
 * OSPPTransactionReportUsage()
 *
 * Reports usage information for a call.
 *
 * The OSPPTransactionReportUsage function reports usage information for a call
 * Once this function returns successfully, it may not be called again for the 
 * life of the transaction object. Parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvDuration: the duration of the call, in seconds.
 *   ospvStartTime: the call start time.
 *   ospvLossPacketsSent: a count of the total number of packets sent by the 
 *      reporting system that were not received by its peer, as reported in 
 *      the peer's RTCP sender and receiver reports.
 *   ospvLossFractionSent: the fraction of packets sent by the reporting system
 *      that were not received by its peer, as reported in the peer's RTCP 
 *      sender and receiver reports. The fraction is expressed as an integer 
 *      number from 0 (no loss) to 255 (total loss).
 *   ospvLossPacketsReceived: a count of the total number of packets that the 
 *      reporting system expected to receive but did not, as reported in the 
 *      system's RTCP sender and receiver reports.
 *   ospvLossFractionReceived: the fraction of packets that the reporting 
 *      system expected to receive but did not, as reported in it's RTCP sender
 *      and receiver reports. The fraction is expressed as an integer number 
 *      from 0 (no loss) to 255 (total loss).
 *   ospvSizeOfDetailLog: pointer to a variable which, on input, contains the 
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero, 
 *      applications indicate that they do not wish a detail log for the usage 
 *      report.
 *   ospvDetailLog: pointer to a location in which to store a detail log for 
 *      the usage report. If this pointer is not NULL, and if the 
 *      ospvSizeOfDetailLog parameter is non-zero, then the library will store 
 *      a copy of the usage confirmation obtained from the settlement provider,
 *      including the settlement provider's digital signature.
 * As delivered in the SDK library, this function blocks until usage has been 
 * reported or an error has been detected. The Open Settlement Protocol SDK 
 * Porting Guide includes information on modifying that behavior to prevent 
 * blocking.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */

int
OSPPTransactionReportUsage(
    OSPTTRANHANDLE          ospvTransaction,            /* In - Transaction handle */
    unsigned                ospvDuration,               /* In - Length of call */
    OSPTTIME                ospvStartTime,              /* In - Call start time */
    unsigned                ospvLossPacketsSent,        /* In - Packets not received by peer */ 
    signed                  ospvLossFractionSent,       /* In - Fraction of packets not received by peer */
    unsigned                ospvLossPacketsReceived,    /* In - Packets not received that were expected */
    signed                  ospvLossFractionReceived,   /* In - Fraction of packets expected but not received */
    unsigned                *ospvSizeOfDetailLog,    /* In/Out - Max size of detail log \ Actual size of detail log */
    void                    *ospvDetailLog)             /* Out - Pointer to detail log storage */
{
    int                     errorcode    = OSPC_ERR_NO_ERROR;
    OSPTTRANS               *trans       = OSPC_OSNULL;
    OSPTUSAGEIND            *usage       = OSPC_OSNULL;
    OSPE_MSG_DATATYPES      datatype     = OSPC_MSG_LOWER_BOUND;
    unsigned char           *xmldoc      = OSPC_OSNULL;
    unsigned                sizeofxmldoc = 0;
    OSPTMSGINFO             *msginfo     = OSPC_OSNULL;
    OSPTDEST                *dest        = OSPC_OSNULL;
    OSPTBOOL                usageallowed = OSPC_FALSE;
    OSPTSTATISTICS          stats;

    OSPM_ARGUSED(ospvSizeOfDetailLog);
    OSPM_ARGUSED(ospvDetailLog);

    OSPM_MEMSET(&stats, 0, sizeof(OSPTSTATISTICS));

    /* Verify Input */
    if((ospvTransaction == OSPC_TRAN_HANDLE_INVALID)    ||
        (ospvTransaction > OSPC_MAX_TRANS))
    {
        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "transaction handle invalid");
    }

    /* Get transaction context */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);
        if(trans == (OSPTTRANS *)OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
            OSPM_DBGERRORLOG(errorcode, "transaction context not found");
        }
    }

    /* Have we already done this? */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPPTransactionGetReportUsageAllowed(trans, &usageallowed);
        if(!usageallowed)
        {
            errorcode = OSPC_ERR_TRAN_USAGE_ALREADY_REPORTED;
            OSPM_DBGERRORLOG(errorcode, "Usage has already been reported.");
        }
    }

    /* Set up Statistics */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        errorcode = OSPPStatisticsReportUsage(&(trans->TNStatistics),
            ospvLossPacketsSent, 
            ospvLossFractionSent,
            ospvLossPacketsReceived, 
            ospvLossFractionReceived);
    }

    /* Set up Usage indication list using current dest list (originating) or
     * authind (terminating) and current data */

    /* Are we originating or terminating */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        /* Warning! Do not change the order of these checks without
         * looking at OSPPTransactionInitializeAtDevice first
         */
        if(trans->AuthRsp != (OSPTAUTHRSP *)OSPC_OSNULL)
        {
            /* originating */
            /* determines where most info will come from */
            datatype = OSPC_MSG_ARESP;
            if (OSPPAuthRspHasDest(trans->AuthRsp))
            {
                OSPPListNew(&(trans->UsageInd));

                /* Loop through all dests from first to current and create
                 * usages for each
                 */
                for((dest = (OSPTDEST *)OSPPListFirst(&(trans->AuthRsp->ospmAuthRspDest)));
                    (dest != trans->CurrentDest) && 
                    (errorcode == OSPC_ERR_NO_ERROR) &&
                    (dest != OSPC_OSNULL);
                    (dest = (OSPTDEST *)OSPPListNext(&(trans->AuthRsp->ospmAuthRspDest), dest)))
                {
                    /*
                     * All dests up to current (if any) must have failreasons.
                     */
                    if(!OSPPDestHasTNFailReason(dest))
                    {
                        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                        OSPM_DBGERRORLOG(errorcode, "No failure code found.");
                        break;
                    }
                    else
                    {
                        errorcode = OSPPTransactionBuildUsage(trans, &usage, dest, datatype);
                    }

                    if(errorcode == OSPC_ERR_NO_ERROR)
                    {
                        /* set FailureReason */
                        OSPPUsageIndSetTNFailReason(usage, OSPPDestGetTNFailReason(dest));
                        OSPPListAppend(&(trans->UsageInd), usage);
                        usage = OSPC_OSNULL;
                    }
                }

                if(errorcode == OSPC_ERR_NO_ERROR)
                {
                    if(dest != OSPC_OSNULL)
                    {
                        /* We are at current, either set failure code or
                         * build a usage detail
                         */

                        errorcode = OSPPTransactionBuildUsage(trans, &usage, dest, datatype);

                        if(errorcode == OSPC_ERR_NO_ERROR)
                        {
                            if(OSPPDestHasTNFailReason(dest))
                            {

                                /* Set failure reason */
                                OSPPUsageIndSetTNFailReason(usage, OSPPDestGetTNFailReason(dest));
                            }
                            else
                            {
                                /* Set Duration */
                                OSPPUsageIndSetDuration(usage, (int)ospvDuration);
                                OSPPUsageIndSetStartTime(usage, ospvStartTime);

                                /* Get Stats */
                                if(OSPPTransactionHasStatistics(trans))
                                {
                                    OSPPTransactionGetStatistics(trans, &stats);
                                    OSPPUsageIndSetTNStatistics(usage, &stats);
                                }
                            }
                        }

                        OSPPListAppend(&(trans->UsageInd), usage);
                        usage = OSPC_OSNULL;
                    }
                    else
                    {
                        errorcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                        OSPM_DBGERRORLOG(errorcode, "No current destination found.");                
                    }
                }
            }
            else
            {
                /* Dest not found */
                errorcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errorcode, "No destinations found.");
            }
        }
        else if(trans->AuthInd != (OSPTAUTHIND *)OSPC_OSNULL)
        {  /*terminating*/
            datatype = OSPC_MSG_AIND;

            errorcode = OSPPTransactionBuildUsage(trans, &usage, dest, datatype);

            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                if(OSPPDestHasTNFailReason(trans->CurrentDest))
                {

                    /* Set failure reason */
                    OSPPUsageIndSetTNFailReason(usage, OSPPDestGetTNFailReason(trans->CurrentDest));
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPPListNew(&(trans->UsageInd));
                OSPPUsageIndSetDuration(usage, (int)ospvDuration);
								OSPPUsageIndSetStartTime(usage, ospvStartTime);
                OSPPListAppend(&(trans->UsageInd), usage);
                usage = OSPC_OSNULL;
            }
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errorcode, "No information available to process this report.");
        }
    }

    /* Create appropriate xml message */
    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        errorcode = OSPPXMLMessageCreate(OSPC_MSG_UIND, 
            &xmldoc, &sizeofxmldoc, &(trans->UsageInd));
    }

    /* Check for audit. If it is turned on add xmldoc to audit
     * storage.
     */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        if(trans->Provider->Comm->Flags & OSPC_COMM_AUDIT_ON)
        {
            errorcode = OSPPAuditAddMessageToBuffer(trans->Provider->Audit, xmldoc,
                                        sizeofxmldoc);
            if(errorcode != OSPC_ERR_NO_ERROR)
            {
                OSPM_DBGERRORLOG(errorcode, "OSPPAuditAddMessageToBuffer failed.");
                errorcode = OSPC_ERR_NO_ERROR;
            }
        }
    }

    /* Create msginfo, put in queue and process return */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgInfoNew(&msginfo);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPTransactionPrepareAndQueMessage(
                trans, xmldoc, sizeofxmldoc, &msginfo);

            if (errorcode == OSPC_ERR_NO_ERROR) 
            {
                /* Set transaction state */
                OSPPTransactionSetState(trans, OSPC_REPORT_USAGE_BLOCK);

                errorcode = OSPPTransactionProcessReturn(
                    trans, msginfo);
            }

            OSPPMsgInfoDelete(&msginfo);
        }
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Set transaction state */
        OSPPTransactionSetState(trans, OSPC_REPORT_USAGE_SUCCESS);
    }
    else
    {
        if(trans != OSPC_OSNULL)
        {
            /* Set transaction state */
            OSPPTransactionSetState(trans, OSPC_REPORT_USAGE_FAIL);

            if(trans->UsageInd != OSPC_OSNULL)
            {
                OSPPTransactionDeleteUsageInd(trans);
            }
        }
    }

    if(usage != OSPC_OSNULL)
    {
        OSPPUsageIndDelete(&usage);
    }

    if (xmldoc != OSPC_OSNULL)
    {
        OSPM_FREE(xmldoc);
        xmldoc = NULL;
    }

    return errorcode;
}

/*
 * The OSPPTransactionRequestAuthorisation function allows an application to 
 * request authorisation and, optionally, routing information for a transaction
 * The parameters to the function are:
 *  ospvTransaction: handle of the (previously created) transaction object.
 *  ospvSource: character string identifying the source of the call. The value 
 *      is expressed as either a DNS name or an IP address enclosed in square 
 *      brackets, followed by an optional colon and TCP port number. 
 *      Examples of valid sources include "gateway1.carrier.com" and 
 *                                                      "[172.16.1.2]:112".
 *  ospvSourceDevice: character string identifying the source device in a 
 *      protocol specific manner (e.g. H.323 Identifier); this string is 
 *      optional and may be empty.
 *  ospvCallingNumber: character string containing the calling party's number 
 *      expressed as a full international number conforming to the ITU E.164 
 *      standard (with no punctuation); if the actual calling party number is 
 *      unavailable (e.g. because the end user has blocked caller ID services),
 *      then the application should supply a local phone number for the device 
 *      originating the call.
 *  ospvCalledNumber: character string containing the called number, expressed 
 *      as a full international number conforming to the ITU E.164 standard 
 *      (with no punctuation).
 *  ospvUser: character string identifying the end user (e.g. calling card and 
 *      PIN number assigned to roaming users); this string may be empty.
 *  ospvNumberOfCallIds: the number of call identifiers in the ospvCallIds list
 *  ospvCallIds: an array of H.323 call identifiers for the call. The 
 *      OSPTCALLID type consists of a length indicator and a pointer to the 
 *      binary data. Applications may provide a list of call identifiers in 
 *      anticipation of the authorisation request returning multiple potential 
 *      destinations. In that case each potential destination is assigned a 
 *      separate call identifier. An application may also provide only a single
 *      call identifier yet still receive multiple potential destinations. In 
 *      that case the same call identifier value must be used for each 
 *      destination. If the ospvCallIds list contains more than one entry, the
 *      number of entries in that list must be the same as the input value of 
 *      the ospvNumberOfDestinations parameter. (Otherwise, an error is 
 *      returned.) The value of a call identifier is opaque to the SDK and is 
 *      treated as an arbitrary array of bytes.
 *  ospvPreferredDestinations: a list of character strings containing preferred
 *      destinations for the call, expressed as either DNS names or IP 
 *      addresses enclosed in square brackets, followed by an optional colon 
 *      and TCP port number. The list is terminated by an empty string or a 
 *      NULL pointer, and, if the application has no preferred destinations, 
 *      the list may be empty. If multiple preferred destinations are included,
 *      they are listed in order of decreasing preference.
 *      Examples of valid destinations include "gateway1.carrier.com" and 
 *                                                          "[172.16.1.2]:112".
 *  ospvNumberOfDestinations: pointer to a variable which, on input, contains 
 *      the maximum number of destinations the application wishes to consider; 
 *      on output the variable will be updated with the actual number of 
 *      destinations authorised.
 *  ospvSizeOfDetailLog: pointer to a variable which, on input, contains the 
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero, 
 *      applications indicate that they do not wish a detail log for the 
 *      authorisation request.
 *  ospvDetailLog: pointer to a location in which to store a detail log for the
 *      authorisation request. If this pointer is not NULL, and if the 
 *      ospvSizeOfDetailLog parameter is non-zero, then the library will store 
 *      a copy of the authorisation response obtained from the settlement 
 *      provider, including the settlement provider's digital signature.
 * As delivered in the SDK library, this function blocks until authorisation 
 * has been received or an error has been detected. The Open Settlement 
 * Protocol SDK Porting Guide includes information on modifying that behavior 
 * to prevent blocking.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */

int
OSPPTransactionRequestAuthorisation(
    OSPTTRANHANDLE  ospvTransaction,            /* In - Transaction handle */
    const char      *ospvSource,                /* In - Source of call */
    const char      *ospvSourceDevice,                /* In - SourceDevice of call */
    const char      *ospvCallingNumber,         /* In - Calling number */
    const char      *ospvCalledNumber,          /* In - Called number */
    const char      *ospvUser,                  /* In - End user (optional) */
    unsigned        ospvNumberOfCallIds,        /* In - Number of call identifiers */
    OSPTCALLID      *ospvCallIds[],             /* In - List of call identifiers */
    const char      *ospvPreferredDestinations[], /* In - List of preferred destinations for call */
    unsigned        *ospvNumberOfDestinations,  /* In/Out - Max number of destinations Actual number of dests authorised */
    unsigned        *ospvSizeOfDetailLog,       /* In/Out - Max size of detail log Actual size of detail log */
    void            *ospvDetailLog)             /* In/Out - Location of detail log storage */
{
    int           errorcode    = OSPC_ERR_NO_ERROR;
    OSPTTRANS     *trans       = OSPC_OSNULL;
    OSPTMSGINFO   *msginfo     = OSPC_OSNULL;
    unsigned char *xmldoc      = OSPC_OSNULL;
    unsigned      sizeofxmldoc = 0,
                  delay        = OSPC_TNPROBE_MAXWAIT;
    OSPTALTINFO   *altinfo     = OSPC_OSNULL;
    OSPT_TN_PROBE *probelist   = OSPC_OSNULL,
                  *tmpprobe    = OSPC_OSNULL;
    OSPTDEST      *dest        = OSPC_OSNULL;
    register unsigned probecnt = 0;
    OSPTSVCPT     *svcpt       = OSPC_OSNULL;

    if( (ospvCallingNumber == (const char *)OSPC_OSNULL)    ||
        (ospvCalledNumber == (const char *)OSPC_OSNULL)     ||
        (ospvNumberOfCallIds == 0)                          ||
        (ospvCallIds[0] == (OSPTCALLID *)OSPC_OSNULL)       ||
        (ospvNumberOfDestinations == 0))
    {
        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "Invalid input for RequestAuthorisation");
    }

    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        if ((ospvNumberOfCallIds > 1) && 
            (ospvNumberOfCallIds != *ospvNumberOfDestinations)) 
        {
            errorcode = OSPC_ERR_TRAN_CALLID_DEST_MISMATCH;
            OSPM_DBGERRORLOG(errorcode, 
                "number of callids != number of dests sent in.");
        }
    }


    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);

        /* Check Callids */
        if (errorcode == OSPC_ERR_NO_ERROR) 
        {

            /* if there is an authreq here already, we have an error */
            if(trans->AuthReq == OSPC_OSNULL)
            {

                /* Build Auth request */
                if (errorcode == OSPC_ERR_NO_ERROR)
                {

                    errorcode =  OSPPTransactionRequestNew (trans, 
                        ospvSource, ospvSourceDevice, ospvCallingNumber,           
                        ospvCalledNumber, ospvUser, ospvNumberOfCallIds,
                        ospvCallIds, ospvPreferredDestinations, 
                        ospvNumberOfDestinations, 
                        ospvSizeOfDetailLog, ospvDetailLog);

                    if (errorcode == OSPC_ERR_NO_ERROR) 
                    {

                        errorcode = OSPPXMLMessageCreate(OSPC_MSG_AREQ, 
                            &xmldoc, &sizeofxmldoc, trans->AuthReq);

                        if (errorcode == OSPC_ERR_NO_ERROR)
                        {

                            errorcode = OSPPMsgInfoNew(&msginfo);

                            if (errorcode == OSPC_ERR_NO_ERROR)
                            {
                                errorcode = OSPPTransactionPrepareAndQueMessage(
                                    trans, xmldoc, sizeofxmldoc, &msginfo);

                                if (errorcode == OSPC_ERR_NO_ERROR) 
                                {
                                    /* Set transaction state */
                                    OSPPTransactionSetState(trans, OSPC_AUTH_REQUEST_BLOCK);

                                    errorcode = OSPPTransactionProcessReturn(
                                        trans, msginfo);

                                    if((errorcode == OSPC_ERR_NO_ERROR) &&
                                        (trans->AuthRsp != OSPC_OSNULL))
                                    {
                                        /* Set num destinations */
                                        *ospvNumberOfDestinations = OSPPAuthRspGetNumDests(trans->AuthRsp);

                                        /* Check status code */
                                        if(trans->AuthRsp->ospmAuthRspStatus->ospmStatusCode > 299)
                                        {
                                            errorcode = OSPPUtilGetErrorFromStatus(
                                                trans->AuthRsp->ospmAuthRspStatus->ospmStatusCode);
                                        }

                                        if((*ospvNumberOfDestinations > 0) && (errorcode == OSPC_ERR_NO_ERROR))
                                        {
                                            /* Do we need to run probe? */
                                            if((*ospvNumberOfDestinations > 1) &&
                                                (OSPPAuthRspHasTNDelayLimit(trans->AuthRsp) || 
                                                OSPPAuthRspHasTNDelayPref(trans->AuthRsp)))
                                            {

                                                /* build probe list */
                                                OSPM_MALLOC(probelist, OSPT_TN_PROBE, 
                                                    sizeof(OSPT_TN_PROBE) * *ospvNumberOfDestinations);
                                                if(probelist == OSPC_OSNULL)
                                                {
                                                    errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                                                }

                                                if(errorcode == OSPC_ERR_NO_ERROR)
                                                {
                                                    OSPM_MEMSET(probelist, 0, 
                                                        sizeof(OSPT_TN_PROBE) * *ospvNumberOfDestinations);

                                                    tmpprobe = probelist;
                                                    for(probecnt = 0, dest = (OSPTDEST *)OSPPAuthRspFirstDest(trans->AuthRsp); 
                                                        probecnt < *ospvNumberOfDestinations; 
                                                        dest = (OSPTDEST *)OSPPAuthRspNextDest(trans->AuthRsp, dest))
                                                    {

                                                        if(dest != OSPC_OSNULL)
                                                        {
                                                            errorcode = OSPPCommParseSvcPt((const char *)OSPPDestGetAddr(dest), 
                                                                                            &svcpt, 
                                                                                            0);
                                                        }

                                                        if(errorcode == OSPC_ERR_NO_ERROR)
                                                        {
                                                            if (svcpt != (OSPTSVCPT *)OSPC_OSNULL)
                                                            {
                                                                tmpprobe[probecnt].ospmipaddr = svcpt->IpAddr;

                                                                if (svcpt->HostName)
                                                                    OSPM_FREE(svcpt->HostName);

                                                                if (svcpt->URI)
                                                                    OSPM_FREE(svcpt->URI);

                                                                OSPM_FREE(svcpt);
                                                                
                                                                probecnt++;
                                                            }
                                                        }
                                                    }
                                                }

                                                /* Check for delay, if present, change from default.
                                                 * Call probe */
                                                if(OSPPAuthRspHasTNDelayLimit(trans->AuthRsp))
                                                {
                                                    delay = OSPPAuthRspGetTNDelayLimit(trans->AuthRsp);
                                                }

                                                errorcode = OSPPTNProbe(probelist, probecnt, delay);

                                                if(errorcode == OSPC_ERR_NO_ERROR)
                                                { 
                                                    if(OSPPAuthRspHasTNDelayLimit(trans->AuthRsp))
                                                    {
                                                        OSPPTNProbePruneList(&(trans->AuthRsp->ospmAuthRspDest),
                                                            probelist,
                                                            OSPPAuthRspGetTNDelayLimit(trans->AuthRsp),
                                                            ospvNumberOfDestinations);
                                                    }

                                                    if((OSPPAuthRspHasTNDelayPref(trans->AuthRsp)) &&
                                                        (*ospvNumberOfDestinations > 1))
                                                    {

                                                        OSPPTNProbeArrangeList(&(trans->AuthRsp->ospmAuthRspDest),
                                                            probelist,
                                                            *ospvNumberOfDestinations);
                                                    }
                                                }

                                                if(probelist != OSPC_OSNULL)
                                                {
                                                    OSPM_FREE(probelist);
                                                }
                                            }
                                        }
                                    }

                                }
                                else
                                {
                                    OSPPTransactionDeleteRequest(trans);
                                }

                                OSPPMsgInfoDelete(&msginfo);

                                if (trans->AuthReq != OSPC_OSNULL && 
                                    trans->AuthReq->ospmAuthReqDestinationAlternate != OSPC_OSNULL)
                                {
                                    /* We don't keep these around */
                                    while (!OSPPListEmpty(&((trans->AuthReq)->ospmAuthReqDestinationAlternate)))
                                    {
                                        altinfo = (OSPTALTINFO *)OSPPListRemove(&((trans->AuthReq)->ospmAuthReqDestinationAlternate));
                                        if (altinfo != OSPC_OSNULL)
                                        {
                                            OSPM_FREE(altinfo);
                                            altinfo = OSPC_OSNULL;
                                        }
                                    }  
                                    OSPPListDelete(&((trans->AuthReq)->ospmAuthReqDestinationAlternate));
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_NO_NEW_AUTHREQ;
                OSPM_DBGERRORLOG(errorcode, "Unfinished AuthReq found.");
            }

            if (xmldoc != NULL)
            {
                OSPM_FREE(xmldoc);
                xmldoc = NULL;
            }
        }

    } /* end else (valid data) */

    /* Set transaction state */
    if ((errorcode == OSPC_ERR_NO_ERROR) ||
        (errorcode == OSPC_ERR_TRAN_NO_NEW_AUTHREQ))
    {
        OSPPTransactionSetState(trans, OSPC_AUTH_REQUEST_SUCCESS);
    }
    else
    {
        OSPPTransactionSetState(trans, OSPC_AUTH_REQUEST_FAIL);
    }

    return errorcode;
}

/*
 * OSPPTransactionRequestReauthorisation()
 *
 * Request reauthorisation for a previously authorised call.
 *
 * The OSPPTransactionRequestReauthorisation function asks the SDK library to 
 * refresh a previously granted authorisation, perhaps, for example, because 
 * the time limit for that authorisation is nearing its expiration. Parameters 
 * to the function are:
 *   ospvTransaction: handle of the (previously created) transaction object.
 *   ospvSizeOfToken: pointer to a variable which, on input, contains the size 
 *      of the memory buffer in which the function should store the 
 *      authorisation token for the destination. If the value is not large 
 *      enough to accommodate the token, then an error is indicated and no 
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the authorisation token.
 *   ospvToken: memory location in which to store the authorisation token for 
 *      this destination. In general, tokens are opaque, binary objects.
 *   ospvAuthorised: pointer to a variable in which the function will indicate 
 *      whether or not the call is reauthorised. On return, a non-zero value 
 *      indicates that the call is reauthorised by the provider, while a zero 
 *      value indicates an authorisation failure.
 *   ospvTimeLimit: pointer to a variable in which to place the total number of
 *      seconds for which the call is now authorised. A value of zero indicates
 *      that no limit exists.
 *   ospvSizeOfDetailLog: pointer to a variable which, on input, contains the 
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero, 
 *      applications indicate that they do not wish a detail log for the 
 *      authorisation reauthorisation.
 *   ospvDetailLog: pointer to a location in which to store a detail log for 
 *      the reauthorisation. If this pointer is not NULL, and if the 
 *      ospvSizeOfDetailLog parameter is non-zero, then the library will store 
 *      a copy of the reauthorisation response obtained from the settlement 
 *      provider, including the settlement provider's digital signature.
 * This function blocks on network input/output until authorisation has been 
 * refreshed, refused, or an error has been detected. The Open Settlement 
 * Protocol SDK Porting Guide includes information on modifying that behavior 
 * to prevent blocking.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */

int
OSPPTransactionRequestReauthorisation(
    OSPTTRANHANDLE  ospvTransaction,  /* In - Transaction handle */
    unsigned        ospvDuration,     /* In - duration of in-progress call */
    unsigned        *ospvSizeOfToken, /* In/Out - max size of authorization token space actual size of authorization token */
    void            *ospvToken,       /* Out - Reauthorisation token storage */
    unsigned        *ospvAuthorised,  /* Out - Indicator of status of reauthorisation (non-zero = OK) */
    unsigned        *ospvTimeLimit,   /* Out - Total number of seconds for reauthorisation ( zero = unlimited )*/
    unsigned        *ospvSizeOfDetailLog, /* In/Out - Max size of detail log\ Actual size of detail log */
    void            *ospvDetailLog)       /* In/Out - Location of detail log storage */
{
    int             errorcode       = OSPC_ERR_NO_ERROR;
    OSPTTRANS       *trans          = OSPC_OSNULL;
    unsigned char   *xmldoc         = OSPC_OSNULL;
    unsigned        sizeofxmldoc    = 0;
    OSPTMSGINFO     *msginfo        = OSPC_OSNULL;
    OSPTSTATUS      *status         = OSPC_OSNULL;
    OSPTTOKEN       *token          = OSPC_OSNULL;

    *ospvAuthorised = OSPC_FALSE;
    OSPM_ARGUSED(ospvSizeOfDetailLog);
    OSPM_ARGUSED(ospvDetailLog);

    /* Get transaction context */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {

        trans = OSPPTransactionGetContext(ospvTransaction, &errorcode);
        if(trans == (OSPTTRANS *)OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
            OSPM_DBGERRORLOG(errorcode, "transaction context not found");
        }
    }

    /* Has to be OGW */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        if(trans->AuthRsp != (OSPTAUTHRSP *)OSPC_OSNULL)
        {
            /* OGW */
            /* Build ReauthReq */
            errorcode = OSPPTransactionBuildReauthRequest(trans, ospvDuration);

        }
        else if(trans->AuthInd != (OSPTAUTHIND *)OSPC_OSNULL)
        {  /* TGW */
            errorcode = OSPC_ERR_TRAN_TRANSACTION_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errorcode, "This transaction not allowed on TGW.");
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errorcode, "No information available to process this request.");
        }

    }

    /* Create appropriate xml message */
    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        errorcode = OSPPXMLMessageCreate(OSPC_MSG_REAREQ, 
            &xmldoc, &sizeofxmldoc, trans->ReauthReq);
    }

    /* Create msginfo, put in queue and process return */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPMsgInfoNew(&msginfo);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPTransactionPrepareAndQueMessage(
                trans, xmldoc, sizeofxmldoc, &msginfo);

            if (errorcode == OSPC_ERR_NO_ERROR) 
            {

                errorcode = OSPPTransactionProcessReturn(
                    trans, msginfo);
            }

            OSPPMsgInfoDelete(&msginfo);
        }
    }

    if((trans->ReauthRsp != OSPC_OSNULL) &&
        (errorcode == OSPC_ERR_NO_ERROR))
    {

        /* verify REARESP TXID == trans->txid*/
        if(trans->ReauthRsp->ospmReauthRspTrxId != trans->TransactionID)
        {
            errorcode = OSPC_ERR_TRAN_TXID_INVALID;
        }

        /* check status code - set authorised */
        if (OSPPReauthRspHasStatus(trans->ReauthRsp) == OSPC_FALSE)
        {
            errorcode = OSPC_ERR_TRAN_STATUS_INVALID;
            OSPM_DBGERRORLOG(errorcode, "status not found");
        }
        else
        {
            status = OSPPReauthRspGetStatus(trans->ReauthRsp);
            if(status != (OSPTSTATUS *)OSPC_OSNULL)
            {
                if (!OSPM_STATUSCODE_SUCCESSFUL(status->ospmStatusCode))
                { 

                    errorcode = status->ospmStatusCode;
                    OSPM_DBGERRORLOG(errorcode, "server returned a status error");
                }
                else
                {
                    *ospvAuthorised = OSPC_TRUE;
                }
            }
            else
            {
                errorcode = OSPC_ERR_TRAN_STATUS_NOT_FOUND;
                OSPM_DBGERRORLOG(errorcode, "status not found");
            }
        }

        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            if(!OSPPReauthRspHasDest(trans->ReauthRsp))
            {
                errorcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
            }
            else
            {
                /* set token from REARESP token*/

                if (!OSPPDestHasToken(trans->ReauthRsp->ospmReauthRspDest))
                {

                    errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                    OSPM_DBGERRORLOG(errorcode, 
                        "null pointer for token.");
                }
                else
                {
                    token =(OSPTTOKEN *)OSPPDestFirstToken(trans->ReauthRsp->ospmReauthRspDest);
                    if (token == (OSPTTOKEN *)OSPC_OSNULL)
                    {

                        errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                        OSPM_DBGERRORLOG(errorcode, 
                            "null pointer for token.");
                    }
                    else
                    {
                        if (*ospvSizeOfToken < (unsigned)OSPPTokenGetSize(token))
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

                if(errorcode == OSPC_ERR_NO_ERROR)
                {
                    /* set timelimit from REARESP duration */
                    if(!OSPPDestHasLimit(trans->ReauthRsp->ospmReauthRspDest))
                    {
                        errorcode = OSPC_ERR_TRAN_DATA_NOT_FOUND;
                    }
                    else
                    {
                        *ospvTimeLimit = OSPPDestGetLimit(trans->ReauthRsp->ospmReauthRspDest);
                    }
                }
            }
        }
    }

    if (xmldoc != OSPC_OSNULL)
    {
        OSPM_FREE(xmldoc);
        xmldoc = NULL;
    }

    /* Now delete the reauth request and reauth response */
    OSPPTransactionDeleteReauthReq(trans);
    OSPPTransactionDeleteReauthRsp(trans);

    return errorcode;
}

/*
 * The OSPPTransactionValidateAuthorisation function asks the SDK library to 
 * validate a requested incoming call, based on the call's parameters and 
 * authorisation tokens included in the call. This function may be called 
 * multiple times for a single call, so that, for example, call requests with 
 * multiple authorisation tokens may result in multiple calls to this function,
 * one for each authorisation token in the request. Parameters to the function 
 * are
 *  ospvTransaction: handle of the (previously created) transaction object.
 *  ospvSource: character string identifying the source of the call. The value 
 *      is expressed as either a DNS name or an IP address enclosed in square 
 *      brackets, followed by an optional colon and TCP port number. 
 *      Examples of valid sources include "gateway1.carrier.com" and 
 *                                                          "[172.16.1.2]:112".
 *  ospvDestination: character string identifying the destination for the call.
 *      The value is expressed as either a DNS name or an IP address enclosed 
 *      in square brackets, followed by an optional colon and TCP port number. 
 *      Examples of valid destinations include "gateway1.carrier.com" and 
 *                                                          "[172.16.1.2]:112".
 *  ospvSourceDevice: character string identifying the source device in a 
 *      protocol specific manner (e.g. H.323 Identifier); this string is 
 *      optional and may be empty.
 *  ospvDestinationDevice: character string identifying the destination device 
 *      in a protocol specific manner (e.g. H.323 Identifier); this string is 
 *      optional and may be empty.
 *  ospvCallingNumber: character string containing the calling party's number 
 *      expressed as a full international number conforming to the ITU E.164 
 *      standard (with no punctuation).
 *  ospvCalledNumber: character string containing the called number, expressed 
 *      as a full international number conforming to the ITU E.164 standard 
 *      (with no punctuation).
 *  ospvSizeOfCallId: size of the memory buffer containing the call identifier.
 *  ospvCallId: memory location containing the H.323 call identifier for the 
 *      call.
 *  ospvSizeOfToken: size of the memory buffer containing an authorisation 
 *      token for the call.
 *  ospvToken: memory location containing an authorisation token.
 *  ospvAuthorised: pointer to a variable in which the function will indicate 
 *      whether or not the call is authorised. On return, a non-zero value 
 *      indicates that the call is authorised by the provider, while a zero 
 *      value indicates an authorisation failure.
 *  ospvTimeLimit: pointer to a variable in which to place the number of 
 *      seconds for which the call is initially authorised. A value of zero 
 *      indicates that no limit exists. Note that the initial time limit may be
 *      extended during the call by using the function 
 *      OSPPTransactionRequestReauthorisation.
 *  ospvSizeOfDetailLog: pointer to a variable which, on input, contains the 
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero, 
 *      applications indicate that they do not wish a detail log for the 
 *      authorisation validation.
 *  ospvDetailLog: pointer to a location in which to store a detail log for the
 *      validation. If this pointer is not NULL, and if the ospvSizeOfDetailLog
 *      parameter is non-zero, then the library will store a copy of the 
 *      authorisation confirmation obtained from the settlement provider, 
 *      including the settlement provider's digital signature.
 * If the provider has been configured to perform local validation, the SDK 
 * library is able to perform this function without network interaction, and, 
 * therefore, does not block for network input or output during its execution. 
 * If local validation is not used, this function blocks until authorisation 
 * has been validated, refused, or an error has been detected. The Open 
 * Settlement Protocol SDK Porting Guide includes information on modifying that
 * behavior to prevent blocking.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int
OSPPTransactionValidateAuthorisation(
    OSPTTRANHANDLE      ospvTransaction,        /* In - Transaction handle */
    const char          *ospvSource,            /* In - Source of call */
    const char          *ospvDestination,       /* In - Destination for call */
    const char          *ospvSourceDevice,      /* In - SourceDevice of call */
    const char          *ospvDestinationDevice, /* In - DestinationDevice for call */
    const char          *ospvCallingNumber,     /* In - Calling number string*/
    const char          *ospvCalledNumber,      /* In - Called number string */
    unsigned            ospvSizeOfCallId,       /* In - Size of call id value */
    const void          *ospvCallId,            /* In - Call Id for this call */
    unsigned            ospvSizeOfToken,        /* In - Size of authorization token */
    const void          *ospvToken,             /* In - Authorisation token */
    unsigned            *ospvAuthorised,        /* Out - Call authorisation indicator */
    unsigned            *ospvTimeLimit,         /* Out - Number of seconds call is authorised for */
    unsigned            *ospvSizeOfDetailLog,   /* In/Out - Max size of detail log\ Actual size of detail log */
    void                *ospvDetailLog)         /* In - Pointer to storage for detail log */
{
    int                 errorcode  = OSPC_ERR_NO_ERROR;
    OSPTTRANS          *trans      = OSPC_OSNULL;
    OSPTAUTHIND        *authind    = OSPC_OSNULL;
    OSPTCALLID         *callid     = OSPC_OSNULL;
    OSPTTOKEN          *token      = OSPC_OSNULL;
    int                 retcode    = 0;
    OSPTTOKENINFO      *tokeninfo  = OSPC_OSNULL;
    OSPE_MSG_DATATYPES  dtype      = OSPC_MSG_LOWER_BOUND;
    OSPTALTINFO        *altinfo    = OSPC_OSNULL;
    OSPTPROVIDER        *provider  = OSPC_OSNULL;
    OSPTSEC             *security  = OSPC_OSNULL;
    unsigned char       *tokenmsg  = OSPC_OSNULL;
    unsigned            sizeoftokenmsg = 0;
    OSPTDEST            *dest       = OSPC_OSNULL;
    int                 BAllowDupTransId = OSPC_TRUE;

    OSPM_ARGUSED(ospvSizeOfDetailLog);
    OSPM_ARGUSED(ospvDetailLog);

    /* Verify input */
    if (((ospvDestination == (const char *)OSPC_OSNULL)&&
        (ospvDestinationDevice == (const char *)OSPC_OSNULL)) ||
        (ospvCallingNumber == (const char *)OSPC_OSNULL) ||
        (ospvCalledNumber == (const char *)OSPC_OSNULL)  ||
        (ospvSizeOfCallId == 0)                          ||
        (ospvCallId == (const void *)OSPC_OSNULL)        ||
        (ospvSizeOfToken == 0)                           ||
        (ospvToken == (const void *)OSPC_OSNULL))
    {

        errorcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errorcode, "Invalid input for ValidateAuth");
    }

    /* Get transaction context */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(ospvTransaction, 
            &errorcode);
    }

    /* Is there an AuthInd here already?
     * If so, we are only adding the token
     */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* not here... make a new one */
        if (trans->AuthInd == OSPC_OSNULL)
        {
            authind = OSPPAuthIndNew();

            /* populate the new one */
            if (authind != (OSPTAUTHIND *)OSPC_OSNULL)
            {

                OSPPAuthIndSetTimestamp(authind, time(OSPC_OSNULL));
                callid = OSPPCallIdNew(ospvSizeOfCallId, 
                    (const unsigned char *)ospvCallId);
 
                if (callid != (OSPTCALLID *)OSPC_OSNULL)
                {
                    OSPPAuthIndSetRole(authind,OSPC_DESTINATION);
                    OSPPAuthIndSetCallId(authind, callid);
                    OSPPAuthIndSetSourceNumber(authind, 
                        (const unsigned char *)ospvCallingNumber);
                    OSPPAuthIndSetDestNumber(authind, 
                        (const unsigned char *)ospvCalledNumber);
                    OSPPListNew(&(authind->ospmAuthIndTokens));
                    OSPPCallIdDelete(&callid);
                }
                else
                {
                    errorcode = OSPC_ERR_TRAN_CALLID_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "callid is null");
                }

                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                 /* create the destination object */
                    dest = OSPPDestNew(); 

                    if (dest == OSPC_OSNULL)
                    {
                        errorcode = OSPC_ERR_DATA_NO_DEST;
                    }
                    else
                    {
                        OSPPDestSetCallId(dest, 
                                          (const unsigned char *)ospvCallId,
                                          ospvSizeOfCallId);

                        OSPPDestSetNumber(dest, (const unsigned char *)ospvCalledNumber);

                        OSPPAuthIndSetDest(authind, dest);

                        trans->CurrentDest = dest;
                        dest = OSPC_OSNULL;

                    }
                }
                


                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                    token = OSPPTokenNew(ospvSizeOfToken, 
                        (const unsigned char *)ospvToken);

                    if(token != (OSPTTOKEN *)OSPC_OSNULL)
                    {
                        OSPPListAppend (&(authind->ospmAuthIndTokens), 
                            (void *)token);

                        trans->AuthInd = authind;
                    }
                    else
                    {
                        errorcode = OSPC_ERR_TRAN_TOKEN_NOT_FOUND;
                        OSPM_DBGERRORLOG(errorcode, "token is null");
                    }
                }

                /* --------------------------------------
                 * ospmAuthIndSourceAlternate (SourceAlternate)
                 * --------------------------------------
                 */

                if(errorcode == OSPC_ERR_NO_ERROR)
                {

                    if((ospvSource != OSPC_OSNULL) ||
                        (ospvSourceDevice != OSPC_OSNULL))
                    {

                        /* source alternates - create a linked list */
                        OSPPListNew((OSPTLIST *)&(trans->AuthInd->ospmAuthIndSourceAlternate));

                        if(ospvSource != OSPC_OSNULL)
                        {

                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSource), 
                                (const unsigned char *)ospvSource,
                                ospeTransport);

                            if(altinfo != OSPC_OSNULL)
                            {

                                OSPPListAppend(
                                    (OSPTLIST *)&(trans->AuthInd->ospmAuthIndSourceAlternate),
                                    (void *)altinfo);
                            }
                        }

                        altinfo = OSPC_OSNULL;

                        if(ospvSourceDevice != OSPC_OSNULL)
                        {

                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSourceDevice),
                                (const unsigned char *)ospvSourceDevice,
                                ospeH323);

                            if(altinfo != OSPC_OSNULL)
                            {

                                OSPPListAppend(
                                    (OSPTLIST *)&(trans->AuthInd->ospmAuthIndSourceAlternate),
                                    (void *)altinfo);
                            }
                        } /* end if ospvSourceDevice != OSPC_OSNULL */
                    }
                    else
                    {
                        errorcode = OSPC_ERR_TRAN_SOURCE_INVALID;
                    }
                } /* end  if(errorcode == OSPC_ERR_NO_ERROR) */

                /* -----------------------------------------------------
                 * ospmAuthIndDestinationAlternate (DestinationAlternate)
                 * -----------------------------------------------------
                 */

                if(errorcode == OSPC_ERR_NO_ERROR)
                {

                    if((ospvDestination != OSPC_OSNULL) ||
                        (ospvDestinationDevice != OSPC_OSNULL)|| (trans->NetworkId!= OSPC_OSNULL))
                    {

                        /* destination alternates - create a linked list */
                        OSPPListNew((OSPTLIST *)&(trans->AuthInd->ospmAuthIndDestinationAlternate));


                        if(trans->NetworkId != OSPC_OSNULL)
                        {

                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->NetworkId), 
                                (const unsigned char *)trans->NetworkId,
                                ospeNetwork);

                            if(altinfo != OSPC_OSNULL)
                            {

                                OSPPListAppend(
                                    (OSPTLIST *)&(trans->AuthInd->ospmAuthIndDestinationAlternate),
                                    (void *)altinfo);
                            }
                        }

                        altinfo = OSPC_OSNULL;

                        if(ospvDestination != OSPC_OSNULL)
                        {

                            altinfo = 
                                OSPPAltInfoNew(OSPM_STRLEN(ospvDestination), 
                                (const unsigned char *)ospvDestination,
                                ospeTransport);

                            if(altinfo != OSPC_OSNULL)
                            {

                                OSPPListAppend(
                                    (OSPTLIST *)&(trans->AuthInd->ospmAuthIndDestinationAlternate),
                                    (void *)altinfo);
                            }
                        }

                        altinfo = OSPC_OSNULL;

                        if(ospvDestinationDevice != OSPC_OSNULL)
                        {

                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestinationDevice),
                                (const unsigned char *)ospvDestinationDevice,
                                ospeH323);

                            if(altinfo != OSPC_OSNULL)
                            {

                                OSPPListAppend(
                                    (OSPTLIST *)&(trans->AuthInd->ospmAuthIndDestinationAlternate),
                                    (void *)altinfo);
                            }
                        } /* end if ospvDestinationDevice != OSPC_OSNULL */
                    }
                    else
                    {
                        errorcode = OSPC_ERR_TRAN_DEST_INVALID;
                    }
                } /* end  if(errorcode == OSPC_ERR_NO_ERROR) */
            }
        }
        else
        {
            /* authind already here, make sure it is the right one */
            retcode = OSPM_STRCMP((const char *)OSPPAuthIndGetDestNumber(trans->AuthInd),
                                    ospvCalledNumber);

            if(retcode != 0)
            {
                errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                OSPM_DBGERRORLOG(errorcode, "Called number does not match");
            }
            else
            {
               retcode = OSPM_STRCMP((const char *)OSPPAuthIndGetSourceNumber(trans->AuthInd),
                                    ospvCallingNumber);

                if(retcode != 0)
                {
                    errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                    OSPM_DBGERRORLOG(errorcode, "Calling number does not match");
                }
            }

            if(errorcode == OSPC_ERR_NO_ERROR)
            {

                if(trans->AuthInd->ospmAuthIndTokens != (OSPTLIST)OSPC_OSNULL)
                {

                    token = OSPPTokenNew(ospvSizeOfToken, 
                        (const unsigned char *)ospvToken);

                    if(token != (OSPTTOKEN *)OSPC_OSNULL)
                    {
                        OSPPListAppend (&(trans->AuthInd->ospmAuthIndTokens), 
                            (void *)token);
                    }
                    else
                    {
                        errorcode = OSPC_ERR_TRAN_TOKEN_NOT_FOUND;
                        OSPM_DBGERRORLOG(errorcode, "token is null");
                    }
                }
                else
                {
                    errorcode = OSPC_ERR_TRAN_LIST_NOT_FOUND;
                    OSPM_DBGERRORLOG(errorcode, "list is null");
                }
            }
        }
    }

    /*
     * Now that we have a complete AuthInd, let's verify it against
     * the token received
     */
    if (errorcode == OSPC_ERR_NO_ERROR) 
    {
        /*
         * Verify Token Signature and pull off Tokeninfo message
         */
        errorcode = OSPPTransactionGetProvider(trans, &provider);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPProviderGetSecurity(provider, &security);
        }

#ifdef OSPC_USE_SIGNED_TOKEN
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPSecSignatureVerify(security,
                &tokenmsg, &sizeoftokenmsg,
                (unsigned char *)ospvToken, ospvSizeOfToken,
                OSPC_SEC_SIGNATURE_AND_CONTENT);
        }

	#ifdef OSPC_VALIDATE_TOKEN_CERT_SUBJECT_NAME
        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPTransactionValidateTokenCert(trans, (unsigned char *)ospvToken, ospvSizeOfToken);
        }
	#endif
#else
        /* just copy the token content into the token msg. No sig present */
        OSPM_MALLOC(tokenmsg, unsigned char, ospvSizeOfToken);
        if (tokenmsg != OSPC_OSNULL)
        {
            OSPM_MEMCPY(tokenmsg, ospvToken, ospvSizeOfToken);
            sizeoftokenmsg = ospvSizeOfToken;
        }
        else
        {
            errorcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        }
#endif

        /*
         * Parse the Token into a TokenInfo structure
         */
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPXMLMessageParse( (unsigned char *)tokenmsg,
                sizeoftokenmsg, 
                (void **)&tokeninfo, &dtype);
        }

        if (errorcode == OSPC_ERR_NO_ERROR && dtype == OSPC_MSG_TOKINFO)
        {

            /*
             * Verify Source Number
             */
            retcode = OSPM_STRCMP((const char *)OSPPAuthIndGetSourceNumber(trans->AuthInd),
                                    (const char *)OSPPTokenInfoGetSourceNumber(tokeninfo));

            if (retcode != 0)
            {
                errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                OSPM_DBGERRORLOG(errorcode, "information does not match");
            }
            else
            {
                /*
                 * Verify Destination Number
                 */
                retcode = OSPM_STRCMP((const char *)OSPPAuthIndGetDestNumber(trans->AuthInd),
                                    (const char *)OSPPTokenInfoGetDestNumber(tokeninfo));

                if (retcode != 0)
                {
                    errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                    OSPM_DBGERRORLOG(errorcode, "information does not match");
                }
                else
                {
                    /*
                     * Verify CallId
                     */
                    retcode = OSPM_MEMCMP(OSPPAuthIndGetCallIdValue(trans->AuthInd),
                        OSPPTokenInfoGetCallIdValue(tokeninfo), ospvSizeOfCallId); 

                    if (retcode != 0)
                    {
                        errorcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                        OSPM_DBGERRORLOG(errorcode, "information does not match");
                    }
                    else
                    {
                        /*
                         * Verify Valid After
                         */
                        if (time((time_t *)0) <  OSPPTokenInfoGetValidAfter(tokeninfo))
                        {
                            errorcode = OSPC_ERR_TRAN_TOO_SOON_TO_USE_TOKEN;
                            OSPM_DBGERRORLOG(errorcode, "too soon to use token");
                        }
                        else
                        {
                            /*
                             * Verify Valid Until
                             */
                            if (time((time_t *)0) >  OSPPTokenInfoGetValidUntil(tokeninfo))
                            {
                                errorcode = OSPC_ERR_TRAN_TOO_LATE_TO_USE_TOKEN;
                                OSPM_DBGERRORLOG(errorcode, "too late to use token");
                            }
                        }
                    }
                }
            }
            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                /*
                 * Add transaction id to transactionid tree and check for reuse.
                 */
#ifdef OSP_ALLOW_DUP_TXN
                BAllowDupTransId = OSPC_TRUE;
#else
                BAllowDupTransId = OSPC_FALSE;
#endif
                if ((OSPPTransIdCheckAndAdd(OSPPTokenInfoGetTrxId(tokeninfo), 
                                         (unsigned long)OSPPTokenInfoGetValidUntil(tokeninfo), 
                                          trans->Provider)) || BAllowDupTransId)
                {

                    /*
                     * Populate Transaction Id
                     */
                    trans->TransactionID = OSPPTokenInfoGetTrxId(tokeninfo);
                    trans->HasTransactionID = OSPC_TRUE;

                    *ospvAuthorised = OSPC_TRAN_AUTHORISED;
                    *ospvTimeLimit = (unsigned)OSPPTokenInfoGetDuration(tokeninfo);
                    OSPPAuthIndSetTimeLimit(trans->AuthInd, *ospvTimeLimit);
                }
                else
                {
                    errorcode = OSPC_ERR_TRAN_TOKEN_REUSE;
                    OSPM_DBGERRORLOG(errorcode, "attemp to reuse token");
                }
            }
        }
    }

    if(tokeninfo != OSPC_OSNULL)
    {
        OSPPTokenInfoDelete(&tokeninfo);
        tokeninfo = OSPC_OSNULL;
    }

    if (tokenmsg != OSPC_OSNULL)
    {
        OSPM_FREE(tokenmsg);
        tokenmsg = OSPC_OSNULL;
    }

    /* Set transaction state */
    if (errorcode == OSPC_ERR_NO_ERROR)
        OSPPTransactionSetState(trans, OSPC_VALIDATE_AUTH_SUCCESS);
    else
    {
        OSPPTransactionSetState(trans, OSPC_VALIDATE_AUTH_FAIL);
    }
    return errorcode;
}

int
OSPPTransactionValidateReAuthorisation(
    OSPTTRANHANDLE      ospvTransaction,      /* In - Transaction handle */
    unsigned            ospvSizeOfToken,      /* In - Size of authorization token */
    const void          *ospvToken,           /* In - Authorisation token */
    unsigned            *ospvAuthorised,      /* Out - Call authorisation indicator */
    unsigned            *ospvTimeLimit,       /* Out - Number of seconds call is authorised for */
    unsigned            *ospvSizeOfDetailLog, /* In/Out - Max size of detail log\ Actual size of detail log */
    void                *ospvDetailLog)       /* In - Pointer to storage for detail log */
{
    int         errorcode       = OSPC_ERR_NO_ERROR;
    OSPTTRANS   *trans          = OSPC_OSNULL;
    OSPTALTINFO *source         = OSPC_OSNULL,
        *sourcealt      = OSPC_OSNULL,
        *destination    = OSPC_OSNULL,
        *destalt        = OSPC_OSNULL;
    const char  *sourceval      = OSPC_OSNULL,
                *sourcealtval   = OSPC_OSNULL,
                *destval        = OSPC_OSNULL,
                *destaltval     = OSPC_OSNULL;

    /* Validate will verify input */

    /* Get transaction context */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        trans = OSPPTransactionGetContext(ospvTransaction, 
            &errorcode);
    }

    /* Make sure this is a terminating gateway */
    if(errorcode == OSPC_ERR_NO_ERROR)
    {
        if(trans->AuthRsp != OSPC_OSNULL)
        {
            errorcode = OSPC_ERR_TRAN_TRANSACTION_NOT_ALLOWED;
        }
    }

    /* Make sure we have an AuthInd */
    if(trans->AuthInd != OSPC_OSNULL)
    {
        source = (OSPTALTINFO *)OSPPAuthIndFirstSourceAlt(trans->AuthInd);
        if(source != OSPC_OSNULL)
        {
            sourceval = (const char *)OSPPAuthIndGetSourceAltValue(source);

            sourcealt = (OSPTALTINFO *)OSPPAuthIndNextSourceAlt(trans->AuthInd, source);
            if(sourcealt != OSPC_OSNULL)
            {
                sourcealtval = (const char *)OSPPAuthIndGetSourceAltValue(sourcealt);
            }
        }

        destination = (OSPTALTINFO *)OSPPAuthIndFirstDestinationAlt(trans->AuthInd);
        if(destination != OSPC_OSNULL)
        {
            destval = (const char *)OSPPAuthIndGetDestinationAltValue(destination);

            destalt = (OSPTALTINFO *)OSPPAuthIndNextDestinationAlt(trans->AuthInd, destination);
            if(destalt != OSPC_OSNULL)
            {
                destaltval = (const char *)OSPPAuthIndGetDestinationAltValue(destalt);
            }
        }

        /* call ValidateAuthorisation */
        if(errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPTransactionValidateAuthorisation(ospvTransaction, 
                sourceval,                
                destval,  
                sourcealtval,                
                destaltval, 
                (const char *)OSPPAuthIndGetSourceNumber(trans->AuthInd),         
                (const char *)OSPPAuthIndGetDestNumber(trans->AuthInd), 
                OSPPAuthIndGetCallIdSize(trans->AuthInd),           
                (const void *)OSPPAuthIndGetCallIdValue(trans->AuthInd), 
                ospvSizeOfToken,            
                ospvToken,  ospvAuthorised,            
                ospvTimeLimit, ospvSizeOfDetailLog,       
                ospvDetailLog);
        }
    }

    return errorcode;
}
