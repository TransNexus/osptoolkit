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

/* ospxmlparse.c - Generic XML document parsing functions. */

#include "osp/osp.h"
#include "osp/ospbfr.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/osperrno.h"
#include "osp/ospxmltype.h"
#include "osp/ospxmldoc.h"
#include "osp/ospmsgelem.h"

/*
 * OSPPXMLDocParse() - parse a document into its elements and attributes
 */
unsigned OSPPXMLDocParse(           /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPT_XML_ELEM **ospvElemAddr)   /* where to put parsed element */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPE_XML_ENC encoding = OSPC_XENC_UNKNOW;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvElemAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* Before doing anything else, determine the encoding in use */
        ospvErrCode = OSPPXMLDocGetEncoding(ospvBfrAddr, &encoding);
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (encoding == OSPC_XENC_UNKNOW) {
            /*
             * If we couldn't recognize the encoding, then punt by
             * indicating an error.
             */
            ospvErrCode = OSPC_ERR_XML_BAD_ENC;
        }
    }
    /*
     * Now skip over the prolog; for now, we don't do any processing
     * of information in the prolog (e.g. document type definitions).
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocSkipProlog(ospvBfrAddr, encoding);
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * Now that we've gotten past the prolog, we can start parsing the
         * document.
         */

        ospvErrCode = OSPPXMLDocParseElem(ospvBfrAddr, encoding, ospvElemAddr);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocParseElem() - parse a document element
 */
