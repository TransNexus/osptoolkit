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
 * osplist.h - Structures and prototypes for linked lists.
 */

#ifndef osplist_h
#define osplist_h

#include "osp.h"

/*
 * This module provides simple, linked list utility functions
 * used through the OSP library. This file defines the two
 * structures used to manipulate lists. The ListLink structure
 * contains the information needed to maintain the linked
 * list. IT MUST BE THE FIRST FIELD OF ANY STRUCTURE THAT
 * IS TO MAINTAINED IN A LIST. The second structure is the
 * List structure, which holds the pointers to the elements
 * that make up the list. Note that, a la Knuth, the List
 * structure actually points to the last item on the list,
 * which is then circularly linked to the first item. This
 * approach minimizes the time needed to find either the
 * front (2 memory references) or back (1 memory reference)
 * of the list. 
 */

typedef struct OSPSListLink
{
    struct OSPSListLink *ospmLinkNext;
}
OSPTLISTLINK;

typedef OSPTLISTLINK *OSPTLIST;


/**/
/*-----------------------------------------------------------------------*/
/* macros that emulate functions                                         */
/*-----------------------------------------------------------------------*/
#ifndef OSPC_DEBUG
/* 
 * Note: all macros are also implemented as functions in osplist.c. For
 * implementation details, see the comments in that file. To replace a
 * macro with a true function, simply comment out the macro definition
 * below.
 */

#define OSPPListLinkNew(ospvLink)      ((ospvLink)->ospmLinkNext = OSPC_OSNULL)
#define OSPPListLinkDelete(ospvLink)   ((ospvLink)->ospmLinkNext = OSPC_OSNULL)
#define OSPPListLinkIsolated(ospvLink) ((ospvLink)->ospmLinkNext == OSPC_OSNULL)

#define OSPPListNew(ospvList)    (*(ospvList) = OSPC_OSNULL)
#define OSPPListDelete(ospvList) (*(ospvList) = OSPC_OSNULL)
#define OSPPListEmpty(ospvList)  (*ospvList == OSPC_OSNULL)
#define OSPPListLast(ospvList)   (*(ospvList))

#define OSPPListFirst(ospvList)  (OSPPListEmpty(ospvList) ? \
    (OSPTLISTLINK *) OSPC_OSNULL : ((OSPTLISTLINK *)(*(ospvList)))->ospmLinkNext)

#define OSPPListNext(ospvList,ospvItem) \
    ( (OSPPListLast(ospvList)==((void *)(ospvItem))) ? OSPC_OSNULL : \
        ((void *)((OSPTLISTLINK *)(ospvItem))->ospmLinkNext) )
#define OSPPListMove(ospvDst,ospvSrc) \
    { *(ospvDst) = *(ospvSrc); OSPPListNew(ospvSrc); }

#endif

/**/
/*-----------------------------------------------------------------------*/
/* true macros                                                           */
/*-----------------------------------------------------------------------*/
#define OSPPListItemInList(List,InItem,ItemFld,ItemSize,PtrType,Found) { \
    PtrType  *item   = OSPC_OSNULL; \
    int       result = 0; \
    for (item = (PtrType *)OSPPListFirst(List); item != OSPC_OSNULL; \
        item = (PtrType *)OSPPListNext(List, item)) \
    { \
        result = OSPM_MEMCMP(InItem->ItemFld, item->ItemFld, ItemSize); \
        if (result == 0) \
        { \
            Found = OSPC_TRUE; \
            break; \
        } \
    } \
}

/**/
/*-----------------------------------------------------------------------*/
/* function prototypes                                                   */
/*-----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" 
{
#endif

    void      OSPPListAppend (OSPTLIST *, void *);
    void     *OSPPListRemove (OSPTLIST *);
    void     *OSPPListRemoveSpecificItem (OSPTLIST *, void *);
    unsigned  OSPPListCount(OSPTLIST *);
    void     *OSPPListNextToLast(OSPTLIST *);

#ifdef OSPC_DEBUG

    void      OSPPListLinkNew (OSPTLISTLINK *);
    void      OSPPListLinkDelete (OSPTLISTLINK *);
    unsigned  OSPPListLinkIsolated (OSPTLISTLINK *);
    void      OSPPListNew (OSPTLIST *);
    void      OSPPListDelete (OSPTLIST *);
    unsigned  OSPPListEmpty (OSPTLIST *);
    void     *OSPPListLast (OSPTLIST *);
    void     *OSPPListFirst (OSPTLIST *);
    void     *OSPPListNext (OSPTLIST *, void *);
    void      OSPPListMove (OSPTLIST *, OSPTLIST *);

#endif /* OSPC_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* osplist_h */
