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

/* ospcustomdebug.h - Structures and prototypes for debugging and logging. */
#ifndef _OSPCUSTOMDEBUG_H
#define _OSPCUSTOMDEBUG_H

#define OSPM_DBG(ospvCond, ospvParams)              if (ospvCond) { }
#define OSPM_DBGERRORLOG(ospvErrCode, ospvErrText)
#define OSPM_DBGENTER(ospvParams)
#define OSPM_DBGEXIT(ospvParams)
#define OSPM_DBGNET(ospvParams)
#define OSPM_DBGMISC(ospvParams)
#define OSPM_DBGSEC(ospvParams)
#define OSPM_DBGERROR(ospvParams)

#endif /* _OSPCUSTOMDEBUG_H */
