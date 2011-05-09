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

/* ospmsgutil.c - utility functions for OSP messages */

#include "osp/osp.h"
#include "osp/ospostime.h"
#include "osp/osperrno.h"
#include "osp/ospbfr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospxmlattr.h"
#include "osp/ospmsgattr.h"
#include "osp/ospaltinfo.h"
#include "osp/ospxmldoc.h"
#include "osp/ospmsg.h"
#include "osp/ospb64.h"
#include "osp/osptnlog.h"

/*
 * OSPPMsgBinFromASCIIElement() - extract binary data from an ASCII element
 */
unsigned OSPPMsgBinFromASCIIElement(    /* returns error code */
    unsigned char *ospvElem,            /* input is ASCII element */
    unsigned *ospvDataLen,              /* in: max size  out: actual size */
    unsigned char **ospvData)           /* where to put binary data */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    unsigned outlen = 0;
    unsigned char *encodeddata = OSPC_OSNULL;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_ASCII_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* as long as there's no error, we know the encoding to use */
        encodeddata = ospvElem;

        /*
         * Allocate memory 3/4 that of the encoded data for the decoded data.
         * The reason for the 3/4 or 0.75, is due to the way base64 encodes
         * data. The result is the unencoded data is 3/4 that of the data
         * when encoded.
         */
        outlen = (unsigned)ceil((0.75) * OSPM_STRLEN((char *)encodeddata));
        OSPM_MALLOC(*ospvData, unsigned char, outlen + 1);
        OSPM_MEMSET(*ospvData, 0, outlen + 1);

        /* decode the base64 */
        errcode = OSPPBase64Decode((const char *)encodeddata, OSPM_STRLEN((char *)encodeddata), *ospvData, &outlen);

        *ospvDataLen = outlen;
    }

    return errcode;
}

/*
 * OSPPMsgBinFromElement() - extract binary data from an element
 */
unsigned OSPPMsgBinFromElement( /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    unsigned *ospvDataLen,      /* in: max size  out: actual size */
    unsigned char **ospvData)   /* where to put binary data */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ATTR *attr;
    unsigned isBase64;
    unsigned verifyDataLen = 0;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* assume default CDATA encoding (not base64) */
        isBase64 = OSPC_FALSE;

        /* look for a type attribute that will identify the encoding */
        for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElem);
            (attr != OSPC_OSNULL) && (errcode == OSPC_ERR_NO_ERROR);
            attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElem, attr))
        {
            if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_ENCODING) {
                /* we found an encoding attribute - is it base64 */
                if (OSPM_STRCMP(OSPPXMLAttrGetValue(attr), OSPPAltInfoTypeGetName(OSPC_ALTINFO_BASE64)) == 0) {
                    /* yes, it's base64 instead of CDATA */
                    isBase64 = OSPC_TRUE;
                    /* don't stop now, in case a later version supercedes */
                } else if (OSPM_STRCMP(OSPPXMLAttrGetValue(attr), OSPPAltInfoTypeGetName(OSPC_ALTINFO_CDATA)) == 0) {
                    /* this says CDATA instead of base64 */
                    isBase64 = OSPC_FALSE;
                    /* again, keep looking in case a later version is there */
                } else {
                    /* if it's not CDATA or base64, we can't interpret it */
                    errcode = OSPC_ERR_XML_BADCID_ENC;
                }
            }
        }

        /* as long as there's no error, we know the encoding to use */
        if (errcode == OSPC_ERR_NO_ERROR) {
            if (isBase64) {
                unsigned outlen = 0;
                unsigned char *encodeddata = OSPC_OSNULL;

                encodeddata = (unsigned char *)OSPPXMLElemGetValue(ospvElem);

                /*
                 * Allocate memory 3/4 that of the encoded data for the decoded data.
                 * The reason for the 3/4 or 0.75, is due to the way base64 encodes
                 * data. The result is the unencoded data is 3/4 that of the data
                 * when encoded.
                 */
                outlen = (unsigned int)ceil((0.75) * OSPM_STRLEN((char *)encodeddata));

                OSPM_MALLOC(*ospvData, unsigned char, outlen + 1);
                OSPM_MEMSET(*ospvData, 0, outlen + 1);

                /* decode the base64 */
                errcode = OSPPBase64Decode((const char *)encodeddata, OSPM_STRLEN((char *)encodeddata), *ospvData, &outlen);

                *ospvDataLen = outlen;
            } else {
                /*
                 * The encoding must be CDATA. Note that the XML parsing
                 * will have already stripped out the "<[CDATA[" and "]]>"
                 * brackets but we still need to resolve any entity
                 * references in the data.
                 */

                errcode = OSPPXMLDereference((const unsigned char *)OSPPXMLElemGetValue(ospvElem), ospvDataLen, OSPC_OSNULL);
                if (errcode == OSPC_ERR_NO_ERROR) {
                    /* Now that we know the size of the data, allocate memory for the data */
                    OSPM_MALLOC(*ospvData, unsigned char, *ospvDataLen + 1);

                    if (*ospvData != OSPC_OSNULL) {
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
                        errcode = OSPPXMLDereference((const unsigned char *)OSPPXMLElemGetValue(ospvElem), &verifyDataLen, *ospvData);

                        /* Verify that the lengths are what we expect */
                        if (*ospvDataLen != verifyDataLen) {
                            /*
                             * If we get to this point, it means that something seriously went
                             * wrong. Somehow, different results were found when using
                             * OSPPXMLDereference twice in a very small amount of time. The data
                             * is somehow corrupted. In order to avoid memory leaks, generate an
                             * error and free the allocated memory.
                             */

                            /* Generate an error! */
                            errcode = OSPC_ERR_XML_BADCID_ENC;

                            /* Free the pointer we recently created */
                            OSPM_FREE(*ospvData);
                        }
                    }
                } else {
                    /*
                     * Something went wrong when using OSPPXMLDereference. Send an error to
                     * indicate this.
                     */

                    errcode = OSPC_ERR_XML_BADCID_ENC;
                }
            }
        }
    }

    return errcode;
}

