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
#ifndef _OSPASN1_H
#define _OSPASN1_H

#include "osp/osp.h"
#include "osp/ospasn1ids.h"

#define OSPC_ASN1_SIGNERINFO_VERSION    1

/* ASN1 ENCODED DATA KNOWN TO THE PARSER THAT GETS ACCESSED */
#define OSPC_ASN1_DATAREFID_CERTIFICATE                     0x01
#define OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE             0x01
#define     OSPC_ASN1_DATAREFID_CERT_EXPLICIT_VERSION       0x01
#define         OSPC_ASN1_DATAREFID_CERT_VERSION            0x01
#define     OSPC_ASN1_DATAREFID_CERT_SERIALNUMBER           0x02
#define     OSPC_ASN1_DATAREFID_CERT_SIGNATURE_TBS          0x03
#define     OSPC_ASN1_DATAREFID_CERT_ISSUER                 0x04
#define     OSPC_ASN1_DATAREFID_CERT_VALIDITY               0x05
#define     OSPC_ASN1_DATAREFID_CERT_NOTBEFORE              0x01
#define     OSPC_ASN1_DATAREFID_CERT_NOTAFTER               0x02
#define     OSPC_ASN1_DATAREFID_CERT_SUBJECT                0x06
#define     OSPC_ASN1_DATAREFID_CERT_SUBJPUBKEYINFO         0x07
#define         OSPC_ASN1_DATAREFID_CERT_PUBLICKEYALG       0x01
#define         OSPC_ASN1_DATAREFID_CERT_PUBLICKEY          0x02
#define         OSPC_ASN1_DATAREFID_CERT_ISSUERUNIQUEID     0x08
#define         OSPC_ASN1_DATAREFID_CERT_SUBJECTUNIQUEID    0x09
#define     OSPC_ASN1_DATAREFID_CERT_EXPLICIT_EXTENSIONS    0x0a
#define     OSPC_ASN1_DATAREFID_CERT_EXTENSIONS             0x01
#define     OSPC_ASN1_DATAREFID_CERT_EXTENSION              0x01
#define OSPC_ASN1_DATAREFID_CERT_SIGNATUREALGORITHM         0x02
#define OSPC_ASN1_DATAREFID_CERT_SIGNATURE                  0x03

/* ASN1 ENCODED CONTENT INFO ELEMENT */
#define OSPC_ASN1_DATAREFID_CONTENTINFO                     0x02
#define OSPC_ASN1_DATAREFID_CNTINF_CONTENTTYPE              0x01
#define OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT         0x02
#define OSPC_ASN1_DATAREFID_CNTINF_CONTENT                  0x01

/* ASN1 ENCODED SIGNED DATA ELEMENTS */
#define OSPC_ASN1_DATAREFID_SIGNEDDATA                      0x03
#define OSPC_ASN1_DATAREFID_SGNDAT_VERSION                  0x01
#define OSPC_ASN1_DATAREFID_SGNDAT_DIGESTALGORITHMS         0x02
#define     OSPC_ASN1_DATAREFID_SGNDAT_DIGESTALGORITHM      0x01
#define OSPC_ASN1_DATAREFID_SGNDAT_CONTENTINFO              0x03
#define OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATES             0x04
#define         OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATE      0x01
#define     OSPC_ASN1_DATAREFID_SGNDAT_CERTREVLISTS         0x05
#define         OSPC_ASN1_DATAREFID_SGNDAT_CERTREVLIST      0x01
#define OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFOS              0x06
#define     OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFO           0x01

/* ASN1 ENCODED SIGNER INFO ELEMENTS */
#define OSPC_ASN1_DATAREFID_SIGNERINFO                      0x04
#define     OSPC_ASN1_DATAREFID_SGNINF_VERSION              0x01
#define     OSPC_ASN1_DATAREFID_SGNINF_ISSUERANDSERNUM      0x02
#define         OSPC_ASN1_DATAREFID_SGNINF_ISSUER           0x01
#define         OSPC_ASN1_DATAREFID_SGNINF_SERIALNUMBER     0x02
#define     OSPC_ASN1_DATAREFID_SGNINF_DIGESTALGORITHM      0x03
#define     OSPC_ASN1_DATAREFID_SGNINF_AUTHATTRIBUTES       0x04
#define         OSPC_ASN1_DATAREFID_SGNINF_AUTHATTRIBUTE    0x01
#define     OSPC_ASN1_DATAREFID_SGNINF_DIGENCRYPTALG        0x05
#define     OSPC_ASN1_DATAREFID_SGNINF_ENCRYPTEDDIGEST      0x06
#define     OSPC_ASN1_DATAREFID_SGNINF_UNAUTHATTRIBUTES     0x07
#define         OSPC_ASN1_DATAREFID_SGNINF_UNAUTHATTRIBUTE  0x01

/* ASN1 ENCODED SIGNER INFO ELEMENTS */
#define OSPC_ASN1_DATAREFID_DIGESTINFO                      0x05
#define     OSPC_ASN1_DATAREFID_DIGINF_DIGESTALGORITHM      0x01
#define     OSPC_ASN1_DATAREFID_DIGINF_DIGEST               0x02

/* ASN1 ENCODED ALGORITHM ID ELEMENTS */
#define OSPC_ASN1_DATAREFID_ALGORITHMID                     0x06
#define     OSPC_ASN1_DATAREFID_ALGID_OID                   0x01
#define     OSPC_ASN1_DATAREFID_ALGID_ATTRTYPEANDVAL        0x02
#define     OSPC_ASN1_DATAREFID_ALGID_TERMINATOR            0x03

