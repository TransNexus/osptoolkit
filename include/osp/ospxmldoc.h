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
 * ospxmldoc.h - Structures and prototypes for generic XML documents.
 *               The corresponding functions are in ospxmlutil.c,
 *               ospxmlenc.c, and ospxmlparse.c
 */

#ifndef _OSPXMLDOC_H
#define _OSPXMLDOC_H

#include "osp/osp.h"
#include "osp/ospbfr.h"
#include "osp/ospxmlelem.h"

/* various XML character constants */
#define OSPC_XMLDOC_LF              '\x0A'
#define OSPC_XMLDOC_OPEN            '<'
#define OSPC_XMLDOC_CLOSE           '>'
#define OSPC_XMLDOC_TRAILER         '/'
#define OSPC_XMLDOC_SPACE           ' '
#define OSPC_XMLDOC_QUOTE           '"'
#define OSPC_XMLDOC_SINGLEQUOTE     '\''
#define OSPC_XMLDOC_DOUBLEQUOTE     '"'
#define OSPC_XMLDOC_EQUALS          '='
#define OSPC_XMLDOC_QUEST           '?'
#define OSPC_XMLDOC_REF             '&'
#define OSPC_XMLDOC_CHARREF         '#'
#define OSPC_XMLDOC_HEXREF          'x'
#define OSPC_XMLDOC_REFEND          ';'
#define OSPC_XMLDOC_MARKUPDECLOPEN  '['
#define OSPC_XMLDOC_MARKUPDECLCLOSE ']'

/* UTF-16 byte order marks */
#define OSPC_XMLDOC_UTF16MSB        0xFE
#define OSPC_XMLDOC_UTF16LSB        0xFF
#define OSPC_XMLDOC_UTF16NULL       0x00

/* various XML string constants */
#define OSPC_XMLDOC_DECL               "<?xml version=\"1.0\"?>"
#define OSPC_XMLDOC_DECLLEN            (sizeof(OSPC_XMLDOC_DECL)-1)

#define OSPC_XMLDOC_DECLBEG            "<?xml"
#define OSPC_XMLDOC_DECLEND            "?>"
#define OSPC_XMLDOC_DECLBEGLEN         (sizeof(OSPC_XMLDOC_DECLBEG)-1)
#define OSPC_XMLDOC_DECLENDLEN         (sizeof(OSPC_XMLDOC_DECLEND)-1)

#define OSPC_XMLDOC_COMMENTBEG         "<--"
#define OSPC_XMLDOC_COMMENTEND         "-->"
#define OSPC_XMLDOC_COMMENTBEGLEN      (sizeof(OSPC_XMLDOC_COMMENTBEG)-1)
#define OSPC_XMLDOC_COMMENTENDLEN      (sizeof(OSPC_XMLDOC_COMMENTEND)-1)

#define OSPC_XMLDOC_PIBEG             "<?"
#define OSPC_XMLDOC_PIEND             "?>"
#define OSPC_XMLDOC_PIBEGLEN          (sizeof(OSPC_XMLDOC_PIBEG)-1)
#define OSPC_XMLDOC_PIENDLEN          (sizeof(OSPC_XMLDOC_PIEND)-1)

#define OSPC_XMLDOC_DTDBEG             "<!DOCTYPE"
#define OSPC_XMLDOC_DTDEND             ">"
#define OSPC_XMLDOC_DTDBEGLEN          (sizeof(OSPC_XMLDOC_DTDBEG)-1)
#define OSPC_XMLDOC_DTDENDLEN          (sizeof(OSPC_XMLDOC_DTDEND)-1)

#define OSPC_XMLDOC_ELEMENTDECLBEG     "<!ELEMENT"
#define OSPC_XMLDOC_ELEMENTDECLEND     ">"
#define OSPC_XMLDOC_ELEMENTDECLBEGLEN  (sizeof(OSPC_XMLDOC_ELEMENTDECLBEG)-1)
#define OSPC_XMLDOC_ELEMENTDECLENDLEN  (sizeof(OSPC_XMLDOC_ELEMENTDECLEND)-1)

#define OSPC_XMLDOC_ATTLISTBEG         "<!ATTLIST"
#define OSPC_XMLDOC_ATTLISTEND         ">"
#define OSPC_XMLDOC_ATTLISTBEGLEN      (sizeof(OSPC_XMLDOC_ATTLISTBEG)-1)
#define OSPC_XMLDOC_ATTLISTENDLEN      (sizeof(OSPC_XMLDOC_ATTLISTEND)-1)

#define OSPC_XMLDOC_ENTITYDECLBEG      "<!ENTITY"
#define OSPC_XMLDOC_ENTITYDECLEND      ">"
#define OSPC_XMLDOC_ENTITYDECLBEGLEN   (sizeof(OSPC_XMLDOC_ENTITYDECLBEG)-1)
#define OSPC_XMLDOC_ENTITYDECLENDLEN   (sizeof(OSPC_XMLDOC_ENTITYDECLEND)-1)

