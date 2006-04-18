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
 * ospxmltype.c - Data variables and functions for XML character
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

#include <assert.h>
#include "osp/osp.h"
#include "osp/ospxmltype.h"

/*
 * the XML ctype-like array; this is not a static array so we can
 * use macros to access it
 */

const unsigned char OSPVXMLCType[] =
{
  /* 00     */  OSPC_XMLTYPE_NONE ,
  /* 01     */  OSPC_XMLTYPE_NONE ,
  /* 02     */  OSPC_XMLTYPE_NONE ,
  /* 03     */  OSPC_XMLTYPE_NONE ,
  /* 04     */  OSPC_XMLTYPE_NONE ,
  /* 05     */  OSPC_XMLTYPE_NONE ,
  /* 06     */  OSPC_XMLTYPE_NONE ,
  /* 07     */  OSPC_XMLTYPE_NONE ,
  /* 08     */  OSPC_XMLTYPE_NONE ,
  /* 09 \t  */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_SPACE ,
  /* 0A \r  */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_SPACE ,
  /* 0B     */  OSPC_XMLTYPE_NONE ,
  /* 0C     */  OSPC_XMLTYPE_NONE ,
  /* 0D \n  */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_SPACE ,
  /* 0E     */  OSPC_XMLTYPE_NONE ,
  /* 0F     */  OSPC_XMLTYPE_NONE ,
  /* 10     */  OSPC_XMLTYPE_NONE ,
  /* 11     */  OSPC_XMLTYPE_NONE ,
  /* 12     */  OSPC_XMLTYPE_NONE ,
  /* 13     */  OSPC_XMLTYPE_NONE ,
  /* 14     */  OSPC_XMLTYPE_NONE ,
  /* 15     */  OSPC_XMLTYPE_NONE ,
  /* 16     */  OSPC_XMLTYPE_NONE ,
  /* 17     */  OSPC_XMLTYPE_NONE ,
  /* 18     */  OSPC_XMLTYPE_NONE ,
  /* 19     */  OSPC_XMLTYPE_NONE ,
  /* 1A     */  OSPC_XMLTYPE_NONE ,
  /* 1B     */  OSPC_XMLTYPE_NONE ,
  /* 1C     */  OSPC_XMLTYPE_NONE ,
  /* 1D     */  OSPC_XMLTYPE_NONE ,
  /* 1E     */  OSPC_XMLTYPE_NONE ,
  /* 1F     */  OSPC_XMLTYPE_NONE ,
  /* 20 ' ' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_SPACE ,
  /* 21 '!' */  OSPC_XMLTYPE_CHAR ,
  /* 22 '"' */  OSPC_XMLTYPE_CHAR ,
  /* 23 '#' */  OSPC_XMLTYPE_CHAR ,
  /* 24 '$' */  OSPC_XMLTYPE_CHAR ,
  /* 25 '%' */  OSPC_XMLTYPE_CHAR ,
  /* 26 '&' */  OSPC_XMLTYPE_CHAR ,
  /* 27 ''' */  OSPC_XMLTYPE_CHAR ,
  /* 28 '(' */  OSPC_XMLTYPE_CHAR ,
  /* 29 ')' */  OSPC_XMLTYPE_CHAR ,
  /* 2A '*' */  OSPC_XMLTYPE_CHAR ,
  /* 2B '+' */  OSPC_XMLTYPE_CHAR ,
  /* 2C ',' */  OSPC_XMLTYPE_CHAR ,
  /* 2D '-' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_NAME ,
  /* 2E '.' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_NAME ,
  /* 2F '/' */  OSPC_XMLTYPE_CHAR ,
  /* 30 '0' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 31 '1' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 32 '2' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 33 '3' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 34 '4' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 35 '5' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 36 '6' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 37 '7' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 38 '8' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 39 '9' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_DIGIT | OSPC_XMLTYPE_NAME ,
  /* 3A ':' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 3B ';' */  OSPC_XMLTYPE_CHAR ,
  /* 3C '<' */  OSPC_XMLTYPE_CHAR ,
  /* 3D '=' */  OSPC_XMLTYPE_CHAR ,
  /* 3E '>' */  OSPC_XMLTYPE_CHAR ,
  /* 3F '?' */  OSPC_XMLTYPE_CHAR ,
  /* 40 '@' */  OSPC_XMLTYPE_CHAR ,
  /* 41 'A' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 42 'B' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 43 'C' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 44 'D' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 45 'E' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 46 'F' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 47 'G' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 48 'H' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 49 'I' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 4A 'J' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 4B 'K' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 4C 'L' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 4D 'M' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 4E 'N' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 4F 'O' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 50 'P' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 51 'Q' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 52 'R' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 53 'S' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 54 'T' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 55 'U' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 56 'V' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 57 'W' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 58 'X' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 59 'Y' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 5A 'Z' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 5B '[' */  OSPC_XMLTYPE_CHAR ,
  /* 5C '\' */  OSPC_XMLTYPE_CHAR ,
  /* 5D ']' */  OSPC_XMLTYPE_CHAR ,
  /* 5E '^' */  OSPC_XMLTYPE_CHAR ,
  /* 5F '_' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 60 '`' */  OSPC_XMLTYPE_CHAR ,
  /* 61 'a' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 62 'b' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 63 'c' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 64 'd' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 65 'e' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 66 'f' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 67 'g' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 68 'h' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 69 'i' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 6A 'j' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 6B 'k' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 6C 'l' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 6D 'm' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 6E 'n' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 6F 'o' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 70 'p' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 71 'q' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 72 'r' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 73 's' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 74 't' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 75 'u' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 76 'v' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 77 'w' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 78 'x' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 79 'y' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 7A 'z' */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* 7B '{' */  OSPC_XMLTYPE_CHAR ,
  /* 7C '|' */  OSPC_XMLTYPE_CHAR ,
  /* 7D '}' */  OSPC_XMLTYPE_CHAR ,
  /* 7E '~' */  OSPC_XMLTYPE_CHAR ,
  /* 7F     */  OSPC_XMLTYPE_CHAR ,
  /* 80     */  OSPC_XMLTYPE_CHAR ,
  /* 81     */  OSPC_XMLTYPE_CHAR ,
  /* 82     */  OSPC_XMLTYPE_CHAR ,
  /* 83     */  OSPC_XMLTYPE_CHAR ,
  /* 84     */  OSPC_XMLTYPE_CHAR ,
  /* 85     */  OSPC_XMLTYPE_CHAR ,
  /* 86     */  OSPC_XMLTYPE_CHAR ,
  /* 87     */  OSPC_XMLTYPE_CHAR ,
  /* 88     */  OSPC_XMLTYPE_CHAR ,
  /* 89     */  OSPC_XMLTYPE_CHAR ,
  /* 8A     */  OSPC_XMLTYPE_CHAR ,
  /* 8B     */  OSPC_XMLTYPE_CHAR ,
  /* 8C     */  OSPC_XMLTYPE_CHAR ,
  /* 8D     */  OSPC_XMLTYPE_CHAR ,
  /* 8E     */  OSPC_XMLTYPE_CHAR ,
  /* 8F     */  OSPC_XMLTYPE_CHAR ,
  /* 90     */  OSPC_XMLTYPE_CHAR ,
  /* 91     */  OSPC_XMLTYPE_CHAR ,
  /* 92     */  OSPC_XMLTYPE_CHAR ,
  /* 93     */  OSPC_XMLTYPE_CHAR ,
  /* 94     */  OSPC_XMLTYPE_CHAR ,
  /* 95     */  OSPC_XMLTYPE_CHAR ,
  /* 96     */  OSPC_XMLTYPE_CHAR ,
  /* 97     */  OSPC_XMLTYPE_CHAR ,
  /* 98     */  OSPC_XMLTYPE_CHAR ,
  /* 99     */  OSPC_XMLTYPE_CHAR ,
  /* 9A     */  OSPC_XMLTYPE_CHAR ,
  /* 9B     */  OSPC_XMLTYPE_CHAR ,
  /* 9C     */  OSPC_XMLTYPE_CHAR ,
  /* 9D     */  OSPC_XMLTYPE_CHAR ,
  /* 9E     */  OSPC_XMLTYPE_CHAR ,
  /* 9F     */  OSPC_XMLTYPE_CHAR ,
  /* A0     */  OSPC_XMLTYPE_CHAR ,
  /* A1     */  OSPC_XMLTYPE_CHAR ,
  /* A2     */  OSPC_XMLTYPE_CHAR ,
  /* A3     */  OSPC_XMLTYPE_CHAR ,
  /* A4     */  OSPC_XMLTYPE_CHAR ,
  /* A5     */  OSPC_XMLTYPE_CHAR ,
  /* A6     */  OSPC_XMLTYPE_CHAR ,
  /* A7     */  OSPC_XMLTYPE_CHAR ,
  /* A8     */  OSPC_XMLTYPE_CHAR ,
  /* A9     */  OSPC_XMLTYPE_CHAR ,
  /* AA     */  OSPC_XMLTYPE_CHAR ,
  /* AB     */  OSPC_XMLTYPE_CHAR ,
  /* AC     */  OSPC_XMLTYPE_CHAR ,
  /* AD     */  OSPC_XMLTYPE_CHAR ,
  /* AE     */  OSPC_XMLTYPE_CHAR ,
  /* AF     */  OSPC_XMLTYPE_CHAR ,
  /* B0     */  OSPC_XMLTYPE_CHAR ,
  /* B1     */  OSPC_XMLTYPE_CHAR ,
  /* B2     */  OSPC_XMLTYPE_CHAR ,
  /* B3     */  OSPC_XMLTYPE_CHAR ,
  /* B4     */  OSPC_XMLTYPE_CHAR ,
  /* B5     */  OSPC_XMLTYPE_CHAR ,
  /* B6     */  OSPC_XMLTYPE_CHAR ,
  /* B7     */  OSPC_XMLTYPE_CHAR ,
  /* B8     */  OSPC_XMLTYPE_CHAR ,
  /* B9     */  OSPC_XMLTYPE_CHAR ,
  /* BA     */  OSPC_XMLTYPE_CHAR ,
  /* BB     */  OSPC_XMLTYPE_CHAR ,
  /* BC     */  OSPC_XMLTYPE_CHAR ,
  /* BD     */  OSPC_XMLTYPE_CHAR ,
  /* BE     */  OSPC_XMLTYPE_CHAR ,
  /* BF     */  OSPC_XMLTYPE_CHAR ,
  /* C0     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C1     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C2     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C3     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C4     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C5     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C6     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C7     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C8     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* C9     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* CA     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* CB     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* CC     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* CD     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* CE     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* CF     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D0     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D1     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D2     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D3     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D4     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D5     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D6     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D7     */  OSPC_XMLTYPE_CHAR ,
  /* D8     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* D9     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* DA     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* DB     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* DC     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* DD     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* DE     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* DF     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E0     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E1     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E2     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E3     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E4     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E5     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E6     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E7     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E8     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* E9     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* EA     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* EB     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* EC     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* ED     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* EE     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* EF     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F0     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F1     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F2     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F3     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F4     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F5     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F6     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F7     */  OSPC_XMLTYPE_CHAR ,
  /* F8     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* F9     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* FA     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* FB     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* FC     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* FD     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* FE     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
  /* FF     */  OSPC_XMLTYPE_CHAR | OSPC_XMLTYPE_LETTER | OSPC_XMLTYPE_NAME | OSPC_XMLTYPE_NAME1 ,
};