unsigned OSPPXMLDocParseElem(       /* returns error code */
    OSPTBFR ** ospvBfrAddr,         /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,      /* character encoding for the document */
    OSPT_XML_ELEM ** ospvElemAddr)  /* where to put parsed element */
{
    /*
     * CAUTION: This function is called recursively; be sure to account for that in any modifications
     */

    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    OSPTBFR *nameBfr = OSPC_OSNULL;
    OSPTBFR *valBfr = OSPC_OSNULL;
    OSPTLIST attrList = OSPC_OSNULL;
    OSPTLIST childList = OSPC_OSNULL;
    unsigned char readChar = '\0';

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvElemAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* set up some of our local variables */
        nameBfr = OSPPBfrNew(OSPC_XMLDOC_NAMESIZE);
        valBfr = OSPPBfrNew(OSPC_XMLDOC_VALUESIZE);
        OSPPListNew(&attrList);
        OSPPListNew(&childList);

        /* make sure the initializations went okay */
        if ((nameBfr == OSPC_OSNULL) || (valBfr == OSPC_OSNULL)) {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        }
    }
    /* to start out, skip until we've found the opening tag */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocSkipPastChar(ospvBfrAddr, ospvEncoding, OSPC_XMLDOC_OPEN);
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * We've found the start of the name, so extract it. Note
         * that we use a message buffer to store the name. We do
         * that because message buffers serve as a convenient
         * "elastic" buffer and we don't have to know how big the
         * name is in advance.
         */

        ospvErrCode = OSPPXMLDocGetName(ospvBfrAddr, ospvEncoding, &nameBfr);
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * The next thing to get is attributes (if there are any).
         * We'll collect those in a local list for now, and then
         * add them to the real element when we've got all the
         * information for it.
         */
        ospvErrCode = OSPPXMLDocGetAttrs(ospvBfrAddr, ospvEncoding, &attrList);
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * Skip past any trailing whitespace to get to the end of the
         * element tag.
         */
        ospvErrCode = OSPPXMLDocSkipWhite(ospvBfrAddr, ospvEncoding);
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * Look at the next character to see if we've got an empty
         * element or not.
         */
        ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (readChar == OSPC_XMLDOC_TRAILER) {
            /*
             * This is an empty element, which means we're pretty much
             * done. The next character should be the closing tag. Grab
             * it to make sure and then we're done.
             *
             * !!TWK 07/26/98
             * had to add a fix here to handle having actual attributes
             * for the empty element.
             */
            ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
            if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
                (readChar != OSPC_XMLDOC_CLOSE)) {
                /* !!TWK -- here is the actual fix....
                 * What I'm assuming is that since this is an empty element
                 * that there will no children
                 *
                 * now I go and grab the attributes for this tag
                 */
                ospvErrCode = OSPPXMLDocGetAttrs(ospvBfrAddr, ospvEncoding, &attrList);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    ospvErrCode = OSPPXMLDocSkipPastChar(ospvBfrAddr, ospvEncoding, OSPC_XMLDOC_CLOSE);
                }
                /* ospvErrCode = OSPC_ERR_XML_BAD_TAG; */
            }
        } else if (readChar == OSPC_XMLDOC_CLOSE) {
            /*
             * We have a non-empty element with real content. Get the
             * content (child elements and/or character data) now.
             */

            ospvErrCode = OSPPXMLDocGetContent(ospvBfrAddr, ospvEncoding, &childList, &valBfr);
            /*
             * The content is collected, skip past the closing bracket of
             * the element's closing tag. Note that we're not verifying that
             * the closing tag name is the same as the opening tag name.
             * That's obviously required for strict XML well-formedness,
             * but we're being a bit more liberal.
             */

            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                ospvErrCode = OSPPXMLDocSkipPastChar(ospvBfrAddr, ospvEncoding, OSPC_XMLDOC_CLOSE);
            }
        } else {
            /*
             * Oops! We got to the end of the element the
             * first non-whitespace character wasn't a trailing slash
             * or a closing tag. That's an XML error.
             */
            ospvErrCode = OSPC_ERR_XML_BAD_TAG;
        }
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * Now that we've got all the pieces, put them together into
         * the element.
         */
        elem = OSPPXMLElemNew((const char *)OSPPBfrLinearPtr(nameBfr), (const char *)OSPPBfrLinearPtr(valBfr));
        if (elem != OSPC_OSNULL) {
            OSPPListMove(&elem->ospmXMLElemAttrs, &attrList);
            OSPPListMove(&elem->ospmXMLElemChild, &childList);
        }
        *ospvElemAddr = elem;
    }

    /* free the buffers we allocated */
    if (nameBfr != OSPC_OSNULL) {
        OSPPBfrDelete(&nameBfr);
    }
    if (valBfr != OSPC_OSNULL) {
        OSPPBfrDelete(&valBfr);
    }

    /* and delete any lists that are still hanging around */
    for (elem = (OSPT_XML_ELEM *)OSPPListRemove(&childList);
         elem != OSPC_OSNULL;
         elem = (OSPT_XML_ELEM *)OSPPListRemove(&childList))
    {
        OSPPXMLElemDelete(&elem);
    }
    for (attr = (OSPT_XML_ATTR *)OSPPListRemove(&attrList);
         attr != OSPC_OSNULL;
         attr = (OSPT_XML_ATTR *)OSPPListRemove(&attrList))
    {
        OSPPXMLAttrDelete(&attr);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocGetAttrs() - get the attributes for an element
 */
unsigned OSPPXMLDocGetAttrs(    /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    OSPTLIST *ospvAttrs)        /* list for attributes */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar = '\0';
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvAttrs == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
    }

    /* go into a loop looking for attributes */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* skip whitespace */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocSkipWhite(ospvBfrAddr, ospvEncoding);
        }

        /* take a peek at the next character to see if it's a name */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLDocPeekCharN(ospvBfrAddr, ospvEncoding, 0, &readChar, (int *)&ospvErrCode);
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (!OSPPXMLIsName1(readChar)) {
                /*
                 * Indeed, the next character isn't a valid name
                 * beginning, so we're done.
                 */
                break;
            }
            /* we've got a valid name - get the whole attribute */
            ospvErrCode = OSPPXMLDocGetAttr(ospvBfrAddr, ospvEncoding, &attr);
            /* if successful, add it to the list */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPListAppend(ospvAttrs, attr);
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocGetAttr() - get a single attribute
 */
