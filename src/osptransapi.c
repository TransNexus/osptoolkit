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

/* osptransapi.cpp - API functions for transaction object. */

#include "osp/osp.h"
#include "osp/osptrans.h"
#include "osp/ospprovider.h"
#include "osp/ospxml.h"
#include "osp/ospmsginfo.h"
#include "osp/ospdest.h"
#include "osp/ospstatus.h"
#include "osp/ospusageind.h"
#include "osp/ospauthind.h"
#include "osp/osptokeninfo.h"
#include "osp/ospmsg.h"
#include "osp/ospfail.h"
#include "osp/osputils.h"
#include "osp/osptnprobe.h"
#include "osp/ospstatistics.h"
#include "osp/ospcapind.h"

#define OSPC_UNDEFINED_CALLID_NUM   ((unsigned)1)
#define OSPC_UNDEFINED_CALLID_STR   ((unsigned char*)"UNDEFINED")
#define OSPC_UNDEFINED_CALLID_SIZE  ((unsigned)9)   /* size of OSPC_UNDEFINED_CALLID_STR */

/*
 * OSPPTransactionSetServiceType
 * The API sets the Service Type in the transaction structure
 */
int OSPPTransactionSetServiceType(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvType)          /* In - Service Type */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPE_TRANS_STATE state;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionGetState(trans, &state);
        if (state != OSPC_TRANSNEW) {
            errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* Set the service type information */
        switch (ospvType) {
        case OSPC_SERVICE_VOICE:
        case OSPC_SERVICE_VIDEO:
        case OSPC_SERVICE_DATA:
        case OSPC_SERVICE_NPQUERY:
        case OSPC_SERVICE_CNAMQUERY:
            trans->HasServiceInfo = OSPC_TRUE;
            trans->ServiceType = ospvType;
            break;
        default:
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "Invalid input for OSPPTransactionSetService");
            break;
        }
    }

    return errcode;
}

/*
 * OSPPTransactionSetPricingInfo
 * The API sets the Pricing Information in the transaction structure
 */
int OSPPTransactionSetPricingInfo(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPT_PRICING_INFO *ospvInfo[])  /* In - Pricing Info */
{
    int errcode = OSPC_ERR_NO_ERROR, i;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPE_TRANS_STATE state;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionGetState(trans, &state);
        if (state != OSPC_TRANSNEW) {
            errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* Set the pricing info */
        for (i = 0; i < MAX_PRICING_INFO_ALLOWED; i++) {
            if ((errcode == OSPC_ERR_NO_ERROR) && ospvInfo[i] &&
                ospvInfo[i]->unit && ospvInfo[i]->currency &&
                (OSPM_STRLEN(ospvInfo[i]->unit) < OSPC_SIZE_UNIT) &&
                (OSPM_STRLEN(ospvInfo[i]->currency) < OSPC_SIZE_CONFID))
            {
                trans->HasPricingInfo = OSPC_TRUE;
                trans->PricingInfo[i].amount = ospvInfo[i]->amount;
                trans->PricingInfo[i].increment = ospvInfo[i]->increment;
                OSPM_STRCPY(trans->PricingInfo[i].unit, (const char *)ospvInfo[i]->unit);
                OSPM_STRCPY(trans->PricingInfo[i].currency, (const char *)ospvInfo[i]->currency);
            } else {
                trans->NumOfPricingInfoElements = i;
                break;
            }
        }
    }

    return errcode;
}

/*
 * OSPPTransactionModifyDeviceIdentifiers
 * This API is called to overwrite the ospvSource/ospvSourceDevice/
 * ospvDestination/ospvDestination device that was passed in the
 * Authorization Req/Validate API's.
 * This API provides the application with a mechanism of reporting Usage
 * with addresses different than those that were used while calling
 * RequestAuth/ValidateAuth/BuildUsageFromScratch/InitializeAtDevice etc APIs.
 * Parameters 2 through 5 are optional. However, at least one of the four needs to be
 * non-NULL.
 */
int OSPPTransactionModifyDeviceIdentifiers(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvSource,             /* In - optional */
    const char *ospvSourceDevice,       /* In - optional */
    const char *ospvDestination,        /* In - optional */
    const char *ospvDestinationDevice)  /* In - optional */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPTBOOL modifyallowed = OSPC_FALSE;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL, *altinfoToKeep = OSPC_OSNULL, *altinfoToKeep2 = OSPC_OSNULL;

    if ((ospvSource == OSPC_OSNULL) && (ospvSourceDevice == OSPC_OSNULL) && (ospvDestination == OSPC_OSNULL) && (ospvDestinationDevice == OSPC_OSNULL)) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for OSPPTransactionModifyDeviceIdentifiers");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionGetIsModifyDeviceIdAllowed(trans, &modifyallowed);
        if (modifyallowed == OSPC_TRUE) {
            if (trans->AuthRsp != OSPC_OSNULL) {
                /*
                 * We are the source. Change the non-NULL values passed in
                 * in the API within the CurrentDest obj.
                 */
                dest = trans->CurrentDest;

                if (ospvSource != OSPC_OSNULL) {
                    /*
                     * The check for duplicate calls has been removed.
                     * First delete the old UpdatedSourceAddr list, if any
                     * Then add the new values.
                     */
                    while (!OSPPListEmpty((OSPTLIST *)&(dest->UpdatedSourceAddr))) {
                        altinfo = (OSPT_ALTINFO *)OSPPListRemove((OSPTLIST *)&(dest->UpdatedSourceAddr));
                        if (altinfo != OSPC_OSNULL) {
                            OSPM_FREE(altinfo);
                            altinfo = OSPC_OSNULL;
                        }
                    }
                    /* Now add. */
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSource), ospvSource, OSPC_ALTINFO_TRANSPORT);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(dest->UpdatedSourceAddr), (void *)altinfo);
                    }
                }

                altinfo = OSPC_OSNULL;
                if (ospvSourceDevice != OSPC_OSNULL) {
                    /*
                     * The check for duplicate calls has been removed.
                     * First delete the old UpdatedDeviceInfo list, if any
                     * Then add the new values.
                     */
                    while (!OSPPListEmpty((OSPTLIST *)&(dest->UpdatedDeviceInfo))) {
                        altinfo = (OSPT_ALTINFO *)OSPPListRemove((OSPTLIST *)&(dest->UpdatedDeviceInfo));
                        if (altinfo != OSPC_OSNULL) {
                            OSPM_FREE(altinfo);
                            altinfo = OSPC_OSNULL;
                        }
                    }
                    /* Now add. */
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSourceDevice), ospvSourceDevice, OSPC_ALTINFO_TRANSPORT);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(dest->UpdatedDeviceInfo), (void *)altinfo);
                    }
                }
                altinfo = OSPC_OSNULL;

                if (ospvDestination != OSPC_OSNULL) {
                    /* Just overwrite on the old values, if any */
                    OSPPDestSetAddr(dest, ospvDestination);
                }

                if (ospvDestinationDevice != OSPC_OSNULL) {
                    /* Just overwrite on the old values, if any */
                    OSPPDestDevSetAddr(dest, ospvDestinationDevice);
                }
            } else if (trans->AuthInd != OSPC_OSNULL) {
                if ((ospvSource != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)) {
                    /* Overwrite the SourceAlternate in the AuthInd */
                    if (OSPPAuthIndHasSourceAlt(trans->AuthInd)) {
                        /*
                         * If srcAlt is present, delete the type- transport
                         * but keep the type-network.
                         */
                        altinfoToKeep = OSPC_OSNULL;
                        while (!OSPPListEmpty(&(trans->AuthInd->SourceAlternate))) {
                            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&(trans->AuthInd->SourceAlternate));
                            if (altinfo->Type == OSPC_ALTINFO_NETWORK) {
                                /* This node in the list corresponds to Network Id. Do not delete it. */
                                altinfoToKeep = altinfo;
                            } else {
                                OSPM_FREE(altinfo);
                                altinfo = OSPC_OSNULL;
                            }
                        }
                        /* Add back the AltInfo for networkId */
                        if (altinfoToKeep) {
                            OSPPListAppend((OSPTLIST *)&(trans->AuthInd->SourceAlternate), (void *)altinfoToKeep);
                            altinfoToKeep = OSPC_OSNULL;
                        }
                    }

                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSource), ospvSource, OSPC_ALTINFO_TRANSPORT);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->SourceAlternate), (void *)altinfo);
                    }
                    altinfo = OSPC_OSNULL;
                }

                if ((ospvSourceDevice != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)) {
                    /* Overwrite the deviceInfo in the AuthInd */
                    if (trans->AuthInd->DeviceInfo != OSPC_OSNULL) {
                        /*
                         * If srcAlt is present, delete the type- transport
                         * but keep the type-network.
                         */
                        altinfoToKeep = OSPC_OSNULL;
                        while (!OSPPListEmpty(&(trans->AuthInd->DeviceInfo))) {
                            altinfo = (OSPT_ALTINFO *)OSPPListRemove(&(trans->AuthInd->DeviceInfo));
                            if (altinfo->Type == OSPC_ALTINFO_NETWORK) {
                                /* This node in the list corresponds to Network Id. Do not delete it. */
                                altinfoToKeep = altinfo;
                            } else {
                                OSPM_FREE(altinfo);
                                altinfo = OSPC_OSNULL;
                            }
                        }
                        /* Add back the AltInfo for networkId */
                        if (altinfoToKeep) {
                            OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DeviceInfo), (void *)altinfoToKeep);
                            altinfoToKeep = OSPC_OSNULL;
                        }
                    }

                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSourceDevice), ospvSourceDevice, OSPC_ALTINFO_TRANSPORT);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DeviceInfo), (void *)altinfo);
                    }
                    altinfo = OSPC_OSNULL;
                }

                if ((ospvDestination != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)) {
                    /*
                     * This is a little messed up.
                     * NetworkId, Destination and DestinationDevice go to the same list.
                     * Thus, when we are modifying the list, we need to replace only the node for destination.
                     * If there is a node for destinationDevice, it should still remain there.
                     */
                    while (!OSPPListEmpty(&(trans->AuthInd->DestinationAlternate))) {
                        altinfo = (OSPT_ALTINFO *)OSPPListRemove(&(trans->AuthInd->DestinationAlternate));
                        if (altinfo != OSPC_OSNULL) {
                            if (altinfo->Type == OSPC_ALTINFO_H323) {
                                /* This node in the list corresponds to DestinationDevice. Do not delete it. */
                                altinfoToKeep = altinfo;
                            } else if (altinfo->Type == OSPC_ALTINFO_NETWORK) {
                                /* This node in the list corresponds to Network Id. Do not delete it. */
                                altinfoToKeep2 = altinfo;
                            } else {
                                OSPM_FREE(altinfo);
                            }
                            altinfo = OSPC_OSNULL;
                        }
                    }

                    /*
                     * We have emptied the list now.
                     * Add back the altinfo that corresponded to destinationDevice and Network Id
                     */
                    if (altinfoToKeep) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfoToKeep);
                        altinfoToKeep = OSPC_OSNULL;
                    }

                    if (altinfoToKeep2) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfoToKeep2);
                        altinfoToKeep2 = OSPC_OSNULL;
                    }
                    /* Now add the new destination */
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestination), ospvDestination, OSPC_ALTINFO_TRANSPORT);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                    }
                    altinfo = OSPC_OSNULL;
                }

                if ((ospvDestinationDevice != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)) {
                    /*
                     * This is a little messed up.
                     * NetworkId, Destination and DestinationDevice go to the same list.
                     * Thus, when we are modifying the list, we need to replace only the node for destination device.
                     * If there is a node for destination, it should still remain there.
                     */
                    while (!OSPPListEmpty(&(trans->AuthInd->DestinationAlternate))) {
                        altinfo = (OSPT_ALTINFO *)OSPPListRemove(&(trans->AuthInd->DestinationAlternate));
                        if (altinfo != OSPC_OSNULL) {
                            if (altinfo->Type == OSPC_ALTINFO_TRANSPORT) {
                                /* This node in the list corresponds to Destination. Do not delete it. */
                                altinfoToKeep = altinfo;
                            } else if (altinfo->Type == OSPC_ALTINFO_NETWORK) {
                                /* This node in the list corresponds to NetworkId. Do not delete it. */
                                altinfoToKeep2 = altinfo;
                            } else {
                                OSPM_FREE(altinfo);
                            }
                            altinfo = OSPC_OSNULL;
                        }
                    }

                    /*
                     * We have emptied the list now.
                     * Add back the altinfo that corresponded to destination
                     */

                    if (altinfoToKeep) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfoToKeep);
                        altinfoToKeep = OSPC_OSNULL;
                    }

                    if (altinfoToKeep2) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfoToKeep2);
                        altinfoToKeep2 = OSPC_OSNULL;
                    }

                    /* Now add the new node for destinationDevice */
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestinationDevice), ospvDestinationDevice, OSPC_ALTINFO_H323);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                    }
                    altinfo = OSPC_OSNULL;
                }
            } else {
                errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
            }
        }
    }

    return errcode;
}

/*
 * OSPPTransactionGetLookAheadInfoIfPresent():
 * This API should be called after calling ValidateAuthorization.
 * The API takes the transaction id as the input and returns the LookAhead
 * information if present in the token. If the LookAhead information is present
 * in the token, ospvHasLookAheadInfo is set to TRUE and the
 * corresponding values are also set. If the information is not present,
 * ospvHasLookAheadInfo is set to FALSE.
 */
int OSPPTransactionGetLookAheadInfoIfPresent(
    OSPTTRANHANDLE ospvTransaction,                     /* In - Transaction handle */
    OSPTBOOL *ospvHasLookAheadInfo,                     /* Out */
    char *ospvLookAheadDestination,                     /* Out */
    OSPE_PROTOCOL_NAME *ospvLookAheadProt,              /* Out */
    OSPE_DEST_OSPENABLED *ospvLookAheadDestOSPStatus)   /* Out */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_AUTH_IND **ospvAuthInd;
    const char *destinfo;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPT_ALTINFO *altinfoToKeep = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        *ospvHasLookAheadInfo = trans->TokenInfoHasLookAheadInfo;

        if (trans->TokenInfoHasLookAheadInfo == OSPC_TRUE) {
            trans->WasLookAheadInfoGivenToApp = OSPC_TRUE;

            /* Copy the LookAhead info into the variables passed */
            destinfo = OSPPTokenInfoGetLookAheadDestAlt(&(trans->TokenLookAheadInfo));
            OSPM_MEMCPY(ospvLookAheadDestination, destinfo, OSPM_STRLEN(destinfo) + 1);

            *ospvLookAheadProt = OSPPTokenInfoGetLookAheadProtocol(&(trans->TokenLookAheadInfo));

            *ospvLookAheadDestOSPStatus = OSPPTokenInfoGetLookAheadOSPVersion(&(trans->TokenLookAheadInfo));

            /*
             * Now, we need to overwrite the destiantionAlt
             * for usage reporting with the look ahead address
             * First, delete the old list.
             */
            ospvAuthInd = &(trans)->AuthInd;
            while (!OSPPListEmpty(&((*ospvAuthInd)->DestinationAlternate))) {
                altinfo = (OSPT_ALTINFO *)OSPPListRemove(&((*ospvAuthInd)->DestinationAlternate));
                if (altinfo != OSPC_OSNULL) {
                    if (altinfo->Type == OSPC_ALTINFO_NETWORK) {
                        /* This node in the list corresponds to Network Id. Do not delete it. */
                        altinfoToKeep = altinfo;
                    } else {
                        OSPM_FREE(altinfo);
                        altinfo = OSPC_OSNULL;
                    }
                }
            }

            OSPPListDelete(&((*ospvAuthInd)->DestinationAlternate));

            /* Create a new list of destiantion Alt */
            OSPPListNew((OSPTLIST *)&(trans->AuthInd->DestinationAlternate));

            /* Add back the AltInfo for networkId */
            if (altinfoToKeep) {
                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfoToKeep);
                altinfoToKeep = OSPC_OSNULL;
            }

            altinfo = OSPPAltInfoNew(OSPM_STRLEN(destinfo), destinfo, OSPC_ALTINFO_TRANSPORT);
            if (altinfo != OSPC_OSNULL) {
                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
            }
        }
    }

    return errcode;
}

/*
 * OSPPTransactionGetDestinationNetworkId() :
 * Reports the destination Network Id as returned in
 * either the (1) AuthRsp or the (2) Token
 * returns OSPC_ERR_NO_ERROR if successful, else a 'Request out of Sequence' errcode.
 */
int OSPPTransactionGetDestinationNetworkId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvSizeOfNetworkId,   /* In - Max size of network ID */
    char *ospvNetworkId)            /* In - network specific information */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    const char *destval = OSPC_OSNULL;
    const char *nid;
    OSPTBOOL found;

    if (ospvSizeOfNetworkId == 0) {
        errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
        OSPM_DBGERRORLOG(errcode, "No enough buffer to copy network ID.");
    } else {
        ospvNetworkId[0] = '\0';

        if ((trans = OSPPTransactionGetContext(ospvTransaction, &errcode)) != OSPC_OSNULL) {
            if (trans->AuthReq != OSPC_OSNULL) {
                /* We are the source. Get the information from the destination structure. */
                if (trans->State == OSPC_GET_DEST_SUCCESS) {
                    if ((dest = trans->CurrentDest) == OSPC_OSNULL) {
                        errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                        OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
                    } else if (OSPPDestHasNetworkAddr(dest)) {
                        nid = OSPPDestGetNetworkAddr(dest);
                        if (ospvSizeOfNetworkId > OSPM_STRLEN(nid)) {
                            OSPM_STRNCPY(ospvNetworkId, nid, ospvSizeOfNetworkId);
                        } else {
                            errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                            OSPM_DBGERRORLOG(errcode, "No enough buffer to copy network ID.");
                        }
                    } else {
                        errcode = OSPC_ERR_TRAN_NO_NETWORK_ID_IN_DEST;
                        OSPM_DBGERRORLOG(errcode, "Destination does not contain network Id\n");
                    }
                } else {
                    errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                    OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
                }
            } else if (trans->AuthInd != OSPC_OSNULL) {
                /* We are the destination. Get the information from the AuthInd structure. */
                found = OSPC_FALSE;
                altinfo = (OSPT_ALTINFO *)OSPPAuthIndFirstDestinationAlt(trans->AuthInd);
                while (altinfo != OSPC_OSNULL) {
                    if (altinfo->Type == OSPC_ALTINFO_NETWORK) {
                        found = OSPC_TRUE;
                        destval = OSPPAuthIndGetDestinationAltValue(altinfo);
                        if (destval != OSPC_OSNULL) {
                            if (ospvSizeOfNetworkId > OSPM_STRLEN(destval)) {
                                OSPM_STRNCPY(ospvNetworkId, destval, ospvSizeOfNetworkId);
                            } else {
                                errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                                OSPM_DBGERRORLOG(errcode, "No enough buffer to copy network ID.");
                            }
                        } else {
                            errcode = OSPC_ERR_TRAN_NO_NETWORK_ID_IN_DEST;
                            OSPM_DBGERRORLOG(errcode, "Destination does not contain network Id\n");
                        }
                        break;
                    } else {
                        altinfo = (OSPT_ALTINFO *)OSPPAuthIndNextDestinationAlt(trans->AuthInd, altinfo);
                    }
                }

                if (found == OSPC_FALSE) {
                    errcode = OSPC_ERR_TRAN_NO_NETWORK_ID_IN_DEST;
                    OSPM_DBGERRORLOG(errcode, "Destination does not contain network Id\n");
                }
            } else {
                errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
            }
        }
    }

    return errcode;
}

/*
 * OSPPTransactionGetDestProtocol() :
 * Reports the Protocol Information at the Current destination.
 * This API should be called right after Calling GetFirstDestination
 * or should be called right after Calling GetNextDestination.
 * The parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvType: Protocol type
 *   ospvName: the memory location in which the Toolkit puts the
 *     protocol. The returned value is one of the types defined in
 *     OSPE_PROTOCOL_NAME.
 * returns OSPC_ERR_NO_ERROR if successful, else a 'Request out of Sequence' errcode.
 */
int OSPPTransactionGetDestProtocol(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_PROTOCOL_NAME *ospvName)   /* Out - Protocol name */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    *ospvName = OSPC_PROTNAME_UNKNOWN;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (trans != OSPC_OSNULL) {
        dest = trans->CurrentDest;
        if (trans->State == OSPC_GET_DEST_SUCCESS) {
            if (dest == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
            } else {
                *ospvName = dest->Protocol;
            }
        } else {
            errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
        }
    }

    return errcode;
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
 *                            defined in OSPE_DEST_OSPENABLED.
 * returns OSPC_ERR_NO_ERROR if successful, else a 'Request out of Sequence' errcode.
 */
int OSPPTransactionIsDestOSPEnabled(
    OSPTTRANHANDLE ospvTransaction,                 /* In - Transaction handle */
    OSPE_DEST_OSPENABLED *ospvDestinationOSPStatus) /* Out - Destination OSP Status */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (trans != (OSPTTRANS *)NULL) {
        dest = trans->CurrentDest;
        if (trans->State == OSPC_GET_DEST_SUCCESS) {
            if (dest == (OSPT_DEST *)NULL) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
            } else {
                *ospvDestinationOSPStatus = dest->OSPVersion;
            }
        } else {
            errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
        }
    }

    return errcode;
}

