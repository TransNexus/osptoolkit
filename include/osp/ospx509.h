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
 * ospx509.h - Constants and prototypes for X509 Certificate object.
 */

#ifndef _OSPX509_H
#define _OSPX509_H

#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/ospx500.h"
#include "osp/osppkcs1.h"

#define OSPC_CERT_STATE_START               0
#define OSPC_CERT_STATE_TLV                 1
#define OSPC_CERT_STATE_DISPLAY_CERT        2
#define OSPC_CERT_STATE_FATAL               9998
#define OSPC_CERT_STATE_EXIT                9999

#define OSPC_CERT_MAX_EXTENSIONS            10
#define OSPC_CERT_MAX_FLD_LENGTH            1000
#define OSPC_CERT_MAX_SERIALNUM_LENGTH      20
#define OSPC_CERT_MAX_SIG_ALGORITHM_LENGTH  20
#define OSPC_CERT_MAX_REQUEST_ATTRIBUTES    10

typedef struct {
    OSPTOBJECTID ExtnId;
    OSPTBOOL Critical;
    OSPTASN1BUFFER *ExtnValue;
} OSPTCERTEXT;

/*
 * X509 certificate typedef
 */

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    int OSPPX509CertGetCustDeviceId(OSPTASN1OBJECT *ospvCertificate, unsigned long *ospvCustomerId, unsigned long *ospvDeviceId);
    int OSPPX509CertIsParentCertificate(OSPTASN1OBJECT *ospvParentCertificate, OSPTASN1OBJECT *ospvChildCertficate);
    int OSPPX509CertCheckCertificateData(OSPTASN1OBJECT *ospvTestCertificate, OSPTASN1OBJECT *ospvSignerPublicKey);
    int OSPPX509CertValidateCertificate(OSPTASN1OBJECT *ospvTestCertificate, OSPTASN1OBJECT *ospvAuthorityCertificates[],
            unsigned int ospvNumberOfAuthorityCertificates, int *ospvParentIndex);
    int OSPPX509CertTestContext(OSPTASN1OBJECT *ospvCertInfo);
    int OSPPX509CertCreate(unsigned char *, OSPTASN1OBJECT **);
    int OSPPX509CertDelete(OSPTASN1OBJECT **);
    int OSPPX509CertGetElement(OSPTASN1OBJECT *ospvCertInfo, OSPEASN1DATAREFID ospvDataRefId, OSPTASN1ELEMENTINFO **ospvElementInfo);
    int OSPPX509CertGetCertificate(OSPTASN1OBJECT *ospvCertInfo,    /* In - Certificate context */
            unsigned char **ospvCertificate, unsigned *ospvCertificateLength);
    int OSPPX509CertCreate(unsigned char *ospvCertificate, OSPTASN1OBJECT **ospvCertInfo);
    int OSPPX509CertSetCertificate(OSPTASN1OBJECT *ospvCertInfo, unsigned char *ospvEncodedCertificate);

#ifdef __cplusplus
}
#endif

#endif /* _OSPX509_H */