/*
 * OSPPMsgBinToElement() - create an XML element from binary data
 */
unsigned OSPPMsgBinToElement(       /* returns error code */
    const char *ospvName,           /* name of element */
    unsigned ospvDataLen,           /* size of binary data */
    unsigned char *ospvData,        /* pointer to binary data */
    const char *ospvAttrType,       /* Attribute type */
    const char *ospvAttrValue,      /* Attribute value */
    OSPTBOOL ospvUseBase64,         /* base64 (1) or CDATA (0) encoding */
    OSPT_XML_ELEM **ospvElem)       /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    OSPTBFR *bfr = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if ((ospvName == OSPC_OSNULL) ||
        ((ospvDataLen == 0) || (ospvData == OSPC_OSNULL)) ||
        (((ospvAttrType != OSPC_OSNULL) && (ospvAttrType[0] != '\0')) && ((ospvAttrValue == OSPC_OSNULL) || (ospvAttrValue[0] == '\0'))))
    {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    } else if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }

#ifdef OSPC_USE_CDATA_ONLY
    ospvUseBase64 = OSPC_FALSE;
#endif

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        if (ospvUseBase64 == OSPC_FALSE) {
            /* allocate the buffer we use for the CDATA */

            /*
             * Possible Optimization -
             * Instead of allocating buffer for the below mentioned size,
             * allocate for 1 byte more.
             * This is because when we then have to finally put a '\0' char
             * and we call WriteByte function, the WriteByte function wont have
             * to allocate more memory. It will already have that space.
             */
            bfr = OSPPBfrNew(ospvDataLen + OSPC_XMLDOC_CDATABEGLEN + OSPC_XMLDOC_CDATAENDLEN);
            if (bfr == OSPC_OSNULL) {
                errcode = OSPC_ERR_BUF_EMPTY;
            }

            /* start with the CDATA header */
            if (errcode == OSPC_ERR_NO_ERROR) {
                if (OSPPBfrWriteBlock(&bfr, (void *)OSPC_XMLDOC_CDATABEG, OSPC_XMLDOC_CDATABEGLEN) != OSPC_XMLDOC_CDATABEGLEN) {
                    errcode = OSPC_ERR_BUF_EMPTY;
                }
            }

            /* encode as CDATA */
            if (errcode == OSPC_ERR_NO_ERROR) {
                errcode = OSPPXMLAddReference(ospvData, ospvDataLen, &bfr);
            }

            /* add the CDATA trailer */
            if (errcode == OSPC_ERR_NO_ERROR) {
                if (OSPPBfrWriteBlock(&bfr, (void *)OSPC_XMLDOC_CDATAEND, OSPC_XMLDOC_CDATAENDLEN) != OSPC_XMLDOC_CDATAENDLEN) {
                    errcode = OSPC_ERR_BUF_EMPTY;
                }
            }
        } else {
            /* base64 encode it */
            unsigned base64bufsz = ospvDataLen * 2 + 1;
            unsigned char *base64buf = OSPC_OSNULL;

            /* base64 buffer must be at least 4 bytes long */
            if (base64bufsz < 4) {
                base64bufsz = 4;
            }

            OSPM_MALLOC(base64buf, unsigned char, base64bufsz);
            if (base64buf != OSPC_OSNULL) {
                errcode = OSPPBase64Encode(ospvData, ospvDataLen, base64buf, &base64bufsz);

#ifdef OSPC_VERIFY_BUILDTOKEN
                if (errcode == OSPC_ERR_NO_ERROR) {
                    unsigned char tmpbuf[5000];
                    unsigned tmpbufsize = sizeof(tmpbuf);
                    errcode = OSPPBase64Decode((const char *)base64buf, base64bufsz, tmpbuf, &tmpbufsize);
                    OSPTNLOGDUMP(ospvData, ospvDataLen, "DATA IN");
                    OSPTNLOGDUMP(tmpbuf, tmpbufsize, "DATA IN");

                    if (errcode == OSPC_ERR_NO_ERROR) {
                        if (OSPM_MEMCMP(tmpbuf, ospvData, tmpbufsize) || (tmpbufsize != ospvDataLen)) {
                            errcode = 666;
                        }
                    }
                }
#endif

                if (errcode == OSPC_ERR_NO_ERROR) {
                    /*
                     * Possible Optimization -
                     * Instead of allocating buffer for the below mentioned size,
                     * allocate for 1 byte more.
                     * This is because when we then have to finally put a '\0' char
                     * and we call WriteByte function, the WriteByte function wont have
                     * to allocate more memory. It will already have that space.
                     */
                    bfr = OSPPBfrNew(base64bufsz);
                    if (bfr != OSPC_OSNULL) {
                        if (OSPPBfrWriteBlock(&bfr, base64buf, base64bufsz) != base64bufsz) {
                            errcode = OSPC_ERR_BUF_EMPTY;
                        }
                    } else {
                        errcode = OSPC_ERR_BUF_EMPTY;
                    }
                }
                OSPM_FREE(base64buf);
            } else {
                errcode = OSPC_ERR_XML_MALLOC_FAILED;
            }
        }

        /* and finally the terminating 0 */
        if (errcode == OSPC_ERR_NO_ERROR) {
            if (OSPPBfrWriteByte(&bfr, '\0') != 1) {
                errcode = OSPC_ERR_BUF_EMPTY;
            }
        }

        /* create the element */
        if (errcode == OSPC_ERR_NO_ERROR) {
            *ospvElem = OSPPXMLElemNew(ospvName, (const char *)OSPPBfrLinearPtr(bfr));
            if (*ospvElem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            } else {
                if ((ospvAttrType != OSPC_OSNULL) && (ospvAttrType[0] != '\0')) {
                    attr = OSPPXMLAttrNew(ospvAttrType, ospvAttrValue);
                    if (attr == OSPC_OSNULL) {
                        OSPPXMLElemDelete(ospvElem);
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(*ospvElem, attr);
                    }
                }
                if ((errcode == OSPC_ERR_NO_ERROR) && (ospvUseBase64 == OSPC_TRUE)) {
                    attr = OSPPXMLAttrNew(OSPPMsgAttrGetName(OSPC_MATTR_ENCODING), OSPPAltInfoTypeGetName(OSPC_ALTINFO_BASE64));
                    if (attr == OSPC_OSNULL) {
                        OSPPXMLElemDelete(ospvElem);
                        errcode = OSPC_ERR_XML_NO_ATTR;
                    } else {
                        OSPPXMLElemAddAttr(*ospvElem, attr);
                   }
                }
            }
        }

        /* destroy the buffer */
        if (bfr != OSPC_OSNULL) {
            OSPPBfrDelete(&bfr);
        }
    }

    return errcode;
}