/*
 * OSPPTransactionSetNetworkIds()
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
int OSPPTransactionSetNetworkIds(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvSrcNetworkId,   /* In - Src network specific information */
    const char *ospvDestNetworkId)  /* In - Dest network specific information */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    if (((ospvSrcNetworkId == OSPC_OSNULL) || (ospvSrcNetworkId[0] == '\0')) &&
        ((ospvDestNetworkId == OSPC_OSNULL) || (ospvDestNetworkId[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for OSPPTransactionSetNetworkIds");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    if ((trans != (OSPTTRANS *)NULL) && (errcode == OSPC_ERR_NO_ERROR)) {
        if (trans->State == OSPC_REPORT_USAGE_SUCCESS) {
            errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errcode, "Calling OSPPTransactionSetNetworkId After Usage has been Reported");
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            if ((trans->SrcNetworkId != OSPC_OSNULL) || (trans->DestNetworkId != OSPC_OSNULL)) {
                errcode = OSPC_ERR_TRAN_DUPLICATE_REQUEST;
                OSPM_DBGERRORLOG(errcode, "Duplicate Calls to OSPPTransactionSetNetworkIds");
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            if ((trans->AuthReq == OSPC_OSNULL) && (trans->AuthInd == OSPC_OSNULL)) {
                /*
                 * Neither Authorization Request nor Validate Authorization has
                 * been called. We dont care if it is the source or destn in
                 * this case. Just add the info to the transaction handler
                 */
                if (ospvSrcNetworkId != OSPC_OSNULL) {
                    OSPM_MALLOC(trans->SrcNetworkId, char, OSPM_STRLEN(ospvSrcNetworkId) + 1);
                    if (trans->SrcNetworkId != OSPC_OSNULL) {
                        OSPM_MEMCPY(trans->SrcNetworkId, ospvSrcNetworkId, OSPM_STRLEN(ospvSrcNetworkId) + 1);
                    } else {
                        errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                    }
                }

                if (ospvDestNetworkId != OSPC_OSNULL) {
                    OSPM_MALLOC(trans->DestNetworkId, char, OSPM_STRLEN(ospvDestNetworkId) + 1);
                    if (trans->DestNetworkId != OSPC_OSNULL) {
                        OSPM_MEMCPY(trans->DestNetworkId, ospvDestNetworkId, OSPM_STRLEN(ospvDestNetworkId) + 1);
                    } else {
                        errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                    }
                }
            } else if ((trans->AuthReq != OSPC_OSNULL) && (trans->AuthInd == OSPC_OSNULL)) {
                /*
                 * End point is a source, however it is calling the
                 * function out of sequence as it has already called
                 * AuthReq, return an error
                 */
                errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                OSPM_DBGERRORLOG(errcode, "Calling OSPPTransactionSetNetworkId After Authorization has been requested");
            } else {
                /*
                 * For the other two cases in which AuthInd!= OSPC_OSNULL
                 * End point is a destination, and Validate Authorization has already been called
                 * Since out of sequence calls are allowed on the destination, add this to the Authind req
                 * Check if there exists a list for destination alternates.If not then,
                 * Make a new list.
                 */
                if (trans->AuthInd->HasDestNetworkIdInToken == OSPC_FALSE) {
                    /*
                     * We will NOT overwrite the Network Id
                     * if specified in the token.
                     */
                    if (ospvDestNetworkId != OSPC_OSNULL) {
                        OSPM_MALLOC(trans->DestNetworkId, char, OSPM_STRLEN(ospvDestNetworkId) + 1);
                        if (trans->DestNetworkId != OSPC_OSNULL) {
                            OSPM_MEMCPY(trans->DestNetworkId, ospvDestNetworkId, OSPM_STRLEN(ospvDestNetworkId) + 1);
                        } else {
                            errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                        }
                    }

                    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvDestNetworkId != OSPC_OSNULL)) {
                        if (trans->AuthInd->DestinationAlternate == OSPC_OSNULL) {
                            /* Make a new list */
                            OSPPListNew((OSPTLIST *)&(trans->AuthInd->DestinationAlternate));
                        }
                        /* Add to the list */
                        altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestNetworkId), ospvDestNetworkId, OSPC_ALTINFO_NETWORK);
                        if (altinfo != OSPC_OSNULL) {
                            OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                            altinfo = OSPC_OSNULL;
                        }
                    }   /* ospvDestNetworkId != OSPC_OSNULL */
                }

                /*
                 * End point is a destination, and Validate Authorization has already been called
                 * Since out of sequence calls are allowed on the destination, add this to the Authind req
                 * Check if there exists a list for source alternates.If not then,
                 * Make a new list.
                 */
                if (ospvSrcNetworkId != OSPC_OSNULL) {
                    OSPM_MALLOC(trans->SrcNetworkId, char, OSPM_STRLEN(ospvSrcNetworkId) + 1);
                    if (trans->SrcNetworkId != OSPC_OSNULL) {
                        OSPM_MEMCPY(trans->SrcNetworkId, ospvSrcNetworkId, OSPM_STRLEN(ospvSrcNetworkId) + 1);
                    } else {
                        errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                    }
                }

                if ((errcode == OSPC_ERR_NO_ERROR) && (ospvSrcNetworkId != OSPC_OSNULL)) {
                    if (trans->AuthInd->SourceAlternate == OSPC_OSNULL) {
                        /* Make a new list */
                        OSPPListNew((OSPTLIST *)&(trans->AuthInd->SourceAlternate));
                    }
                    /* Add to the list */
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSrcNetworkId), ospvSrcNetworkId, OSPC_ALTINFO_NETWORK);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->SourceAlternate), (void *)altinfo);
                        altinfo = OSPC_OSNULL;
                    }
                }   /* errcode == OSPC_ERR_NO_ERROR */
            }
        }   /* errcode == OSPC_ERR_NO_ERROR */
    }
    /* trans != (OSPTTRANS*)NULL */
    return errcode;
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
 *   ospvSamples: the number of samples included in these statistics.
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
int OSPPTransactionAccumulateOneWayDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvSamples,           /* In - Number of samples included */
    unsigned ospvMinimum,           /* In - Minimum delay in milliseconds */
    unsigned ospvMean,              /* In - Mean of delay in milliseconds */
    float ospvVariance)             /* In - Variance of delay in square ms */
{
    int errcode = OSPC_ERR_NO_ERROR;
    int tnisnan = OSPC_TRUE;
    OSPTTRANS *trans = OSPC_OSNULL;
    unsigned currnumber = 0;
    double topower = 0;
    double mean = 0;
    double intpart = 0;
    OSPTBOOL accumallowed = OSPC_FALSE;
    OSPT_STATS_METRICS metrics;

    OSPM_MEMSET(&metrics, 0, sizeof(metrics));

    if ((ospvSamples == 0) || ((ospvSamples == 1) && ((ospvMinimum != ospvMean) || (ospvVariance != 0)))) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for AccumulateOneWayDelay");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionGetAccumAllowed(trans, &accumallowed);
        if (!accumallowed) {
            errcode = OSPC_ERR_TRAN_ACCUMULATE_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errcode, "AccumulateOneWay not allowed in this transaction state.");
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
                OSPM_DBGERRORLOG(errcode, "New failed for stats struct.");
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            /* make temporary copy so we don't corrupt our accumulator */
            OSPM_MEMCPY(&metrics, &(trans->Statistics->OneWay), sizeof(metrics));

            /* number of measurements */
            currnumber = metrics.samples;

            metrics.samples += ospvSamples;

            /* minimum measured value */
            if (metrics.hasvalue) {
                metrics.minimum = tr_min(metrics.minimum, ospvMinimum);
            } else {
                metrics.minimum = ospvMinimum;
            }

            /* sample mean - have to cast Samples to a float to get some precision on the mean */
            mean = ((metrics.mean * currnumber) + (ospvMean * ospvSamples)) / (float)metrics.samples;

            /* make sure we don't have overflow */
            OSPM_ISNAN(mean, tnisnan);

            if (tnisnan) {
                errcode = OSPC_ERR_TRAN_INVALID_CALC;
            } else {
                /* if remainder is >= .5, round up, else round down */
                if (OSPM_MODF(mean, &intpart) >= .5) {
                    metrics.mean = (unsigned)OSPM_CEIL(mean);
                } else {
                    metrics.mean = (unsigned)OSPM_FLOOR(mean);
                }

                /* sum of squares of samples */
                OSPM_POW((double)ospvMean, 2, topower);
                if (topower != OSPC_ERR_POW) {
                    metrics.squaressum = metrics.squaressum + ((ospvSamples - 1) * (double)ospvVariance) + (ospvSamples * topower);
                    topower = 0;
                } else {
                    errcode = (int)topower;
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                /* variance */
                OSPM_POW((double)metrics.mean, 2, topower);
                if (topower != OSPC_ERR_POW) {
                    metrics.variance = (float)(metrics.squaressum - (metrics.samples * topower)) / (metrics.samples - 1);
                    topower = 0;
                } else {
                    errcode = (int)topower;
                }
            }

            /* Only set state if we have actually done something, otherwise no change in state. */
            if (errcode == OSPC_ERR_NO_ERROR) {
                metrics.hasvalue = OSPC_TRUE;
                /* now copy values back to permanent accumulator */
                OSPM_MEMCPY(&(trans->Statistics->OneWay), &(metrics), sizeof(trans->Statistics->OneWay));
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_SUCCESS);
            } else {
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_FAIL);
            }
        }
    }

    return errcode;
}

/*
 * OSPPTransactionAccumulateTwoWayDelay()
 *
 * Accumulates round trip delay for transaction.
 *
 * The OSPPTransactionAccumulateTwoWayDelay function accumulates round trip
 * delay statistics for the call. These measurements can be made using, for
 * example, H.245 round trip delay requests during the call. Applications may
 * call this function an unlimited number of times during a transaction, but
 * only after the transaction has been authorised and before its usage details
 * are reported (i.e. after calling either the function
 * OSPPTransactionRequestAuthorisation or the function
 * OSPPTransactionValidateAuthorisation and before calling the function
 * OSPPTransactionReportUsage). Also, each call to this function must report
 * statistics for a separate and distinct set of measurements. In other words,
 * once OSPPTransactionAccumulateTwoWayDelay is successfully called,
 * the application should discard (at least for subsequent calls to the function)
 * the data and start calculating minimum, mean, variance measures anew.
 * Applications may use this function to report a single sample, or they may
 * report statistical measurements from a collection of samples.
 * The parameters to the function are:
 *   ospvTransaction: handle of the transaction object.
 *   ospvSamples: the number of samples included in these statistics.
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
int OSPPTransactionAccumulateTwoWayDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvSamples,           /* In - Number of samples included */
    unsigned ospvMinimum,           /* In - Minimum delay in milliseconds */
    unsigned ospvMean,              /* In - Mean of delay in milliseconds */
    float ospvVariance)             /* In - Variance of delay in square milliseconds */
{
    int errcode = OSPC_ERR_NO_ERROR;
    int tnisnan = OSPC_TRUE;
    OSPTTRANS *trans = OSPC_OSNULL;
    unsigned currnumber = 0;
    double topower = 0;
    double mean = 0;
    double intpart = 0;
    OSPTBOOL accumallowed = OSPC_FALSE;
    OSPT_STATS_METRICS metrics;

    OSPM_MEMSET(&metrics, 0, sizeof(metrics));

    if ((ospvSamples == 0) || ((ospvSamples == 1) && ((ospvMinimum != ospvMean) || (ospvVariance != 0)))) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for AccumulateOneWayDelay");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionGetAccumAllowed(trans, &accumallowed);
        if (!accumallowed) {
            errcode = OSPC_ERR_TRAN_ACCUMULATE_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errcode, "AccumulateTwoWay not allowed in this transaction state.");
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();

            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
                OSPM_DBGERRORLOG(errcode, "New failed for stats struct.");
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            /* make temporary copy so we don't corrupt our accumulator */
            OSPM_MEMCPY(&metrics, &(trans->Statistics->TwoWay), sizeof(metrics));

            /* number of measurements */
            currnumber = metrics.samples;

            metrics.samples += ospvSamples;

            /* minimum measured value */
            if (metrics.hasvalue) {
                metrics.minimum = tr_min(metrics.minimum, ospvMinimum);
            } else {
                metrics.minimum = ospvMinimum;
            }

            /* sample mean - have to cast Samples to a float to get some precision on the mean */
            mean = ((metrics.mean * currnumber) + (ospvMean * ospvSamples)) / (float)metrics.samples;

            OSPM_ISNAN(metrics.mean, tnisnan);

            if (tnisnan) {
                errcode = OSPC_ERR_TRAN_INVALID_CALC;
            } else {
                /* if remainder is >= .5, round up, else round down */
                if (OSPM_MODF(mean, &intpart) >= .5) {
                    metrics.mean = (unsigned)OSPM_CEIL(mean);
                } else {
                    metrics.mean = (unsigned)OSPM_FLOOR(mean);
                }

                /* sum of squares of samples */
                OSPM_POW((double)ospvMean, 2, topower);
                if (topower != OSPC_ERR_POW) {
                    metrics.squaressum = metrics.squaressum + ((ospvSamples - 1) * (ospvVariance)) + ((ospvSamples) * (float)topower);
                    topower = 0;
                } else {
                    errcode = (int)topower;
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                /* variance */
                OSPM_POW((double)metrics.mean, 2, topower);
                if (topower != OSPC_ERR_POW) {
                    metrics.variance = (float)(metrics.squaressum - (metrics.samples * topower)) / (metrics.samples - 1);
                    topower = 0;
                } else {
                    errcode = (int)topower;
                }
            }

            /* Only set state if we have actually done something, otherwise no change in state. */
            if (errcode == OSPC_ERR_NO_ERROR) {
                metrics.hasvalue = OSPC_TRUE;
                /* now copy values back to permanent accumulator */
                OSPM_MEMCPY(&(trans->Statistics->TwoWay), &metrics, sizeof(trans->Statistics->TwoWay));
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_SUCCESS);
            } else {
                OSPPTransactionSetState(trans, OSPC_ACCUMULATE_FAIL);
            }
        }
    }

    return errcode;
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
int OSPPTransactionDelete(
    OSPTTRANHANDLE ospvTransaction) /* In - Transaction handle. */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPTBOOL deleteallowed = OSPC_FALSE;
    OSPTTRANCOLLECTION *trancoll = OSPC_OSNULL;
    OSPTCOLLECTIONINDEX tranindex;
    unsigned cnt;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionGetDeleteAllowed(trans, &deleteallowed);

        if (deleteallowed == OSPC_TRUE) {
            OSPPTransactionDeleteRequest(trans);

            OSPPTransactionDeleteResponse(trans);

            OSPPTransactionDeleteAuthInd(trans);

            OSPPTransactionDeleteAuthCnf(trans);

            OSPPTransactionDeleteUsageInd(trans);

            OSPPTransactionDeleteUsageCnf(trans);

            OSPPTransactionDeleteStatistics(trans);

            OSPPTransactionDeleteCapCnf(trans);

            errcode = OSPPProviderGetTransactionCollection(trans->Provider, &trancoll);

            if (errcode == OSPC_ERR_NO_ERROR) {
                tranindex.Index = OSPM_GET_TRANSACTION_INDEX(ospvTransaction);
                OSPPTransactionCollectionRemoveItem(trancoll, tranindex);
            }

            if (trans->SrcNetworkId != OSPC_OSNULL) {
                OSPM_FREE(trans->SrcNetworkId);
            }

            if (trans->DestNetworkId != OSPC_OSNULL) {
                OSPM_FREE(trans->DestNetworkId);
            }

            for (cnt = OSPC_SESSIONID_START; cnt < OSPC_SESSIONID_NUMBER; cnt++) {
                if (trans->SessionId[cnt] != OSPC_OSNULL) {
                    OSPPCallIdDelete(&trans->SessionId[cnt]);
                }
            }

            for (cnt = 0; cnt < OSPC_MAX_INDEX; cnt++) {
                if (trans->CustomInfo[cnt] != OSPC_OSNULL) {
                    OSPM_FREE(trans->CustomInfo[cnt]);
                }
            }

            OSPM_FREE(trans);
            trans = OSPC_OSNULL;
        } else {
            errcode = OSPC_ERR_TRAN_DELETE_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errcode, "delete not allowed in this trans state");
        }
    }

    return errcode;
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
 *      the destination device. The value is expressed as either a DNS name or an IP
 *      address enclosed in square brackets, followed by an optional colon and
 *      TCP port number. Examples of valid destinations include
 *                      "gateway1.carrier.com" and "[172.16.1.2]:112".
 *  ospvSizeOfToken: pointer to a variable which, on input, contains the size
 *      of the memory buffer in which the function should store the
 *      authorization token for the destination. If the value is not large
 *      enough to accommodate the token, then an error is indicated and no
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the authorization token.
 *  ospvToken: memory location in which to store the authorization token for
 *      this destination. In general, tokens are opaque, binary objects.
 * The SDK library is able to perform this function without network
 * interaction, and, therefore, does not block for network input or output
 * during its execution.
 * The function returns an error code or zero (if the operation was successful).
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionGetFirstDestination(
    OSPTTRANHANDLE ospvTransaction,         /* In  - Transaction handle */
    unsigned ospvSizeOfTimestamp,           /* In  - Max size for timestamp string */
    char *ospvValidAfter,                   /* Out - Valid After time in string format */
    char *ospvValidUntil,                   /* Out - Valid Until time in string format */
    unsigned *ospvTimeLimit,                /* Out - Number of seconds call is authorised for */
    unsigned *ospvSizeOfCallId,             /* In/Out - Max size for CallId string Actual size of CallId string */
    void *ospvCallId,                       /* Out - Call Id string */
    unsigned ospvSizeOfCalledNumber,        /* In - Max size of called number */
    char *ospvCalledNumber,                 /* Out - Called number string */
    unsigned ospvSizeOfCallingNumber,       /* In - Max size of calling number */
    char *ospvCallingNumber,                /* Out - Calling number string */
    unsigned ospvSizeOfDestination,         /* In - Max size of destination string */
    char *ospvDestination,                  /* Out - Destination string */
    unsigned ospvSizeOfDestinationDevice,   /* In - Max size of dest device string */
    char *ospvDestinationDevice,            /* Out - Dest device string */
    unsigned *ospvSizeOfToken,              /* In/Out - Max size of token string Actual size of token string */
    void *ospvToken)                        /* Out - Token string */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &error);
    if ((error == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
        error = OSPPTransactionGetDestAllowed(trans);
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* Make sure we have a response */
        if (trans->AuthRsp == OSPC_OSNULL) {
            error = OSPC_ERR_TRAN_RESPONSE_NOT_FOUND;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* if no errors have occurred, get the destination information */
        if (OSPPAuthRspHasDest(trans->AuthRsp) == OSPC_FALSE) {
            error = OSPC_ERR_TRAN_DEST_INVALID;
            OSPM_DBGERRORLOG(error, "destination not found");
        } else {
            error = OSPPTransactionGetDestination(trans,
               OSPC_FAIL_NONE,
               ospvSizeOfTimestamp, ospvValidAfter, ospvValidUntil,
               ospvTimeLimit,
               ospvSizeOfCallId, ospvCallId,
               ospvSizeOfCalledNumber, ospvCalledNumber,
               ospvSizeOfCallingNumber, ospvCallingNumber,
               ospvSizeOfDestination, ospvDestination,
               ospvSizeOfDestinationDevice, ospvDestinationDevice,
               ospvSizeOfToken, ospvToken);
        }
    }

    /* Set transaction state */
    if (error == OSPC_ERR_NO_ERROR) {
        OSPPTransactionSetState(trans, OSPC_GET_DEST_SUCCESS);
        trans->HasGetDestSucceeded = OSPC_TRUE;
    } else {
        OSPPTransactionSetState(trans, OSPC_GET_DEST_FAIL);
    }

    return error;
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
 *      of the destination device. The value is expressed as either a DNS name or an IP
 *      address enclosed in square brackets, followed by an optional colon
 *      and TCP port number.
 *          Examples of valid destinations include "gateway1.carrier.com" and
 *                                                          "[172.16.1.2]:112".
 *  ospvSizeOfToken: pointer to a variable which, on input, contains the size
 *      of the memory buffer in which the function should store the
 *      authorization token for the destination. If the value is not large
 *      enough to accommodate the token, then an error is indicated and no
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the authorization token.
 *  ospvToken: memory location in which to store the authorization token for
 *      this destination. In general, tokens are opaque, binary objects.
 * The SDK library is able to perform this function without network
 * interaction, and, therefore, does not block for network input or output
 * during its execution.
 * The function returns an error code or zero (if the operation was successful).
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionGetNextDestination(
    OSPTTRANHANDLE ospvTransaction,         /* In - Transaction handle */
    OSPEFAILREASON ospvFailureReason,       /* In - Failure code */
    unsigned ospvSizeOfTimestamp,           /* In - Max size of timestamp string */
    char *ospvValidAfter,                   /* Out - Valid after time string */
    char *ospvValidUntil,                   /* Out - Valid until time string */
    unsigned *ospvTimeLimit,                /* Out - Number of seconds call is authorised for */
    unsigned *ospvSizeOfCallId,             /* In - Max size of call id string */
    void *ospvCallId,                       /* Out - Call Id string */
    unsigned ospvSizeOfCalledNumber,        /* In - Max size of called number */
    char *ospvCalledNumber,                 /* Out - Called number string */
    unsigned ospvSizeOfCallingNumber,       /* In - Max size of calling number */
    char *ospvCallingNumber,                /* Out - Calling number string */
    unsigned ospvSizeOfDestination,         /* In - Max size of destination string */
    char *ospvDestination,                  /* Out - Destination string */
    unsigned ospvSizeOfDestinationDevice,   /* In - Max size of dest device string */
    char *ospvDestinationDevice,            /* Out - Dest device string */
    unsigned *ospvSizeOfToken,              /* In/Out - Max size of token string Actual size of token string */
    void *ospvToken)                        /* Out - Token string */
{
    int error = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &error);
    if ((error == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
        error = OSPPTransactionGetDestAllowed(trans);
    }

    if (error == OSPC_ERR_NO_ERROR) {
        /* In ver2.9.3, we have removed the check to not accept 0 as a TC Code. The toolkit now accepts 0. */
        /* Now check for acceptable failure code */
        error = OSPPFailReasonFind(ospvFailureReason);

        /*
         * If the FailureReason = 0, change it to DEFAULT_GETNEXTDEST_NO_ERROR, which is a NONNULL value.
         * This is being done because both the - GetFirst and GetNext function calls internally call
         * GetDestination. This function expects ospvFailureReason to be 0 when the parent calling function
         * is GetFirst and NonZero when the parent calling function is GetNext.
         * To avoid too many changes in the Toolkit, we are overwriting the value 0 with a unique
         * NonZero value here. When we have to actually set the Error Code in the destination structure,
         * we will convert it back to 0. Probably not the best method, but continuing like this now.
         */
        if (ospvFailureReason == OSPC_FAIL_NONE) {
            ospvFailureReason = DEFAULT_GETNEXTDEST_NO_ERROR;
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        if (trans->CurrentDest == OSPC_OSNULL) {
            error = OSPC_ERR_TRAN_NO_MORE_RESPONSES;
            OSPM_DBGERRORLOG(error, "No more destinations.");
        }
    }

    if (error == OSPC_ERR_NO_ERROR) {
        error = OSPPTransactionGetDestination(trans,
            ospvFailureReason,
            ospvSizeOfTimestamp, ospvValidAfter, ospvValidUntil,
            ospvTimeLimit,
            ospvSizeOfCallId, ospvCallId,
            ospvSizeOfCalledNumber, ospvCalledNumber,
            ospvSizeOfCallingNumber, ospvCallingNumber,
            ospvSizeOfDestination, ospvDestination,
            ospvSizeOfDestinationDevice, ospvDestinationDevice,
            ospvSizeOfToken, ospvToken);
    }

    /* Set transaction state */
    if (error == OSPC_ERR_NO_ERROR) {
        OSPPTransactionSetState(trans, OSPC_GET_DEST_SUCCESS);
        trans->HasGetDestSucceeded = OSPC_TRUE;
    } else {
        OSPPTransactionSetState(trans, OSPC_GET_DEST_FAIL);
    }

    if (error == OSPC_ERR_TRAN_DEST_INVALID) {
        error = OSPC_ERR_TRAN_NO_MORE_RESPONSES;
    }

    return error;
}

/*
 * OSPPTransactionBuildUsageFromScratch()
 *
 * The OSPPTransactionBuildUsageFromScratch function builds a (newly created)
 * transaction object. Applications can use this with a distributed
 * architecture in which the systems requesting and validating authorization
 * (e.g. H.323 gatekeepers) are different than the systems that ultimately
 * report usage information (e.g. H.323 gateways). As such, this function is
 * (in a source device) an alternative to the combination of  the
 * OSPPTransactionRequestAuthorisation  function (to initiate a call) and the
 * OSPPTransactionGetFirstDestination function (to define the endpoints of the
 * call. In the destination device, this function serves as an alternative to
 * the function  OSPPTransactionValidateAuthorisation.
 * The function is very similar to the OSPPTransactionInitializeAtDevice function
 * except that it does not expect the user to include the token as a part of
 * the API.
 * Parameters to the function are:
 *  ospvTransaction: handle of the (previously created) transaction object.
 *      For multiple calls to the API, this value should remain same.
 *  ospvServerTransactionId: The transaction id generated by the OSP Server.
 *      This need not be initialized when the API is called for the 2nd time or thereafter.
 *  ospvRole: indicates the system calling this function is acting  as the
 *      source/destination for the call, or RADIUS.
 *      This is mandatory and should always be populated.
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
 *      Either ospvDestination or ospvDestinationDevice should be non-NULL.
 *  ospvSourceDevice: character string identifying the source device.
 *      This could be the previous hop Gateway.
 *      It is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                  "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 *  ospvDestinationDevice: character string identifying the destination device.
 *      The value is expressed as either a DNS name or an IP address enclosed
 *      in square brackets, followed by an optional colon and TCP port number.
 *      Examples of valid destinations include "gateway1.carrier.com" and
 *                                                      "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 *  ospvCallingNumber: character string containing the calling party's number
 *      expressed as a full international number conforming to the ITU E.164
 *      standard (with no punctuation).
 *      When the API is called for the 2nd time or thereafter this value can
 *      be left NULL.
 *  ospvCallingNumberFormat: Value that identifies the type of calling number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvCalledNumber: character string containing the called number, expressed
 *      as a full international number conforming to the ITU E.164 standard
 *      (with no punctuation).
 *      When the API is called for the 2nd time or thereafter this value can
 *      be left NULL.
 *  ospvCalledNumberFormat: Value that identifies the type of called number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvSizeOfCallId: size of the memory buffer containing the call identifier.
 *      This should always be populated.
 *  ospvCallId: memory location containing the H.323 call identifier for the
 *      call.
 *      This should always be populated.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionBuildUsageFromScratch(
    OSPTTRANHANDLE ospvTransaction,             /* In - Transaction handle */
    OSPTUINT64 ospvServerTransactionId,         /* In - OSP Server Transaction Id */
    unsigned ospvRole,                          /* In - ogw, tgw, or radius */
    const char *ospvSource,                     /* In - Source of call */
    const char *ospvDestination,                /* In - Destination for call */
    const char *ospvSourceDevice,               /* In - SourceDevice */
    const char *ospvDestinationDevice,          /* In - DestinationDevice */
    const char *ospvCallingNumber,              /* In - Calling number */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat, /* In - Calling number format : sip/e.164/url */
    const char *ospvCalledNumber,               /* In - Called number */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat,  /* In - Called number format : sip/e.164/url */
    unsigned ospvSizeOfCallId,                  /* In - Size of Callid */
    const void *ospvCallId,                     /* In - Call identifier */
    OSPEFAILREASON ospvFailureReason,           /* Reason that the previous destination failed */
    unsigned *ospvSizeOfDetailLog,              /* In/Out - Max size of detail log\ Actual size of detail log */
    void *ospvDetailLog)                        /* In - Pointer to storage for detail log */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    unsigned numcallids = 1;
    OSPT_CALL_ID *callid = OSPC_OSNULL;
    OSPT_AUTH_IND *authind = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;

    /* verify input */
    if (((ospvDestination == OSPC_OSNULL) &&
         (ospvDestinationDevice == OSPC_OSNULL)) ||
        (ospvSizeOfCallId == 0) ||
        ((ospvCallingNumberFormat < OSPC_NFORMAT_START) || (ospvCallingNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvCalledNumberFormat < OSPC_NFORMAT_START) || (ospvCalledNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        (ospvCallId == OSPC_OSNULL)) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "invalid input for Initialize");
    }

    /* Get transaction context */
    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    /*
     * move data into authreq & authrsp, (OGW) struct so
     * report usage will work.
     * We are adding a destination to authrsp->dest, setting current dest to
     * point to it.
     */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvRole == OSPC_ROLE_SOURCE) {
            if (trans->AuthReq != OSPC_OSNULL) {
                /*
                 * This is the 2nd time that the API is being called.
                 * Just add this destination to the list.
                 */
                errcode = OSPPFailReasonFind(ospvFailureReason);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    /* We are only adding a destination
                     * first set failure code in authrsp->currentdest
                     */
                    OSPPDestSetTermCause(trans->CurrentDest, OSPC_TCAUSE_Q850, ospvFailureReason, OSPC_OSNULL);

                    /* now build new dest */
                    errcode = OSPPTransactionResponseBuild(trans, ospvDestination, ospvCallingNumber, ospvSizeOfCallId, ospvCallId,
                        5,  /* Just giving a size because the Response function does not like a size of 0 */
                        "ABCDE");
                }
            } else {
                if ((ospvCallingNumber == OSPC_OSNULL) || (ospvCalledNumber == OSPC_OSNULL)) {
                    errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                    OSPM_DBGERRORLOG(errcode, "invalid input for Initialize");
                }

                /* create callid structure */
                if (errcode == OSPC_ERR_NO_ERROR) {
                    callid = OSPPCallIdNew(ospvSizeOfCallId, (const unsigned char *)ospvCallId);
                    if (callid == OSPC_OSNULL) {
                        errcode = OSPC_ERR_DATA_NOCALLID;
                    }
                }

                if (errcode == OSPC_ERR_NO_ERROR) {
                    /* we need to build authreq and authrsp */
                    errcode = OSPPTransactionRequestNew(trans, ospvSource,
                        ospvSourceDevice,
                        ospvCallingNumber,
                        ospvCalledNumber,
                        OSPC_OSNULL,
                        numcallids, &callid,
                        OSPC_OSNULL, &numcallids, ospvSizeOfDetailLog, ospvDetailLog);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        trans->CallingNumberFormat = ospvCallingNumberFormat;
                        trans->CalledNumberFormat = ospvCalledNumberFormat;
                    }

                    /* delete callid - TransactionRequestNew created new one */
                    OSPPCallIdDelete(&callid);
                }

                if (errcode == OSPC_ERR_NO_ERROR) {
                    errcode = OSPPTransactionResponseBuild(trans, ospvDestination, ospvCallingNumber, ospvSizeOfCallId, ospvCallId,
                        5,  /* Just giving a size because the Response function does not like a size of 0 */
                        "ABCDE");

                    /* Populate Transaction Id */
                    trans->TransactionID = ospvServerTransactionId;
                    trans->HasTransactionID = OSPC_TRUE;

                    /* Set correct role */
                    OSPPAuthRspSetRole(trans->AuthRsp, ospvRole);
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                trans->CurrentDest->RoleState = trans->RoleState;
                trans->CurrentDest->RoleFormat = trans->RoleFormat;
                trans->CurrentDest->RoleVendor = trans->RoleVendor;
            }
        } else if (ospvRole == OSPC_ROLE_DESTINATION) {
            if (trans->AuthInd != OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                OSPM_DBGERRORLOG(errcode, "Transaction already initialized");
            } else {
                if ((ospvCallingNumber == OSPC_OSNULL) || (ospvCalledNumber == OSPC_OSNULL)) {
                    errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                    OSPM_DBGERRORLOG(errcode, "invalid input for Initialize");
                }

                /* Need to build the auth Ind element now */
                if (errcode == OSPC_ERR_NO_ERROR) {
                    authind = OSPPAuthIndNew();
                }

                /* populate the new one */
                if ((authind != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)) {
                    OSPPAuthIndSetTimestamp(authind, time(OSPC_OSNULL));
                    callid = OSPPCallIdNew(ospvSizeOfCallId, (const unsigned char *)ospvCallId);
                    if (callid != OSPC_OSNULL) {
                        OSPPAuthIndSetRole(authind, ospvRole);
                        OSPPAuthIndSetCallId(authind, callid);
                        OSPPAuthIndSetSourceNumber(authind, ospvCallingNumber);
                        trans->CallingNumberFormat = ospvCallingNumberFormat;
                        OSPPAuthIndSetDestNumber(authind, ospvCalledNumber);
                        trans->CalledNumberFormat = ospvCalledNumberFormat;
                        OSPPListNew(&(authind->Tokens));
                        OSPPCallIdDelete(&callid);
                    } else {
                        errcode = OSPC_ERR_TRAN_CALLID_NOT_FOUND;
                        OSPM_DBGERRORLOG(errcode, "callid is null");
                    }

                    if (errcode == OSPC_ERR_NO_ERROR) {
                        /* create the destination object */
                        dest = OSPPDestNew();
                        if (dest == OSPC_OSNULL) {
                            errcode = OSPC_ERR_DATA_NO_DEST;
                        } else {
                            OSPPDestSetCallId(dest, (const unsigned char *)ospvCallId, ospvSizeOfCallId);

                            OSPPDestSetNumber(dest, ospvCalledNumber);

                            dest->RoleState = trans->RoleState;
                            dest->RoleFormat = trans->RoleFormat;
                            dest->RoleVendor = trans->RoleVendor;

                            OSPPAuthIndSetDest(authind, dest);

                            trans->CurrentDest = dest;
                            dest = OSPC_OSNULL;
                        }
                    }
                    trans->AuthInd = authind;

                    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvSourceDevice != OSPC_OSNULL)) {
                        /* device information - create a linked list */
                        OSPPListNew((OSPTLIST *)&(trans->AuthInd->DeviceInfo));
                        altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSourceDevice), ospvSourceDevice, OSPC_ALTINFO_TRANSPORT);
                        if (altinfo != OSPC_OSNULL) {
                            OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DeviceInfo), (void *)altinfo);
                        }
                        altinfo = OSPC_OSNULL;
                    }

                    /* end if ospvSourceDevice != OSPC_OSNULL */
                    /* --------------------------------------
                     * SourceAlternate
                     * --------------------------------------
                     */
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        if ((ospvSource != OSPC_OSNULL) || (trans->SrcNetworkId != OSPC_OSNULL)) {
                            /* source alternates - create a linked list */
                            OSPPListNew((OSPTLIST *)&(trans->AuthInd->SourceAlternate));

                            if (trans->SrcNetworkId != OSPC_OSNULL) {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->SrcNetworkId), trans->SrcNetworkId, OSPC_ALTINFO_NETWORK);
                                if (altinfo != OSPC_OSNULL) {
                                    OSPPListAppend((OSPTLIST *)&(trans->AuthInd->SourceAlternate), (void *)altinfo);
                                }
                            }
                            altinfo = OSPC_OSNULL;

                            if (ospvSource != OSPC_OSNULL) {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSource), ospvSource, OSPC_ALTINFO_TRANSPORT);
                                if (altinfo != OSPC_OSNULL) {
                                    OSPPListAppend((OSPTLIST *)&(trans->AuthInd->SourceAlternate), (void *)altinfo);
                                }
                            }

                            altinfo = OSPC_OSNULL;
                        } else {
                            errcode = OSPC_ERR_TRAN_SOURCE_INVALID;
                        }
                    }

                    /* end  if (errcode == OSPC_ERR_NO_ERROR) */
                    /* -----------------------------------------------------
                     * DestinationAlternate
                     * -----------------------------------------------------
                     */
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        if ((ospvDestination != OSPC_OSNULL) || (ospvDestinationDevice != OSPC_OSNULL) || (trans->DestNetworkId != OSPC_OSNULL)) {
                            /* destination alternates - create a linked list */
                            OSPPListNew((OSPTLIST *)&(trans->AuthInd->DestinationAlternate));

                            if (trans->DestNetworkId != OSPC_OSNULL) {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->DestNetworkId), trans->DestNetworkId, OSPC_ALTINFO_NETWORK);
                                if (altinfo != OSPC_OSNULL) {
                                    OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                                }
                            }

                            altinfo = OSPC_OSNULL;

                            if (ospvDestination != OSPC_OSNULL) {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestination), ospvDestination, OSPC_ALTINFO_TRANSPORT);
                                if (altinfo != OSPC_OSNULL) {
                                    OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                                }
                            }

                            altinfo = OSPC_OSNULL;

                            if (ospvDestinationDevice != OSPC_OSNULL) {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestinationDevice), ospvDestinationDevice, OSPC_ALTINFO_H323);
                                if (altinfo != OSPC_OSNULL) {
                                    OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                                }
                            }   /* end if ospvDestinationDevice != OSPC_OSNULL */
                        } else {
                            errcode = OSPC_ERR_TRAN_DEST_INVALID;
                        }
                    } /* end  if (errcode == OSPC_ERR_NO_ERROR) */

                    /* Populate Transaction Id */
                    trans->TransactionID = ospvServerTransactionId;
                    trans->HasTransactionID = OSPC_TRUE;

                    /* Make sure role is correct */
                    OSPPAuthIndSetRole(trans->AuthInd, ospvRole);
                }
            }
        } else {
            errcode = OSPC_ERR_TRAN_NOT_IMPLEMENTED;
            OSPM_DBGERRORLOG(errcode, "Invalid system type.");

        }
    }

    /* Set transaction state */
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionSetState(trans, OSPC_INITIALIZE_SUCCESS);
    } else {
        OSPPTransactionSetState(trans, OSPC_INITIALIZE_FAIL);
    }

    return errcode;
}

