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
 * ospmsgutil.c - utility functions for OSP messages
 */

#include "osp/osp.h"
#include "osp/ospostime.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospxmlattr.h"
#include "osp/ospmsgattr.h"
#include "osp/ospxmldoc.h"
#include "osp/ospmsg.h"
#include "osp/ospb64.h"
#include "osp/osptnlog.h"


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgBinFromASCIIElement() - extract binary data from an ASCII element
 *-----------------------------------------------------------------------*/
unsigned                          /* returns error code */
OSPPMsgBinFromASCIIElement(
    unsigned char   *ospvElem,      /* input is ASCII element */
    unsigned      *ospvDataLen,   /* in: max size  out: actual size */
    unsigned char **ospvData      /* where to put binary data */
)

{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned outlen = 0;
    unsigned char *encodeddata = OSPC_OSNULL;

    if (ospvElem    == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_ASCII_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* as long as there's no error, we know the encoding to use */
        encodeddata = (unsigned char *)ospvElem;

        /* Allocate memory 3/4 that of the encoded data for the decoded data.
         * The reason for the 3/4 or 0.75, is due to the way base64 encodes
         * data. The result is the unencoded data is 3/4 that of the data
         * when encoded.
         */
         outlen = (unsigned int)ceil((0.75)*OSPM_STRLEN((char *)encodeddata));
         OSPM_MALLOC(*ospvData, unsigned char, outlen + 1);
         OSPM_MEMSET(*ospvData, 0, outlen + 1);

         /* decode the base64 */
         ospvErrCode = OSPPBase64Decode((const char *)encodeddata,
                    OSPM_STRLEN((char *)encodeddata), *ospvData, &outlen);

         *ospvDataLen = outlen;
    }
    return(ospvErrCode);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgBinFromElement() - extract binary data from an element
 *-----------------------------------------------------------------------*/

unsigned                          /* returns error code */
OSPPMsgBinFromElement(
    OSPT_XML_ELEM   *ospvElem,      /* input is XML element */
    unsigned      *ospvDataLen,   /* in: max size  out: actual size */
    unsigned char **ospvData      /* where to put binary data */
)

{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR  *attr;
    unsigned      isBase64;
    unsigned      verifyDataLen = 0;

    if (ospvElem    == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* assume default CDATA encoding (not base64) */
        isBase64 = OSPC_FALSE;

        /* look for a type attribute that will identify the encoding */
        for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElem);
            (attr != (OSPT_XML_ATTR *)OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR);
            attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElem, attr))
        {
            if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_ENCODING)
            {
                /* we found an encoding attribute - is it base64 */
                if (OSPM_STRCMP(OSPPXMLAttrGetValue(attr), "base64") == 0)
                {
                    /* yes, it's base64 instead of CDATA */
                    isBase64 = OSPC_TRUE;
                    /* don't stop now, in case a later version supercedes */
                }
                else if (OSPM_STRCMP(OSPPXMLAttrGetValue(attr), "cdata") == 0)
                {
                    /* this says CDATA instead of base64 */
                    isBase64 = OSPC_FALSE;
                    /* again, keep looking in case a later version is there */
                }
                else
                {
                    /* if it's not CDATA or base64, we can't interpret it */
                    ospvErrCode = OSPC_ERR_XML_BADCID_ENC;
                }
            }
        }

        /* as long as there's no error, we know the encoding to use */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            if (isBase64)
            {
                unsigned outlen = 0;
                unsigned char *encodeddata = OSPC_OSNULL;

                encodeddata = (unsigned char *)OSPPXMLElemGetValue(ospvElem);

                /* Allocate memory 3/4 that of the encoded data for the decoded data.
                 * The reason for the 3/4 or 0.75, is due to the way base64 encodes
                 * data. The result is the unencoded data is 3/4 that of the data
                 * when encoded.
                 */
                outlen = (unsigned int)ceil((0.75)*OSPM_STRLEN((char *)encodeddata));
                OSPM_MALLOC(*ospvData, unsigned char, outlen + 1);
                OSPM_MEMSET(*ospvData, 0, outlen + 1);

                /* decode the base64 */
                ospvErrCode = OSPPBase64Decode((const char *)encodeddata,
                    OSPM_STRLEN((char *)encodeddata), *ospvData, &outlen);

                *ospvDataLen = outlen;
            }
            else
            {
                /*
                 * The encoding must be CDATA. Note that the XML parsing
                 * will have already stripped out the "<[CDATA[" and "]]>"
                 * brackets but we still need to resolve any entity
                 * references in the data.
                 */

                ospvErrCode = OSPPXMLDereference((const unsigned char *)OSPPXMLElemGetValue(ospvElem),
                    ospvDataLen, OSPC_OSNULL);
                if(ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    /* Now that we know the size of the data, allocate memory for the data */
                    OSPM_MALLOC(*ospvData, unsigned char, *ospvDataLen + 1);

                    if(*ospvData != OSPC_OSNULL)
                    {
                    /* If there were no errors allocating the data, fill it with the data */
                        OSPM_MEMSET(*ospvData, 0, *ospvDataLen + 1);

                        /* Pass the value of the data size to verifyDataLen (see reason below): */
                        verifyDataLen = *ospvDataLen;

                        /* This time, fill in ospvData with the value and reverify the length
                         * by passing the length back to verifyDataLen (this is just a double
                         * check to make sure everything is fine). By giving a valid pointer
                         * in ospvData, and a value greater than zero to verifyDataLen, we are
                         * telling OSPPXMLDereference that we want the data--not just the length.
                         */
                        ospvErrCode = OSPPXMLDereference((const unsigned char *)OSPPXMLElemGetValue(ospvElem),
                             &verifyDataLen, *ospvData);

                        /* Verify that the lengths are what we expect */
                        if(*ospvDataLen != verifyDataLen)
                        {
                            /* If we get to this point, it means that something seriously went
                             * wrong. Somehow, different results were found when using 
                             * OSPPXMLDereference twice in a very small amount of time. The data
                             * is somehow corrupted. In order to avoid memory leaks, generate an
                             * error and free the allocated memory.
                             */

                            /* Generate an error! */
                            ospvErrCode = OSPC_ERR_XML_BADCID_ENC;

                            /* Free the pointer we recently created */
                            OSPM_FREE(*ospvData);
                        }
                    }
                }
                else
                {
                    /* Something went wrong when using OSPPXMLDereference. Send an error to
                     * indicate this.
                     */

                    ospvErrCode = OSPC_ERR_XML_BADCID_ENC;
                }
            }
        }

    }
    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgBinToElement() - create an XML element from binary data
 *-----------------------------------------------------------------------*/

