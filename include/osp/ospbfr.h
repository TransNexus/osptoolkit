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
 * ospbfr.h - Structures and prototypes for TransNexus message buffers.
 *            These should replaced by native buffer management routines,
 *            and, therefore, are optimized for readability rather than
 *            performance. Suggested optimizations are noted, though,
 *            in case native buffer management is not available.
 *
 *            See ospbfr.c for multi-threaded implications.
 */

#ifndef _OSPBFR_H
#define _OSPBFR_H

#include "ospossys.h"


typedef struct
{
    unsigned char *ospmBfrGuard;    /* to detect invalid pointers */
    unsigned char *ospmBfrEnd;      /* end of message buffer */
    unsigned char *ospmBfrRead;     /* current read position */
    unsigned char *ospmBfrWrite;    /* current write position */

    /* be sure structure ends on aligned byte boundary */
}
OSPTBFR;


#ifdef __cplusplus
extern "C" 
{
#endif

    /**/
    /*-----------------------------------------------------------------------*/
    /* function prototypes                                                   */
    /*-----------------------------------------------------------------------*/
    OSPTBFR  *OSPPBfrNew(unsigned);
    unsigned  OSPPBfrWriteByte(OSPTBFR **, unsigned char );
    unsigned  OSPPBfrWriteBlock(OSPTBFR **, const void *, unsigned );
    unsigned  OSPPBfrReadBlock(OSPTBFR **, void *, unsigned );
    void      OSPPBfrClear(OSPTBFR  *);

    void     *OSPPBfrLinearPtr(OSPTBFR *);
    unsigned  OSPPBfrSize(OSPTBFR *);
    int       OSPPBfrReadByte(OSPTBFR *);
    int       OSPPBfrPeekByte(OSPTBFR *);
    int       OSPPBfrPeekByteN(OSPTBFR *, unsigned);
    void      OSPPBfrDelete(OSPTBFR **);


#ifdef __cplusplus
}
#endif

#endif /* _OSPBFR_H */

