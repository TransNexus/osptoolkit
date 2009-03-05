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
 * osplist.c - Utility functions for linked lists.
 */

#include "osp/osp.h"
#include "osp/osplist.h"

/*
 * The following functions implement a fairly straightforward
 * linked list, though there is a small twist. Specifically,
 * a la Knuth, the List structure actually points to the last
 * item on the list, which is then circularly linked to the
 * first item. This approach minimizes the time needed to
 * find either the front (2 memory references) or back (1
 * memory reference) of the list.
 *
 *    List Head: ---------------------------------+
 *                                                |
 *                                                v
 *         +-------+    +-------+             +-------+
 *    +--->| Item1 |--->| Item2 |---> ... --->| ItemN |---+
 *    |    +-------+    +-------+             +-------+   |
 *    +---------------------------------------------------+
 *
 */

/*
 * Many of the following functions are also implemented as
 * macros for speed. If so implemented, the macro implementation
 * takes precedence. To force the use of the function
 * implementation (e.g. for debugging), simply comment out
 * the macro definition in osplist.h.
 */

/*
 * OSPPListLinkNew() - initialize a list link
 */
void OSPPListLinkNew(       /* no return value */
    OSPTLISTLINK *ospvLink) /* address of link to initialize */
{
    if (ospvLink != OSPC_OSNULL) {
        /* just NULL out the link pointer */
        ospvLink->ospmLinkNext = OSPC_OSNULL;
    }
}

/*
 * OSPPListNew() - initialize a list
 */
void OSPPListNew(       /* no return value */
    OSPTLIST *ospvList) /* address of list to initialize */
{
    if (ospvList != OSPC_OSNULL) {
        /* just NULL out the list pointer */
        *ospvList = OSPC_OSNULL;
    }
}

/*
 * OSPPListDelete() - clean up for a list that's not needed
 */
void OSPPListDelete(    /* no return value */
    OSPTLIST *ospvList) /* address of list to destroy */
{
    if (ospvList != OSPC_OSNULL) {
        /* check to see that the list is empty */
        if (*ospvList == OSPC_OSNULL) {
            /* NULL out the list pointer */
            *ospvList = OSPC_OSNULL;
        }
    }
}

/*
 * OSPPListEmpty() - is a list empty?
 */
unsigned OSPPListEmpty(/* non-zero if empty, zero otherwise */
    OSPTLIST *ospvList)   /* list to check */
{
    unsigned ospvEmpty = OSPC_TRUE;

    if (ospvList != OSPC_OSNULL) {
        ospvEmpty = (*ospvList == OSPC_OSNULL);
    }

    return ospvEmpty;
}

/*
 * OSPPListLast() - returns last item on a list
 */
void *OSPPListLast(     /* returns pointer to item or NULL */
    OSPTLIST *ospvList) /* list to reference */
{
    /* see introductory comment on list structure at beginning of file */
    OSPTLISTLINK *ospvLast = OSPC_OSNULL;

    if (ospvList != OSPC_OSNULL) {
        ospvLast = *ospvList;
    }

    return ospvLast;
}

/*
 * OSPPListFirst() - returns first item on list
 */
void *OSPPListFirst(    /* returns pointer to item or null */
    OSPTLIST *ospvList) /* list to reference */
{
    /* see introductory comment on list structure at beginning of file */
    OSPTLISTLINK *ospvFirst = OSPC_OSNULL;    /* item to return */

    if (ospvList != OSPC_OSNULL) {
        if (OSPPListEmpty(ospvList)) {
            ospvFirst = OSPC_OSNULL;
        } else {
            ospvFirst = ((OSPTLISTLINK *) (*ospvList))->ospmLinkNext;
        }
    }

    return ospvFirst;
}

/*
 * OSPPListNext() - returns next item on list without removing any item
 */
void *OSPPListNext(     /* returns pointer to item or null */
    OSPTLIST *ospvList, /* list to reference */
    void *ospvItem)     /* current item */
{
    /* see introductory comment on list structure at beginning of file */

    OSPTLISTLINK *ospvNext = OSPC_OSNULL;    /* item to return */

    if (ospvList != OSPC_OSNULL) {
        if (ospvItem != OSPC_OSNULL) {
            if (OSPPListLast(ospvList) == ospvItem) {
                ospvNext = OSPC_OSNULL;
            } else {
                ospvNext = ((OSPTLISTLINK *)ospvItem)->ospmLinkNext;
            }
        }
    }

    return ospvNext;
}

/*
 * OSPPListMove() - move list contents from one list to another
 */
void OSPPListMove(      /* no return */
    OSPTLIST *ospvDst,  /* destination list */
    OSPTLIST *ospvSrc)  /* source list */
{
    if (ospvDst != OSPC_OSNULL) {
        if (ospvSrc != OSPC_OSNULL) {
            *ospvDst = *ospvSrc;
            OSPPListNew(ospvSrc);
        }
    }
}

/*
 * OSPPListAppend() - add item to end of list
 */
