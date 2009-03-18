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
 * ospxmlelem.c - Functions for generic XML attributes.
 */

#include "osp/osp.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"
#include "osp/ospxmlelem.h"

/*
 * OSPPXMLElemNew() - create a new element
 */

OSPT_XML_ELEM *OSPPXMLElemNew(  /* returns the new element (or NULL) */                
    const char *ospvName,       /* name of the element */
    const char *ospvValue)      /* character string value for element */
{
    OSPT_XML_ELEM *ospvElem = OSPC_OSNULL;
    char *nameptr = OSPC_OSNULL;
    unsigned namelen = 0;
    char *valptr = OSPC_OSNULL;
    unsigned vallen = 0;

    /*
     * XMLElem objects are actually two parts -- the first is the XMLElem
     * structure and the second is the name & value for the element. The two
     * parts are contiguous in memory, and are created (and destroyed)
     * at the same time. Here's a rough picture:
     *
     *    Memory:
     *      +--------+
     *      |        |  This part is the structure itself. It's actually
     *      |        |  visible to the functions (and macros).
     *      +--------+
     *      |        |  This part contains the character strings for the
     *      |        |  element's name & value. It's not directly accessible,
     *      |        |  but pointers to the name/value within the XMLElem
     *      |        |  structure are set to point to it.
     *      +--------+
     *
     * We do it this way to conveniently manage variable-length name and
     * value strings. If we put either in a visible part of the structure,
     * then we'd have to define that field, and that would require
     * defining a size for it. Since we can't predict how big the
     * element values may be that arrive in some arbitrary XML
     * document, picking the size for the structure would be pretty
     * difficult.
     *
     * Note that this technique does use dynamic memory allocation.
     * If memory fragmentation is a concern, one possible optimization
     * would be to define a pool of XMLElem objects in which the
     * value size was fixed to some reasonable size. Then you could
     * allocate from the pool in most cases, and fall back to this
     * approach only if the value was too big for the pool objects
     * (or, perhaps, if the pool was empty).
     */

    if (ospvName != OSPC_OSNULL) {
        if (ospvValue != OSPC_OSNULL) {
            /* get the length of the name & value since we'll need it a few times */
            namelen = OSPM_STRLEN(ospvName) + 1;    /* include terminating 0 */
            vallen = OSPM_STRLEN(ospvValue) + 1;    /* include terminating 0 */
            /* try to allocate the memory for the entire object */
            OSPM_MALLOC(ospvElem, OSPT_XML_ELEM, sizeof(OSPT_XML_ELEM) + namelen + vallen);

            /* make sure the allocation succeeded before proceeding */
            if (ospvElem != OSPC_OSNULL) {
                /* calculate where the "hidden" value will go */
                nameptr = ((char *)(ospvElem)) + sizeof(OSPT_XML_ELEM);
                valptr = nameptr + namelen;

                /* copy the value into it's hidden location */
                OSPM_MEMCPY(nameptr, ospvName, namelen);
                OSPM_MEMCPY(valptr, ospvValue, vallen);

                /* fill in the structure fields */
                OSPPListLinkNew(&ospvElem->ospmXMLElemLink);
                OSPPListNew(&ospvElem->ospmXMLElemChild);
                OSPPListNew(&ospvElem->ospmXMLElemAttrs);
                ospvElem->ospmXMLElemName = nameptr;
                ospvElem->ospmXMLElemValue = valptr;
            }
        }
    }

    return ospvElem;
}

/*
 * OSPPXMLElemDelete() - destroy an XML element
 */
void OSPPXMLElemDelete(         /* no return value */
    OSPT_XML_ELEM **ospvElem)   /* element to destroy */
{
    OSPT_XML_ELEM *elem = OSPC_OSNULL;
    OSPT_XML_ATTR *attr = OSPC_OSNULL;

    if (*ospvElem != OSPC_OSNULL) {
        /* destroy any attributes */
        while ((attr = (OSPT_XML_ATTR *)OSPPListRemove(&((*ospvElem)->ospmXMLElemAttrs))) != OSPC_OSNULL) {
            OSPPXMLAttrDelete(&attr);
        }

        /* destroy any child elements */
        while ((elem = (OSPT_XML_ELEM *)OSPPListRemove(&((*ospvElem)->ospmXMLElemChild))) != OSPC_OSNULL) {
            OSPPXMLElemDelete(&elem);
        }

        /* free the memory */
        OSPM_FREE(*ospvElem);
        *ospvElem = OSPC_OSNULL;
    }
}

