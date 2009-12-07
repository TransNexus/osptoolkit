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

/* ospasn1.h - Constants and typedefs for ASN.1 */
#ifndef _OSPASN1ID_H
#define _OSPASN1ID_H

#include "osp/ospasn1.h"

/*
 * NOTE: The following definitions are used to index the ospgASN1IDIndex
 * table below.  DO NOT MODIFY ONE WITHOUT MODIFYING THE OTHER
 */
typedef enum _OSPEASN1ID {
    OSPEID_RSAENCRYPTION,
    OSPEID_MD2WITHRSA,
    OSPEID_MD4WITHRSA,
    OSPEID_MD5WITHRSA,
    OSPEID_DKKEYAGREEMENT,
    OSPEID_PBEWITHMD2ANDDES_CBC,
    OSPEID_PBEWITHMD5ANDDES_CBC,
    OSPEID_EMAILADDRESS,
    OSPEID_UNSTRUCTUREDNAME,
    OSPEID_CONTENTTYPE,
    OSPEID_MESSAGEDIGEST,
    OSPEID_SIGNINGTIME,
    OSPEID_COUNTERSIGNATURE,
    OSPEID_CHALLENGEPASSWORD,
    OSPEID_UNSTRUCTUREDADDRESS,
    OSPEID_EXTENDEDCERTIFICATEATTRIBUTES,
    OSPEID_OBJECTCLASS,
    OSPEID_ALIASEDOBJECTNAME,
    OSPEID_KNOWLEDGEINFORMATION,
    OSPEID_COMMONNAME,
    OSPEID_SURNAME,
    OSPEID_SERIALNUMBER,
    OSPEID_COUNTRYNAME,
    OSPEID_LOCALITYNAME,
    OSPEID_STATEPROVINCENAME,
    OSPEID_STREETADDRESS,
    OSPEID_ORGANIZATIONNAME,
    OSPEID_ORGANIZATIONALUNITNAME,
    OSPEID_TITLE,
    OSPEID_DESCRIPTION,
    OSPEID_BUSINESSCATEGORY,
    OSPEID_SEARCHGUIDE,
    OSPEID_POSTALADDRESS,
    OSPEID_POSTALCODE,
    OSPEID_POSTOFFICEBOX,
    OSPEID_PHYSICALDELIVERYOFFICENAME,
    OSPEID_TELEPHONENUMBER,
    OSPEID_TELEXNUMBER,
    OSPEID_TELEXTERMINALIDENTIFIER,
    OSPEID_FACSIMILETELEPHONENUMBER,
    OSPEID_X121ADDRESS,
    OSPEID_INTERNATIONALISDNNUMBER,
    OSPEID_REGISTEREDADDRESS,
    OSPEID_DESTINATIONINDICATOR,
    OSPEID_PREFERREDDELIVERYMETHOD,
    OSPEID_PRESENTATIONADDRESS,
    OSPEID_SUPPORTEDAPPLICATIONCONTEXT,
    OSPEID_MEMBER,
    OSPEID_OWNER,
    OSPEID_ROLEOCCUPANT,
    OSPEID_SEEALSO,
    OSPEID_USERPASSWORD,
    OSPEID_USERCERTIFICATE,
    OSPEID_CACERTIFICATE,
    OSPEID_AUTHORITYREVOCATIONLIST,
    OSPEID_CERTIFICATEREVOCATIONLIST,
    OSPEID_CROSSCERTIFICATEPAIR,
    OSPEID_SIGNEDDATA,
    OSPEID_DATA,
    OSPEID_MD2,
    OSPEID_MD4,
    OSPEID_MD5,
    OSPEID_LISTEND
} OSPEASN1ID;

#ifdef OSPC_OSPASN1ID_INCLUDE_STATIC
/*
 * The following data is only used in ospasn1id.c where the #define is defined
 */

/*
 * The object id values are already encoded in ASN1 Format where:
 * The first octet is 40*v1 + v2,
 * Subsequent octets contain v3, ... vn, where:
 * each value is encoded base in base 128 with MSB of each octet
 * except the last set to 1.
 */

/*
 * AlgorithmId encoded values contain only the oid and attribute types and values. The sequence prefix is not included in the definition
 */
#define OSPM_PKCS1  42, 134, 72, 134, 247, 13, 1

