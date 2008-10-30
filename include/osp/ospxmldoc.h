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
    ospeXMLEncUnknown,      /* unrecognized encoding */
    ospeXMLEncUTF8,         /* UTF-8 (8-bit Unicode) */
    ospeXMLEncUTF16l,       /* UTF-16 little endian  */
    ospeXMLEncUTF16b        /* UTF-16 big endian */
} OSPTXMLENC;


/* structure used to store entity references */
typedef struct {
    unsigned char ospmEntValue;
    unsigned char *ospmEntName;
} OSPTXMLDOCENTITY;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    unsigned OSPPXMLDocIsMatch(OSPTBFR **, OSPTXMLENC, const unsigned char *, unsigned, unsigned char *, unsigned *);
    unsigned OSPPXMLDocSkipPast(OSPTBFR **, OSPTXMLENC, const unsigned char *, unsigned char *);
    unsigned OSPPXMLDocSkipPastChar(OSPTBFR **, OSPTXMLENC, unsigned char);
    unsigned OSPPXMLDocGetEncoding(OSPTBFR **, OSPTXMLENC *);
    unsigned OSPPXMLDocTranslateEntity(unsigned char *, unsigned char *);
    unsigned OSPPXMLDocReadChar(OSPTBFR **, OSPTXMLENC, unsigned char *);
    unsigned OSPPXMLDocSkipWhite(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLAddReference(const unsigned char *, unsigned, OSPTBFR **);
    unsigned OSPPXMLDereference(const unsigned char *, unsigned *, unsigned char *);

    /* functions in ospxmlenc.c */

    int OSPPXMLDocCreate(OSPTXMLELEM *, OSPTBFR **);
    int OSPPXMLDocAddElem(OSPTXMLELEM *, OSPTBFR **);
    int OSPPXMLDocAddElemName(OSPTXMLELEM *, OSPTBFR **);
    int OSPPXMLDocAddAttr(OSPTXMLATTR *, OSPTBFR **);
    int OSPPXMLDocAddAttrName(OSPTXMLATTR *, OSPTBFR **);
    int OSPPXMLDocAddChar(OSPTBFR **, char);

    /* functions in ospxmlparse.c */

    unsigned OSPPXMLDocParse(OSPTBFR **, OSPTXMLELEM **);
    unsigned OSPPXMLDocParseElem(OSPTBFR **, OSPTXMLENC, OSPTXMLELEM **);
    unsigned OSPPXMLDocGetAttrs(OSPTBFR **, OSPTXMLENC, OSPTLIST *);
    unsigned OSPPXMLDocGetAttr(OSPTBFR **, OSPTXMLENC, OSPTXMLATTR **);
    unsigned OSPPXMLDocGetContent(OSPTBFR **, OSPTXMLENC, OSPTLIST *, OSPTBFR **);
    unsigned OSPPXMLDocGetCdata(OSPTBFR **, OSPTXMLENC, OSPTBFR **);
    unsigned OSPPXMLDocGetName(OSPTBFR **, OSPTXMLENC, OSPTBFR **);
    unsigned OSPPXMLDocSkipProlog(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipAllMisc(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipDTD(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipMarkupDecl(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipDecl(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipComment(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipPI(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipElementDecl(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipEntityDecl(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipAttlist(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipPastCdataBeg(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocSkipPastCdataEnd(OSPTBFR **, OSPTXMLENC);
    unsigned OSPPXMLDocIsDecl(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsComment(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsPI(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsDTD(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsElementDecl(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsAttlist(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsEntityDecl(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsCdata(OSPTBFR **, OSPTXMLENC, unsigned *);
    unsigned OSPPXMLDocIsCdataEnd(OSPTBFR **, OSPTXMLENC, unsigned *);

    void OSPPXMLDocPeekCharN(OSPTBFR **, OSPTXMLENC, unsigned, unsigned char *, int *);

#ifdef __cplusplus
}
#endif

#endif /* _OSPXMLDOC_H */
