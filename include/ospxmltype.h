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
 * ospxmltype.h - Structures and prototypes for generic XML character
 *                types. These are more or less a duplication of the
 *                ANSI ctype.h functions. It is necessary to write
 *                them from scratch, though, in order to ensure
 *                strict XML compliance.
 *
 *                Note that the implementation treats each XML character
 *                as a C char, which is mostly likely 8 bits. The XML
 *                decoding routines still accept 16-bit characters
 *                though. Since the OSP uses only ASCII characters,
 *                any 16-bit encoding can be safely converted to 8-bit
 *                char variables.
 */

#ifndef _OSPXMLTYPE_H
#define _OSPXMLTYPE_H

/*
 * The character types we care about, as bit masks for an eventual
 * ctype-like array.
 */

#define OSPC_XMLTYPE_NONE   0x00
#define OSPC_XMLTYPE_CHAR   0x01
#define OSPC_XMLTYPE_SPACE  0x02
#define OSPC_XMLTYPE_LETTER 0x04
#define OSPC_XMLTYPE_DIGIT  0x08
#define OSPC_XMLTYPE_NAME   0x10
#define OSPC_XMLTYPE_NAME1  0x20

/*
 * external definition of the XML ctype-like array; this is not
 * a static array so we can use macros to access it
 */

extern const unsigned char OSPVXMLCType[];

/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospxmlelem.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */

#define OSPPXMLIsChar(ospvChar)   (OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_CHAR)
#define OSPPXMLIsSpace(ospvChar)  (OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_SPACE)
#define OSPPXMLIsLetter(ospvChar) (OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_LETTER)
#define OSPPXMLIsDigit(ospvChar)  (OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_DIGIT)
#define OSPPXMLIsName(ospvChar)   (OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_NAME)
#define OSPPXMLIsName1(ospvChar)  (OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_NAME1)


/**/
/*-----------------------------------------------------------------------*
 * function prototypes
 *-----------------------------------------------------------------------*/

#ifndef OSPPXMLIsChar
unsigned OSPPXMLIsChar(unsigned char);
#endif

#ifndef OSPPXMLIsSpace
unsigned OSPPXMLIsSpace(unsigned char);
#endif

#ifndef OSPPXMLIsLetter
unsigned OSPPXMLIsLetter(unsigned char);
#endif

#ifndef OSPPXMLIsDigit
unsigned OSPPXMLIsDigit(unsigned char);
#endif

#ifndef OSPPXMLIsName
unsigned OSPPXMLIsName(unsigned char);
#endif

#ifndef OSPPXMLIsName1
unsigned OSPPXMLIsName1(unsigned char);
#endif

#endif

