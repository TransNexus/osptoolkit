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
 * ospdebug.h - Structures and prototypes for debugging and logging.
 */

#ifndef _OSPDEBUG_H
#define _OSPDEBUG_H

#include "osp/ospossys.h"

/*
 * The OSPC_DEBUG constant is the master constant for all debugging
 * functions and structures. If that constant is undefined, then
 * all debugging code is removed from the library.
 * 
 * It must be defined at compile time
 *
 * -DOSPC_DEBUG
 */

/*
 * The basic debugging tool is the OSPM_DBG macro. Although not
 * normally directly accessible by the main SDK code, it is the
 * foundation on which all other debugging tools are built. If
 * OSPC_DEBUG is not defined, then the macro has no effect. If it
 * is defined, though, the macro checks a condition and, if the
 * condition is true, executes a printf-like function. The
 * actual function called is OSPM_DBGPRINTF, which can be re-
 * defined to be a special debugging printf function if
 * appropriate for the environment. For now, it's just defined
 * to be printf.
 */

#define OSPM_DBGPRINTF OSPM_PRINTF
#define OSPM_PRINTTOERR(ospvParams); {fprintf ospvParams; fflush(stdout); }

/*
 * The OSPM_DBG macro is slightly tricky since printf takes
 * a variable number of arguments. Notice that there is no
 * parentheses around the params parameter when it's used
 * in the macro. Those parentheses must be supplied when
 * the macro is invoked in the code. The following example
 * shows how the macro might be used:
 *
 *      OSPM_DBG(cond,("debugging %u %u %u\n",1,2,3));
 *
 * Notice the extra parentheses around the parameters that
 * follow the first.
 */

#ifdef OSP_SDK  /* Use standard OSP SDK debug code */

#ifdef OSPC_DEBUG
#  ifdef _WIN32
#    define OSPM_DBG(ospvCond,ospvParams) \
        ( ospvCond ? OSPM_DBGPRINTF ospvParams : 0 )
#  else
#    define OSPM_DBG(ospvCond,ospvParams) \
        if ( ospvCond ) {  OSPM_DBGPRINTF ospvParams; fflush(stdout); }
#  endif
#else
#   define OSPM_DBG(ospvCond,ospvParams)
#endif

/*
 * The next set of macros build on the OSPM_DBG macro to make
 * more specialized debugging macros. First we define a number
 * of constants for various debugging conditions.
 */

#define OSPC_DBGERROR  0x00000001   /* error conditions */
#define OSPC_DBGENTER  0x00000002   /* trace entry into functions */
#define OSPC_DBGEXIT   0x00000004   /* trace exit from functions */
#define OSPC_DBGMEM    0x00000008   /* memory allocations and frees */
#define OSPC_DBGBFR    0x00000010   /* buffer stuff */
#define OSPC_DBGNET    0x00000020   /* network input/output */
#define OSPC_DBGSEC    0x00000040   /* security */
#define OSPC_DBGMISC   0x00000080   /* miscellaneous debugging */

#define OSPC_DBGALL    0xFFFFFFFF   /* debug everything */

/*
 * Next is a constant that determines which debug conditions
 * are in effect. By making this a constant, the debug conditions
 * are set at compile time. To make the debug conditions alterable
 * while the code is running, change the following constant to
 * a variable. The variable can then be changed while the code
 * is executing. Of course, if OSPC_DEBUG is not defined, then
 * no debug code is included.
 */

#define OSPVDbgFlag  OSPC_DBGERROR 

/*
 * At last, here are the real debug macros. They are intended
 * for debugging specific aspects of the library. For example,
 * OSPM_DBGMEM is used to show debugging information relating
 * to memory management. When memory allocation problems need
 * to be debugged, OSPVDbgFlag should be set so that the
 * OSPC_DBGMEM bit is set. For the same reasons given in the
 * description of OSPM_DBG, whenever these macros are used in
 * the code, two sets of parentheses must be used. For example:
 *
 *    OSPM_DBGMEM(("allocating %u bytes of memory\n", size));
 */

#define OSPM_DBGERROR(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGERROR),ospvParams)

#define OSPM_DBGENTER(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGENTER),ospvParams)

#define OSPM_DBGEXIT(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGEXIT),ospvParams)

#define OSPM_DBGMEM(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGMEM),ospvParams)

#define OSPM_DBGBFR(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGBFR),ospvParams)

#define OSPM_DBGNET(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGNET),ospvParams)

#define OSPM_DBGSEC(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGSEC),ospvParams)

#define OSPM_DBGMISC(ospvParams) \
    OSPM_DBG((OSPVDbgFlag&OSPC_DBGMISC),ospvParams)

/*
 * Finally, a special debugging macro just for error logging.
 * The macro takes two parameters: an error code and descriptive
 * text. It is called as:
 *
 * OSPM_DBGERRORLOG(OSPC_ERR_COMM_ALLOC_FAIL, "memory allocation failed");
 *
 * The resulting print line will include the code and text, as
 * well as the file name and line number from which the macro was
 * invoked (as in assert). For example:
 *
 *     ERROR 0001: memory allocation failed
 *           File: ospcomm.c  line 178
 *
 * As with all debug macros, if OSPC_DEBUG is not defined, the macro
 * leaves no footprint or artifacts within the production code.
 */

#define OSPM_DBGERRORLOG(ospvErrCode,ospvErrText) \
     OSPM_DBGERROR(("ERROR %0d: %s\n      File: %s  line: %u\n", \
                  (ospvErrCode), (ospvErrText), __FILE__, __LINE__))

#else       /* Use customized debug code */
#   include    "ospcustomdebug.h"
#endif  /* OSP_SDK */


#endif
