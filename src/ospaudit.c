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

/* ospaudit.c - OSP audit functions */

#include "osp/osp.h"
#include "osp/ospaudit.h"
#include "osp/ospconfig.h"
#include "osp/ospmime.h"
#include "osp/ospxml.h"
#include "osp/ospxmldoc.h"
#include "osp/osputils.h"

/* copy message data into audit buffer */
int OSPPAuditAddMessageToBuffer(
    OSPT_AUDIT *ospvAudit,
    unsigned char *ospvAuditData,
    unsigned ospvAuditDataSz)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned numbyteswritten = 0;
    unsigned char *dataptr = ospvAuditData;
    unsigned char *endptr = OSPC_OSNULL;
    unsigned char *begptr = OSPC_OSNULL;
    OSPT_COMPONENT_ID *compid = OSPC_OSNULL;

    if ((ospvAudit != OSPC_OSNULL) && (dataptr != OSPC_OSNULL) && (ospvAuditDataSz > 0)) {
        /* Clean buffer up */
        if (OSPM_STRSTR((const char *)dataptr, OSPC_AUDIT_XML_HEADER) != OSPC_OSNULL) {
            dataptr += OSPC_AUDIT_XML_HDR_LEN;
            ospvAuditDataSz -= OSPC_AUDIT_XML_HDR_LEN;
        }

        if (OSPM_STRSTR((const char *)dataptr, OSPC_AUDIT_MSG_HEADER) != OSPC_OSNULL) {
            dataptr += OSPC_AUDIT_MSG_HDR_LEN;
            ospvAuditDataSz -= OSPC_AUDIT_MSG_HDR_LEN;
        }

        if ((endptr = (unsigned char *)OSPM_STRSTR((const char *)dataptr, "<")) != OSPC_OSNULL) {
            ospvAuditDataSz -= (endptr - dataptr);
            dataptr = endptr;
        }

        if ((endptr = (unsigned char *)OSPM_STRSTR((const char *)dataptr, OSPC_AUDIT_BFR_END)) != OSPC_OSNULL) {
            ospvAuditDataSz -= (OSPC_AUDIT_BFR_END_LEN + 1);
        }

        /* get access to buffer */
        OSPM_MUTEX_LOCK(ospvAudit->AccessMutex, errorcode);

        if (errorcode == OSPC_ERR_NO_ERROR) {
            if (((ospvAudit->UsedSpace + ospvAuditDataSz) <= ospvAudit->MaxSpace) &&
                ((ospvAudit->NumMessages + 1) <= ospvAudit->MaxMessages))
            {
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* add data to buffer */
                    numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), dataptr, ospvAuditDataSz);
                    /* increment used space */
                    OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);

                    /* increment nummessages */
                    ospvAudit->NumMessages++;

                    /* Find componentId and save it */
                    if ((endptr = (unsigned char *)OSPM_STRSTR((const char *)dataptr, "componentId")) != OSPC_OSNULL) {
                        if ((endptr = (unsigned char *)OSPM_STRSTR((const char *)endptr, "=")) != OSPC_OSNULL) {
                            begptr = endptr + 1;
                            while (!isalnum(*begptr) && ispunct(*begptr))
                                begptr++;

                            if ((endptr = (unsigned char *)OSPM_STRSTR((const char *)begptr, "\"")) != OSPC_OSNULL) {
                                compid = OSPPAuditComponentIdNew((const char *)begptr, (endptr - begptr));
                                if (compid != OSPC_OSNULL) {
                                    OSPPListAppend(&(ospvAudit)->ComponentIdList, compid);
                                }
                            }
                        }
                    }
                }
            } else {
                /* signal worker thread to flush buffer */
                OSPM_MUTEX_LOCK(ospvAudit->WorkerMutex, errorcode);

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    /* signal condvar to send data */
                    ospvAudit->Flags |= OSPC_AUDIT_FLUSH_BUFFER_NOW;

                    OSPM_CONDVAR_SIGNAL(ospvAudit->WorkerCondVar, errorcode);
                    OSPM_MUTEX_UNLOCK(ospvAudit->WorkerMutex, errorcode);
                }
            }

            /* release mutex */
            OSPM_MUTEX_UNLOCK(ospvAudit->AccessMutex, errorcode);
        }
    }

    return errorcode;
}

/*
 * check audit structure. If new data found, replace data in audit structure.
 * If state indicates some action, start that action. If some error occurs, just
 * produce some debug.
 */