unsigned OSPPXMLDocGetAttr(         /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,      /* character encoding for the document */
    OSPT_XML_ATTR **ospvAttrAddr)   /* list for attributes */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar = '\0';
    unsigned char quoteChar = '\0';
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    OSPTBFR *nameBfr = OSPC_OSNULL;
    OSPTBFR *valBfr = OSPC_OSNULL;
    unsigned cnt;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvAttrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* set up some of our local variables */
        nameBfr = OSPPBfrNew(OSPC_XMLDOC_NAMESIZE);
        valBfr = OSPPBfrNew(OSPC_XMLDOC_VALUESIZE);

        /* make sure the initializations went okay */
        if ((nameBfr == OSPC_OSNULL) || (valBfr == OSPC_OSNULL)) {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        }
    }
    /* get the name of the attribute */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocGetName(ospvBfrAddr, ospvEncoding, &nameBfr);
    }

    /* the next character should be an equals sign */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
        (readChar != OSPC_XMLDOC_EQUALS)) {
        ospvErrCode = OSPC_ERR_XML_BAD_ATTR;
    }

    /* the next character is either a single or double quote mark */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &quoteChar);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) &&
        (quoteChar != OSPC_XMLDOC_SINGLEQUOTE) &&
        (quoteChar != OSPC_XMLDOC_DOUBLEQUOTE))
    {
        ospvErrCode = OSPC_ERR_XML_BAD_ATTR;
    }

    /* now loop on the attribute value - terminate with matching quote */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (readChar == quoteChar) {
                /*
                 * the matching quote, so we're done. Before we leave,
                 * add the terminating NULL to the buffer.
                 */
                cnt = OSPPBfrWriteByte(&valBfr, '\0');
                if (cnt != 1) {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
                break;
            }

            /* the character's still a part of the value, add it */
            cnt = OSPPBfrWriteByte(&valBfr, readChar);
            if (cnt != 1) {
                ospvErrCode = OSPC_ERR_BUF_EMPTY;
            }
        }
    }

    /* create the attribute object */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        attr = OSPPXMLAttrNew((const char *)OSPPBfrLinearPtr(nameBfr), (const char *)OSPPBfrLinearPtr(valBfr));
        *ospvAttrAddr = attr;
    }

    /* free the buffers we allocated */
    if (nameBfr != OSPC_OSNULL) {
        OSPPBfrDelete(&nameBfr);
    }
    if (valBfr != OSPC_OSNULL) {
        OSPPBfrDelete(&valBfr);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocGetContent() - get the content an element
 */
unsigned OSPPXMLDocGetContent(  /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    OSPTLIST *ospvChild,        /* list for children */
    OSPTBFR **ospvValBfr)       /* place to store value */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char char1 = '\0';
    unsigned char char2 = '\0';
#ifdef COMPLETE_XML_PARSING
    unsigned isComment = OSPC_FALSE;
    unsigned isPI = OSPC_FALSE;
    unsigned isCdata = OSPC_FALSE;
#endif
    unsigned cnt;
    OSPT_XML_ELEM *elem = OSPC_OSNULL;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvChild == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_CHILD_NOT_FOUND;
    }
    if (ospvValBfr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (*ospvValBfr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    /* loop while parsing the content */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* first take care of any whitespace */
        ospvErrCode = OSPPXMLDocSkipWhite(ospvBfrAddr, ospvEncoding);

#ifdef COMPLETE_XML_PARSING
        /* see if there's a comment - if so, skip it */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsComment(ospvBfrAddr, ospvEncoding, &isComment);
        }
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isComment) {
            ospvErrCode = OSPPXMLDocSkipComment(ospvBfrAddr, ospvEncoding);
            continue;
        }

        /* see if there's a processing instruction - if so, skip it */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsPI(ospvBfrAddr, ospvEncoding, &isPI);
        }

        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isPI) {
            ospvErrCode = OSPPXMLDocSkipPI(ospvBfrAddr, ospvEncoding);
            continue;
        }

        /* how about a CDATA section? */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsCdata(ospvBfrAddr, ospvEncoding, &isCdata);
        }

        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isCdata) {
            ospvErrCode = OSPPXMLDocGetCdata(ospvBfrAddr, ospvEncoding, ospvValBfr);
            continue;
        }
