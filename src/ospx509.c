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
 * ospx509.c - Member functions for X509 Certificate object.
 */

#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/osppkcs1.h"
#include "osp/ospx509.h"
#include "osp/ospx500.h"
#include "osp/osptnlog.h"
#include "osp/ospostime.h"
#include "osp/ospcrypto.h"



/* ---------------------------------------------------------*/
/* Member functions                                         */
/* ---------------------------------------------------------*/

#define PROVIDERDOMAIN "transnexus.com"
#define PROVIDERINFO   "transnexus.com %ld %ld"

int
OSPPX509CertGetCustDeviceId(
    OSPTASN1OBJECT *ospvCertificate,
    unsigned long *ospvCustomerId,
    unsigned long *ospvDeviceId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    char *domainName = PROVIDERDOMAIN;
    unsigned domainNameLength = sizeof(PROVIDERDOMAIN)-1;
    unsigned dataLength = 0;
    char *data = OSPC_OSNULL;
    char *cptr = OSPC_OSNULL; 
    char *lcptr = OSPC_OSNULL;
    char *pname = OSPC_OSNULL;
    char *value = OSPC_OSNULL;

    *ospvCustomerId = 0;
    *ospvDeviceId = 0;

    /* Get the subject element */
    errorcode = OSPPX509CertTestContext(ospvCertificate);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get pointer to parse result that is head of subject name */
        errorcode = OSPPASN1ObjectGetElementByDataRef(ospvCertificate, &eInfo, 
            OSPEDRID_CERT_SUBJECT);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPASN1ElementGetElementData(eInfo,
                (unsigned char **)&value,&dataLength);
        }


        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            OSPM_MALLOC(data, char, dataLength+1);

            if (data == OSPC_OSNULL)
            {
                errorcode = OSPC_ERR_X509_UNABLE_TO_ALLOCATE_SPACE;
                OSPM_DBGERRORLOG(errorcode, "Unable to allocate space");
            }

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                OSPM_MEMCPY(data, value, dataLength);
                data[dataLength] = '\0';
            }
        }

    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Search for the string in domainName */
        for (cptr = data; *cptr; cptr++)
        {
            /* Looking for a [ */
            if (*cptr != '[')
            {
                /* Not found */
                continue;
            }

            /* Found a [, is it followed by the domain name? */
            cptr++;
            if (OSPM_STRNCMP(cptr, domainName, domainNameLength))
            {
                /* Nope */
                continue;
            }

            /* Domain Name Found, we ARE in the parameter OU element */
            pname = OSPM_STRTOK((char *)cptr, " ", &lcptr); /* Skip domainname */
            while(pname)
            {
                /* Get the next parameter name */
                pname = OSPM_STRTOK(OSPC_OSNULL, " :]", &lcptr);
                if (pname == OSPC_OSNULL)
                {
                    /* Parameter was not found, done */
                    break;
                }

                /* Have a parameter name, get the value */
                value = OSPM_STRTOK(OSPC_OSNULL, " :]", &lcptr);
                if (value == OSPC_OSNULL)
                {
                    /* Badly formed paramter value pair, done */
                    break;
                }

                /* We have a parameter and a value - which one? */
                if (!OSPM_STRNCMP(pname, "GWID", 4))
                {
                    /* Found a gateway id */
                    *ospvDeviceId = atol(value);
                    continue;
                }

                if (!OSPM_STRNCMP(pname, "CSID", 4))
                {
                    /* Found a customer id */
                    *ospvCustomerId = atol(value);
                    continue;
                }
            }

            break;
        }

    }
    OSPPASN1ElementDelete(&eInfo, 0);
    OSPM_FREE(data);

    return errorcode;
}


