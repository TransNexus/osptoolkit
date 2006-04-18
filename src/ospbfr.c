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
 * ospbfr.c - TransNexus message buffer functions. Provided mainly as
 *            as a convenience, as they are expected to be replaced by
 *            native buffer management routines. As such, they are
 *            optimized for readability not performance. Suggested
 *            optimizations are noted, however.
 *
 *            MULTI-THREADING IMPLICATIONS
 *
 *            The functions themselves are completely thread-safe;
 *            however, they call the OSPPOSmalloc(), OSPM_FREE(),
 *            and OSPM_MEMCPY() functions. Multi-threaded applications
 *            using these message buffer functions must either ensure
 *            that the called routines are thread-safe.
 */

#include <stdlib.h>

#include "osp/osp.h"
#include "osp/ospbfr.h"

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrNew() - allocate a new message buffer
 *-----------------------------------------------------------------------*/

OSPTBFR *                           /* returns allocated buffer or NULL */
    OSPPBfrNew(
    unsigned ospvSize               /* size in bytes of buffer */
    )
{
    OSPTBFR *ospvBfr = OSPC_OSNULL;

    /* try to get enough memory to hold the buffer and its header */
    OSPM_MALLOC(ospvBfr, OSPTBFR,sizeof(OSPTBFR) + ospvSize +1);

    /* if malloc succeeded, set up the message header */
    if (ospvBfr != OSPC_OSNULL)
    {
        /*
         * Since many of these functions take, as input, pointers to
         * pointers to buffers, it's worthwhile to take some extra
         * precautions to detect invalid pointer indirections. We
         * do that by defining a guard pointer in the beginning of
         * the Bfr structure and setting that field's value to NULL.
         * That way, if we're ever given a pointer instead of a
         * pointer to a pointer, we'll end up with NULL when we undo
         * the indirection. That's easy to detect (e.g. with asserts)
         * and it should be straightforward to trap during testing.
         */

        ospvBfr->ospmBfrGuard = OSPC_OSNULL;
        ospvBfr->ospmBfrRead  = ((unsigned char *)ospvBfr) + sizeof(OSPTBFR);
        ospvBfr->ospmBfrWrite = ospvBfr->ospmBfrRead;
        ospvBfr->ospmBfrEnd   = ospvBfr->ospmBfrRead + ospvSize;
        *(ospvBfr->ospmBfrRead) = 0;
    }

    return(ospvBfr);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrClear() - reset buffer
 *-----------------------------------------------------------------------*/
void       
OSPPBfrClear(
    OSPTBFR  *ospvBfr
)
{
    if(ospvBfr != OSPC_OSNULL)
    {
        ospvBfr->ospmBfrGuard = OSPC_OSNULL;
        ospvBfr->ospmBfrRead  = ((unsigned char *)ospvBfr) + sizeof(OSPTBFR);
        ospvBfr->ospmBfrWrite = ospvBfr->ospmBfrRead;
        *(ospvBfr->ospmBfrRead) = 0;
        OSPM_MEMSET(ospvBfr->ospmBfrWrite, 0, ospvBfr->ospmBfrEnd - ospvBfr->ospmBfrRead);
    }

    return;
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrWriteBlock() - add data to a message buffer
 *-----------------------------------------------------------------------*/

unsigned                           /* returns number of bytes written */
OSPPBfrWriteBlock(
    OSPTBFR   **ospvBfrAddr,       /* address of pointer to buffer */
    const void *ospvData,          /* pointer to data to add */
    unsigned    ospvCnt            /* number of bytes to add */
)
{
    OSPTBFR  *oldBfr;              /* used for current message buffer */
    OSPTBFR  *newBfr;              /* new message buffer, if needed */
    unsigned  ospvWritten=0;         /* count of bytes written */

    if (ospvBfrAddr != OSPC_OSNULL)
    {
        if (*ospvBfrAddr != OSPC_OSNULL) /* guard will catch bad indirection */
        {
            oldBfr = *ospvBfrAddr;
            ospvWritten = 0;

            /* will the data fit in the current buffer? */
            if ( (oldBfr->ospmBfrWrite + ospvCnt) <= oldBfr->ospmBfrEnd )
            {
                /* yes, it fits */
                OSPM_MEMCPY(oldBfr->ospmBfrWrite, ospvData, ospvCnt);
                oldBfr->ospmBfrWrite += ospvCnt;
                ospvWritten = ospvCnt;
            }
            else
            {
                /*
                 * data won't fit in current buffer, get a new one
                 * possible optimizations include trying realloc first, and
                 * intelligent over-allocation
                 */

                newBfr = OSPPBfrNew(OSPPBfrSize(oldBfr) + ospvCnt);
                if (newBfr != OSPC_OSNULL)
                {
                    /* copy existing data */
                    OSPPBfrWriteBlock(&newBfr, OSPPBfrLinearPtr(oldBfr), OSPPBfrSize(oldBfr));
                    /*            assert(OSPPBfrSize(newBfr) == OSPPBfrSize(oldBfr)); */

                    /* add new data */
                    OSPPBfrWriteBlock(&newBfr, ospvData, ospvCnt);
                    /*            assert((unsigned)OSPPBfrSize(newBfr) == (OSPPBfrSize(oldBfr) + ospvCnt)); */

                    /* replace the old buffer with the new */
                    *ospvBfrAddr = newBfr;

                    /* and free the old buffer */
                    OSPPBfrDelete(&oldBfr);

                    ospvWritten = ospvCnt;
                }
            }
        }
    }
    return(ospvWritten);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrWriteByte() - add a single byte to a message buffer
 *-----------------------------------------------------------------------*/

unsigned                           /* returns number of bytes written */
OSPPBfrWriteByte(
    OSPTBFR     **ospvBfrAddr,     /* address of pointer to buffer */
    unsigned char ospvChar         /* pointer to data to add */
)
{
    OSPTBFR  *oldBfr;            /* used for current message buffer */
    OSPTBFR  *newBfr;            /* new message buffer, if needed */
    unsigned  ospvWritten=0;     /* count of bytes written */
    unsigned  addSize;           /* if new buffer needed, how much to grow */

    if (ospvBfrAddr != OSPC_OSNULL)
    {
        if (*ospvBfrAddr != OSPC_OSNULL)  /* guard will catch bad indirection */
        {
            oldBfr = *ospvBfrAddr;
            ospvWritten = 0;

            /* will the data fit in the current buffer? */
            if (oldBfr->ospmBfrWrite < oldBfr->ospmBfrEnd)
            {
                /* yes, it fits */
                *((oldBfr)->ospmBfrWrite)++ = ospvChar;
                ospvWritten = 1;
            }
            else
            {
                /*
                 * data won't fit in current buffer, get a new one
                 * possible optimizations include trying realloc first
                 */

                addSize = 1 + OSPPBfrSize(oldBfr)/10;    /* try adding 10% */
                newBfr = OSPPBfrNew(OSPPBfrSize(oldBfr) + addSize);
                if (newBfr != OSPC_OSNULL)
                {
                    /*
                     * we only check for errors here via asserts because
                     * errors technically can't happen
                     */

                    /* copy existing data */
                    OSPPBfrWriteBlock(&newBfr, OSPPBfrLinearPtr(oldBfr), OSPPBfrSize(oldBfr));
                    /*            assert(OSPPBfrSize(newBfr) == OSPPBfrSize(oldBfr)); */

                    /* add new data */
                    *((newBfr)->ospmBfrWrite)++ = ospvChar;
                    /*            assert(OSPPBfrSize(newBfr) == (OSPPBfrSize(oldBfr) + 1)); */

                    /* replace the old buffer with the new */
                    *ospvBfrAddr = newBfr;

                    /* and free the old buffer */
                    OSPPBfrDelete(&oldBfr);

                    ospvWritten = 1;
                }
            }
        }
    }
    return(ospvWritten);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrReadBlock() - read data from a message buffer
 *-----------------------------------------------------------------------*/

unsigned                            /* returns number of bytes read */
OSPPBfrReadBlock(
    OSPTBFR **ospvBfrAddr,          /* address of pointer to buffer */
    void     *ospvPtr,              /* place to put data */
    unsigned  ospvCnt               /* max bytes to read */
)
{
    OSPTBFR  *bfr;
    unsigned  ospvRead=0;

    if (ospvBfrAddr != OSPC_OSNULL)
    {
        if (*ospvBfrAddr != OSPC_OSNULL)  /* guard will catch bad indirection */
        {
            bfr = *ospvBfrAddr;
            ospvRead = OSPPBfrSize(bfr);             /* how many bytes to read */
            if (ospvRead > ospvCnt)
            {
                ospvRead = ospvCnt;
            }

            OSPM_MEMCPY(ospvPtr, OSPPBfrLinearPtr(bfr), ospvRead);
            bfr->ospmBfrRead += ospvRead;                    /* update header */
        }
    }
    return(ospvRead);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrDelete() - free an allocated buffer
 *-----------------------------------------------------------------------*/

void                                    /* no return value */
OSPPBfrDelete(
    OSPTBFR **ospvBfr                    /* buffer to free */
)
{
    if (*ospvBfr != OSPC_OSNULL)
    {
        OSPM_FREE(*ospvBfr);
        *ospvBfr = OSPC_OSNULL;
    }
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrLinearPtr() - returns a pointer to linear buffer contents
 *-----------------------------------------------------------------------*/
void *
OSPPBfrLinearPtr(
    OSPTBFR *ospvBfr
)
{
    void *ospvPtr = OSPC_OSNULL;

    if (ospvBfr != OSPC_OSNULL)
    {

        /*
         * This function returns a linear pointer to the contents of a buffer.
         * In this context "linear" means that the resulting data is continguous
         * and the pointer can be passed, for example, to memcpy. This is
         * a pretty straightfoward implementation here, because the buffer
         * structures use linear memory. If the SDK is to be ported to an
         * environment without linear memory buffers (e.g. scatter/gather
         * structures), however, this functionality might present a challenge.
         * The intent, in those cases, is not to provide a function that
         * must be ported to a difficult environment. Rather, this function
         * exists so that porting engineers can easily identify all the
         * locations in the SDK that assume linear memory. Those locations
         * can then be converted to the native buffer management.
         */

        ospvPtr = ospvBfr->ospmBfrRead;
    }

    return(ospvPtr);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrSize() - get the number of bytes currently in buffer
 *-----------------------------------------------------------------------*/
unsigned                             /* returns the size */
OSPPBfrSize(
    OSPTBFR *ospvBfr                 /* buffer in question */
)
{
    unsigned ospvSize = 0;

    if (ospvBfr != OSPC_OSNULL)
    {
        ospvSize = ospvBfr->ospmBfrWrite - ospvBfr->ospmBfrRead;
    }
    return(ospvSize);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrReadByte() - read the next byte in the buffer (or -1 if empty)
 *-----------------------------------------------------------------------*/
int                                   /* returns -1 if empty */
OSPPBfrReadByte(
    OSPTBFR *ospvBfr                  /* buffer in question */
)
{
    int ospvByte = -1;

    if (ospvBfr != OSPC_OSNULL)
    {
        if (OSPPBfrSize(ospvBfr) > 0)
        {
            ospvByte = (int) *(ospvBfr->ospmBfrRead)++;
        }
        else
        {
            ospvByte = -1;
        }
    }
    return(ospvByte);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrPeekByte() - look at the next byte in the buffer (-1 if empty)
 *-----------------------------------------------------------------------*/
int                                   /* returns -1 if empty */
OSPPBfrPeekByte(
    OSPTBFR *ospvBfr                  /* buffer in question */
)
{
    int ospvByte = -1;

    /*
     * Since these objects are limited to internal SDK use only, we use
     * assert to validate input parameters; that way the validation
     * checks can be optimized out of production systems.
     */
    if (ospvBfr != OSPC_OSNULL)
    {
        if (OSPPBfrSize(ospvBfr) > 0)
        {
            ospvByte = (int) *(ospvBfr->ospmBfrRead);
        }
        else
        {
            ospvByte = -1;
        }
    }
    return(ospvByte);
}

/**/
/*-----------------------------------------------------------------------*
 * OSPPBfrPeekByteN() - look at the Nth byte in the buffer
 *-----------------------------------------------------------------------*/
int                                   /* returns -1 if < N bytes */
OSPPBfrPeekByteN(
    OSPTBFR *ospvBfr,                 /* buffer in question */
    unsigned ospvCnt                  /* which byte to look at */
)
{
    int ospvByte = -1;

    if (ospvBfr != OSPC_OSNULL)
    {
        if (OSPPBfrSize(ospvBfr) > ospvCnt)
        {
            ospvByte = (int) *((ospvBfr->ospmBfrRead)+ospvCnt);
        }
        else
        {
            ospvByte = -1;
        }
    }
    return(ospvByte);
}