#endif

        /* we need a look at the next two characters for several cases */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLDocPeekCharN(ospvBfrAddr, ospvEncoding, 0, &char1, (int *)&ospvErrCode);
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            OSPPXMLDocPeekCharN(ospvBfrAddr, ospvEncoding, 1, &char2, (int *)&ospvErrCode);
        }

        /* might this be a child element ? */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (char1 == OSPC_XMLDOC_OPEN) && (OSPPXMLIsName1(char2))) {
            elem = OSPC_OSNULL;
            ospvErrCode = OSPPXMLDocParseElem(ospvBfrAddr, ospvEncoding, &elem);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                OSPPListAppend(ospvChild, elem);
                /* OSPPXMLElemDelete(elem); */
            }
            continue;
        }

        /* is this the end of the element (i.e. the closing tag)? */
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && (char1 == OSPC_XMLDOC_OPEN) && (char2 == OSPC_XMLDOC_TRAILER)) {
            /*
             * This looks like the end of the element. We're not being
             * picky and making sure that the closing tag name matches
             * the opening tag.
             */
            break;
        }

        /* the only thing left is actual data for the element */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &char1);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                cnt = OSPPBfrWriteByte(ospvValBfr, char1);
                if (cnt != 1) {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
            }
        }
    }

    /* before we leave, NULL terminate the value string */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        cnt = OSPPBfrWriteByte(ospvValBfr, '\0');
        if (cnt != 1) {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocGetCdata() - get CDATA from an XML document
 */
unsigned OSPPXMLDocGetCdata(    /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    OSPTBFR **ospvValBfr)       /* place to store value */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar = '\0';
    unsigned isCdataEnd = OSPC_FALSE;
    unsigned cnt;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvValBfr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (*ospvValBfr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* skip past CDATA beginning */
        ospvErrCode = OSPPXMLDocSkipPastCdataBeg(ospvBfrAddr, ospvEncoding);
    }

    /* loop while looking for the end of the CDATA section */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* check to see if we're at the end of CDATA */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsCdataEnd(ospvBfrAddr, ospvEncoding, &isCdataEnd);
        }
        if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isCdataEnd) {
            /* we're at the end, skip past the CDATA end marker and break */
            ospvErrCode = OSPPXMLDocSkipPastCdataEnd(ospvBfrAddr, ospvEncoding);
            break;
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            /* there's more CDATA there, get the next character */
            ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                cnt = OSPPBfrWriteByte(ospvValBfr, readChar);
                if (cnt != 1) {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocGetName() - get the name from and XML document
 */
unsigned OSPPXMLDocGetName(     /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    OSPTBFR **ospvNameBfr)      /* buffer for storing name */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar = '\0';
    unsigned cnt;

    /*
     * The only thing particularly noteworthy in this function is that
     * we use a message buffer to store the extracted name. We do that
     * because message buffers are elastic, and by using them we don't
     * have to figure out how big the name is before we start extracting
     * it.
     */

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvNameBfr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }
    if (*ospvNameBfr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* the first character in a name has special restrictions */
        ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        if (!OSPPXMLIsName1(readChar)) {
            ospvErrCode = OSPC_ERR_XML_BAD_NAME;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        cnt = OSPPBfrWriteByte(ospvNameBfr, readChar);
        if (cnt != 1) {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        }
    }

    /* now we can fall into a loop gathering the remaining characters */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * First let's get a peek at the next character in the document.
         * We don't want to read it yet because it may not be part of
         * the name.
         */
        OSPPXMLDocPeekCharN(ospvBfrAddr, ospvEncoding, 0, &readChar, (int *)&ospvErrCode);
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (!OSPPXMLIsName(readChar)) {
                /*
                 * Indeed, the next character isn't a valid name
                 * character, so we're done. Before we leave, though,
                 * add the terminating NULL to the name buffer.
                 */
                cnt = OSPPBfrWriteByte(ospvNameBfr, '\0');
                if (cnt != 1) {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
                break;
            }
            /* the character is legitimate, go ahead and read it */
            ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding,
                                             &readChar);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                /* just a sanity check here */
                /* assert(OSPPXMLIsName(readChar)); */

                /* put the character into the name buffer */
                cnt = OSPPBfrWriteByte(ospvNameBfr, readChar);
                if (cnt != 1) {
                    ospvErrCode = OSPC_ERR_BUF_EMPTY;
                }
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipProlog() - skip the XML prolog
 */
unsigned OSPPXMLDocSkipProlog(  /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;

    /*
     * An XML prolog consists of the XML declaration, following by zero
     * or more miscellaneous items (comments or processing instructions),
     * followed by zero or one document type definitions, and then
     * followed by zero or more miscellaneous items.
     */

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* skip the XML declaration, if present */
        ospvErrCode = OSPPXMLDocSkipDecl(ospvBfrAddr, ospvEncoding);
    }

    /* skip any misc items (comments or PIs) in the prolog */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocSkipAllMisc(ospvBfrAddr, ospvEncoding);
    }

    /* skip a document type declaration */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocSkipDTD(ospvBfrAddr, ospvEncoding);
    }

    /* skip any final misc items (comments or PIs) in the prolog */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocSkipAllMisc(ospvBfrAddr, ospvEncoding);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipAllMisc() - skip miscellaneous components in prolog
 */