int
OSPPX509CertCheckCertificateData(
    OSPTASN1OBJECT *ospvCertInfo,
    OSPTASN1OBJECT *ospvSignerPublicKey)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    unsigned char *tbsCertificate = OSPC_OSNULL;
    unsigned int tbsCertificateLength = 0;
    OSPTASN1OBJECT *signature = OSPC_OSNULL;
    /* OSPTASN1OBJECT *publicKey = OSPC_OSNULL; */
    OSPTASN1OBJECT *certInfo = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *elementInfo = OSPC_OSNULL;
    char currentDate[OSPC_TIMESTRINGSIZE] = "";
    char notBeforeString[OSPC_TIMESTRINGSIZE] = "";
    char notAfterString[OSPC_TIMESTRINGSIZE] = "";
    OSPTTIME currentTime = 0;
    char *century = OSPC_OSNULL;
    unsigned char *date = OSPC_OSNULL;
    unsigned int  dateLength = 0;
    unsigned char *certificate = OSPC_OSNULL;
    unsigned int certificateLength = 0;

    errorcode = OSPPASN1ObjectGetElementInfo(ospvCertInfo, &elementInfo);

    memset(&notBeforeString,0,sizeof(notBeforeString));
    memset(&notAfterString,0,sizeof(notAfterString));

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Reparse the certificate */
        errorcode = OSPPASN1ElementGetElementData(elementInfo,
            &certificate, &certificateLength);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPX509CertCreate(certificate, &certInfo);
        }
    }


    /* To validate the certificate you need to verify the certificate
    signature using teh TBSCertificate portion of the certificate being
    validated.  You also need to compre the not-before and not-after dates
    against the current date.  
    */
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Check the certificate's signature */
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get the "to be signed" part of the certificate */
            errorcode = OSPPASN1ObjectGetElementByDataRef(certInfo, 
                &elementInfo, OSPEDRID_CERT_TBSCERTIFICATE);

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPASN1ElementGetElementData(elementInfo,
                    &tbsCertificate,
                    &tbsCertificateLength);
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Get the signature from certificate */
            errorcode = OSPPASN1ObjectCopyElementObject(&signature,
                certInfo, OSPEDRID_CERT_SIGNATURE);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPCryptoVerify( signature, ospvSignerPublicKey,
                tbsCertificate, tbsCertificateLength); 
        }

        OSPM_FREE(tbsCertificate);  
        OSPPASN1ElementDelete(&(signature->ParseResults->ElementInfo),0);
        OSPPASN1ObjectDelete(&signature);

        /* OSPPASN1ElementDelete(&(publicKey->ParseResults->ElementInfo),0); */
        /* OSPPASN1ObjectDelete(&publicKey); */
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Signature must be good.  Check validity dates */

        /* Get the current time - don't need milliseconds */
        errorcode = OSPPOSTimeGetTime(&currentTime, 
            OSPC_OSNULL);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Format the current time in YYYYMMDDhhmmssZ format */
            errorcode = OSPPOSTimeFormatGMTTime(currentTime, "%Y%m%d%H%M%SZ", 
                currentDate);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {   
            if(elementInfo) 
            {
                OSPM_FREE(elementInfo);
            }
            /* Get the Not before time from the certificate */  
            errorcode = OSPPASN1ObjectGetElementByDataRef(certInfo,
                &elementInfo, OSPEDRID_CERT_NOTBEFORE);

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPASN1ElementGetContentData(elementInfo,
                    &date, &dateLength);
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                    if (dateLength != 13)
                    {
                        errorcode = OSPC_ERR_X509_INVALID_DATE;
                        OSPM_DBGERRORLOG(errorcode, 
                            "Date is not 13 characters long");
                    }
                    else
                    {
                        /* Format the not before date */
                        century = (OSPM_MEMCMP(date, "49", 2) > 0)? "19" : "20";
                        OSPM_STRCPY(notBeforeString,century);
                        OSPM_STRNCAT(notBeforeString,(char *)date,13);
                        /*OSPM_SPRINTF(notBeforeString,"%s%13s.13s", century, date);*/
                    }
                }
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            if(elementInfo) /* !!! PS */
            {
                OSPM_FREE(elementInfo->Element);
                elementInfo->ElementLength = 0;
                OSPM_FREE(elementInfo);
            }
            /* Get the Not after time from the certificate */   
            errorcode = OSPPASN1ObjectGetElementByDataRef( certInfo, 
                &elementInfo, OSPEDRID_CERT_NOTAFTER);

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPASN1ElementGetContentData(elementInfo,
                    &date, &dateLength);
                if (errorcode == OSPC_ERR_NO_ERROR)
                {
                    if (dateLength != 13)
                    {
                        errorcode = OSPC_ERR_X509_INVALID_DATE;
                        OSPM_DBGERRORLOG(errorcode, 
                            "Date is not 13 characters long");
                    }
                    else
                    {
                        /* Format the not after date */
                        century = (OSPM_MEMCMP(date, "49", 2) > 0)? "19" : "20";
                        OSPM_STRCPY(notAfterString,century);
                        OSPM_STRNCAT(notAfterString,(char *)date,13);
                        /*OSPM_SPRINTF(notAfterString,"%s%13s.13s", century, date);*/
                    }
                }
            }
            if(elementInfo) /* !!! PS */
            {
                OSPM_FREE(elementInfo->Element);
                elementInfo->ElementLength = 0;
                OSPM_FREE(elementInfo);
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* At this time, Only UTC times are supported.  Format is
            YYMMDDHHMMSSZ.  Accorting to X509, if YY is less than 50, then the
            century is 2000.  If date is greater than 50, century is 1900.
            After 2050, generalized time will be used (not supported), and that
            will eliminate the problem since centurey is explicit in date. */

            if ((OSPM_MEMCMP(currentDate,notBeforeString,
                OSPM_STRLEN(currentDate))<0) ||
                (OSPM_MEMCMP(currentDate,notAfterString,
                OSPM_STRLEN(currentDate))>0)) 
            {
                errorcode = OSPC_ERR_X509_CERTIFICATE_EXPIRED;
#ifdef IGNOREOUTOFDATEERR
                errorcode = OSPC_ERR_NOERROR;
#endif
                OSPM_DBGERRORLOG(errorcode, 
                    "Current date is outside certificate validity dates");
            }
        }
    }

    OSPPASN1ObjectDelete(&certInfo);
    return errorcode;
} /* OSPPX509CertCheckCertificateData */



