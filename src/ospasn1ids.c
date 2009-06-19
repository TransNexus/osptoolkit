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
 * ospasn1ids.c - ASN1 defined ID's definitions and functions.
 */

#define OSPC_OSPASN1ID_INCLUDE_STATIC   1   /* Forces inclusion of static */
/* data defined in ospasn1id.h */
#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/ospasn1ids.h"

/* FUNCTION PROTOTYPES */

/*
 * Member functions
 */

int OSPPASN1IdGetValue(
    OSPEASN1ID ospvId, 
    unsigned char **ospvIdValue, 
    unsigned *ospvIdLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTIDINDEX *idxRec = OSPC_OSNULL;

    if ((ospvId < 0) || (ospvId > OSPEID_LISTEND)) {
        errorcode = OSPC_ERR_ASN1_OBJECTID_NOT_FOUND;
        OSPM_DBGERRORLOG(errorcode, "Object Id enum out of range");
    } else if (ospvIdValue == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_NULL_POINTER;
        OSPM_DBGERRORLOG(errorcode, "Invalid Null Pointer provided for object Id return");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        idxRec = &(ospgASN1IDIndex[ospvId]);
        if (idxRec->Id != ospvId) {
            errorcode = OSPC_ERR_ASN1_OBJECTID_MISMATCH;
            OSPM_DBGERRORLOG(errorcode, "ID definitions and reference enums are out-of-sync");
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvIdValue = idxRec->Value;
        *ospvIdLength = idxRec->ValueLength;
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        *ospvIdValue = OSPC_OSNULL;
        *ospvIdLength = 0;
    }

    return errorcode;
}
