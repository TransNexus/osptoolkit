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







#include "osp/osp.h"
#include "osp/ospasn1.h"
#include "osp/osperrno.h"
#include "osp/osplist.h"

#include "osptneputil.h"
#include "osptnep.h"


/* Given a string of characters, print out their hex and alphanumeric encoding.
 *
 * Input: the string to be printed and the length of that string.
 */
void OSPPDumpHex( 
    unsigned char* ospvDumpee, 
    unsigned       ospvDumpeeLen
)
{
    unsigned lineIndex     = 0;
    int nextLineLen   = 0; 
  
    if ( ( ospvDumpee == NULL ) || ( ospvDumpeeLen <= 0 ) )
    {
        OSPM_DBGMISC(( "Unable to display string - null or empty\n" ));
    }

    else
    {
        OSPM_DBGMISC(( "length: %d\n", ospvDumpeeLen ));

        for ( lineIndex = 0; 
              lineIndex < ospvDumpeeLen; 
              lineIndex += OSPC_ENROLL_HEX_CHARS_PER_LINE )
        {
            if ( (ospvDumpeeLen - lineIndex) < OSPC_ENROLL_HEX_CHARS_PER_LINE )
            {
                nextLineLen = ospvDumpeeLen - lineIndex;
            }

            else
            {
                nextLineLen = OSPC_ENROLL_HEX_CHARS_PER_LINE;
            }

            OSPPDumpHexLine( ospvDumpee + lineIndex, nextLineLen );
        }
    }

}

/* Dump a line of hex characters. Pad the end with spaces if there aren't
 * enough characters.
 *
 * Input: the line to be dumped and the number of characters to be dumped.
 */
void OSPPDumpHexLine (
    unsigned char* ospvDumpeeLine,
    unsigned       ospvDumpeeLineLen
)
{
    unsigned charIndex = 0;

    if ( ( ospvDumpeeLine != OSPC_OSNULL ) && 
         ( ospvDumpeeLineLen > 0 ) )
    {
        /* For ( each character in the line )
         *  o print out a character from the line if the index is within
         *    the length of the string
         *  o print out a space if the index is out of the string's bounds.
         */
        for ( charIndex = 0; 
              charIndex < OSPC_ENROLL_HEX_CHARS_PER_LINE; 
              charIndex++ )
        {
            if ( charIndex < ospvDumpeeLineLen )
            {
                OSPM_DBGMISC(( "%.2X ", ospvDumpeeLine[ charIndex ] ));
            }

            else
            {
                OSPM_DBGMISC(( "   " ));
            }

            /* At the end of each octet, print an additional space: */
            if ( ( charIndex % 8 ) == 7 )
            {
                OSPM_DBGMISC(( "   " ));
            }
        }

        /* For ( every character in the line )
         *  o print it out at the end of the line if it's alphanumeric
         *  o print out a period to represent a non-alphanumeric character
         *    otherwise
         */
        for ( charIndex = 0; charIndex < ospvDumpeeLineLen; charIndex++ )
        {
            if ( OSPM_ISALNUM( ospvDumpeeLine[ charIndex ] ) )
            {
                OSPM_DBGMISC(( "%c", ospvDumpeeLine[ charIndex ] ));
            }

            else
            {    
                OSPM_DBGMISC(( "." ));
            }
        }

        OSPM_DBGMISC(( "\n" ));
    }

}