#define OSPC_XMLDOC_CDATABEG           "<![CDATA["
#define OSPC_XMLDOC_CDATAEND           "]]>"
#define OSPC_XMLDOC_CDATABEGLEN        (sizeof(OSPC_XMLDOC_CDATABEG)-1)
#define OSPC_XMLDOC_CDATAENDLEN        (sizeof(OSPC_XMLDOC_CDATAEND)-1)

/*
 * Best guess sizes for element/attribute names and values - These
 * don't have to be strict since we use elastic buffers. They're just
 * supposed to be good first guesses. Too small and we end up resizing
 * buffers a lot; too large and we waste a little space during XML
 * parsing.
 */
#define OSPC_XMLDOC_NAMESIZE   100
#define OSPC_XMLDOC_VALUESIZE  1000

/* maximum size of entity name  */
#define OSPC_XMLDOC_ENTITYSIZE 10

/* character encodings supported */
typedef enum {
    OSPC_XENC_UNKNOW,       /* unrecognized encoding */
    OSPC_XENC_UTF8,         /* UTF-8 (8-bit Unicode) */
    OSPC_XENC_UTF16L,       /* UTF-16 little endian  */
    OSPC_XENC_UTF16B        /* UTF-16 big endian */
} OSPE_XML_ENC;

/* structure used to store entity references */
typedef struct {
    unsigned char Value;
    unsigned char *Name;
} OSPT_XML_ENTITY;

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    unsigned OSPPXMLDocIsMatch(OSPTBFR **, OSPE_XML_ENC, const unsigned char *, unsigned, unsigned char *, unsigned *);
    unsigned OSPPXMLDocSkipPast(OSPTBFR **, OSPE_XML_ENC, const unsigned char *, unsigned char *);
    unsigned OSPPXMLDocSkipPastChar(OSPTBFR **, OSPE_XML_ENC, unsigned char);
    unsigned OSPPXMLDocGetEncoding(OSPTBFR **, OSPE_XML_ENC *);
    unsigned OSPPXMLDocTranslateEntity(unsigned char *, unsigned char *);
    unsigned OSPPXMLDocReadChar(OSPTBFR **, OSPE_XML_ENC, unsigned char *);
    unsigned OSPPXMLDocSkipWhite(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLAddReference(const unsigned char *, unsigned, OSPTBFR **);
    unsigned OSPPXMLDereference(const unsigned char *, unsigned *, unsigned char *);

    /* functions in ospxmlenc.c */
    int OSPPXMLDocCreate(OSPT_XML_ELEM *, OSPTBFR **);
    int OSPPXMLDocAddElem(OSPT_XML_ELEM *, OSPTBFR **);
    int OSPPXMLDocAddElemName(OSPT_XML_ELEM *, OSPTBFR **);
    int OSPPXMLDocAddAttr(OSPT_XML_ATTR *, OSPTBFR **);
    int OSPPXMLDocAddAttrName(OSPT_XML_ATTR *, OSPTBFR **);
    int OSPPXMLDocAddChar(OSPTBFR **, char);

    /* functions in ospxmlparse.c */
    unsigned OSPPXMLDocParse(OSPTBFR **, OSPT_XML_ELEM **);
    unsigned OSPPXMLDocParseElem(OSPTBFR **, OSPE_XML_ENC, OSPT_XML_ELEM **);
    unsigned OSPPXMLDocGetAttrs(OSPTBFR **, OSPE_XML_ENC, OSPTLIST *);
    unsigned OSPPXMLDocGetAttr(OSPTBFR **, OSPE_XML_ENC, OSPT_XML_ATTR **);
    unsigned OSPPXMLDocGetContent(OSPTBFR **, OSPE_XML_ENC, OSPTLIST *, OSPTBFR **);
    unsigned OSPPXMLDocGetCdata(OSPTBFR **, OSPE_XML_ENC, OSPTBFR **);
    unsigned OSPPXMLDocGetName(OSPTBFR **, OSPE_XML_ENC, OSPTBFR **);
    unsigned OSPPXMLDocSkipProlog(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipAllMisc(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipDTD(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipMarkupDecl(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipDecl(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipComment(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipPI(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipElementDecl(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipEntityDecl(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipAttlist(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipPastCdataBeg(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocSkipPastCdataEnd(OSPTBFR **, OSPE_XML_ENC);
    unsigned OSPPXMLDocIsDecl(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsComment(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsPI(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsDTD(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsElementDecl(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsAttlist(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsEntityDecl(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsCdata(OSPTBFR **, OSPE_XML_ENC, unsigned *);
    unsigned OSPPXMLDocIsCdataEnd(OSPTBFR **, OSPE_XML_ENC, unsigned *);

    void OSPPXMLDocPeekCharN(OSPTBFR **, OSPE_XML_ENC, unsigned, unsigned char *, int *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPXMLDOC_H */