void OSPPListAppend(    /* no return value */
    OSPTLIST *ospvList, /* list to which item is to be added */
    void *ospvItem)     /* item to add */
{
    /* see introductory comment on list structure at beginning of file */
    if (ospvList != OSPC_OSNULL) {
        if (ospvItem != OSPC_OSNULL) {
            if (OSPPListEmpty(ospvList)) {
                ((OSPTLISTLINK *)ospvItem)->ospmLinkNext = (OSPTLISTLINK *)ospvItem;
            } else {
                ((OSPTLISTLINK *)ospvItem)->ospmLinkNext = ((OSPTLISTLINK *)(*ospvList))->ospmLinkNext;
                ((OSPTLISTLINK *)(*ospvList))->ospmLinkNext = (OSPTLISTLINK *)ospvItem;
            }
            *ospvList = (OSPTLISTLINK *)ospvItem;
        }
    }
}

/*
 * OSPPListRemove()
 */
void *OSPPListRemove(   /* returns item removed or NULL */
    OSPTLIST *ospvList)
{
    /* see introductory comment on list structure at beginning of file */
    OSPTLISTLINK *ospvItem = OSPC_OSNULL;    /* item to return */

    if (ospvList != OSPC_OSNULL) {
        if (OSPPListEmpty(ospvList)) {
            ospvItem = OSPC_OSNULL;
        } else if (OSPPListFirst(ospvList) == OSPPListLast(ospvList)) {
            ospvItem = *ospvList;
            *ospvList = OSPC_OSNULL;
            ospvItem->ospmLinkNext = OSPC_OSNULL;
        } else {
            ospvItem = ((OSPTLISTLINK *) (*ospvList))->ospmLinkNext;
            ((OSPTLISTLINK *) (*ospvList))->ospmLinkNext = ospvItem->ospmLinkNext;
            ospvItem->ospmLinkNext = OSPC_OSNULL;
        }
    }

    return ospvItem;
}

/*
 * OSPPListNextToLast() - returns next to last item
 */
void *OSPPListNextToLast(   /* returns item or NULL */
    OSPTLIST *ospvList)
{
    void *lastitem = OSPC_OSNULL, *curritem = OSPC_OSNULL, *nextolast = OSPC_OSNULL;

    lastitem = OSPPListLast(ospvList);
    curritem = OSPPListFirst(ospvList);

    while (curritem != OSPC_OSNULL && ((OSPTLISTLINK *) curritem)->ospmLinkNext != lastitem)
        curritem = OSPPListNext(ospvList, curritem);

    nextolast = curritem;

    return nextolast;
}

/*
 * OSPPListRemoveSpecificItem() - removes a specific item
 */
void *OSPPListRemoveSpecificItem(   /* returns item removed or NULL */
    OSPTLIST *ospvList, 
    void *ospvItem)
{
    /* see introductory comment on list structure at beginning of file */

    void *curritem = OSPC_OSNULL, *previtem = OSPC_OSNULL;

    if (ospvList != OSPC_OSNULL) {

        if (OSPPListEmpty(ospvList)) {
            ospvItem = OSPC_OSNULL;
        } else {
            previtem = curritem = OSPPListFirst(ospvList);
            if (ospvItem == curritem && curritem == OSPPListLast(ospvList)) {
                ospvItem = (void *) *ospvList;
                *ospvList = OSPC_OSNULL;
                ((OSPTLISTLINK *)ospvItem)->ospmLinkNext = OSPC_OSNULL;
            } else {
                for (; curritem != OSPC_OSNULL; curritem = OSPPListNext(ospvList, curritem)) {
                    if (curritem == ospvItem) {
                        /* if first item in list */
                        if (curritem == previtem) {
                            ospvItem = ((OSPTLISTLINK *) (*ospvList))->ospmLinkNext;
                            ((OSPTLISTLINK *) (*ospvList))->ospmLinkNext = ((OSPTLISTLINK *)ospvItem)->ospmLinkNext;
                            ((OSPTLISTLINK *)ospvItem)->ospmLinkNext = OSPC_OSNULL;
                        } else {
                            /* if last item in list */
                            if (curritem == OSPPListLast(ospvList)) {
                                ospvItem = (void *) *ospvList;
                                previtem = OSPPListNextToLast(ospvList);
                                *ospvList = (OSPTLIST) previtem;
                            }
                            ((OSPTLISTLINK *) previtem)->ospmLinkNext = ((OSPTLISTLINK *)ospvItem)->ospmLinkNext;
                        }
                        break;
                    }
                    previtem = curritem;
                }
            }
        }
    }

    return ospvItem;
}

/*
 * OSPPListCount() - returns list count
 */
unsigned OSPPListCount( /* returns list count */
    OSPTLIST *ospvList)
{
    unsigned count = 0;
    void *item = OSPC_OSNULL;

    for (item = OSPPListFirst(ospvList); item != OSPC_OSNULL; item = OSPPListNext(ospvList, item)) {
        count++;
    }

    return count;
}