void OSPPAuditCheck(
    OSPT_AUDIT *ospvAudit,
    void *ospvResponse,
    OSPE_MESSAGE ospvMsgType)
{
    OSPT_TN_AUDIT *tnaudit = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTCSAUDIT *csaudit = OSPC_OSNULL;
    unsigned char *trigger = OSPC_OSNULL;

    if ((ospvAudit != OSPC_OSNULL) && (ospvResponse != OSPC_OSNULL)) {
        /* set up audit structure pointer */
        switch (ospvMsgType) {
        case OSPC_MSG_ARESP:
            if (((OSPT_AUTH_RSP *)ospvResponse)->TNAudit != OSPC_OSNULL) {
                tnaudit = OSPPAuthRspGetTNAudit((OSPT_AUTH_RSP *)ospvResponse);
            }

            if (((OSPT_AUTH_RSP *)ospvResponse)->CSAudit != OSPC_OSNULL) {
                csaudit = OSPPAuthRspGetCSAudit((OSPT_AUTH_RSP *)ospvResponse);
            }
            break;

        case OSPC_MSG_UCNF:
            if (((OSPT_USAGE_CNF *)ospvResponse)->TNAudit != OSPC_OSNULL) {
                tnaudit = OSPPUsageCnfGetTNAudit((OSPT_USAGE_CNF *)ospvResponse);
            }

            if (((OSPT_USAGE_CNF *)ospvResponse)->CSAudit != OSPC_OSNULL) {
                csaudit = OSPPUsageCnfGetCSAudit((OSPT_USAGE_CNF *)ospvResponse);
            }
            break;

        case OSPC_MSG_REARESP:
            if (((OSPT_REAUTH_RSP *)ospvResponse)->TNAudit != OSPC_OSNULL) {
                tnaudit = OSPPReauthRspGetTNAudit((OSPT_REAUTH_RSP *)ospvResponse);
            }

                /*
                if(((OSPTREAUTHRSP *)ospvResponse)->ospmReauthRspCSAudit != OSPC_OSNULL) {
                    csaudit = OSPPReauthRspGetCSAudit((OSPTREAUTHRSP *)ospvResponse);
                }
                */
            break;

        default:
            break;
        }

        /* check data in structure so we know what to do */
        if (tnaudit != OSPC_OSNULL) {
            if (OSPPTNAuditHasURL(tnaudit)) {
                OSPPAuditSetURL(ospvAudit, OSPPTNAuditGetURL(tnaudit));
                OSPPCommSetAuditURL(ospvAudit->Comm, OSPPTNAuditGetURL(tnaudit));
            }

            if (OSPPTNAuditHasTimeLimit(tnaudit)) {
                OSPPAuditSetMaxTime(ospvAudit, OSPPTNAuditGetTimeLimit(tnaudit));
            }

            if (OSPPTNAuditHasMaxMessages(tnaudit)) {
                OSPPAuditSetMaxMessages(ospvAudit, OSPPTNAuditGetMaxMessages(tnaudit));
            }

            if (OSPPTNAuditHasState(tnaudit)) {
                switch (OSPPTNAuditGetState(tnaudit)) {
                case OSPC_AUDIT_INIT:
                    /* If already on, don't turn on again */
                    if (!(ospvAudit->Comm->Flags & OSPC_COMM_AUDIT_ON)) {
                        errorcode = OSPPAuditInit(ospvAudit);
                        OSPPCommSetAuditFlag(ospvAudit->Comm, OSPC_COMM_AUDIT_ON);
                    }
                    break;

                case OSPC_AUDIT_FLUSH_BUFFER:
                    /* signal worker thread to flush buffer */
                    OSPM_MUTEX_LOCK(ospvAudit->WorkerMutex, errorcode);

                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        /* signal condvar to send data */
                        ospvAudit->Flags |= OSPC_AUDIT_FLUSH_BUFFER_NOW;

                        OSPM_CONDVAR_SIGNAL(ospvAudit->WorkerCondVar, errorcode);
                        OSPM_MUTEX_UNLOCK(ospvAudit->WorkerMutex, errorcode);
                    }
                    break;

                default:
                    break;
                }
            }
        }

        if (csaudit != OSPC_OSNULL) {
            if (OSPPCSAuditHasTrigger(csaudit)) {
                trigger = OSPPCSAuditGetTrigger(csaudit);
                if (OSPM_STRCMP((const char *)trigger, "stop") == 0) {
                    /* Unset auditing flag */
                    OSPPCommSetAuditFlag(ospvAudit->Comm, OSPC_COMM_AUDIT_OFF);

                    /* Check for stuff in buffer */
                    if (!(ospvAudit->Flags & OSPC_AUDIT_BUFFER_IS_EMPTY)) {

                        /* signal worker thread to flush buffer */
                        OSPM_MUTEX_LOCK(ospvAudit->WorkerMutex, errorcode);

                        if (errorcode == OSPC_ERR_NO_ERROR) {
                            /* signal condvar to send data */
                            ospvAudit->Flags |= OSPC_AUDIT_FLUSH_BUFFER_NOW;

                            OSPM_CONDVAR_SIGNAL(ospvAudit->WorkerCondVar, errorcode);
                            OSPM_MUTEX_UNLOCK(ospvAudit->WorkerMutex, errorcode);
                        }
                    }
                } else if (OSPM_STRCMP((const char *)trigger, "start") == 0) {
                    /* If already on, don't turn on again */
                    if (!(ospvAudit->Comm->Flags & OSPC_COMM_AUDIT_ON)) {
                        /* Make sure buffer is clear */
                        if (ospvAudit->Flags & OSPC_AUDIT_BUFFER_IS_EMPTY) {
                            errorcode = OSPPAuditInit(ospvAudit);
                            OSPPCommSetAuditFlag(ospvAudit->Comm, OSPC_COMM_AUDIT_ON);
                        }
                    }
                }
            }
        }
    }
}