/* ASN1 ENCODED PRIVATE KEY INFO ELEMENTS */
#define OSPC_ASN1_DATAREFID_PRIVATEKEYINFO                  0x07
#define     OSPC_ASN1_DATAREFID_PVTKEYINF_VERSION           0x01
#define     OSPC_ASN1_DATAREFID_PVTKEYINF_ALGORITHM         0x02
#define     OSPC_ASN1_DATAREFID_PVTKEYINF_PRIVATEKEY        0x03
#define     OSPC_ASN1_DATAREFID_PVTKEYINF_ATTRIBUTES        0x04
#define         OSPC_ASN1_DATAREFID_PVTKEYINF_ATTRIBUTE     0x01

/*
 * These are defined for parsing a PKCS#10 certificate request.
 * A certificate request contains a CertificateRequestInfo structure
 * ( with the version, subject, public key, and optional attributes ) and
 * a signature.
 */
#define OSPC_ASN1_DATAREFID_CERTREQ                                0x01
#define     OSPC_ASN1_DATAREFID_CERTREQINFO                        0x01
#define         OSPC_ASN1_DATAREFID_CERTREQINFO_VERSION            0x01
#define         OSPC_ASN1_DATAREFID_CERTREQINFO_SUBJECT            0x02
#define         OSPC_ASN1_DATAREFID_CERTREQINFO_SUBJPUBKEYINFO     0x03
#define         OSPC_ASN1_DATAREFID_CERTREQINFO_ATTRIBUTES         0x04
#define     OSPC_ASN1_DATAREFID_CERTREQ_SIGNATURE_ALGORITHM        0x02
#define     OSPC_ASN1_DATAREFID_CERTREQ_SIGNATURE                  0x03

/*
 * The definition for a subjectPublicKeyInfo is separate from
 * that of those for the PKCS#10 certificate request and X.509 certificates.
 */
#define OSPC_ASN1_DATAREFID_SUBJPUBKEYINFO  0x01

/*
 * NOTE: THE OSPEASN1PARSETABLEID enum list must stay in sync with
 * ospgParseTableIndex[].  The enum values are the index into the
 * ParseTableIndex for the parse table array.
 */

typedef enum _OSPEASN1PARSETABLEID {
    /*
     * DO NOT CHANGE THIS TABLE WITHOUT UPDATING THE POSITIONS OF
     * THE PARSE TABLES IN ospgParseTableIndex
     */
    OSPEPTID_NOTDEFINED = 0,
    OSPEPTID_CERTIFICATE,
    OSPEPTID_TBSCERTIFICATE,
    OSPEPTID_SIGNATUREALGORITHM,
    OSPEPTID_DERALGORITHMID,
    OSPEPTID_EXPLICIT_VERSION,
    OSPEPTID_VERSION,
    OSPEPTID_CERTSERIALNUM,
    OSPEPTID_NAME,
    OSPEPTID_RDNSEQUENCE,
    OSPEPTID_RELDISNAME,
    OSPEPTID_ATTRTYPEANDVAL,
    OSPEPTID_ATTRIBUTETYPE,
    OSPEPTID_ATTRIBUTEVALUE,
    OSPEPTID_VALIDITY,
    OSPEPTID_TIME,
    OSPEPTID_UTCTIME,
    OSPEPTID_SUBJPUBKEYINFO,
    OSPEPTID_ISSUERUNIQUEID,
    OSPEPTID_SUBJECTUNIQUEID,
    OSPEPTID_UNIQUEID,
    OSPEPTID_EXPLICIT_EXTENSIONS,
    OSPEPTID_EXTENSIONS,
    OSPEPTID_EXTENSION,
    OSPEPTID_CONTENTINFO_DATA,
    OSPEPTID_EXPLICIT_DATA,
    OSPEPTID_DATA,
    OSPEPTID_CONTENTINFO_SIGNEDDATA,
    OSPEPTID_EXPLICIT_SIGNEDDATA,
    OSPEPTID_SIGNEDDATA,
    OSPEPTID_DIGESTALGORITHMS,
    OSPEPTID_CERTIFICATES,
    OSPEPTID_CERTREVLISTS,
    OSPEPTID_CERTREVLIST,
    OSPEPTID_SIGNERINFOS,
    OSPEPTID_SIGNERINFO,
    OSPEPTID_ENCRYPTEDDIGEST,
    OSPEPTID_AUTHATTRIBUTES,
    OSPEPTID_UNAUTHATTRIBUTES,
    OSPEPTID_ISSUERANDSERNUM,
    OSPEPTID_DIGESTINFO,
    OSPEPTID_ALGORITHMID,
    OSPEPTID_PRIVATEKEYINFO,
    OSPEPTID_PRIVATEKEYINFO_ATTRIBUTES,

    /*
     * These were added for parsing the certificate requests used for
     * enrolling devices; they represent the same entities described
     * in PKCS#10:
     */
    OSPEPTID_CERTREQ,
    OSPEPTID_CERTREQINFO,

    OSPEPTID_LAST_PTID
} OSPEASN1PARSETABLEID;

