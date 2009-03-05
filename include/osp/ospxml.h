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
 * ospxml.h - Header file for xml calls
 */

#ifndef _OSPXML_H
#define _OSPXML_H

#include "osp/osp.h"
#include "osp/ospmsg.h"
#include "osp/osptrans.h"

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPXMLElementProcess(OSPT_XML_ELEM *, unsigned char **, unsigned *);
    int OSPPXMLGetDataType(OSPT_XML_ELEM *, OSPE_MESSAGE *);
    int OSPPXMLMessageCreate(OSPE_MESSAGE, unsigned char **, unsigned *, void *, OSPTTRANS *);
    int OSPPXMLMessageParse(unsigned char *, unsigned, void **, OSPE_MESSAGE *);
    int OSPPXMLMessageProcess(OSPT_XML_ELEM *, void **, OSPE_MESSAGE);

#ifdef __cplusplus
}
#endif

#endif /* _OSPXML_H */