/* Delete component id list */
void OSPPAuditComponentIdDelete(
    OSPT_AUDIT *ospvAudit)
{
    OSPT_COMPONENT_ID *compid = OSPC_OSNULL;

    if (ospvAudit != OSPC_OSNULL) {
        if (ospvAudit->ComponentIdList != OSPC_OSNULL) {
            while (!OSPPListEmpty(&(ospvAudit->ComponentIdList))) {
                compid = (OSPT_COMPONENT_ID *)OSPPListRemove(&(ospvAudit->ComponentIdList));
                if (compid->Id != OSPC_OSNULL) {
                    OSPM_FREE(compid->Id);
                }

                if (compid != OSPC_OSNULL) {
                    OSPM_FREE(compid);
                }
            }

            OSPPListDelete(&(ospvAudit->ComponentIdList));
        }
    }
}

/* Create new component id item */
OSPT_COMPONENT_ID *OSPPAuditComponentIdNew(
    const char *ospvComponentId,
    unsigned ospvComponentIdLen)
{
    OSPT_COMPONENT_ID *compid = OSPC_OSNULL;

    if (ospvComponentId != OSPC_OSNULL) {
        OSPM_MALLOC(compid, OSPT_COMPONENT_ID, sizeof(OSPT_COMPONENT_ID));
        if (compid != OSPC_OSNULL) {
            OSPPListLinkNew(&(compid->Link));
            OSPM_MALLOC(compid->Id, char, ospvComponentIdLen + 1);
            OSPM_MEMSET(compid->Id, 0, ospvComponentIdLen + 1);
            OSPM_MEMCPY(compid->Id, ospvComponentId, ospvComponentIdLen);
        }
    }

    return compid;
}

/* delete audit object and all space allocated within it */
int OSPPAuditDelete(
    OSPT_AUDIT **ospvAudit)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (*ospvAudit != OSPC_OSNULL) {
        if ((*ospvAudit)->Storage != OSPC_OSNULL) {
            OSPPBfrDelete(&((*ospvAudit)->Storage));
        }

        /* free url pointer */
        if ((*ospvAudit)->URL != OSPC_OSNULL) {
            OSPM_FREE((*ospvAudit)->URL);
        }

        /* destroy mutexes and conditional vars */
        if ((*ospvAudit)->AccessHasMutex) {
            OSPM_CONDVAR_DESTROY((*ospvAudit)->AccessCondVar, errorcode);

            OSPM_MUTEX_DESTROY((*ospvAudit)->AccessMutex, errorcode);
        }

        if ((*ospvAudit)->WorkerHasMutex) {
            OSPM_CONDVAR_DESTROY((*ospvAudit)->WorkerCondVar, errorcode);

            OSPM_MUTEX_DESTROY((*ospvAudit)->WorkerMutex, errorcode);
        }

        /* Clean out componentId list */
        OSPPAuditComponentIdDelete(*ospvAudit);

        if (*ospvAudit != OSPC_OSNULL) {
            /* free audit block */
            OSPM_FREE(*ospvAudit);
        }
    }

    return errorcode;
}

/* Get buffer access conditional variable */
OSPTCONDVAR OSPPAuditGetAccessCondVar(
    OSPT_AUDIT *ospvAudit)
{
    OSPTCONDVAR condvar;
    OSPM_MEMSET(&condvar, 0, sizeof(OSPTCONDVAR));

    if (ospvAudit != OSPC_OSNULL) {
        return ospvAudit->AccessCondVar;
    } else {
        return condvar;
    }
}

unsigned OSPPAuditGetMaxMessages(
    OSPT_AUDIT *ospvAudit)
{
    if (ospvAudit != OSPC_OSNULL) {
        return ospvAudit->MaxMessages;
    } else {
        return 0;
    }
}

unsigned OSPPAuditGetMaxSpace(
    OSPT_AUDIT *ospvAudit)
{
    if (ospvAudit != OSPC_OSNULL) {
        return ospvAudit->MaxSpace;
    } else {
        return 0;
    }
}

unsigned OSPPAuditGetMaxTime(
    OSPT_AUDIT *ospvAudit)
{
    if (ospvAudit != OSPC_OSNULL) {
        return ospvAudit->MaxTime;
    } else {
        return 0;
    }
}

/* Get start time for auditing */
unsigned long OSPPAuditGetStartTime(
    OSPT_AUDIT *ospvAudit)
{
    if (ospvAudit != OSPC_OSNULL) {
        return ospvAudit->StartTime;
    } else {
        return 0;
    }
}

