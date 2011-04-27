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

/* osptransids.h - structures and prototypes for functions that manage TransNexus transaction IDs */
#ifndef _OSPTRANSIDS_H
#define _OSPTRANSIDS_H

#include "osp/osp.h"

/* MACROS
 * #define tnListNewest (tnSentinel.tidOlderPtr)
 * #define tnListOldest (tnSentinel.tidNewerPtr)
 * #define tnListEnd    (&tnSentinel)
 */
typedef struct _OSPTTRANSID {       /* info about a transaction ID */
    OSPTUINT64 Id;                  /* the actual transaction ID value */
    unsigned long Expires;          /* expiration time */
    struct _OSPTTRANSID *LessPtr;   /* lower valued ID in tree */
    struct _OSPTTRANSID *MorePtr;   /* greater valued ID in tree */
    struct _OSPTTRANSID **Parent;   /* parent pointer (for removes) */
    struct _OSPTTRANSID *OlderPtr;  /* older in list */
    struct _OSPTTRANSID *NewerPtr;  /* more recent in list */
} OSPT_TRANS_ID;

#include "osp/ospprovider.h"

/* Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    OSPTBOOL OSPPTransIdAdd(OSPT_TRANS_ID *, struct _OSPTPROVIDER *);
    OSPTBOOL OSPPTransIdCheckAndAdd(OSPTUINT64, unsigned long, struct _OSPTPROVIDER *);
    void OSPPTransIdDelete(OSPT_TRANS_ID *);
    void OSPPTransIdInit(struct _OSPTPROVIDER *);
    OSPT_TRANS_ID *OSPPTransIdNew(OSPTUINT64, unsigned long);
    void OSPPTransIdPurge(OSPT_TRANS_ID *);
    void OSPPTransIdRemove(OSPT_TRANS_ID *);
    unsigned long OSPPTransIdSecNow(void);
    void OSPPTransIdTimeAdd(OSPT_TRANS_ID *, struct _OSPTPROVIDER *);
    void OSPPTransIDTreeDelete(struct _OSPTPROVIDER *);

#ifdef TN_TRANSDBG
    void tnPrintTree(OSPT_TRANS_ID *pTrans);
    void tnTransById(OSPTPROVIDER *);   /* print by transaction ID */
    void tnTransByTime(OSPTPROVIDER *); /* print by expiration time */
#endif

#ifdef __cplusplus
}
#endif

#endif /* _OSPTRANSIDS_H */
