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
 * ospx500.h - Constants for X500 objects.
 */

#ifndef _OSPX500_H
#define _OSPX500_H

#include "osp/osp.h"
#include "osp/ospasn1.h"

#define OSPC_DN_ELEMENT_TERMINATOR  (-1)    /* Not a legal OID value */

#define OSPC_MAX_DN_ELEMENTS        10      /* Max DN elements */

#define OSPC_MAX_FIELDNAME_LENGTH   50
#define OSPC_MAX_FIELDVALUE_LENGTH  50

typedef struct {
    char fieldname[OSPC_MAX_FIELDNAME_LENGTH];
    char fieldvalue[OSPC_MAX_FIELDVALUE_LENGTH];
} OSPTDNTEXTELEMENT;

typedef struct {
    unsigned int elementCount;
    OSPTATTRTYPEVAL *elements[OSPC_MAX_DN_ELEMENTS];
} OSPTDISTINGUISHEDNAME;

#endif /* _OSPX500_H */