/* Get the URL for auditing */
const char *OSPPAuditGetURL(
    OSPT_AUDIT *ospvAudit)
{
    if (ospvAudit != OSPC_OSNULL) {
        return ospvAudit->URL;
    } else {
        return OSPC_OSNULL;
    }
}

/* Get worker conditional variable */
OSPTCONDVAR OSPPAuditGetWorkerCondVar(
    OSPT_AUDIT *ospvAudit)
{
    OSPTCONDVAR condvar;
    OSPM_MEMSET(&condvar, 0, sizeof(OSPTCONDVAR));

    if (ospvAudit != OSPC_OSNULL) {
        return ospvAudit->WorkerCondVar;
    } else {
        return condvar;
    }
}

/* Increment used space counter */
void OSPPAuditIncrementUsedSpace(
    OSPT_AUDIT *ospvAudit,
    unsigned ospvIncrement)
{
    if (ospvAudit != OSPC_OSNULL) {
        ospvAudit->UsedSpace += ospvIncrement;
    }
}

/* Begin collecting audit data */
int OSPPAuditInit(
    OSPT_AUDIT *ospvAudit)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvAudit != OSPC_OSNULL) {
        /* Set start time */
        OSPPAuditSetStartTime(ospvAudit, time(NULL));
        OSPPListNew(&(ospvAudit->ComponentIdList));

        /* Start worker thread */
        errorcode = OSPPAuditStartWorker(ospvAudit);
        if (errorcode != OSPC_ERR_NO_ERROR) {
            OSPM_DBGERRORLOG(errorcode, "error from OSPPAuditStartWorker.");
        }
    } else {
        errorcode = OSPC_ERR_AUDIT_NOT_FOUND;
        OSPM_DBGERRORLOG(errorcode, "Audit not found.");
    }

    return errorcode;
}

void OSPPAuditInitializeBuffer(
    OSPT_AUDIT *ospvAudit)
{
    int numbyteswritten = 0;
    char random[OSPC_MAX_RANDOM];

    OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);

    /* add XML header to buffer */
    numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), OSPC_AUDIT_XML_HEADER, OSPC_AUDIT_XML_HDR_LEN);
    /* increment used space */
    OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);
    numbyteswritten = 0;

    /* add "Message" header to buffer */
    numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), OSPC_AUDIT_MSG_HEADER, OSPC_AUDIT_MSG_HDR_LEN);
    /* increment used space */
    OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);
    numbyteswritten = 0;

    /* add message id string to buffer.
       use the random generator since we have no TransactionID
     */
    numbyteswritten = OSPPUtilGetRandom(random, 0);
    if (numbyteswritten > 0) {
        numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), random, numbyteswritten);
        /* increment used space */
        OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);
        numbyteswritten = 0;
        OSPM_MEMSET(random, 0, OSPC_MAX_RANDOM);
    }

    /* add "random" string to buffer */
    numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), OSPC_AUDIT_MSG_RANDOM, OSPC_AUDIT_MSG_RANDOM_LEN);
    /* increment used space */
    OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);
    numbyteswritten = 0;

    /* add random attr string to buffer */
    numbyteswritten = OSPPUtilGetRandom(random, 0);
    if (numbyteswritten > 0) {
        numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), random, numbyteswritten);
        /* increment used space */
        OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);
        numbyteswritten = 0;

        /* add end tag to buffer */
        numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), "\"", 1);
        /* increment used space */
        OSPPAuditIncrementUsedSpace(ospvAudit, 1);
        numbyteswritten = 0;
    }

    /* add end tag to buffer */
    numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), OSPC_AUDIT_TAG_END, OSPC_AUDIT_TAG_END_LEN);
    /* increment used space */
    OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);
    numbyteswritten = 0;
}

/* Monitor worker conditional variable in audit and wait for conditions to be met */
static OSPTTHREADRETURN OSPPAuditMonitor(
    void *ospvArg)      /* Audit block cast to a void */
{
    int errcode = OSPC_ERR_NO_ERROR, tmperror = OSPC_ERR_NO_ERROR;
    OSPT_AUDIT *audit = OSPC_OSNULL;
    OSPTBOOL do_forever = OSPC_TRUE;

    audit = (OSPT_AUDIT *)ospvArg;

    while (do_forever) {
        /*
         * acquire worker mutex
         */
        OSPM_MUTEX_LOCK(audit->WorkerMutex, errcode);

        if (errcode == OSPC_ERR_NO_ERROR) {
            /*
             * wait for conditional variable on the flush flag
             */
            while ((errcode != OSPC_ERR_OS_CONDVAR_TIMEOUT) && (audit->Flags & OSPC_AUDIT_FLUSH_BUFFER_NOW) == 0) {
                OSPM_CONDVAR_TIMEDWAIT(audit->WorkerCondVar, audit->WorkerMutex, audit->MaxTime, errcode);
            }

            if ((audit->Flags & OSPC_AUDIT_FLUSH_BUFFER_NOW) || (errcode == OSPC_ERR_OS_CONDVAR_TIMEOUT)) {
                OSPM_MUTEX_LOCK(audit->AccessMutex, errcode);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * prepare and send the data to the auditurl
                     */
                    errcode = OSPPAuditPrepareAndSend(audit);

                    /* If all Usage Indications have been confirmed, clear the buffer */
                    if ((audit->NumMessages == 0) && (errcode == OSPC_ERR_NO_ERROR)) {
                        OSPPAuditComponentIdDelete(audit);
                        errcode = OSPPAuditResetDefaults(audit);
                    }

                    if (errcode == OSPC_ERR_NO_ERROR) {
                        audit->Flags |= OSPC_AUDIT_BUFFER_IS_EMPTY;
                        /* signal in case this is auditdelete */
                        OSPM_CONDVAR_SIGNAL(audit->AccessCondVar, errcode);
                    }

                    OSPM_MUTEX_UNLOCK(audit->AccessMutex, errcode);
                }
                /*               break; */
            }

            /*
             * release the mutex lock
             */
            OSPM_MUTEX_UNLOCK(audit->WorkerMutex, tmperror);
        }
    }