typedef enum _OSPEASN1DATAREFID {
    /* CERTIFICATE ELEMENTS THAT GET ASKED FOR */
    OSPEDRID_CERTIFICATE,
    OSPEDRID_CERT_TBSCERTIFICATE,
    OSPEDRID_CERT_SIGNATUREALGORITHM,
    OSPEDRID_CERT_SIGNATURE,
    OSPEDRID_CERT_EXPLICIT_VERSION,
    OSPEDRID_CERT_VERSION,
    OSPEDRID_CERT_SERIALNUMBER,
    OSPEDRID_CERT_SIGNATURE_TBS,
    OSPEDRID_CERT_ISSUER,
    OSPEDRID_CERT_NOTBEFORE,
    OSPEDRID_CERT_NOTAFTER,
    OSPEDRID_CERT_SUBJECT,
    OSPEDRID_CERT_SUBJPUBKEYINFO,
    OSPEDRID_CERT_PUBLICKEYALG,
    OSPEDRID_CERT_PUBLICKEY,
    OSPEDRID_CERT_ISSUERUNIQUEID,
    OSPEDRID_CERT_SUBJECTUNIQUEID,
    OSPEDRID_CERT_EXPLICIT_EXTENSIONS,
    OSPEDRID_CERT_EXTENSIONS,
    OSPEDRID_CERT_EXTENSION,

    /* SIGNED DATA ELEMENTS THAT GET ASKED FOR */
    OSPEDRID_SIGNEDDATA,
    OSPEDRID_SGNDAT_VERSION,
    OSPEDRID_SGNDAT_DIGESTALGORITHMS,
    OSPEDRID_SGNDAT_DIGESTALGORITHM,
    OSPEDRID_SGNDAT_CONTENTINFO,
    OSPEDRID_SGNDAT_CERTIFICATES,
    OSPEDRID_SGNDAT_CERTIFICATE,
    OSPEDRID_SGNDAT_CERTIFICATE_SUBJPUBKEYINFO,
    OSPEDRID_SGNDAT_CERTREVLISTS,
    OSPEDRID_SGNDAT_CERTREVLIST,
    OSPEDRID_SGNDAT_SIGNERINFOS,
    OSPEDRID_SGNDAT_SIGNERINFO,

    /* SIGNED INFO DATA ELEMENTS */
    OSPEDRID_SIGNERINFO,
    OSPEDRID_SGNINF_VERSION,
    OSPEDRID_SGNINF_ISSUER,
    OSPEDRID_SGNINF_SERIALNUMBER,
    OSPEDRID_SGNINF_DIGESTALGORITHM,
    OSPEDRID_SGNINF_AUTHATTRIBUTES,
    OSPEDRID_SGNINF_DIGENCRYPTALG,
    OSPEDRID_SGNINF_ENCRYPTEDDIGEST,
    OSPEDRID_SGNINF_UNAUTHATTRIBUTES,

    /* CONTENT INFO DATA ELEMENTS */
    OSPEDRID_CONTENTINFO,
    OSPEDRID_CNTINF_CONTENTTYPE,
    OSPEDRID_CNTINF_EXPLICIT_CONTENT,
    OSPEDRID_CNTINF_CONTENT,

    /* DIGEST INFO DATA ELEMENTS */
    OSPEDRID_DIGESTINFO,
    OSPEDRID_DIGINF_DIGESTALGORITHM,
    OSPEDRID_DIGINF_DIGEST,

    /* ALGORITHM ID DATA ELEMENTS */
    OSPEDRID_ALGORITHMID,
    OSPEDRID_ALGID_OID,
    OSPEDRID_ALGID_ATTRIBUTES,
    OSPEDRID_ALGID_TERMINATOR,

    /* PRIVATEKEYINFO DATA ELEMENTS */
    OSPEDRID_PRIVATEKEYINFO,
    OSPEDRID_PVTKEYINF_VERSION,
    OSPEDRID_PVTKEYINF_ALGORITHM,
    OSPEDRID_PVTKEYINF_PRIVATEKEY,
    OSPEDRID_PVTKEYINF_ATTRIBUTES,
    OSPEDRID_PVTKEYINF_ATTRIBUTE,

    OSPEDRID_SIGNATURE,
    OSPEDRID_SIG_CONTENTTYPE,
    OSPEDRID_SIG_SIGNEDDATA,
    OSPEDRID_SIG_SGNDAT_VERSION,
    OSPEDRID_SIG_SGNDAT_DIGESTALGORITHM,
    OSPEDRID_SIG_SGNDAT_DATA,
    OSPEDRID_SIG_SGNDAT_CERTIFICATE,
    OSPEDRID_SIG_SGNDAT_CERTIFICATE_SUBJPUBKEYINFO,
    OSPEDRID_SIG_SGNDAT_CERTREVLIST,
    OSPEDRID_SIG_SGNDAT_SIGNERINFO,
    OSPEDRID_SIG_SGNDAT_SGNINF_DIGESTALGORITHM,
    OSPEDRID_SIG_SGNDAT_SGNINF_DIGENCRYPTALG,
    OSPEDRID_SIG_SGNDAT_SGNINF_ENCRYPTEDDIGEST,

    /*
     * These are defined for parsing certificate requests; all possible
     * elements of a certificate request ( with the exception of the possible
     * values of the optional attributes list, which is unknown at compile
     * time anyway ) is given here. This listing is based off of PKCS#10.
     */
    OSPEDRID_CERTREQ,
    OSPEDRID_CERTREQINFO,
    OSPEDRID_CERTREQINFO_VERSION,
    OSPEDRID_CERTREQINFO_SUBJECT,
    OSPEDRID_CERTREQINFO_SUBJPUBKEYINFO,
    OSPEDRID_CERTREQINFO_ATTRIBUTES,
    OSPEDRID_CERTREQ_SIGNATUREALGID,
    OSPEDRID_CERTREQ_SIGNATURE,

    OSPEDRID_NOTDEFINED,
    OSPEDRID_LAST_DRID
} OSPEASN1DATAREFID;

