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
#include "osp.h"
#include "ospfail.h"

int
OSPPFailReasonFind(enum OSPEFAILREASON ospvFailureReason)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    switch(ospvFailureReason)
    {

        case OSPC_FAIL_NONE:
        case OSPC_FAIL_LUSER_DISC:  
        case OSPC_FAIL_LUSER_ADMIN:  
        case OSPC_FAIL_LOCAL_NORSRC: 
        case OSPC_FAIL_LOCAL_NOAUTH: 
        case OSPC_FAIL_LOCAL_BADAUTH:
        case OSPC_FAIL_LOCAL_BADROUTE:
        case OSPC_FAIL_LOCAL_ADMIN:   
        case OSPC_FAIL_NET_UNREACH:   
        case OSPC_FAIL_NET_BADPATH:   
        case OSPC_FAIL_NET_BADPERF:   
        case OSPC_FAIL_NET_ADMIN:     
        case OSPC_FAIL_REMOTE_GENERAL:
        case OSPC_FAIL_REMOTE_NORESP: 
        case OSPC_FAIL_REMOTE_TCPFIN: 
        case OSPC_FAIL_REMOTE_BADSEC: 
        case OSPC_FAIL_REMOTE_BADCOMM:
        case OSPC_FAIL_REMOTE_REFUSE: 
        case OSPC_FAIL_REMOTE_NORSRC: 
        case OSPC_FAIL_REMOTE_INT:   
        case OSPC_FAIL_REMOTE_EXT:    
        case OSPC_FAIL_ADMIN:         
        case OSPC_FAIL_RUSER_BUSY:    
        case OSPC_FAIL_RUSER_NETBUSY: 
        case OSPC_FAIL_RUSER_UNAVAIL: 
        case OSPC_FAIL_RUSER_DISC: 
        case OSPC_FAIL_RUSER_ADMIN:
        case OSPC_FAIL_GENERAL:
        break;
        default:
        errorcode = OSPC_ERR_FAILRSN_INVALID;
        OSPM_DBGERRORLOG(errorcode, "Failure code invalid");
    }

    return errorcode;
}