/*
 * OSPPMsgNumFromElement() - extract number value from an element
 */
unsigned OSPPMsgNumFromElement( /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    unsigned long *ospvNumber)  /* where to put number */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char *cptr;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvNumber == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* get the number - must be decimal */
        *ospvNumber = OSPM_STRTOUL(OSPPXMLElemGetValue(ospvElem), &cptr, 10);

        /* check for errors */
        if (cptr == OSPPXMLElemGetValue(ospvElem)) {
            errcode = OSPC_ERR_DATA_BAD_NUMBER;
        }
    }

    return errcode;
}

/*
 * OSPPMsgNumToElement() - create an XML element from a number
 */
unsigned OSPPMsgNumToElement(   /* returns error code */
    unsigned long ospvNumber,   /* number to serve as data */
    const char *ospvName,       /* name of element */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char val[41];   /* 39 digits will accomodate 2^128 */   /*!!!PS added 1 */
    char *cptr;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /*
         * So we don't have to worry about the size of unsigned longs on
         * the system, we work backwards.
         */
        val[sizeof(val) - 1] = 0;   /* !!!PS Make sure it looks like a string */
        cptr = &val[sizeof(val) - 2];
        do {
            *cptr-- = (char)('0' + (ospvNumber % 10));
            ospvNumber = ospvNumber / 10;
        } while ((ospvNumber != 0) && (cptr >= &val[0]));

        if (ospvNumber != 0) {
            /* error - we ran out of space before completing the number */
            errcode = OSPC_ERR_DATA_BAD_NUMBER;
        } else {
            /* create the element */
            *ospvElem = OSPPXMLElemNew(ospvName, &val[cptr - &val[0] + 1]);
            if (*ospvElem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }

    return errcode;
}

/*
 * OSPPMsgFloatFromElement() - extract float value from an element
 */
int OSPPMsgFloatFromElement(    /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    float *ospvNumber)          /* where to put number */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvNumber == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* get the number  */
        *ospvNumber = (float)OSPM_ATOF(OSPPXMLElemGetValue(ospvElem));
    }

    return errcode;
}

