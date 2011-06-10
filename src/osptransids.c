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
 *
 *
 *           OVERALL DESIGN STRATEGY
 *
 *           Transaction ID's are stored in two parallel
 *           structures -- a binary tree and a doubly-linked
 *           list. The binary tree organizes the IDs by
 *           their value. Its purpose is to quickly search
 *           for a particular ID value. A binary tree is
 *           efficient for two reasons:
 *
 *             1) Nearly all attempts to search for an
 *                ID should fail. If a search does succeed,
 *                it means that a duplicate ID had been
 *                detected, and that *should* never happen.
 *                Since most searches are expected to end
 *                in "failure", simpler data structures
 *                that force an exhaustive search would
 *                be particularly costly.
 *
 *             2) By nature of their Hamming coding, ID
 *                values are essentially random. Trees
 *                built from these values should therefore
 *                be relatively balanced. There is no
 *                need, consequently, to explicitly balance
 *                the data a la AVL trees.
 *
 *           The second parallel data structure -- a doubly-
 *           linked list -- sorts the transaction IDs by
 *           their expiration time. A doubly-linked list
 *           is efficient for this operation because:
 *
 *             1) Transaction IDs should be added to the
 *                list in order (more or less). In other
 *                words, each additional ID is likely to
 *                be newer than all previously added IDs.
 *                Keeping the list sorted, therefore, should
 *                be relatively inexpensive. (Note, though,
 *                that the order in which IDs are added is
 *                only approximately sorted, so some manual
 *                sorting in these routines is still essential.)
 *
 *             2) Only the oldest Transaction is removed
 *                from the list. This is true in the strictest
 *                sense, making a linked list (provided it
 *                maintains pointers to its head and tail)
 *                perfect for the removal operation.
 *
 *             3) Because IDs are added in nearly sorted order,
 *                binary tree-based structures are likely to be
 *                very unbalanced, and explicitly balanced
 *                trees (e.g. AVL trees) are likely to require
 *                relatively significant CPU cycles to balance.
 *
 *             4) Double links are needed (instead of a single
 *                link) because of the complications of
 *                removing a node from a binary tree. (Since
 *                the tree and list data structures are
 *                parallel, every node simultaneously exists
 *                on both structures.) See the binary tree
 *                deletion routine for details.
 *
 *           IMPLEMENTATION ISSUES
 *
 *           The binary tree stuff is pretty straightforward
 *           (check out any decent Algorithms text), but the
 *           linked list might seem a bit unusual to some.
 *           (It's still pretty standard, though.) The
 *           list uses a dummy node for a sentinel instead
 *           of explicit head (and/or tail) pointers. Using
 *           a sentinel makes insertions and deletions a
 *           lot cleaner, since we don't have to check for
 *           those messy boundary conditions where pointers
 *           are equal to null. It does mean, though, that
 *           the list must be explicitly initialized before
 *           it can be used.
 *
 *           The last (e.g. newest) node in the list is set
 *           so that its newer pointer points to the sentinel,
 *           and the sentinel's own newer pointer points to
 *           the first (oldest) node in the list. Similarly,
 *           the first node's older pointer points to the
 *           sentinel, and the sentinel's older pointer points
 *           to the last node in the list. This makes it easy
 *           to find both the head or the tail of the list
 *           through the sentinel. The following picture shows
 *           the organization:
 *
 *
 *                             SENTINEL <-------------+
 *                            [=======]               |
 *                            [ xx:xx ]               |
 *            +-------------> [ older ] -------+      |
 *            |      +------- [ newer ]        |      |
 *            |      |        [=======]        |      |
 *            |      |                         |      |
 *            |      v                         V      |
 *            |  [=======]    [=======]    [=======]  |
 *            |  [ 01:00 ]    [ 02:00 ]    [ 03:00 ]  |
 *            +- [ older ] <- [ older ] <- [ older ]  |
 *               [ newer ] -> [ newer ] -> [ newer ] -+
 *               [=======]    [=======]    [=======]
 *
 *
 *           DEBUGGING ISSUES
 *
 *           Moderately liberal use of assertions throughout
 *           these routines. In the author's experience,
 *           asserts are extremely valuable in debugging
 *           complex data structures like trees and lists
 *           because they allow you to detect a corrupt
 *           structure just about as soon as possible. If
 *           you suspect problems with the routines, be
 *           sure to compile with appropriate DEBUG flags
 *           to enable the assert statements. (Check your
 *           compiler for details, ANSI standard is to
 *           undefine NDEBUG.)
 *
 *           MULTI-THREADING IMPLICATIONS
 *
 *           The functions, as currently written, are not
 *           thread-safe at all. Making them thread-safe,
 *           however, is a relatively simple task, and the
 *           code includes comments that indicate where
 *           multi-threading support (mainly semaphore
 *           locking and unlocking) should go. The current
 *           code omits the actual semaphore function calls
 *           so as to maintain maximum clarity and portability.
 *           Actual implementations must replace the comments
 *           below with actual calls to the environment's
 *           appropriate semaphore mechanisms.
 */

#include "osp/osptransids.h"
#include "osp/ospprovider.h"

/* OSPPTransIdAdd - add a transaction ID to the ordered tree */
OSPTBOOL OSPPTransIdAdd(        /* returns false if duplicate */
    OSPT_TRANS_ID *ospvTransId, /* transaction ID to add */
    OSPTPROVIDER *ospvProvider)
{
    OSPT_TRANS_ID **root = OSPC_OSNULL;

    /* check to make sure the transaction ID structure is clean */
    if (!(ospvTransId->LessPtr == 0) || !(ospvTransId->MorePtr == 0)) {
        return OSPC_FALSE;
    }

    /*
     * Pretty much a standard, non-recursive binary tree algorithm.
     * We start looking at the root of the tree and keep following
     * down until we reach the place to add the new node. The
     * variable ppRoot keeps track of where we are in the tree. When
     * it's value is zero, we're there.
     */

    root = OSPPProviderGetTransIdRoot(ospvProvider);

    while (*root != 0) {

        if (ospvTransId->Id < (*root)->Id) {
            /*
             * We're not at the end of the tree yet, so we have to
             * keep searching. In this case, the node to be added
             * is numerically less than the current root, so we
             * need to continue looking down the "Less" side of
             * of the tree.
             */

            root = &((*root)->LessPtr);
        } else if (ospvTransId->Id > (*root)->Id) {
            /*
             * This is the same as the previous case, except that
             * the node being added is numerically greater than the
             * current root. In this case, we keep looking on the
             * "More" side of the tree.
             */

            root = &((*root)->MorePtr);
        } else {
            /*
             * This isn't *supposed* to happen, but we've found a duplicate
             * node already in the tree. Our response is to return false.
             */

            return OSPC_FALSE;
        }
    }

    /*
     * We've reached the end of the tree in our current search,
     * so this is where we want to add the new node. A simple
     * assignment will do.
     *
     * If this is the first item to be added, then ppRoot will
     * point to the absolute root of the tree and we'll be
     * updating that root. Otherwise, root will point to
     * a child pointer (either Less or More) in an existing
     * node, and we'll update that node's pointer.
     */

    *root = ospvTransId;
    ospvTransId->Parent = root;

    /* since everything's okay, return true */
    return OSPC_TRUE;

}

/* OSPPTransIdCheckAndAdd - add a transaction ID, as long as it's unique */

OSPTBOOL OSPPTransIdCheckAndAdd(    /* returns false if addition fails */
    OSPTUINT64 ospvTransId,         /* transaction ID value */
    unsigned long ospvExpires,      /* expiration time in seconds */
    OSPTPROVIDER *ospvProvider)
{
    OSPT_TRANS_ID *ptransid = OSPC_OSNULL;  /* pointer to Transaction ID */
    int errorcode = OSPC_ERR_NO_ERROR;

    /* LOCK GLOBAL DATA NOW */
    errorcode = OSPPProviderLockTransIdMutex(ospvProvider);

    /*
     * As a first step, we purge any transaction IDs that have
     * expired. That may free up memory which will make it easier
     * to add the new ID.
     *
     * Note that we never actually check to make sure the current
     * transaction hasn't itself expired. We don't think that's
     * critical (nothing wrong with an old ID hanging around for
     * a little extra time), and it should never happen (presumably,
     * the caller checks the expiration time before accepting the
     * transaction in the first place), but it does make it possible
     * to purge some old transactions and then add an even older
     * one. A bit funny looking if you test for it, but no harm
     * done.
     */

    if (errorcode == OSPC_ERR_NO_ERROR) {
        OSPPTransIdPurge(OSPPProviderGetTransIdSentinel(ospvProvider));
    }

    /*
     * Now get memory to store the current transaction ID. This
     * is a little optimistic, since we might not be able to
     * add the new ID (in which case we'd have to free the
     * memory later). That's a pretty rare occurrence, though,
     * so we can afford the slight inefficiency.
     */

    if (errorcode == OSPC_ERR_NO_ERROR) {
        ptransid = OSPPTransIdNew(ospvTransId, ospvExpires);

        if (ptransid != OSPC_OSNULL) {

            /* add it to the binary tree of ordered IDs */
            if (OSPPTransIdAdd(ptransid, ospvProvider)) {

                /*
                 * If we were able to add the ID okay, also add it to
                 * the sorted list by expiration time.
                 */

                OSPPTransIdTimeAdd(ptransid, ospvProvider);
            } else {
                /*
                 * Couldn't add the ID (there must be a duplicate), so
                 * we need to free the memory we allocated.
                 */
                OSPPTransIdDelete(ptransid);

                /* RELEASE LOCK ON GLOBAL DATA NOW */
                errorcode = OSPPProviderUnLockTransIdMutex(ospvProvider);
                return OSPC_FALSE;
            }
        }
    }

    /* RELEASE LOCK ON GLOBAL DATA NOW */
    errorcode = OSPPProviderUnLockTransIdMutex(ospvProvider);

    return ((OSPTBOOL) ((errorcode == OSPC_ERR_NO_ERROR) ? OSPC_TRUE : OSPC_FALSE));
}

/* OSPPTransIdDelete - free a transaction ID structure */
void  OSPPTransIdDelete(        /* no return value */
    OSPT_TRANS_ID *ospvTransId) /* structure to free */
{
    OSPM_FREE(ospvTransId);
}

/*OSPPTransIDInit - initialize tree and linked list*/
void OSPPTransIdInit(
    OSPTPROVIDER *ospvProvider)
{
    int errcode = OSPC_ERR_NO_ERROR;

    /*initialize transaction tracking
     * initialize the doubly-linked list sentinel */
    OSPT_TRANS_ID *sentinel = OSPPProviderGetTransIdSentinel(ospvProvider);
    OSPT_TRANS_ID **treeroot = OSPPProviderGetTransIdRoot(ospvProvider);

    OSPM_MEMSET(sentinel, 0, sizeof(OSPT_TRANS_ID));
    sentinel->OlderPtr = sentinel;
    sentinel->NewerPtr = sentinel;

    OSPM_MUTEX_INIT(ospvProvider->TransIdMutex, 0, errcode);

#ifdef TN_TRANSDBG
    if (errcode != OSPC_ERR_NO_ERROR) {
        fprintf(stderr, "\nMutex Init failed Err=  %d.", errcode);
    }
#endif
    /* initialize binary tree */
    *treeroot = 0;
}

/* OSPPTransIdNew - allocate and initialize a new transaction ID structure */
OSPT_TRANS_ID *  OSPPTransIdNew(/* returns null on failure */
    OSPTUINT64 ospvTransIdVal,  /* transaction ID value */
    unsigned long ospvExpires)  /* expiration time in seconds */
{
    OSPT_TRANS_ID *transid = OSPC_OSNULL;   /* pointer to Transaction ID */

    OSPM_MALLOC(transid, OSPT_TRANS_ID, sizeof(OSPT_TRANS_ID));
    if (transid) {
        OSPM_MEMSET(transid, 0, sizeof(OSPT_TRANS_ID));
        transid->Id = ospvTransIdVal;
        transid->Expires = ospvExpires;
    }

    return transid;
}

/* OSPPTransIdPurge - remove expired transaction IDs */
void OSPPTransIdPurge(      /* returns nothing */
    OSPT_TRANS_ID *ospvSentinel)
{
    OSPT_TRANS_ID *ptransid = OSPC_OSNULL;
    unsigned long now = 0;

    /* Get the current time. Anything older will be deleted. */
    now = OSPPTransIdSecNow();

    for (ptransid = ospvSentinel->OlderPtr;
         (ptransid != ospvSentinel) && (ptransid->Expires < now); ptransid = ospvSentinel->NewerPtr) {
        /* this one's expired, remove it from the list */
        ptransid->OlderPtr->NewerPtr = ptransid->NewerPtr;
        ptransid->NewerPtr->OlderPtr = ptransid->OlderPtr;

        /* and then delete it completely */
        OSPPTransIdRemove(ptransid);

    }
}

/* OSPPTransIdRemove - remove a transaction ID from the ordered tree */
void OSPPTransIdRemove(         /* no return value */
    OSPT_TRANS_ID *ospvTransId) /* transaction ID to remove */
{
    OSPT_TRANS_ID *splice = OSPC_OSNULL;/* node to be spliced */
    OSPT_TRANS_ID *child = OSPC_OSNULL; /* child of spliced node */
    int errorcode = OSPC_ERR_NO_ERROR;

    /*
     * Yep, the worst part of any binary tree stuff -- removing
     * a single node from the tree. (Why is this routine always
     * left as an exercise to the reader in algorithm and data
     * structures text books?) The approach is pretty standard,
     * though. Three cases:
     *
     * 1) Node has no children! Piece of cake, just update its
     *    parent to point to null.
     *
     * 2) Node has only a single child. Not too much harder, just
     *    "splice" the current node out of the tree. (I.e. update
     *     its parent to point to the node's only child.)
     *
     * 3) Node has two children. This one's the hard one. We "splice"
     *    out the node's successor and replace the current node
     *    with its successor. See details below.
     */

    /* figure out which node to splice out */

    if ((ospvTransId->LessPtr == 0) || (ospvTransId->MorePtr == 0)) {
        /*
         * If the node has no more than one child, then it is
         * itself the one to remove from the tree. This actually
         * takes care of cases 1 and 2 above, since we'll be
         * sure to "splice" correctly even the node has no
         * children.
         */

        splice = ospvTransId;
    } else {
        /*
         * Okay, the node we want to delete has two children. That
         * means we can't simply splice it out, because we'd have
         * to leave one of those poor children orphaned. What we
         * want to do is find the "successor" of the node to
         * delete. The successor is the next greatest node after
         * the current one (the smallest transaction ID greater
         * that the current node's.) In a tree structure, the
         * successor is the left-most node (the "tidLessPtr" path)
         * in the right (the "tidMorePtr" path) subtree.
         */

        splice = ospvTransId->MorePtr;
        while (splice->LessPtr) {
            splice = splice->LessPtr;
        }
    }

    /*
     * Now we've found the node to splice out. Next figure out which
     * of that node's children is going to get "promoted".
     */

    if (splice->LessPtr) {
        child = splice->LessPtr;
    } else {
        /*
         * This handles case 1 in the introduction. If the node
         * had no children, the pChild will end up being null.
         */

        child = splice->MorePtr;
    }

    /*
     * Okay, we've identified the child to promote, change it's
     * parent to the spliced node's parent.
     */

    if (child) {
        if (!(*(child->Parent) == splice->LessPtr) &&
            !(*(child->Parent) == splice->MorePtr)) {
            errorcode = OSPC_ERR_TRANSID;
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            child->Parent = splice->Parent;
        }
    }

    /*
     * Last part of the splice: update the parent to point to the
     * new child.
     */

    if (errorcode == OSPC_ERR_NO_ERROR) {
        *(splice->Parent) = child;

        /*
         * Final cleanup, if the node we spliced out wasn't the actual
         * node to delete, then we need to replace the deleted node's
         * contents with the spliced node's.
         */

        if (splice != ospvTransId) {
            /* good place for some checks */
            if (!(splice->OlderPtr->NewerPtr == splice) ||
                !(splice->NewerPtr->OlderPtr == splice)) {
                errorcode = OSPC_ERR_TRANSID;
            }

            if (errorcode == OSPC_ERR_NO_ERROR) {
                /* first extract the spliced node from the linked list */
                splice->OlderPtr->NewerPtr = ospvTransId;
                splice->NewerPtr->OlderPtr = ospvTransId;

                /* now insert the existing node into the linked list */
                ospvTransId->OlderPtr = splice->OlderPtr;
                ospvTransId->NewerPtr = splice->NewerPtr;

                /* finally, update the existing node's actual data */
                ospvTransId->Id = splice->Id;
                ospvTransId->Expires = splice->Expires;
            }
        }
    }

    if (splice != OSPC_OSNULL) {
        /* release the memory */
        OSPPTransIdDelete(splice);
    }
}

/* OSPPTransIdSecNow - current time in seconds since midnight 1Jan1970 */
unsigned long OSPPTransIdSecNow(void)   /* returns current time in seconds */
{
    time_t now;

    time(&now);

    return now;
}

/* OSPPTransIdTimeAdd - add transaction in time-ordered linked list */
void  OSPPTransIdTimeAdd(       /* no return value */
    OSPT_TRANS_ID *ospvTransId, /* transaction ID to add */
    OSPTPROVIDER *ospvProvider)
{
    OSPT_TRANS_ID *curr = OSPC_OSNULL;    /* current position on list */
    OSPT_TRANS_ID *sentinel = OSPPProviderGetTransIdSentinel(ospvProvider);
    int errorcode = OSPC_ERR_NO_ERROR;
    /*
     * We put this transaction on the list in sorted order. To
     * do that, we start with the newest transaction and move
     * to older ones until we find where the current transaction
     * needs to go.
     */

    if (sentinel != OSPC_OSNULL) {
        for (curr = sentinel->OlderPtr;
             ((curr != sentinel) && (curr->Expires > ospvTransId->Expires));) {
            /*
             * The loop iteration could be moved into the for statement,
             * but we wanted to go ahead and use the for loop to check
             * pointer consistency using the assert statements below.
             * If the asserts were the only thing inside the braces, then
             * some compilers might complain when a non-assert (e.g.
             * production version) is built. To be on the safe side,
             * then, we've moved the iteration statement inside the
             * body of the for statement.
             */
            curr = curr->OlderPtr;

            /* now do the checks we were just talking about */
            if (!(curr->OlderPtr->NewerPtr == curr) && !(curr->OlderPtr->OlderPtr == curr)) {
                errorcode = OSPC_ERR_TRANSID;
                break;
            }
        }

        if (errorcode == OSPC_ERR_NO_ERROR) {
            /* Now just plop the new transaction in the list */
            ospvTransId->OlderPtr = curr;
            ospvTransId->NewerPtr = curr->NewerPtr;
            curr->NewerPtr->OlderPtr = ospvTransId;
            curr->NewerPtr = ospvTransId;
        }
    }
}

/*
 * OSPPTransIDTreeDelete - delete the whole tree
 */
void OSPPTransIDTreeDelete(
    OSPTPROVIDER *ospvProvider)
{
    OSPT_TRANS_ID *ptransid = OSPC_OSNULL;
    OSPT_TRANS_ID *sentinel = OSPPProviderGetTransIdSentinel(ospvProvider);

    /* LOCK GLOBAL DATA NOW */
    OSPPProviderLockTransIdMutex(ospvProvider);

    if (sentinel != OSPC_OSNULL) {
        for (ptransid = sentinel->OlderPtr; (ptransid != sentinel); ptransid = sentinel->OlderPtr) {
            /* remove it from the list */
            ptransid->OlderPtr->NewerPtr = ptransid->NewerPtr;
            ptransid->NewerPtr->OlderPtr = ptransid->OlderPtr;

            /* and then delete it completely */
            OSPPTransIdRemove(ptransid);
        }
    }
    /* RELEASE LOCK ON GLOBAL DATA NOW */
    OSPPProviderUnLockTransIdMutex(ospvProvider);
}

/* tnTransById - list transactions in order of ID */

#ifdef TN_TRANSDBG
void tnTransById(   /* no return value */
    OSPTPROVIDER *ospvProvider)
{
    /* LOCK MUTEX ON GLOBAL DATA NOW */
    OSPPProviderLockTransIdMutex(ospvProvider);

    /*
     * Since this function is really only for debug purposes, we
     * can be inefficient and use a recursive algorithm.
     */
    tnPrintTree(*(OSPPProviderGetTransIdRoot(ospvProvider)));

    /* RELEASE MUTEX ON GLOBAL DATA NOW */
    OSPPProviderUnLockTransIdMutex(ospvProvider);
}
#endif

/* tnPrintTree - print the contents of a tree of transactions */

#ifdef TN_TRANSDBG
void tnPrintTree(
    OSPTTRANSID *pTrans)
{
    char buf[30];

    if (pTrans) {
        /* use the opportunity to check consistency */
        assert((pTrans->ospmTransactionIdLessPtr == 0) ||
               (pTrans->ospmTransactionIdLessPtr->ospmTransactionIdParent == &(pTrans->ospmTransactionIdLessPtr)));
        assert((pTrans->ospmTransactionIdMorePtr == 0) ||
               (pTrans->ospmTransactionIdMorePtr->ospmTransactionIdParent == &(pTrans->ospmTransactionIdMorePtr)));

        tnPrintTree(pTrans->ospmTransactionIdLessPtr);
#ifdef _DEBUG
        OSPM_MEMSET(&buf, 0, sizeof(buf));
        sprintf(buf, "%016I64x", pTrans->ospmTransactionId);
        fprintf(stderr, "\nTID:%s\n", buf);
#endif
        tnPrintTree(pTrans->ospmTransactionIdMorePtr);
    }
}
#endif


/* tnTransByTime - list transactions in order of expiration time */
#ifdef TN_TRANSDBG
void tnTransByTime( /* no return value */
    OSPTPROVIDER *ospvProvider)
{
    OSPTTRANSID *pTrans;
    char buf[30];

    /* LOCK MUTEX ON GLOBAL DATA NOW */
    OSPPProviderLockTransIdMutex(ospvProvider);

    for (pTrans = (OSPPProviderGetTransIdSentinel(ospvProvider))->ospmTransactionIdNewerPtr;
         pTrans != OSPPProviderGetTransIdSentinel(ospvProvider); pTrans = pTrans->ospmTransactionIdNewerPtr) {
        assert(pTrans->ospmTransactionIdNewerPtr->ospmTransactionIdOlderPtr == pTrans);
        assert(pTrans->ospmTransactionIdOlderPtr->ospmTransactionIdNewerPtr == pTrans);
#ifdef _DEBUG
        OSPM_MEMSET(&buf, 0, sizeof(buf));
        sprintf(buf, "%016I64x", pTrans->ospmTransactionId);
        fprintf(stderr, "\nAt %s transaction %s expires.", ctime(&(pTrans->ospmTransactionIdExpires)), buf);
        /*afxDump << "\nAt " <<pTrans->tidExpires << ", transaction " << buf << "expires."; */
#endif
    }

    /* RELEASE SEMAPHORE ON GLOBAL DATA NOW */
    OSPPProviderUnLockTransIdMutex(ospvProvider);

}
#endif