int
OSPPX509CertValidateCertificate(
    OSPTASN1OBJECT *ospvTestCertificate,
    OSPTASN1OBJECT *ospvAuthorityCertificates[],
    unsigned int    ospvNumberOfAuthorityCertificates,
    int             *ospvParentCertificateIndex)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *publicKey = OSPC_OSNULL;
    unsigned int i = 0;


    /* Loop through Authority Certificates to find issuer for testCert */
    for (i = 0; 
        (errorcode == OSPC_ERR_NO_ERROR || errorcode == OSPC_ERR_X509_CA_NOT_FOUND) && 
        (i < ospvNumberOfAuthorityCertificates); i ++)
    {
        /* Test ca certificate against test certificate */ 
        errorcode = OSPPX509CertIsParentCertificate(
            ospvAuthorityCertificates[i], 
            ospvTestCertificate);

        if (errorcode != OSPC_ERR_X509_CA_NOT_FOUND)
        {
            /* Found the ca for the test certificate or a serious error */
            break; 
        }

    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get the public key from certificate */
        errorcode = OSPPASN1ObjectCopyElementObject(&publicKey,
            ospvAuthorityCertificates[i], 
            OSPEDRID_CERT_SUBJPUBKEYINFO);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Check the test certificate to see if it is signed/not expired */
        errorcode = OSPPX509CertCheckCertificateData(ospvTestCertificate,
            publicKey);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Is authority certificate self signed? */
        if (OSPPX509CertIsParentCertificate(ospvAuthorityCertificates[i],
            ospvAuthorityCertificates[i]))
        {
            /* Yes - is is a Self Signed Certificate */
            /* Check ca certificate (self signed) to make sure it is valid */
            errorcode = OSPPX509CertCheckCertificateData(
                ospvAuthorityCertificates[i],
                publicKey);
        } 
        else
        {

            /* Nope, not self signed - continue down chain or return index
             of ca certificate */
            if (ospvParentCertificateIndex == OSPC_OSNULL)
            {
                /* Validate Certificate Chain */
                errorcode = OSPPX509CertValidateCertificate(
                    ospvAuthorityCertificates[i],
                    ospvAuthorityCertificates,
                    ospvNumberOfAuthorityCertificates,
                    OSPC_OSNULL);
            }
        }
    }
    if(OSPC_OSNULL!=publicKey)  /* !!! PS */
    {
        OSPPASN1ElementDelete(&(publicKey->ParseResults->ElementInfo),0);
        OSPPASN1ObjectDelete(&publicKey);
    }


    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* If supposed to return index, then set it here */
        if (ospvParentCertificateIndex != OSPC_OSNULL)
        {
            *ospvParentCertificateIndex = i;
        }
    }

    return errorcode;
}