static unsigned char ospgRSAEncryption[] = { OSPM_PKCS1, 1, 1 };
static unsigned char ospgMD2WithRSA[] = { OSPM_PKCS1, 1, 2 };
static unsigned char ospgMD4WithRSA[] = { OSPM_PKCS1, 1, 3 };
static unsigned char ospgMD5WithRSA[] = { OSPM_PKCS1, 1, 4 };
static unsigned char ospgDKKeyAgreement[] = { OSPM_PKCS1, 3, 1 };
static unsigned char ospgPBEWithMD2AndDES_CBC[] = { OSPM_PKCS1, 5, 1 };
static unsigned char ospgPBEWithMD5AndDES_CBC[] = { OSPM_PKCS1, 5, 3 };
static unsigned char ospgEmailAddress[] = { OSPM_PKCS1, 9, 1 };
static unsigned char ospgUnstructuredName[] = { OSPM_PKCS1, 9, 2 };
static unsigned char ospgContentType[] = { OSPM_PKCS1, 9, 3 };
static unsigned char ospgMessageDigest[] = { OSPM_PKCS1, 9, 4 };
static unsigned char ospgSigningTime[] = { OSPM_PKCS1, 9, 5 };
static unsigned char ospgCounterSignature[] = { OSPM_PKCS1, 9, 6 };
static unsigned char ospgChallengePassword[] = { OSPM_PKCS1, 9, 7 };
static unsigned char ospgUnstructuredAddress[] = { OSPM_PKCS1, 9, 8 };
static unsigned char ospgExtendedCertificateAttributes[] = { OSPM_PKCS1, 9, 9 };

#define OSPM_X520   85, 4

static unsigned char ospgObjectClass[] = { OSPM_X520, 0 };
static unsigned char ospgAliasedObjectName[] = { OSPM_X520, 1 };
static unsigned char ospgKnowledgeInformation[] = { OSPM_X520, 2 };
static unsigned char ospgCommonName[] = { OSPM_X520, 3 };
static unsigned char ospgSurName[] = { OSPM_X520, 4 };
static unsigned char ospgSerialNumber[] = { OSPM_X520, 5 };
static unsigned char ospgCountryName[] = { OSPM_X520, 6 };
static unsigned char ospgLocalityName[] = { OSPM_X520, 7 };
static unsigned char ospgStateProvinceName[] = { OSPM_X520, 8 };
static unsigned char ospgStreetAddress[] = { OSPM_X520, 9 };
static unsigned char ospgOrganizationName[] = { OSPM_X520, 10 };
static unsigned char ospgOrganizationalUnitName[] = { OSPM_X520, 11 };
static unsigned char ospgTitle[] = { OSPM_X520, 12 };
static unsigned char ospgDescription[] = { OSPM_X520, 13 };
static unsigned char ospgSearchGuide[] = { OSPM_X520, 14 };
static unsigned char ospgBusinessCategory[] = { OSPM_X520, 15 };
static unsigned char ospgPostalAddress[] = { OSPM_X520, 16 };
static unsigned char ospgPostalCode[] = { OSPM_X520, 17 };
static unsigned char ospgPostOfficeBox[] = { OSPM_X520, 18 };
static unsigned char ospgPhysicalDeliveryOfficeName[] = { OSPM_X520, 19 };
static unsigned char ospgTelephoneNumber[] = { OSPM_X520, 20 };
static unsigned char ospgTelexNumber[] = { OSPM_X520, 21 };
static unsigned char ospgTelexTerminalIdentifier[] = { OSPM_X520, 22 };
static unsigned char ospgFacsimileTelephoneNumber[] = { OSPM_X520, 23 };
static unsigned char ospgX_121Address[] = { OSPM_X520, 24 };
static unsigned char ospgInternationalISDNNumber[] = { OSPM_X520, 25 };
static unsigned char ospgRegisteredAddress[] = { OSPM_X520, 26 };
static unsigned char ospgDestinationIndicator[] = { OSPM_X520, 27 };
static unsigned char ospgPreferredDeliveryMethod[] = { OSPM_X520, 28 };
static unsigned char ospgPresentationAddress[] = { OSPM_X520, 29 };
static unsigned char ospgSupportedApplicationContext[] = { OSPM_X520, 30 };
static unsigned char ospgMember[] = { OSPM_X520, 31 };
static unsigned char ospgOwner[] = { OSPM_X520, 32 };
static unsigned char ospgRoleOccupant[] = { OSPM_X520, 33 };
static unsigned char ospgSeeAlso[] = { OSPM_X520, 34 };
static unsigned char ospgUserPassword[] = { OSPM_X520, 35 };
static unsigned char ospgUserCertificate[] = { OSPM_X520, 36 };
static unsigned char ospgCACertificate[] = { OSPM_X520, 37 };
static unsigned char ospgAuthorityRevocationList[] = { OSPM_X520, 38 };
static unsigned char ospgCertificateRevocationList[] = { OSPM_X520, 39 };
static unsigned char ospgCrossCertificatePair[] = { OSPM_X520, 40 };