unsigned OSPPXMLDocSkipAllMisc( /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned isComment = OSPC_FALSE;
    unsigned isPI = OSPC_FALSE;

    /*
     * Miscellaneous items can be either comments or processing instructions
     * (or whitespace). Since this function skips ALL misc items, we loop
     * looking for either of the two (and skipping over whitespace in the
     * process). If we find one, we skip it.
     */

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    /* now go into the loop looking for and skipping misc items */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* first take care of any whitespace */
        ospvErrCode = OSPPXMLDocSkipWhite(ospvBfrAddr, ospvEncoding);

#ifdef COMPLETE_XML_PARSING
        /* Misc can be either comments ... */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsComment(ospvBfrAddr, ospvEncoding, &isComment);
        }

        /* ... or processing instructions */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsPI(ospvBfrAddr, ospvEncoding, &isPI);
        }
#endif

        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (isComment) {
                ospvErrCode = OSPPXMLDocSkipComment(ospvBfrAddr, ospvEncoding);
            } else if (isPI) {
                ospvErrCode = OSPPXMLDocSkipPI(ospvBfrAddr, ospvEncoding);
            } else {
                /*
                 * If we didn't find a comment or PI, then we're done,
                 * so break from the loop
                 */
                break;
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipDTD() - skip document type definition (if present)
 */
unsigned OSPPXMLDocSkipDTD(     /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar = '\0';
    unsigned isDTD = OSPC_FALSE;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* skip any whitespace */
        ospvErrCode = OSPPXMLDocSkipWhite(ospvBfrAddr, ospvEncoding);
    }
    /*
     * The first thing we do is look for the beginning of a DTD
     * declaration. If there's not one, we don't have any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocIsDTD(ospvBfrAddr, ospvEncoding, &isDTD);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isDTD) {
        /*
         * We've got a match for the DTD tag, so we go into a loop
         * that skips over all the stuff that could be stuck inside
         * a DTD (unfortunately, that's a lot of stuff.
         */
        while (ospvErrCode == OSPC_ERR_NO_ERROR) {
            /*
             * Either one of two things will happen now. If
             * things are easy, we'll find the ending tag ('>')
             * that marks the end of the DTD. If that happens,
             * we're done. A more complicated possibility,
             * though, is that we run across the square opening
             * bracket ('['). If that's the case then what
             * follows is the whole DTD included in the document.
             * That's not really efficient in this application,
             * but it is legal so we're prepared to deal with
             * it.
             */

            ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                if (readChar == OSPC_XMLDOC_CLOSE) {
                    /*
                     * Wow, that was easy. We're done so break from
                     * the loop.
                     */
                    break;
                } else if (readChar == OSPC_XMLDOC_MARKUPDECLOPEN) {
                    /*
                     * Oh well, we've got some markup declaration
                     * here. The only thing to do is trudge through
                     */
                    ospvErrCode = OSPPXMLDocSkipMarkupDecl(ospvBfrAddr, ospvEncoding);
                }
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipMarkupDecl() - skip markup declarations in prolog
 */
unsigned OSPPXMLDocSkipMarkupDecl(  /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)      /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned isComment = OSPC_FALSE;
    unsigned isPI = OSPC_FALSE;
    unsigned isElementDecl = OSPC_FALSE;
    unsigned isAttlist = OSPC_FALSE;
    unsigned isEntityDecl = OSPC_FALSE;
    unsigned char readChar = '\0';

    /*
     * What we're calling "markup declaration" here includes not only
     * the XML markupdecl but also PEReference and whitespace. All of
     * these can appear in the spot that markup declaration does in
     * the prolog, and all of them we need to skip.
     *
     * What we're really going to do is look for the closing square
     * bracket (']') that marks the end of the markup declaration.
     * The complication, though, is that the bracket could also
     * theoretically appear inside another element (such as a
     * comment). That means we have to keep track of where we are
     * and what we're looking at.
     */

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    /* now go into the loop looking for and skipping items */
    while (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* first take care of any whitespace */
        ospvErrCode = OSPPXMLDocSkipWhite(ospvBfrAddr, ospvEncoding);

        /*
         * Now take a peek at what's next. We need to handle several
         * possibilities.
         */
#ifdef COMPLETE_XML_PARSING
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsComment(ospvBfrAddr, ospvEncoding, &isComment);
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsPI(ospvBfrAddr, ospvEncoding, &isPI);
        }
#endif
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsElementDecl(ospvBfrAddr, ospvEncoding, &isElementDecl);
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsAttlist(ospvBfrAddr, ospvEncoding, &isAttlist);
        }
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            ospvErrCode = OSPPXMLDocIsEntityDecl(ospvBfrAddr, ospvEncoding, &isEntityDecl);
        }

        /*
         * Now that we've had a chance to see what's coming, we should
         * know how to handle it.
         */
        if (ospvErrCode == OSPC_ERR_NO_ERROR) {
            if (isComment) {
                ospvErrCode = OSPPXMLDocSkipComment(ospvBfrAddr, ospvEncoding);
            } else if (isPI) {
                ospvErrCode = OSPPXMLDocSkipPI(ospvBfrAddr, ospvEncoding);
            } else if (isElementDecl) {
                ospvErrCode = OSPPXMLDocSkipElementDecl(ospvBfrAddr, ospvEncoding);
            } else if (isAttlist) {
                ospvErrCode = OSPPXMLDocSkipAttlist(ospvBfrAddr, ospvEncoding);
            } else if (isEntityDecl) {
                ospvErrCode = OSPPXMLDocSkipEntityDecl(ospvBfrAddr, ospvEncoding);
            } else {
                /*
                 * We didn't find any of the special items, so read
                 * the next character. It might be the end of our
                 * markup declaration. Of course, it could be a
                 * a PEReference, in which case we'll just keep
                 * going.
                 */
                ospvErrCode = OSPPXMLDocReadChar(ospvBfrAddr, ospvEncoding, &readChar);
                if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                    if (readChar == OSPC_XMLDOC_MARKUPDECLCLOSE) {
                        /* we've finally reached the end; break the loop */
                        break;
                    }
                }
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipDecl() - skip XML declaration (if present)
 */
unsigned OSPPXMLDocSkipDecl(    /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_DECLEND };
    unsigned char scratch[OSPC_XMLDOC_DECLENDLEN + 1];
    unsigned isDecl = OSPC_FALSE;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    /*
     * The first thing we do is look for the beginning of an XML
     * declaration. If there's not one, we don't have any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocIsDecl(ospvBfrAddr, ospvEncoding, &isDecl);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isDecl) {
        /*
         * Yes, we've found a comment. Now we just read through
         * the buffer until we've found the closing tag. Note
         * that we don't have to explicitly read the opening
         * tag (we've only "peeked" it so far) since the
         * closing tag does not appear in the opening tag.
         */

        ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipComment() - skip XML comment (if present)
 */
unsigned OSPPXMLDocSkipComment( /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_COMMENTEND };
    unsigned char scratch[OSPC_XMLDOC_COMMENTENDLEN + 1];
    unsigned isComment = OSPC_FALSE;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }

    /*
     * The first thing we do is look for the beginning of an XML
     * comment. If there's not one, we don't have any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocIsComment(ospvBfrAddr, ospvEncoding, &isComment);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isComment) {
        /*
         * Yes, we've found a comment. Now we just read through
         * the buffer until we've found the closing tag. Note
         * that we don't have to explicitly read the opening
         * tag (we've only "peeked" it so far) since the
         * closing tag does not appear in the opening tag.
         */
        ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipPI() - skip XML processing instruction (if present)
 */
