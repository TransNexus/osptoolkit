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
 * ospxmlattr.h - Structures and prototypes for generic XML attributes.
 */

#ifndef _OSPXMLATTR_H
#define _OSPXMLATTR_H

#include "osp.h"
#include "osplist.h"

/*
 * An XMLAttr structure represents an XML attribute in its
 * natural form ("natural" in the sense of easiest to reference
 * in code). It includes the attribute's name, as well as the
 * character value. The structure is defined as a linked list
 * so that multiple attributes (e.g. for the same element) can
 * be linked together.
 */

typedef struct OSPSXMLAttr
{
    OSPTLISTLINK ospmXMLAttrLink;
    const char  *ospmXMLAttrName;
    const char  *ospmXMLAttrValue;
}
OSPTXMLATTR;


#define OSPC_MAX_ID_LEN 25

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospxmlattr.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG
#define OSPPXMLAttrGetName(ospvXMLAttr)  (ospvXMLAttr)?((ospvXMLAttr)->ospmXMLAttrName):OSPC_OSNULL
#define OSPPXMLAttrGetValue(ospvXMLAttr) (ospvXMLAttr)?((ospvXMLAttr)->ospmXMLAttrValue):OSPC_OSNULL
#endif
/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" 
{
#endif

OSPTXMLATTR *OSPPXMLAttrNew(const unsigned char *, const unsigned char *);
void         OSPPXMLAttrDelete(OSPTXMLATTR **);

#ifndef OSPPXMLAttrGetName
const char *OSPPXMLAttrGetName(OSPTXMLATTR *);
#endif

#ifndef OSPPXMLAttrGetValue
const char *OSPPXMLAttrGetValue(OSPTXMLATTR *);
#endif


#ifdef __cplusplus
}
#endif

#endif

