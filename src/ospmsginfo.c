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
 * ospmsginfo.c
 */
#include "osp/osp.h"
#include "osp/ospmsginfo.h"

static
int
osppMsgInfoInitSync(
    OSPTMSGINFO *ospvMsgInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR,
        tmperror  = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: osppMsgInfoInitSync()\n"));
    OSPM_MUTEX_INIT(ospvMsgInfo->Mutex, NULL, errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        OSPM_CONDVAR_INIT(ospvMsgInfo->CondVar, NULL, errorcode);
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            OSPM_MUTEX_DESTROY(ospvMsgInfo->Mutex, tmperror);
        }
    }
    OSPM_DBGEXIT(("EXIT : osppMsgInfoInitSync()\n"));
    return errorcode;
}

int
OSPPMsgInfoNew(
    OSPTMSGINFO **ospvMsgInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPMsgInfoNew()\n"));
    OSPM_MALLOC(*ospvMsgInfo, OSPTMSGINFO, sizeof(OSPTMSGINFO));
    if (*ospvMsgInfo != (OSPTMSGINFO *)OSPC_OSNULL)
    {
        OSPM_MEMSET(*ospvMsgInfo, 0, sizeof(OSPTMSGINFO));

        errorcode = osppMsgInfoInitSync(*ospvMsgInfo);
        if (errorcode != OSPC_ERR_NO_ERROR)
        {
            OSPM_FREE(*ospvMsgInfo);
            *ospvMsgInfo = (OSPTMSGINFO *)OSPC_OSNULL;
        }
    }
    OSPM_DBGEXIT(("EXIT : OSPPMsgInfoNew()\n"));
    return errorcode;
}

void
OSPPMsgInfoDelete(
    OSPTMSGINFO **ospvMsgInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPMsgInfoDelete()\n"));
    if (*ospvMsgInfo)
    {
        if ((*ospvMsgInfo)->ResponseMsg != (unsigned char *)OSPC_OSNULL)
            OSPM_FREE((*ospvMsgInfo)->ResponseMsg);

        if ((*ospvMsgInfo)->RequestMsg != (unsigned char *)OSPC_OSNULL)
            OSPM_FREE((*ospvMsgInfo)->RequestMsg);

        if ((*ospvMsgInfo)->ContentType != (unsigned char *)OSPC_OSNULL)
            OSPM_FREE((*ospvMsgInfo)->ContentType);


        OSPM_MUTEX_DESTROY((*ospvMsgInfo)->Mutex, errorcode);

        OSPM_CONDVAR_DESTROY((*ospvMsgInfo)->CondVar, errorcode);

        OSPM_FREE(*ospvMsgInfo);
        *ospvMsgInfo = (OSPTMSGINFO *)OSPC_OSNULL;
    }
    OSPM_DBGEXIT(("EXIT : OSPPMsgInfoDelete()\n"));
    return;
}

void
OSPPMsgInfoAssignRequestMsg(
    OSPTMSGINFO   *ospvMsgInfo,
    unsigned char *ospvRequestMsg,
    unsigned      ospvRequestSz)
{
    OSPM_DBGENTER(("ENTER: OSPPMsgInfoAssignRequestMsg()\n"));
    if (ospvMsgInfo != (OSPTMSGINFO *)OSPC_OSNULL) 
    {

        ospvMsgInfo->RequestMsg = ospvRequestMsg;
        ospvMsgInfo->RequestSz  = ospvRequestSz;
    }
    OSPM_DBGEXIT(("EXIT : OSPPMsgInfoAssignRequestMsg()\n"));
    return;
}

int
OSPPMsgInfoProcessResponse(
    OSPTMSGINFO *ospvMsgInfo)
{
    int errorcode  = OSPC_ERR_NO_ERROR,
        tmperrcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPMsgInfoProcessResponse()\n"));

    if (ospvMsgInfo->IsNonBlocking == OSPC_FALSE)
    {
        /* 
         * once signalled, acquire the mutex for the individual
         * transaction.
         */
        OSPM_MUTEX_LOCK(ospvMsgInfo->Mutex, errorcode);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            ospvMsgInfo->HasBeenProcessed = OSPC_TRUE;
            OSPM_CONDVAR_SIGNAL(ospvMsgInfo->CondVar, errorcode);
            OSPM_MUTEX_UNLOCK(ospvMsgInfo->Mutex, tmperrcode);
        }
    }
    else
    {
        OSPPMsgInfoDelete(&ospvMsgInfo);
    }


    OSPM_DBGEXIT(("EXIT : OSPPMsgInfoProcessResponse()\n"));
    return errorcode;
}

int
OSPPMsgInfoWaitForMsg(
    OSPTMSGINFO *ospvMsgInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_DBGENTER(("ENTER: OSPPMsgInfoWaitForMsg()\n"));

    OSPM_MUTEX_LOCK(ospvMsgInfo->Mutex, errorcode);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
       while (ospvMsgInfo->HasBeenProcessed == OSPC_FALSE)
       {
           OSPM_CONDVAR_WAIT(ospvMsgInfo->CondVar, ospvMsgInfo->Mutex,
                    errorcode);
       }

            OSPM_MUTEX_UNLOCK(ospvMsgInfo->Mutex, errorcode);
    }
    OSPM_DBGEXIT(("EXIT : OSPPMsgInfoWaitForMsg()\n"));
    return errorcode;
}
