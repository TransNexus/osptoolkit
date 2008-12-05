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
 * ospxmlattr.c - Functions for generic XML attributes.
 */

#include "osp/osp.h"
#include "osp/osplist.h"
#include "osp/ospxmlattr.h"

/*
 * OSPPXMLAttrNew() - create a new attribute
 */

OSPTXMLATTR *OSPPXMLAttrNew(        /* returns the new attribute (or NULL) */ 
    const unsigned char *ospvName,  /* name of attribute */
    const unsigned char *ospvValue) /* character string value for attr */
{
    OSPTXMLATTR *ospvAttr = OSPC_OSNULL;
    char *nameptr = OSPC_OSNULL;
    unsigned namelen = 0;
    char *valptr = OSPC_OSNULL;
    unsigned vallen = 0;

    /*
     * XMLAttr objects are actually two parts -- the first is the XMLAttr
     * structure and the second is the name and value for the attribute.
     * The two parts are contiguous in memory, and are created (and destroyed)
     * at the same time. Here's a rough picture:
     *
     *    Memory:
     *      +--------+
     *      |        |  This part is the structure itself. It's actually
     *      |        |  visible to the functions (and macros).
     *      +--------+
     *      |        |  This part contains the character strings for the
     *      |        |  attribute name and value. It's not directly
     *      |        |  accessible, but pointers to the name and value within
     *      |        |  the structure are set to point to it.
     *      +--------+
     *
     * We do it this way to conveniently manage variable-length name & value
     * strings. If we put the name or value in a visible part of the structure,
     * then we'd have to define that field, and that would require
     * defining a size for it. Since we can't predict the size of
     * attribute names or values may be that arrive in some arbitrary XML
     * document, picking the size for the structure would be pretty
     * difficult.
     *
     * Note that this technique does use dynamic memory allocation.
     * If memory fragmentation is a concern, one possible optimization
     * would be to define a pool of XMLAttr objects in which the name and
     * value sizes were fixed to some reasonable size. Then you could
     * allocate from the pool in most cases, and fall back to this
     * approach only if the name/value was too big for the pool objects
     * (or, perhaps, if the pool was empty).
     */

    if (ospvName != OSPC_OSNULL) {
        if (ospvValue != OSPC_OSNULL) {
            /* get the length of the name and value since we'll need it a few times */
            namelen = OSPM_STRLEN((const char *) ospvName) + 1;    /* including terminating 0 */
            vallen = OSPM_STRLEN((const char *) ospvValue) + 1;    /* including terminating 0 */

            /* try to allocate the memory for the entire object */
            OSPM_MALLOC(ospvAttr, OSPTXMLATTR, sizeof(OSPTXMLATTR) + namelen + vallen);

            /* make sure the allocation succeeded before proceeding */
            if (ospvAttr != OSPC_OSNULL) {
                /* calculate where the "hidden" values will go */
                nameptr = ((char *) (ospvAttr)) + sizeof(OSPTXMLATTR);
                valptr = nameptr + namelen;

                /* copy the values into their hidden location */
                OSPM_MEMCPY(nameptr, ospvName, namelen);
                OSPM_MEMCPY(valptr, ospvValue, vallen);

                /* fill in the structure fields */
                OSPPListLinkNew(&ospvAttr->ospmXMLAttrLink);
                ospvAttr->ospmXMLAttrName = nameptr;
                ospvAttr->ospmXMLAttrValue = valptr;
            }
        }
    }

    return (ospvAttr);
}

/*
 * OSPPXMLAttrDelete() - destroy an XML attribute
 */

void OSPPXMLAttrDelete(         /* no return value */
    OSPTXMLATTR **ospvAttr)    /* attribute to destroy */
{
    if (*ospvAttr != OSPC_OSNULL) {
        OSPM_FREE(*ospvAttr);
        *ospvAttr = OSPC_OSNULL;
    }
}

/*
 * OSPPXMLAttrGetName()
 */

const char *OSPPXMLAttrGetName(
    OSPTXMLATTR * ospvAttr)
{
    const char *ospvName = OSPC_OSNULL;

    if (ospvAttr != OSPC_OSNULL) {
        ospvName = ospvAttr->ospmXMLAttrName;
    }

    return (ospvName);
}

/*
 * OSPPXMLAttrGetValue()
 */

const char *OSPPXMLAttrGetValue(
    OSPTXMLATTR * ospvAttr)
{
    const char *ospvValue = OSPC_OSNULL;

    if (ospvAttr != OSPC_OSNULL) {
        ospvValue = ospvAttr->ospmXMLAttrValue;
    }

    return (ospvValue);
}
