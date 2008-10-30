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
 * osptransids.h - structures and prototypes for functions that manage
 *                 TransNexus transaction IDs
 */

#ifndef _OSPTRANSIDS_H
#define _OSPTRANSIDS_H

#include "osp/osp.h"

/* MACROS
 * #define tnListNewest (tnSentinel.tidOlderPtr)
 * #define tnListOldest (tnSentinel.tidNewerPtr)
 * #define tnListEnd    (&tnSentinel)
*/

typedef struct _OSPTTRANSID {                           /* info about a transaction ID */
    OSPTUINT64 ospmTransactionId;                       /* the actual transaction ID value */
    unsigned long ospmTransactionIdExpires;             /* expiration time */
    struct _OSPTTRANSID *ospmTransactionIdLessPtr;      /* lower valued ID in tree */
    struct _OSPTTRANSID *ospmTransactionIdMorePtr;      /* greater valued ID in tree */
    struct _OSPTTRANSID **ospmTransactionIdParent;      /* parent pointer (for removes) */
    struct _OSPTTRANSID *ospmTransactionIdOlderPtr;     /* older in list */
    struct _OSPTTRANSID *ospmTransactionIdNewerPtr;     /* more recent in list */
} OSPTTRANSID;

#include "osp/ospprovider.h"

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

    OSPTBOOL OSPPTransIdAdd(OSPTTRANSID *, struct _OSPTPROVIDER *);
    OSPTBOOL OSPPTransIdCheckAndAdd(OSPTUINT64, unsigned long, struct _OSPTPROVIDER *);
    void OSPPTransIdDelete(OSPTTRANSID *);
    void OSPPTransIdInit(struct _OSPTPROVIDER *);
    OSPTTRANSID *OSPPTransIdNew(OSPTUINT64, unsigned long);
    void OSPPTransIdPurge(OSPTTRANSID *);
    void OSPPTransIdRemove(OSPTTRANSID *);
    unsigned long OSPPTransIdSecNow(void);
    void OSPPTransIdTimeAdd(OSPTTRANSID *, struct _OSPTPROVIDER *);
    void OSPPTransIDTreeDelete(struct _OSPTPROVIDER *);

#ifdef TN_TRANSDBG
    void tnPrintTree(OSPTTRANSID *pTrans);
    void tnTransById(struct _OSPTPROVIDER *);       /* print by transaction ID */
    void tnTransByTime(struct _OSPTPROVIDER *);     /* print by expiration time */
#endif

#ifdef __cplusplus
}
#endif

#endif /* _OSPTRANSIDS_H */
