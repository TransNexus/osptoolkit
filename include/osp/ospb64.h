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

#ifndef _OSPB64_H
#define _OSPB64_H

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    int OSPPBase64Encode(const unsigned char *in, unsigned inSize, unsigned char *out, unsigned *outSize);
    int OSPPBase64Decode(const char *in, unsigned inSize, unsigned char *out, unsigned *outSize);

#ifdef __cplusplus
}
#endif

#endif /* _OSPB64_H */