unsigned OSPPXMLDocSkipPI(      /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_PIEND };
    unsigned char scratch[OSPC_XMLDOC_PIENDLEN + 1];
    unsigned isPI = OSPC_FALSE;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    /*
     * The first thing we do is look for the beginning of an XML
     * processing instruction. If there's not one, we don't have
     * any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocIsPI(ospvBfrAddr, ospvEncoding, &isPI);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isPI) {
        /*
         * Yes, we've found a PI. Now we just read through
         * the buffer until we've found the closing tag. Note
         * that we don't have to explicitly read the opening
         * tag (we've only "peeked" it so far) since the
         * closing tag does not appear in the opening tag.
         */
        ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipElementDecl() - skip XML element declaration (if present)
 */
unsigned OSPPXMLDocSkipElementDecl( /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)      /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_ELEMENTDECLEND };
    unsigned char scratch[OSPC_XMLDOC_ELEMENTDECLENDLEN + 1];
    unsigned isDecl = OSPC_FALSE;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    /*
     * The first thing we do is look for the beginning of an XML
     * element declaration. If there's not one, we don't have
     * any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocIsElementDecl(ospvBfrAddr, ospvEncoding, &isDecl);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isDecl) {
        /*
         * Yes, we've found a declaration. Now we just read through
         * the buffer until we've found the closing tag. Note
         * that we don't have to explicitly read the opening
         * tag (we've only "peeked" it so far) since the
         * closing tag does not appear in the opening tag.
         */
        ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipEntityDecl() - skip XML entity declaration (if present)
 */