unsigned                             /* returns error code */
OSPPMsgBinToElement(
    unsigned       ospvDataLen,      /* size of binary data */
    unsigned char *ospvData,         /* pointer to binary data */
    const unsigned char *ospvName,   /* name of element */
    OSPT_XML_ELEM **ospvElem,          /* where to put XML element pointer */
    OSPTBOOL      ospvUseBase64      /* base64 (1) or CDATA (0) encoding */
)
{
    unsigned      ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPTBFR      *bfr         = OSPC_OSNULL;

    if (ospvElem    == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvData    == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (ospvName    == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (ospvDataLen == 0) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

#ifdef OSPC_USE_CDATA_ONLY
    ospvUseBase64 = OSPC_FALSE;
#endif 

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        if (ospvUseBase64 == OSPC_FALSE)
        {
            /* allocate the buffer we use for the CDATA */

            /* Possible Optimization - 
             * Instead of allocating buffer for the below mentioned size,
             * allocate for 1 byte more.
             * This is because when we then have to finally put a '\0' char 
             * and we call WriteByte function, the WriteByte function wont have 
             * to allocate more memory. It will already have that space.
             */ 
            bfr = OSPPBfrNew(ospvDataLen + OSPC_XMLDOC_CDATABEGLEN +
                OSPC_XMLDOC_CDATAENDLEN);
            if (bfr == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_BUF_EMPTY;
            }

            /* start with the CDATA header */
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                if (OSPPBfrWriteBlock(&bfr, (void *)OSPC_XMLDOC_CDATABEG, OSPC_XMLDOC_CDATABEGLEN)
                    != OSPC_XMLDOC_CDATABEGLEN)
                {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
            }

            /* encode as CDATA */
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                ospvErrCode = OSPPXMLAddReference(ospvData, ospvDataLen, &bfr);
            }

            /* add the CDATA trailer */
            if (ospvErrCode == OSPC_ERR_NO_ERROR)
            {
                if (OSPPBfrWriteBlock(&bfr, (void *)OSPC_XMLDOC_CDATAEND, OSPC_XMLDOC_CDATAENDLEN)
                    != OSPC_XMLDOC_CDATAENDLEN)
                {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
            }

        }
        else
        {
            /* base64 encode it */
            unsigned       base64bufsz = ospvDataLen * 2  + 1;
            unsigned char *base64buf  = OSPC_OSNULL;
            
            /* base64 buffer must be at least 4 bytes long */
            if(base64bufsz < 4)
            {
                base64bufsz = 4;
            }

            OSPM_MALLOC(base64buf, unsigned char, base64bufsz);
            if (base64buf != OSPC_OSNULL)
            {
                ospvErrCode = OSPPBase64Encode(ospvData, ospvDataLen, base64buf, &base64bufsz);

#ifdef OSPC_VERIFY_BUILDTOKEN
                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {   
                    unsigned char tmpbuf[5000];
                    unsigned tmpbufsize = sizeof(tmpbuf);
                    ospvErrCode = OSPPBase64Decode((const char *)base64buf,
                        base64bufsz, tmpbuf, &tmpbufsize);
                    OSPTNLOGDUMP(ospvData, ospvDataLen, "DATA IN");
                    OSPTNLOGDUMP(tmpbuf, tmpbufsize, "DATA IN");

                    if (ospvErrCode == OSPC_ERR_NO_ERROR)
                    {
                        if (OSPM_MEMCMP(tmpbuf, ospvData, tmpbufsize) ||
                            (tmpbufsize != ospvDataLen))
                        {
                            ospvErrCode = 666;
                        }
                    }
                }
#endif 

                if (ospvErrCode == OSPC_ERR_NO_ERROR)
                {
                    /* Possible Optimization - 
                     * Instead of allocating buffer for the below mentioned size,
                     * allocate for 1 byte more.
                     * This is because when we then have to finally put a '\0' char 
                     * and we call WriteByte function, the WriteByte function wont have 
                     * to allocate more memory. It will already have that space.
                     */ 
                    bfr = OSPPBfrNew(base64bufsz);
                    if (bfr != OSPC_OSNULL)
                    {
                        if (OSPPBfrWriteBlock(&bfr, base64buf, base64bufsz) != base64bufsz)
                        {
                            ospvErrCode = OSPC_ERR_BUF_EMPTY;
                        }
                    } 
                    else
                    {
                        ospvErrCode = OSPC_ERR_BUF_EMPTY;
                    }
                }
                OSPM_FREE(base64buf);
            }
            else
            {
                ospvErrCode = OSPC_ERR_XML_MALLOC_FAILED;
            }
        }

        /* and finally the terminating 0 */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            if (OSPPBfrWriteByte(&bfr, '\0') != 1)
            {
                ospvErrCode = OSPC_ERR_BUF_EMPTY;
            }
        }

        /* create the element */
        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            *ospvElem = OSPPXMLElemNew((const char *)ospvName,(const char *)OSPPBfrLinearPtr(bfr));
            if (*ospvElem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
            else
            {
                if (ospvUseBase64 == OSPC_TRUE)
                {
                    OSPT_XML_ATTR *attr = OSPC_OSNULL;
                    attr = OSPPXMLAttrNew((const unsigned char *)"encoding", 
                        (const unsigned char *)"base64");
                    if (attr != OSPC_OSNULL) 
                    {
                        OSPPXMLElemAddAttr(*ospvElem, attr);
                        attr = OSPC_OSNULL;
                    }
                    else
                    {
                        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
                    }
                }
            }

        }

        /* destroy the buffer */
        if (bfr != OSPC_OSNULL)
        {
            OSPPBfrDelete(&bfr);
        }
    }
    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgNumFromElement() - extract number value from an element
 *-----------------------------------------------------------------------*/
unsigned                          /* returns error code */
OSPPMsgNumFromElement(
    OSPT_XML_ELEM   *ospvElem,      /* input is XML element */
    unsigned long *ospvNumber     /* where to put number */
)
{
    unsigned  ospvErrCode = OSPC_ERR_NO_ERROR;
    char     *cptr;

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvNumber == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* get the number - must be decimal */
        *ospvNumber = OSPM_STRTOUL(OSPPXMLElemGetValue(ospvElem), &cptr, 10);

        /* check for errors */
        if (cptr == ((char *)OSPPXMLElemGetValue(ospvElem)))
        {
            ospvErrCode = OSPC_ERR_DATA_BAD_NUMBER;
        }
    }
    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgNumToElement() - create an XML element from a number
 *-----------------------------------------------------------------------*/
unsigned                           /* returns error code */
OSPPMsgNumToElement(
    unsigned long  ospvNumber,     /* number to serve as data */
    const unsigned char *ospvName, /* name of element */
    OSPT_XML_ELEM **ospvElem         /* where to put XML element pointer */
)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    char     val[41];         /* 39 digits will accomodate 2^128 */ /*!!!PS added 1 */
    char    *cptr;

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /*
         * So we don't have to worry about the size of unsigned longs on
         * the system, we work backwards.
         */
        val[sizeof(val)-1]=0;   /* !!!PS Make sure it looks like a string */
        cptr = &val[sizeof(val)-2];
        do
        {
            *cptr-- = (char)('0' + (ospvNumber%10));
            ospvNumber = ospvNumber/10;
        }
        while ((ospvNumber != 0) && (cptr >= &val[0]));

        if (ospvNumber != 0)
        {
            /* error - we ran out of space before completing the number */
            ospvErrCode = OSPC_ERR_DATA_BAD_NUMBER;
        }
        else
        {
            /* create the element */
            *ospvElem = OSPPXMLElemNew((const char *)ospvName, &val[cptr-&val[0]+1]);
            if (*ospvElem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }
    return(ospvErrCode);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgFloatFromElement() - extract float value from an element
 *-----------------------------------------------------------------------*/
int                          /* returns error code */
OSPPMsgFloatFromElement(
    OSPT_XML_ELEM   *ospvElem,      /* input is XML element */
    float         *ospvNumber     /* where to put number */
)
{
    unsigned  ospvErrCode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvNumber == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* get the number  */
        *ospvNumber = (float)OSPM_ATOF(OSPPXMLElemGetValue(ospvElem));
    }
    return(ospvErrCode);
}
/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgFloatToElement() - create an XML element from a float
 * float 1.175494351 E - 38(min) 3.402823466 E + 38 (max)
 *-----------------------------------------------------------------------*/
int                                 /* returns error code */
OSPPMsgFloatToElement(
    float  ospvFloat,               /* number to serve as data */
    const unsigned char *ospvName,  /* name of element */
    OSPT_XML_ELEM **ospvElem          /* where to put XML element pointer */
)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    char     val[41];         /* 39 digits will accomodate 2^128 */ /*!!!PS added 1 */

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /* convert number */
        OSPM_SPRINTF(val, "%.4f", ospvFloat);

        /* create the element */
        *ospvElem = OSPPXMLElemNew((const char *)ospvName, val);
        if (*ospvElem == OSPC_OSNULL)
        {

            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    return(ospvErrCode);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgCodeFromElement() - extract status code from an element
 *-----------------------------------------------------------------------*/
unsigned                          /* returns error code */
OSPPMsgCodeFromElement(
    OSPT_XML_ELEM   *ospvElem,      /* input is XML element */
    unsigned long *ospvNumber     /* where to put number */
)
{
    return (OSPPMsgNumFromElement(ospvElem, ospvNumber));
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgCodeToElement() - create an XML element from status code
 *-----------------------------------------------------------------------*/
unsigned                           /* returns error code */
OSPPMsgCodeToElement(
    unsigned long  ospvNumber,     /* number to serve as data */
    const unsigned char *ospvName, /* name of element */
    OSPT_XML_ELEM **ospvElem         /* where to put XML element pointer */
)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    char     val[4];         /* 39 digits will accomodate 2^128 */ /*!!!PS added 1 */

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /* format the status code with leading zeroes */
        sprintf(val,"%03ld",ospvNumber);
        /* create the element */
        *ospvElem = OSPPXMLElemNew((const char *)ospvName, (const char *)val);
        if (*ospvElem == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }
    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgTXFromElement() - extract transaction ID value from an element
 *-----------------------------------------------------------------------*/
unsigned                          /* returns error code */
OSPPMsgTXFromElement(
    OSPT_XML_ELEM   *ospvElem,      /* input is XML element */
    OSPTTRXID     *ospvTX         /* where to put transaction ID */
)
{
    unsigned  ospvErrCode = OSPC_ERR_NO_ERROR;
    char     *cptr = OSPC_OSNULL;
    unsigned    pos;


    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvTX == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* check for errors */
        if ((cptr = (char *) OSPPXMLElemGetValue(ospvElem)) == OSPC_OSNULL)
        {
            ospvErrCode = OSPC_ERR_DATA_BAD_NUMBER;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* get the number - must be decimal */
        *ospvTX = 0;
        for (pos = 0; pos < OSPM_STRLEN(cptr); pos++)
        {
            *ospvTX *= 10;
            *ospvTX += cptr[pos]-'0';
        }

    }
    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgTXToElement() - create an XML element from a Transaction
 *-----------------------------------------------------------------------*/
unsigned                           /* returns error code */
OSPPMsgTXToElement(
    OSPTTRXID     ospvNumber,     /* number to serve as data */
    const unsigned char *ospvName, /* name of element */
    OSPT_XML_ELEM **ospvElem         /* where to put XML element pointer */
)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    char     val[41];         /* 39 digits will accomodate 2^128 */ /*!!!PS added 1 */
    char    *cptr;

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /*
         * So we don't have to worry about the size of unsigned longs on
         * the system, we work backwards.
         */
        val[sizeof(val)-1]=0;   /* !!!PS Make sure it looks like a string */
        cptr = &val[sizeof(val)-2];
        do
        {
            *cptr-- = (char)('0' + (ospvNumber%10));
            ospvNumber = ospvNumber/10;
        }
        while ((ospvNumber != 0) && (cptr >= &val[0]));

        if (ospvNumber != 0)
        {
            /* error - we ran out of space before completing the number */
            ospvErrCode = OSPC_ERR_DATA_BAD_NUMBER;
        }
        else
        {
            /* create the element */
            *ospvElem = OSPPXMLElemNew((const char *)ospvName, &val[cptr-&val[0]+1]);
            if (*ospvElem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }
    return(ospvErrCode);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgTimeFromElement() - extract time value from an element
 *-----------------------------------------------------------------------*/
unsigned                        /* returns error code */
OSPPMsgTimeFromElement(
    OSPT_XML_ELEM *ospvElem,      /* input is XML element */
    OSPTTIME    *ospvTime)      /* where to put time */
{
    unsigned  ospvErrCode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvTime == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        ospvErrCode = OSPPOSTimeStringToCal(OSPPXMLElemGetValue(ospvElem),
            ospvTime);
    }
    return(ospvErrCode);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgTimeToElement() - create an XML element from a time value
 *-----------------------------------------------------------------------*/
unsigned                              /* returns error code */
OSPPMsgTimeToElement(
    OSPTTIME             ospvTime,    /* number to serve as data */
    const unsigned char *ospvName,    /* name of element */
    OSPT_XML_ELEM        **ospvElem     /* where to put XML element pointer */
)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    char     tstr[OSPC_TIMESTRINGSIZE];

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        ospvErrCode = OSPPOSTimeCalToString(ospvTime, tstr);

        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            /* create the element */
            *ospvElem = OSPPXMLElemNew((const char *)ospvName, tstr);
            if (*ospvElem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }
    return(ospvErrCode);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgRoleToElement() - create an XML element from a role value
 *-----------------------------------------------------------------------*/
unsigned                              /* returns error code */
OSPPMsgRoleToElement(
    unsigned             ospvRole,    /* number to serve as data */
    const unsigned char *ospvName,    /* name of element */
    OSPT_XML_ELEM        **ospvElem     /* where to put XML element pointer */
)
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    char     rolestr[OSPC_SIZE_ROLESTR];

    OSPM_MEMSET(rolestr, 0, OSPC_SIZE_ROLESTR);

    if (ospvElem == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) 
    {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) 
    {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        ospvErrCode = OSPPOSRoleValToString(ospvRole, rolestr);

        if (ospvErrCode == OSPC_ERR_NO_ERROR)
        {
            /* create the element */
            *ospvElem = OSPPXMLElemNew((const char *)ospvName, rolestr);
            if (*ospvElem == OSPC_OSNULL)
            {
                ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }
    return(ospvErrCode);
}


/**/
/*-----------------------------------------------------------------------*
 * OSPPMsgElemIsCritical() - is an element critical
 *-----------------------------------------------------------------------*/
unsigned                          /* returns non-zero if critical */
OSPPMsgElemIsCritical(
    OSPT_XML_ELEM   *ospvElem       /* input is XML element */
)
{
    unsigned      ospvIsCritical = OSPC_TRUE;
    OSPT_XML_ATTR  *attr;

    if (ospvElem != OSPC_OSNULL) 
    {
        /* look for a critical attribute */
        for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElem);
            attr != (OSPT_XML_ATTR *)OSPC_OSNULL;
            attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElem, attr))
        {
            if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_CRITICAL)
            {
                /* we found an critical attribute - is it false? */
                if (OSPM_STRCASECMP(OSPPXMLAttrGetValue(attr), "False") == 0)
                {
                    /* yes, change our return value */
                    ospvIsCritical = OSPC_FALSE;
                    /* don't stop now, in case a later version supercedes */
                }
            }
        }
    }

    return(ospvIsCritical);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPOSRoleValToString() - convert role value to string
 *-----------------------------------------------------------------------*/
int
OSPPOSRoleValToString(unsigned ospvRole, 
    char *ospvRolestr)
{
    int errorcode = OSPC_ERR_NO_ERROR;

    switch(ospvRole)
    {
        case OSPC_MROLE_SOURCE:
            memcpy(ospvRolestr, "source", strlen("source"));
            break;
        case OSPC_MROLE_DESTINATION:
            memcpy(ospvRolestr, "destination", strlen("destination"));
            break;
        case OSPC_MROLE_OTHER:
            memcpy(ospvRolestr, "other", strlen("other"));
            break;
        case OSPC_MROLE_RADSRCSTART:
            memcpy(ospvRolestr, "radsrcstart", strlen("radsrcstart"));
            break;
        case OSPC_MROLE_RADDSTSTART:
            memcpy(ospvRolestr, "raddststart", strlen("raddststart"));
            break;
        case OSPC_MROLE_RADSRCSTOP:
            memcpy(ospvRolestr, "radsrcstop", strlen("radsrcstop"));
            break;
        case OSPC_MROLE_RADDSTSTOP:
            memcpy(ospvRolestr, "raddststop", strlen("raddststop"));
            break;
        default:
            errorcode = OSPC_ERR_DATA_INVALID;
            break;
    }

    return errorcode;
}