/*
 * OSPPTransactionSetDestinationCount()
 *
 * The OSPPTransactionSetDestinationCount function sets destination count.
 * When a stateless proxy sends a stop source CDR, it may indicate which
 * destination number the usage indication is for.  For example, if the first
 * two set-up attempts failed and the call was completed by the 3rd destination,
 * the proxy should set the destination count to 3.  The proxy would call this
 * API after building a usage indication (using OSPPTransactionBuildUsageFromScratch)
 * and before reporting the message to a settlement server (using OSPPTransactionReportUsage())
 *
 * Parameters to the function are:
 *  ospvTransaction: handle of the (previously created) transaction object.
 *  ospvDestinationCount: The destination count
 *
 * Specific error codes and their meanings can be found in the osperrno.h file.
 *
 */
int OSPPTransactionSetDestinationCount(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvDestinationCount)  /* In - Optional Destination Count, 0 if n/a */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) && (trans->AuthReq != OSPC_OSNULL) && (trans->CurrentDest != OSPC_OSNULL)) {
        OSPPDestSetDestinationCount(trans->CurrentDest, ospvDestinationCount);
    } else {
        errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
        OSPM_DBGERRORLOG(errcode,
            "OSPPTransactionSetDestinationCount should be called after OSPPTransactionBuildUsageFromScratch for the source CDR");
    }

    return errcode;
}

/*
 * OSPPTransactionInitializeAtDevice()
 *
 * The OSPPTransactionInitializeAtDevice function initializes a (newly created)
 * transaction object. Applications can use this with a distributed
 * architecture in which the systems requesting and validating authorization
 * (e.g. H.323 gatekeepers) are different than the systems that ultimately
 * report usage information (e.g. H.323 gateways). As such, this function is
 * (in a source device) an alternative to the combination of  the
 * OSPPTransactionRequestAuthorisation  function (to initiate a call) and the
 * OSPPTransactionGetFirstDestination function (to define the endpoints of the
 * call. In the destination device, this function serves as an alternative to
 * the function  OSPPTransactionValidateAuthorisation.
 * Parameters to the function are:
 *  ospvTransaction: handle of the (previously created) transaction object.
 *  ospvRole: indicates the system calling this function is acting
 *      as the source or destination for the call.
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
 *  ospvSourceDevice: character string identifying the source device.
 *      This could be the previous hop Gateway.
 *      It is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                  "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 *  ospvDestinationDevice: character string identifying the destination device.
 *      The value is expressed as either a DNS name or an IP address enclosed
 *      in square brackets, followed by an optional colon and TCP port number.
 *      Examples of valid destinations include "gateway1.carrier.com" and
 *                                                      "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 *  ospvCallingNumber: character string containing the calling party's number
 *      expressed as a full international number conforming to the ITU E.164
 *      standard (with no punctuation).
 *  ospvCallingNumberFormat: Value that identifies the type of calling number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvCalledNumber: character string containing the called number, expressed
 *      as a full international number conforming to the ITU E.164 standard
 *      (with no punctuation).
 *  ospvCalledNumberFormat: Value that identifies the type of called number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvSizeOfCallId: size of the memory buffer containing the call identifier.
 *  ospvCallId: memory location containing the H.323 call identifier for the
 *      call.
 *  ospvSizeOfToken: size of the memory buffer containing an authorization
 *      token for the call.
 *  ospvToken: memory location containing an authorization token.
 *  ospvAuthorised: pointer to a variable in which the function will indicate
 *      whether or not the call is authorised. On return, a non-zero value
 *      indicates that the call is authorised by the provider, while a zero
 *      value indicates an authorization failure.
 *  ospvTimeLimit: pointer to a variable in which to place the number of
 *      seconds for which the call is initially authorised. A value of zero
 *      indicates that no limit exists. Note that the initial time limit may be
 *      extended during the call by using the function
 *      OSPPTransactionRequestReauthorisation.
 *  ospvSizeOfDetailLog: pointer to a variable which, on input, contains the
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero,
 *      applications indicate that they do not wish a detail log for the
 *      authorization validation.
 *  ospvDetailLog: pointer to a location in which to store a detail log for the
 *      validation. If this pointer is not NULL, and if the ospvSizeOfDetailLog
 *      parameter is non-zero, then the library will store a copy of the
 *      authorization confirmation obtained from the settlement provider,
 *      including the settlement provider's digital signature.
 *  ospvTokenAlgo: This can take either of the 3 values - TOKEN_ALGO_SIGNED,
 *      TOKEN_ALGO_UNSIGNED, and TOKEN_ALGO_BOTH. If the value is set to
 *      TOKEN_ALGO_SIGNED the toolkit expects the token to be signed, and
 *      validates the signature as a part of token validation. If the value
 *      is set to TOKEN_ALGO_UNSIGNED, an unsigned token is expected. If the
 *      value is set to TOKEN_ALGO_BOTH, then the toolkit accepts either a
 *      signed token or an unsigned token.
 * If the provider has been configured to perform local validation, the SDK
 * library is able to perform this function without network interaction, and,
 * therefore, does not block for network input or output during its execution.
 * If local validation is not used, this function blocks until authorization
 * has been validated, refused, or an error has been detected. The Open
 * Settlement Protocol SDK Porting Guide includes information on modifying that
 * behavior to prevent blocking.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionInitializeAtDevice(
    OSPTTRANHANDLE ospvTransaction,                 /* In - Transaction handle */
    unsigned ospvRole,                              /* In - Is this the ogw or tgw */
    const char *ospvSource,                         /* In - Source of call */
    const char *ospvDestination,                    /* In - Destination for call */
    const char *ospvSourceDevice,                   /* In - SourceDevice */
    const char *ospvDestinationDevice,              /* In - DestinationDevice */
    const char *ospvCallingNumber,                  /* In - Calling number */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat,     /* In - Calling number Format : sip/e.164/url */
    const char *ospvCalledNumber,                   /* In - Called number */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat,      /* In - Called number Format : sip/e.164/url */
    unsigned ospvSizeOfCallId,                      /* In - Size of Callid */
    const void *ospvCallId,                         /* In - Call identifier */
    unsigned ospvSizeOfToken,                       /* In - Size of Token */
    const void *ospvToken,                          /* In - token */
    unsigned *ospvAuthorised,                       /* Out - indicates authorization */
    unsigned *ospvTimeLimit,                        /* Out - number of seconds allowed */
    unsigned *ospvSizeOfDetailLog,                  /* In/Out - Max size of detail log Actual size of detail log */
    void *ospvDetailLog, unsigned ospvTokenAlgo)    /* In - Algorithm to be used for Validating Token */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    unsigned numcallids = 1;
    OSPT_CALL_ID *callid = OSPC_OSNULL;

    /* verify input */
    if (((ospvDestination == OSPC_OSNULL) &&
        (ospvDestinationDevice == OSPC_OSNULL)) ||
        (ospvCallingNumber == OSPC_OSNULL) ||
        (ospvCalledNumber == OSPC_OSNULL) ||
        (ospvSizeOfCallId == 0) ||
        (ospvCallId == OSPC_OSNULL) ||
        (ospvSizeOfToken == 0) ||
        ((ospvCallingNumberFormat < OSPC_NFORMAT_START) || (ospvCallingNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvCalledNumberFormat < OSPC_NFORMAT_START) || (ospvCalledNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        (ospvToken == OSPC_OSNULL)) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "invalid input for Initialize");
    }

    /* Get transaction context */
    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    /* call validate */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPTransactionValidateAuthorisation(ospvTransaction,
            ospvSource,
            ospvDestination,
            ospvSourceDevice,
            ospvDestinationDevice,
            ospvCallingNumber, ospvCallingNumberFormat, ospvCalledNumber, ospvCalledNumberFormat,
            ospvSizeOfCallId, ospvCallId,
            ospvSizeOfToken, ospvToken,
            ospvAuthorised, ospvTimeLimit, ospvSizeOfDetailLog, ospvDetailLog, ospvTokenAlgo);
    }

    /*
     * move data into authreq & authrsp, (OGW) struct so
     * report usage will work.
     * We are adding a destination to authrsp->dest, setting current dest to
     * point to it.
     */
    *ospvAuthorised = OSPC_TRAN_AUTHORISED;

    if ((errcode == OSPC_ERR_NO_ERROR) && (*ospvAuthorised == OSPC_TRAN_AUTHORISED)) {
        if (ospvRole == OSPC_ROLE_SOURCE) {
            if (trans->AuthReq != OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                OSPM_DBGERRORLOG(errcode, "Transaction already initialized");
            } else {
                /* create callid structure */
                callid = OSPPCallIdNew(ospvSizeOfCallId, (const unsigned char *)ospvCallId);

                if (callid == OSPC_OSNULL) {
                    errcode = OSPC_ERR_DATA_NOCALLID;
                }

                if (errcode == OSPC_ERR_NO_ERROR) {
                    /* we need to build authreq and authrsp */
                    errcode = OSPPTransactionRequestNew(trans, ospvSource,
                        ospvSourceDevice,
                        ospvCallingNumber,
                        ospvCalledNumber,
                        OSPC_OSNULL,
                        numcallids, &callid,
                        OSPC_OSNULL, &numcallids, ospvSizeOfDetailLog, ospvDetailLog);
                }

                /* delete callid - TransactionRequestNew created new one */
                OSPPCallIdDelete(&callid);
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                errcode = OSPPTransactionResponseBuild(trans,
                    ospvDestination, ospvCallingNumber,
                    ospvSizeOfCallId, ospvCallId, ospvSizeOfToken, ospvToken);
            }

            /* Set correct role */
            OSPPAuthIndSetRole(trans->AuthInd, ospvRole);
        } else if (ospvRole == OSPC_ROLE_DESTINATION) {
            /* authind  already built by validate, just make sure role is correct */
            OSPPAuthIndSetRole(trans->AuthInd, ospvRole);
        } else {
            errcode = OSPC_ERR_TRAN_NOT_IMPLEMENTED;
            OSPM_DBGERRORLOG(errcode, "Invalid system type.");
        }
    } else {
        if (errcode == OSPC_ERR_NO_ERROR) {
            /* no error from validate, but token not authorised */
            errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
            OSPM_DBGERRORLOG(errcode, "Token invalid");
        }
    }

    /* Set transaction state */
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionSetState(trans, OSPC_INITIALIZE_SUCCESS);
    } else {
        OSPPTransactionSetState(trans, OSPC_INITIALIZE_FAIL);
    }

    return errcode;
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
 *      the call authorization functions from call setup and usage reporting
 *      functions.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionNew(
    OSPTPROVHANDLE ospvProvider,    /* In - Provider Handle */
    OSPTTRANHANDLE *ospvTransaction)/* Out - Transaction Handle */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPE_OPERATOR_NAME opname;
    OSPE_PROTOCOL_TYPE prot;
    OSPE_SERVICE svc;
    OSPE_CODEC_TYPE codec;
    OSPE_SESSION_ID sess;
    int index;
    OSPE_NUMBER_FORMAT format;

    errcode = OSPPTransactionGetNewContext(ospvProvider, ospvTransaction);
    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(*ospvTransaction, &errcode);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        trans->Provider = OSPPProviderGetContext(ospvProvider, &errcode);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionSetState(trans, OSPC_TRANSNEW);
        trans->HasGetDestSucceeded = OSPC_FALSE;
        trans->HasServiceInfo = OSPC_FALSE;
        trans->HasPricingInfo = OSPC_FALSE;
        trans->NumOfPricingInfoElements = 0;
        trans->CurrentPricingInfoElement = 0;
        trans->WasLookAheadInfoGivenToApp = OSPC_FALSE;
        trans->TokenInfoHasLookAheadInfo = OSPC_FALSE;
        trans->SrcNetworkId = OSPC_OSNULL;
        trans->DestNetworkId = OSPC_OSNULL;
        trans->NPRn[0] = '\0';
        trans->NPCic[0] = '\0';
        trans->NPNpdi = OSPC_FALSE;
        for (opname = OSPC_OPNAME_START; opname < OSPC_OPNAME_NUMBER; opname++) {
            trans->OpName[opname][0] = '\0';
        }
        for (prot = OSPC_PROTTYPE_START; prot < OSPC_PROTTYPE_NUMBER; prot++) {
            trans->Protocol[prot] = OSPC_PROTNAME_UNKNOWN;
        }
        for (svc = OSPC_SERVICE_START; svc < OSPC_SERVICE_NUMBER; svc++) {
            for (codec = OSPC_CODEC_START; codec < OSPC_CODEC_NUMBER; codec++) {
                trans->Codec[svc][codec][0] = '\0';
            }
        }
        for (sess = OSPC_SESSIONID_START; sess < OSPC_SESSIONID_NUMBER; sess++) {
            trans->SessionId[sess] = OSPC_OSNULL;
        }
        for (index = 0; index < OSPC_MAX_INDEX; index++) {
            trans->CustomInfo[index] = OSPC_OSNULL;
        }
        trans->UsageSrcNetworkId[0] = '\0';
        trans->SrcRealm[0] = '\0';
        trans->DestRealm[0] = '\0';
        for (format = OSPC_NFORMAT_START; format < OSPC_NFORMAT_NUMBER; format++) {
            trans->From[format][0] = '\0';
            trans->AssertedId[format][0] = '\0';
            trans->RemotePartyId[format][0] = '\0';
            trans->ChargeInfo[format][0] = '\0';
            trans->DivSrcInfo[format][0] = '\0';
        }
        trans->DivDevInfo[0] = '\0';
        trans->ApplicationId[0] = '\0';
        trans->RoleState = OSPC_RSTATE_UNKNOWN;
        trans->RoleFormat = OSPC_RFORMAT_UNKNOWN;
        trans->RoleVendor = OSPC_RVENDOR_UNKNOWN;
        trans->NetworkTranslatedCalledFormat = OSPC_NFORMAT_E164;
        trans->NetworkTranslatedCalled[0] = '\0';
        trans->ServiceProvider[0] = '\0';
        trans->SystemId[0] = '\0';
        trans->RelatedReason[0] = '\0';
        trans->TotalSetupAttempts = -1;
        trans->CDRProxyHost[0] = '\0';
        trans->CDRProxyFolder[0] = '\0';
        trans->CDRProxySubfolder[0] = '\0';
        trans->UserAgent[0] = '\0';
        trans->SrcAudioAddr[0] = '\0';
        trans->SrcVideoAddr[0] = '\0';
        trans->ProxyIngressAddr[0] = '\0';
        trans->JIP[0] = '\0';
        trans->CallingParty.UserName[0] = '\0';
        trans->CallingParty.UserId[0] = '\0';
        trans->CallingParty.UserGroup[0] = '\0';
    }

    return errcode;
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
int OSPPTransactionRecordFailure(
    OSPTTRANHANDLE ospvTransaction,
    OSPEFAILREASON ospvFailureReason)
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
        /*
         * The failurereason should be either 0 or a positive value. This is the only
         * restriction we have now.
         */
        if (ospvFailureReason < OSPC_FAIL_NONE) {
            errcode = OSPC_ERR_FAILRSN_INVALID;
            OSPM_DBGERRORLOG(errcode, "Failure code invalid");
        }

        if (errcode == OSPC_ERR_NO_ERROR) {

            /* Set failure reason for current destination */
            OSPPDestSetTermCause(trans->CurrentDest, OSPC_TCAUSE_Q850, ospvFailureReason, OSPC_OSNULL);
        }
    } else if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
        OSPM_DBGERRORLOG(errcode, "Transaction pointer is NULL.");

    }

    return errcode;
}