/*    errorcode = OSPPAuditResetDefaults(audit);
*/

    OSPTTHREADRETURN_NULL();
}

/* Create new audit object */
OSPT_AUDIT *OSPPAuditNew(
    const char *ospvAuditURL)
{
    OSPT_AUDIT *audit = OSPC_OSNULL;
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC(audit, OSPT_AUDIT, sizeof(OSPT_AUDIT));
    if (audit != OSPC_OSNULL) {
        OSPM_MEMSET(audit, 0, sizeof(OSPT_AUDIT));

        /* set up the audit url */
        if (ospvAuditURL != OSPC_OSNULL) {
            OSPPAuditSetURL(audit, ospvAuditURL);

            /*
             * initialize the audit worker mutex
             */
            OSPM_MUTEX_INIT(audit->WorkerMutex, 0, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                /*
                 * initialize the audit worker conditional variable
                 */
                audit->WorkerHasMutex = OSPC_TRUE;
                OSPM_CONDVAR_INIT(audit->WorkerCondVar, NULL, errorcode);
            }

            /* set up storage */
            if (errorcode == OSPC_ERR_NO_ERROR) {
                audit->Storage = OSPPBfrNew((unsigned)OSPC_AUDIT_MAX_SPACE);
                if (audit->Storage == OSPC_OSNULL) {
                    errorcode = OSPC_ERR_AUDIT_MALLOC_FAILED;
                }
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /*
                 * initialize the audit access mutex
                 */
                OSPM_MUTEX_INIT(audit->AccessMutex, 0, errorcode);

                if (errorcode == OSPC_ERR_NO_ERROR) {
                    audit->AccessHasMutex = OSPC_TRUE;
                    /*
                     * initialize the audit access conditional variable
                     */
                    OSPM_CONDVAR_INIT(audit->AccessCondVar, NULL, errorcode);
                }
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPAuditSetMaxMessages(audit, OSPC_AUDIT_MAX_MESSAGES);
                /* adjust space for space needed for
                 * Message end tag.
                 */
                OSPPAuditSetMaxSpace(audit, OSPC_AUDIT_MAX_SPACE - (OSPC_AUDIT_BFR_END_LEN + 1));
                OSPPAuditSetMaxTime(audit, OSPC_AUDIT_MAX_TIME);
            }

            /* Set up XML and Message tag at beginning of buffer */
            if (errorcode == OSPC_ERR_NO_ERROR) {
                OSPPAuditInitializeBuffer(audit);

                /* Set the "buffer is empty" flag */
                audit->Flags |= OSPC_AUDIT_BUFFER_IS_EMPTY;
            }
        } else {
            errorcode = OSPC_ERR_AUDIT_MALLOC_FAILED;
        }
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPPAuditDelete(&audit);
    }

    return audit;
}

