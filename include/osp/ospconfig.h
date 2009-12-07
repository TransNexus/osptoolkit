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

/* ospconfig.h - OSP configuration items */
#ifndef _OSPCONFIG_H
#define _OSPCONFIG_H

#include "osp/osp.h"

#define OSPC_AUDIT_MAX_SPACE            1024 * 10       /* 10 K */
#define OSPC_AUDIT_MAX_TIME             60 * 60         /* 1 hour in seconds */
#define OSPC_AUDIT_MAX_MESSAGES         100
#define OSPC_AUDIT_RETRY_DELAY          3
#define OSPC_AUDIT_RETRY_LIMIT          1

#define OSPC_DEFAULT_SSLLIFETIME        4 * 60 * 60     /* 4 hours in seconds */
#define OSPC_DEFAULT_HTTPMAXCONNECTIONS 1               /* at least one */
#define OSPC_DEFAULT_HTTPPERSISTENCE    60              /* seconds */
#define OSPC_DEFAULT_HTTPRETRYDELAY     2               /* seconds */
#define OSPC_DEFAULT_HTTPTIMEOUT        3 * 1000        /* 3 seconds in milliseconds */
#define OSPC_DEFAULT_PROBE_CONNECT      1 * 1000        /* 1 second in milliseconds */
#define OSPC_DEFAULT_BLOCKING_FLAG      OSPC_FALSE

#endif /* _OSPCONFIG_H */
