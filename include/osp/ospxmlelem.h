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

typedef struct {
    OSPTLISTLINK ospmXMLElemLink;
    OSPTLIST ospmXMLElemChild;
    OSPTLIST ospmXMLElemAttrs;
    char *ospmXMLElemName;
    char *ospmXMLElemValue;
} OSPT_XML_ELEM;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPT_XML_ELEM *OSPPXMLElemNew(const char *, const char *);
    void OSPPXMLElemDelete(OSPT_XML_ELEM **);
    const char *OSPPXMLElemGetName(OSPT_XML_ELEM *);
    const char *OSPPXMLElemGetValue(OSPT_XML_ELEM *);
    void OSPPXMLElemAddChild(OSPT_XML_ELEM *, OSPT_XML_ELEM *);
    OSPT_XML_ELEM *OSPPXMLElemFirstChild(OSPT_XML_ELEM *);
    OSPT_XML_ELEM *OSPPXMLElemNextChild(OSPT_XML_ELEM *, OSPT_XML_ELEM *);
    void OSPPXMLElemAddAttr(OSPT_XML_ELEM *, OSPT_XML_ATTR *);
    OSPT_XML_ATTR *OSPPXMLElemFirstAttr(OSPT_XML_ELEM *);
    OSPT_XML_ATTR *OSPPXMLElemNextAttr(OSPT_XML_ELEM *, OSPT_XML_ATTR *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPXMLELEM_H */