/*
 * OSPPXMLElemGetName() - returns the name from an XML attribute
 */

const char *OSPPXMLElemGetName( /* returns pointer to name */ 
    OSPT_XML_ELEM *ospvElem)    /* element being querried */
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvElem != OSPC_OSNULL) {
        ospvName = ospvElem->ospmXMLElemName;
    }                  

    return ospvName;
}

/*
 * OSPPXMLElemGetValue() - returns the value of an XML element
 */

const char *OSPPXMLElemGetValue(    /* returns pointer to character value */    
    OSPT_XML_ELEM *ospvElem)        /* element in question */
{
    const char *ospvValue = OSPC_OSNULL;

    if (ospvElem != OSPC_OSNULL) {
        ospvValue = ospvElem->ospmXMLElemValue;
    }

    return ospvValue;
}

/*
 * OSPPXMLElemAddChild() - add a child element to the current element
 */

void OSPPXMLElemAddChild(       /* no return value */
    OSPT_XML_ELEM *ospvElem,    /* element to which child is added */
    OSPT_XML_ELEM *ospvChild)   /* child element to add */
{
    if (ospvElem != OSPC_OSNULL) {
        if (ospvChild != OSPC_OSNULL) {
            OSPPListAppend(&ospvElem->ospmXMLElemChild, ospvChild);
        }
    }
}

/*
 * OSPPXMLElemFirstChild() - returns the first child of an element
 */

OSPT_XML_ELEM *OSPPXMLElemFirstChild(   /* returns pointer to child or NULL */     
    OSPT_XML_ELEM *ospvElem)            /* parent element in question */
{
    OSPT_XML_ELEM *ospvChild = OSPC_OSNULL;

    if (ospvElem != OSPC_OSNULL) {
        ospvChild = (OSPT_XML_ELEM *)OSPPListFirst(&(ospvElem->ospmXMLElemChild));
    }

    return ospvChild;
}

/*
 * OSPPXMLElemNextChild() - returns the next child of an element
 */

OSPT_XML_ELEM *OSPPXMLElemNextChild(    /* returns pointer to child or NULL */
    OSPT_XML_ELEM *ospvElem,            /* parent element in question */
    OSPT_XML_ELEM *ospvChild)           /* current child element */
{
    OSPT_XML_ELEM *ospvNext = OSPC_OSNULL;

    if (ospvElem != OSPC_OSNULL) {
        if (ospvChild != OSPC_OSNULL) {
            ospvNext = (OSPT_XML_ELEM *)OSPPListNext(&(ospvElem->ospmXMLElemChild), ospvChild);
        }
    }

    return ospvNext;
}

/*
 * OSPPXMLElemAddAttr() - add an attribute to the current element
 */

void OSPPXMLElemAddAttr(        /* no return value */
    OSPT_XML_ELEM *ospvElem,    /* element to which attribute is added */
    OSPT_XML_ATTR *ospvAttr)    /* attribute to add */
{
    if (ospvElem != OSPC_OSNULL) {
        if (ospvAttr != OSPC_OSNULL) {
            OSPPListAppend(&ospvElem->ospmXMLElemAttrs, ospvAttr);
        }
    }
}

/*
 * OSPPXMLElemFirstAttr() - returns the first attribute of an element
 */

OSPT_XML_ATTR *OSPPXMLElemFirstAttr(    /* returns pointer to attribute or NULL */
    OSPT_XML_ELEM *ospvElem)            /* parent element in question */
{
    OSPT_XML_ATTR *ospvAttr = OSPC_OSNULL;

    if (ospvElem != OSPC_OSNULL) {
        ospvAttr = (OSPT_XML_ATTR *)OSPPListFirst(&(ospvElem->ospmXMLElemAttrs));
    }
    return ospvAttr;
}

/*
 * OSPPXMLElemNextAttr() - returns the next attribute of an element
 */

OSPT_XML_ATTR *OSPPXMLElemNextAttr(     /* returns pointer to attribute or NULL */
    OSPT_XML_ELEM *ospvElem,            /* parent element in question */
    OSPT_XML_ATTR *ospvAttr)            /* current attribute */
{
    OSPT_XML_ATTR *ospvNext = OSPC_OSNULL;

    if (ospvElem != OSPC_OSNULL) {
        if (ospvAttr != OSPC_OSNULL) {
            ospvNext = (OSPT_XML_ATTR *)OSPPListNext(&(ospvElem->ospmXMLElemAttrs), ospvAttr);
        }
    }

    return ospvNext;
}
