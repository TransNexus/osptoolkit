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

/* osputils.h - Constants and prototypes for utility functions. */
#ifndef _OSPUTILS_H
#define _OSPUTILS_H

#include "osp/ospdatatypes.h"

#define OSPC_MIN_TIMESTAMP_LEN  21
#define OSPC_MAX_RANDOM         256

#define OSPC_TEST_DEST_STRING   "www.transnexus.com"
#define OSPC_FAKE_TOKEN         ")&(^%&$^%#"

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPTBOOL OSPPUtilIsDottedNumericIP(const char *);
    int OSPPUtilGetErrorFromStatus(unsigned);
    int OSPPUtilGetRandom(char *, int);
    int OSPPUtilBuildString(OSPTUINT64, int, char **);
    void OSPPUtilStringToLowercase(char **);
    int OSPPUtilLoadPEMPrivateKey(unsigned char *fileName, OSPTPRIVATEKEY *key);
    int OSPPUtilLoadPEMCert(unsigned char *fileName, OSPT_CERT *cert);

#ifdef __cplusplus
}
#endif

#endif /* _OSPUTILS_H */