/*
 * OSPPTransactionReinitializeAtDevice
 * The OSPPTransactionReinitializeAtDevice function re-initializes a (previously
 * initialized) transaction object. Applications can use this with a distributed
 * architecture in which the systems requesting and validating authorization
 * (e.g. H.323 gatekeepers) are different than the systems that ultimately
 * report usage information (e.g. H.323 gateways). The reporting device can call
 * this function after failing to reach a previous destination. As such, this
 * function is an alternative to the OSPPTransactionGetNextDestination function.
 * Parameters to the function are:
 *   ospvTransaction: handle of the (previously created) transaction object.
 *   ospvFailureReason: the reason that attempts to use the previously
 *       identified destination failed; values for this parameter are listed in
 *       the ospfail.h file.
 * ospvRole: indicates whether the system calling this function is acting as
 *       the source or destination for the call.
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
 * ospvSourceDevice: character string identifying the source device.
 *      This could be the previous hop Gateway.
 *      It is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                  "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 * ospvDestinationDevice: character string identifying the destination device.
 *       The value is expressed as either a DNS name or an IP address enclosed
 *       in square brackets, followed by an optional colon and TCP port number.
 *       Examples of valid destinations include "gateway1.carrier.com" and
 *           "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 * ospvCallingNumber: character string containing the calling party's number
 *       expressed as a full international number conforming to the ITU E.164
 *       standard (with no punctuation).
 * ospvCalledNumber: character string containing the called number, expressed
 *       as a full international number conforming to the ITU E.164 standard
 *       (with no punctuation).
 * ospvSizeOfCallId: size of the memory buffer containing the call identifier.
 * ospvCallId: memory location containing the H.323 call identifier for the
 *       call.
 * ospvSizeOfToken: size of the memory buffer containing an authorization token
 *       for the call.
 * ospvToken: memory location containing an authorization token.
 * ospvAuthorised: pointer to a variable in which the function will indicate
 *       whether or not the call is authorised. On return, a non-zero value
 *       indicates that the call is authorised by the provider, while a zero
 *       value indicates an authorization failure.
 * ospvTimeLimit: pointer to a variable in which to place the number of seconds
 *       for which the call is initially authorised. A value of zero indicates
 *       that no limit exists. Note that the initial time limit may be extended
 *       during the call by using the function
 *       OSPPTransactionRequestReauthorisation.
 * ospvSizeOfDetailLog: pointer to a variable which, on input, contains the
 *       maximum size of the detail log; on output, the variable will be updated
 *       with the actual size of the detail log. By setting this value to zero,
 *       applications indicate that they do not wish a detail log for the
 *       authorization validation.
 * ospvDetailLog: pointer to a location in which to store a detail log for the
 *       validation. If this pointer is not NULL, and if the ospvSizeOfDetailLog
 *       parameter is non-zero, then the library will store a copy of the
 *       authorization confirmation obtained from the settlement provider,
 *       including the settlement provider's digital signature.
 *  ospvTokenAlgo: This can take either of the 3 values - TOKEN_ALGO_SIGNED,
 *      TOKEN_ALGO_UNSIGNED, and TOKEN_ALGO_BOTH. If the value is set to
 *      TOKEN_ALGO_SIGNED the toolkit expects the token to be signed, and
 *      validates the signature as a part of token validation. If the value
 *      is set to TOKEN_ALGO_UNSIGNED, an unsigned token is expected. If the
 *      value is set to TOKEN_ALGO_BOTH, then the toolkit accepts either a
 *      signed token or an unsigned token.
 * If the provider has been configured to perform local validation, the SDK
 * library is able to perform this function without network interaction, and,
 * therefore, does not block for network input or output during its execution.
 * If local validation is not used, this function blocks until authorization has
 * been validated, refused, or an error has been detected. The Open Settlement
 * Protocol SDK Porting Guide includes information on modifying that behavior to
 * prevent blocking.
 * The function returns an error code or zero (if the operation was successful).
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionReinitializeAtDevice(
    OSPTTRANHANDLE ospvTransaction,
    OSPEFAILREASON ospvFailureReason,
    unsigned ospvRole,
    const char *ospvSource,
    const char *ospvDestination,
    const char *ospvSourceDevice,
    const char *ospvDestinationDevice,
    const char *ospvCallingNumber,
    const char *ospvCalledNumber,
    unsigned ospvSizeOfCallId,
    const void *ospvCallId,
    unsigned ospvSizeOfToken,
    const void *ospvToken,
    unsigned *ospvAuthorised,
    unsigned *ospvTimeLimit, unsigned *ospvSizeOfDetailLog, void *ospvDetailLog, unsigned ospvTokenAlgo)
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    /* verify input */
    if (((ospvSource == OSPC_OSNULL) &&
         (ospvSourceDevice == OSPC_OSNULL)) ||
        ((ospvDestination == OSPC_OSNULL) &&
         (ospvDestinationDevice == OSPC_OSNULL)) ||
        (ospvCallingNumber == OSPC_OSNULL) ||
        (ospvCalledNumber == OSPC_OSNULL) ||
        (ospvSizeOfCallId == 0) ||
        (ospvCallId == OSPC_OSNULL) || (ospvSizeOfToken == 0) || (ospvToken == OSPC_OSNULL)) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for ReInitialize");
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);

        /*
         * In ver2.9.3, we have removed the check to not accept 0 as a TC Code.
         * The toolkit now accepts 0.
         */

        /* Set failure reason for current destination, validate token and add new
           destination. only on OGW
         */

        /* Now check for acceptable failure code */
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = OSPPFailReasonFind(ospvFailureReason);
        }

        /* call validate */
        if (errcode == OSPC_ERR_NO_ERROR) {
            /*
             * The toolkit expects the calling and called numbers to be in
             * the same format as the format that was used while callinf
             * InitializeAtDevice API
             * The application should make sure of this !
             */
            errcode = OSPPTransactionValidateAuthorisation(ospvTransaction,
                ospvSource,
                ospvDestination,
                ospvSourceDevice,
                ospvDestinationDevice,
                ospvCallingNumber, trans->CallingNumberFormat, ospvCalledNumber,
                trans->CalledNumberFormat,
                ospvSizeOfCallId, ospvCallId,
                ospvSizeOfToken, ospvToken,
                ospvAuthorised, ospvTimeLimit, ospvSizeOfDetailLog, ospvDetailLog, ospvTokenAlgo);
        }

        if ((errcode == OSPC_ERR_NO_ERROR) && (*ospvAuthorised == OSPC_TRAN_AUTHORISED)) {
            /* should only be called by OGW */
            if (ospvRole == OSPC_ROLE_SOURCE) {
                /* we are only adding a destination */
                /* first set failure code in authrsp->currentdest */
                OSPPDestSetTermCause(trans->CurrentDest, OSPC_TCAUSE_Q850, ospvFailureReason, OSPC_OSNULL);
                /* now build new dest */
                errcode = OSPPTransactionResponseBuild(trans,
                    ospvDestination, ospvCallingNumber, ospvSizeOfCallId,
                    ospvCallId, ospvSizeOfToken, ospvToken);
            } else {
                errcode = OSPC_ERR_TRAN_NOT_IMPLEMENTED;
                OSPM_DBGERRORLOG(errcode, "Invalid system type.");
            }
        } else {
            if (errcode == OSPC_ERR_NO_ERROR) {
                /* no error from validate, but token not authorised */
                errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                OSPM_DBGERRORLOG(errcode, "Token invalid");
            }
        }
    }   /* end else (parameters are correct) */

    /* Set transaction state */
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionSetState(trans, OSPC_REINITIALIZE_SUCCESS);
    } else {
        OSPPTransactionSetState(trans, OSPC_REINITIALIZE_FAIL);
    }

    return errcode;
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
int OSPPTransactionReportUsage(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    unsigned ospvDuration,              /* In - Length of call */
    OSPTTIME ospvStartTime,             /* In - Call start time */
    OSPTTIME ospvEndTime,               /* In - Call end time */
    OSPTTIME ospvAlertTime,             /* In - Call alert time */
    OSPTTIME ospvConnectTime,           /* In - Call connect time */
    OSPTBOOL ospvHasPDDInfo,            /* In - Is PDD Info present */
    unsigned ospvPostDialDelay,         /* In - Post Dial Delay, in milliseconds */
    OSPE_RELEASE ospvReleaseSource,     /* In - EP that released the call */
    const char *ospvConferenceId,       /* In - conference Id. Max 100 char long */
    int ospvLossPacketsSent,            /* In - Packets not received by peer */
    int ospvLossFractionSent,           /* In - Fraction of packets not received by peer */
    int ospvLossPacketsReceived,        /* In - Packets not received that were expected */
    int ospvLossFractionReceived,       /* In - Fraction of packets expected but not received */
    unsigned *ospvSizeOfDetailLog,      /* In/Out - Max size of detail log \ Actual size of detail log */
    void *ospvDetailLog)                /* Out - Pointer to detail log storage */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_USAGE_IND *usage = OSPC_OSNULL;
    OSPE_MESSAGE datatype = OSPC_MSG_UNKNOWN;
    unsigned char *xmldoc = OSPC_OSNULL;
    unsigned sizeofxmldoc = 0;
    OSPT_MSG_INFO *msginfo = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPTBOOL usageallowed = OSPC_FALSE;
    OSPT_STATS stats;
    OSPE_PROTOCOL_NAME protocol;
    OSPE_SERVICE svc;
    OSPE_CODEC_TYPE codec;
    OSPE_SESSION_ID sess;
    OSPE_PROTOCOL_TYPE prot;
    unsigned destcount;
    int attempt;

    OSPM_ARGUSED(ospvSizeOfDetailLog);
    OSPM_ARGUSED(ospvDetailLog);

    OSPM_MEMSET(&stats, 0, sizeof(OSPT_STATS));

    /* Verify Input */
    if (ospvTransaction == OSPC_TRAN_HANDLE_INVALID) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "transaction handle invalid");
    }

    /* Get transaction context */
    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if (trans == OSPC_OSNULL) {
            errcode = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
            OSPM_DBGERRORLOG(errcode, "transaction context not found");
        }
    }

    /* Have we already done this? */
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionGetReportUsageAllowed(trans, &usageallowed);
        if (!usageallowed) {
            errcode = OSPC_ERR_TRAN_USAGE_ALREADY_REPORTED;
            OSPM_DBGERRORLOG(errcode, "Usage has already been reported.");
        }
    }

    /* Set up Statistics */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPStatsReportUsage(&(trans->Statistics),
            ospvLossPacketsSent, ospvLossFractionSent, ospvLossPacketsReceived, ospvLossFractionReceived);
    }

    /* Set up Usage indication list using current dest list (originating) or
     * authind (terminating) and current data */

    /* Are we originating or terminating */
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* Warning! Do not change the order of these checks without
         * looking at OSPPTransactionInitializeAtDevice first
         */
        if (trans->AuthRsp != OSPC_OSNULL) {
            /* originating */
            /* determines where most info will come from */
            datatype = OSPC_MSG_ARESP;
            if (OSPPAuthRspHasDest(trans->AuthRsp)) {
                OSPPListNew(&(trans->UsageInd));

                /* Loop through all dests from first to current and create usages for each */
                for ((dest = (OSPT_DEST *)OSPPListFirst(&(trans->AuthRsp->Destination)));
                     (dest != trans->CurrentDest) &&
                     (errcode == OSPC_ERR_NO_ERROR) &&
                     (dest != OSPC_OSNULL); (dest = (OSPT_DEST *)OSPPListNext(&(trans->AuthRsp->Destination), dest)))
                {
                    /* All dests up to current (if any) must have failreasons. */
                    if (!OSPPDestHasTermCauseAny(dest)) {
                        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                        OSPM_DBGERRORLOG(errcode, "No failure code found.");
                        break;
                    } else {
                        errcode = OSPPTransactionBuildUsage(trans, &usage, dest, datatype);
                    }

                    if (errcode == OSPC_ERR_NO_ERROR) {
                        /* Add destination count and setup attempt */
                        destcount = OSPPDestGetDestinationCount(dest);
                        attempt = OSPPDestGetSetupAttempt(dest);
                        if (attempt >= 0) {
                            OSPPUsageIndSetDestinationCount(usage, attempt);
                        } else if (destcount > 0) {
                            OSPPUsageIndSetDestinationCount(usage, destcount);
                        }

                        /* Add conference Id */
                        if (ospvConferenceId && (ospvConferenceId[0] != '\0') && (OSPM_STRLEN(ospvConferenceId) < OSPC_SIZE_CONFID)) {
                            OSPPUsageIndSetConferenceId(usage, ospvConferenceId);
                        }

                        /* Add source network ID */
                        if (trans->UsageSrcNetworkId[0] != '\0') {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->UsageSrcNetworkId),
                                trans->UsageSrcNetworkId,
                                OSPC_ALTINFO_NETWORK);
                            OSPPUsageIndAddSourceAlt(usage, altinfo);
                            altinfo = OSPC_OSNULL;
                        }

                        /* Add local ingress address */
                        if (trans->ProxyIngressAddr[0] != '\0') {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->ProxyIngressAddr),
                                trans->ProxyIngressAddr,
                                OSPC_ALTINFO_INGRESS);
                            OSPPUsageIndAddSourceAlt(usage, altinfo);
                            altinfo = OSPC_OSNULL;
                        }

                        /* Add local egress address */
                        if (dest->ProxyEgressAddr[0] != '\0') {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(dest->ProxyEgressAddr),
                                dest->ProxyEgressAddr,
                                OSPC_ALTINFO_EGRESS);
                            OSPPUsageIndAddSourceAlt(usage, altinfo);
                            altinfo = OSPC_OSNULL;
                        }

                        OSPPListAppend(&(trans->UsageInd), usage);
                        usage = OSPC_OSNULL;
                    }
                }

                if (errcode == OSPC_ERR_NO_ERROR) {
                    if (dest != OSPC_OSNULL) {
                        /* We are at current, either set failure code or build a usage detail */
                        errcode = OSPPTransactionBuildUsage(trans, &usage, dest, datatype);
                        if (errcode == OSPC_ERR_NO_ERROR) {
                            if (OSPPDestHasTermCauseAny(dest)) {
                                /* Set failure reason */
                                OSPPUsageIndCopyTermCause(usage, OSPPDestGetTermCause(dest));
                            }

                            destcount = OSPPDestGetDestinationCount(dest);
                            attempt = OSPPDestGetSetupAttempt(dest);
                            if (attempt >= 0) {
                                OSPPUsageIndSetDestinationCount(usage, attempt);
                            } else if (destcount > 0) {
                                OSPPUsageIndSetDestinationCount(usage, destcount);
                            }

                            /* Set Duration */
                            OSPPUsageIndSetDuration(usage, (int)ospvDuration);
                            OSPPUsageIndSetStartTime(usage, ospvStartTime);
                            OSPPUsageIndSetEndTime(usage, ospvEndTime);
                            OSPPUsageIndSetAlertTime(usage, ospvAlertTime);
                            OSPPUsageIndSetConnectTime(usage, ospvConnectTime);
                            if (ospvHasPDDInfo) {
                                OSPPUsageIndSetPostDialDelay(usage, ospvPostDialDelay);
                            }
                            OSPPUsageIndSetReleaseSource(usage, ospvReleaseSource);

                            /* Add Conference Id */
                            if ((ospvConferenceId) && (ospvConferenceId[0] != '\0') &&
                                (OSPM_STRLEN(ospvConferenceId) < OSPC_SIZE_CONFID)) {
                                OSPPUsageIndSetConferenceId(usage, ospvConferenceId);
                            }

                            /* Get Stats */
                            if (OSPPTransactionHasStatistics(trans)) {
                                OSPPTransactionGetStatistics(trans, &stats);
                                OSPPUsageIndSetStatistics(usage, &stats);
                            }

                            /* Add codec */
                            for (svc = OSPC_SERVICE_START; svc < OSPC_SERVICE_NUMBER; svc++) {
                                for (codec = OSPC_CODEC_START; codec < OSPC_CODEC_NUMBER; codec++) {
                                    if (trans->Codec[svc][codec][0] != '\0') {
                                        OSPPUsageIndSetCodec(usage, svc, codec, trans->Codec[svc][codec]);
                                    }
                                }
                            }

                            /* Add session IDs */
                            for (sess = OSPC_SESSIONID_START; sess < OSPC_SESSIONID_NUMBER; sess++) {
                                if (trans->SessionId[sess] != OSPC_OSNULL) {
                                    OSPPUsageIndSetSessionId(usage, sess, trans->SessionId[sess]);
                                }
                            }

                            /* Add source network ID */
                            if (trans->UsageSrcNetworkId[0] != '\0') {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->UsageSrcNetworkId),
                                    trans->UsageSrcNetworkId,
                                    OSPC_ALTINFO_NETWORK);
                                OSPPUsageIndAddSourceAlt(usage, altinfo);
                                altinfo = OSPC_OSNULL;
                            }

                            /* Add local ingress address */
                            if (trans->ProxyIngressAddr[0] != '\0') {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->ProxyIngressAddr),
                                    trans->ProxyIngressAddr,
                                    OSPC_ALTINFO_INGRESS);
                                OSPPUsageIndAddSourceAlt(usage, altinfo);
                                altinfo = OSPC_OSNULL;
                            }

                            /* Add local egress address */
                            if (dest->ProxyEgressAddr[0] != '\0') {
                                altinfo = OSPPAltInfoNew(OSPM_STRLEN(dest->ProxyEgressAddr),
                                    dest->ProxyEgressAddr,
                                    OSPC_ALTINFO_EGRESS);
                                OSPPUsageIndAddSourceAlt(usage, altinfo);
                                altinfo = OSPC_OSNULL;
                            }

                            OSPPListAppend(&(trans->UsageInd), usage);
                            usage = OSPC_OSNULL;
                        }
                    } else {
                        errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                        OSPM_DBGERRORLOG(errcode, "No current destination found.");
                    }
                }
            } else {
                /* Dest not found */
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "No destinations found.");
            }
        } else if (trans->AuthInd != OSPC_OSNULL) {
            datatype = OSPC_MSG_AIND;

            errcode = OSPPTransactionBuildUsage(trans, &usage, trans->CurrentDest, datatype);

            if (errcode == OSPC_ERR_NO_ERROR) {
                if (OSPPDestHasTermCauseAny(trans->CurrentDest)) {
                    /* Set failure reason */
                    OSPPUsageIndCopyTermCause(usage, OSPPDestGetTermCause(trans->CurrentDest));
                }
            }

            /* Move protocols to the usage structure */
            if (errcode == OSPC_ERR_NO_ERROR) {
                for (prot = OSPC_PROTTYPE_START; prot < OSPC_PROTTYPE_NUMBER; prot++) {
                    if (prot == OSPC_PROTTYPE_DESTINATION) {
                        protocol = trans->CurrentDest->Protocol;
                    } else {
                        protocol = trans->Protocol[prot];
                    }
                    if ((protocol >= OSPC_PROTNAME_START) && (protocol < OSPC_PROTNAME_NUMBER)) {
                        OSPPUsageIndSetProtocol(usage, prot, protocol);
                    } else {
                        OSPPUsageIndSetProtocol(usage, prot, OSPC_PROTNAME_UNKNOWN);
                    }
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {
                OSPPListNew(&(trans->UsageInd));
                OSPPUsageIndSetDuration(usage, (int)ospvDuration);
                OSPPUsageIndSetStartTime(usage, ospvStartTime);
                OSPPUsageIndSetEndTime(usage, ospvEndTime);
                OSPPUsageIndSetAlertTime(usage, ospvAlertTime);
                OSPPUsageIndSetConnectTime(usage, ospvConnectTime);
                if (ospvHasPDDInfo) {
                    OSPPUsageIndSetPostDialDelay(usage, ospvPostDialDelay);
                }
                OSPPUsageIndSetReleaseSource(usage, ospvReleaseSource);
                if ((ospvConferenceId) && (ospvConferenceId[0] != '\0') && (OSPM_STRLEN(ospvConferenceId) < OSPC_SIZE_CONFID)) {
                    OSPPUsageIndSetConferenceId(usage, ospvConferenceId);
                }
                if (trans->UsageSrcNetworkId[0] != '\0') {
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->UsageSrcNetworkId), trans->UsageSrcNetworkId, OSPC_ALTINFO_NETWORK);
                    OSPPUsageIndAddSourceAlt(usage, altinfo);
                    altinfo = OSPC_OSNULL;
                }
                OSPPListAppend(&(trans->UsageInd), usage);
                usage = OSPC_OSNULL;
            }
        } else {
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
        }
    }

    /* Create appropriate xml message */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPXMLMessageCreate(OSPC_MSG_UIND, &xmldoc, &sizeofxmldoc, &(trans->UsageInd), trans);
    }

    /* Check for audit. If it is turned on add xmldoc to audit storage. */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (trans->Provider->Comm->Flags & OSPC_COMM_AUDIT_ON) {
            errcode = OSPPAuditAddMessageToBuffer(trans->Provider->Audit, xmldoc, sizeofxmldoc);
            if (errcode != OSPC_ERR_NO_ERROR) {
                OSPM_DBGERRORLOG(errcode, "OSPPAuditAddMessageToBuffer failed.");
                errcode = OSPC_ERR_NO_ERROR;
            }
        }
    }

    /* Create msginfo, put in queue and process return */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgInfoNew(&msginfo);

        if (errcode == OSPC_ERR_NO_ERROR) {
            /* Set transaction state */
            OSPPTransactionSetState(trans, OSPC_REPORT_USAGE_BLOCK);

            errcode = OSPPTransactionPrepareAndQueMessage(trans, xmldoc, sizeofxmldoc, &msginfo);

            if (errcode == OSPC_ERR_NO_ERROR) {
                errcode = OSPPTransactionProcessReturn(trans, msginfo);
            }

            OSPPMsgInfoDelete(&msginfo);
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* Set transaction state */
        OSPPTransactionSetState(trans, OSPC_REPORT_USAGE_SUCCESS);
    } else {
        if (trans != OSPC_OSNULL) {
            /* Set transaction state */
            OSPPTransactionSetState(trans, OSPC_REPORT_USAGE_FAIL);

            if (trans->UsageInd != OSPC_OSNULL) {
                OSPPTransactionDeleteUsageInd(trans);
            }
        }
    }

    if (usage != OSPC_OSNULL) {
        OSPPUsageIndDelete(&usage);
    }

    if (xmldoc != OSPC_OSNULL) {
        OSPM_FREE(xmldoc);
        xmldoc = OSPC_OSNULL;
    }

    return errcode;
}

