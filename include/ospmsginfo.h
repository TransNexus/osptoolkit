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







#ifndef _OSPMSGINFO_H
#define _OSPMSGINFO_H

#include "osp.h"

/*-------------------------------------------*/
/* message info typedef                      */
/*-------------------------------------------*/ 
typedef struct _OSPTMSGINFO 
{
    OSPTLISTLINK        Link;
    unsigned char       *RequestMsg;
    unsigned char       *ResponseMsg;
    unsigned char       *ContentType;
    unsigned            RequestSz;
    unsigned            ResponseSz;
    unsigned            ContentSz;
    OSPTMUTEX           Mutex;
    OSPTCONDVAR         CondVar;
    int                 ErrorCode;
    unsigned char       Flags;
    /*
     * By default the flag is set to False (memset to 0s in OSPPMsgInfoNew)
     * and does not alter current behavior.  However, when the flag is set to True
     * the following two functions should behave differently.
     * 
     * OSPPMsgInfoProcessResponse should delete the structure instead of
     * signaling that the response is ready for processing.
     * 
     * OSPPMsgInfoWaitForMsg should return immediately instead of waiting for
     * a signal from OSPPMsgInfoProcessResponse.
     */ 
    int                 IsNonBlocking;
} OSPTMSGINFO;

#define OSPC_MSGINFO_AUDIT_TYPE 0x01    /* bit 1 - Message type flag */

#ifdef __cplusplus
extern "C" 
{
#endif

    int  OSPPMsgInfoNew(OSPTMSGINFO **);
    void OSPPMsgInfoDelete(OSPTMSGINFO **);
    int  OSPPMsgInfoWaitForMsg(OSPTMSGINFO *);
    int  OSPPMsgInfoProcessResponse(OSPTMSGINFO *);
    void OSPPMsgInfoAssignRequestMsg(OSPTMSGINFO *, unsigned char *, unsigned);

#ifdef __cplusplus
}
#endif

#endif