int
OSPPX509CertIsParentCertificate(
    OSPTASN1OBJECT *ospvParentCertificate,
    OSPTASN1OBJECT *ospvTestCertificate)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1OBJECT *parentCertInfo = OSPC_OSNULL;
    OSPTASN1OBJECT *testCertInfo = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *elementInfo1 = OSPC_OSNULL;    /* !!! PS */
    OSPTASN1ELEMENTINFO *elementInfo2 = OSPC_OSNULL;    /* !!! PS */
    unsigned char *certificate = OSPC_OSNULL;
    unsigned int certificateLength = 0;
    unsigned char *subjectName = OSPC_OSNULL;
    unsigned int subjectNameLength = 0;
    unsigned char *issuerName = OSPC_OSNULL;
    unsigned int issuerNameLength = 0;

    /* Reparse the superior certificate */
    errorcode = OSPPASN1ObjectGetElementInfo(ospvParentCertificate, 
        &elementInfo1);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Reparse the certificate */
        errorcode = OSPPASN1ElementGetElementData(elementInfo1,
            &certificate, &certificateLength);

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPX509CertCreate(certificate, &parentCertInfo);
        }

        /* Reparse the test certificate */
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPASN1ObjectGetElementInfo(ospvTestCertificate, 
                &elementInfo1);
        }

        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            /* Reparse the certificate */
            errorcode = OSPPASN1ElementGetElementData(elementInfo1,
                &certificate, &certificateLength);

            if (errorcode == OSPC_ERR_NO_ERROR)
            {
                errorcode = OSPPX509CertCreate(certificate, &testCertInfo);
            }
        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get the parent certificate subject name */
        errorcode = OSPPASN1ObjectGetElementByDataRef(parentCertInfo,
            &elementInfo1, 
            OSPEDRID_CERT_SUBJECT);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPASN1ElementGetElementData(elementInfo1,
                &subjectName, &subjectNameLength);

        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get the test certificate issuer name */
        errorcode = OSPPASN1ObjectGetElementByDataRef(testCertInfo,
            &elementInfo2, 
            OSPEDRID_CERT_ISSUER);
        if (errorcode == OSPC_ERR_NO_ERROR)
        {
            errorcode = OSPPASN1ElementGetElementData(elementInfo2,
                &issuerName, &issuerNameLength);

        }
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Now we have both Names.  For now, assume they are formatted
        exactly the same on both certificates.  Should be able to do a
        memcmp to make sure they are the same. If there is a possibility of
        encoding differences (possible?) then it might be necessary to
        add code to take each name apart and search and compare elements. */

        errorcode = OSPC_ERR_X509_CA_NOT_FOUND;
        if (issuerNameLength == subjectNameLength)
        {
            if (OSPM_MEMCMP(issuerName, subjectName, issuerNameLength) == 0)
            {
                errorcode = OSPC_ERR_NO_ERROR;
            }
        }
    }

    if(OSPC_OSNULL!=elementInfo1)   /* !!! PS */
    {
        OSPPASN1ElementDelete(&elementInfo1,0);
    }
    if(OSPC_OSNULL!=elementInfo2)   /* !!! PS */
    {
        OSPPASN1ElementDelete(&elementInfo2,0);
    }
    OSPPASN1ObjectDelete(&parentCertInfo);
    OSPPASN1ObjectDelete(&testCertInfo);
    return errorcode;
} /* OSPPX509CertIsParentCertificate */


