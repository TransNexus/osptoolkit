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
 *  ospaltinfo.h - OSP Alternate information element
 */

#ifndef _OSPALTINFO_H
#define _OSPALTINFO_H

#include "osp.h"
#include "ospxmlelem.h"
#include "ospmsgelem.h"
#include "ospmsg.h"


/* max size of an altinfo (bytes) */
#define OSPC_ALTINFOMAXSIZE 1024

typedef enum
{
    ospetypeupper,
    ospeE164,
    ospeH323,
    ospeUrl,
    ospeEmail,
    ospeTransport,
    ospeInternational,
    ospeNational,
    ospeNetwork,
    ospeSubscriber,
    ospeAbbreviated,
    ospeE164prefix,
    ospetypelower
}OSPE_TYPE_ATTR_VAL;

/* the basic altinfo structure */

typedef struct
{
    OSPTLISTLINK        ospmAltInfoLink;
    unsigned            ospmAltInfoLen;
    OSPE_TYPE_ATTR_VAL  ospmAltInfoType;
    unsigned char       *ospmAltInfoVal;
}
OSPTALTINFO;

typedef struct
{
    OSPE_TYPE_ATTR_VAL  ospmType;
    const char         *ospmTypeStr;
}OSP_TYPE_ATTR_STRUCT;


/**/
/*-----------------------------------------------------------------------*
 * macros that emulate functions
 *-----------------------------------------------------------------------*/

/*
 * Note: all macros are also implemented as functions in ospaltinfo.c.
 * For implementation details, see the comments in that file. To replace
 * a macro with a true function, simply comment out the macro definition
 * below.
 */
#ifndef OSPC_DEBUG
#define OSPPAltInfoDelete(ospvAltInfo)  OSPM_FREE(*ospvAltInfo); *ospvAltInfo = OSPC_OSNULL;

#define OSPPAltInfoGetSize(ospvAltInfo)  ospvAltInfo?((ospvAltInfo)->ospmAltInfoLen):0
#define OSPPAltInfoGetType(ospvAltInfo)  ospvAltInfo?((ospvAltInfo)->ospmAltInfoType):ospetypeupper

#define OSPPAltInfoGetValue(ospvAltInfo) ospvAltInfo?((ospvAltInfo)->ospmAltInfoVal):OSPC_OSNULL

#endif 


#ifdef __cplusplus
extern "C"
{
#endif
    /**/
    /*-----------------------------------------------------------------------*
     * function prototypes
     *-----------------------------------------------------------------------*/

    OSPTALTINFO *OSPPAltInfoNew(unsigned, const unsigned char *, 
                                              OSPE_TYPE_ATTR_VAL);

#ifdef OSPC_DEBUG
        
    void                OSPPAltInfoDelete(OSPTALTINFO **);
    unsigned            OSPPAltInfoGetSize(OSPTALTINFO *);
    OSPE_TYPE_ATTR_VAL  OSPPAltInfoGetType(OSPTALTINFO *);
    const unsigned char *OSPPAltInfoGetValue(OSPTALTINFO *);

#endif

    unsigned            OSPPAltInfoToElement(OSPTALTINFO *, 
                                        OSPTXMLELEM **, OSPTMSGELEMPART);
    const char *OSPPAltInfoGetTypeName(OSPTALTINFO *);


#ifdef __cplusplus
}
#endif

#endif
