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
 * ospxmlenc.c - Generic XML document encoding functions.
 */

#include "osp/osp.h"
#include "osp/ospbfr.h"
#include "osp/osperrno.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"
#include "osp/ospxmltype.h"
#include "osp/ospxmldoc.h"

/*
 * OSPPXMLDocCreate() - create a linear document from a root element
 */

int OSPPXMLDocCreate(         /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* root element for document */
    OSPTBFR **ospvBfrAddr)    /* buffer in which to put doc */
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned cnt = 0;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }

    /*
     * First start off with the XML declaration. This is not technically
     * required by the XML specification since we're not doing anything
     * other than XML version 1.0, nor are we using any fancy character
     * encodings.
     */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        cnt = OSPPBfrWriteBlock(ospvBfrAddr, OSPC_XMLDOC_DECL, OSPC_XMLDOC_DECLLEN);
        if (cnt == OSPC_XMLDOC_DECLLEN) {
            /*
             * If the declaration was successfully added, go ahead and
             * construct the root element.
             */

            ospvErrCode = OSPC_ERR_BUF_INCOMPLETE;

            ospvErrCode = OSPPXMLDocAddElem(ospvElem, ospvBfrAddr);

            /* re-add this code later (maybe) */
            /*
             * ospvErrCode = OSPPXMLDocAddElem(ospvElem, ospvBfrAddr);
             */
        } else {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocAddElem() - add an element to a document
 */

int OSPPXMLDocAddElem(        /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* element to add */
    OSPTBFR **ospvBfrAddr)    /* buffer to add to */
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    OSPT_XML_ELEM *child = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;
    const char *val = OSPC_OSNULL;
    unsigned vallen = 0;
    unsigned cnt;

    /*
     * Because of the length of this routine, it's broken up into the
     * eight major steps involved in encoding an element. Look for the
     * "**** STEP n:" comments that emphasize these steps.
     */

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * Pre-calculate some useful variables; we might need them in several
         * places in the following code. We're pre-calculating the element
         * value, and the element value's length.
         */
        val = OSPPXMLElemGetValue(ospvElem);
        if (val != OSPC_OSNULL) {
            vallen = OSPM_STRLEN(val);
        } else {
            vallen = 0;
        }
    }
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /**** STEP 1: begin with the opening bracket */
        ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_OPEN);
    }

    /**** STEP 2: add the element name */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocAddElemName(ospvElem, ospvBfrAddr);
    }

    /**** STEP 3: add the attributes */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* the "for" loop steps through each attribute */
        for (attr = (OSPT_XML_ATTR *)OSPPXMLElemFirstAttr(ospvElem);
            ((attr != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
            attr = (OSPT_XML_ATTR *)OSPPXMLElemNextAttr(ospvElem, attr))
        {
            /*
             * Attributes are separated by spaces, and we add a space
             * first in order to separate the first attribute from the
             * element name. That also means that we won't end up with
             * any extra trailing space, which is correct.
             */
            ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_SPACE);

            /* now add the attributes themselves */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                ospvErrCode = OSPPXMLDocAddAttr(attr, ospvBfrAddr);
            }
        }
    }

    /**** STEP 4: if no children or contents, add trailer */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * Empty elements have neither children or content; they
         * can, however, include attributes (so make sure the step
         * followes adding attributes
         */
        child = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
        if ((child == OSPC_OSNULL) && (vallen == 0)) {
            /* if the element is empty, go ahead and add the trailer */
            ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_TRAILER);
        }
    }

    /**** STEP 5: add the closing bracket (for the opening tag) */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_CLOSE);
        /* add newline for readability */
        ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_LF);
    }

    /**** STEP 6: add the child elements */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* the "for" loop walks through all child elements */
        for (child = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
            ((child != OSPC_OSNULL) && (ospvErrCode == OSPC_ERR_NO_ERROR));
            child = (OSPT_XML_ELEM *)OSPPXMLElemNextChild(ospvElem, child))
        {
            /*
             * Call ourselves recursively. Since OSP documents aren't
             * terribly deep in structure, this shouldn't cause too
             * much grief with stack space, etc.. Still, there is a
             * possible optimization here.
             */
            ospvErrCode = OSPPXMLDocAddElem(child, ospvBfrAddr);
        }
    }

    /**** STEP 7: add the value itself */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* of course, make sure there's something to add */
        if (vallen > 0) {
            cnt = OSPPBfrWriteBlock(ospvBfrAddr, (const void *)val, vallen);
            if (cnt != vallen) {
                ospvErrCode = OSPC_ERR_BUF_EMPTY;
            }
        }
    }

    /**** STEP 8: add the closing tag (but only if there was some content) */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * Empty elements have neither children or content; they
         * can, however, include attributes (so we don't bother to
         * check for attributes or not).
         */
        child = (OSPT_XML_ELEM *)OSPPXMLElemFirstChild(ospvElem);
        if ((child != OSPC_OSNULL) || (vallen != 0)) {
            /* first is the opening bracket */
            ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_OPEN);

            /* then the trailing slash */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_TRAILER);
            }

            /* then the element name */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                ospvErrCode = OSPPXMLDocAddElemName(ospvElem, ospvBfrAddr);
            }

            /* add the closing bracket */
            if (ospvErrCode == OSPC_ERR_NO_ERROR) {
                ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_CLOSE);
                /* add newline for readability */
                ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_LF);
            }
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocAddElemName() - add an element name to a document
 */

