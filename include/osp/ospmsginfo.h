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

#include "osp/osp.h"

/* message info typedef */
typedef struct {
    OSPTLISTLINK Link;
    unsigned char *RequestMsg;
    unsigned char *ResponseMsg;
    unsigned char *ContentType;
    unsigned RequestSz;
    unsigned ResponseSz;
    unsigned ContentSz;
    OSPTMUTEX Mutex;
    OSPTCONDVAR CondVar;
    int ErrorCode;
    unsigned char Flags;
    /*
     * By default the flag is set to False (memset to 0s in OSPPMsgInfoNew)
     * and does not alter current behavior.  However, when the flag is set to True
     * then the OSPPMsgInfoProcessResponse function should delete the structure instead of
     * signaling that the response is ready for processing.
     *
     * If IsNonBlocking is set to FALSE,only then should we wait for a response back
     * by calling the OSPPMsgInfoWaitForMsg function.
     * If IsNonBlocking is set to TRUE, then we should never wait for a response because
     * in that case, OSPPMsgInfoProcessResponse function would delete the structure.
     */
    int IsNonBlocking;
    OSPTBOOL HasBeenProcessed;
    /*
     * Similar to IsNonBlocking.
     * By default the flag is set to False (memset to 0s in OSPPMsgInfoNew)
     * and does not alter current behavior.  However, when the flag is set to True
     * then the osppHttpBuildMsg function should use the HTTP Get (query in
     * the URI of the header) instead of POST (in the body of the message) for
     * sending the RequestMsg
     */
    OSPTBOOL UseGet;
} OSPT_MSG_INFO;

#define OSPC_MINFO_AUDITTYPE 0x01    /* bit 1 - Message type flag */

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    int OSPPMsgInfoNew(OSPT_MSG_INFO **);
    void OSPPMsgInfoDelete(OSPT_MSG_INFO **);
    int OSPPMsgInfoWaitForMsg(OSPT_MSG_INFO *);
    int OSPPMsgInfoProcessResponse(OSPT_MSG_INFO *);
    void OSPPMsgInfoAssignRequestMsg(OSPT_MSG_INFO *, unsigned char *, unsigned);

#ifdef __cplusplus
}
#endif

#endif /* _OSPMSGINFO_H */