unsigned OSPPXMLDocSkipEntityDecl(  /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)      /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_ENTITYDECLEND };
    unsigned char scratch[OSPC_XMLDOC_ENTITYDECLENDLEN + 1];
    unsigned isDecl = OSPC_FALSE;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    /*
     * The first thing we do is look for the beginning of an XML
     * entity declaration. If there's not one, we don't have
     * any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocIsEntityDecl(ospvBfrAddr, ospvEncoding, &isDecl);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isDecl) {
        /*
         * Yes, we've found a declaration. Now we just read through
         * the buffer until we've found the closing tag. Note
         * that we don't have to explicitly read the opening
         * tag (we've only "peeked" it so far) since the
         * closing tag does not appear in the opening tag.
         */
        ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipAttlist() - skip XML attribute list (if present)
 */
unsigned OSPPXMLDocSkipAttlist( /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)  /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_ATTLISTEND };
    unsigned char scratch[OSPC_XMLDOC_ATTLISTENDLEN + 1];
    unsigned isList = OSPC_FALSE;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    /*
     * The first thing we do is look for the beginning of an XML
     * attribute list. If there's not one, we don't have
     * any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocIsAttlist(ospvBfrAddr, ospvEncoding, &isList);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && isList) {
        /*
         * Yes, we've found a list. Now we just read through
         * the buffer until we've found the closing tag. Note
         * that we don't have to explicitly read the opening
         * tag (we've only "peeked" it so far) since the
         * closing tag does not appear in the opening tag.
         */
        ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipPastCdataBeg() - skip XML CDATA beginning section
 */