/*
 * The OSPPTransactionRequestAuthorisation function allows an application to
 * request authorization and, optionally, routing information for a transaction
 * The parameters to the function are:
 *  ospvTransaction: handle of the (previously created) transaction object.
 *  ospvSource: character string identifying the source of the call. The value
 *      is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                      "[172.16.1.2]:112".
 * ospvSourceDevice: character string identifying the source device.
 *      This could be the previous hop Gateway.
 *      It is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                  "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 *  ospvCallingNumber: character string containing the calling party's number
 *      expressed as a full international number conforming to the ITU E.164
 *      standard (with no punctuation); if the actual calling party number is
 *      unavailable (e.g. because the end user has blocked caller ID services),
 *      then the application should supply a local phone number for the device
 *      originating the call.
 *  ospvCallingNumberFormat: Value that identifies the type of calling number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvCalledNumber: character string containing the called number, expressed
 *      as a full international number conforming to the ITU E.164 standard
 *      (with no punctuation).
 *  ospvCalledNumberFormat: Value that identifies the type of called number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvUser: character string identifying the end user (e.g. calling card and
 *      PIN number assigned to roaming users); this string may be empty.
 *  ospvNumberOfCallIds: the number of call identifiers in the ospvCallIds list
 *  ospvCallIds: an array of H.323 call identifiers for the call. The
 *      OSPT_CALL_ID type consists of a length indicator and a pointer to the
 *      binary data. Applications may provide a list of call identifiers in
 *      anticipation of the authorization request returning multiple potential
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
 *      authorization request.
 *  ospvDetailLog: pointer to a location in which to store a detail log for the
 *      authorization request. If this pointer is not NULL, and if the
 *      ospvSizeOfDetailLog parameter is non-zero, then the library will store
 *      a copy of the authorization response obtained from the settlement
 *      provider, including the settlement provider's digital signature.
 * As delivered in the SDK library, this function blocks until authorization
 * has been received or an error has been detected. The Open Settlement
 * Protocol SDK Porting Guide includes information on modifying that behavior
 * to prevent blocking.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionRequestAuthorisation(
    OSPTTRANHANDLE ospvTransaction,             /* In - Transaction handle */
    const char *ospvSource,                     /* In - Source of call */
    const char *ospvSourceDevice,               /* In - SourceDevice of call */
    const char *ospvCallingNumber,              /* In - Calling number */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat, /* In - Calling number format : sip/e.164/url */
    const char *ospvCalledNumber,               /* In - Called number */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat,  /* In - Called number format : sip/e.164/url */
    const char *ospvUser,                       /* In - End user (optional) */
    unsigned ospvNumberOfCallIds,               /* In - Number of call identifiers */
    OSPT_CALL_ID *ospvCallIds[],                /* In - List of call identifiers */
    const char *ospvPreferredDestinations[],    /* In - List of preferred destinations for call */
    unsigned *ospvNumberOfDestinations,         /* In/Out - Max number of destinations Actual number of dests authorised */
    unsigned *ospvSizeOfDetailLog,              /* In/Out - Max size of detail log Actual size of detail log */
    void *ospvDetailLog)                        /* In/Out - Location of detail log storage */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_MSG_INFO *msginfo = OSPC_OSNULL;
    unsigned char *xmldoc = OSPC_OSNULL;
    unsigned sizeofxmldoc = 0, delay = OSPC_TNPROBE_MAXWAIT;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPT_TN_PROBE *probelist = OSPC_OSNULL, *tmpprobe = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;
    unsigned probecnt = 0;
    OSPTSVCPT *svcpt = OSPC_OSNULL;
    unsigned tmpNumberOfCallIds = 0;
    OSPT_CALL_ID **tmpCallIds = OSPC_OSNULL;
    static OSPT_CALL_ID undefinedCallId = { {OSPC_OSNULL}, OSPC_UNDEFINED_CALLID_SIZE, OSPC_UNDEFINED_CALLID_STR };
    static OSPT_CALL_ID *undefinedCallIds[OSPC_UNDEFINED_CALLID_NUM] = { &undefinedCallId };

    if ((ospvCallingNumber == OSPC_OSNULL) ||
        (ospvCalledNumber == OSPC_OSNULL) ||
        ((ospvNumberOfCallIds != 0) && ((ospvCallIds == OSPC_OSNULL) || (ospvCallIds[0] == OSPC_OSNULL))) ||
        ((ospvCallingNumberFormat < OSPC_NFORMAT_START) || (ospvCallingNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvCalledNumberFormat < OSPC_NFORMAT_START) || (ospvCalledNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        (ospvNumberOfDestinations == 0))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for RequestAuthorisation");
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        if (ospvNumberOfCallIds == 0) {
            tmpNumberOfCallIds = OSPC_UNDEFINED_CALLID_NUM;
            tmpCallIds = undefinedCallIds;
        } else {
            tmpNumberOfCallIds = ospvNumberOfCallIds;
            tmpCallIds = ospvCallIds;

/* Relax the number of call ids check for Emergent's two call ids solution */
#if 0
            if ((ospvNumberOfCallIds > 1) && (ospvNumberOfCallIds != *ospvNumberOfDestinations)) {
                errcode = OSPC_ERR_TRAN_CALLID_DEST_MISMATCH;
                OSPM_DBGERRORLOG(errcode, "number of callids != number of dests sent in.");
            }
#endif
        }
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);

        /* Check Callids */
        if (errcode == OSPC_ERR_NO_ERROR) {

            /* if there is an authreq here already, we have an error */
            if (trans->AuthReq == OSPC_OSNULL) {

                /* Build Auth request */
                errcode = OSPPTransactionRequestNew(trans,
                    ospvSource, ospvSourceDevice, ospvCallingNumber,
                    ospvCalledNumber, ospvUser, tmpNumberOfCallIds,
                    tmpCallIds, ospvPreferredDestinations,
                    ospvNumberOfDestinations, ospvSizeOfDetailLog, ospvDetailLog);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    trans->CallingNumberFormat = ospvCallingNumberFormat;
                    trans->CalledNumberFormat = ospvCalledNumberFormat;

                    errcode = OSPPXMLMessageCreate(OSPC_MSG_AREQ, &xmldoc, &sizeofxmldoc, trans->AuthReq, trans);
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        if ((errcode = OSPPMsgInfoNew(&msginfo)) == OSPC_ERR_NO_ERROR) {
                            /* Set transaction state */
                            OSPPTransactionSetState(trans, OSPC_AUTH_REQUEST_BLOCK);

                            errcode = OSPPTransactionPrepareAndQueMessage(trans, xmldoc, sizeofxmldoc, &msginfo);
                            if (errcode == OSPC_ERR_NO_ERROR) {
                                errcode = OSPPTransactionProcessReturn(trans, msginfo);
                                if ((errcode == OSPC_ERR_NO_ERROR) && (trans->AuthRsp != OSPC_OSNULL)) {
                                    /* Set num destinations */
                                    *ospvNumberOfDestinations = OSPPAuthRspGetNumDests(trans->AuthRsp);

                                    /* Check status code */
                                    if (trans->AuthRsp->Status->Code > 299) {
                                        errcode = OSPPUtilGetErrorFromStatus(trans->AuthRsp->Status->Code);
                                    }

                                    if ((*ospvNumberOfDestinations > 0) && (errcode == OSPC_ERR_NO_ERROR)) {
                                        /* Do we need to run probe? */
                                        if ((*ospvNumberOfDestinations > 1) &&
                                            (OSPPAuthRspHasDelayLimit(trans->AuthRsp) || OSPPAuthRspHasDelayPref(trans->AuthRsp))) {

                                            /* build probe list */
                                            OSPM_MALLOC(probelist, OSPT_TN_PROBE, sizeof(OSPT_TN_PROBE) * *ospvNumberOfDestinations);
                                            if (probelist == OSPC_OSNULL) {
                                                errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
                                            }

                                            if (errcode == OSPC_ERR_NO_ERROR) {
                                                OSPM_MEMSET(probelist, 0, sizeof(OSPT_TN_PROBE) * *ospvNumberOfDestinations);

                                                tmpprobe = probelist;
                                                for (probecnt = 0, dest = OSPPAuthRspFirstDest(trans->AuthRsp);
                                                     probecnt < *ospvNumberOfDestinations;
                                                     dest = OSPPAuthRspNextDest(trans->AuthRsp, dest))
                                                {
                                                    if (dest != OSPC_OSNULL) {
                                                        errcode = OSPPCommParseSvcPt(OSPPDestGetAddr(dest), &svcpt, 0);
                                                    }

                                                    if (errcode == OSPC_ERR_NO_ERROR) {
                                                        if (svcpt != OSPC_OSNULL) {
                                                            tmpprobe[probecnt].IpAddr = svcpt->IpAddr;

                                                            if (svcpt->HostName) {
                                                                OSPM_FREE(svcpt->HostName);
                                                            }

                                                            if (svcpt->URI) {
                                                                OSPM_FREE(svcpt->URI);
                                                            }

                                                            OSPM_FREE(svcpt);

                                                            probecnt++;
                                                        }
                                                    }
                                                }
                                            }

                                            /* Check for delay, if present, change from default. Call probe */
                                            if (OSPPAuthRspHasDelayLimit(trans->AuthRsp)) {
                                                delay = OSPPAuthRspGetDelayLimit(trans->AuthRsp);
                                            }

                                            if ((errcode = OSPPTNProbe(probelist, probecnt, delay)) == OSPC_ERR_NO_ERROR) {
                                                if (OSPPAuthRspHasDelayLimit(trans->AuthRsp)) {
                                                    OSPPTNProbePruneList(&(trans->AuthRsp->Destination),
                                                        probelist,
                                                        OSPPAuthRspGetDelayLimit(trans->AuthRsp),
                                                        ospvNumberOfDestinations);
                                                }

                                                if ((OSPPAuthRspHasDelayPref(trans->AuthRsp)) && (*ospvNumberOfDestinations > 1)) {
                                                    OSPPTNProbeArrangeList(&(trans->AuthRsp->Destination),
                                                        probelist, *ospvNumberOfDestinations);
                                                }
                                            }

                                            if (probelist != OSPC_OSNULL) {
                                                OSPM_FREE(probelist);
                                            }
                                        }
                                    }
                                }
                            } else {
                                OSPPTransactionDeleteRequest(trans);
                            }

                            OSPPMsgInfoDelete(&msginfo);

                            if ((trans->AuthReq != OSPC_OSNULL) && (trans->AuthReq->DestinationAlternate != OSPC_OSNULL)) {
                                /* We don't keep these around */
                                while (!OSPPListEmpty(&(trans->AuthReq->DestinationAlternate))) {
                                    altinfo = (OSPT_ALTINFO *)OSPPListRemove(&(trans->AuthReq->DestinationAlternate));
                                    if (altinfo != OSPC_OSNULL) {
                                        OSPM_FREE(altinfo);
                                        altinfo = OSPC_OSNULL;
                                    }
                                }
                                OSPPListDelete(&(trans->AuthReq->DestinationAlternate));
                            }
                        }
                    }
                }
            } else {
                errcode = OSPC_ERR_TRAN_NO_NEW_AUTHREQ;
                OSPM_DBGERRORLOG(errcode, "Unfinished AuthReq found.");
            }

            if (xmldoc != OSPC_OSNULL) {
                OSPM_FREE(xmldoc);
                xmldoc = OSPC_OSNULL;
            }
        }
    }

    /* end else (valid data) */
    /* Set transaction state */
    if ((errcode == OSPC_ERR_NO_ERROR) || (errcode == OSPC_ERR_TRAN_NO_NEW_AUTHREQ)) {
        OSPPTransactionSetState(trans, OSPC_AUTH_REQUEST_SUCCESS);
    } else {
        OSPPTransactionSetState(trans, OSPC_AUTH_REQUEST_FAIL);
    }

    return errcode;
}

/*
 * OSPPTransactionRequestReauthorisation()
 *
 * Request reauthorization for a previously authorised call.
 *
 * The OSPPTransactionRequestReauthorisation function asks the SDK library to
 * refresh a previously granted authorization, perhaps, for example, because
 * the time limit for that authorization is nearing its expiration. Parameters
 * to the function are:
 *   ospvTransaction: handle of the (previously created) transaction object.
 *   ospvSizeOfToken: pointer to a variable which, on input, contains the size
 *      of the memory buffer in which the function should store the
 *      authorization token for the destination. If the value is not large
 *      enough to accommodate the token, then an error is indicated and no
 *      destination is returned. On output this variable is updated to indicate
 *      the actual size of the authorization token.
 *   ospvToken: memory location in which to store the authorization token for
 *      this destination. In general, tokens are opaque, binary objects.
 *   ospvAuthorised: pointer to a variable in which the function will indicate
 *      whether or not the call is reauthorised. On return, a non-zero value
 *      indicates that the call is reauthorised by the provider, while a zero
 *      value indicates an authorization failure.
 *   ospvTimeLimit: pointer to a variable in which to place the total number of
 *      seconds for which the call is now authorised. A value of zero indicates
 *      that no limit exists.
 *   ospvSizeOfDetailLog: pointer to a variable which, on input, contains the
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero,
 *      applications indicate that they do not wish a detail log for the
 *      authorization reauthorization.
 *   ospvDetailLog: pointer to a location in which to store a detail log for
 *      the reauthorization. If this pointer is not NULL, and if the
 *      ospvSizeOfDetailLog parameter is non-zero, then the library will store
 *      a copy of the reauthorization response obtained from the settlement
 *      provider, including the settlement provider's digital signature.
 * This function blocks on network input/output until authorization has been
 * refreshed, refused, or an error has been detected. The Open Settlement
 * Protocol SDK Porting Guide includes information on modifying that behavior
 * to prevent blocking.
 *
 * returns OSPC_ERR_NO_ERROR if successful, otherwise error code.
 */
int OSPPTransactionRequestReauthorisation(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvDuration,          /* In - duration of in-progress call */
    unsigned *ospvSizeOfToken,      /* In/Out - max size of authorization token space actual size of authorization token */
    void *ospvToken,                /* Out - Reauthorization token storage */
    unsigned *ospvAuthorised,       /* Out - Indicator of status of reauthorization (non-zero = OK) */
    unsigned *ospvTimeLimit,        /* Out - Total number of seconds for reauthorization ( zero = unlimited ) */
    unsigned *ospvSizeOfDetailLog,  /* In/Out - Max size of detail log\ Actual size of detail log */
    void *ospvDetailLog)            /* In/Out - Location of detail log storage */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    unsigned char *xmldoc = OSPC_OSNULL;
    unsigned sizeofxmldoc = 0;
    OSPT_MSG_INFO *msginfo = OSPC_OSNULL;
    OSPT_STATUS *status = OSPC_OSNULL;
    OSPT_TOKEN *token = OSPC_OSNULL;

    *ospvAuthorised = OSPC_FALSE;
    OSPM_ARGUSED(ospvSizeOfDetailLog);
    OSPM_ARGUSED(ospvDetailLog);

    /* Get transaction context */
    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if (trans == OSPC_OSNULL) {
            errcode = OSPC_ERR_TRAN_TRANSACTION_NOT_FOUND;
            OSPM_DBGERRORLOG(errcode, "transaction context not found");
        }
    }

    /* Has to be OGW */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (trans->AuthRsp != OSPC_OSNULL) {
            /* OGW */
            /* Build ReauthReq */
            errcode = OSPPTransactionBuildReauthRequest(trans, ospvDuration);

        } else if (trans->AuthInd != OSPC_OSNULL) {     /* TGW */
            errcode = OSPC_ERR_TRAN_TRANSACTION_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errcode, "This transaction not allowed on TGW.");
        } else {
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "No information available to process this request.");
        }
    }

    /* Create appropriate xml message */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPXMLMessageCreate(OSPC_MSG_REAREQ, &xmldoc, &sizeofxmldoc, trans->ReauthReq, trans);
    }

    /* Create msginfo, put in queue and process return */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPMsgInfoNew(&msginfo);
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = OSPPTransactionPrepareAndQueMessage(trans, xmldoc, sizeofxmldoc, &msginfo);
            if (errcode == OSPC_ERR_NO_ERROR) {
                errcode = OSPPTransactionProcessReturn(trans, msginfo);
            }

            OSPPMsgInfoDelete(&msginfo);
        }
    }

    if ((trans->ReauthRsp != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR)) {

        /* verify REARESP TXID == trans->txid */
        if (trans->ReauthRsp->TrxId != trans->TransactionID) {
            errcode = OSPC_ERR_TRAN_TXID_INVALID;
        }

        /* check status code - set authorized */
        if (OSPPReauthRspHasStatus(trans->ReauthRsp) == OSPC_FALSE) {
            errcode = OSPC_ERR_TRAN_STATUS_INVALID;
            OSPM_DBGERRORLOG(errcode, "status not found");
        } else {
            status = OSPPReauthRspGetStatus(trans->ReauthRsp);
            if (status != OSPC_OSNULL) {
                if (!OSPM_STATUSCODE_SUCCESSFUL(status->Code)) {

                    errcode = status->Code;
                    OSPM_DBGERRORLOG(errcode, "server returned a status error");
                } else {
                    *ospvAuthorised = OSPC_TRUE;
                }
            } else {
                errcode = OSPC_ERR_TRAN_STATUS_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "status not found");
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            if (!OSPPReauthRspHasDest(trans->ReauthRsp)) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
            } else {
                /* set token from REARESP token */
                if (!OSPPDestHasToken(trans->ReauthRsp->Destination)) {
                    errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                    OSPM_DBGERRORLOG(errcode, "null pointer for token.");
                } else {
                    token = (OSPT_TOKEN *)OSPPDestFirstToken(trans->ReauthRsp->Destination);
                    if (token == OSPC_OSNULL) {

                        errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                        OSPM_DBGERRORLOG(errcode, "null pointer for token.");
                    } else {
                        if (*ospvSizeOfToken < (unsigned)OSPPTokenGetSize(token)) {
                            errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                            OSPM_DBGERRORLOG(errcode, "not enough space for token");
                        } else {
                            /* Get the token */
                            OSPM_MEMCPY(ospvToken, OSPPTokenGetValue(token), OSPPTokenGetSize(token));
                        }
                        *ospvSizeOfToken = OSPPTokenGetSize(token);
                    }
                }

                if (errcode == OSPC_ERR_NO_ERROR) {
                    /* set timelimit from REARESP duration */
                    if (!OSPPDestHasLimit(trans->ReauthRsp->Destination)) {
                        errcode = OSPC_ERR_TRAN_DATA_NOT_FOUND;
                    } else {
                        *ospvTimeLimit = OSPPDestGetLimit(trans->ReauthRsp->Destination);
                    }
                }
            }
        }
    }

    if (xmldoc != OSPC_OSNULL) {
        OSPM_FREE(xmldoc);
        xmldoc = OSPC_OSNULL;
    }

    /* Now delete the reauth request and reauth response */
    OSPPTransactionDeleteReauthReq(trans);
    OSPPTransactionDeleteReauthRsp(trans);

    return errcode;
}

/*
 * The OSPPTransactionValidateAuthorisation function asks the SDK library to
 * validate a requested incoming call, based on the call's parameters and
 * authorization tokens included in the call. This function may be called
 * multiple times for a single call, so that, for example, call requests with
 * multiple authorization tokens may result in multiple calls to this function,
 * one for each authorization token in the request. Parameters to the function
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
 * ospvSourceDevice: character string identifying the source device.
 *      This could be the previous hop Gateway.
 *      It is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                  "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 *  ospvDestinationDevice: character string identifying the destination device.
 *      The value is expressed as either a DNS name or an IP address enclosed
 *      in square brackets, followed by an optional colon and TCP port number.
 *      Examples of valid destinations include "gateway1.carrier.com" and
 *                                                          "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 *  ospvCallingNumber: character string containing the calling party's number
 *      expressed as a full international number conforming to the ITU E.164
 *      standard (with no punctuation).
 *  ospvCallingNumberFormat: Value that identifies the type of calling number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvCalledNumber: character string containing the called number, expressed
 *      as a full international number conforming to the ITU E.164 standard
 *      (with no punctuation).
 *  ospvCalledNumberFormat: Value that identifies the type of called number.
 *      It can be either {0-E.164, 1-SIP or 2-URL}
 *  ospvSizeOfCallId: size of the memory buffer containing the call identifier.
 *  ospvCallId: memory location containing the H.323 call identifier for the
 *      call.
 *  ospvSizeOfToken: size of the memory buffer containing an authorization
 *      token for the call.
 *  ospvToken: memory location containing an authorization token.
 *  ospvAuthorised: pointer to a variable in which the function will indicate
 *      whether or not the call is authorised. On return, a non-zero value
 *      indicates that the call is authorised by the provider, while a zero
 *      value indicates an authorization failure.
 *  ospvTimeLimit: pointer to a variable in which to place the number of
 *      seconds for which the call is initially authorised. A value of zero
 *      indicates that no limit exists. Note that the initial time limit may be
 *      extended during the call by using the function
 *      OSPPTransactionRequestReauthorisation.
 *  ospvSizeOfDetailLog: pointer to a variable which, on input, contains the
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero,
 *      applications indicate that they do not wish a detail log for the
 *      authorization validation.
 *  ospvDetailLog: pointer to a location in which to store a detail log for the
 *      validation. If this pointer is not NULL, and if the ospvSizeOfDetailLog
 *      parameter is non-zero, then the library will store a copy of the
 *      authorization confirmation obtained from the settlement provider,
 *      including the settlement provider's digital signature.
 *  ospvTokenAlgo: This can take either of the 3 values - TOKEN_ALGO_SIGNED,
 *      TOKEN_ALGO_UNSIGNED, and TOKEN_ALGO_BOTH. If the value is set to
 *      TOKEN_ALGO_SIGNED the toolkit expects the token to be signed, and
 *      validates the signature as a part of token validation. If the value
 *      is set to TOKEN_ALGO_UNSIGNED, an unsigned token is expected. If the
 *      value is set to TOKEN_ALGO_BOTH, then the toolkit accepts either a
 *      signed token or an unsigned token.
 * If the provider has been configured to perform local validation, the SDK
 * library is able to perform this function without network interaction, and,
 * therefore, does not block for network input or output during its execution.
 * If local validation is not used, this function blocks until authorization
 * has been validated, refused, or an error has been detected. The Open
 * Settlement Protocol SDK Porting Guide includes information on modifying that
 * behavior to prevent blocking.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionValidateAuthorisation(
    OSPTTRANHANDLE ospvTransaction,                 /* In - Transaction handle */
    const char *ospvSource,                         /* In - Source of call */
    const char *ospvDestination,                    /* In - Destination for call */
    const char *ospvSourceDevice,                   /* In - SourceDevice of call */
    const char *ospvDestinationDevice,              /* In - DestinationDevice for call */
    const char *ospvCallingNumber,                  /* In - Calling number string */
    OSPE_NUMBER_FORMAT ospvCallingNumberFormat,     /* In - Calling number Format : sip/e.164/url */
    const char *ospvCalledNumber,                   /* In - Called number string */
    OSPE_NUMBER_FORMAT ospvCalledNumberFormat,      /* In - Called number Format : sip/e.164/url */
    unsigned ospvSizeOfCallId,                      /* In - Size of call id value */
    const void *ospvCallId,                         /* In - Call Id for this call */
    unsigned ospvSizeOfToken,                       /* In - Size of authorization token */
    const void *ospvToken,                          /* In - Authorization token */
    unsigned *ospvAuthorised,                       /* Out - Call authorization indicator */
    unsigned *ospvTimeLimit,                        /* Out - Number of seconds call is authorised for */
    unsigned *ospvSizeOfDetailLog,                  /* In/Out - Max size of detail log\ Actual size of detail log */
    void *ospvDetailLog,                            /* In - Pointer to storage for detail log */
    unsigned ospvTokenAlgo)                         /* In - Algorithm to be used for Validating Token */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_AUTH_IND *authind = OSPC_OSNULL;
    OSPT_CALL_ID *callid = OSPC_OSNULL;
    OSPT_TOKEN *token = OSPC_OSNULL;
    int retcode = 0;
    OSPT_TOKEN_INFO *tokeninfo = OSPC_OSNULL;
    OSPE_MESSAGE dtype = OSPC_MSG_UNKNOWN;
    OSPT_ALTINFO *altinfo = OSPC_OSNULL;
    OSPTPROVIDER *provider = OSPC_OSNULL;
    OSPTSEC *security = OSPC_OSNULL;
    unsigned char *tokenmsg = OSPC_OSNULL;
    unsigned sizeoftokenmsg = 0;
    OSPT_DEST *dest = OSPC_OSNULL;
    int BAllowDupTransId = OSPC_TRUE;
    token_algo_t tokenAlgo = (token_algo_t)ospvTokenAlgo;
    OSPTBOOL IsTokenSigned = OSPC_FALSE;
    OSPE_DEST_OSPENABLED dstOSPStatus;
    OSPE_PROTOCOL_NAME protocol;
    unsigned char *CallIdValue = OSPC_OSNULL;
    unsigned CallIdSize = 0;
    OSPTBOOL callidundefined = OSPC_FALSE;
    unsigned char *AuthIndCallId = OSPC_OSNULL;
    unsigned char AsciiTokenMsg[1000];   /* The assumption is that the ASCII
                                          * token will be less than 1000 bytes
                                          * long.
                                          */

    OSPM_ARGUSED(ospvSizeOfDetailLog);
    OSPM_ARGUSED(ospvDetailLog);

    /* Verify input */
    if (((ospvDestination == OSPC_OSNULL) &&
         (ospvDestinationDevice == OSPC_OSNULL)) ||
        (ospvCallingNumber == OSPC_OSNULL) ||
        (ospvCalledNumber == OSPC_OSNULL) ||
        (ospvSizeOfToken == 0) ||
        (ospvToken == OSPC_OSNULL) ||
        ((ospvCallingNumberFormat < OSPC_NFORMAT_START) || (ospvCallingNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvCalledNumberFormat < OSPC_NFORMAT_START) || (ospvCalledNumberFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((tokenAlgo < TOKEN_ALGO_SIGNED) || (tokenAlgo > TOKEN_ALGO_BOTH))) {

        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for ValidateAuth");
    }

    /* Check to see what tokenAlgo value is passed,
     * and whether it is consistent with the kind of token passed in the API.
     */
    if ((OSPM_STRNCMP("<?xml", ospvToken, 5) == 0) || (OSPM_STRNCMP("V=1\n", ospvToken, 4) == 0)) {
        /*
         * The token is unsigned.
         * if tokenAlgo = TOKEN_ALGO_SIGNED, then send an error back
         */
        if (tokenAlgo == TOKEN_ALGO_SIGNED) {
            errcode = OSPC_ERR_TRAN_UNSIGNED_TOKEN_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errcode, "Not configured to do unsigned token in ValidateAuth");
        } else {
            IsTokenSigned = OSPC_FALSE;
        }
    } else {
        /*
         * The token is signed.
         * if tokenAlgo = TOKEN_ALGO_UNSIGNED, then send an error back
         */
        if (tokenAlgo == TOKEN_ALGO_UNSIGNED) {
            errcode = OSPC_ERR_TRAN_SIGNED_TOKEN_NOT_ALLOWED;
            OSPM_DBGERRORLOG(errcode, "Not configured to do signed token in ValidateAuth");
        } else {
            IsTokenSigned = OSPC_TRUE;
        }
    }

    /* Get transaction context */
    if (errcode == OSPC_ERR_NO_ERROR) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    /* Validate the signature and get the call id from the token */
    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPTransactionGetProvider(trans, &provider);
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPProviderGetSecurity(provider, &security);
    }

    if ((errcode == OSPC_ERR_NO_ERROR) && (IsTokenSigned == OSPC_TRUE)) {
        errcode = OSPPSecSignatureVerify(security,
            &tokenmsg, &sizeoftokenmsg, (unsigned char *)ospvToken, ospvSizeOfToken, OSPC_SEC_SIGNATURE_AND_CONTENT);

#ifdef OSPC_VALIDATE_TOKEN_CERT_SUBJECT_NAME
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = OSPPTransactionValidateTokenCert(trans, (unsigned char *)ospvToken, ospvSizeOfToken);
        }