int OSPPXMLDocAddElemName(    /* returns error code */
    OSPT_XML_ELEM *ospvElem,    /* element whose name to add */
    OSPTBFR **ospvBfrAddr)    /* buffer to add to */
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    const char *name = OSPC_OSNULL;
    unsigned namelen;
    unsigned cnt;

    if (ospvElem == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ELEMENT;
    }
    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* get the name and its length */
        name = OSPPXMLElemGetName(ospvElem);
        namelen = OSPM_STRLEN(name);

        /* actually write the name now */
        cnt = OSPPBfrWriteBlock(ospvBfrAddr, (const void *)name, namelen);

        /* finally, make sure that the name was added okay */
        if (cnt == namelen) {
            ospvErrCode = OSPC_ERR_NO_ERROR;
        } else {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocAddAttr() - add an attribute to a document
 */

int OSPPXMLDocAddAttr(        /* returns error code */
    OSPT_XML_ATTR *ospvAttr,    /* attribute to add */
    OSPTBFR **ospvBfrAddr)    /* buffer to add to */
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    const char *val;
    unsigned vallen;
    unsigned cnt;

    if (ospvAttr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
    }
    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* begin with the attribute name */
        ospvErrCode = OSPPXMLDocAddAttrName(ospvAttr, ospvBfrAddr);
    }
    /* then the equals sign */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_EQUALS);
    }

    /* and the quotation mark */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_QUOTE);
    }

    /* the value itself */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /* what is the value that we need to write? */
        val = OSPPXMLAttrGetValue(ospvAttr);
        if (val != OSPC_OSNULL) {
            vallen = OSPM_STRLEN(val);
        } else {
            vallen = 0;
        }

        /* make sure we've got something to actually write */
        if (vallen > 0) {
            cnt = OSPPBfrWriteBlock(ospvBfrAddr, (const void *)val, vallen);
            if (cnt != vallen) {
                /* if the write failed, note the error */
                ospvErrCode = OSPC_ERR_BUF_EMPTY;
            }
        }
    }

    /* and the closing quotation mark */
    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        ospvErrCode = OSPPXMLDocAddChar(ospvBfrAddr, OSPC_XMLDOC_QUOTE);
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocAddAttrName() - add an attribute name to a document
 */

int OSPPXMLDocAddAttrName(    /* returns error code */
    OSPT_XML_ATTR *ospvAttr,    /* attribute whose name to add */
    OSPTBFR **ospvBfrAddr)    /* buffer to add to */
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    const char *name = OSPC_OSNULL;
    unsigned namelen;
    unsigned cnt;

    if (ospvAttr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_XML_NO_ATTR;
    }
    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * First let's get the attribute tag and then use that to find the
         * character name of the attribute.
         */

        /* get the name and its length */
        name = OSPPXMLAttrGetName(ospvAttr);
        /*        assert(name != OSPC_OSNULL); */
        namelen = OSPM_STRLEN(name);

        /* actually write the name */
        cnt = OSPPBfrWriteBlock(ospvBfrAddr, (const void *)name, namelen);

        /* make sure that the name was written okay */
        if (cnt == namelen) {
            ospvErrCode = OSPC_ERR_NO_ERROR;
        } else {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        }
    }

    return ospvErrCode;
}

/*
 * OSPPXMLDocAddChar() - add a single character to an XML document
 */

int OSPPXMLDocAddChar(        /* returns error code */
    OSPTBFR **ospvBfrAddr,    /* buffer in which to add */
    char ospvChar)            /* character to add */
{
    int ospvErrCode = OSPC_ERR_NO_ERROR;
    unsigned cnt;

    if (ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }
    if (*ospvBfrAddr == OSPC_OSNULL) {
        ospvErrCode = OSPC_ERR_BUF_EMPTY;
    }

    if (ospvErrCode == OSPC_ERR_NO_ERROR) {
        /*
         * This is really just a wrapper around the BFR function that
         * writes a byte, but it does allow us to handle the error
         * conditions in way that's consistent with the rest of the
         * XML encoding routines.
         */
        cnt = OSPPBfrWriteByte(ospvBfrAddr, ospvChar);
        if (cnt == 0) {
            ospvErrCode = OSPC_ERR_BUF_EMPTY;
        } else {
            ospvErrCode = OSPC_ERR_NO_ERROR;
        }
    }

    return ospvErrCode;
}
