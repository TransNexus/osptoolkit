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
 * osppkcs1.h - PKCS1 definitions used for key objects.
 */

#ifndef _OSP_PKCS1_H
#define _OSP_PKCS1_H

#include "osp.h"
#include "ospasn1.h"
#include "ospdatatypes.h"


#define OSPC_ASN1_PRIVATEKEYINFO_VERSION 0

#ifdef __cplusplus
extern "C" 
{
#endif

    int
    OSPPPKCS1PrivateKeyDelete(
        OSPTPRIVATEKEY **ospvPrivateKey);

    int
    OSPPPKCS1PrivateKeyCreate(
        OSPTPRIVATEKEY *ospvSourcePrivateKey,
        OSPTPRIVATEKEY **ospvNewPrivateKey);

    int
    OSPPPKCS1PrivateKeyCopy(
        OSPTPRIVATEKEY *ospvDestinationPrivateKey,
        OSPTPRIVATEKEY *ospvSourcePrivateKey);

    int
    OSPPPKCS1PrivateKeyTestContext(
        OSPTPRIVATEKEY *ospvPrivateKey);

    int
    OSPPPKCS1Encrypt(
        OSPTASN1OBJECT **ospvEncryptedDigest,
        OSPTASN1OBJECT *ospvAlgorithmId,
        OSPTASN1OBJECT *ospvDigestInfo,
        OSPTPRIVATEKEY *ospvPrivateKey);

#ifdef __cplusplus
}
#endif


#endif