unsigned OSPPXMLDocSkipPastCdataBeg(    /* returns error code */
    OSPTBFR **ospvBfrAddr,              /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)          /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = OSPC_XMLDOC_CDATABEG;
    unsigned char scratch[OSPC_XMLDOC_CDATABEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocSkipPastCdataEnd() - skip XML CDATA end section
 */

unsigned OSPPXMLDocSkipPastCdataEnd(/* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding)      /* character encoding for the document */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_CDATAEND };
    unsigned char scratch[OSPC_XMLDOC_CDATAENDLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    ospvErrCode = OSPPXMLDocSkipPast(ospvBfrAddr, ospvEncoding, tag, scratch);

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsDecl() - is the document at an XML declaration
 */

unsigned OSPPXMLDocIsDecl(      /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned *ospvIsDecl)       /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned char readChar = '\0';
    const unsigned char tag[] = OSPC_XMLDOC_DECLBEG;
    unsigned char scratch[OSPC_XMLDOC_DECLBEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsDecl == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    /*
     * The first thing we do is look for the beginning of an XML
     * declaration. If there's not one, we don't have any work to do.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        *ospvIsDecl = OSPC_FALSE;
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_DECLBEGLEN, scratch, ospvIsDecl);
    }
    if ((ospvErrCode == OSPC_ERR_NO_ERROR) && *ospvIsDecl) {
        /*
         * We've got a match for the header tag, so see if the next
         * character is a whitespace.
         */
        OSPPXMLDocPeekCharN(ospvBfrAddr, ospvEncoding, OSPC_XMLDOC_DECLBEGLEN, &readChar, (int *)&ospvErrCode);
        if ((ospvErrCode != OSPC_ERR_NO_ERROR) || (!OSPPXMLIsSpace(readChar))) {
            /* not a whitespace; this isn't a declaration after all */
            *ospvIsDecl = OSPC_FALSE;
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsComment() - is the document at an XML comment
 */
unsigned OSPPXMLDocIsComment(   /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned *ospvIsComment)    /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = OSPC_XMLDOC_COMMENTBEG;
    unsigned char scratch[OSPC_XMLDOC_COMMENTBEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsComment == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        *ospvIsComment = OSPC_FALSE;
        /* now look for the match of the comment beginning */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_COMMENTBEGLEN, scratch, ospvIsComment);
    }
    return ospvErrCode;
}

/*
 * OSPPXMLDocIsPI() - is the document at an XML processing instruction
 */
unsigned OSPPXMLDocIsPI(        /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned *ospvIsPI)         /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_PIBEG };
    unsigned char scratch[OSPC_XMLDOC_PIBEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsPI == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume that it isn't a PI (in case of premature error exit) */
        *ospvIsPI = OSPC_FALSE;

        /* now look for the match of the PI beginning */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_PIBEGLEN, scratch, ospvIsPI);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsDTD() - is the document at an XML data type definition
 */
unsigned OSPPXMLDocIsDTD(       /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned *ospvIsDTD)        /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = OSPC_XMLDOC_DTDBEG;
    unsigned char scratch[OSPC_XMLDOC_DTDBEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsDTD == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume that it isn't a DTD (in case of premature error exit) */
        *ospvIsDTD = OSPC_FALSE;

        /* now look for the match of the DTD beginning */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_DTDBEGLEN, scratch, ospvIsDTD);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsElementDecl() - is document at an XML element declaration
 */
unsigned OSPPXMLDocIsElementDecl(   /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,      /* character encoding for the document */
    unsigned *ospvIsElemDecl)       /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = OSPC_XMLDOC_ELEMENTDECLBEG;
    unsigned char scratch[OSPC_XMLDOC_ELEMENTDECLBEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsElemDecl == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume that it isn't a declaration (in case of premature error exit) */
        *ospvIsElemDecl = OSPC_FALSE;

        /* now look for the match of the element declaration beginning */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_ELEMENTDECLBEGLEN, scratch, ospvIsElemDecl);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsAttlist() - is document at an XML attribute list
 */
unsigned OSPPXMLDocIsAttlist(   /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned *ospvIsAttlist)    /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = OSPC_XMLDOC_ATTLISTBEG;
    unsigned char scratch[OSPC_XMLDOC_ATTLISTBEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsAttlist == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume that it isn't a list (in case of premature error exit) */
        *ospvIsAttlist = OSPC_FALSE;

        /* now look for the match of the attribute list beginning */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_ATTLISTBEGLEN, scratch, ospvIsAttlist);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsEntityDecl() - is document at an XML entity declaration
 */
unsigned OSPPXMLDocIsEntityDecl(    /* returns error code */
    OSPTBFR **ospvBfrAddr,          /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,      /* character encoding for the document */
    unsigned *ospvIsEntityDecl)     /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = OSPC_XMLDOC_ENTITYDECLBEG;
    unsigned char scratch[OSPC_XMLDOC_ENTITYDECLBEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsEntityDecl == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume that it isn't a declaration (in case of premature error exit) */
        *ospvIsEntityDecl = OSPC_FALSE;

        /* now look for the match of the entity declaration beginning */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_ENTITYDECLBEGLEN, scratch, ospvIsEntityDecl);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsCdata() - is document at an XML CDATA section
 */
unsigned OSPPXMLDocIsCdata(     /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned *ospvIsCdata)      /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = OSPC_XMLDOC_CDATABEG;
    unsigned char scratch[OSPC_XMLDOC_CDATABEGLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsCdata == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume that it isn't CDATA (in case of premature error exit) */
        *ospvIsCdata = OSPC_FALSE;

        /* now look for the match of the CDATA beginning */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_CDATABEGLEN, scratch, ospvIsCdata);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocIsCdataEnd() - is document at the end of an XML CDATA section
 */
unsigned OSPPXMLDocIsCdataEnd(  /* returns error code */
    OSPTBFR **ospvBfrAddr,      /* buffer containing document */
    OSPE_XML_ENC ospvEncoding,  /* character encoding for the document */
    unsigned *ospvIsCdataEnd)   /* place to put answer */
{
    unsigned ospvErrCode = OSPC_ERR_NO_ERROR;
    const unsigned char tag[] = { OSPC_XMLDOC_CDATAEND };
    unsigned char scratch[OSPC_XMLDOC_CDATAENDLEN + 1];

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (ospvEncoding == OSPC_XENC_UNKNOW) {
        ospvErrCode = OSPC_ERR_XML_BAD_ENC;
    }
    if (ospvIsCdataEnd == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_INVALID_ARGS;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* assume that it isn't at the end (in case of premature error exit) */
        *ospvIsCdataEnd = OSPC_FALSE;

        /* now look for the match of the CDATA end marker */
        ospvErrCode = OSPPXMLDocIsMatch(ospvBfrAddr, ospvEncoding, tag, OSPC_XMLDOC_CDATAENDLEN, scratch, ospvIsCdataEnd);
    }

    return ospvErrCode;
}
