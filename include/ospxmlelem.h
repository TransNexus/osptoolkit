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
 * ospxmlelem.h - Structures and prototypes for generic XML elements.
 */

#ifndef _OSPXMLELEM_H
#define _OSPXMLELEM_H

#include "osp.h"
#include "osplist.h"
#include "ospxmlattr.h"


/*
 * An XMLElem structure represents an XML element in its natural
 * form ("natural" in the sense of easiest to reference in code).
 * It includes pointers to child elements and attributes, as
 * well as the element name and actual character string for the
 * element.
 */

typedef struct
{
    OSPTLISTLINK ospmXMLElemLink;
    OSPTLIST     ospmXMLElemChild;
    OSPTLIST     ospmXMLElemAttrs;
    char        *ospmXMLElemName;
    char        *ospmXMLElemValue;
}
OSPTXMLELEM;

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospxmlelem.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG
#define OSPPXMLElemGetName(ospvElem)    (ospvElem)?((ospvElem)->ospmXMLElemName):OSPC_OSNULL
#define OSPPXMLElemGetValue(ospvElem)   (ospvElem)?((ospvElem)->ospmXMLElemValue):OSPC_OSNULL
#define OSPPXMLElemAddChild(ospvElem, ospvChild) \
    OSPPListAppend((OSPTLIST *) &(ospvElem)->ospmXMLElemChild, (ospvChild))
#define OSPPXMLElemFirstChild(ospvElem) \
    OSPPListFirst((OSPTLIST *) &(ospvElem)->ospmXMLElemChild)
#define OSPPXMLElemNextChild(ospvElem, ospvChild) \
    OSPPListNext((OSPTLIST *) &(ospvElem)->ospmXMLElemChild, (ospvChild))
#define OSPPXMLElemAddAttr(ospvElem, ospvAttr)   \
    OSPPListAppend((OSPTLIST *) &(ospvElem)->ospmXMLElemAttrs, (ospvAttr))
#define OSPPXMLElemFirstAttr(ospvElem) \
    OSPPListFirst((OSPTLIST *) &(ospvElem)->ospmXMLElemAttrs)
#define OSPPXMLElemNextAttr(ospvElem, ospvAttr) \
    OSPPListNext((OSPTLIST *) &(ospvElem)->ospmXMLElemAttrs, (ospvAttr))
#endif

/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" 
{
#endif

    OSPTXMLELEM *OSPPXMLElemNew(const char *, const char *);

    void OSPPXMLElemDelete(OSPTXMLELEM **);

#ifndef OSPPXMLElemGetName
    const char *OSPPXMLElemGetName(OSPTXMLELEM *);
#endif

#ifndef OSPPXMLElemGetValue
    const char *OSPPXMLElemGetValue(OSPTXMLELEM *);
#endif

#ifndef OSPPXMLElemAddChild
    void OSPPXMLElemAddChild(OSPTXMLELEM *, OSPTXMLELEM *);
#endif

#ifndef OSPPXMLElemFirstChild
    OSPTXMLELEM *OSPPXMLElemFirstChild(OSPTXMLELEM *);
#endif

#ifndef OSPPXMLElemNextChild
    OSPTXMLELEM *OSPPXMLElemNextChild(OSPTXMLELEM *, OSPTXMLELEM *);
#endif

#ifndef OSPPXMLElemAddAttr
    void OSPPXMLElemAddAttr(OSPTXMLELEM *, OSPTXMLATTR *);
#endif

#ifndef OSPPXMLElemFirstAttr
    OSPTXMLATTR *OSPPXMLElemFirstAttr(OSPTXMLELEM *);
#endif

#ifndef OSPPXMLElemNextAttr
    OSPTXMLATTR *OSPPXMLElemNextAttr(OSPTXMLELEM *, OSPTXMLATTR *);
#endif


#ifdef __cplusplus
}
#endif

#endif