/*
 * OSPPMsgFloatToElement() - create an XML element from a float
 * float 1.175494351 E - 38(min) 3.402823466 E + 38 (max)
 */
int OSPPMsgFloatToElement(      /* returns error code */
    float ospvFloat,            /* number to serve as data */
    const char *ospvName,       /* name of element */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char val[41];               /* 39 digits will accomodate 2^128 *//*!!!PS added 1 */

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /* convert number */
        OSPM_SPRINTF(val, "%.4f", ospvFloat);

        /* create the element */
        *ospvElem = OSPPXMLElemNew(ospvName, val);
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    return errcode;
}

/*
 * OSPPMsgCodeFromElement() - extract status code from an element
 */
unsigned OSPPMsgCodeFromElement(/* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    unsigned long *ospvNumber)  /* where to put number */
{
    return OSPPMsgNumFromElement(ospvElem, ospvNumber);
}

/*
 * OSPPMsgCodeToElement() - create an XML element from status code
 */
unsigned OSPPMsgCodeToElement(  /* returns error code */
    unsigned long ospvNumber,   /* number to serve as data */
    const char *ospvName,       /* name of element */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char val[4];                /* 39 digits will accomodate 2^128 *//*!!!PS added 1 */

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /* format the status code with leading zeroes */
        sprintf(val, "%03ld", ospvNumber);
        /* create the element */
        *ospvElem = OSPPXMLElemNew(ospvName, (const char *)val);
        if (*ospvElem == OSPC_OSNULL) {
            errcode = OSPC_ERR_XML_NO_ELEMENT;
        }
    }

    return errcode;
}

/*
 * OSPPMsgTXFromElement() - extract transaction ID value from an element
 */