static unsigned char ospgData[] = { OSPM_PKCS1, 7, 1 };
static unsigned char ospgSignedData[] = { OSPM_PKCS1, 7, 2 };

#define OSPM_DIGESTALGORITHM    42, 134, 72, 134, 247, 13, 2

static unsigned char ospgMD2[] = { OSPM_DIGESTALGORITHM, 2 };
static unsigned char ospgMD4[] = { OSPM_DIGESTALGORITHM, 4 };
static unsigned char ospgMD5[] = { OSPM_DIGESTALGORITHM, 5 };
static unsigned char ospgNotAnId[] = { 0, 0 };

typedef struct _OSPTIDINDEX {
    OSPEASN1ID Id;
    unsigned char *Value;
    unsigned ValueLength;
} OSPTIDINDEX;

/*
 * NOTE: The following indexed are referenced using the value of the enums
 * defined in OSPEASNID above.  DO NOT MODIFY ONE WITHOUT MODIFYING THE OTHER
 */
#define OSPM_IDIDX(a,b) {a, b, sizeof(b)}

static OSPTIDINDEX ospgASN1IDIndex[] = {

    OSPM_IDIDX(OSPEID_RSAENCRYPTION, ospgRSAEncryption),
    OSPM_IDIDX(OSPEID_MD2WITHRSA, ospgMD2WithRSA),
    OSPM_IDIDX(OSPEID_MD4WITHRSA, ospgMD4WithRSA),
    OSPM_IDIDX(OSPEID_MD5WITHRSA, ospgMD5WithRSA),
    OSPM_IDIDX(OSPEID_DKKEYAGREEMENT, ospgDKKeyAgreement),
    OSPM_IDIDX(OSPEID_PBEWITHMD2ANDDES_CBC, ospgPBEWithMD2AndDES_CBC),
    OSPM_IDIDX(OSPEID_PBEWITHMD5ANDDES_CBC, ospgPBEWithMD5AndDES_CBC),
    OSPM_IDIDX(OSPEID_EMAILADDRESS, ospgEmailAddress),
    OSPM_IDIDX(OSPEID_UNSTRUCTUREDNAME, ospgUnstructuredName),
    OSPM_IDIDX(OSPEID_CONTENTTYPE, ospgContentType),
    OSPM_IDIDX(OSPEID_MESSAGEDIGEST, ospgMessageDigest),
    OSPM_IDIDX(OSPEID_SIGNINGTIME, ospgSigningTime),
    OSPM_IDIDX(OSPEID_COUNTERSIGNATURE, ospgCounterSignature),
    OSPM_IDIDX(OSPEID_CHALLENGEPASSWORD, ospgChallengePassword),
    OSPM_IDIDX(OSPEID_UNSTRUCTUREDADDRESS, ospgUnstructuredAddress),
    OSPM_IDIDX(OSPEID_EXTENDEDCERTIFICATEATTRIBUTES, ospgExtendedCertificateAttributes),
    OSPM_IDIDX(OSPEID_OBJECTCLASS, ospgObjectClass),
    OSPM_IDIDX(OSPEID_ALIASEDOBJECTNAME, ospgAliasedObjectName),
    OSPM_IDIDX(OSPEID_KNOWLEDGEINFORMATION, ospgKnowledgeInformation),
    OSPM_IDIDX(OSPEID_COMMONNAME, ospgCommonName),
    OSPM_IDIDX(OSPEID_SURNAME, ospgSurName),
    OSPM_IDIDX(OSPEID_SERIALNUMBER, ospgSerialNumber),
    OSPM_IDIDX(OSPEID_COUNTRYNAME, ospgCountryName),
    OSPM_IDIDX(OSPEID_LOCALITYNAME, ospgLocalityName),
    OSPM_IDIDX(OSPEID_STATEPROVINCENAME, ospgStateProvinceName),
    OSPM_IDIDX(OSPEID_STREETADDRESS, ospgStreetAddress),
    OSPM_IDIDX(OSPEID_ORGANIZATIONNAME, ospgOrganizationName),
    OSPM_IDIDX(OSPEID_ORGANIZATIONALUNITNAME, ospgOrganizationalUnitName),
    OSPM_IDIDX(OSPEID_TITLE, ospgTitle),
    OSPM_IDIDX(OSPEID_DESCRIPTION, ospgDescription),
    OSPM_IDIDX(OSPEID_SEARCHGUIDE, ospgSearchGuide),
    OSPM_IDIDX(OSPEID_BUSINESSCATEGORY, ospgBusinessCategory),
    OSPM_IDIDX(OSPEID_POSTALADDRESS, ospgPostalAddress),
    OSPM_IDIDX(OSPEID_POSTALCODE, ospgPostalCode),
    OSPM_IDIDX(OSPEID_POSTOFFICEBOX, ospgPostOfficeBox),
    OSPM_IDIDX(OSPEID_PHYSICALDELIVERYOFFICENAME, ospgPhysicalDeliveryOfficeName),
    OSPM_IDIDX(OSPEID_TELEPHONENUMBER, ospgTelephoneNumber),
    OSPM_IDIDX(OSPEID_TELEXNUMBER, ospgTelexNumber),
    OSPM_IDIDX(OSPEID_TELEXTERMINALIDENTIFIER, ospgTelexTerminalIdentifier),
    OSPM_IDIDX(OSPEID_FACSIMILETELEPHONENUMBER, ospgFacsimileTelephoneNumber),
    OSPM_IDIDX(OSPEID_X121ADDRESS, ospgX_121Address),
    OSPM_IDIDX(OSPEID_INTERNATIONALISDNNUMBER, ospgInternationalISDNNumber),
    OSPM_IDIDX(OSPEID_REGISTEREDADDRESS, ospgRegisteredAddress),
    OSPM_IDIDX(OSPEID_DESTINATIONINDICATOR, ospgDestinationIndicator),
    OSPM_IDIDX(OSPEID_PREFERREDDELIVERYMETHOD, ospgPreferredDeliveryMethod),
    OSPM_IDIDX(OSPEID_PRESENTATIONADDRESS, ospgPresentationAddress),
    OSPM_IDIDX(OSPEID_SUPPORTEDAPPLICATIONCONTEXT, ospgSupportedApplicationContext),
    OSPM_IDIDX(OSPEID_MEMBER, ospgMember),
    OSPM_IDIDX(OSPEID_OWNER, ospgOwner),
    OSPM_IDIDX(OSPEID_ROLEOCCUPANT, ospgRoleOccupant),
    OSPM_IDIDX(OSPEID_SEEALSO, ospgSeeAlso),
    OSPM_IDIDX(OSPEID_USERPASSWORD, ospgUserPassword),
    OSPM_IDIDX(OSPEID_USERCERTIFICATE, ospgUserCertificate),
    OSPM_IDIDX(OSPEID_CACERTIFICATE, ospgCACertificate),
    OSPM_IDIDX(OSPEID_AUTHORITYREVOCATIONLIST, ospgAuthorityRevocationList),
    OSPM_IDIDX(OSPEID_CERTIFICATEREVOCATIONLIST, ospgCertificateRevocationList),
    OSPM_IDIDX(OSPEID_CROSSCERTIFICATEPAIR, ospgCrossCertificatePair),
    OSPM_IDIDX(OSPEID_SIGNEDDATA, ospgSignedData),
    OSPM_IDIDX(OSPEID_DATA, ospgData),
    OSPM_IDIDX(OSPEID_MD2, ospgMD2),
    OSPM_IDIDX(OSPEID_MD4, ospgMD4),
    OSPM_IDIDX(OSPEID_MD5, ospgMD5),
    OSPM_IDIDX(OSPEID_LISTEND, ospgNotAnId)
};
#endif /* OSPC_OSPASN1_INCLUDE_STATIC */

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    int OSPPASN1IdGetValue(OSPEASN1ID ospvObjectId, unsigned char **ospvObjectIdValue, unsigned *ospvObjectIdLength);

#ifdef __cplusplus
}
#endif

#endif /* _OSPASN1ID_H */
