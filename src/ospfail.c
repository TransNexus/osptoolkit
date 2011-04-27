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

/* ospfail.cpp - Failure reasons. */

#include "osp/osp.h"
#include "osp/ospfail.h"

/*
 * Will return success as long as ospvFailureReason is between
 * OSPC_FAIL_NONE (0) and OSPC_FAIL_GENERAL (999)
 */
int OSPPFailReasonFind(
    OSPEFAILREASON ospvFailureReason)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if ((ospvFailureReason < OSPC_FAIL_NONE) || (ospvFailureReason > OSPC_FAIL_GENERAL)) {
        errorcode = OSPC_ERR_FAILRSN_INVALID;
        OSPM_DBGERRORLOG(errorcode, "Failure code invalid");
    }

    return errorcode;
}

/*
 * OSPPHasTermCause() - Does request have termination cause
 */
OSPTBOOL OSPPHasTermCause(          /* returns non-zero if time */
    OSPT_TERM_CAUSE *ospvTermCause, /* usage request in question */
    OSPE_TERM_CAUSE ospvType)       /* termination cause type */
{
    OSPTBOOL has = OSPC_FALSE;

    if (ospvTermCause != OSPC_OSNULL) {
        if ((ospvType >= OSPC_TCAUSE_START) && (ospvType < OSPC_TCAUSE_NUMBER)) {
            has = ospvTermCause->hastermcause[ospvType];
        }
    }

    return has;
}

/*
 * OSPPHasTermCauseAny() - Does request have a termination cause
 */
OSPTBOOL OSPPHasTermCauseAny(       /* returns non-zero if time */
    OSPT_TERM_CAUSE *ospvTermCause) /* usage request in question */
{
    OSPTBOOL has = OSPC_FALSE;
    OSPE_TERM_CAUSE ospvType;

    if (ospvTermCause != OSPC_OSNULL) {
        for (ospvType = OSPC_TCAUSE_START; ospvType < OSPC_TCAUSE_NUMBER; ospvType++) {
            has |= ospvTermCause->hastermcause[ospvType];
        }
    }

    return has;
}

/*
 * OSPPSetTermCause() - Set termination cause
 */
void OSPPSetTermCause(  /* nothing returned */
    OSPT_TERM_CAUSE *ospvTermCause,
    OSPE_TERM_CAUSE ospvType,
    unsigned ospvTCCode,
    const char *ospvTCDesc)
{
    if (ospvTermCause != OSPC_OSNULL) {
        if ((ospvType >= OSPC_TCAUSE_START) && (ospvType < OSPC_TCAUSE_NUMBER)) {
            ospvTermCause->tccode[ospvType] = ospvTCCode;
            if ((ospvTCDesc != OSPC_OSNULL) && (ospvTCDesc[0] != '\0')) {
                OSPM_STRNCPY(ospvTermCause->tcdesc[ospvType], ospvTCDesc, sizeof(ospvTermCause->tcdesc[ospvType]) - 1);
            }
            ospvTermCause->hastermcause[ospvType] = OSPC_TRUE;
        }
    }
}

/*
 * OSPPGetTCCode() - returns termination cause code
 */
unsigned OSPPGetTCCode(
    OSPT_TERM_CAUSE *ospvTermCause, /* Termination cause */
    OSPE_TERM_CAUSE ospvType)       /* Termination cause type */
{
    unsigned ospvTCCode = 0;

    if (ospvTermCause != OSPC_OSNULL) {
        if ((ospvType >= OSPC_TCAUSE_START) && (ospvType < OSPC_TCAUSE_NUMBER)) {
            if (ospvTermCause->hastermcause[ospvType]) {
                ospvTCCode = ospvTermCause->tccode[ospvType];
            }
        }
    }

    return ospvTCCode;
}

/*
 * OSPPGetTCDesc() - returns termination cause description
 */
const char *OSPPGetTCDesc(
    OSPT_TERM_CAUSE *ospvTermCause, /* Termination cause */
    OSPE_TERM_CAUSE ospvType)       /* Termination cause type */
{
    const char *ospvTCDesc = OSPC_OSNULL;

    if (ospvTermCause != OSPC_OSNULL) {
        if ((ospvType >= OSPC_TCAUSE_START) && (ospvType < OSPC_TCAUSE_NUMBER)) {
            if (ospvTermCause->hastermcause[ospvType]) {
                ospvTCDesc = ospvTermCause->tcdesc[ospvType];
            }
        }
    }

    return ospvTCDesc;
}

