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
 * ospasn1parse.c - Member functions for ASN1 parsing library
 */

#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/osptnlog.h"

/*
 * Member functions
 */

#if 0

THE PARSE RESULTS TABLE WILL HAVE:
    The parse results table will provide a cross reference for values found in
    the element info list (derived from breaking down the asn1 encoded data)
    Only the fields found will be included.  It will be the respsibility of the
    program that called the parser to handle missing fields correctly.  The
    parseing program must also handle multiple occurrances of a field.  This
    table will only be returned if the parse is error free.

    datafieldid     Number referenceing the data structure field data is stored in.

    This will come from the parse table for an entry.

    Scan table looking for this reference number when trying to
    populate data structure fields.  Scanning program must know
    if field is required, whether it can have multiple
    entries. etc.

    datafieldindex  If a data field id can have multiple occurances, then this
    value indicates which occurance it is. 0 indicates it is
    the only one.  

    treeAddress     Tree address to the data. 

    Format is 1.2.3: Last index should be a primitive.  Other 
    indexes must be constructed.  

    Count index following next pointers. Follow child pointer at
    period.  

    null indicates not found.

    use this value to walk the tree to find the eInfo element.

    used for diagnostics only.


    eInfoPtr        Pointer to eInfo structure that contains the element
    related to the data structure field.  Extractor can access
    data by value, element, or element content.

    null indicates not found. 

    defaultValuePtr Null indicates eInfoPtr should be followed for data.  If
    not null, then a void pointer is set to point to the
    default data contained in the parse table.  It is
    appliation dependent what is actually stored there.

