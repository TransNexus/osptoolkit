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
 * ospmsgdesc.h - Structures and prototypes for generic XML message
 *                descriptors.
 */

#ifndef OSPMSGDESC_H
#define OSPMSGDESC_H


#include "osp.h"
#include "ospmsgpart.h"


/*
 * The OSPPMsgDescGetName includes two different implementations: the
 * first is safer and the second has higher performance. The safer option
 * does an exhaustive search through the descriptors array. The second
 * simply uses the part ID as an index into that array. The risk with the
 * second approach is that it requires that the array be built correctly,
 * with elements in the precise, right order. If you enable the second
 * approach, it is essential to test the array ordering using the the code
 * in the included main() procedures in the individual components.
 */

#define OSPC_FASTMSG_DESC

/*
 * An OPSMsgDesc structure is used to associate a part (for an attribute
 * or an element) with a name.
 */

typedef struct
{
    OSPTMSGPART  ospmMsgDescPart;
    const char  *ospmMsgDescName;
}
OSPTMSGDESC;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/
#ifndef OSPC_DEBUG
/*
 * Note: all macros are also implemented as functions in ospmsgdesc.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

#define OSPPMsgDescGetName(ospvPart, ospvDescs, ospvNumDesc) \
        (((ospvPart) < (ospvNumDesc)) ? \
        (ospvDescs)[(int)(ospvPart)].ospmMsgDescName : OSPC_OSNULL )
#endif /* OSPC_DEBUG */

/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/


OSPTMSGPART  OSPPMsgDescGetPart(const char *, const OSPTMSGDESC *, unsigned);

#ifdef OSPC_DEBUG
const char  *OSPPMsgDescGetName(OSPTMSGPART,  const OSPTMSGDESC *, unsigned);
#endif /* OSPC_DEBUG */

#endif /* OSPMSGDESC_H */

