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
 *  ospaudit.h 
 */

#ifndef _OSPAUDIT_H
#define _OSPAUDIT_H

#include "osp/osp.h"
#include "osp/ospbfr.h"
#include "osp/ospcomm.h"
#include "osp/ospsecurity.h"

typedef  struct _OSPTAUDIT
{
    OSPTMUTEX       ospmAuditWorkerMutex;
    OSPTBOOL        ospmAuditWorkerHasMutex;
    OSPTMUTEX       ospmAuditAccessMutex;
    OSPTBOOL        ospmAuditAccessHasMutex;
    OSPTTHREADID    ospmAuditThreadId;
    OSPTCOMM        *ospmAuditComm;
    OSPTCONDVAR     ospmAuditWorkerCondVar;
    OSPTCONDVAR     ospmAuditAccessCondVar;
    OSPTSEC	        *ospmAuditSecurity;
    unsigned        ospmAuditUsedSpace;
    unsigned        ospmAuditMaxSpace;
    unsigned        ospmAuditNumMessages;
    unsigned        ospmAuditMaxMessages;
    OSPTBFR         *ospmAuditStorage;
    unsigned long   ospmAuditStartTime;
    unsigned long   ospmAuditMaxTime;
    char            *ospmAuditURL;
    unsigned char   ospmAuditFlags;         /* bit 1 - Flush Buffer - 0x01 
                                             * bit 2 - Buffer Empty - 0x02 */
    OSPTLIST        ospmAuditComponentIdList;
}OSPTAUDIT;

/*-------------------------------------------*/
/* ComponentId list item                     */
/*-------------------------------------------*/
typedef struct _OSPTCOMPONENTID
{
    OSPTLISTLINK    ospmComponentIdLink;
    unsigned char   *ospmComponentId;
}OSPTCOMPONENTID;

#define OSPC_AUDIT_FLUSH_BUFFER_NOW   0x01
#define OSPC_AUDIT_BUFFER_IS_EMPTY    0x02
#define OSPC_AUDIT_BUFFER_CLEAR_FLAGS 0x00

#define OSPC_AUDIT_XML_HEADER   OSPC_XMLDOC_DECL
#define OSPC_AUDIT_XML_HDR_LEN  OSPC_XMLDOC_DECLLEN
#define OSPC_AUDIT_MSG_HEADER   "<Message messageId=\""
#define OSPC_AUDIT_MSG_RANDOM   " random=\""
#define OSPC_AUDIT_MSG_HDR_LEN  (sizeof(OSPC_AUDIT_MSG_HEADER)-1)
#define OSPC_AUDIT_MSG_RANDOM_LEN   (sizeof(OSPC_AUDIT_MSG_RANDOM)-1)
#define OSPC_AUDIT_TAG_END      OSPC_XMLDOC_ELEMENTDECLEND
#define OSPC_AUDIT_TAG_END_LEN  OSPC_XMLDOC_ELEMENTDECLENDLEN
#define OSPC_AUDIT_BFR_END      "</Message>"
#define OSPC_AUDIT_BFR_END_LEN  (sizeof(OSPC_AUDIT_BFR_END)-1)


    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

int         OSPPAuditAddMessageToBuffer(OSPTAUDIT *ospvAudit, 
                                        unsigned char *ospvAuditData, 
                                        unsigned ospvAuditDataSz);
void        OSPPAuditCheck(OSPTAUDIT *, void *, OSPE_MSG_DATATYPES);

OSPTCOMPONENTID *OSPPAuditComponentIdNew(unsigned char *ospvComponentId, 
                                         int ospvComponentIdLen);

void        OSPPAuditComponentIdDelete(OSPTAUDIT *ospvAudit);

int         OSPPAuditDelete(OSPTAUDIT	**ospvAudit);

OSPTCONDVAR OSPPAuditGetAccessCondVar(OSPTAUDIT *ospvAudit);

unsigned    OSPPAuditGetMaxMessages(OSPTAUDIT *ospvAudit);
unsigned    OSPPAuditGetMaxSpace(OSPTAUDIT *ospvAudit);

unsigned    OSPPAuditGetMaxTime(OSPTAUDIT *ospvAudit);
unsigned long   OSPPAuditGetStartTime(OSPTAUDIT *ospvAudit);

const char  *OSPPAuditGetURL(OSPTAUDIT *ospvAudit);

OSPTCONDVAR OSPPAuditGetWorkerCondVar(OSPTAUDIT *ospvAudit);
void        OSPPAuditIncrementUsedSpace(OSPTAUDIT *ospvAudit, unsigned ospvIncrement);
int         OSPPAuditInit(OSPTAUDIT *ospvAudit);
void        OSPPAuditInitializeBuffer(OSPTAUDIT   *ospvAudit);
OSPTAUDIT   *OSPPAuditNew(const char *ospvAuditURL);
int         OSPPAuditPrepareAndSend(OSPTAUDIT *ospvAudit);

int         OSPPAuditProcessReturn(OSPTAUDIT *ospvAudit, OSPTMSGINFO *ospvMsgInfo);
void        OSPPAuditRemoveComponentIdFromList(unsigned char   *ospvCompid,
                                               OSPTAUDIT       *ospvAudit);
int         OSPPAuditResetDefaults(OSPTAUDIT *ospvAudit);

void        OSPPAuditSetComm(OSPTAUDIT *ospvAudit, OSPTCOMM *ospvComm);
void        OSPPAuditSetMaxMessages(OSPTAUDIT *ospvAudit, unsigned ospvNumMessages);
void        OSPPAuditSetMaxSpace(OSPTAUDIT *ospvAudit, unsigned ospvMaxSpace);
void        OSPPAuditSetMaxTime(OSPTAUDIT *ospvAudit, unsigned ospvMaxTime);
void        OSPPAuditSetNumMessages(OSPTAUDIT *ospvAudit, unsigned ospvNumMsgs);

void        OSPPAuditSetSecurity(OSPTAUDIT *ospvAudit, OSPTSEC *ospvSecurity);
void        OSPPAuditSetStartTime(OSPTAUDIT *ospvAudit, unsigned long ospvTime);

void        OSPPAuditSetURL(OSPTAUDIT	*ospvAudit, const char *ospvAuditURL);
void        OSPPAuditSetUsedSpace(OSPTAUDIT *ospvAudit, unsigned ospvUsedSpace);

int         OSPPAuditStartWorker(OSPTAUDIT *ospvAudit);
void        OSPPAuditVerifyUsageCnf(OSPTUSAGECNF *ospvUsageCnf, OSPTAUDIT *ospvAudit);

#ifdef __cplusplus
}
#endif

#endif