THE PARSING TABLE WILL HAVE:
    A complete description of the structure being parsed.  Default fields, etc
    must be described. All elements will be described.  The description will
    Fields must be in the order they occur in the table.  

    componentname   Used for diagnostics.

    Textual description of the expected element.

    parsetableid    Enum indicating a parse table to use for recursive parse.
    if PRIMITIVE, (datafieldid != NONE), then no parsetable should 
    be followed - generate a parse result record and write it to 
                                            result table.

    If constructed, recursively parse current element using 
    parsetableid to reference the appropriate parse table for the
    element.

    If SEQUENCE, then get subsequent elements in parse table.

    If SET, then get subsequent elements in table, repeat according to the min/max fields. 

    If SET_OF, then get subsequent elements in table, repeat 
    according to min/max fields

    If ANY_DEFINED_BY   ?

    If ANY, then ?

    If CHOICE, then ?

    datafieldid     Field reference (context specific) to  used by calling
    routine to reference the fields in the data structure.
    When a parse result record is written, this datafieldid is
    written to it so the calling program will have something to
    search for to find data for a particular data structure
    field.  
    ex: When the calling program is ready to populate the
    version number field for a certificate.  It would look
    for a record in the parse results table whose
    datafieldid is OSPE_ASN1_X509_CERT_FIELD_ID_VERSION.
    If it finds the record, it can go to the data through
    the eInfo pointer field, or if the eInfo pointer field
    is empty, it can get default data from the defaultvalue
    field in the parse results table.

    minimumnumber   Minimum number of this type of element

    maximumnumber   Maximum number of this type of element

    defaultvaluePtr If null, then no default data is provided.  If not null,
    then it is a void pointer to something.  pointer to a
    number, pointer to a string (ASN1 encoding of the defualt value.
    Initialize the parse table as necessary.  Assume the
    reading application will figure out what to do with the
    pointer it finds. If not null, then a parse error occurs
    when an element is not found if a minimum is required.

    BOOLEAN TYPE IS A PRIMITIVE 0x01 0x01 0xFF(TRUE) 0x00(FALSE)

#endif

static OSPTASN1PARSETABLE ospgParseTable_NotDefined[] = {
    {"NotDefined", OSPC_TAG_TYPE_NULL, OSPEPTID_NOTDEFINED, 0x0, 0, 0, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Certificate[] = {
    {"Certificate", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"tbsCertificate", OSPC_TAG_TYPE_DERIVED, OSPEPTID_TBSCERTIFICATE, 0x1, 1, 1, 0, 0},
    {"signatureAlgorithm", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x2, 1, 1, 0, 0},
    {"signature", OSPC_TAG_TYPE_BIT_STRING, OSPEPTID_NOTDEFINED, 0x3, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_TBSCertificate[] = {
    {"TBSCertificate", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"version", OSPC_TAG_TYPE_DERIVED, OSPEPTID_EXPLICIT_VERSION, 0x1, 1, 1, 1, 0},
    {"serialNumber", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CERTSERIALNUM, 0x2, 1, 1, 0, 0},
    {"signature", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x3, 1, 1, 0, 0},
    {"issuer", OSPC_TAG_TYPE_DERIVED, OSPEPTID_RDNSEQUENCE, 0x4, 1, 1, 0, 0},
    {"validity", OSPC_TAG_TYPE_DERIVED, OSPEPTID_VALIDITY, 0x5, 1, 1, 0, 0},
    {"subject", OSPC_TAG_TYPE_DERIVED, OSPEPTID_RDNSEQUENCE, 0x6, 1, 1, 0, 0},
    {"subjectPublicKeyInfo", OSPC_TAG_TYPE_DERIVED, OSPEPTID_SUBJPUBKEYINFO, 0x7, 1, 1, 0, 0},
    {"issuerUniqueID", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ISSUERUNIQUEID, 0x8, 0, 1, 0, 0},
    {"subjectUniqueID", OSPC_TAG_TYPE_DERIVED, OSPEPTID_SUBJECTUNIQUEID, 0x9, 0, 1, 0, 0},
    {"extensions", OSPC_TAG_TYPE_DERIVED, OSPEPTID_EXPLICIT_EXTENSIONS, 0xa, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_SignatureAlgorithm[] = {
    {"SignatureAlgorithm", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_DERAlgorithmId[] = {
    {"AlgorithmId", OSPC_TAG_TYPE_DER_FORMAT, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
    /* May need to expand this definition to get all the pieces. */
};

static OSPTASN1PARSETABLE ospgParseTable_Explicit_Version[] = {
    {"ExplicitVersion", OSPM_TAG_TYPE_EXPLICIT_TAG(0), OSPEPTID_NOTDEFINED, 0x0, 1, 1, 1, 0},
    {"version", OSPC_TAG_TYPE_DERIVED, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 1, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Version[] = {
    {"Version", OSPC_TAG_TYPE_INTEGER, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 1, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_CertSerialNum[] = {
    {"CertSerialNum", OSPC_TAG_TYPE_INTEGER, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Name[] = {
    {"Name", OSPC_TAG_TYPE_DERIVED, OSPEPTID_RDNSEQUENCE, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}

    /* This definition is strange because Name is a CHOICE of RDNSequence.  
       Choice does not generate an element. Special handling at beginning 
       of element parse to handle parse tables with first entry of Type 
       Tag "CONSTRUCTED" to have it immediately parse new element. */
};

static OSPTASN1PARSETABLE ospgParseTable_RDNSequence[] = {
    {"RDNSequence", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 0xff, 0, 0},
    {"relDisName", OSPC_TAG_TYPE_DERIVED, OSPEPTID_RELDISNAME, 0x1, 0, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_RelDisName[] = {
    {"RelDisName", OSPC_TAG_TYPE_SET, OSPEPTID_NOTDEFINED, 0x0, 1, 0xff, 0, 0},
    {"attrTypeAndVal", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ATTRTYPEANDVAL, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_AttrTypeAndValue[] = {
    {"AttrTypeAndVal", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"type", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ATTRIBUTETYPE, 0x1, 1, 1, 0, 0},
    {"value", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ATTRIBUTEVALUE, 0x2, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_AttributeType[] = {
    {"AttributeType", OSPC_TAG_TYPE_OBJECT_IDENTIFIER, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_AttributeValue[] = {
    {"AttributeValue", OSPC_TAG_TYPE_PRINTABLESTRING, OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"AttributeValue", OSPC_TAG_TYPE_T61STRING, OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"AttributeValue", OSPC_TAG_TYPE_IA5STRING, OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Validity[] = {
    {"Validity", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"notBefore", OSPC_TAG_TYPE_DERIVED, OSPEPTID_UTCTIME, 0x1, 1, 1, 0, 0},
    {"notAfter", OSPC_TAG_TYPE_DERIVED, OSPEPTID_UTCTIME, 0x2, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Time[] = {
    {"Time", OSPC_TAG_TYPE_DERIVED, OSPEPTID_UTCTIME, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}

    /* This definition is strange because Time is a CHOICE of RDNSequence.  Choice does not generate an
       element. Special handling at beginning of element parse to handle parse tables with first entry of Type
       Tag "CONSTRUCTED" to have it immediately parse new element. */
};

static OSPTASN1PARSETABLE ospgParseTable_UTCTime[] = {
    {"UTCTime", OSPC_TAG_TYPE_UTCTIME, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
    /* This one is a little strange because UTCTIME is a named type as well as a primitive type.  
       Made it a named type as well so that the Time choice would have someplace to go. */
};

static OSPTASN1PARSETABLE ospgParseTable_SubjPubKeyInfo[] = {
    {"SubjPubKeyInfo", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"algorithm", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x1, 1, 1, 0, 0},
    {"subjPubKey", OSPC_TAG_TYPE_BIT_STRING, OSPEPTID_NOTDEFINED, 0x2, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_IssuerUniqueId[] = {
    {"IssuerUniqueId", OSPM_TAG_TYPE_IMPLICIT_TAG(1), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 1},
    {"issuerUniqueId", OSPC_TAG_TYPE_DERIVED, OSPEPTID_UNIQUEID, 0x1, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_SubjectUniqueId[] = {
    {"SubjectUniqueId", OSPM_TAG_TYPE_IMPLICIT_TAG(2), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 2},
    {"subjectUniqueId", OSPC_TAG_TYPE_DERIVED, OSPEPTID_UNIQUEID, 0x1, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_UniqueId[] = {
    {"UniqueId", OSPC_TAG_TYPE_BIT_STRING, OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Explicit_Extensions[] = {
    {"ExplicitExtensions", OSPM_TAG_TYPE_EXPLICIT_TAG(3), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 3},
    {"explicitExtensions", OSPC_TAG_TYPE_DERIVED, OSPEPTID_EXTENSION, 0x1, 1, 0xff, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Extensions[] = {
    {"Extensions", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"extension", OSPC_TAG_TYPE_DERIVED, OSPEPTID_EXTENSION, 0x1, 1, 0xff, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Extension[] = {
    {"Extension", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"extnID", OSPC_TAG_TYPE_OBJECT_IDENTIFIER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"critical", OSPC_TAG_TYPE_BOOLEAN, OSPEPTID_NOTDEFINED, 0x2, 1, 1, 0, 0},
    {"extnValue", OSPC_TAG_TYPE_OCTET_STRING, OSPEPTID_NOTDEFINED, 0x3, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_ContentInfo_Data[] = {
    {"ContentInfo", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"contentType", OSPC_TAG_TYPE_OBJECT_IDENTIFIER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"data", OSPC_TAG_TYPE_DERIVED, OSPEPTID_EXPLICIT_DATA, 0x2, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_ContentInfo_SignedData[] = {
    {"ContentInfo", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"contentType", OSPC_TAG_TYPE_OBJECT_IDENTIFIER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"signedData", OSPC_TAG_TYPE_DERIVED, OSPEPTID_EXPLICIT_SIGNEDDATA, 0x2, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Explicit_Data[] = {
    {"ExplicitData", OSPM_TAG_TYPE_EXPLICIT_TAG(0), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"explicitData", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DATA, 0x1, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Explicit_SignedData[] = {
    {"ExplicitSignedData", OSPM_TAG_TYPE_EXPLICIT_TAG(0), OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"explicitSignedData", OSPC_TAG_TYPE_DERIVED, OSPEPTID_SIGNEDDATA, 0x1, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Data[] = {
    {"Data", OSPC_TAG_TYPE_OCTET_STRING, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_SignedData[] = {
    {"SignedData", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"version", OSPC_TAG_TYPE_INTEGER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"digestAlgorithms", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DIGESTALGORITHMS, 0x2, 1, 1, 0, 0},
    {"contentInfo", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CONTENTINFO_DATA, 0x3, 1, 1, 0, 0},
    {"certificates", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CERTIFICATES, 0x4, 1, 1, 0, 0},
    {"crls", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CERTREVLISTS, 0x5, 0, 1, 0, 0},
    {"signerInfos", OSPC_TAG_TYPE_DERIVED, OSPEPTID_SIGNERINFOS, 0x6, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_DigestAlgorithms[] = {
    {"DigestAlgorithms", OSPC_TAG_TYPE_SET, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"digestAlgorithmId", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_Certificates[] = {
    {"Certificates", OSPM_TAG_TYPE_IMPLICIT_TAG(0), OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"certificate", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CERTIFICATE, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_CertRevLists[] = {
    {"CertRevLists", OSPM_TAG_TYPE_IMPLICIT_TAG(1), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 1},
    {"certRevList", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CERTREVLIST, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_CertRevList[] = {
    {"CertRevList", OSPC_TAG_TYPE_DER_FORMAT, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_SignerInfos[] = {
    {"SignerInfos", OSPC_TAG_TYPE_SET, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"signerInfo", OSPC_TAG_TYPE_DERIVED, OSPEPTID_SIGNERINFO, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_SignerInfo[] = {
    {"SignerInfo", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"version", OSPC_TAG_TYPE_INTEGER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"issuerAndSerNum", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ISSUERANDSERNUM, 0x2, 1, 1, 0, 0},
    {"digestAlgorithm", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x3, 1, 1, 0, 0},
    {"authAttributes", OSPC_TAG_TYPE_DERIVED, OSPEPTID_AUTHATTRIBUTES, 0x4, 0, 1, 1, 0},
    {"digEncryptAlgorithm", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x5, 1, 1, 0, 0},
    {"encryptedDigest", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ENCRYPTEDDIGEST, 0x6, 1, 1, 0, 0},
    {"unauthAttributes", OSPC_TAG_TYPE_DERIVED, OSPEPTID_UNAUTHATTRIBUTES, 0x7, 0, 1, 1, 1},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_IssuerAndSerNum[] = {
    {"IssuerAndSerNum", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"issuer", OSPC_TAG_TYPE_DERIVED, OSPEPTID_NAME, 0x1, 1, 1, 0, 0},
    {"serialNumber", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CERTSERIALNUM, 0x2, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_AuthAttributes[] = {
    {"AuthenticatedAttributes", OSPM_TAG_TYPE_IMPLICIT_TAG(0), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"authenticatedAttribute", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ATTRTYPEANDVAL, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_UnauthAttributes[] = {
    {"UnauthenticatedAttributes", OSPM_TAG_TYPE_IMPLICIT_TAG(1), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"unauthenticatedAttribute", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ATTRTYPEANDVAL, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_EncryptedDigest[] = {
    {"EncryptedDigest", OSPC_TAG_TYPE_OCTET_STRING, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_DigestInfo[] = {
    {"DigestInfo", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"digestAlgorithm", OSPC_TAG_TYPE_DER_FORMAT, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"digest", OSPC_TAG_TYPE_OCTET_STRING, OSPEPTID_NOTDEFINED, 0x2, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_AlgorithmId[] = {
    {"AlgorithmId", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"algorithmOID", OSPC_TAG_TYPE_OBJECT_IDENTIFIER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"attrTypeAndVal", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ATTRTYPEANDVAL, 0x2, 0, 0xff, 0, 0},
    {"terminator", OSPC_TAG_TYPE_NULL, OSPEPTID_NOTDEFINED, 0x3, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_PrivateKeyInfo[] = {
    {"PrivateKeyInfo", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"version", OSPC_TAG_TYPE_INTEGER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"algorithm", OSPC_TAG_TYPE_DERIVED, OSPEPTID_DERALGORITHMID, 0x2, 1, 1, 0, 0},
    {"privateKey", OSPC_TAG_TYPE_OCTET_STRING, OSPEPTID_NOTDEFINED, 0x3, 1, 1, 0, 0},
    {"attributes", OSPC_TAG_TYPE_DERIVED, OSPEPTID_PRIVATEKEYINFO_ATTRIBUTES, 0x4, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_PrivateKeyInfo_Attributes[] = {
    {"UnauthenticatedAttributes", OSPM_TAG_TYPE_IMPLICIT_TAG(0), OSPEPTID_NOTDEFINED, 0x0, 0, 1, 0, 0},
    {"unauthenticatedAttribute", OSPC_TAG_TYPE_DERIVED, OSPEPTID_ATTRTYPEANDVAL, 0x1, 1, 0xff, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_CertReq[] = {
    {"CertReq", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"certReqInfo", OSPC_TAG_TYPE_DERIVED, OSPEPTID_CERTREQINFO, 0x1, 1, 1, 0, 0},
    {"signatureAlgorithm", OSPC_TAG_TYPE_DERIVED, OSPEPTID_SIGNATUREALGORITHM, 0x2, 1, 1, 0, 0},
    {"signature", OSPC_TAG_TYPE_BIT_STRING, OSPEPTID_NOTDEFINED, 0x3, 1, 1, 0, 0},
    {OSPC_OSNULL}
};

static OSPTASN1PARSETABLE ospgParseTable_CertReqInfo[] = {
    {"CertReqInfo", OSPC_TAG_TYPE_SEQUENCE, OSPEPTID_NOTDEFINED, 0x0, 1, 1, 0, 0},
    {"version", OSPC_TAG_TYPE_INTEGER, OSPEPTID_NOTDEFINED, 0x1, 1, 1, 0, 0},
    {"Subject", OSPC_TAG_TYPE_DERIVED, OSPEPTID_RDNSEQUENCE, 0x2, 1, 1, 0, 0},
    {"subjectPubKeyInfo", OSPC_TAG_TYPE_DERIVED, OSPEPTID_SUBJPUBKEYINFO, 0x3, 1, 1, 0, 0},
    {"attributes", OSPM_TAG_TYPE_EXPLICIT_TAG(0), OSPEPTID_NOTDEFINED, 0x4, 0, 1, 0, 0},
    {OSPC_OSNULL}
};

/* NOTE:    THE OSPEASN1PARSETABLEID enum list must stay in sync with ospgParseTableIndex[].  The enum
values are the index into the ParseTableIndex for the parse table array.
*/
static OSPTASN1PARSETABLE *ospgParseTableIndex[] = {
    /* TO NOT CHANGE THIS TABLE WITHOUT UPDATING THE ENUM VALUES IN OSPEASN1PARSETABLEID */
    /* CERTIFICATE ELEMENTS */
    ospgParseTable_NotDefined,
    ospgParseTable_Certificate,
    ospgParseTable_TBSCertificate,
    ospgParseTable_SignatureAlgorithm,
    ospgParseTable_DERAlgorithmId,
    ospgParseTable_Explicit_Version,
    ospgParseTable_Version,
    ospgParseTable_CertSerialNum,
    ospgParseTable_Name,
    ospgParseTable_RDNSequence,
    ospgParseTable_RelDisName,
    ospgParseTable_AttrTypeAndValue,
    ospgParseTable_AttributeType,
    ospgParseTable_AttributeValue,
    ospgParseTable_Validity,
    ospgParseTable_Time,
    ospgParseTable_UTCTime,
    ospgParseTable_SubjPubKeyInfo,
    ospgParseTable_IssuerUniqueId,
    ospgParseTable_SubjectUniqueId,
    ospgParseTable_UniqueId,
    ospgParseTable_Explicit_Extensions,
    ospgParseTable_Extensions,
    ospgParseTable_Extension,
    ospgParseTable_ContentInfo_Data,
    ospgParseTable_Explicit_Data,
    ospgParseTable_Data,
    ospgParseTable_ContentInfo_SignedData,
    ospgParseTable_Explicit_SignedData,
    ospgParseTable_SignedData,
    ospgParseTable_DigestAlgorithms,
    ospgParseTable_Certificates,
    ospgParseTable_CertRevLists,
    ospgParseTable_CertRevList,
    ospgParseTable_SignerInfos,
    ospgParseTable_SignerInfo,
    ospgParseTable_EncryptedDigest,
    ospgParseTable_AuthAttributes,
    ospgParseTable_UnauthAttributes,
    ospgParseTable_IssuerAndSerNum,
    ospgParseTable_DigestInfo,
    ospgParseTable_AlgorithmId,
    ospgParseTable_PrivateKeyInfo,
    ospgParseTable_PrivateKeyInfo_Attributes,
    ospgParseTable_CertReq,
    ospgParseTable_CertReqInfo,
    OSPC_OSNULL
};

/* NOTE:    THE OSPEASN1DATAREFID enum list must stay in sync with ospgDataReferenceIdIndex[].  The enum
values are the index into the DataReferenceIdIndex. */

static unsigned char ospgDataRefId_NotDefined[] = { 0xff };

static unsigned char ospgDataRefId_Certificate[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE, 0xff
};

static unsigned char ospgDataRefId_Cert_TBSCertificate[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE, 0xff
};

static unsigned char ospgDataRefId_Cert_SignatureAlgorithm[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SIGNATUREALGORITHM, 0xff
};

static unsigned char ospgDataRefId_Cert_Signature[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SIGNATURE, 0xff
};

static unsigned char ospgDataRefId_Cert_Explicit_Version[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_EXPLICIT_VERSION, 0xff
};

static unsigned char ospgDataRefId_Cert_Version[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_VERSION, 0xff
};

static unsigned char ospgDataRefId_Cert_SerialNumber[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SERIALNUMBER, 0xff
};

static unsigned char ospgDataRefId_Cert_Signature_TBS[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SIGNATURE_TBS, 0xff
};

static unsigned char ospgDataRefId_Cert_Issuer[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_ISSUER, 0xff
};

static unsigned char ospgDataRefId_Cert_NotBefore[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_VALIDITY,
    OSPC_ASN1_DATAREFID_CERT_NOTBEFORE, 0xff
};

static unsigned char ospgDataRefId_Cert_NotAfter[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_VALIDITY,
    OSPC_ASN1_DATAREFID_CERT_NOTAFTER, 0xff
};

static unsigned char ospgDataRefId_Cert_Subject[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SUBJECT, 0xff
};

static unsigned char ospgDataRefId_Cert_SubjPubKeyInfo[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SUBJPUBKEYINFO, 0xff
};

static unsigned char ospgDataRefId_Cert_PublicKeyAlg[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SUBJPUBKEYINFO,
    OSPC_ASN1_DATAREFID_CERT_PUBLICKEYALG, 0xff
};

static unsigned char ospgDataRefId_Cert_PublicKey[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SUBJPUBKEYINFO,
    OSPC_ASN1_DATAREFID_CERT_PUBLICKEY, 0xff
};

static unsigned char ospgDataRefId_Cert_IssuerUniqueId[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_ISSUERUNIQUEID, 0xff
};

static unsigned char ospgDataRefId_Cert_SubjectUniqueId[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SUBJECTUNIQUEID, 0xff
};

static unsigned char ospgDataRefId_Cert_Explicit_Extensions[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_EXPLICIT_EXTENSIONS, 0xff
};

static unsigned char ospgDataRefId_Cert_Extensions[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_EXPLICIT_EXTENSIONS,
    OSPC_ASN1_DATAREFID_CERT_EXTENSIONS, 0xff
};

static unsigned char ospgDataRefId_Cert_Extension[] = {
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_EXPLICIT_EXTENSIONS,
    OSPC_ASN1_DATAREFID_CERT_EXTENSIONS,
    OSPC_ASN1_DATAREFID_CERT_EXTENSION, 0xff
};

static unsigned char ospgDataRefId_SignedData[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA, 0xff
};

static unsigned char ospgDataRefId_SgnDat_Version[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_VERSION, 0xff
};

static unsigned char ospgDataRefId_SgnDat_DigestAlgorithms[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_DIGESTALGORITHMS, 0xff
};

static unsigned char ospgDataRefId_SgnDat_DigestAlgorithm[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_DIGESTALGORITHMS,
    OSPC_ASN1_DATAREFID_SGNDAT_DIGESTALGORITHM, 0xff
};

static unsigned char ospgDataRefId_SgnDat_Content[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_CONTENTINFO, 0xff
};

static unsigned char ospgDataRefId_SgnDat_Certificates[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATES, 0xff
};

static unsigned char ospgDataRefId_SgnDat_Certificate[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATES,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATE, 0xff
};

static unsigned char ospgDataRefId_SgnDat_Certificate_SubjPubKeyInfo[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATES,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SUBJPUBKEYINFO, 0xff
};

static unsigned char ospgDataRefId_SgnDat_CertRevLists[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTREVLISTS, 0xff
};

static unsigned char ospgDataRefId_SgnDat_CertRevList[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTREVLISTS,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTREVLIST, 0xff
};

static unsigned char ospgDataRefId_SgnDat_SignerInfos[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFOS, 0xff
};

static unsigned char ospgDataRefId_SgnDat_SignerInfo[] = {
    OSPC_ASN1_DATAREFID_SIGNEDDATA,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFOS,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFO, 0xff
};

static unsigned char ospgDataRefId_SignerInfo[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO, 0xff
};

static unsigned char ospgDataRefId_SgnInf_Version[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_VERSION, 0xff
};

static unsigned char ospgDataRefId_SgnInf_Issuer[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_ISSUERANDSERNUM,
    OSPC_ASN1_DATAREFID_SGNINF_ISSUER, 0xff
};

static unsigned char ospgDataRefId_SgnInf_SerialNumber[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_ISSUERANDSERNUM,
    OSPC_ASN1_DATAREFID_SGNINF_SERIALNUMBER, 0xff
};

static unsigned char ospgDataRefId_SgnInf_DigestAlgorithm[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_DIGESTALGORITHM, 0xff
};

static unsigned char ospgDataRefId_SgnInf_AuthAttributes[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_AUTHATTRIBUTES, 0xff
};

static unsigned char ospgDataRefId_SgnInf_DigEncryptAlg[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_DIGENCRYPTALG, 0xff
};

static unsigned char ospgDataRefId_SgnInf_EncryptedDigest[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_ENCRYPTEDDIGEST, 0xff
};

static unsigned char ospgDataRefId_SgnInf_UnauthAttributes[] = {
    OSPC_ASN1_DATAREFID_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_UNAUTHATTRIBUTES, 0xff
};

static unsigned char ospgDataRefId_ContentInfo[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO, 0xff
};

static unsigned char ospgDataRefId_CntInf_ContentType[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENTTYPE, 0xff
};

static unsigned char ospgDataRefId_CntInf_Explicit_Content[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT, 0xff
};

static unsigned char ospgDataRefId_CntInf_Content[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT, 0xff
};

static unsigned char ospgDataRefId_DigestInfo[] = {
    OSPC_ASN1_DATAREFID_DIGESTINFO, 0xff
};

static unsigned char ospgDataRefId_DigInf_DigestAlgorithm[] = {
    OSPC_ASN1_DATAREFID_DIGESTINFO,
    OSPC_ASN1_DATAREFID_DIGINF_DIGESTALGORITHM, 0xff
};

static unsigned char ospgDataRefId_DigInf_Digest[] = {
    OSPC_ASN1_DATAREFID_DIGESTINFO,
    OSPC_ASN1_DATAREFID_DIGINF_DIGEST, 0xff
};

static unsigned char ospgDataRefId_AlgorithmId[] = {
    OSPC_ASN1_DATAREFID_ALGORITHMID, 0xff
};

static unsigned char ospgDataRefId_AlgId_OID[] = {
    OSPC_ASN1_DATAREFID_ALGORITHMID,
    OSPC_ASN1_DATAREFID_ALGID_OID, 0xff
};

static unsigned char ospgDataRefId_AlgId_AttrTypeAndVal[] = {
    OSPC_ASN1_DATAREFID_ALGORITHMID,
    OSPC_ASN1_DATAREFID_ALGID_ATTRTYPEANDVAL, 0xff
};

static unsigned char ospgDataRefId_AlgId_Terminator[] = {
    OSPC_ASN1_DATAREFID_ALGORITHMID,
    OSPC_ASN1_DATAREFID_ALGID_TERMINATOR, 0xff
};


static unsigned char ospgDataRefId_PrivateKeyInfo[] = {
    OSPC_ASN1_DATAREFID_PRIVATEKEYINFO, 0xff
};

static unsigned char ospgDataRefId_PvtKeyInf_Version[] = {
    OSPC_ASN1_DATAREFID_PRIVATEKEYINFO,
    OSPC_ASN1_DATAREFID_PVTKEYINF_VERSION, 0xff
};

static unsigned char ospgDataRefId_PvtKeyInf_Algorithm[] = {
    OSPC_ASN1_DATAREFID_PRIVATEKEYINFO,
    OSPC_ASN1_DATAREFID_PVTKEYINF_ALGORITHM, 0xff
};

static unsigned char ospgDataRefId_PvtKeyInf_PrivateKey[] = {
    OSPC_ASN1_DATAREFID_PRIVATEKEYINFO,
    OSPC_ASN1_DATAREFID_PVTKEYINF_PRIVATEKEY, 0xff
};

static unsigned char ospgDataRefId_PvtKeyInf_Attributes[] = {
    OSPC_ASN1_DATAREFID_PRIVATEKEYINFO,
    OSPC_ASN1_DATAREFID_PVTKEYINF_ATTRIBUTES, 0xff
};

static unsigned char ospgDataRefId_PvtKeyInf_Attribute[] = {
    OSPC_ASN1_DATAREFID_PRIVATEKEYINFO,
    OSPC_ASN1_DATAREFID_PVTKEYINF_ATTRIBUTES,
    OSPC_ASN1_DATAREFID_PVTKEYINF_ATTRIBUTE, 0xff
};


static unsigned char ospgDataRefId_Signature[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO, 0xff
};

static unsigned char ospgDataRefId_Sig_ContentType[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENTTYPE, 0xff
};

static unsigned char ospgDataRefId_Sig_SignedData[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_Version[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_VERSION, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_DigestAlgorithm[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_DIGESTALGORITHMS,
    OSPC_ASN1_DATAREFID_SGNDAT_DIGESTALGORITHM, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_Data[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_Certificate[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATES,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATE, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_Certificate_SubjPubKeyInfo[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATES,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_TBSCERTIFICATE,
    OSPC_ASN1_DATAREFID_CERT_SUBJPUBKEYINFO, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_CertRevList[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTREVLISTS,
    OSPC_ASN1_DATAREFID_SGNDAT_CERTREVLIST, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_SignerInfo[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFOS,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFO, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_SgnInf_DigestAlgorithm[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFOS,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_DIGESTALGORITHM, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_SgnInf_DigEncryptAlg[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFOS,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_DIGENCRYPTALG, 0xff
};

static unsigned char ospgDataRefId_Sig_SgnDat_SgnInf_EncryptedDigest[] = {
    OSPC_ASN1_DATAREFID_CONTENTINFO,
    OSPC_ASN1_DATAREFID_CNTINF_EXPLICIT_CONTENT,
    OSPC_ASN1_DATAREFID_CNTINF_CONTENT,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFOS,
    OSPC_ASN1_DATAREFID_SGNDAT_SIGNERINFO,
    OSPC_ASN1_DATAREFID_SGNINF_ENCRYPTEDDIGEST, 0xff
};

/* These are defined for parsing certificate requests; all possible
 * elements of a certificate request ( with the exception of the possible
 * values of the optional attributes list, which is unknown at compile
 * time anyway ) is given here. This listing is based off of PKCS#10.
 */
static unsigned char ospgDataRefId_CertReq[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    0xff
};

/* Define the location of the certificate request info of a PKCS#10
 * certificate request; this structure will contain the version, subject,
 * public key, and optional attributes that are then signed afterwards
 * by the requester:
 */
static unsigned char ospgDataRefId_CertReqInfo[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    OSPC_ASN1_DATAREFID_CERTREQINFO,
    0xff
};

/* Define the location of the version for a
 * PKCS#10 certificate request info: 
 */
static unsigned char ospgDataRefId_CertReqInfo_Version[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    OSPC_ASN1_DATAREFID_CERTREQINFO,
    OSPC_ASN1_DATAREFID_CERTREQINFO_VERSION,
    0xff
};

/* Define the location of the subject for a 
 * PKCS#10 certificate request info: 
 */
static unsigned char ospgDataRefId_CertReqInfo_Subject[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    OSPC_ASN1_DATAREFID_CERTREQINFO,
    OSPC_ASN1_DATAREFID_CERTREQINFO_SUBJECT,
    0xff
};

/* Define the location of the subjectPublicKeyInfo for a
 * PKCS#10 certificate request info: 
 */
static unsigned char ospgDataRefId_CertReqInfo_SubjPubKeyInfo[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    OSPC_ASN1_DATAREFID_CERTREQINFO,
    OSPC_ASN1_DATAREFID_CERTREQINFO_SUBJPUBKEYINFO,
    0xff
};

/* Define the location of the optional attributes for a 
 * PKCS#10 certificate request info: 
 */
static unsigned char ospgDataRefId_CertReqInfo_Attributes[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    OSPC_ASN1_DATAREFID_CERTREQINFO,
    OSPC_ASN1_DATAREFID_CERTREQINFO_ATTRIBUTES,
    0xff
};

/* Define the location of the signature algorithm id for 
 * a PKCS#10 certificate request: 
 */
static unsigned char ospgDataRefId_CertReq_Signature_Algorithm[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    OSPC_ASN1_DATAREFID_CERTREQ_SIGNATURE_ALGORITHM,
    0xff
};

/* Define the location of the signature for PKCS#10 a certificate request: */
static unsigned char ospgDataRefId_CertReq_Signature[] = {
    OSPC_ASN1_DATAREFID_CERTREQ,
    OSPC_ASN1_DATAREFID_CERTREQ_SIGNATURE,
    0xff
};

static unsigned char *ospgDataRefIdIndex[] = {
    /* TO NOT CHANGE THIS TABLE WITHOUT UPDATING THE ENUM VALUES IN OSPEASN1DATAREFID */
    ospgDataRefId_Certificate,
    ospgDataRefId_Cert_TBSCertificate,
    ospgDataRefId_Cert_SignatureAlgorithm,
    ospgDataRefId_Cert_Signature,
    ospgDataRefId_Cert_Explicit_Version,
    ospgDataRefId_Cert_Version,
    ospgDataRefId_Cert_SerialNumber,
    ospgDataRefId_Cert_Signature_TBS,
    ospgDataRefId_Cert_Issuer,
    ospgDataRefId_Cert_NotBefore,
    ospgDataRefId_Cert_NotAfter,
    ospgDataRefId_Cert_Subject,
    ospgDataRefId_Cert_SubjPubKeyInfo,
    ospgDataRefId_Cert_PublicKeyAlg,
    ospgDataRefId_Cert_PublicKey,
    ospgDataRefId_Cert_IssuerUniqueId,
    ospgDataRefId_Cert_SubjectUniqueId,
    ospgDataRefId_Cert_Explicit_Extensions,
    ospgDataRefId_Cert_Extensions,
    ospgDataRefId_Cert_Extension,

    ospgDataRefId_SignedData,
    ospgDataRefId_SgnDat_Version,
    ospgDataRefId_SgnDat_DigestAlgorithms,
    ospgDataRefId_SgnDat_DigestAlgorithm,
    ospgDataRefId_SgnDat_Content,
    ospgDataRefId_SgnDat_Certificates,
    ospgDataRefId_SgnDat_Certificate,
    ospgDataRefId_SgnDat_Certificate_SubjPubKeyInfo,
    ospgDataRefId_SgnDat_CertRevLists,
    ospgDataRefId_SgnDat_CertRevList,
    ospgDataRefId_SgnDat_SignerInfos,
    ospgDataRefId_SgnDat_SignerInfo,

    ospgDataRefId_SignerInfo,
    ospgDataRefId_SgnInf_Version,
    ospgDataRefId_SgnInf_Issuer,
    ospgDataRefId_SgnInf_SerialNumber,
    ospgDataRefId_SgnInf_DigestAlgorithm,
    ospgDataRefId_SgnInf_AuthAttributes,
    ospgDataRefId_SgnInf_DigEncryptAlg,
    ospgDataRefId_SgnInf_EncryptedDigest,
    ospgDataRefId_SgnInf_UnauthAttributes,

    ospgDataRefId_ContentInfo,
    ospgDataRefId_CntInf_ContentType,
    ospgDataRefId_CntInf_Explicit_Content,
    ospgDataRefId_CntInf_Content,

    ospgDataRefId_DigestInfo,
    ospgDataRefId_DigInf_DigestAlgorithm,
    ospgDataRefId_DigInf_Digest,

    ospgDataRefId_AlgorithmId,
    ospgDataRefId_AlgId_OID,
    ospgDataRefId_AlgId_AttrTypeAndVal,
    ospgDataRefId_AlgId_Terminator,

    ospgDataRefId_PrivateKeyInfo,
    ospgDataRefId_PvtKeyInf_Version,
    ospgDataRefId_PvtKeyInf_Algorithm,
    ospgDataRefId_PvtKeyInf_PrivateKey,
    ospgDataRefId_PvtKeyInf_Attributes,
    ospgDataRefId_PvtKeyInf_Attribute,

    ospgDataRefId_Signature,
    ospgDataRefId_Sig_ContentType,
    ospgDataRefId_Sig_SignedData,
    ospgDataRefId_Sig_SgnDat_Version,
    ospgDataRefId_Sig_SgnDat_DigestAlgorithm,
    ospgDataRefId_Sig_SgnDat_Data,
    ospgDataRefId_Sig_SgnDat_Certificate,
    ospgDataRefId_Sig_SgnDat_Certificate_SubjPubKeyInfo,
    ospgDataRefId_Sig_SgnDat_CertRevList,
    ospgDataRefId_Sig_SgnDat_SignerInfo,
    ospgDataRefId_Sig_SgnDat_SgnInf_DigestAlgorithm,
    ospgDataRefId_Sig_SgnDat_SgnInf_DigEncryptAlg,
    ospgDataRefId_Sig_SgnDat_SgnInf_EncryptedDigest,

    /* These are defined for parsing certificate requests; all possible
     * elements of a certificate request ( with the exception of the possible
     * values of the optional attributes list, which is unknown at compile
     * time anyway ) is given here. This listing is based off of PKCS#10.
     */
    ospgDataRefId_CertReq,
    ospgDataRefId_CertReqInfo,
    ospgDataRefId_CertReqInfo_Version,
    ospgDataRefId_CertReqInfo_Subject,
    ospgDataRefId_CertReqInfo_SubjPubKeyInfo,
    ospgDataRefId_CertReqInfo_Attributes,
    ospgDataRefId_CertReq_Signature_Algorithm,
    ospgDataRefId_CertReq_Signature,

    ospgDataRefId_NotDefined,
    OSPC_OSNULL
};

/* IMPLEMENTATION */

int PTPAddParseResults(
    OSPTASN1PARSERULE *ospvParseRule,
    OSPTASN1ELEMENTINFO *ospvElementInfo, 
    OSPTASN1PARSERESULT **ospvParseResult, 
    unsigned char ospvDataRef)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1PARSERESULT *parseResult = OSPC_OSNULL;
    OSPTASN1ELEMENTINFO *eInfo = OSPC_OSNULL;

    eInfo = ospvElementInfo;

    OSPM_ARGUSED(ospvParseRule);

    /* Malloc space for a result */
    OSPM_MALLOC(parseResult, OSPTASN1PARSERESULT, sizeof(OSPTASN1PARSERESULT));
    if (parseResult == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for parse result");
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MEMSET(parseResult, 0, sizeof(OSPTASN1PARSERESULT));

        /* Update parse result */
        OSPM_MEMSET(parseResult->DataReference, 0xff, OSPC_ASN1_DATAREF_MAXLENGTH);
        if (ospvDataRef != 0) {
            parseResult->DataReference[0] = ospvDataRef;
        }

        /* Attach the element info object */
        parseResult->ElementInfo = ospvElementInfo;
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        PTPResultsDelete(ospvParseResult);
    } else {
        *ospvParseResult = parseResult;
    }

    return errorcode;
}

int PTPTableGet(
    OSPEASN1PARSETABLEID ospvParseTableId, 
    OSPTASN1PARSETABLE *ospvParseTable[])
{
    int errorcode = OSPC_ERR_NO_ERROR;

    *ospvParseTable = ospgParseTableIndex[ospvParseTableId];

    return errorcode;
}

int PTPTableGetRule(
    OSPEASN1PARSETABLEID ospvParseTableId, 
    OSPTASN1PARSERULE **ospvParseRule, 
    unsigned int *ospvRuleIndex)    /* Updated to point to next rule */
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1PARSETABLE *parseTable = OSPC_OSNULL;

    errorcode = PTPTableGet(ospvParseTableId, &parseTable);
    if (errorcode == OSPC_ERR_NO_ERROR) {
        *ospvParseRule = &(parseTable[*ospvRuleIndex]);
        (*ospvRuleIndex)++;

        if (*ospvParseRule != OSPC_OSNULL) {
            if ((*ospvParseRule)->Name == OSPC_OSNULL) {
                *ospvParseRule = OSPC_OSNULL;
                errorcode = OSPC_ERR_ASN1_PARSE_COMPLETE;
            }
        }
    }

    return errorcode;
}

int PTPRuleIsOptional(
    OSPTASN1PARSERULE *ospvParseRule)
{
    return (ospvParseRule->MinimumCount == 0);
}

int PTPRuleIsPrimitive( /* Boolean */
    OSPTASN1PARSERULE *ospvParseRule)
{
    int is_primitive = 0;

    switch (ospvParseRule->Tag) {
    case OSPC_TAG_TYPE_SEQUENCE:
    case OSPC_TAG_TYPE_SET:
        /* These types are always NOT primitive */
        is_primitive = 0;
        break;
    case OSPC_TAG_TYPE_DER_FORMAT:
        /* This one is always primitive - used to stop parsing for 
           a construct */
        is_primitive = 1;
        break;
    default:
        /* Probably primitive unless tag type is EXPLICIT */
        is_primitive = !(OSPM_IS_EXPLICIT(ospvParseRule->Tag));
        break;
    }

    return is_primitive;
}

int PTPRuleIsDerived(
    OSPTASN1PARSERULE *ospvParseRule)
{
    return (ospvParseRule->Tag == OSPC_TAG_TYPE_DERIVED);
}

int PTPRuleIsDERFormat(
    OSPTASN1PARSERULE *ospvParseRule)
{
    return (ospvParseRule->Tag == OSPC_TAG_TYPE_DER_FORMAT);
}

int PTPRuleGetParseTableId(
    OSPTASN1PARSERULE *ospvParseRule, 
    OSPEASN1PARSETABLEID *ospvParseTableId)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    *ospvParseTableId = ospvParseRule->ParseTableId;

    return errorcode;
}

void PTPResultsDelete(
    OSPTASN1PARSERESULT **ospvParseResult)
{
    OSPTASN1PARSERESULT *thisResult = OSPC_OSNULL;
    OSPTASN1PARSERESULT *nextResult = OSPC_OSNULL;

    if (ospvParseResult != OSPC_OSNULL) {
        for (thisResult = *ospvParseResult; thisResult != OSPC_OSNULL; thisResult = nextResult) {
            /* Point at the next element */
            nextResult = thisResult->NextResult;

            /* Remove the current list node */
            OSPM_FREE(thisResult);
        }

        *ospvParseResult = OSPC_OSNULL;
    }
}

int PTPDataReferencesMatch(
    unsigned char *ospvDataReferenceId1, 
    unsigned char *ospvDataReferenceId2)
{
    unsigned char *ptr1 = OSPC_OSNULL;
    unsigned char *ptr2 = OSPC_OSNULL;

    /* Data reference ids are unsigned char arrays terminated with 0xff */

    for (ptr1 = ospvDataReferenceId1, ptr2 = ospvDataReferenceId2;
         ptr1 && ptr2 && (*ptr1 == *ptr2) && (*ptr1 != 0xff) && (*ptr2 != 0xff); ptr1++, ptr2++);

    return ((*ptr1 == 0xff) && (*ptr2 == 0xff));
}

int PTPResultIsRuleComponent(
    unsigned char *ospvRuleDataReference, 
    unsigned char *ospvResultDataReference)
{
    unsigned char *ptr1 = OSPC_OSNULL;
    unsigned char *ptr2 = OSPC_OSNULL;

    /* Data reference ids are unsigned char arrays terminated with 0xff, if
       hit the end of the rule DataReference, then the result data reference
       must match up to the end of the rule data reference.  */

    for (ptr1 = ospvRuleDataReference, ptr2 = ospvResultDataReference;
         ptr1 && ptr2 && (*ptr1 == *ptr2) && (*ptr1 != 0xff) && (*ptr2 != 0xff); ptr1++, ptr2++);

    return (*ptr1 == 0xff);
}

int PTPDataRefIdGetValue(
    OSPEASN1DATAREFID ospvDataRefId, 
    unsigned char **ospvDataRefValue)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    if (ospvDataRefId >= OSPEDRID_LAST_DRID) {
        errorcode = OSPC_ERR_ASN1_INVALID_DATAREFID;
        OSPM_DBGERRORLOG(errorcode, "Invalid data reference id value");
    } else {
        *ospvDataRefValue = ospgDataRefIdIndex[ospvDataRefId];
    }

    return errorcode;
}

int PTPResultsGetElement(
    OSPEASN1DATAREFID ospvDataReferenceId, 
    OSPTASN1PARSERESULT *ospvParseResult, 
    OSPTASN1ELEMENTINFO **ospvFoundElement)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1PARSERESULT *thisResult = OSPC_OSNULL;
    unsigned char *dataReference = OSPC_OSNULL;

    errorcode = PTPDataRefIdGetValue(ospvDataReferenceId, &dataReference);

    if (errorcode == OSPC_ERR_NO_ERROR) {
        for (thisResult = ospvParseResult, *ospvFoundElement = OSPC_OSNULL;
             (thisResult != OSPC_OSNULL) && (*ospvFoundElement == OSPC_OSNULL); thisResult = thisResult->NextResult) {
            if (PTPDataReferencesMatch(dataReference, thisResult->DataReference)) {
                *ospvFoundElement = thisResult->ElementInfo;
                break;
            }
        }
    }

    return errorcode;
}

void PTPResultUpdateDataRef(
    unsigned char ospvParentDataRef, 
    OSPTASN1PARSERESULT *ospvParseResult)
{
    int i = 0;
    unsigned char *ucptr = OSPC_OSNULL;
    OSPTASN1PARSERESULT *nextResult = OSPC_OSNULL;

    /* Updates the data references in all of the parse results from the
       head to the tail by prefixing them with the value retrieved for the
       supplied DataRefId. */

    if (ospvParseResult == OSPC_OSNULL) {
        OSPM_DBGERRORLOG(-1, "WARNING: ospvParseResult=NULL in UpdateDataRef");
    } else {
        for (nextResult = ospvParseResult; nextResult != OSPC_OSNULL; nextResult = nextResult->NextResult) {
            ucptr = nextResult->DataReference;

            /* Make space for the new reference */
            for (i = (OSPC_ASN1_DATAREF_MAXLENGTH - 1); ucptr && (i > 0); i--) {
                ucptr[i] = ucptr[i - 1];
            }

            *ucptr = ospvParentDataRef;

            /* Test Data Reference to make sure it is terminated,
               trash it if not */
            if (ucptr[OSPC_ASN1_DATAREF_MAXLENGTH - 1] != 0xff) {
                /* The Data Reference overflowed - should always
                   terminate with 0xff, set to 0xff when adding result */
                ucptr[0] = 0xff;
            }
        }
    }
}

int PTPResultsCreate(
    OSPTASN1PARSERESULT **ospvParseResult, 
    OSPTASN1ELEMENTINFO *ospvElement, 
    OSPEASN1DATAREFID ospvDataRefId)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;
    unsigned char *dataReference = OSPC_OSNULL;

    OSPM_MALLOC(parseResults, OSPTASN1PARSERESULT, sizeof(OSPTASN1PARSERESULT));

    if (parseResults == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for new parse result");
    } else {
        OSPM_MEMSET(parseResults, 0, sizeof(OSPTASN1PARSERESULT));
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPM_FREE(parseResults);
    } else {
        errorcode = PTPDataRefIdGetValue(ospvDataRefId, &dataReference);
    }

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPM_MEMCPY(parseResults->DataReference, dataReference, OSPC_ASN1_DATAREF_MAXLENGTH);
        parseResults->ElementInfo = ospvElement;

        *ospvParseResult = parseResults;
    }

    return errorcode;
}

int PTPResultsCopy(
    OSPTASN1PARSERESULT **ospvDstResults, 
    OSPTASN1PARSERESULT *ospvSrcResults)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    OSPTASN1PARSERESULT *parseResults = OSPC_OSNULL;

    OSPM_MALLOC(parseResults, OSPTASN1PARSERESULT, sizeof(OSPTASN1PARSERESULT));

    if (parseResults == OSPC_OSNULL) {
        errorcode = OSPC_ERR_ASN1_UNABLE_TO_ALLOCATE_SPACE;
        OSPM_DBGERRORLOG(errorcode, "Unable to allocate space for new parse result");
    } else {
        OSPM_MEMSET(parseResults, 0, sizeof(parseResults));
    }

    if (errorcode != OSPC_ERR_NO_ERROR) {
        OSPM_FREE(parseResults);
    } else {
        OSPM_MEMCPY(parseResults->DataReference, ospvSrcResults->DataReference, OSPC_ASN1_DATAREF_MAXLENGTH);

        errorcode = OSPPASN1ElementCopy(&(parseResults->ElementInfo), ospvSrcResults->ElementInfo);
        parseResults->NextResult = OSPC_OSNULL;

        *ospvDstResults = parseResults;
    }

    return errorcode;
}

int PTPDataRefAddRef(
    unsigned char ospvDataReference[], 
    unsigned char ospvNewReference)
{
    int errorcode = OSPC_ERR_NO_ERROR;
    unsigned i = 0;

    if (ospvNewReference > 0) {    /* Don't add zeros - primitive as top rule */
        /* Data reference must be an array of MAXLENGTH SIZE */
        for (i = 0; (i < (OSPC_ASN1_DATAREF_MAXLENGTH - 1)) && (ospvDataReference[i] != 0xff); i++);

        if (i == (OSPC_ASN1_DATAREF_MAXLENGTH - 1)) {
            errorcode = OSPC_ERR_ASN1_BUFFER_OVERFLOW;
            OSPM_DBGERRORLOG(errorcode, "Increase the size of OSPC_ASN1_DATAREF_MAXLENGTH");
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            ospvDataReference[i++] = ospvNewReference;
            ospvDataReference[i] = 0xff;    /* Has to have a terminator */
        }
    }

    return errorcode;
}

OSPTASN1PARSERESULT *PTPResultsEndOfList(
    OSPTASN1PARSERESULT *ospvParseResult)
{
    OSPTASN1PARSERESULT *lastResult = OSPC_OSNULL;

    for (lastResult = ospvParseResult; (lastResult != OSPC_OSNULL) && (lastResult->NextResult != OSPC_OSNULL); lastResult = lastResult->NextResult);

    return lastResult;
}
