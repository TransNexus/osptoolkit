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

/* ospaudit.h */
#ifndef _OSPAUDIT_H
#define _OSPAUDIT_H

#include "osp/osp.h"
#include "osp/ospbfr.h"
#include "osp/ospcomm.h"
#include "osp/ospsecurity.h"

typedef struct {
    OSPTMUTEX ospmAuditWorkerMutex;
    OSPTBOOL ospmAuditWorkerHasMutex;
    OSPTMUTEX ospmAuditAccessMutex;
    OSPTBOOL ospmAuditAccessHasMutex;
    OSPTTHREADID ospmAuditThreadId;
    OSPTCOMM *ospmAuditComm;
    OSPTCONDVAR ospmAuditWorkerCondVar;
    OSPTCONDVAR ospmAuditAccessCondVar;
    OSPTSEC *ospmAuditSecurity;
    unsigned ospmAuditUsedSpace;
    unsigned ospmAuditMaxSpace;
    unsigned ospmAuditNumMessages;
    unsigned ospmAuditMaxMessages;
    OSPTBFR *ospmAuditStorage;
    unsigned long ospmAuditStartTime;
    unsigned long ospmAuditMaxTime;
    char *ospmAuditURL;
    /*
     * bit 1 - Flush Buffer - 0x01
     * bit 2 - Buffer Empty - 0x02
     */
    unsigned char ospmAuditFlags;
    OSPTLIST ospmAuditComponentIdList;
} OSPT_AUDIT;

/* ComponentId list item */
typedef struct {
    OSPTLISTLINK ospmComponentIdLink;
    char *ospmComponentId;
} OSPT_COMPONENT_ID;

#define OSPC_AUDIT_FLUSH_BUFFER_NOW     0x01
#define OSPC_AUDIT_BUFFER_IS_EMPTY      0x02
#define OSPC_AUDIT_BUFFER_CLEAR_FLAGS   0x00

#define OSPC_AUDIT_XML_HEADER           OSPC_XMLDOC_DECL
#define OSPC_AUDIT_XML_HDR_LEN          OSPC_XMLDOC_DECLLEN
#define OSPC_AUDIT_MSG_HEADER           "<Message messageId=\""
#define OSPC_AUDIT_MSG_RANDOM           " random=\""
#define OSPC_AUDIT_MSG_HDR_LEN          (sizeof(OSPC_AUDIT_MSG_HEADER)-1)
#define OSPC_AUDIT_MSG_RANDOM_LEN       (sizeof(OSPC_AUDIT_MSG_RANDOM)-1)
#define OSPC_AUDIT_TAG_END              OSPC_XMLDOC_ELEMENTDECLEND
#define OSPC_AUDIT_TAG_END_LEN          OSPC_XMLDOC_ELEMENTDECLENDLEN
#define OSPC_AUDIT_BFR_END              "</Message>"
#define OSPC_AUDIT_BFR_END_LEN          (sizeof(OSPC_AUDIT_BFR_END)-1)

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    int OSPPAuditAddMessageToBuffer(OSPT_AUDIT *ospvAudit, unsigned char *ospvAuditData, unsigned ospvAuditDataSz);
    void OSPPAuditCheck(OSPT_AUDIT *, void *, OSPE_MESSAGE);
    OSPT_COMPONENT_ID *OSPPAuditComponentIdNew(const char *ospvComponentId, unsigned ospvComponentIdLen);
    void OSPPAuditComponentIdDelete(OSPT_AUDIT *ospvAudit);
    int OSPPAuditDelete(OSPT_AUDIT **ospvAudit);
    OSPTCONDVAR OSPPAuditGetAccessCondVar(OSPT_AUDIT *ospvAudit);
    unsigned OSPPAuditGetMaxMessages(OSPT_AUDIT *ospvAudit);
    unsigned OSPPAuditGetMaxSpace(OSPT_AUDIT *ospvAudit);
    unsigned OSPPAuditGetMaxTime(OSPT_AUDIT *ospvAudit);
    unsigned long OSPPAuditGetStartTime(OSPT_AUDIT *ospvAudit);
    const char *OSPPAuditGetURL(OSPT_AUDIT *ospvAudit);
    OSPTCONDVAR OSPPAuditGetWorkerCondVar(OSPT_AUDIT *ospvAudit);
    void OSPPAuditIncrementUsedSpace(OSPT_AUDIT *ospvAudit, unsigned ospvIncrement);
    int OSPPAuditInit(OSPT_AUDIT *ospvAudit);
    void OSPPAuditInitializeBuffer(OSPT_AUDIT *ospvAudit);
    OSPT_AUDIT *OSPPAuditNew(const char *ospvAuditURL);
    int OSPPAuditPrepareAndSend(OSPT_AUDIT *ospvAudit);
    int OSPPAuditProcessReturn(OSPT_AUDIT *ospvAudit, OSPT_MSG_INFO *ospvMsgInfo);
    void OSPPAuditRemoveComponentIdFromList(const char *ospvCompid, OSPT_AUDIT *ospvAudit);
    int OSPPAuditResetDefaults(OSPT_AUDIT *ospvAudit);
    void OSPPAuditSetComm(OSPT_AUDIT *ospvAudit, OSPTCOMM *ospvComm);
    void OSPPAuditSetMaxMessages(OSPT_AUDIT *ospvAudit, unsigned ospvNumMessages);
    void OSPPAuditSetMaxSpace(OSPT_AUDIT *ospvAudit, unsigned ospvMaxSpace);
    void OSPPAuditSetMaxTime(OSPT_AUDIT *ospvAudit, unsigned ospvMaxTime);
    void OSPPAuditSetNumMessages(OSPT_AUDIT *ospvAudit, unsigned ospvNumMsgs);
    void OSPPAuditSetSecurity(OSPT_AUDIT *ospvAudit, OSPTSEC *ospvSecurity);
    void OSPPAuditSetStartTime(OSPT_AUDIT *ospvAudit, unsigned long ospvTime);
    void OSPPAuditSetURL(OSPT_AUDIT *ospvAudit, const char *ospvAuditURL);
    void OSPPAuditSetUsedSpace(OSPT_AUDIT *ospvAudit, unsigned ospvUsedSpace);
    int OSPPAuditStartWorker(OSPT_AUDIT *ospvAudit);
    void OSPPAuditVerifyUsageCnf(OSPTUSAGECNF *ospvUsageCnf, OSPT_AUDIT *ospvAudit);

#ifdef __cplusplus
}
#endif

#endif /* _OSPAUDIT_H */
