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
 *  ospacapind.h - OSP capabiliti indication objects
 */

#ifndef _OSPACAPIND_H
#define _OSPACAPIND_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsg.h"
#include "ospaltinfo.h"
#include "osptrans.h"



typedef struct
{
    OSPTTIME          ospmTimestamp;
    unsigned char    *ospmMessageId;
    unsigned char    *ospmComponentId;
    OSPTLIST          ospmDeviceInfo;
    unsigned          ospmAlmostOutOfResources;
    OSPTLIST          ospmSrcAlternate;
}
OSPTCAPIND;


#ifdef __cplusplus
extern "C" 
{
#endif

    /*-----------------------------------------------------------------------*
     *  function prototypes
     *-----------------------------------------------------------------------*/
    unsigned       OSPPGenerateUniqueId(OSPTTRANS*, unsigned char **);
    unsigned       OSPPCapIndNew(OSPTCAPIND**, OSPTTRANS*, const char*, const char*, unsigned);
    void           OSPPCapIndDelete(OSPTCAPIND **);
  int            OSPPCapIndToElement(OSPTCAPIND *, OSPTXMLELEM **);

#ifdef __cplusplus
}
#endif

#endif /* _OSPACAPIND_H */
