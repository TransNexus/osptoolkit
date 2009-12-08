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

/* ospmsgpart.h - Structures and prototypes for generic OSP message parts */

#ifndef _OSPMSGPART_H
#define _OSPMSGPART_H

/*
 * An OSPMsgPart is a convenient way to represent attributes and
 * elements. It's defined as a type that can be used, for example,
 * directly in switch statements without having to use messy
 * OSPM_STRNCMP() calls.
 *
 * We define it as an int here, though actual attributes and
 * elements will likely use enumerated types.
 */

typedef int OSPT_MSG_PART;

#define OSPC_MPART_UNKNOWN      ((OSPT_MSG_PART)-16)    /* rsv'd for unknown part */
#define OSPC_MPART_START        ((OSPT_MSG_PART)0)      /* rsv'd for start part */

#endif /* _OSPMSGPART_H */