int
OSPPX509CertGetCertificate(
    OSPTASN1OBJECT *ospvCertInfo,
    unsigned char **ospvCertificate,
    unsigned int  *ospvCertificateLength)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    *ospvCertificate = OSPC_OSNULL;
    *ospvCertificateLength = 0;

    errorcode = OSPPX509CertGetElement( ospvCertInfo, 
        OSPEDRID_CERTIFICATE, 
        &eInfo);
    if (eInfo != OSPC_OSNULL)
    {
        *ospvCertificate = eInfo->Element;
        *ospvCertificateLength = eInfo->ElementLength;
    }

    return errorcode;
}



int
OSPPX509CertGetElement(
    OSPTASN1OBJECT *ospvCertInfo,           /* In - X509 Cert Context */
    OSPEASN1DATAREFID ospvDataRefId,
    OSPTASN1ELEMENTINFO **ospvElementInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *foundElement = OSPC_OSNULL;

    errorcode = OSPPX509CertTestContext(ospvCertInfo);
    if ( errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPASN1ElementGet(ospvDataRefId, 
            ospvCertInfo->ParseResults, &foundElement);
    }

    *ospvElementInfo = foundElement;

    return errorcode;
}


int
OSPPX509CertTestContext(
    OSPTASN1OBJECT *ospvCertInfo)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvCertInfo == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_X509_INVALID_CONTEXT;
        OSPM_PRINTTOERR((stderr,"X509 CertInfo context is null pointer\n"));
    }

    return errorcode;
}



int
OSPPX509CertCreate(
    unsigned char *ospvEncodedCertificate,
    OSPTASN1OBJECT **ospvCert)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPM_MALLOC(*ospvCert, OSPTASN1OBJECT, sizeof(OSPTASN1OBJECT));
    if (*ospvCert == OSPC_OSNULL)
    {
        errorcode = OSPC_ERR_CERT_MALLOC_FAILED;
        OSPM_DBGERRORLOG(errorcode, 
            "malloc of new X509 Certificate failed");
    }
    else
    {
        OSPM_MEMSET(*ospvCert, 0, sizeof(OSPTASN1OBJECT));
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPX509CertSetCertificate(*ospvCert, 
            ospvEncodedCertificate);
    }

    return errorcode;
}


int
OSPPX509CertSetCertificate(
    OSPTASN1OBJECT *ospvCert,
    unsigned char *ospvEncodedCertificate)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;
    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;

    errorcode = OSPPX509CertTestContext(ospvCert);

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get rid of the old certificate data */
        OSPPASN1ElementDelete(&(ospvCert->ElementInfo),0);
        OSPPASN1ElementParseDelete(&(ospvCert->ParseResults));
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPASN1ElementDecode(ospvEncodedCertificate, &eInfo, 0);
    }

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        errorcode = OSPPASN1ElementParse( eInfo, OSPEPTID_CERTIFICATE, 
            OSPC_OSNULL,
            &parseResults, 
            OSPC_ASN1_DATAREFID_CERTIFICATE);
    }   

    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        ospvCert->ElementInfo = eInfo;
        ospvCert->ParseResults = parseResults;
    }
    else
    {
        if (eInfo) 
        {
            OSPPASN1ElementDelete(&eInfo,0);
        }

        if (parseResults) 
        {
            PTPResultsDelete(&parseResults);
        }
    }


    return errorcode;
}

int
OSPPX509CertDelete(
    OSPTASN1OBJECT **ospvCert)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    errorcode = OSPPX509CertTestContext(*ospvCert);
    if (errorcode == OSPC_ERR_NO_ERROR)
    {
        /* Get rid of the old certificate data */
        OSPPASN1ObjectDelete(ospvCert);
    }
    return(errorcode);
}