/* Prepare audit message and send */
int OSPPAuditPrepareAndSend(
    OSPT_AUDIT *ospvAudit)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *signature = OSPC_OSNULL;
    unsigned sizeofsignature = 0;
    unsigned char *auditbuffer = OSPC_OSNULL;
    unsigned auditbuffersz = 0;
    unsigned char *outgoingmessage = OSPC_OSNULL;
    unsigned sizeofoutmsg = 0;
    OSPT_MSG_INFO *msginfo = OSPC_OSNULL;
    int numbyteswritten = 0;

    if (ospvAudit != OSPC_OSNULL) {
        if ((ospvAudit->Security != OSPC_OSNULL) && (ospvAudit->Storage != OSPC_OSNULL)) {
            /* Add ending "Message" tag */
            numbyteswritten = OSPPBfrWriteBlock(&(ospvAudit->Storage), OSPC_AUDIT_BFR_END, OSPC_AUDIT_BFR_END_LEN);
            /* increment used space */
            OSPPAuditIncrementUsedSpace(ospvAudit, numbyteswritten);
            numbyteswritten = 0;

            auditbuffer = (unsigned char *)OSPPBfrLinearPtr(ospvAudit->Storage);
            auditbuffersz = OSPPBfrSize(ospvAudit->Storage);

            if (auditbuffersz > 0) {
                errorcode = OSPPSecSignatureCreate(ospvAudit->Security,
                    auditbuffer, auditbuffersz, &signature, &sizeofsignature, OSPC_SEC_SIGNATURE_ONLY);

                if ((errorcode == OSPC_ERR_NO_ERROR) && (signature == OSPC_OSNULL)) {
                    OSPM_MALLOC(signature, unsigned char, 64);
                    if (signature != OSPC_OSNULL) {
                        OSPM_STRCPY((char *)signature, "Signature placeholder");
                        sizeofsignature = OSPM_STRLEN((char *)signature);
                    } else {
                        errorcode = OSPC_ERR_SEC_NO_MEMORY;
                    }
                }
                if (errorcode == OSPC_ERR_NO_ERROR) {
                    errorcode = OSPPMimeMessageCreate(auditbuffer, auditbuffersz,
                        signature, sizeofsignature, (unsigned char **)&outgoingmessage, &sizeofoutmsg);

                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        errorcode = OSPPMsgInfoNew(&msginfo);
                        if (errorcode == OSPC_ERR_NO_ERROR) {

                            /* set the content type depending on the presence
                             * or absence of a signature
                             */
                            if (signature != OSPC_OSNULL) {
                                OSPM_MALLOC(msginfo->ContentType, unsigned char, OSPM_STRLEN(OSPC_COMM_MULTI_MSG) + 1);
                                if (msginfo->ContentType != OSPC_OSNULL) {
                                    OSPM_MEMSET(msginfo->ContentType, 0, OSPM_STRLEN(OSPC_COMM_MULTI_MSG) + 1);
                                    OSPM_MEMCPY(msginfo->ContentType, OSPC_COMM_MULTI_MSG, OSPM_STRLEN(OSPC_COMM_MULTI_MSG));
                                }
                            } else {
                                OSPM_MALLOC(msginfo->ContentType, unsigned char, OSPM_STRLEN(OSPC_COMM_TEXT_MSG) + 1);
                                if (msginfo->ContentType != OSPC_OSNULL) {
                                    OSPM_MEMSET(msginfo->ContentType, 0, OSPM_STRLEN(OSPC_COMM_TEXT_MSG) + 1);
                                    OSPM_MEMCPY(msginfo->ContentType, OSPC_COMM_TEXT_MSG, OSPM_STRLEN(OSPC_COMM_TEXT_MSG));
                                }
                            }

                            /* set the audit flag */
                            msginfo->Flags = (unsigned char) (msginfo->Flags | OSPC_MINFO_AUDITTYPE);
                        } else {    /* msginfo isn't here */
                            errorcode = OSPC_ERR_AUDIT_NOT_FOUND;
                        }
                    }
                    /* end if errorcode = OSPC_ERR_NO_ERROR after mimemessagecreate */
                    if (errorcode == OSPC_ERR_NO_ERROR) {
                        OSPPMsgInfoAssignRequestMsg(msginfo, outgoingmessage, sizeofoutmsg);

                        errorcode = OSPPCommAddTransaction(ospvAudit->Comm, msginfo);

                        if (errorcode == OSPC_ERR_NO_ERROR) {
                            errorcode = msginfo->ErrorCode;
                        }

                        if (errorcode == OSPC_ERR_NO_ERROR) {
                            errorcode = OSPPAuditProcessReturn(ospvAudit, msginfo);
                        }
                    }
                }               /* end if errorcode == OSPC_ERR_NO_ERROR before mimemessagecreate */
            }                   /* end if auditbuffersz > 0 */
        }                       /* end if security and storage != OSPC_OSNULL */
    }
    /* end if ospvAudit != OSPC_OSNULL */
    if (signature != OSPC_OSNULL) {
        OSPM_FREE(signature);
        signature = OSPC_OSNULL;
    }

    if (msginfo != OSPC_OSNULL) {
        OSPPMsgInfoDelete(&msginfo);
    }

    return errorcode;
}