#endif

    }

    if ((errcode == OSPC_ERR_NO_ERROR) && (IsTokenSigned == OSPC_FALSE)) {
        /* just copy the token content into the token msg. No sig present */
        OSPM_MALLOC(tokenmsg, unsigned char, ospvSizeOfToken);
        if (tokenmsg != OSPC_OSNULL) {
            OSPM_MEMCPY(tokenmsg, ospvToken, ospvSizeOfToken);
            sizeoftokenmsg = ospvSizeOfToken;
            errcode = OSPC_ERR_NO_ERROR;
        } else {
            errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
        }
    }

    /* Parse the Token into a TokenInfo structure */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (OSPM_STRNCMP("<?xml", (const char *)tokenmsg, 5) == 0) {
            errcode = OSPPXMLMessageParse((unsigned char *)tokenmsg, sizeoftokenmsg, (void **)&tokeninfo, &dtype);
        } else {
            /*
             * This is an IAX or SIP token
             * we need to copy the info to another string
             * because the tokeninfo array is not null terminated,
             * and there is not enough memory to add a '\0' at the end.
             * Explicitly NULL terminate the string
             */
            OSPM_MEMCPY(AsciiTokenMsg, tokenmsg, sizeoftokenmsg);
            AsciiTokenMsg[sizeoftokenmsg] = '\0';
            dtype = OSPC_MSG_TOKINFO;
            errcode = OSPPParseTokenInfoFromASCIIToken((unsigned char *)AsciiTokenMsg, sizeoftokenmsg, &tokeninfo);
        }
    }


    /* Get the call id from the token, if it is "UNDEFINED", set callidundefined */
    if (errcode == OSPC_ERR_NO_ERROR) {
        CallIdSize = OSPPTokenInfoGetCallIdSize(tokeninfo);
        CallIdValue = OSPPTokenInfoGetCallIdValue(tokeninfo);
        if ((CallIdSize == OSPC_UNDEFINED_CALLID_SIZE) && (OSPM_MEMCMP(CallIdValue, OSPC_UNDEFINED_CALLID_STR, OSPC_UNDEFINED_CALLID_SIZE) == 0)) {
            callidundefined = OSPC_TRUE;
        } else {
            callidundefined = OSPC_FALSE;
        }
    }

    /* Is there an AuthInd here already? If so, we are only adding the token */
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* not here... make a new one */
        if (trans->AuthInd == OSPC_OSNULL) {
            authind = OSPPAuthIndNew();

            /* populate the new one */
            if (authind != OSPC_OSNULL) {

                OSPPAuthIndSetTimestamp(authind, time(OSPC_OSNULL));

                if ((ospvSizeOfCallId > 0) && (ospvCallId != OSPC_OSNULL)) {
                    callid = OSPPCallIdNew(ospvSizeOfCallId, (const unsigned char *)ospvCallId);
                } else {
                    /* Copy the callId from the token. */
                    callid = OSPPCallIdNew(CallIdSize, (const unsigned char *)CallIdValue);
                }

                if (callid != OSPC_OSNULL) {
                    OSPPAuthIndSetCallId(authind, callid);
                    OSPPCallIdDelete(&callid);
                } else {
                    errcode = OSPC_ERR_TRAN_CALLID_NOT_FOUND;
                    OSPM_DBGERRORLOG(errcode, "callid is null");
                }

                if (errcode == OSPC_ERR_NO_ERROR) {
                    OSPPAuthIndSetRole(authind, OSPC_ROLE_DESTINATION);
                    OSPPAuthIndSetSourceNumber(authind, ospvCallingNumber);
                    trans->CallingNumberFormat = ospvCallingNumberFormat;

                    OSPPAuthIndSetDestNumber(authind, ospvCalledNumber);
                    trans->CalledNumberFormat = ospvCalledNumberFormat;

                    OSPPListNew(&(authind->Tokens));

                    /* create the destination object */
                    dest = OSPPDestNew();

                    if (dest == OSPC_OSNULL) {
                        errcode = OSPC_ERR_DATA_NO_DEST;
                    } else {
                        if ((ospvCallId != OSPC_OSNULL) && (ospvSizeOfCallId > 0)) {
                            OSPPDestSetCallId(dest, (const unsigned char *)ospvCallId, ospvSizeOfCallId);
                        }

                        OSPPDestSetNumber(dest, ospvCalledNumber);

                        OSPPAuthIndSetDest(authind, dest);

                        trans->CurrentDest = dest;
                        dest = OSPC_OSNULL;

                    }
                }

                if (errcode == OSPC_ERR_NO_ERROR) {
                    token = OSPPTokenNew(ospvSizeOfToken, (const unsigned char *)ospvToken);

                    if (token != OSPC_OSNULL) {
                        OSPPListAppend(&(authind->Tokens), (void *)token);

                        trans->AuthInd = authind;
                    } else {
                        errcode = OSPC_ERR_TRAN_TOKEN_NOT_FOUND;
                        OSPM_DBGERRORLOG(errcode, "token is null");
                    }
                }

                if ((errcode == OSPC_ERR_NO_ERROR) && (ospvSourceDevice != OSPC_OSNULL)) {
                    /* device information - create a linked list */
                    OSPPListNew((OSPTLIST *)&(trans->AuthInd->DeviceInfo));
                    altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSourceDevice), ospvSourceDevice, OSPC_ALTINFO_TRANSPORT);
                    if (altinfo != OSPC_OSNULL) {
                        OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DeviceInfo), (void *)altinfo);
                    }
                    altinfo = OSPC_OSNULL;
                }

                /* end if ospvSourceDevice != OSPC_OSNULL */
                /* --------------------------------------
                 * SourceAlternate
                 * --------------------------------------
                 */
                if (errcode == OSPC_ERR_NO_ERROR) {
                    if ((ospvSource != OSPC_OSNULL) || (trans->SrcNetworkId != OSPC_OSNULL)) {
                        /* source alternates - create a linked list */
                        OSPPListNew((OSPTLIST *)&(trans->AuthInd->SourceAlternate));
                        if (trans->SrcNetworkId != OSPC_OSNULL) {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->SrcNetworkId), trans->SrcNetworkId, OSPC_ALTINFO_NETWORK);
                            if (altinfo != OSPC_OSNULL) {
                                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->SourceAlternate), (void *)altinfo);
                            }
                        }
                        altinfo = OSPC_OSNULL;

                        if (ospvSource != OSPC_OSNULL) {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvSource), ospvSource, OSPC_ALTINFO_TRANSPORT);
                            if (altinfo != OSPC_OSNULL) {
                                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->SourceAlternate), (void *)altinfo);
                            }
                        }
                        altinfo = OSPC_OSNULL;
                    } else {
                        errcode = OSPC_ERR_TRAN_SOURCE_INVALID;
                    }
                }

                /* end  if (errcode == OSPC_ERR_NO_ERROR) */
                /* -----------------------------------------------------
                 * DestinationAlternate
                 * -----------------------------------------------------
                 */
                if (errcode == OSPC_ERR_NO_ERROR) {

                    if ((ospvDestination != OSPC_OSNULL) || (ospvDestinationDevice != OSPC_OSNULL) || (trans->DestNetworkId != OSPC_OSNULL)) {

                        /* destination alternates - create a linked list */
                        OSPPListNew((OSPTLIST *)&(trans->AuthInd->DestinationAlternate));

                        /*
                         * We want to copy the Dest Trnk Group from the
                         * Transaction Structure only when the tokenInfo
                         * does not contain the Network Id.
                         * Thus, we are overwriting what had been Set using
                         * the SetNetworkIds API
                         */
                        if ((trans->DestNetworkId != OSPC_OSNULL) && tokeninfo && (tokeninfo->HasDestinationNetworkId == OSPC_FALSE)) {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(trans->DestNetworkId), trans->DestNetworkId, OSPC_ALTINFO_NETWORK);
                            if (altinfo != OSPC_OSNULL) {
                                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                            }
                        }
                        altinfo = OSPC_OSNULL;

                        if (ospvDestination != OSPC_OSNULL) {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestination), ospvDestination, OSPC_ALTINFO_TRANSPORT);
                            if (altinfo != OSPC_OSNULL) {
                                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                            }
                        }
                        altinfo = OSPC_OSNULL;

                        if (ospvDestinationDevice != OSPC_OSNULL) {
                            altinfo = OSPPAltInfoNew(OSPM_STRLEN(ospvDestinationDevice), ospvDestinationDevice, OSPC_ALTINFO_H323);
                            if (altinfo != OSPC_OSNULL) {
                                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
                            }
                        }   /* end if ospvDestinationDevice != OSPC_OSNULL */
                    } else {
                        errcode = OSPC_ERR_TRAN_DEST_INVALID;
                    }
                }   /* end  if (errcode == OSPC_ERR_NO_ERROR) */
            }
        } else {
            /* authind already here, make sure it is the right one */
            retcode = OSPM_STRCMP(OSPPAuthIndGetDestNumber(trans->AuthInd), ospvCalledNumber);

            if (retcode != 0) {
                errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                OSPM_DBGERRORLOG(errcode, "Called number does not match");
            } else {
                retcode = OSPM_STRCMP(OSPPAuthIndGetSourceNumber(trans->AuthInd), ospvCallingNumber);

                if (retcode != 0) {
                    errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                    OSPM_DBGERRORLOG(errcode, "Calling number does not match");
                }
            }

            if (errcode == OSPC_ERR_NO_ERROR) {

                if (trans->AuthInd->Tokens != OSPC_OSNULL) {

                    token = OSPPTokenNew(ospvSizeOfToken, (const unsigned char *)ospvToken);

                    if (token != OSPC_OSNULL) {
                        OSPPListAppend(&(trans->AuthInd->Tokens), (void *)token);
                    } else {
                        errcode = OSPC_ERR_TRAN_TOKEN_NOT_FOUND;
                        OSPM_DBGERRORLOG(errcode, "token is null");
                    }
                } else {
                    errcode = OSPC_ERR_TRAN_LIST_NOT_FOUND;
                    OSPM_DBGERRORLOG(errcode, "list is null");
                }
            }
        }
    }

    /*
     * Now that we have a complete AuthInd, let's verify it against
     * the token received
     */
    if (errcode == OSPC_ERR_NO_ERROR) {
        /*
         * The token validation part has been moved from here to the
         * the top. This is so that we may extract the call Id from
         * the token early enough.
         */

        /*
         * If Forward Route information is present,
         * Store it in the transaction object
         */
        if (tokeninfo != OSPC_OSNULL) {
            trans->TokenInfoHasLookAheadInfo = tokeninfo->HasLookAheadInfo;
        }

        if (trans->TokenInfoHasLookAheadInfo) {
            /* Set the destination */
            OSPPTokenInfoSetLookAheadDestAlt(&(trans->TokenLookAheadInfo), OSPPTokenInfoGetLookAheadDestAlt(&(tokeninfo->LookAheadInfo)));

            /* Set the destination Protocol */
            protocol = OSPPTokenInfoGetLookAheadProtocol(&(tokeninfo->LookAheadInfo));
            trans->TokenLookAheadInfo.lookAheadProt = protocol;

            /* Set the OSP Version */
            dstOSPStatus = OSPPTokenInfoGetLookAheadOSPVersion(&(tokeninfo->LookAheadInfo));
            trans->TokenLookAheadInfo.lookAheadDestOSPStatus = dstOSPStatus;
        }

        /*
         * If the token contains the Network Id
         * then add that to the list of destination Alternates
         */
        if (tokeninfo && (tokeninfo->HasDestinationNetworkId == OSPC_TRUE)) {
            altinfo = OSPC_OSNULL;
            altinfo = OSPPAltInfoNew(OSPM_STRLEN(OSPPTokenInfoGetDestNetworkId(tokeninfo)),
                OSPPTokenInfoGetDestNetworkId(tokeninfo), OSPC_ALTINFO_NETWORK);

            if (altinfo != OSPC_OSNULL) {

                OSPPListAppend((OSPTLIST *)&(trans->AuthInd->DestinationAlternate), (void *)altinfo);
            }
            altinfo = OSPC_OSNULL;
            trans->AuthInd->HasDestNetworkIdInToken = OSPC_TRUE;
        }


        if (errcode == OSPC_ERR_NO_ERROR && dtype == OSPC_MSG_TOKINFO) {

            /* Verify Source Number */
            retcode = OSPM_STRCMP(OSPPAuthIndGetSourceNumber(trans->AuthInd), OSPPTokenInfoGetSourceNumber(tokeninfo));

            if (retcode != 0) {
                errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                OSPM_DBGERRORLOG(errcode, "information does not match");
            } else {
                /* Verify Destination Number */
                retcode = OSPM_STRCMP(OSPPAuthIndGetDestNumber(trans->AuthInd), OSPPTokenInfoGetDestNumber(tokeninfo));

                if (retcode != 0) {
                    errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                    OSPM_DBGERRORLOG(errcode, "information does not match");
                } else {
                    /* Verify CallId */
                    if ((CallIdValue != OSPC_OSNULL) &&
                        ((AuthIndCallId = OSPPAuthIndGetCallIdValue(trans->AuthInd)) != OSPC_OSNULL) &&
                        (ospvSizeOfCallId > 0) && (callidundefined == OSPC_FALSE)) {
                        retcode = OSPM_MEMCMP(AuthIndCallId, CallIdValue, ospvSizeOfCallId);
                    }

                    if (retcode != 0) {
                        errcode = OSPC_ERR_TRAN_TOKEN_INVALID;
                        OSPM_DBGERRORLOG(errcode, "information does not match");
                    } else {
                        /* Verify Valid After */
                        if (time((time_t *)0) < OSPPTokenInfoGetValidAfter(tokeninfo)) {
                            errcode = OSPC_ERR_TRAN_TOO_SOON_TO_USE_TOKEN;
                            OSPM_DBGERRORLOG(errcode, "too soon to use token");
                        } else {
                            /* Verify Valid Until */
                            if (time((time_t *)0) > OSPPTokenInfoGetValidUntil(tokeninfo)) {
                                errcode = OSPC_ERR_TRAN_TOO_LATE_TO_USE_TOKEN;
                                OSPM_DBGERRORLOG(errcode, "too late to use token");
                            }
                        }
                    }
                }
            }
            if (errcode == OSPC_ERR_NO_ERROR) {
                /* Add transaction id to transactionid tree and check for reuse. */
#ifdef OSP_ALLOW_DUP_TXN
                BAllowDupTransId = OSPC_TRUE;
#else
                BAllowDupTransId = OSPC_FALSE;
#endif
                if (BAllowDupTransId ||
                    (OSPPTransIdCheckAndAdd(OSPPTokenInfoGetTrxId(tokeninfo),
                    (unsigned long)OSPPTokenInfoGetValidUntil(tokeninfo), trans->Provider)))
                {
                    /* Populate Transaction Id */
                    trans->TransactionID = OSPPTokenInfoGetTrxId(tokeninfo);
                    trans->HasTransactionID = OSPC_TRUE;

                    *ospvAuthorised = OSPC_TRAN_AUTHORISED;
                    *ospvTimeLimit = (unsigned)OSPPTokenInfoGetDuration(tokeninfo);
                    OSPPAuthIndSetTimeLimit(trans->AuthInd, *ospvTimeLimit);
                } else {
                    errcode = OSPC_ERR_TRAN_TOKEN_REUSE;
                    OSPM_DBGERRORLOG(errcode, "attemp to reuse token");
                }
            }
        }
    }

    if (tokeninfo != OSPC_OSNULL) {
        OSPPTokenInfoDelete(&tokeninfo);
        tokeninfo = OSPC_OSNULL;
    }

    if (tokenmsg != OSPC_OSNULL) {
        OSPM_FREE(tokenmsg);
        tokenmsg = OSPC_OSNULL;
    }

    /* Set transaction state */
    if (errcode == OSPC_ERR_NO_ERROR) {
        OSPPTransactionSetState(trans, OSPC_VALIDATE_AUTH_SUCCESS);
    } else {
        OSPPTransactionSetState(trans, OSPC_VALIDATE_AUTH_FAIL);
    }
    return errcode;
}

int OSPPTransactionValidateReAuthorisation(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvSizeOfToken,       /* In - Size of authorization token */
    const void *ospvToken,          /* In - Authorization token */
    unsigned *ospvAuthorised,       /* Out - Call authorization indicator */
    unsigned *ospvTimeLimit,        /* Out - Number of seconds call is authorised for */
    unsigned *ospvSizeOfDetailLog,  /* In/Out - Max size of detail log\ Actual size of detail log */
    void *ospvDetailLog,            /* In - Pointer to storage for detail log */
    unsigned ospvTokenAlgo)         /* In - Algorithm to be used for Validating Token */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_ALTINFO *source = OSPC_OSNULL, *sourcealt = OSPC_OSNULL, *destination = OSPC_OSNULL, *destalt = OSPC_OSNULL;
    const char *sourceval = OSPC_OSNULL, *sourcealtval = OSPC_OSNULL, *destval = OSPC_OSNULL, *destaltval = OSPC_OSNULL;

    /* Validate will verify input */

    /* Get transaction context */
    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);

    /* Make sure this is a terminating gateway */
    if (errcode == OSPC_ERR_NO_ERROR) {
        if (trans->AuthRsp != OSPC_OSNULL) {
            errcode = OSPC_ERR_TRAN_TRANSACTION_NOT_ALLOWED;
        }
    }

    /* Make sure we have an AuthInd */
    if (trans->AuthInd != OSPC_OSNULL) {
        source = (OSPT_ALTINFO *)OSPPAuthIndFirstSourceAlt(trans->AuthInd);
        if (source != OSPC_OSNULL) {
            sourceval = OSPPAuthIndGetSourceAltValue(source);

            sourcealt = (OSPT_ALTINFO *)OSPPAuthIndNextSourceAlt(trans->AuthInd, source);
            if (sourcealt != OSPC_OSNULL) {
                sourcealtval = OSPPAuthIndGetSourceAltValue(sourcealt);
            }
        }

        destination = (OSPT_ALTINFO *)OSPPAuthIndFirstDestinationAlt(trans->AuthInd);
        if (destination != OSPC_OSNULL) {
            destval = OSPPAuthIndGetDestinationAltValue(destination);

            destalt = (OSPT_ALTINFO *)OSPPAuthIndNextDestinationAlt(trans->AuthInd, destination);
            if (destalt != OSPC_OSNULL) {
                destaltval = OSPPAuthIndGetDestinationAltValue(destalt);
            }
        }

        /* call ValidateAuthorisation */
        if (errcode == OSPC_ERR_NO_ERROR) {
            errcode = OSPPTransactionValidateAuthorisation(ospvTransaction,
                sourceval,
                destval,
                sourcealtval,
                destaltval,
                OSPPAuthIndGetSourceNumber(trans->AuthInd),
                trans->CallingNumberFormat,
                OSPPAuthIndGetDestNumber(trans->AuthInd),
                trans->CalledNumberFormat,
                OSPPAuthIndGetCallIdSize(trans->AuthInd),
                (const void *)OSPPAuthIndGetCallIdValue(trans->AuthInd),
                ospvSizeOfToken,
                ospvToken, ospvAuthorised,
                ospvTimeLimit, ospvSizeOfDetailLog, ospvDetailLog, ospvTokenAlgo);
        }
    }

    return errcode;
}

/*
 * The OSPPTransactionIndicateCapability function allows an application to
 * indicate its availability.
 * The parameters to the function are:
 *  ospvTransaction: handle of the (previously created) transaction object.
 *  ospvSource: character string identifying the source of the call. The value
 *      is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                      "[172.16.1.2]:112".
 *  ospvSourceDevice: character string identifying the source device.
 *      This could be the previous hop Gateway.
 *      It is expressed as either a DNS name or an IP address enclosed in square
 *      brackets, followed by an optional colon and TCP port number.
 *      Examples of valid sources include "gateway1.carrier.com" and
 *                                                  "[172.16.1.2]:112".
 *      This string is optional and may be empty.
 * ospvAlmostOutOfResources: a Boolean value to indicate whether or not the
 *      is about to run out of resources.  The value can also be used to
 *      indicate that a device is about to go off-line or come on-line.
 *  ospvSizeOfDetailLog: pointer to a variable which, on input, contains the
 *      maximum size of the detail log; on output, the variable will be updated
 *      with the actual size of the detail log. By setting this value to zero,
 *      applications indicate that they do not wish a detail log for the
 *      authorization request.
 *  ospvDetailLog: pointer to a location in which to store a detail log for the
 *      authorization request. If this pointer is not NULL, and if the
 *      ospvSizeOfDetailLog parameter is non-zero, then the library will store
 *      a copy of the authorization response obtained from the settlement
 *      provider, including the settlement provider's digital signature.
 * As delivered in the SDK library, this function blocks until confirmation
 * has been received or an error has been detected. The Open Settlement
 * Protocol SDK Porting Guide includes information on modifying that behavior
 * to prevent blocking.
 * The function returns an error code or zero (if the operation was successful)
 * Specific error codes and their meanings can be found in the osperrno.h file.
 */
