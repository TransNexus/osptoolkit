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
 * ospxmlutil.c - Utility functions for parsing and encoding XML documents.
 */

#include "osp/osp.h"
#include "osp/ospbfr.h"
#include "osp/ospxmltype.h"
#include "osp/ospxmldoc.h"

/* pre-defined entity names */
const OSPT_XML_ENTITY OSPVXMLDocEntities[] = {
    { '<',  (unsigned char *)"lt" },
    { '>',  (unsigned char *)"gt" },
    { '&',  (unsigned char *)"amp" },
    { '\'', (unsigned char *)"apos" },
    { '"',  (unsigned char *)"quot" }
};

const unsigned OSPVXMLDocEntitiesSize = sizeof(OSPVXMLDocEntities) / sizeof(OSPT_XML_ENTITY);

/*
 * OSPPXMLDocIsMatch() - does the document match a given string
 */

unsigned OSPPXMLDocIsMatch(             /* returns error code */
    OSPTBFR **ospvBfrAddr,              /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,          /* character encoding         */
    const unsigned char *ospvString,    /* string to check for match  */
    unsigned ospvStringLen,             /* length of match string     */
    unsigned char ospvScratch[],        /* place to store characters  */
    unsigned *ospvIsMatch)              /* place to put answer        */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    int tmpErrCode = OSPC_ERR_NO_ERROR;
    unsigned cnt = 0;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvString == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (ospvScratch == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (ospvIsMatch == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume it's not a match for safety (in case of error break) */
        *ospvIsMatch = OSPC_FALSE;
        /*
         * In the following loop we "peek" at enough characters to
         * compare against the string. Note that we're storing the
         * "peeked" characters in the scratch array, which is passed
         * to us. That way the calling function has to worry about
         * sizing the scratch array. But that's okay, since the caller
         * presumably knows how big it needs to be. (Hint: It should
         * be the same size as the match string.)
         */

        for (cnt = 0; cnt < ospvStringLen; cnt++) {
            OSPPXMLDocPeekCharN(ospvBfrAddr, ospvEncoding, cnt, &ospvScratch[cnt], &tmpErrCode);
            if (tmpErrCode != OSPC_ERR_NO_ERROR) {
                break;
            }
        }

        /*
         * If we didn't get enough characters to compare, that's not
         * an error; but it's certainly not a match either. If we
         * didn't get a full scratch buffer, there's no need to
         * continue. We've already set the default return to false.
         */
        if (cnt == ospvStringLen) {
            /* be sure to null terminate the scratch string */
            ospvScratch[cnt] = 0;

            /* now that we've extracted the tag characters, see what we've got */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                if (OSPM_MEMCMP((const char *)ospvScratch, (const char *)ospvString, ospvStringLen) == 0) {
                    *ospvIsMatch = OSPC_TRUE;
                }
            }
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLDocSkipPast() - skip past a string in the document
 */

unsigned OSPPXMLDocSkipPast(            /* returns error code */
    OSPTBFR **ospvBfrAddr,              /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,          /* character encoding */
    const unsigned char *ospvString,    /* string to skip past */
    unsigned char ospvScratch[])        /* place to store characters */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned cnt;
    unsigned len = 0;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvString == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (ospvScratch == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    /*
     * In the following loop we pre-load the scratch buffer with enough
     * to compare against the string. Note that we're storing the
     * characters in the scratch array, which is passed to us. That way
     * the calling function has to worry about sizing the scratch array.
     * But that's okay, since the caller presumably knows how big it
     * needs to be. (Hint: It should be the same size as the match string.)
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        for (cnt = 0, len = OSPM_STRLEN((const char *) ospvString);
            ((cnt < len) && (ospvErrCode == OSPC_ERR_NO_ERROR)); cnt++) 
        {
            ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &ospvScratch[cnt]);
        }

        /* be sure to null terminate the scratch string */
        ospvScratch[cnt] = 0;
    }
    /*
     * Now we start the main loop of the function. Here we're checking
     * for a match and we continue reading characters until we find one.
     */

    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (OSPM_MEMCMP((const char *) ospvScratch, (const char *) ospvString, len) == 0) {
            /* we've found the match, so we're done */
            break;
        }

        /*
         * No match yet, so we keep looking. First we need to shift
         * the scratch buffer to make room for the new character.
         * If a non-destructive memcpy is available, it might be
         * a good candidate for optimizing this part.
         */
        for (cnt = 0; cnt < (len - 1); cnt++) {
            ospvScratch[cnt] = ospvScratch[cnt + 1];
        }
        ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &ospvScratch[len - 1]);
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLDocSkipPastChar() - skip XML until just past a single character
 */

unsigned OSPPXMLDocSkipPastChar(    /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,      /* character encoding for the document */
    unsigned char ospvChar)         /* character to skip past */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    /* loop looking for the character */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
        if (readChar == ospvChar) {
            break;
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLDocGetEncoding()
 */

unsigned OSPPXMLDocGetEncoding( /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC *ospvEncoding  /* place to store encoding type */
    )
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    int readChar;
    unsigned char char1 = '\0';
    unsigned char char2 = '\0';

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    *ospvEncoding = OSPC_XENC_UNKNOW;

    /*
     * First have a look at the first two bytes in the buffer. From
     * these, we can deduce the character encoding.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        readChar = OSPPBfrPeekByte(*ospvBfrAddr);
        if (readChar != -1) {
            char1 = (char) readChar;
        } else {
            ospvErrCode = OSPC_ERR_BUF_INCOMPLETE;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        readChar = OSPPBfrPeekByteN(*ospvBfrAddr, 1);
        if (readChar != -1) {
            char2 = (char) readChar;
        } else {
            ospvErrCode = OSPC_ERR_XML_INCOMPLETE;
        }
    }

    /*
     * Now we check the characters, this stuff is pretty much
     * straight from the XML specification.
     */

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if ((char1 == OSPC_XMLDOC_UTF16MSB) && (char2 == OSPC_XMLDOC_UTF16LSB)) {
            /* we found the UTF-16 byte order mark, it's big-endian */
            *ospvEncoding = OSPC_XENC_UTF16B;
        } else if ((char1 == OSPC_XMLDOC_UTF16LSB) && (char2 == OSPC_XMLDOC_UTF16LSB)) {
            /* we found the UTF-16 byte order mark, it's little-endian */
            *ospvEncoding = OSPC_XENC_UTF16L;
        } else if ((char1 == OSPC_XMLDOC_UTF16NULL) && (char2 == OSPC_XMLDOC_OPEN)) {
            /* UTF-16 without the byte order mark, big-endian */
            *ospvEncoding = OSPC_XENC_UTF16B;
        } else if ((char1 == OSPC_XMLDOC_OPEN) && (char2 == OSPC_XMLDOC_UTF16NULL)) {
            /* UTF-16 without the byte order mark, little-endian */
            *ospvEncoding = OSPC_XENC_UTF16L;
        } else if ((char1 == OSPC_XMLDOC_OPEN) && (char2 == OSPC_XMLDOC_QUEST)) {
            /* this is UTF-8 */
            *ospvEncoding = OSPC_XENC_UTF8;
        } else {
            /*
             * According to the XML standard, anything other
             * than UTF8 or UTF16 must have a character
             * encoding declaration. Since we've already
             * checked for UTF16 and we've already looked
             * for the start of a character encoding, that
             * only leaves UTF8.
             */
            *ospvEncoding = OSPC_XENC_UTF8;
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLDocTranslateEntity() - translate an entity name into its value
 */

unsigned OSPPXMLDocTranslateEntity( /* returns error code */
    unsigned char *ospvName, 
    unsigned char *ospvChar)        /* place to store character */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned cnt;

    if (ospvName == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (ospvChar == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* is this a character reference */
        if (*ospvName == OSPC_XMLDOC_CHARREF) {
            ospvName++;
            /* is it hex? */
            if (*ospvName == OSPC_XMLDOC_HEXREF) {
                /* this one's hex */
                ospvName++;
                for (*ospvChar = 0; *ospvName; ospvName++) {
                    *ospvChar *= 0x10;
                    if ((*ospvName >= '0') && (*ospvName <= '9')) {
                        unsigned char x = *ospvChar;
                        *ospvChar = (unsigned char)(x + ((unsigned char)*ospvName - (unsigned char)'0'));
                    } else if ((*ospvName >= 'a') && (*ospvName <= 'f')) {
                        unsigned char x = *ospvChar;
                        *ospvChar = (unsigned char)(x + ((unsigned char)*ospvName - (unsigned char)'a') + 10);
                    } else if ((*ospvName >= 'A') && (*ospvName <= 'F')) {
                        unsigned char x = *ospvChar;
                        *ospvChar = (unsigned char)(x + ((unsigned char)*ospvName - (unsigned char)'A') + 10);
                    } else {
                        ospvErrCode = OSPC_ERR_XML_BAD_ENTITY;
                    }
                }
            } else {
                /* just a decimal character reference */
                for (*ospvChar = 0; *ospvName; ospvName++) {
                    *ospvChar *= 10;
                    if ((*ospvName >= '0') && (*ospvName <= '9')) {
                        unsigned char x = *ospvChar;
                        *ospvChar = (unsigned char)(x + ((unsigned char)*ospvName - (unsigned char)'0'));
                    } else {
                        ospvErrCode = OSPC_ERR_XML_BAD_ENTITY;
                    }
                }
            }
        } else {
            /* not a character reference - check for predefined name */
            for (cnt = 0; cnt < OSPVXMLDocEntitiesSize; cnt++) {
                if (OSPM_MEMCMP((const char *)ospvName, (const char *)OSPVXMLDocEntities[cnt].ospmEntName, 
                    OSPM_STRLEN((const char *)OSPVXMLDocEntities[cnt].ospmEntName)) == 0) 
                {
                    break;
                }
            }
            if (cnt < OSPVXMLDocEntitiesSize) {
                *ospvChar = OSPVXMLDocEntities[cnt].ospmEntValue;
            } else {
                ospvErrCode = OSPC_ERR_XML_BAD_ENTITY;
            }
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLDocReadChar() - read a single character from an XML document
 */

unsigned OSPPXMLDocReadChar(    /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for document */
    unsigned char *ospvChar)    /* place to store read character */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    int readChar = '\0';

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvChar == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* read the first byte */
        readChar = OSPPBfrReadByte(*ospvBfrAddr);
        if (readChar == -1) {
            ospvErrCode = OSPC_ERR_BUF_INCOMPLETE;
        }
    }

    /* what we do next depends on the encoding */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* UTF-8 is pretty easy -- we're done */
        if (ospvEncoding == OSPC_XENC_UTF8) {
            *ospvChar = (unsigned char)readChar;
        } else {
            if (ospvEncoding == OSPC_XENC_UTF16L) {
                *ospvChar = (unsigned char)readChar;
                readChar = OSPPBfrReadByte(*ospvBfrAddr);
                if (readChar == -1) {
                    ospvErrCode = OSPC_ERR_BUF_INCOMPLETE;
                } else if (readChar != OSPC_XMLDOC_UTF16NULL) {
                    ospvErrCode = OSPC_ERR_XML_BAD_ENC;
                }
            } else {
                if (ospvEncoding == OSPC_XENC_UTF16B) {
                    readChar = OSPPBfrReadByte(*ospvBfrAddr);
                    if (readChar == -1) {
                        ospvErrCode = OSPC_ERR_BUF_INCOMPLETE;
                    } else if (readChar != OSPC_XMLDOC_UTF16NULL) {
                        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
                    }
                    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                        *ospvChar = (unsigned char)readChar;
                    }
                }
            }
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLDocPeekCharN() - look at the Nth character in an XML document
 */

void OSPPXMLDocPeekCharN(
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned ospvCnt,           /* character to look at */
    unsigned char *ospvChar,    /* place to store character */
    int *ospvErrCode)           /* error code               */
{
    int readChar = '\0';

    if (ospvBfrAddr == OSPC_OSNULL) {
        *ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        *ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        *ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvChar == OSPC_OSNULL) {
        *ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (*ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* handle each encoding separately */
        switch (ospvEncoding) {
            case OSPC_XENC_UTF8:
                readChar = OSPPBfrPeekByteN(*ospvBfrAddr, ospvCnt);
                break;
            case OSPC_XENC_UTF16L:
                readChar = OSPPBfrPeekByteN(*ospvBfrAddr, 2 * ospvCnt);
                break;
            case OSPC_XENC_UTF16B:
                readChar = OSPPBfrPeekByteN(*ospvBfrAddr, (2 * ospvCnt + 1));
                break;
            default:
                *ospvErrCode = (unsigned) OSPC_ERR_OS_FAILURE;
                break;
        }
    }
    if (*ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (readChar == -1) {
            *ospvErrCode = OSPC_ERR_BUF_INCOMPLETE;
        } else {
            *ospvChar = (unsigned char) readChar;
        }
    }

    return;
}

/*
 * OSPPXMLDocSkipWhite() - skip whitespace in an XML document
 */

unsigned OSPPXMLDocSkipWhite(   /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar = 0;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        OSPPXMLDocPeekCharN(ospvBfrAddr, ospvEncoding, 0, &readChar, (int *)&ospvErrCode);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (OSPPXMLIsSpace(readChar)) {
                ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
            } else {
                break;
            }
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLAddReference() - add entity references to XML character data
 */

unsigned OSPPXMLAddReference(           /* returns error code */
    const unsigned char *ospvRawData,   /* data to reference */
    unsigned ospvDataSize,              /* number of bytes of data */
    OSPTBFR **ospvBfrAddr)              /* buffer to stuff output */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned dataCnt;
    unsigned char ch;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvRawData == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        for (dataCnt = 0;
             (dataCnt < ospvDataSize) && (ospvErrCode == OSPC_ERR_NO_ERROR);
             dataCnt++, ospvRawData++) 
        {
            /*
             * There are two things that will cause us to replace a raw
             * data byte with an entity reference: (1) a value that
             * XML does not consider a valid character (e.g. 0); and
             * (2) the "greater than" character (>) that marks the
             * end of element tags and CDATA sections. (Other sections
             * aren't relevant because they won't be confused with
             * character data anyway.)
             */
            if (OSPPXMLIsChar(*ospvRawData) &&
                (*ospvRawData != OSPC_XMLDOC_CLOSE)) {
                /* this character's okay as is */
                if (OSPPBfrWriteByte(ospvBfrAddr, *ospvRawData) != 1) {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
            } else {
                /* this one needs to be referenced */
                if (OSPPBfrWriteByte(ospvBfrAddr, OSPC_XMLDOC_REF) != 1) {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    if (OSPPBfrWriteByte(ospvBfrAddr, OSPC_XMLDOC_CHARREF) != 1) {
                        ospvErrCode = OSPC_ERR_BUF_EMPTY;
                    }
                }
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    if (OSPPBfrWriteByte(ospvBfrAddr, OSPC_XMLDOC_HEXREF) != 1) {
                        ospvErrCode = OSPC_ERR_BUF_EMPTY;
                    }
                }
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    ch = (char) ('0' + (*ospvRawData) / 16);
                    if (ch > '9') {
                        ch += 'a' - '0' - 10;
                    }
                    if (OSPPBfrWriteByte(ospvBfrAddr, ch) != 1) {
                        ospvErrCode = OSPC_ERR_BUF_EMPTY;
                    }
                }
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    ch = (char) ('0' + ((*ospvRawData) % 16));
                    if (ch > '9') {
                        ch += 'a' - '0' - 10;
                    }
                    if (OSPPBfrWriteByte(ospvBfrAddr, ch) != 1) {
                        ospvErrCode = OSPC_ERR_BUF_EMPTY;
                    }
                }
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    if (OSPPBfrWriteByte(ospvBfrAddr, OSPC_XMLDOC_REFEND) != 1) {
                        ospvErrCode = OSPC_ERR_BUF_EMPTY;
                    }
                }
            }
        }
    }

    return (ospvErrCode);
}

/*
 * OSPPXMLDereference() - resolve entity references in XML character data
 */

unsigned OSPPXMLDereference(            /* returns error code */
    const unsigned char *ospvCharData,  /* character data to deference */
    unsigned *ospvDataSize,             /* input: max, output: actual */
    unsigned char *ospvRawData)         /* place to put output */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned dataCnt;
    unsigned charCnt;
    unsigned char entity[OSPC_XMLDOC_ENTITYSIZE];
    unsigned char *start = ospvRawData;

    /* Description of the following variable:
     * TRUE = Find the length of the data and write the data to ospvRawData
     * FALSE = Find only the length of the data
     */
    unsigned flag = OSPC_FALSE;

    /* If flag == TRUE, the following is used to store the size of the entity
     * after it has been translated. 
     */
    unsigned char translatedEntity[OSPC_XMLDOC_ENTITYSIZE];

    if (ospvCharData == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvDataSize == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvRawData == OSPC_OSNULL) {
        if (*ospvDataSize == 0) {
            flag = OSPC_FALSE;
        } else {
            ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
        }
    } else {
        if (*ospvDataSize > 0) {
            flag = OSPC_TRUE;
        } else {
            ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
        }
    }

    /*
     * As we start the loop through the characters, we can end
     * the loop one of two ways: (1) we get to the end of the
     * string, which is indicated by a NULL byte; or (2) something
     * bad happens. Bad things will be flagged by ospvErrCode
     * so the loop can watch that.
     */

    /* The variable flag determines which mode this function is in.
     * If flag is set to FALSE, then this function only determines
     * the size of the data in ospvCharData; otherwise, this
     * function returns the processed data in ospvRawData as well
     * as its size.
     */

    /* keep track of how much data we've written */
    dataCnt = 0;

    /* and go into our loop */
    while ((ospvErrCode == OSPC_ERR_NO_ERROR) && (*ospvCharData != '\0')) {
        /* is the byte a reference instead of a raw character? */
        if (*ospvCharData == OSPC_XMLDOC_REF) {
            /* this is an entity reference, get the whole entity name */

            /* first skipping past the reference character */
            ospvCharData++;

            for (charCnt = 0; charCnt < OSPC_XMLDOC_ENTITYSIZE; charCnt++) {
                /* are we at the end of the reference? */
                if (*ospvCharData == '\0') {
                    /* yes, but we didn't finish! */
                    ospvErrCode = OSPC_ERR_XML_BAD_ENTITY;
                    break;
                } else if (*ospvCharData == OSPC_XMLDOC_REFEND) {
                    /*
                     * We've found the end of the entity reference;
                     * skip past it and break.
                     */
                    ospvCharData++;
                    break;
                } else {
                    /* just another part of the reference */
                    entity[charCnt] = *ospvCharData++;
                }
            }
            /* we got to the end of the reference, make sure no errors */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                entity[charCnt] = '\0';
                if (flag == OSPC_TRUE) {
                    ospvErrCode = OSPPXMLDocTranslateEntity(entity, ospvRawData);
                    ospvRawData++;
                    dataCnt++;
                } else {

                    /* Clear the buffer that temporarily holds the entity for counting */
                    for (charCnt = 0; charCnt < OSPC_XMLDOC_ENTITYSIZE; charCnt++) {
                        translatedEntity[charCnt] = '\0';
                    }

                    /* Get the entity */
                    ospvErrCode = OSPPXMLDocTranslateEntity(entity, translatedEntity);

                    /* Determine the size of the entity and add it to dataCnt *
                       dataCnt += OSPM_STRLEN((const char *)translatedEntity); */
                    dataCnt++;
                }
            }
        } else {
            if (flag == OSPC_TRUE) {
                /* just a plain old character */
                *ospvRawData++ = *ospvCharData++;
            } else {
                ospvCharData++;
            }
            dataCnt++;
        }

        /* make sure there's still room in the data area */
        if ((flag == OSPC_TRUE) && (dataCnt > *ospvDataSize)) {
            ospvErrCode = (unsigned) OSPC_ERR_OS_FAILURE;
            break;
        }
    }

    /* update the size of the data */
    *ospvDataSize = dataCnt;
    if (flag == OSPC_TRUE) {
        ospvRawData = start;
    }

    return (ospvErrCode);
}