/* Process return from server */
int OSPPAuditProcessReturn(
    OSPT_AUDIT *ospvAudit,
    OSPT_MSG_INFO *ospvMsgInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned char *incomingmsg = OSPC_OSNULL;
    unsigned char *incomingsignature = OSPC_OSNULL;
    unsigned sizeofmsg = 0;
    unsigned sizeofsig = 0;
    void *resultrsp = OSPC_OSNULL;
    OSPE_MESSAGE msgtype = OSPC_MSG_UNKNOWN;
    OSPT_USAGE_CNF *usagecnf = OSPC_OSNULL;

    if (ospvMsgInfo != OSPC_OSNULL) {
        errorcode = OSPPMimeMessageParse(ospvMsgInfo->ResponseMsg,
            ospvMsgInfo->ResponseSz,
            ospvMsgInfo->ContentType, ospvMsgInfo->ContentSz, &incomingmsg, &sizeofmsg, &incomingsignature, &sizeofsig);
    } else {
        errorcode = OSPC_ERR_AUDIT_NOT_FOUND;
    }

#ifdef ENABLESMIME

    /*
     * Validate the S/MIME signature
     */
    if (errorcode == OSPC_ERR_NO_ERROR) {
        if ((incomingsignature != OSPC_OSNULL) && (ospvAudit->ospmAuditSecurity != OSPC_OSNULL)) {
            errorcode = OSPPSecSignatureVerify(ospvAudit->ospmAuditSecurity,
                                               &incomingmsg, &sizeofmsg, incomingsignature, sizeofsig, OSPC_SEC_SIGNATURE_ONLY);
        } else {
            errorcode = OSPC_ERR_AUDIT_NOT_FOUND;
        }
    }
#endif

    if (errorcode == OSPC_ERR_NO_ERROR) {
        /*
         * Send the XML message doc to the XML parser
         */
        errorcode = OSPPXMLMessageParse(incomingmsg, sizeofmsg, &resultrsp, &msgtype);

        if (errorcode == OSPC_ERR_NO_ERROR) {

            switch (msgtype) {
                /*
                 * Authorization Response received
                 */

            case OSPC_MSG_ARESP:
                break;

                /*
                 * Usage Confirmation received
                 */

            case OSPC_MSG_UCNF:
                for ((usagecnf = (OSPT_USAGE_CNF *)OSPPListFirst((OSPTLIST *)&(resultrsp)));
                     (usagecnf != OSPC_OSNULL); (usagecnf = (OSPT_USAGE_CNF *)OSPPListNext((OSPTLIST *)&(resultrsp), usagecnf))) {
                    OSPPAuditVerifyUsageCnf(usagecnf, ospvAudit);
                }

                while (!OSPPListEmpty((OSPTLIST *)&resultrsp)) {
                    usagecnf = (OSPT_USAGE_CNF *)OSPPListRemove((OSPTLIST *)&resultrsp);
                    if (usagecnf != OSPC_OSNULL) {
                        OSPPUsageCnfDelete(&usagecnf);
                    }
                }

                OSPPListDelete((OSPTLIST *)&resultrsp);

                break;

                /*
                 * Authorization Confirmation received
                 */

            case OSPC_MSG_ACNF:
                break;

                /*
                 * Reauthorization Response received
                 */

            case OSPC_MSG_REARESP:
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

    if (incomingmsg != OSPC_OSNULL) {
        OSPM_FREE(incomingmsg);
        incomingmsg = OSPC_OSNULL;
    }

    if (incomingsignature != OSPC_OSNULL) {
        OSPM_FREE(incomingsignature);
        incomingsignature = OSPC_OSNULL;
    }

    return errorcode;
}

/*
 * Subtract Usage Indications from the buffer according to the component ID
 */
void OSPPAuditRemoveComponentIdFromList(
    const char *ospvCompid,
    OSPT_AUDIT *ospvAudit)
{
    OSPT_COMPONENT_ID *compid = OSPC_OSNULL;

    for (compid = (OSPT_COMPONENT_ID *)OSPPListFirst(&(ospvAudit->ComponentIdList));
         (compid != OSPC_OSNULL); compid = (OSPT_COMPONENT_ID *)OSPPListNext(&(ospvAudit->ComponentIdList), compid)) {
        if (compid != OSPC_OSNULL) {
            if (compid->Id != OSPC_OSNULL) {
                if (OSPM_STRCMP(compid->Id, ospvCompid) == 0) {
                    ospvAudit->NumMessages--;
                    break;
                }
            }
        }
    }
}

/* Reset audit values to current defaults */
int OSPPAuditResetDefaults(
    OSPT_AUDIT *ospvAudit)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvAudit != OSPC_OSNULL) {
        /* OSPM_MUTEX_LOCK(ospvAudit->ospmAuditAccessMutex, errorcode); */
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPPBfrClear(ospvAudit->Storage);
            /* not sure if we want to worry about an error here MOC */
            /* OSPM_MUTEX_UNLOCK(ospvAudit->ospmAuditAccessMutex, errorcode); */
        }

        OSPPAuditSetStartTime(ospvAudit, 0);
        OSPPAuditSetNumMessages(ospvAudit, 0);
        OSPPAuditSetUsedSpace(ospvAudit, 0);

        OSPPAuditSetMaxMessages(ospvAudit, OSPC_AUDIT_MAX_MESSAGES);
        OSPPAuditSetMaxSpace(ospvAudit, OSPC_AUDIT_MAX_SPACE - (OSPC_AUDIT_BFR_END_LEN + 1));
        OSPPAuditSetMaxTime(ospvAudit, OSPC_AUDIT_MAX_TIME);
        OSPPAuditInitializeBuffer(ospvAudit);
        ospvAudit->Flags = (unsigned char) (ospvAudit->Flags & OSPC_AUDIT_BUFFER_CLEAR_FLAGS);
    }

    return errorcode;
}