unsigned OSPPMsgTXFromElement(  /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    OSPTTRXID *ospvTX)          /* where to put transaction ID */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char *cptr = OSPC_OSNULL;
    unsigned pos;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvTX == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* check for errors */
        if ((cptr = (char *)OSPPXMLElemGetValue(ospvElem)) == OSPC_OSNULL) {
            errcode = OSPC_ERR_DATA_BAD_NUMBER;
        }
    }
    if (errcode == OSPC_ERR_NO_ERROR) {
        /* get the number - must be decimal */
        *ospvTX = 0;
        for (pos = 0; pos < OSPM_STRLEN(cptr); pos++) {
            *ospvTX *= 10;
            *ospvTX += cptr[pos] - '0';
        }
    }

    return errcode;
}

/*
 * OSPPMsgTXToElement() - create an XML element from a Transaction
 */
unsigned OSPPMsgTXToElement(    /* returns error code */
    OSPTTRXID ospvNumber,       /* number to serve as data */
    const char *ospvName,       /* name of element */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char val[41];               /* 39 digits will accomodate 2^128 *//*!!!PS added 1 */
    char *cptr;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        /*
         * So we don't have to worry about the size of unsigned longs on
         * the system, we work backwards.
         */
        val[sizeof(val) - 1] = 0;   /* !!!PS Make sure it looks like a string */
        cptr = &val[sizeof(val) - 2];
        do {
            *cptr-- = (char)('0' + (ospvNumber % 10));
            ospvNumber = ospvNumber / 10;
        } while ((ospvNumber != 0) && (cptr >= &val[0]));

        if (ospvNumber != 0) {
            /* error - we ran out of space before completing the number */
            errcode = OSPC_ERR_DATA_BAD_NUMBER;
        } else {
            /* create the element */
            *ospvElem = OSPPXMLElemNew(ospvName, &val[cptr - &val[0] + 1]);
            if (*ospvElem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }

    return errcode;
}

/*
 * OSPPMsgTimeFromElement() - extract time value from an element
 */
unsigned OSPPMsgTimeFromElement(/* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* input is XML element */
    OSPTTIME *ospvTime)         /* where to put time */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvTime == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        errcode = OSPPOSTimeStringToCal(OSPPXMLElemGetValue(ospvElem), ospvTime);
    }

    return errcode;
}

/*
 * OSPPMsgTimeToElement() - create an XML element from a time value
 */
unsigned OSPPMsgTimeToElement(  /* returns error code */
    OSPTTIME ospvTime,          /* number to serve as data */
    const char *ospvName,       /* name of element */
    OSPT_XML_ELEM **ospvElem)   /* where to put XML element pointer */
{
    unsigned errcode = OSPC_ERR_NO_ERROR;
    char tstr[OSPC_SIZE_TIMESTRING];

    if (ospvElem == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvName == OSPC_OSNULL) {
        errcode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (errcode == OSPC_ERR_NO_ERROR) {
        /* start with no element */
        *ospvElem = OSPC_OSNULL;

        errcode = OSPPOSTimeCalToString(ospvTime, tstr);

        if (errcode == OSPC_ERR_NO_ERROR) {
            /* create the element */
            *ospvElem = OSPPXMLElemNew(ospvName, tstr);
            if (*ospvElem == OSPC_OSNULL) {
                errcode = OSPC_ERR_XML_NO_ELEMENT;
            }
        }
    }

    return errcode;
}

/*
 * OSPPMsgElemIsCritical() - is an element critical
 */
unsigned OSPPMsgElemIsCritical( /* returns non-zero if critical */
    OSPT_XML_ELEM *ospvElem)    /* input is XML element */
{
    unsigned ospvIsCritical = OSPC_TRUE;
    OSPT_XML_ATTR *attr;

    if (ospvElem != OSPC_OSNULL) {
        /* look for a critical attribute */
        for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElem);
            attr != OSPC_OSNULL;
            attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElem, attr))
        {
            if (OSPPMsgAttrGetPart(OSPPXMLAttrGetName(attr)) == OSPC_MATTR_CRITICAL) {
                /* we found an critical attribute - is it false? */
                if (OSPM_STRCASECMP(OSPPXMLAttrGetValue(attr), OSPPAltInfoTypeGetName(OSPC_ALTINFO_FALSE)) == 0) {
                    /* yes, change our return value */
                    ospvIsCritical = OSPC_FALSE;
                    /* don't stop now, in case a later version supercedes */
                }
            }
        }
    }

    return ospvIsCritical;
}

