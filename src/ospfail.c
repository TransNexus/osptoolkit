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
 * ospfail.cpp - Failure reasons.
 */
#include "osp/osp.h"
#include "osp/ospfail.h"

/*
 * Will return success as long as ospvFailureReason is between
 * OSPC_FAIL_NONE (0) and OSPC_FAIL_GENERAL (999)
 */
int
OSPPFailReasonFind(enum OSPEFAILREASON ospvFailureReason)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if( (ospvFailureReason < OSPC_FAIL_NONE) || (ospvFailureReason > OSPC_FAIL_GENERAL) )
    {
        errorcode = OSPC_ERR_FAILRSN_INVALID;
        OSPM_DBGERRORLOG(errorcode, "Failure code invalid");
    }

    return errorcode;
}