int OSPPTransactionIndicateCapabilities(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvSource,             /* In - Source of call */
    const char *ospvSourceDevice,       /* In - SourceDevice of call */
    const char *ospvSourceNetworkId,    /* In - NetworkId of call. Could be trunk grp */
    unsigned ospvAlmostOutOfResources,  /* In - Boolean almost out of resources indicator */
    unsigned *ospvSizeOfDetailLog,      /* In/Out - Max size of detail log Actual size of detail log */
    void *ospvDetailLog)                /* In/Out - Location of detail log storage */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_MSG_INFO *msginfo = OSPC_OSNULL;
    unsigned char *xmldoc = OSPC_OSNULL;
    unsigned sizeofxmldoc = 0;
    OSPT_CAP_IND *capind = OSPC_OSNULL;

    OSPM_ARGUSED(ospvSizeOfDetailLog);
    OSPM_ARGUSED(ospvDetailLog);

    /* Validate input parameters - Source and SourceDevice must not be NULLs */
    if (OSPC_OSNULL == ospvSource || OSPC_OSNULL == ospvSourceDevice) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for OSPPTransactionIndicateCapabilities");
        OSPM_DBGERRORLOG(errcode, "ospvSource and ospvSourceDevice must not be NULLs");
    }

    /*
     * Get transaction context, the call can fail if the transaction
     * handle passed to the function is not valid.
     */
    if (OSPC_ERR_NO_ERROR == errcode) {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    }

    /*
     * Insure that the transaction does not already have a capability
     * response, auth response, or token structures assigned to it.
     */
    if (OSPC_ERR_NO_ERROR == errcode) {
        if (trans->CapCnf != OSPC_OSNULL) {
            errcode = OSPC_ERR_TRAN_DUPLICATE_REQUEST;
            OSPM_DBGERRORLOG(errcode, "Duplicate Call To OSPPTransactionIndicateCapabilities");
        } else if (trans->AuthRsp != OSPC_OSNULL || trans->AuthInd != OSPC_OSNULL) {
            errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
            OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
        }
    }

    /* Input parameters and state validation are over, we are off to the business logic */
    if (OSPC_ERR_NO_ERROR == errcode) {
        /* Create and initialize new Capability Indication structure */
        errcode = OSPPCapIndNew(&capind, trans, ospvSource, ospvSourceDevice, ospvSourceNetworkId, ospvAlmostOutOfResources);

        /* XML-encode the structure */
        if (OSPC_ERR_NO_ERROR == errcode) {
            errcode = OSPPXMLMessageCreate(OSPC_MSG_CAPIND, &xmldoc, &sizeofxmldoc, capind, trans);
        }

        /* Prepare a structure for sending the message over the network */
        if (OSPC_ERR_NO_ERROR == errcode) {
            errcode = OSPPMsgInfoNew(&msginfo);
        }

        /*
         * Hand off the request to a communication manager and block waiting for a response.
         * The function will decide which comm manager should handle the call.
         * The function call will not return untill a valid response is received or all
         * service points and retries have been exausted.
         */
        if (OSPC_ERR_NO_ERROR == errcode) {
            OSPPTransactionSetState(trans, OSPC_CAP_IND_BLOCK);

            errcode = OSPPTransactionPrepareAndQueMessage(trans, xmldoc, sizeofxmldoc, &msginfo);
        }

        /*
         * Parse the XML message, build a response structure and assign it to the
         * transaction object.  The structure will be released when the transaction
         * is deleted
         */
        if (OSPC_ERR_NO_ERROR == errcode) {
            errcode = OSPPTransactionProcessReturn(trans, msginfo);
        }

        /* If the response code is above 299, translate it to an error code */
        if (OSPC_ERR_NO_ERROR == errcode) {
            if (trans->CapCnf->ospmStatus->Code > 299) {
                errcode = OSPPUtilGetErrorFromStatus(trans->CapCnf->ospmStatus->Code);
            }
        }

        /* Update the transaction status based on the error code */
        if (OSPC_ERR_NO_ERROR == errcode) {
            OSPPTransactionSetState(trans, OSPC_CAP_IND_SUCCESS);
        } else {
            OSPPTransactionSetState(trans, OSPC_CAP_IND_FAIL);
        }
    } else {
        /* Data or state validation must have failed */
    }

    /* Clean-up temporary objects */
    if (xmldoc != OSPC_OSNULL) {
        OSPM_FREE(xmldoc);
        xmldoc = OSPC_OSNULL;
    }

    if (capind != OSPC_OSNULL) {
        OSPPCapIndDelete(&capind);
    }

    if (msginfo != OSPC_OSNULL) {
        OSPPMsgInfoDelete(&msginfo);
    }

    return errcode;
}

/*
 * OSPPTransactionSetNumberPortability()
 *
 * Reports number portability parameters for a particular transaction
 * This function can be used to report NP specific information to the server.
 *   ospvTransaction: handle of the transaction object.
 *   ospvNPRn: the routing number specific information to be reported to the server.
 *   ospvNPCic: the carrier identification code specific information to be reported to the server.
 *   ospvNPNpdi: the npdi flag specific information to be reported to the server.
 * returns OSPC_ERR_NO_ERROR if successful, else error code.
 */
int OSPPTransactionSetNumberPortability(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvRn,             /* In - rn specific information */
    const char *ospvCic,            /* In - cic specific information */
    int ospvNpdi)                   /* In - npdi specific information */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
        if (trans->State == OSPC_REPORT_USAGE_SUCCESS) {
            errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
        } else {
            if (ospvRn != OSPC_OSNULL) {
                OSPM_STRNCPY(trans->NPRn, ospvRn, sizeof(trans->NPRn) - 1);
            }

            if (ospvCic != OSPC_OSNULL) {
                OSPM_STRNCPY(trans->NPCic, ospvCic, sizeof(trans->NPCic) - 1);
            }

            if (ospvNpdi) {
                trans->NPNpdi = OSPC_TRUE;
            } else {
                trans->NPNpdi = OSPC_FALSE;
            }
        }
    }

    return errcode;
}

int OSPPTransactionSetTermCause(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_TERM_CAUSE ospvType,       /* In - Termination cause type */
    unsigned ospvCode,              /* In - TCCode */
    const char *ospvDesc)           /* In - Description */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if ((ospvType < OSPC_TCAUSE_START) || (ospvType >= OSPC_TCAUSE_NUMBER)) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            dest = trans->CurrentDest;
            if (dest != OSPC_OSNULL) {
                OSPPDestSetTermCause(dest, ospvType, ospvCode, ospvDesc);
            }
        }
    }

    return errcode;
}

int OSPPTransactionSetDiversion(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvNumber,         /* In - Diversion number, DiversionSourceInfo e164 */
    const char *ospvDomain)         /* In - Diversion domain, DiversionDeviceInfo transport */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvNumber == OSPC_OSNULL) || (ospvNumber[0] == '\0')) || ((ospvDomain == OSPC_OSNULL) || (ospvDomain[0] == '\0'))) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->DivSrcInfo[OSPC_NFORMAT_E164], ospvNumber, sizeof(trans->DivSrcInfo[OSPC_NFORMAT_E164]) - 1);
            OSPM_STRNCPY(trans->DivDevInfo, ospvDomain, sizeof(trans->DivDevInfo) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetProtocol(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_PROTOCOL_TYPE ospvType,    /* In - Protocol type */
    OSPE_PROTOCOL_NAME ospvName)    /* In - Protocol name */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if (!OSPM_VALIDATE_PROTTYPE(ospvType) || !OSPM_VALIDATE_PROTNAME(ospvName)) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            if (ospvType == OSPC_PROTTYPE_DESTINATION) {
                dest = trans->CurrentDest;
                if (dest != OSPC_OSNULL) {
                    dest->Protocol = ospvName;
                }
            } else {
                trans->Protocol[ospvType] = ospvName;
            }
        }
    }

    return errcode;
}

int OSPPTransSetCodec(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvService,       /* In - Service type */
    OSPE_CODEC_TYPE ospvType,       /* In - Codec type */
    const char *ospvCodec)          /* In - Codec */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvService < OSPC_SERVICE_START) || (ospvService >= OSPC_SERVICE_NUMBER)) ||
        ((ospvType < OSPC_CODEC_START) || (ospvType >= OSPC_CODEC_NUMBER)) ||
        ((ospvCodec == OSPC_OSNULL) || (ospvCodec[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->Codec[ospvService][ospvType], ospvCodec, sizeof(trans->Codec[ospvService][ospvType]));
        }
    }

    return errcode;
}

int OSPPTransactionSetCodec(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_CODEC_TYPE ospvType,       /* In - Codec type */
    const char *ospvCodec)          /* In - Codec */
{
    return OSPPTransSetCodec(ospvTransaction, OSPC_SERVICE_VOICE, ospvType, ospvCodec);
}

int OSPPTransactionSetVideoCodec(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_CODEC_TYPE ospvType,       /* In - Codec type */
    const char *ospvCodec)          /* In - Codec */
{
    return OSPPTransSetCodec(ospvTransaction, OSPC_SERVICE_VIDEO, ospvType, ospvCodec);
}

int OSPPTransactionSetSessionId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SESSION_ID ospvType,       /* In - Session ID type */
    OSPT_CALL_ID *ospvSessionId)    /* In - Session ID */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvType < OSPC_SESSIONID_START) || (ospvType >= OSPC_SESSIONID_NUMBER)) ||
        (ospvSessionId == OSPC_OSNULL))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            if (trans->SessionId[ospvType] != OSPC_OSNULL) {
                OSPPCallIdDelete(&(trans->SessionId[ospvType]));
            }
            trans->SessionId[ospvType] = OSPPCallIdNew(ospvSessionId->Length, ospvSessionId->Value);
        }
    }

    return errcode;
}

int OSPPTransactionSetCustomInfo(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvIndex,             /* In - Index start from 0 */
    const char *ospvInfo)           /* In - User-defined info */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvIndex > OSPC_MAX_INDEX) ||
        (ospvInfo == OSPC_OSNULL) || (ospvInfo[0] == '\0') || (OSPM_STRLEN(ospvInfo) >= OSPC_SIZE_CUSTINFO))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            if (trans->CustomInfo[ospvIndex] == OSPC_OSNULL) {
                OSPM_MALLOC(trans->CustomInfo[ospvIndex], char, OSPC_SIZE_CUSTINFO);
            }
            if (trans->CustomInfo[ospvIndex] != OSPC_OSNULL) {
                OSPM_STRCPY(trans->CustomInfo[ospvIndex], ospvInfo);
            } else {
                errcode = OSPC_ERR_TRAN_MALLOC_FAILED;
            }
        }
    }

    return errcode;
}

int OSPPTransactionSetSrcNetworkId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvSrcNetworkId)   /* In - Source network ID */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvSrcNetworkId == OSPC_OSNULL) || (ospvSrcNetworkId[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->UsageSrcNetworkId, ospvSrcNetworkId, sizeof(trans->UsageSrcNetworkId) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetDestNetworkId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvDestNetworkId)  /* In - Destination network ID */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if ((ospvDestNetworkId == OSPC_OSNULL) || (ospvDestNetworkId[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            dest = trans->CurrentDest;
            if (dest != OSPC_OSNULL) {
                OSPPDestSetNetworkAddr(dest, ospvDestNetworkId);
            }
        }
    }

    return errcode;
}

int OSPPTransSetLost(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvService,       /* In - Service type */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvPackets,                /* In - Packets, -1 means unavailable */
    int ospvFraction)               /* In - Fraction, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetPacket(trans->Statistics, OSPC_STATS_LOST, ospvService, ospvMetric, ospvDir, ospvPackets, ospvFraction);
        }
    }

    return errcode;
}

int OSPPTransactionSetLost(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvPackets,                /* In - Packets, -1 means unavailable */
    int ospvFraction)               /* In - Fraction, -1 means unavailable */
{
    return OSPPTransSetLost(ospvTransaction, OSPC_SERVICE_VOICE, ospvMetric, ospvDir, ospvPackets, ospvFraction);
}

int OSPPTransactionSetVideoLost(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvPackets,                /* In - Packets, -1 means unavailable */
    int ospvFraction)               /* In - Fraction, -1 means unavailable */
{
    return OSPPTransSetLost(ospvTransaction, OSPC_SERVICE_VIDEO, ospvMetric, ospvDir, ospvPackets, ospvFraction);
}

int OSPPTransSetJitter(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvService,       /* In - Service type */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvSamples,                /* In - Samples of Jitter, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of Jitter in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of Jitter in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of Jitter in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of delay, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetMetrics(
                trans->Statistics,
                OSPC_STATS_JITTER,
                ospvService,
                ospvMetric,
                ospvDir,
                OSPC_SLEG_UNDEFINED,
                ospvSamples,
                ospvMin,
                ospvMax,
                ospvMean,
                ospvVariance);
        }
    }

    return errcode;
}

int OSPPTransactionSetJitter(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvSamples,                /* In - Samples of Jitter, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of Jitter in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of Jitter in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of Jitter in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of delay, -1 means unavailable */
{
    return OSPPTransSetJitter(ospvTransaction, OSPC_SERVICE_VOICE, ospvMetric, ospvDir, ospvSamples, ospvMin, ospvMax, ospvMean, ospvVariance);
}

int OSPPTransactionSetVideoJitter(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvSamples,                /* In - Samples of Jitter, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of Jitter in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of Jitter in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of Jitter in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of delay, -1 means unavailable */
{
    return OSPPTransSetJitter(ospvTransaction, OSPC_SERVICE_VIDEO, ospvMetric, ospvDir, ospvSamples, ospvMin, ospvMax, ospvMean, ospvVariance);
}

int OSPPTransSetDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvService,       /* In - Service type */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvSamples,                /* In - Samples of Delay, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of Delay in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of Delay in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of Delay in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of delay, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetMetrics(
                trans->Statistics,
                OSPC_STATS_DELAY,
                ospvService,
                ospvMetric,
                ospvDir,
                OSPC_SLEG_UNDEFINED,
                ospvSamples,
                ospvMin,
                ospvMax,
                ospvMean,
                ospvVariance);
        }
    }

    return errcode;
}

int OSPPTransactionSetDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvSamples,                /* In - Samples of Delay, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of Delay in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of Delay in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of Delay in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of delay, -1 means unavailable */
{
    return OSPPTransSetDelay(ospvTransaction, OSPC_SERVICE_VOICE, ospvMetric, ospvDir, ospvSamples, ospvMin, ospvMax, ospvMean, ospvVariance);
}

int OSPPTransactionSetVideoDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvSamples,                /* In - Samples of Delay, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of Delay in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of Delay in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of Delay in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of delay, -1 means unavailable */
{
    return OSPPTransSetDelay(ospvTransaction, OSPC_SERVICE_VIDEO, ospvMetric, ospvDir, ospvSamples, ospvMin, ospvMax, ospvMean, ospvVariance);
}

int OSPPTransSetRTDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvService,       /* In - Service type */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_SESSION_LEG ospvLeg,       /* In - Session leg */
    int ospvSamples,                /* In - Samples of RTDelay, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of RTDelay in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of RTDelay in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of RTDelay in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of round trip delay, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetMetrics(
                trans->Statistics,
                OSPC_STATS_RTDELAY,
                ospvService,
                ospvMetric,
                OSPC_SDIR_UNDEFINED,
                ospvLeg,
                ospvSamples,
                ospvMin,
                ospvMax,
                ospvMean,
                ospvVariance);
        }
    }

    return errcode;
}

int OSPPTransactionSetRTDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_SESSION_LEG ospvLeg,       /* In - Session leg */
    int ospvSamples,                /* In - Samples of RTDelay, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of RTDelay in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of RTDelay in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of RTDelay in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of round trip delay, -1 means unavailable */
{
    return OSPPTransSetRTDelay(ospvTransaction, OSPC_SERVICE_VOICE, ospvMetric, ospvLeg, ospvSamples, ospvMin, ospvMax, ospvMean, ospvVariance);
}

int OSPPTransactionSetVideoRTDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_SESSION_LEG ospvLeg,       /* In - Session leg */
    int ospvSamples,                /* In - Samples of RTDelay, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of RTDelay in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of RTDelay in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of RTDelay in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of round trip delay, -1 means unavailable */
{
    return OSPPTransSetRTDelay(ospvTransaction, OSPC_SERVICE_VIDEO, ospvMetric, ospvLeg, ospvSamples, ospvMin, ospvMax, ospvMean, ospvVariance);
}

int OSPPTransSetOctets(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvService,       /* In - Service type */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvOctets)                 /* In - Octets, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetInteger(trans->Statistics, OSPC_STATS_OCTETS, ospvService, ospvMetric, ospvDir, ospvOctets);
        }
    }

    return errcode;
}

int OSPPTransactionSetOctets(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvOctets)                 /* In - Octets, -1 means unavailable */
{
    return OSPPTransSetOctets(ospvTransaction, OSPC_SERVICE_VOICE, ospvMetric, ospvDir, ospvOctets);
}

int OSPPTransactionSetVideoOctets(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvOctets)                 /* In - Octets, -1 means unavailable */
{
    return OSPPTransSetOctets(ospvTransaction, OSPC_SERVICE_VIDEO, ospvMetric, ospvDir, ospvOctets);
}

int OSPPTransSetPackets(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE ospvService,       /* In - Service type */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvPackets)                /* In - Packets, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetInteger(trans->Statistics, OSPC_STATS_PACKETS, ospvService, ospvMetric, ospvDir, ospvPackets);
        }
    }

    return errcode;
}

int OSPPTransactionSetPackets(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvPackets)                /* In - Packets, -1 means unavailable */
{
    return OSPPTransSetPackets(ospvTransaction, OSPC_SERVICE_VOICE, ospvMetric, ospvDir, ospvPackets);
}

int OSPPTransactionSetVideoPackets(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_METRIC ospvMetric,   /* In - Statistics metric */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvPackets)                /* In - Packets, -1 means unavailable */
{
    return OSPPTransSetPackets(ospvTransaction, OSPC_SERVICE_VIDEO, ospvMetric, ospvDir, ospvPackets);
}

int OSPPTransactionSetRFactor(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    float ospvRFactor)              /* In - R-Factor, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetFloat(trans->Statistics, OSPC_STATS_RFACTOR, ospvDir, ospvRFactor);
        }
    }

    return errcode;
}

int OSPPTransactionSetMOSCQ(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    float ospvMOSCQ)                /* In - MOS-CQ, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetFloat(trans->Statistics, OSPC_STATS_MOSCQ, ospvDir, ospvMOSCQ);
        }
    }

    return errcode;
}

int OSPPTransactionSetMOSLQ(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    float ospvMOSLQ)                /* In - MOS-LQ, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetFloat(trans->Statistics, OSPC_STATS_MOSLQ, ospvDir, ospvMOSLQ);
        }
    }

    return errcode;
}

int OSPPTransactionSetICPIF(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_STATS_DIR ospvDir,         /* In - Statistics direction */
    int ospvICPIF)                  /* In - ICPIF, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPPStatsSetInteger(trans->Statistics, OSPC_STATS_ICPIF, OSPC_SERVICE_UNDEFINED, OSPC_SMETRIC_UNDEFINED, ospvDir, ospvICPIF);
        }
    }

    return errcode;
}

/*
 * OSPPTransactionGetNumberPortabilityParameters() :
 * Reports number portability parameters returned in AuthRsp
 * returns OSPC_ERR_NO_ERROR if successful, else a 'Request out of Sequence' errcode.
 */
int OSPPTransactionGetNumberPortabilityParameters(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvSizeOfNPRn,        /* In - Max size of NPRn */
    char *ospvNPRn,                 /* Out - routing number */
    unsigned ospvSizeOfNPCic,       /* In - Max size of NPCic */
    char *ospvNPCic,                /* Out - carrier identification code */
    int *ospvNPNpdi)                /* Out - npdi */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if (ospvSizeOfNPRn != 0) {
        ospvNPRn[0] = '\0';
    }
    if (ospvSizeOfNPCic != 0) {
        ospvNPCic[0] = '\0';
    }
    *ospvNPNpdi = OSPC_FALSE;


    if ((trans = OSPPTransactionGetContext(ospvTransaction, &errcode)) != OSPC_OSNULL) {
        if (trans->AuthReq != OSPC_OSNULL) {
            /* We are the source.  Get the information from the destination structure. */
            if (trans->State == OSPC_GET_DEST_SUCCESS) {
                if ((dest = trans->CurrentDest) == OSPC_OSNULL) {
                    errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                    OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
                } else {
                    if (ospvSizeOfNPRn != 0) {
                        if (ospvSizeOfNPRn > OSPM_STRLEN(dest->NPRn)) {
                            OSPM_STRNCPY(ospvNPRn, dest->NPRn, ospvSizeOfNPRn);
                        } else {
                            errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                            OSPM_DBGERRORLOG(errcode, "No enough buffer to copy NPRn.");
                        }
                    }
                    if ((errcode == OSPC_ERR_NO_ERROR) && (ospvSizeOfNPCic != 0)) {
                        if (ospvSizeOfNPCic > OSPM_STRLEN(dest->NPCic)) {
                            OSPM_STRNCPY(ospvNPCic, dest->NPCic, ospvSizeOfNPCic);
                        } else {
                            errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                            OSPM_DBGERRORLOG(errcode, "No enough buffer to copy NPCic.");
                        }
                    }
                    if (errcode == OSPC_ERR_NO_ERROR) {
                        *ospvNPNpdi = dest->NPNpdi;
                    }
                }
            } else {
                errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
            }
        } else if (trans->AuthInd != OSPC_OSNULL) {
            /* We are the destination.  Get the information from the AuthInd structure. */
            if ((dest = trans->AuthInd->Destination) == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
            } else {
                if (ospvSizeOfNPRn != 0) {
                    if (ospvSizeOfNPRn > OSPM_STRLEN(dest->NPRn)) {
                        OSPM_STRNCPY(ospvNPRn, dest->NPRn, ospvSizeOfNPRn);
                    } else {
                        errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errcode, "No enough buffer to copy NPRn.");
                    }
                }
                if ((errcode == OSPC_ERR_NO_ERROR) && (ospvSizeOfNPCic != 0)) {
                    if (ospvSizeOfNPCic > OSPM_STRLEN(dest->NPCic)) {
                        OSPM_STRNCPY(ospvNPCic, dest->NPCic, ospvSizeOfNPCic);
                    } else {
                        errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errcode, "No enough buffer to copy NPCic.");
                    }
                }
                if (errcode == OSPC_ERR_NO_ERROR) {
                    *ospvNPNpdi = dest->NPNpdi;
                }
            }
        } else {
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
        }
    }

    return errcode;
}

/*
 * OSPPTransactionSetOperatorName() :
 * The API sets the operator names in the transaction structure
 */
int OSPPTransactionSetOperatorName(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_OPERATOR_NAME ospvType,    /* In - Operator name type */
    const char *ospvName)           /* In - Operator name */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvType >= OSPC_OPNAME_START) && (ospvType < OSPC_OPNAME_NUMBER)) &&
        (ospvName != OSPC_OSNULL) && (*ospvName != '\0'))
    {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if (errcode == OSPC_ERR_NO_ERROR) {
            OSPM_STRNCPY(trans->OpName[ospvType], ospvName, sizeof(trans->OpName[ospvType]) - 1);
        }
    } else {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
        OSPM_DBGERRORLOG(errcode, "Invalid input for OSPPTransactionSetOperatorName");
    }

    return errcode;
}

/*
 * OSPPTransactionGetOperatorName() :
 * Reports operator names returned in AuthRsp
 * returns OSPC_ERR_NO_ERROR if successful, else a 'Request out of Sequence' errcode.
 */
