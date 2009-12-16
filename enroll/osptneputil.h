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

#include "osp/osposincl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines how many octets are dumped per line: */
#define OSPC_ENROLL_OCTETS_PER_LINE         2

/* Defines how may characters are dumped line: */
#define OSPC_ENROLL_HEX_CHARS_PER_LINE      (8 * OSPC_ENROLL_OCTETS_PER_LINE)

/*
 * Given a string of characters, print out their hex and alphanumeric encoding.
 *
 * Input: the string to be printed and the length of that string.
 */
void OSPPDumpHex(unsigned char *ospvDumpee, unsigned ospvDumpeeLen);

/*
 * Dump a line of hex characters. Pad the end with spaces if there aren't
 * enough characters.
 *
 * Input: the line to be dumped and the number of characters to be dumped.
 */
void OSPPDumpHexLine(unsigned char *ospvDumpeeLine, unsigned ospvDumpeeLineLen);

#ifdef __cplusplus
}
#endif
