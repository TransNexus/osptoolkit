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

/* ospcallid.h - OSP call identifier objects */
#ifndef _OSPSTIR_H
#define _OSPSTIR_H

/* SDP finger print structure */
typedef struct {
    OSPTLISTLINK Link;
    char Value[OSPC_SIZE_NORSTR];
} OSPT_SDP_FINGERPRINT;

/* Identity structure */
typedef struct {
    unsigned SignSize;
    unsigned char IdSign[OSPC_SIZE_SIGNSTR];
    char IdAlg[OSPC_SIZE_ALGSTR];
    char IdInfo[OSPC_SIZE_NORSTR];
    char IdSpec[OSPC_SIZE_NORSTR];
    char IdCanon[OSPC_SIZE_NORSTR];
} OSPT_IDENTITY;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

#define OSPPHasFingerPrint(list)                (OSPPFirstFingerPrint(list) != OSPC_OSNULL)
#define OSPPFirstFingerPrint(list)              ((OSPT_SDP_FINGERPRINT *)OSPPListFirst(&list))
#define OSPPNextFingerPrint(list, fingerprint)  ((OSPT_SDP_FINGERPRINT *)OSPPListNext(&list, fingerprint))

OSPT_SDP_FINGERPRINT *OSPPFingerPrintNew(const char *);
int OSPPFingerPrintToElement(OSPT_SDP_FINGERPRINT *, OSPT_XML_ELEM **);

int OSPPIdentityToElement(OSPT_IDENTITY *, OSPT_XML_ELEM **);
int OSPPIdentityFromElement(OSPT_XML_ELEM *, OSPT_IDENTITY *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPSTIR_H */
