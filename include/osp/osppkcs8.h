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
 * osppkcs8.h - PKCS8 definitions used for key objects.
 */

#ifndef _OSP_PKCS8_H
#define _OSP_PKCS8_H

#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/ospdatatypes.h"


#define OSPC_PKCS8_PRIVATEKEYINFO_VERSION 0

#ifdef __cplusplus
extern "C" 
{
#endif
    int
    OSPPPKCS8KeyInfoCreate(
        OSPTASN1OBJECT **ospvPrivateKeyInfo,
        OSPTPRIVATEKEY *ospvPrivateKey);

    int
    OSPPPKCS8KeyInfoTestContext(
        OSPTASN1OBJECT  *ospvPrivateKeyInfo);

    void 
    OSPPPKCS8KeyInfoDelete(
        OSPTASN1OBJECT  **ospvPrivateKeyInfo);

    int
    OSPPPKCS8KeyInfoGetPrivateKey(
        OSPTASN1OBJECT *ospvPrivateKeyInfo,
        unsigned char **ospvPrivateKey,
        unsigned      *ospvPrivateKeyLength);

    int
    OSPPPKCS8KeyInfoGetPrivateKeyElement(
        OSPTASN1OBJECT *ospvPrivateKeyInfo, 
        OSPEASN1DATAREFID ospvDataRefId,
        OSPTASN1ELEMENTINFO **ospvElementInfo);

    int
    OSPPKCS8KeyInfoGetEncodedKeyInfo(
        OSPTASN1OBJECT *ospvPrivateKeyInfo,
        unsigned char **ospvBERPrivateKeyInfo,
        unsigned      *ospvBERPrivateKeyInfoLength);

#ifdef __cplusplus
}
#endif


#endif
