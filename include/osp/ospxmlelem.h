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

#include "osp/osp.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"


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
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" 
{
#endif

    OSPTXMLELEM *OSPPXMLElemNew(const char *, const char *);

    void OSPPXMLElemDelete(OSPTXMLELEM **);

    const char *OSPPXMLElemGetName(OSPTXMLELEM *);

    const char *OSPPXMLElemGetValue(OSPTXMLELEM *);

    void OSPPXMLElemAddChild(OSPTXMLELEM *, OSPTXMLELEM *);

    OSPTXMLELEM *OSPPXMLElemFirstChild(OSPTXMLELEM *);

    OSPTXMLELEM *OSPPXMLElemNextChild(OSPTXMLELEM *, OSPTXMLELEM *);

    void OSPPXMLElemAddAttr(OSPTXMLELEM *, OSPTXMLATTR *);

    OSPTXMLATTR *OSPPXMLElemFirstAttr(OSPTXMLELEM *);

    OSPTXMLATTR *OSPPXMLElemNextAttr(OSPTXMLELEM *, OSPTXMLATTR *);


#ifdef __cplusplus
}
#endif

#endif