typedef struct _OSPTASN1ELEMENTINFO {
    unsigned char Tag;              /* ASN1 Data Type Tag Number */
    unsigned char *Element;         /* Pointer to beginning of encoded data for this element. */
    unsigned int ElementLength;     /* Length of data pointed to by Element above */
    /*
     * Space for element and possibly for children is allocated by this element
     * and must be deleted when this element is deleted (after child elements are
     * deleted of course)
     */
    unsigned int ElementSpaceAllocated;
    unsigned char *Content;        /* Pointer to encoded content data that defines this element */
    unsigned int ContentLength;    /* Length of data pointed to by  Content above */
    /* "child" element - points to this element's component elements - constructed elements only. */
    struct _OSPTASN1ELEMENTINFO *ContentElementInfo;
    /* Pointer to next peer element */
    struct _OSPTASN1ELEMENTINFO *NextElementInfo;
} OSPTASN1ELEMENTINFO;

#define OSPC_ASN1_DATAREF_MAXLENGTH 30

typedef struct _OSPTASN1PARSERESULT {
    unsigned char DataReference[OSPC_ASN1_DATAREF_MAXLENGTH];
    struct _OSPTASN1PARSERESULT *NextResult;
    OSPTASN1ELEMENTINFO *ElementInfo;
} OSPTASN1PARSERESULT;

typedef struct _OSPASN1OBJECT {
    OSPTASN1ELEMENTINFO *ElementInfo;
    OSPTASN1PARSERESULT *ParseResults;
} OSPTASN1OBJECT;

typedef struct _OSPTASN1PARSERULE {
    char *Name;
    /*
     * ASN1 Type Tag that the current element must match.  If the Tag is 0xFF, then this rule is
     * a content rule and directs the parser to a differnt parse table to continue parsing.
     */
    unsigned char Tag;
    /*
     * Enum identifier the parse table that describes this elements contents.  Only used
     * for elements whose Tag is 0xFF indicating that the element is constructed.
    */
    OSPEASN1PARSETABLEID ParseTableId;

    /* Reference for this rule data in the Result's table. */
    unsigned char DataReference;
    /*
     * Minimum number of occurances of this item that are allowed. If the data element is
     * optional, then Minimum occurances will be 0
     */
    unsigned int MinimumCount;
    /*
     * Maximum number of occurances of this item that are allowed.  Usually 1, unless the
     * element contains a list (SET/SET_OF).
     */
    unsigned int MaximumCount;
    /*
     * Indicates that the value can have a default value.  The default value must be assigned by
     * using app.
     */
    OSPTBOOL HasDefault;
    /*
     * Tag number used with explicit and implicit tagging.  Used in body rules, not in the id
     * rule (index 0). Passed to Parser to modify tag within new rule table.
     */
    unsigned char ClassTag;
} OSPTASN1PARSERULE;

typedef OSPTASN1PARSERULE OSPTASN1PARSETABLE;

typedef struct _OSPTOBJECTID {
    unsigned int *Elements;
    unsigned int ElementCount;
} OSPTASN1OBJECTID;

typedef int OSPTASN1BOOLEAN;

typedef struct _OSPTASN1UTCTIME {
    struct tm Time;
    int FractionalSeconds;
} OSPTASN1UTCTIME;

typedef struct _OSPTASN1BUFFER {
    unsigned char *Buffer;
    unsigned int BufferLength;
} OSPTASN1BUFFER;


typedef struct _OSPTBITSTRING {
    int UnusedBits;
    OSPTASN1BUFFER *Buffer;
} OSPTASN1BITSTRING;

/*
 * INTEGER
 *  The integer structure holds DER encoded big integers.
 *  Assumes base 256, two's complement encoding.  digits[0] contains the
 *  MOST Significate digit in the integer.
 */
typedef struct _OSPTASN1INTEGER {
    unsigned int DigitCount;
    unsigned char *Digits;
    int *SmallValue;    /* Pointer to value if less than +|- 0x7fffffff */
} OSPTASN1INTEGER;

typedef enum {
    OSPC_ALGORITHM_NONE,
    OSPC_ALGORITHM_RSA_ENCRYPTION,
    OSPC_ALGORITHM_MD5
} OSPE_ALGORITHM;