/*
 * OSPPRoleGetName() - get an role name from a part value
 */
const char *OSPPRoleGetName(    /* returns pointer to the name */
    OSPE_ROLE ospvPart)
{
    const char *name = OSPC_OSNULL;

    if ((ospvPart >= OSPC_ROLE_START) && (ospvPart < OSPC_ROLE_NUMBER)) {
        name = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_RTYPE_DESCS, OSPC_ROLE_NUMBER);
    }

    return name;
}

/*
 * OSPPRoleGetPart() - get a role value part ID from its name
 */
OSPE_ROLE OSPPRoleGetPart(  /* returns part */
    const char *ospvName)
{
    OSPE_ROLE role = OSPC_ROLE_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        role = (OSPE_ROLE)OSPPMsgDescGetPart(ospvName, OSPV_RTYPE_DESCS, OSPC_ROLE_NUMBER);
    }

    return role;
}

/*
 * OSPPRoleStateGetName() - get an role state from a part value
 */
const char *OSPPRoleStateGetName(   /* returns pointer to the name */
    OSPE_ROLE_STATE ospvPart)
{
    const char *state = OSPC_OSNULL;

    if ((ospvPart >= OSPC_RSTATE_START) && (ospvPart < OSPC_RSTATE_NUMBER)) {
        state = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_RSTATE_DESCS, OSPC_RSTATE_NUMBER);
    }

    return state;
}

/*
 * OSPPRoleStateGetPart() - get a role state value part ID from its name
 */
OSPE_ROLE_STATE OSPPRoleStateGetPart(   /* returns part */
    const char *ospvName)
{
    OSPE_ROLE_STATE state = OSPC_RSTATE_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        state = (OSPE_ROLE_STATE)OSPPMsgDescGetPart(ospvName, OSPV_RSTATE_DESCS, OSPC_RSTATE_NUMBER);
    }

    return state;
}

/*
 * OSPPRoleFormatGetName() - get an role format from a part value
 */
const char *OSPPRoleFormatGetName(  /* returns pointer to the name */
    OSPE_ROLE_FORMAT ospvPart)
{
    const char *format = OSPC_OSNULL;

    if ((ospvPart >= OSPC_RFORMAT_START) && (ospvPart < OSPC_RFORMAT_NUMBER)) {
        format = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_RFORMAT_DESCS, OSPC_RFORMAT_NUMBER);
    }

    return format;
}

/*
 * OSPPRoleFormatGetPart() - get a role format value part ID from its name
 */
OSPE_ROLE_FORMAT OSPPRoleFormatGetPart( /* returns part */
    const char *ospvName)
{
    OSPE_ROLE_FORMAT format = OSPC_RFORMAT_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        format = (OSPE_ROLE_FORMAT)OSPPMsgDescGetPart(ospvName, OSPV_RFORMAT_DESCS, OSPC_RFORMAT_NUMBER);
    }

    return format;
}

/*
 * OSPPRoleVendorGetName() - get an role vendor from a part value
 */
const char *OSPPRoleVendorGetName(  /* returns pointer to the name */
    OSPE_ROLE_VENDOR ospvPart)
{
    const char *vendor = OSPC_OSNULL;

    if ((ospvPart >= OSPC_RVENDOR_START) && (ospvPart < OSPC_RVENDOR_NUMBER)) {
        vendor = OSPPMsgDescGetName((OSPT_MSG_PART)ospvPart, OSPV_RVENDOR_DESCS, OSPC_RVENDOR_NUMBER);
    }

    return vendor;
}

/*
 * OSPPRoleVendorGetPart() - get a role vendor value part ID from its name
 */
OSPE_ROLE_VENDOR OSPPRoleVendorGetPart( /* returns part */
    const char *ospvName)
{
    OSPE_ROLE_VENDOR vendor = OSPC_RVENDOR_UNKNOWN;

    if (ospvName != OSPC_OSNULL) {
        vendor = (OSPE_ROLE_VENDOR)OSPPMsgDescGetPart(ospvName, OSPV_RVENDOR_DESCS, OSPC_RVENDOR_NUMBER);
    }

    return vendor;
}