/* Set the audit comm pointer for sending data */
void OSPPAuditSetComm(
    OSPT_AUDIT *ospvAudit,
    OSPTCOMM *ospvComm)
{
    if (ospvAudit != OSPC_OSNULL) {
        if (ospvComm != OSPC_OSNULL) {
            ospvAudit->Comm = ospvComm;
        }
    }
}

void OSPPAuditSetMaxMessages(
    OSPT_AUDIT *ospvAudit,
    unsigned ospvNumMessages)
{
    if (ospvAudit != OSPC_OSNULL) {
        ospvAudit->MaxMessages = ospvNumMessages;
    }
}

void OSPPAuditSetMaxSpace(
    OSPT_AUDIT *ospvAudit,
    unsigned ospvMaxSpace)
{
    if (ospvAudit != OSPC_OSNULL) {
        ospvAudit->MaxSpace = ospvMaxSpace;
    }
}

void OSPPAuditSetMaxTime(
    OSPT_AUDIT *ospvAudit,
    unsigned ospvMaxTime)
{
    if (ospvAudit != OSPC_OSNULL) {
        ospvAudit->MaxTime = ospvMaxTime;
    }
}

void OSPPAuditSetNumMessages(
    OSPT_AUDIT *ospvAudit,
    unsigned ospvNumMsgs)
{
    if (ospvAudit != OSPC_OSNULL) {
        ospvAudit->NumMessages = ospvNumMsgs;
    }
}

/* Set the audit security pointer for signing */
void OSPPAuditSetSecurity(
    OSPT_AUDIT *ospvAudit,
    OSPTSEC *ospvSecurity)
{
    if (ospvAudit != OSPC_OSNULL) {
        if (ospvSecurity != OSPC_OSNULL) {
            ospvAudit->Security = ospvSecurity;
        }
    }
}

/* Set the start time for collecting data */
void OSPPAuditSetStartTime(
    OSPT_AUDIT *ospvAudit,
    unsigned long ospvTime)
{
    if (ospvAudit != OSPC_OSNULL) {
        ospvAudit->StartTime = ospvTime;
    }
}

/* Set the URL for audit purposes */
void OSPPAuditSetURL(
    OSPT_AUDIT *ospvAudit,
    const char *ospvAuditURL)
{
    if (ospvAudit != OSPC_OSNULL) {
        if (ospvAuditURL != OSPC_OSNULL) {
            if (ospvAudit->URL != OSPC_OSNULL) {
                OSPM_FREE(ospvAudit->URL);
            }

            OSPM_MALLOC(ospvAudit->URL, char, OSPM_STRLEN(ospvAuditURL) + 1);
            if (ospvAudit->URL != OSPC_OSNULL) {
                OSPM_MEMCPY(ospvAudit->URL, ospvAuditURL, OSPM_STRLEN(ospvAuditURL) + 1);
            }
        }
    }
}

void OSPPAuditSetUsedSpace(
    OSPT_AUDIT *ospvAudit,
    unsigned ospvUsedSpace)
{
    if (ospvAudit != OSPC_OSNULL) {
        ospvAudit->UsedSpace = ospvUsedSpace;
    }
}

/* Start the worker thread to wait for favorable conditions to send audit */
int OSPPAuditStartWorker(
    OSPT_AUDIT *ospvAudit)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTTHRATTR thread_attr;

    if (ospvAudit == OSPC_OSNULL) {
        errorcode = OSPC_ERR_AUDIT_NOT_FOUND;
    } else {
        OSPM_THRATTR_INIT(thread_attr, errorcode);
        if (errorcode == OSPC_ERR_NO_ERROR) {
            OSPM_SETDETACHED_STATE(thread_attr, errorcode);
            if (errorcode == OSPC_ERR_NO_ERROR) {
                /*
                 * create the new thread which will monitor
                 * the access buffer flag in order to send data
                 */
                OSPM_CREATE_THREAD(ospvAudit->ThreadId, &thread_attr, OSPPAuditMonitor, ospvAudit, errorcode);
            }
            OSPM_THRATTR_DESTROY(thread_attr);
        }

        if (errorcode != OSPC_ERR_NO_ERROR) {
            errorcode = OSPC_ERR_AUDIT_THREAD_INIT_FAILED;
            OSPM_DBGERRORLOG(errorcode, "thread initialization failed");
        }
    }

    return errorcode;
}

void OSPPAuditVerifyUsageCnf(
    OSPT_USAGE_CNF *ospvUsageCnf,
    OSPT_AUDIT *ospvAudit)
{
    /* Get the component Id from the Usage Confirm,
     * remove the associated Usage Indication from the
     * storage buffer.
     */
    const char *compid = OSPC_OSNULL;

    if (OSPPUsageCnfHasComponentId(ospvUsageCnf)) {
        compid = OSPPUsageCnfGetComponentId(ospvUsageCnf);
        OSPPAuditRemoveComponentIdFromList(compid, ospvAudit);
    }
}