#define OSPC_XML_CTYPESIZE (sizeof(OSPVXMLCType)/sizeof(unsigned char))

/**/
/*-----------------------------------------------------------------------*
 * OSPPXMLIsChar() - is a byte a valid XML character
 *-----------------------------------------------------------------------*/

unsigned              /* returns non-zero if true */
OSPPXMLIsChar(
    unsigned char ospvChar
)
{
    unsigned ospvIs = OSPC_FALSE;

    if (ospvChar <= OSPC_XML_CTYPESIZE)
    {
        ospvIs = OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_CHAR;
    }
    return(ospvIs);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPXMLIsSpace() - is a byte a valid XML space character
 *-----------------------------------------------------------------------*/

unsigned              /* returns non-zero if true */
OSPPXMLIsSpace(
    unsigned char ospvChar
)
{
    unsigned ospvIs = OSPC_FALSE;

    if (ospvChar <= OSPC_XML_CTYPESIZE);
    {
        ospvIs = OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_SPACE;
    }
    return(ospvIs);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPXMLIsLetter() - is a byte a valid XML letter character
 *-----------------------------------------------------------------------*/

unsigned              /* returns non-zero if true */
OSPPXMLIsLetter(
    unsigned char ospvChar
)
{
    unsigned ospvIs = OSPC_FALSE;

    if (ospvChar <= OSPC_XML_CTYPESIZE)
    {
        ospvIs = OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_LETTER;
    }
    return(ospvIs);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPXMLIsDigit() - is a byte a valid XML digit character
 *-----------------------------------------------------------------------*/

unsigned              /* returns non-zero if true */
OSPPXMLIsDigit(
    unsigned char ospvChar
)
{
    unsigned ospvIs = OSPC_FALSE;

    if (ospvChar <= OSPC_XML_CTYPESIZE)
    {
        ospvIs = OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_DIGIT;
    }
    return(ospvIs);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPXMLIsName() - is a byte a valid XML name character
 *-----------------------------------------------------------------------*/

unsigned              /* returns non-zero if true */
OSPPXMLIsName(
    unsigned char ospvChar
)
{
    unsigned ospvIs = OSPC_FALSE;

    if (ospvChar <= OSPC_XML_CTYPESIZE)
    {
        ospvIs = OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_NAME;
    }
    return(ospvIs);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPXMLIsName1() - is byte valid as first character in XML name
 *-----------------------------------------------------------------------*/

unsigned              /* returns non-zero if true */
OSPPXMLIsName1(
    unsigned char ospvChar
)
{
    unsigned ospvIs = OSPC_FALSE;

    if (ospvChar <= OSPC_XML_CTYPESIZE)
    {
        ospvIs = OSPVXMLCType[ospvChar] & OSPC_XMLTYPE_NAME1;
    }
    return(ospvIs);
}