int OSPPTransactionGetOperatorName(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_OPERATOR_NAME ospvType,    /* In - Operator name type */
    unsigned ospvSizeOfName,        /* In - Max size of name */
    char *ospvName)                 /* Out - Operator name */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    ospvName[0] = '\0';
    if ((trans = OSPPTransactionGetContext(ospvTransaction, &errcode)) != OSPC_OSNULL) {
        if (trans->AuthReq != OSPC_OSNULL) {
            /* We are the source.  Get the information from the destination structure. */
            if (trans->State == OSPC_GET_DEST_SUCCESS) {
                if ((dest = trans->CurrentDest) == OSPC_OSNULL) {
                    errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                    OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
                } else {
                    if ((ospvType >= OSPC_OPNAME_START) && (ospvType < OSPC_OPNAME_NUMBER)) {
                        if (ospvSizeOfName > OSPM_STRLEN(dest->OpName[ospvType])) {
                            OSPM_STRNCPY(ospvName, dest->OpName[ospvType], ospvSizeOfName);
                        } else {
                            errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                            OSPM_DBGERRORLOG(errcode, "No enough buffer to copy operator name.");
                        }
                    } else {
                        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                        OSPM_DBGERRORLOG(errcode, "Invalid operator name type.");
                    }
                }
            } else {
                errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
            }
        } else if (trans->AuthInd != OSPC_OSNULL) {
            /* We are the destination.  Get the information from the AuthInd structure. */
            if ((dest = trans->AuthInd->Destination) == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
            } else {
                if ((ospvType >= OSPC_OPNAME_START) && (ospvType < OSPC_OPNAME_NUMBER)) {
                    if (ospvSizeOfName > OSPM_STRLEN(dest->OpName[ospvType])) {
                        OSPM_STRNCPY(ospvName, dest->OpName[ospvType], ospvSizeOfName);
                    } else {
                        errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errcode, "No enough buffer to copy operator name.");
                    }
                } else {
                    errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                    OSPM_DBGERRORLOG(errcode, "Invalid operator name type.");
                }
            }
        } else {
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
        }
    }

    return errcode;
}

int OSPPTransactionSetTwoWayDelay(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    int ospvSamples,                /* In - Samples of Round Trip Delay, -1 means unavailable */
    int ospvMin,                    /* In - Minimum of Round Trip Delay in milliseconds, -1 means unavailable */
    int ospvMax,                    /* In - Maximum of Round Trip Delay in milliseconds, -1 means unavailable */
    int ospvMean,                   /* In - Mean of Round Trip Delay in milliseconds, -1 means unavailable */
    float ospvVariance)             /* In - Variance of Round Trip delay, -1 means unavailable */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* if no statistics structure, make one */
        if (trans->Statistics == OSPC_OSNULL) {
            trans->Statistics = OSPPStatsNew();
            if (trans->Statistics == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_STATS_NEW_FAIL;
            }
        }

        if (errcode == OSPC_ERR_NO_ERROR) {
            if (ospvSamples >= 0) {
                trans->Statistics->TwoWay.hasvalue |= OSPC_SVALUE_SAMPLES;
                trans->Statistics->TwoWay.samples = ospvSamples;
            }
            if (ospvMin >= 0) {
                trans->Statistics->TwoWay.hasvalue |= OSPC_SVALUE_MINIMUM;
                trans->Statistics->TwoWay.minimum = ospvMin;
            }
            if (ospvMax >= 0) {
                trans->Statistics->TwoWay.hasvalue |= OSPC_SVALUE_MAXIMUM;
                trans->Statistics->TwoWay.maximum = ospvMax;
            }
            if (ospvMean >= 0) {
                trans->Statistics->TwoWay.hasvalue |= OSPC_SVALUE_MEAN;
                trans->Statistics->TwoWay.mean = ospvMean;
            }
            if (ospvVariance >= 0) {
                trans->Statistics->TwoWay.hasvalue |= OSPC_SVALUE_VARIANCE;
                trans->Statistics->TwoWay.variance = ospvVariance;
            }
        }
    }

    return errcode;
}

int OSPPTransactionSetSrcRealm(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvSrcRealm)       /* In - Source realm name */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvSrcRealm == OSPC_OSNULL) || (ospvSrcRealm[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
               OSPM_STRNCPY(trans->SrcRealm, ospvSrcRealm, sizeof(trans->SrcRealm) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetDestRealm(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvDestRealm)      /* In - Destiantion realm name */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvDestRealm == OSPC_OSNULL) || (ospvDestRealm[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
               OSPM_STRNCPY(trans->DestRealm, ospvDestRealm, sizeof(trans->DestRealm) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionGetURL(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_URL_TYPE ospvType,         /* In - URL type */
    unsigned ospvSizeOfURL,         /* In - Max size of name */
    char *ospvURL)                  /* Out - URL */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    ospvURL[0] = '\0';
    if ((trans = OSPPTransactionGetContext(ospvTransaction, &errcode)) != OSPC_OSNULL) {
        if (trans->AuthReq != OSPC_OSNULL) {
            /* We are the source.  Get the information from the destination structure. */
            if (trans->State == OSPC_GET_DEST_SUCCESS) {
                if ((dest = trans->CurrentDest) == OSPC_OSNULL) {
                    errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                    OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
                } else {
                    if ((ospvType >= OSPC_URL_START) && (ospvType < OSPC_URL_NUMBER)) {
                        if (ospvSizeOfURL > OSPM_STRLEN(dest->URL[ospvType])) {
                            OSPM_STRNCPY(ospvURL, dest->URL[ospvType], ospvSizeOfURL);
                        } else {
                            errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                            OSPM_DBGERRORLOG(errcode, "No enough buffer to copy URL.");
                        }
                    } else {
                        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                        OSPM_DBGERRORLOG(errcode, "Invalid URL type.");
                    }
                }
            } else {
                errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
            }
        } else if (trans->AuthInd != OSPC_OSNULL) {
            /* We are the destination.  Get the information from the AuthInd structure. */
            if ((dest = trans->AuthInd->Destination) == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
            } else {
                if ((ospvType >= OSPC_URL_START) && (ospvType < OSPC_URL_NUMBER)) {
                    if (ospvSizeOfURL > OSPM_STRLEN(dest->URL[ospvType])) {
                        OSPM_STRNCPY(ospvURL, dest->URL[ospvType], ospvSizeOfURL);
                    } else {
                        errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errcode, "No enough buffer to copy URL.");
                    }
                } else {
                    errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
                    OSPM_DBGERRORLOG(errcode, "Invalid URL type.");
                }
            }
        } else {
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
        }
    }

    return errcode;
}

int OSPPTransactionSetFrom(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_NUMBER_FORMAT ospvFormat,  /* In - From format */
    const char *ospvFrom)           /* In - From */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvFormat < OSPC_NFORMAT_START) || (ospvFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvFrom == OSPC_OSNULL) || (ospvFrom[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->From[ospvFormat], ospvFrom, sizeof(trans->From[ospvFormat]) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetAssertedId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_NUMBER_FORMAT ospvFormat,  /* In - Asserted ID format */
    const char *ospvAssertedId)     /* In - Asserted ID */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvFormat < OSPC_NFORMAT_START) || (ospvFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvAssertedId == OSPC_OSNULL) || (ospvAssertedId[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->AssertedId[ospvFormat], ospvAssertedId, sizeof(trans->AssertedId[ospvFormat]) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetRemotePartyId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_NUMBER_FORMAT ospvFormat,  /* In - Remote party ID format */
    const char *ospvRPId)           /* In - Remote party ID */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvFormat < OSPC_NFORMAT_START) || (ospvFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvRPId == OSPC_OSNULL) || (ospvRPId[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->RemotePartyId[ospvFormat], ospvRPId, sizeof(trans->RemotePartyId[ospvFormat]) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetChargeInfo(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_NUMBER_FORMAT ospvFormat,  /* In - Charge info format */
    const char *ospvChargeInfo)     /* In - Charge info */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvFormat < OSPC_NFORMAT_START) || (ospvFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvChargeInfo == OSPC_OSNULL) || (ospvChargeInfo[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->ChargeInfo[ospvFormat], ospvChargeInfo, sizeof(trans->ChargeInfo[ospvFormat]) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetDivSrcInfo(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_NUMBER_FORMAT ospvFormat,  /* In - Diversion format */
    const char *ospvSrcInfo)        /* In - Diversion */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvFormat < OSPC_NFORMAT_START) || (ospvFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvSrcInfo == OSPC_OSNULL) || (ospvSrcInfo[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->DivSrcInfo[ospvFormat], ospvSrcInfo, sizeof(trans->DivSrcInfo[ospvFormat]) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetApplicationId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvApplId)         /* In - Application ID */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvApplId == OSPC_OSNULL) || (ospvApplId[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
           OSPM_STRNCPY(trans->ApplicationId, ospvApplId, sizeof(trans->ApplicationId) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetRoleInfo(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_ROLE_STATE ospvState,      /* In - Role state */
    OSPE_ROLE_FORMAT ospvFormat,    /* In - Role format */
    OSPE_ROLE_VENDOR ospvVendor)    /* In - Role vendor */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
        if ((ospvState >= OSPC_RSTATE_START) && (ospvState < OSPC_RSTATE_NUMBER)) {
            trans->RoleState = ospvState;
        }

        if ((ospvFormat >= OSPC_RFORMAT_START) && (ospvFormat < OSPC_RFORMAT_NUMBER)) {
            trans->RoleFormat = ospvFormat;
        }

        if ((ospvVendor >= OSPC_RVENDOR_START) && (ospvVendor < OSPC_RVENDOR_NUMBER)) {
            trans->RoleVendor = ospvVendor;
        }
    }

    return errcode;
}

int OSPPTransactionSetCallPartyInfo(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_CALL_PARTY ospvParty,      /* In - Call party */
    const char *ospvUserName,       /* In - User name */
    const char *ospvUserId,         /* In - User ID */
    const char *ospvUserGroup)      /* In - User group */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if ((ospvParty < OSPC_CPARTY_START) || (ospvParty >= OSPC_CPARTY_NUMBER)) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if (errcode == OSPC_ERR_NO_ERROR) {
            if ((trans != OSPC_OSNULL)) {
                if (ospvParty == OSPC_CPARTY_SOURCE) {
                    if (ospvUserName != OSPC_OSNULL) {
                        OSPM_STRNCPY(trans->CallingParty.UserName, ospvUserName, sizeof(trans->CallingParty.UserName) - 1);
                    }
                    if (ospvUserId != OSPC_OSNULL) {
                        OSPM_STRNCPY(trans->CallingParty.UserId, ospvUserId, sizeof(trans->CallingParty.UserId) - 1);
                    }
                    if (ospvUserGroup != OSPC_OSNULL) {
                        OSPM_STRNCPY(trans->CallingParty.UserGroup, ospvUserGroup, sizeof(trans->CallingParty.UserGroup) - 1);
                    }
                } else if ((ospvParty == OSPC_CPARTY_DESTINATION)&& (trans->AuthReq != OSPC_OSNULL) && ((dest = trans->CurrentDest) != OSPC_OSNULL)) {
                    if (ospvUserName != OSPC_OSNULL) {
                        OSPM_STRNCPY(dest->CalledParty.UserName, ospvUserName, sizeof(dest->CalledParty.UserName) - 1);
                    }
                    if (ospvUserId != OSPC_OSNULL) {
                        OSPM_STRNCPY(dest->CalledParty.UserId, ospvUserId, sizeof(dest->CalledParty.UserId) - 1);
                    }
                    if (ospvUserGroup != OSPC_OSNULL) {
                        OSPM_STRNCPY(dest->CalledParty.UserGroup, ospvUserGroup, sizeof(dest->CalledParty.UserGroup) - 1);
                    }
                }
            }
        }
    }

    return errcode;
}

int OSPPTransactionSetTransferId(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvTransferId)     /* In - Transfer ID  */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) &&
        (trans != OSPC_OSNULL) &&
        (trans->AuthReq != OSPC_OSNULL) &&
        ((dest = trans->CurrentDest) != OSPC_OSNULL))
    {
        if (ospvTransferId != OSPC_OSNULL) {
            OSPM_STRNCPY(dest->TransferId, ospvTransferId, sizeof(dest->TransferId) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetTransferStatus(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    OSPE_TRANSFER_STATUS ospvStatus)    /* In - Transfer status  */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) &&
        (trans != OSPC_OSNULL) &&
        (trans->AuthReq != OSPC_OSNULL) &&
        ((dest = trans->CurrentDest) != OSPC_OSNULL))
    {
        if ((ospvStatus >= OSPC_TSTATUS_START) && (ospvStatus < OSPC_TSTATUS_NUMBER)) {
            dest->TransferStatus = ospvStatus;
        }
    }

    return errcode;
}

int OSPPTransactionSetNetworkTranslatedCalledNumber(
    OSPTTRANHANDLE ospvTransaction,             /* In - Transaction handle */
    OSPE_NUMBER_FORMAT ospvFormat,              /* In - Network translated called number format */
    const char *ospvNetworkTranslatedCalled)    /* In - Network translated called number */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (((ospvFormat < OSPC_NFORMAT_START) || (ospvFormat >= OSPC_NFORMAT_NUMBER)) ||
        ((ospvNetworkTranslatedCalled == OSPC_OSNULL) || (ospvNetworkTranslatedCalled[0] == '\0')))
    {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            trans->NetworkTranslatedCalledFormat = ospvFormat;
            OSPM_STRNCPY(trans->NetworkTranslatedCalled, ospvNetworkTranslatedCalled, sizeof(trans->NetworkTranslatedCalled) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetServiceProvider(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvServiceProvider)    /* In - Service provider */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvServiceProvider == OSPC_OSNULL) || (ospvServiceProvider[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
           OSPM_STRNCPY(trans->ServiceProvider, ospvServiceProvider, sizeof(trans->ServiceProvider) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetSystemId(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvSystemId)           /* In - System ID */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvSystemId == OSPC_OSNULL) || (ospvSystemId[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
           OSPM_STRNCPY(trans->SystemId, ospvSystemId, sizeof(trans->SystemId) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetRelatedReason(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvRelatedReason)      /* In - Related reason */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvRelatedReason == OSPC_OSNULL) || (ospvRelatedReason[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->RelatedReason, ospvRelatedReason, sizeof(trans->RelatedReason) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetSetupAttempt(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    int ospvAttempt)                    /* In - Setup attempt */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) && (trans->AuthReq != OSPC_OSNULL) && (trans->CurrentDest != OSPC_OSNULL)) {
        OSPPDestSetSetupAttempt(trans->CurrentDest, ospvAttempt);
    } else {
        errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
        OSPM_DBGERRORLOG(errcode,
            "OSPPTransactionSetSetupAttempt should be called after OSPPTransactionBuildUsageFromScratch for the source CDR");
    }

    return errcode;
}

int OSPPTransactionSetTotalSetupAttempts(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    int ospvTotal)                      /* In - Total setup attempts */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if (ospvTotal < 0) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            trans->TotalSetupAttempts = ospvTotal;
        }
    }

    return errcode;
}

int OSPPTransactionSetCDRProxy(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvHost,           /* In - CDR proxy host */
    const char *ospvFolder,         /* In - CDR proxy folder */
    const char *ospvSubfolder)      /* In - CDR proxy subfolder */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans;

    if (((ospvHost == OSPC_OSNULL) || (ospvHost[0] == '\0')) && ((ospvFolder == OSPC_OSNULL) || (ospvFolder[0] == '\0')) && ((ospvSubfolder == OSPC_OSNULL) || (ospvSubfolder[0] == '\0'))) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            if ((ospvHost != OSPC_OSNULL) && (ospvHost[0] != '\0')) {
                OSPM_STRNCPY(trans->CDRProxyHost, ospvHost, sizeof(trans->CDRProxyHost) - 1);
            }

            if ((ospvFolder != OSPC_OSNULL) && (ospvFolder[0] != '\0')) {
                OSPM_STRNCPY(trans->CDRProxyFolder, ospvFolder, sizeof(trans->CDRProxyFolder) - 1);
            }

            if ((ospvSubfolder != OSPC_OSNULL) && (ospvSubfolder[0] != '\0')) {
                OSPM_STRNCPY(trans->CDRProxySubfolder, ospvSubfolder, sizeof(trans->CDRProxySubfolder) - 1);
            }
        }
    }

    return errcode;
}

int OSPPTransactionSetUserAgent(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvUserAgent)          /* In - User-Agent */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvUserAgent == OSPC_OSNULL) || (ospvUserAgent[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->UserAgent, ospvUserAgent, sizeof(trans->UserAgent) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetSrcAudioAddr(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvSrcAudioAddr)       /* In - Source audio address */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvSrcAudioAddr == OSPC_OSNULL) || (ospvSrcAudioAddr[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->SrcAudioAddr, ospvSrcAudioAddr, sizeof(trans->SrcAudioAddr) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetSrcVideoAddr(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvSrcVideoAddr)       /* In - Source video address */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvSrcVideoAddr == OSPC_OSNULL) || (ospvSrcVideoAddr[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->SrcVideoAddr, ospvSrcVideoAddr, sizeof(trans->SrcVideoAddr) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetDestAudioAddr(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvDestAudioAddr)      /* In - Destination audio address */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if ((ospvDestAudioAddr == OSPC_OSNULL) || (ospvDestAudioAddr[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) &&
            (trans != OSPC_OSNULL) &&
            (trans->AuthReq != OSPC_OSNULL) &&
            ((dest = trans->CurrentDest) != OSPC_OSNULL))
        {
            OSPM_STRNCPY(dest->DestAudioAddr, ospvDestAudioAddr, sizeof(dest->DestAudioAddr) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetDestVideoAddr(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvDestVideoAddr)      /* In - Destination video address */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if ((ospvDestVideoAddr == OSPC_OSNULL) || (ospvDestVideoAddr[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) &&
            (trans != OSPC_OSNULL) &&
            (trans->AuthReq != OSPC_OSNULL) &&
            ((dest = trans->CurrentDest) != OSPC_OSNULL))
        {
            OSPM_STRNCPY(dest->DestVideoAddr, ospvDestVideoAddr, sizeof(dest->DestVideoAddr) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetProxyIngressAddr(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvProxyIngressAddr)   /* In - Proxy ingress address */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvProxyIngressAddr == OSPC_OSNULL) || (ospvProxyIngressAddr[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->ProxyIngressAddr, ospvProxyIngressAddr, sizeof(trans->ProxyIngressAddr) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetProxyEgressAddr(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvProxyEgressAddr)    /* In - Proxy ingress address */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    if ((ospvProxyEgressAddr == OSPC_OSNULL) || (ospvProxyEgressAddr[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) &&
            (trans != OSPC_OSNULL) &&
            (trans->AuthReq != OSPC_OSNULL) &&
            ((dest = trans->CurrentDest) != OSPC_OSNULL))
        {
            OSPM_STRNCPY(dest->ProxyEgressAddr, ospvProxyEgressAddr, sizeof(dest->ProxyEgressAddr) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetProviderPDD(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    int ospvProviderPDD)                /* In - Provider post dial delay in milliseconds */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) &&
        (trans != OSPC_OSNULL) &&
        (trans->AuthReq != OSPC_OSNULL) &&
        ((dest = trans->CurrentDest) != OSPC_OSNULL) &&
        (ospvProviderPDD >= 0))
    {
        dest->ProviderPDD = ospvProviderPDD;
    }

    return errcode;
}

int OSPPTransactionSetJIP(
    OSPTTRANHANDLE ospvTransaction,     /* In - Transaction handle */
    const char *ospvJIP)                /* In - Jurisdiction Information Parameter */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;

    if ((ospvJIP == OSPC_OSNULL) || (ospvJIP[0] == '\0')) {
        errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
    } else {
        trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
        if ((errcode == OSPC_ERR_NO_ERROR) && (trans != OSPC_OSNULL)) {
            OSPM_STRNCPY(trans->JIP, ospvJIP, sizeof(trans->JIP) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionGetCNAM(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    unsigned ospvSizeOfCNAM,        /* In - Max size of CNAM */
    char *ospvCNAM)                 /* Out - CNAM */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    ospvCNAM[0] = '\0';
    if ((trans = OSPPTransactionGetContext(ospvTransaction, &errcode)) != OSPC_OSNULL) {
        if (trans->AuthReq != OSPC_OSNULL) {
            /* We are the source.  Get the information from the destination structure. */
            if (trans->State == OSPC_GET_DEST_SUCCESS) {
                if ((dest = trans->CurrentDest) == OSPC_OSNULL) {
                    errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                    OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
                } else {
                    if (ospvSizeOfCNAM > OSPM_STRLEN(dest->CNAM)) {
                        OSPM_STRNCPY(ospvCNAM, dest->CNAM, ospvSizeOfCNAM);
                    } else {
                        errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                        OSPM_DBGERRORLOG(errcode, "No enough buffer to copy CNAM.");
                    }
                }
            } else {
                errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
            }
        } else if (trans->AuthInd != OSPC_OSNULL) {
            /* We are the destination.  Get the information from the AuthInd structure. */
            if ((dest = trans->AuthInd->Destination) == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
            } else {
                if (ospvSizeOfCNAM > OSPM_STRLEN(dest->CNAM)) {
                    OSPM_STRNCPY(ospvCNAM, dest->CNAM, ospvSizeOfCNAM);
                } else {
                    errcode = OSPC_ERR_TRAN_NOT_ENOUGH_SPACE_FOR_COPY;
                    OSPM_DBGERRORLOG(errcode, "No enough buffer to copy CNAM.");
                }
            }
        } else {
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
        }
    }

    return errcode;
}

int OSPPTransactionGetServiceType(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    OSPE_SERVICE *ospvServiceType)  /* Out - Service type */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    *ospvServiceType = OSPC_SERVICE_UNKNOWN;
    if ((trans = OSPPTransactionGetContext(ospvTransaction, &errcode)) != OSPC_OSNULL) {
        if (trans->AuthReq != OSPC_OSNULL) {
            /* We are the source.  Get the information from the destination structure. */
            if (trans->State == OSPC_GET_DEST_SUCCESS) {
                if ((dest = trans->CurrentDest) == OSPC_OSNULL) {
                    errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                    OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
                } else {
                    *ospvServiceType = dest->ServiceType;
                }
            } else {
                errcode = OSPC_ERR_TRAN_REQ_OUT_OF_SEQ;
                OSPM_DBGERRORLOG(errcode, "Called API Not In Sequence\n");
            }
        } else if (trans->AuthInd != OSPC_OSNULL) {
            /* We are the destination.  Get the information from the AuthInd structure. */
            if ((dest = trans->AuthInd->Destination) == OSPC_OSNULL) {
                errcode = OSPC_ERR_TRAN_DEST_NOT_FOUND;
                OSPM_DBGERRORLOG(errcode, "Could not find Destination for this Transaction\n");
            } else {
                *ospvServiceType = dest->ServiceType;
            }
        } else {
            errcode = OSPC_ERR_TRAN_INVALID_ENTRY;
            OSPM_DBGERRORLOG(errcode, "No information available to process this report.");
        }
    }

    return errcode;
}

int OSPPTransactionSetCallType(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvCallType)       /* In - Call type  */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) &&
        (trans != OSPC_OSNULL) &&
        (trans->AuthReq != OSPC_OSNULL) &&
        ((dest = trans->CurrentDest) != OSPC_OSNULL))
    {
        if (ospvCallType != OSPC_OSNULL) {
            OSPM_STRNCPY(dest->CallType, ospvCallType, sizeof(dest->CallType) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetCallCategory(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvCallCategory)       /* In - Call type  */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) &&
        (trans != OSPC_OSNULL) &&
        (trans->AuthReq != OSPC_OSNULL) &&
        ((dest = trans->CurrentDest) != OSPC_OSNULL))
    {
        if (ospvCallCategory != OSPC_OSNULL) {
            OSPM_STRNCPY(dest->CallCategory, ospvCallCategory, sizeof(dest->CallCategory) - 1);
        }
    }

    return errcode;
}

int OSPPTransactionSetNetworkType(
    OSPTTRANHANDLE ospvTransaction, /* In - Transaction handle */
    const char *ospvNetworkType)       /* In - Call type  */
{
    int errcode = OSPC_ERR_NO_ERROR;
    OSPTTRANS *trans = OSPC_OSNULL;
    OSPT_DEST *dest = OSPC_OSNULL;

    trans = OSPPTransactionGetContext(ospvTransaction, &errcode);
    if ((errcode == OSPC_ERR_NO_ERROR) &&
        (trans != OSPC_OSNULL) &&
        (trans->AuthReq != OSPC_OSNULL) &&
        ((dest = trans->CurrentDest) != OSPC_OSNULL))
    {
        if (ospvNetworkType != OSPC_OSNULL) {
            OSPM_STRNCPY(dest->NetworkType, ospvNetworkType, sizeof(dest->NetworkType) - 1);
        }
    }

    return errcode;
}