/* Function Prototypes */
#ifdef  __cplusplus
extern "C" {
#endif

    int OSPPASN1UTCTimeEncode(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1PrintableStringEncode(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1IntegerEncode(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1EndOfContentEncode(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1BitStringEncode(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1ElementEncode(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1ObjectGetElementByDataRef(OSPTASN1OBJECT *ospvObject, OSPTASN1ELEMENTINFO **ospvElementInfo, OSPEASN1DATAREFID OospvDataRefId);
    int OSPPASN1ElementCopyElementData(OSPTASN1ELEMENTINFO *ospvElementInfo, unsigned char **ospvData, unsigned int *ospvDataLength);
    int OSPPASN1SmallInt2UnsignedChar(unsigned ospvIntegerValue, unsigned ospvBase, unsigned char **ospvBuffer, unsigned *ospvBufferLength);
    int OSPPASN1SmallIntegerEncode(OSPTASN1OBJECT **ospvIntegerObject, unsigned ospvInteger, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1ElementFormat(OSPTASN1ELEMENTINFO **ospvElement, unsigned char *ospvTag, unsigned char ospvTagFlags, unsigned ospvTagLength,
            unsigned char *ospvData, unsigned ospvDataLength);
    int OSPPASN1ObjectFormat(OSPTASN1OBJECT **ospvObject, OSPTASN1ELEMENTINFO *ospvElement, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1ObjectEncode(OSPTASN1OBJECT **ospvEncodedObject, unsigned char *ospvTag, unsigned ospvTagLength, unsigned char ospvTagFlags,
            unsigned char *ospvContent, unsigned ospvContentLength, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1ObjectDeparse(OSPTASN1OBJECT *ospvObject, OSPEASN1PARSETABLEID ospvParseTableId, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1ObjectCopyElementObject(OSPTASN1OBJECT **ospvDestObject, OSPTASN1OBJECT *ospvSrcObject, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1ObjectGetElementInfo(OSPTASN1OBJECT *ospvObject, OSPTASN1ELEMENTINFO **ospvElementInfo);
    int OSPPASN1ObjectGetContentElementInfo(OSPTASN1OBJECT *ospvObject, OSPTASN1ELEMENTINFO **ospvContentElementInfo);
    int OSPPASN1ObjectGetParseResults(OSPTASN1OBJECT *ospvObject, OSPTASN1PARSERESULT **ospvParseResults);
    int OSPPASN1ElementCopy(OSPTASN1ELEMENTINFO **ospvToElement, OSPTASN1ELEMENTINFO *ospvFromElement);
    int OSPPASN1ElementDeparse(OSPTASN1ELEMENTINFO **ospvElementInfo, OSPTASN1PARSERESULT **ospvParseResults, OSPEASN1PARSETABLEID ospvParseTableId,
            unsigned char *ospvDataReference);
    int OSPPASN1IntegerGetSmallValue(OSPTASN1ELEMENTINFO *ospvInteger, int *ospvValue);
    int OSPPASN1ElementGetContentValue(OSPTASN1ELEMENTINFO *ospvElement, void **ospvContentValue);
    int OSPPASN1ElementGetContentData(OSPTASN1ELEMENTINFO *ospvElement, unsigned char **ospvContent, unsigned int *ospvContentLength);
    int OSPPASN1ElementGetElementData(OSPTASN1ELEMENTINFO *ospvElementInfo, unsigned char **ospvData, unsigned int *ospvDataLength);
    int OSPPASN1ObjectCopy(OSPTASN1OBJECT **ospvToObject, OSPTASN1OBJECT *ospvFromObject);
    int OSPPASN1AlgorithmIdEncode(OSPTASN1OBJECT **ospvAlgorithIdObject, OSPEASN1ID ospvAlgorithmIdCode, OSPEASN1DATAREFID ospvDataRefId);
    void PTPResultUpdateDataRef(unsigned char ospvDataRef, OSPTASN1PARSERESULT *ospvParseResult);
    int PTPDataRefIdGetValue(OSPEASN1DATAREFID ospvDataRefId, unsigned char **ospvDataRefIdValue);
    int PTPDataRefAddRef(unsigned char ospvDataReference[OSPC_ASN1_DATAREF_MAXLENGTH], unsigned char ospvNewReference);

    /*
     * If primitive can be a list (max >= 1), then all peers of this element should be the same element and should all be
     * added to the list. Add Parse Result assumes this and adds a result for the element passed, and then traverses the
     * element's "next" pointer to add entries for each of the elements it finds.  It returns when an element's
     * "next" pointer is NULL.
     */
    int PTPAddParseResults(OSPTASN1PARSERULE *ospvParseRule, OSPTASN1ELEMENTINFO *ospvElementInfo, OSPTASN1PARSERESULT **ospvParseResult,
            unsigned char ospvDataRef);
    int PTPDataReferencesMatch(unsigned char *ospvDataReferenceId1, unsigned char *ospvDataReferenceId2);
    int PTPResultIsRuleComponent(unsigned char *ospvRuleDataReference, unsigned char *ospvResultDataReference);
    int PTPResultsCopy(OSPTASN1PARSERESULT **ospvDestParseResults, OSPTASN1PARSERESULT *ospvSrcParseResults);
    int PTPResultsGetElement(OSPEASN1DATAREFID ospvDataReferenceId, OSPTASN1PARSERESULT *ospvParseResults, OSPTASN1ELEMENTINFO **ospvFoundElement);
    int PTPResultsCreate(OSPTASN1PARSERESULT **ospvParseResult, OSPTASN1ELEMENTINFO *ospvElementInfo, OSPEASN1DATAREFID ospvDataRefId);
    OSPTASN1PARSERESULT *PTPResultsEndOfList(OSPTASN1PARSERESULT *ospvResults);
    int PTPParseResultAppend(OSPTASN1PARSERESULT **ospvLastResult, OSPTASN1PARSERESULT *ospvNewResults);
    void PTPResultsDelete(OSPTASN1PARSERESULT **ospvParseResult);
    int PTPRuleIsOptional(OSPTASN1PARSERULE *ospvParseRule);    /* Boolean */
    void *PTPRuleGetDefault(OSPTASN1PARSERULE *ospvParseRule);
    int PTPRuleIsPrimitive(OSPTASN1PARSERULE *ospvParseRule);   /* Boolean */
    int PTPRuleIsDerived(OSPTASN1PARSERULE *ospvParseRule);     /* Boolean */
    int PTPRuleIsDERFormat(OSPTASN1PARSERULE *ospvParseRule);   /* Boolean */
    int OSPPASN1ElementParse(OSPTASN1ELEMENTINFO *ospvElementInfo, OSPEASN1PARSETABLEID ospvParseTableId, OSPTASN1PARSERULE *ospvParentParseRule,
            OSPTASN1PARSERESULT **ospvParseResult, unsigned char ospvDataRef);
    void OSPPASN1ElementParseDelete(OSPTASN1PARSERESULT **ospvParseResult);
    int PTPTableGet(OSPEASN1PARSETABLEID ospvParseTableId, OSPTASN1PARSETABLE *ospvParseTable[]);
    /* Updated to point to next rule */
    int PTPTableGetRule(OSPEASN1PARSETABLEID ospvParseTableId, OSPTASN1PARSERULE **ospvParseRule, unsigned int *ospvRuleIndex);
    int PTPRuleGetParseTableId(OSPTASN1PARSERULE *ospvParseRule, OSPEASN1PARSETABLEID *ospvParseTableId);
    void OSPPASN1IntegerDelete(OSPTASN1INTEGER **ospvInteger);
    int OSPPASN1IntegerCreate(unsigned char *ospvContent, unsigned int ospvContentLength, OSPTASN1INTEGER **ospvInteger);
    void OSPPASN1BitStringDelete(OSPTASN1BITSTRING **ospvBitString);
    int OSPPASN1BitStringCreate(unsigned char *ospvContent, unsigned ospvContentLength, unsigned ospvUnusedBits, OSPTASN1BITSTRING **ospvBitString);
    int OSPPASN1ObjectAddChild(OSPTASN1OBJECT *ospvParent, OSPTASN1OBJECT *ospvChild, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1ObjectNew(OSPTASN1OBJECT **ospvASN1Object, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1ObjectCreate(OSPTASN1OBJECT **ospvASN1Object, OSPTASN1ELEMENTINFO *ospvElementInfo, OSPTASN1PARSERESULT *ospvParseResults);
    void OSPPASN1ObjectDelete(OSPTASN1OBJECT **ospvASN1Object);
    int OSPPASN1ElementTestContext(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1PrimitiveDecode(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1PrimitiveDelete(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1ElementDecode(unsigned char *ospvASN1Element, OSPTASN1ELEMENTINFO **ospvASN1ElementInfo, unsigned int ospvLevel);
    int OSPPASN1ElementCreate(OSPTASN1ELEMENTINFO **ospvElementInfo);
    void OSPPASN1ElementDelete(OSPTASN1ELEMENTINFO **ospvElement, unsigned int ospvLevel);
    int OSPPASN1ElementGet(OSPEASN1DATAREFID ospvDataRefId, OSPTASN1PARSERESULT *ospvParseResults, OSPTASN1ELEMENTINFO **ospvFoundElement);
    void OSPPASN1EndOfContentDelete(OSPTASN1ELEMENTINFO *ospvElementInfo);
    void OSPPASN1NullDelete(OSPTASN1ELEMENTINFO *ospvElementInfo);
    void OSPPASN1ObjectIdentifierDelete(OSPTASN1ELEMENTINFO *ospvElementInfo);
    void OSPPASN1PrintableStringDelete(OSPTASN1ELEMENTINFO *ospvElementInfo);
    void OSPPASN1UTCTimeDelete(OSPTASN1ELEMENTINFO *ospvElementInfo);
    int OSPPASN1IntegerDecode(void *ospvContentValue, unsigned char *ospvContent, unsigned int ospvContentLength);
    int OSPPASN1ObjectIdentifierDecode(void *ospvContentValue, unsigned char *ospvContent, unsigned int ospvContentLength);
    int OSPPASN1PrintableStringDecode(void *ospvContentValue, unsigned char *ospvContent, unsigned int ospvContentLength);
    int OSPPASN1UTCTimeDecode(void *ospvContentValue, unsigned char *ospvContent, unsigned int ospvContentLength);
    int OSPPASN1BitStringDecode(void *ospvContentValue, unsigned char *ospvContent, unsigned int ospvContentLength);
    int OSPPASN1OctetStringDecode(void *ospvContentValue, unsigned char *ospvContent, unsigned int ospvContentLength);
    int OSPPASN1NullEncode(OSPTASN1OBJECT **ospvNullObject);
    int OSPPASN1BigIntegerEncode(OSPTASN1OBJECT **ospvIntegerObject, unsigned char *ospvIntegerData, unsigned int ospvIntegerLength);
    int OSPPASN1ObjectIdentifierEncode(OSPTASN1OBJECT **ospvOIDObject, OSPEASN1ID ospvObjectId, OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1OctetStringEncode(OSPTASN1OBJECT **ospvOctetStringObject, unsigned char *ospvContent, unsigned ospvContentLength,
            OSPEASN1DATAREFID ospvDataRefId);
    int OSPPASN1BufferCreate(unsigned char *ospvContent, unsigned ospvContentLength, OSPTASN1BUFFER **ospvBuffer);
    void OSPPASN1BufferDelete(OSPTASN1BUFFER **ospvBuffer);
    int OSPPASN1PrimitiveEncode(OSPTASN1ELEMENTINFO *ospvElementInfo);

#ifdef __cplusplus
}
#endif

typedef struct _OSPTDERBUF {
    unsigned char *data;    /* Pointer to storage for the certificate   */
    unsigned char *ptr;     /* Pointer to beginning of buffer remainder */
} OSPTDERBUF;

/*****************************************************************
    ASN1 SIMPLE TYPES
*****************************************************************/
#define OSPC_ASN1_OID_MAX_ELEMENTS  20

typedef struct _OSPTOBJID {
    int element[OSPC_ASN1_OID_MAX_ELEMENTS];
} OSPTOBJECTID;

typedef struct {
    struct tm time;
    int fracsec;
} OSPTUTCTIME;

#define OSPC_OID_VALUE_TERMINATOR   (-1)    /* Not a legal OID value */
#define OSPC_OID_VALUE_DIGIT_MASK   0x7F    /* Base 128 */
#define OSPC_OID_VALUE_END_MASK     0x80    /* MSB = 0 in last octet of value */

/*****************************************************************
    OBJECT ID DEFINITIONS/XREF/TAGS
*****************************************************************/
#define OSPC_OID_CODE_NOT_FOUND                             0
#define OSPC_OID_CODE_ALGORITHM                             1, 3, 14, 3, 2
#define OSPC_OID_CODE_RSADSI                                1, 2, 840, 113549
#define     OSPC_OID_CODE_PKCS                              OID_CODE_RSADSI, 1
#define         OSPC_OID_CODE_PKCS9                         OID_CODE_PKCS, 9
#define         OSPC_OID_CODE_ALG_RSAENCRYPTION             OSPC_OID_CODE_PKCS, 1, 1
#define            OSPC_OID_CODE_ALG_MD2WITHRSAENCRYPTION   OSPC_OID_CODE_PKCS, 1, 2
#define            OSPC_OID_CODE_ALG_MD5WITHRSAENCRYPTION   OSPC_OID_CODE_PKCS, 1, 4
#define     OSPC_OID_CODE_MD2                               OID_CODE_RSADSI, 2, 2
#define     OSPC_OID_CODE_MD5                               OID_CODE_RSADSI, 2, 5
#define     OSPC_OID_CODE_RC4                               OID_CODE_RSADSI, 3, 4
#define OSPC_OID_CODE_X500                                  2, 5
#define     OSPC_OID_CODE_X509                              OID_CODE_X500, 4
#define         OSPC_OID_CODE_DN_COUNTRY                    OID_CODE_X509, 6
#define         OSPC_OID_CODE_DN_STATE                      OID_CODE_X509, 8
#define         OSPC_OID_CODE_DN_LOCALITY                   OID_CODE_X509, 7
#define         OSPC_OID_CODE_DN_ORGANIZATION               OSPC_OID_CODE_X509, 10
#define         OSPC_OID_CODE_DN_ORG_UNIT                   OID_CODE_X509, 11
#define         OSPC_OID_CODE_DN_COMMON_NAME                OID_CODE_X509,  3
#define     OSPC_OID_CODE_RSA                               OID_CODE_X500, 8, 1, 1
#define     OSPC_OID_CODE_LD_CE                             OID_CODE_X500, 29
#define            OSPC_OID_CODE_EXT_KEY_USAGE              OID_CODE_LD_CE, 15
#define            OSPC_OID_CODE_EXT_SUBJECT_KEY_ID         OSPC_OID_CODE_LD_CE, 14

/*
 * ENTRIES IN THE ARRAY THAT FOLLOWS MUST BE IN THE SAME ORDER AS
 * THE ENUM
*/
typedef enum {
    OSPC_OID_TAG_DN_COUNTRY,
    OSPC_OID_TAG_DN_STATE,
    OSPC_OID_TAG_DN_LOCALITY,
    OSPC_OID_TAG_DN_ORGANIZATION,
    OSPC_OID_TAG_DN_ORG_UNIT,
    OSPC_OID_TAG_DN_COMMON_NAME,
    OSPC_OID_TAG_ALG_RSAENCRYPTION,
    OSPC_OID_TAG_ALG_MD5WITHRSAENCRYPTION,
    OSPC_OID_TAG_EXT_KEY_USAGE,
    OSPC_OID_TAG_EXT_SUBJECT_KEY_ID,
    OSPC_OID_TAG_NOT_FOUND
} OSPE_OID_TAG;

typedef struct {
    OSPE_OID_TAG tag;
    OSPTOBJECTID oid;
    char *sname;
    char *lname;
} OSPT_OID_XREF;

#define OSPC_OID_XX     OSPC_OID_VALUE_TERMINATOR

typedef struct {
    int type;
    unsigned char *value;
    unsigned length;
} OSPTPARAMETER;

#define OSPC_ATV_MAX_PARAMS     10

typedef struct {
    OSPTOBJECTID oid;
    OSPTPARAMETER *params[OSPC_ATV_MAX_PARAMS];     /* Dynamically allocated */
} OSPTATTRTYPEVAL;


/* typedef OSPTALGORITHMID OSPTDIGESTALGORITHMID; */

/* DER/BER (Encoding/Decoding Rules) Definitions */
#define OSPC_BER_MAX_TAG_LENGTH         10
#define OSPC_BER_MODE_MASK              0x20
#define OSPC_BER_MODE_PRIMITIVE         0x00
#define OSPC_BER_MODE_CONSTRUCTED       0x20
#define OSPM_CONSTRUCTED_TAG(a)         ((a) | OSPC_BER_MODE_CONSTRUCTED)
#define OSPM_IS_PRIMITIVE(a)            (((a)&OSPC_BER_MODE_MASK) == OSPC_BER_MODE_PRIMITIVE)
#define OSPM_IS_CONSTRUCTED(a)          (((a)&OSPC_BER_MODE_MASK) == OSPC_BER_MODE_CONSTRUCTED)
#define OSPC_BER_CLASS_MASK             0xc0
#define OSPC_BER_CLASS_UNIVERSAL        0x00
#define OSPC_BER_CLASS_APPLICATION      0x40
#define OSPC_BER_CLASS_CONTEXT_SENS     0x80
#define OSPC_BER_CLASS_PRIVATE          0xc0
#define OSPC_BER_EXPLICIT_MASK          (OSPC_BER_MODE_CONSTRUCTED | OSPC_BER_CLASS_CONTEXT_SENS)
#define OSPC_BER_IMPLICIT_MASK          (OSPC_BER_MODE_CONSTRUCTED | OSPC_BER_CLASS_CONTEXT_SENS)
#define OSPC_BER_LENGTH_MODE_MASK       0x80
#define OSPC_BER_LENGTH_MODE_SHORT      0x00
#define OSPC_BER_LENGTH_MODE_LONG       0x80
#define OSPC_BER_LENGTH_MASK            0x7f
#define OSPM_IS_SHORT_LENGTH(a)         (((a)&OSPC_BER_LENGTH_MODE_MASK)==OSPC_BER_LENGTH_MODE_SHORT)
#define OSPM_IS_LONG_LENGTH(a)          (((a)&OSPC_BER_LENGTH_MODE_MASK)==OSPC_BER_LENGTH_MODE_LONG)
#define OSPM_BER_LENGTH(a)              (((a)&OSPC_BER_LENGTH_MASK))
#define OSPC_BER_TAG_MASK               0x1f
#define OSPC_BER_TAG_HIGH               0x1f

#define OSPM_BASE_TAG(a)                ((a) & (OSPC_BER_TAG_MASK))
#define OSPM_IS_HIGH_TAG(a)             (((a) & (OSPC_BER_TAG_HIGH)) == OSPC_BER_TAG_HIGH)

#define OSPC_TAG_TYPE_EOC               0x00
#define OSPC_TAG_TYPE_BOOLEAN           0x01
#define OSPC_TAG_TYPE_INTEGER           0x02
#define OSPC_TAG_TYPE_BIT_STRING        0x03
#define OSPC_TAG_TYPE_OCTET_STRING      0x04
#define OSPC_TAG_TYPE_NULL              0x05
#define OSPC_TAG_TYPE_OBJECT_IDENTIFIER 0x06
#define OSPC_TAG_TYPE_SEQUENCE          0x10
#define OSPC_TAG_TYPE_SET               0x11
#define OSPC_TAG_TYPE_PRINTABLESTRING   0x13
#define OSPC_TAG_TYPE_T61STRING         0x14
#define OSPC_TAG_TYPE_IA5STRING         0x16
#define OSPC_TAG_TYPE_UTCTIME           0x17
#define OSPC_TAG_TYPE_GENERALIZEDTIME   0x18
#define OSPC_TAG_TYPE_BMPSTRING         0x1e

#define OSPC_TAG_TYPE_IMPLICIT          0xa0
#define OSPM_TAG_TYPE_IMPLICIT_TAG(a)   ((OSPC_TAG_TYPE_IMPLICIT) | (a))
#define OSPM_IS_IMPLICIT(a)             (((a) & OSPC_BER_IMPLICIT_MASK) == OSPC_BER_IMPLICIT_MASK)
#define OSPC_TAG_TYPE_EXPLICIT          0xa0
#define OSPM_TAG_TYPE_EXPLICIT_TAG(a)   ((OSPC_TAG_TYPE_EXPLICIT) | (a))
#define OSPM_IS_EXPLICIT(a)             (((a) & OSPC_BER_EXPLICIT_MASK) == OSPC_BER_EXPLICIT_MASK)

/* SPECIAL NOT-REALLY-A-TAG TAGS */
#define OSPC_TAG_TYPE_DER_FORMAT        0xFE
#define OSPC_TAG_TYPE_DERIVED           0xFF

#define OSPM_EXPLICIT_CLASSNUMBER(a)    ((a) & OSPC_BER_TAG_MASK)
#define OSPC_BER_MAX_EXTENSIONS         10

typedef union char2int_union {
    OSPTUINT64 num;
    unsigned char buf[sizeof(OSPTUINT64)];
}
OSPTCHAR2INT;

typedef struct {
    unsigned char tag[OSPC_BER_MAX_TAG_LENGTH];     /* Complete tag */
    unsigned int type;                              /* Element's type */
    unsigned int taglength;                         /* Length of tag and length fields */
    unsigned int length;                            /* Element's length */
    unsigned char *value;                           /* Pointer to element's data */
} OSPTBERELEMENT;

typedef struct {
    int length;
    unsigned char *buffer;
} OSPTDERELEMENT;

#define OSPC_BER_MAX_EXTENSIONS     10

typedef struct {
    int id;
    char *text;
} OSPTTEXTLISTELEMENT;

#endif /* _OSPASN1_H */
